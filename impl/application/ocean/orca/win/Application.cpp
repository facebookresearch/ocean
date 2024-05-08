/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/orca/win/Application.h"
#include "application/ocean/orca/win/AboutWindow.h"
#include "application/ocean/orca/win/MainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/fonts/FontManager.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/FileConfig.h"
#include "ocean/io/FileResolver.h"

#include "ocean/interaction/Manager.h"

#include "ocean/media/Manager.h"

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/mediafoundation/MediaFoundation.h"
#include "ocean/media/wic/WIC.h"

#include "ocean/platform/win/RegistryConfig.h"
#include "ocean/platform/win/System.h"

#include "ocean/rendering/Manager.h"

#include "ocean/rendering/glescenegraph/windows/Windows.h"
#include "ocean/rendering/globalillumination/GlobalIllumination.h"

#include "ocean/scenedescription/Manager.h"
#include "ocean/scenedescription/sdl/assimp/Assimp.h"
#include "ocean/scenedescription/sdl/obj/OBJ.h"
#include "ocean/scenedescription/sdx/x3d/X3D.h"

namespace Ocean
{

namespace Orca
{

namespace Win
{

Application application;

BEGIN_MESSAGE_MAP(Application, CWinApp)
	ON_COMMAND(ID_FILE_OPEN, &Application::OnFileOpen)
	ON_COMMAND(ID_FILE_ADD, &Application::onFileAdd)
	ON_COMMAND(ID_FILE_RELOAD, &Application::onFileReload)
	ON_COMMAND(ID_FILE_CLOSE, &Application::onFileClose)
	ON_COMMAND(ID_APP_ABOUT, &Application::onAboutWindow)
	ON_COMMAND(ID_NAVIGATION_FIT_TO_SCREEN, &Application::onNavigationFitToScreen)
END_MESSAGE_MAP()

Application::Application() :
	CWinApp()
{
	configurationSet_.emplace(L"defaultplugindirectory");
}

Application::~Application()
{
	// nothing to do here
}

std::string Application::convertFilenames(const Filenames& filenames)
{
	std::string result;

	for (Filenames::const_iterator i = filenames.begin(); i != filenames.end(); ++i)
	{
		if (i != filenames.begin())
		{
			result += ";";
		}

		result += *i;
	}

	return result;
}

Application::Filenames Application::convertFilenames(const std::string& filenames)
{
	std::string value(filenames);
	Filenames result;

	while (true)
	{
		std::string::size_type pos = value.find(";");
		std::string filename = value.substr(0, pos);

		if (filename.empty())
		{
			break;
		}

		result.emplace_back(std::move(filename));

		if (pos == std::string::npos)
		{
			break;
		}

		value = value.substr(pos + 1);
	}

	return result;
}

bool Application::usesDefaultPluginDirectory() const
{
	for (const std::wstring& command : commands_)
	{
		if (command == L"defaultplugindirectory")
		{
			return true;
		}
	}

	return false;
}

Application& Application::get()
{
	ocean_assert(AfxGetApp() != nullptr);
	ocean_assert(dynamic_cast<Application*>(AfxGetApp()) != nullptr);

	return *(Application*)(AfxGetApp());
}

BOOL Application::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls = {};
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	Messenger::get().setOutputType(Messenger::MessageOutput(Messenger::OUTPUT_DEBUG_WINDOW | Messenger::OUTPUT_QUEUED));

	commands_ = Platform::Utilities::parseCommandLine(m_lpCmdLine);

	for (const std::wstring& command : commands_)
	{
		if (configurationSet_.find(command) == configurationSet_.cend())
		{
			const IO::File configFile(String::toAString(command));

			if (String::toLower(configFile.extension()) == "ocf")
			{
				if (configFile.exists())
				{
					config_ = std::make_unique<IO::FileConfig>(configFile());
					break;
				}
				else
				{
					Log::warning() << "Could not find config file \"" << configFile() << "\".";
				}
			}
		}
	}

	if (!config_)
	{
		config_ = std::make_unique<Platform::Win::RegistryConfig>("Software\\Ocean\\Orca\\0.9");
	}

	ocean_assert(config_);

	WorkerPool::get().setCapacity(4);

	CV::Fonts::FontManager::get().registerSystemFonts();

	const IO::Directory currentDirectory(Platform::Win::System::currentDirectory());
	IO::FileResolver::get().addReferencePath(currentDirectory);
	Log::info() << "Added the current path \"" << currentDirectory() << "\" to the file resolver.";

	const IO::Directory processDirectory(Platform::Win::System::processDirectory());
	IO::FileResolver::get().addReferencePath(processDirectory);
	Log::info() << "Added the process path \"" << processDirectory() << "\" to the file resolver.";

	const IO::File cameraCalibrationFile((*config_)["application"]["cameracalibrationfile"](""));

	if (cameraCalibrationFile.isValid())
	{
		if (IO::CameraCalibrationManager::get().registerCalibrationFile(cameraCalibrationFile()))
		{
			Log::info() << "Successfully loaded the camera calibration file.";
		}
		else
		{
			Log::warning() << "Failed to load the camera calibration file \"" << cameraCalibrationFile() << "\".";
		}
	}
	else
	{
		IO::File file(processDirectory + IO::File("data/cameracalibration/cameracalibration.occ"));

		if (IO::CameraCalibrationManager::get().registerCalibrationFile(file()))
		{
			Log::info() << "Successfully loaded the camera calibration file \"" << file() << "\".";
		}
		else
		{
			Log::warning() << "Failed to load the camera calibration file \"" << file() << "\".";
		}
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::registerWICLibrary();
	Media::DirectShow::registerDirectShowLibrary();
	Media::MediaFoundation::registerMediaFoundationLibrary();

	Rendering::GLESceneGraph::Windows::registerGLESceneGraphEngine();
	Rendering::GlobalIllumination::registerGlobalIlluminationEngine();

	SceneDescription::SDL::OBJ::registerOBJLibrary();
	SceneDescription::SDX::X3D::registerX3DLibrary();
	SceneDescription::SDL::Assimp::registerAssimpLibrary();
#else
	IO::Directory directory((*config_)["plugins"]["version"][Build::buildString()]["plugindirectory"](""));
	if (directory.isNull())
	{
		directory = IO::Directory((*config_)["plugins"]["plugindirectory"](""));
	}

	if (usesDefaultPluginDirectory() || directory.isValid() == false)
	{
		directory = IO::Directory(processDirectory + IO::Directory("plugins\\"));
	}

	PluginManager::get().collectPlugins(directory());
	PluginManager::get().loadAllPlugins();
#endif

	MainWindow* mainWindow = new MainWindow;

	m_pMainWnd = mainWindow;
	mainWindow->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, nullptr, nullptr);
	mainWindow->ShowWindow(SW_SHOW);
	mainWindow->UpdateWindow();
	mainWindow->applyConfiguration();

	MainWindow::Filenames files;

	for (const std::wstring& command : commands_)
	{
		if (configurationSet_.find(command) == configurationSet_.end())
		{
			const IO::File configFile(String::toAString(command));

			if (String::toLower(configFile.extension()) != std::string("ocf"))
			{
				files.emplace_back(String::toAString(command));
			}
		}
	}

	if (!files.empty())
	{
		MainWindow::Filenames resolvedFileStrings;

		for (MainWindow::Filenames::const_iterator i = files.begin(); i != files.end(); ++i)
		{
			const IO::Files resolvedFiles(IO::FileResolver::get().resolve(IO::File(*i), true));

			if (resolvedFiles.empty())
			{
				Log::error() << "Failed to find file: \"" << *i << "\".";
			}
			else
			{
				resolvedFileStrings.push_back(resolvedFiles.front()());
			}
		}

		mainWindow->loadFiles(resolvedFileStrings, true);
	}

	return TRUE;
}

BOOL Application::OnIdle(LONG /*count*/)
{
	if (m_pMainWnd == nullptr)
	{
		return FALSE;
	}

	ocean_assert(dynamic_cast<MainWindow*>(m_pMainWnd) != nullptr);

	if (((MainWindow*)m_pMainWnd)->onIdle())
	{
		return TRUE;
	}

	return FALSE;
}

void Application::openFiles(const bool replace)
{
	IO::FileManager::FileExtensions fileExtensions;

	const IO::FileManager::FileExtensions sceneFileExtensions = SceneDescription::Manager::get().supportedExtensions();
	const IO::FileManager::FileExtensions interactionFileExtensions = Interaction::Manager::get().supportedExtensions();

	fileExtensions.insert(sceneFileExtensions.begin(), sceneFileExtensions.end());
	fileExtensions.insert(interactionFileExtensions.begin(), interactionFileExtensions.end());

	ocean_assert(fileExtensions.find("oproj") == fileExtensions.end());
	fileExtensions["oproj"] = "Ocean project file.";

	const Filenames filenames(openFileDialog(fileExtensions));
	if (filenames.empty())
	{
		return;
	}

	MainWindow* mainWindow = dynamic_cast<MainWindow*>(m_pMainWnd);
	ocean_assert(mainWindow);

	if (mainWindow)
	{
		mainWindow->loadFiles(filenames, !replace);
	}
}

Application::Filenames Application::openFileDialog(const IO::FileManager::FileExtensions& fileExtensions)
{
	ocean_assert(config_);
	if (!config_)
	{
		return Filenames();
	}

	const std::string filter(fileFilter(fileExtensions));
	const std::string initialFilepath = (*config_)["application"]["lastfilepath"]("");

	CFileDialog openDialog(TRUE, nullptr, nullptr, OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_ENABLESIZING, String::toWString(filter).c_str(), nullptr, 0, TRUE);

	wchar_t pathnameBuffer[32768];
	memset(pathnameBuffer, 0, sizeof(wchar_t) * 32768);

	openDialog.m_ofn.nFilterIndex = DWORD(fileExtensions.size() + 1);
	openDialog.m_ofn.lpstrFile = pathnameBuffer;
	openDialog.m_ofn.nMaxFile = 32767;

	std::wstring directory(String::toWString(initialFilepath));
	openDialog.m_ofn.lpstrInitialDir = directory.c_str();

	Filenames filenames;

	if (openDialog.DoModal() == IDOK)
	{
		POSITION pos = openDialog.GetStartPosition();

		while (pos != nullptr)
		{
			std::string filename(String::toAString(openDialog.GetNextPathName(pos).GetBuffer()));
			ocean_assert(filename.empty() == false);

			filenames.push_back(filename);
		}
	}

	return filenames;
}

void Application::OnFileOpen()
{
	openFiles();
}

void Application::onFileAdd()
{
	openFiles(false);
}

void Application::onFileReload()
{
	ocean_assert(config_);
	if (!config_)
	{
		return;
	}

	Filenames filenames(convertFilenames((*config_)["application"]["lastfilenames"]("")));

	if (filenames.empty())
	{
		Log::warning() << "There is no file to reload.";
		return;
	}

	MainWindow* mainWindow = dynamic_cast<MainWindow*>(m_pMainWnd);
	ocean_assert(mainWindow);

	if (mainWindow)
	{
		mainWindow->loadFiles(filenames);
	}
}

void Application::onFileClose()
{
	ContentManager::get().cancel();
	ContentManager::get().removeContent();
}

void Application::onAboutWindow()
{
	AboutWindow aboutWindow;
	aboutWindow.DoModal();
}

int Application::ExitInstance()
{
	ContentManager::get().release();

#ifdef OCEAN_RUNTIME_STATIC
	SceneDescription::SDL::Assimp::unregisterAssimpLibrary();
	SceneDescription::SDX::X3D::unregisterX3DLibrary();
	SceneDescription::SDL::OBJ::unregisterOBJLibrary();

	Rendering::GlobalIllumination::unregisterGlobalIlluminationEngine();
	Rendering::GLESceneGraph::unregisterGLESceneGraphEngine();

	Media::MediaFoundation::unregisterMediaFoundationLibrary();
	Media::DirectShow::unregisterDirectShowLibrary();
	Media::WIC::unregisterWICLibrary();
#else
	PluginManager::get().release();
#endif

	return CWinApp::ExitInstance();
}

void Application::onNavigationFitToScreen()
{
	MainWindow* mainWindow = dynamic_cast<MainWindow*>(m_pMainWnd);
	ocean_assert(mainWindow);
	if (mainWindow == nullptr)
	{
		return;
	}

	mainWindow->mainView().fitSceneToScreen();
}

std::string Application::fileFilter(const IO::FileManager::FileExtensions& fileExtension)
{
	std::string filter;

	if (fileExtension.empty() == false)
	{
		for (IO::FileManager::FileExtensions::const_iterator i = fileExtension.begin(); i != fileExtension.end(); ++i)
		{
			filter += i->second + std::string(" (*.") + i->first + std::string(")|*.") + i->first + std::string("|");
		}

		filter += "All supported files|";

		for (IO::FileManager::FileExtensions::const_iterator i = fileExtension.begin(); i != fileExtension.end(); ++i)
		{
			if (i != fileExtension.begin())
			{
				filter += ";";
			}

			filter += std::string("*.") + i->first;
		}

		filter += "|";
	}

	return filter;
}

}

}

}
