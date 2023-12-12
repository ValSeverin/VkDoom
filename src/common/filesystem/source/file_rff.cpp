/*
** file_rff.cpp
**
**---------------------------------------------------------------------------
** Copyright 1998-2009 Randy Heit
** Copyright 2005-2009 Christoph Oelckers
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
**
*/

#include "resourcefile_internal.h"
#include "fs_swap.h"

namespace FileSys {
	using namespace byteswap;

//==========================================================================
//
//
//
//==========================================================================

struct RFFInfo
{
	// Should be "RFF\x18"
	uint32_t		Magic;
	uint32_t		Version;
	uint32_t		DirOfs;
	uint32_t		NumLumps;
};

struct RFFLump
{
	uint32_t		DontKnow1[4];
	uint32_t		FilePos;
	uint32_t		Size;
	uint32_t		DontKnow2;
	uint32_t		Time;
	uint8_t		Flags;
	char		Extension[3];
	char		Name[8];
	uint32_t		IndexNum;	// Used by .sfx, possibly others
};

//==========================================================================
//
// BloodCrypt
//
//==========================================================================

void BloodCrypt (void *data, int key, int len)
{
	int p = (uint8_t)key, i;

	for (i = 0; i < len; ++i)
	{
		((uint8_t *)data)[i] ^= (unsigned char)(p+(i>>1));
	}
}


//==========================================================================
//
// Blood RFF file
//
//==========================================================================

class FRFFFile : public FResourceFile
{

public:
	FRFFFile(const char * filename, FileReader &file, StringPool* sp);
	virtual bool Open(LumpFilterInfo* filter);
};


//==========================================================================
//
// Initializes a Blood RFF file
//
//==========================================================================

FRFFFile::FRFFFile(const char *filename, FileReader &file, StringPool* sp)
: FResourceFile(filename, file, sp)
{
}

//==========================================================================
//
// Initializes a Blood RFF file
//
//==========================================================================

bool FRFFFile::Open(LumpFilterInfo*)
{
	RFFLump *lumps;
	RFFInfo header;

	Reader.Read(&header, sizeof(header));

	AllocateEntries(LittleLong(header.NumLumps));
	NumLumps = LittleLong(header.NumLumps);
	header.DirOfs = LittleLong(header.DirOfs);
	lumps = new RFFLump[header.NumLumps];
	Reader.Seek (header.DirOfs, FileReader::SeekSet);
	Reader.Read (lumps, header.NumLumps * sizeof(RFFLump));
	BloodCrypt (lumps, header.DirOfs, header.NumLumps * sizeof(RFFLump));

	for (uint32_t i = 0; i < NumLumps; ++i)
	{
		Entries[i].Position = LittleLong(lumps[i].FilePos);
		Entries[i].Length = LittleLong(lumps[i].Size);
		Entries[i].Flags = 0;
		Entries[i].Method = METHOD_STORED;
		if (lumps[i].Flags & 0x10)
		{
			Entries[i].Flags = RESFF_COMPRESSED;	// flags the lump as not directly usable
			Entries[i].Method = METHOD_INVALID;
		}
		else
		{
			Entries[i].Flags = 0;
			Entries[i].Method = METHOD_STORED;
		}
		Entries[i].Namespace = ns_global;
		Entries[i].ResourceID = LittleLong(lumps[i].IndexNum);
	
		// Rearrange the name and extension to construct the fullname.
		char name[13];
		strncpy(name, lumps[i].Name, 8);
		name[8] = 0;
		size_t len = strlen(name);
		assert(len + 4 <= 12);
		name[len+0] = '.';
		name[len+1] = lumps[i].Extension[0];
		name[len+2] = lumps[i].Extension[1];
		name[len+3] = lumps[i].Extension[2];
		name[len+4] = 0;
		Entries[i].FileName = NormalizeFileName(name);
	}
	delete[] lumps;
	GenerateHash();
	return true;
}

//==========================================================================
//
// Fills the lump cache and performs decryption
//
//==========================================================================
#if 0
int FRFFLump::FillCache()
{
	int res = FUncompressedLump::FillCache();

	if (Flags & LUMPF_COMPRESSED)
	{
		int cryptlen = std::min<int> (LumpSize, 256);
		uint8_t *data = (uint8_t *)Cache;

		for (int i = 0; i < cryptlen; ++i)
		{
			data[i] ^= i >> 1;
		}
	}
	return res;
}
#endif


//==========================================================================
//
// File open
//
//==========================================================================

FResourceFile *CheckRFF(const char *filename, FileReader &file, LumpFilterInfo* filter, FileSystemMessageFunc Printf, StringPool* sp)
{
	char head[4];

	if (file.GetLength() >= 16)
	{
		file.Seek(0, FileReader::SeekSet);
		file.Read(&head, 4);
		file.Seek(0, FileReader::SeekSet);
		if (!memcmp(head, "RFF\x1a", 4))
		{
			auto rf = new FRFFFile(filename, file, sp);
			if (rf->Open(filter)) return rf;
			file = rf->Destroy();
		}
	}
	return NULL;
}


}
