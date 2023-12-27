
#include "launcherwindow.h"
#include "v_video.h"
#include "version.h"
#include "i_interface.h"
#include <zwidget/core/resourcedata.h>
#include <zwidget/core/image.h>
#include <zwidget/window/window.h>
#include <zwidget/widgets/textedit/textedit.h>
#include <zwidget/widgets/listview/listview.h>
#include <zwidget/widgets/imagebox/imagebox.h>
#include <zwidget/widgets/textlabel/textlabel.h>
#include <zwidget/widgets/pushbutton/pushbutton.h>
#include <zwidget/widgets/checkboxlabel/checkboxlabel.h>

EXTERN_CVAR(Bool, queryiwad);

int LauncherWindow::ExecModal(WadStuff* wads, int numwads, int defaultiwad, int* autoloadflags)
{
	Size screenSize = GetScreenSize();
	double windowWidth = 615.0;
	double windowHeight = 668.0;

	auto launcher = new LauncherWindow(wads, numwads, defaultiwad, autoloadflags);
	launcher->SetFrameGeometry((screenSize.width - windowWidth) * 0.5, (screenSize.height - windowHeight) * 0.5, windowWidth, windowHeight);
	launcher->Show();

	DisplayWindow::RunLoop();

	return 0;
}

LauncherWindow::LauncherWindow(WadStuff* wads, int numwads, int defaultiwad, int* autoloadflags) : Widget(nullptr, WidgetType::Window)
{
	SetWindowBackground(Colorf::fromRgba8(51, 51, 51));
	SetWindowBorderColor(Colorf::fromRgba8(51, 51, 51));
	SetWindowCaptionColor(Colorf::fromRgba8(33, 33, 33));
	SetWindowCaptionTextColor(Colorf::fromRgba8(226, 223, 219));
	SetWindowTitle(GAMENAME);

	Logo = new ImageBox(this);
	WelcomeLabel = new TextLabel(this);
	VersionLabel = new TextLabel(this);
	SelectLabel = new TextLabel(this);
	GeneralLabel = new TextLabel(this);
	ExtrasLabel = new TextLabel(this);
	FullscreenCheckbox = new CheckboxLabel(this);
	DisableAutoloadCheckbox = new CheckboxLabel(this);
	DontAskAgainCheckbox = new CheckboxLabel(this);
	LightsCheckbox = new CheckboxLabel(this);
	BrightmapsCheckbox = new CheckboxLabel(this);
	WidescreenCheckbox = new CheckboxLabel(this);
	PlayButton = new PushButton(this);
	ExitButton = new PushButton(this);
	GamesList = new ListView(this);

	SelectLabel->SetText("Select which game file (IWAD) to run.");
	PlayButton->SetText("Play Game");
	ExitButton->SetText("Exit");

	GeneralLabel->SetText("General");
	ExtrasLabel->SetText("Extra Graphics");
	FullscreenCheckbox->SetText("Fullscreen");
	DisableAutoloadCheckbox->SetText("Disable autoload");
	DontAskAgainCheckbox->SetText("Don't ask me again");
	LightsCheckbox->SetText("Lights");
	BrightmapsCheckbox->SetText("Brightmaps");
	WidescreenCheckbox->SetText("Widescreen");

	FString welcomeText, versionText;
	welcomeText.Format("Welcome to %s!", GAMENAME);
	versionText.Format("Version %s.", GetVersionString());
	WelcomeLabel->SetText(welcomeText.GetChars());
	VersionLabel->SetText(versionText.GetChars());

	int flags = *autoloadflags;
	FullscreenCheckbox->SetChecked(vid_fullscreen);
	DisableAutoloadCheckbox->SetChecked(flags & 1);
	DontAskAgainCheckbox->SetChecked(!queryiwad);
	LightsCheckbox->SetChecked(flags & 2);
	BrightmapsCheckbox->SetChecked(flags & 4);
	WidescreenCheckbox->SetChecked(flags & 8);

	for (int i = 0; i < numwads; i++)
	{
		const char* filepart = strrchr(wads[i].Path.GetChars(), '/');
		if (filepart == NULL)
			filepart = wads[i].Path.GetChars();
		else
			filepart++;

		FString work;
		if (*filepart) work.Format("%s (%s)", wads[i].Name.GetChars(), filepart);
		else work = wads[i].Name.GetChars();

		GamesList->AddItem(work.GetChars());
	}

	/*try
	{
		auto filedata = ReadAllBytes("C:/Development/ZWidget/example/banner.png");
		std::vector<unsigned char> pixels;
		unsigned long width = 0, height = 0;
		int result = decodePNG(pixels, width, height, (const unsigned char*)filedata.data(), filedata.size(), true);
		if (result == 0)
		{
			Logo->SetImage(Image::Create(width, height, ImageFormat::R8G8B8A8, pixels.data()));
		}
	}
	catch (...)
	{
	}*/
}

void LauncherWindow::OnGeometryChanged()
{
	double y = 0.0;

	Logo->SetFrameGeometry(0.0, y, GetWidth(), Logo->GetPreferredHeight());
	y += Logo->GetPreferredHeight();

	y += 10.0;

	WelcomeLabel->SetFrameGeometry(20.0, y, GetWidth() - 40.0, WelcomeLabel->GetPreferredHeight());
	y += WelcomeLabel->GetPreferredHeight();

	VersionLabel->SetFrameGeometry(20.0, y, GetWidth() - 40.0, VersionLabel->GetPreferredHeight());
	y += VersionLabel->GetPreferredHeight();

	y += 10.0;

	SelectLabel->SetFrameGeometry(20.0, y, GetWidth() - 40.0, SelectLabel->GetPreferredHeight());
	y += SelectLabel->GetPreferredHeight();

	double listViewTop = y + 10.0;

	y = GetHeight() - 15.0 - PlayButton->GetPreferredHeight();
	PlayButton->SetFrameGeometry(20.0, y, 120.0, PlayButton->GetPreferredHeight());
	ExitButton->SetFrameGeometry(GetWidth() - 20.0 - 120.0, y, 120.0, PlayButton->GetPreferredHeight());

	y -= 20.0;

	double panelWidth = 150.0;
	y -= DontAskAgainCheckbox->GetPreferredHeight();
	DontAskAgainCheckbox->SetFrameGeometry(20.0, y, 190.0, DontAskAgainCheckbox->GetPreferredHeight());
	WidescreenCheckbox->SetFrameGeometry(GetWidth() - 20.0 - panelWidth, y, panelWidth, WidescreenCheckbox->GetPreferredHeight());

	y -= DisableAutoloadCheckbox->GetPreferredHeight();
	DisableAutoloadCheckbox->SetFrameGeometry(20.0, y, 190.0, DisableAutoloadCheckbox->GetPreferredHeight());
	BrightmapsCheckbox->SetFrameGeometry(GetWidth() - 20.0 - panelWidth, y, panelWidth, BrightmapsCheckbox->GetPreferredHeight());

	y -= FullscreenCheckbox->GetPreferredHeight();
	FullscreenCheckbox->SetFrameGeometry(20.0, y, 190.0, FullscreenCheckbox->GetPreferredHeight());
	LightsCheckbox->SetFrameGeometry(GetWidth() - 20.0 - panelWidth, y, panelWidth, LightsCheckbox->GetPreferredHeight());

	y -= GeneralLabel->GetPreferredHeight();
	GeneralLabel->SetFrameGeometry(20.0, y, 190.0, GeneralLabel->GetPreferredHeight());
	ExtrasLabel->SetFrameGeometry(GetWidth() - 20.0 - panelWidth, y, panelWidth, ExtrasLabel->GetPreferredHeight());

	double listViewBottom = y - 10.0;
	GamesList->SetFrameGeometry(20.0, listViewTop, GetWidth() - 40.0, std::max(listViewBottom - listViewTop, 0.0));
}


// To do: clean this up
#ifdef WIN32
#include <Windows.h>
#include <stdexcept>
static std::wstring to_utf16(const std::string& str)
{
	if (str.empty()) return {};
	int needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);
	if (needed == 0)
		throw std::runtime_error("MultiByteToWideChar failed");
	std::wstring result;
	result.resize(needed);
	needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &result[0], (int)result.size());
	if (needed == 0)
		throw std::runtime_error("MultiByteToWideChar failed");
	return result;
}

static std::vector<uint8_t> ReadAllBytes(const std::string& filename)
{
	HANDLE handle = CreateFile(to_utf16(filename).c_str(), 0x0001/*FILE_READ_ACCESS*/, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (handle == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Could not open " + filename);

	LARGE_INTEGER fileSize;
	BOOL result = GetFileSizeEx(handle, &fileSize);
	if (result == FALSE)
	{
		CloseHandle(handle);
		throw std::runtime_error("GetFileSizeEx failed");
	}

	std::vector<uint8_t> buffer(fileSize.QuadPart);

	DWORD bytesRead = 0;
	result = ReadFile(handle, buffer.data(), (DWORD)buffer.size(), &bytesRead, nullptr);
	if (result == FALSE || bytesRead != buffer.size())
	{
		CloseHandle(handle);
		throw std::runtime_error("ReadFile failed");
	}

	CloseHandle(handle);

	return buffer;
}

std::vector<uint8_t> LoadWidgetFontData(const std::string& name)
{
	return ReadAllBytes("C:\\Windows\\Fonts\\segoeui.ttf");
}

Size LauncherWindow::GetScreenSize()
{
	HDC screenDC = GetDC(0);
	int screenWidth = GetDeviceCaps(screenDC, HORZRES);
	int screenHeight = GetDeviceCaps(screenDC, VERTRES);
	double dpiScale = GetDeviceCaps(screenDC, LOGPIXELSX) / 96.0;
	ReleaseDC(0, screenDC);

	return Size(screenWidth / dpiScale, screenHeight / dpiScale);
}

#else
std::vector<uint8_t> LoadWidgetFontData(const std::string& name)
{
	return {};
}
#endif