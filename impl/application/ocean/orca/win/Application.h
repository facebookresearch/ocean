// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_APPLICATION_H
#define FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_APPLICATION_H

#include "application/ocean/orca/win/OrcaWin.h"
#include "application/ocean/orca/win/Resource.h"

#include "ocean/base/Config.h"

#include "ocean/io/FileManager.h"

#include "ocean/platform/Utilities.h"

#include "ocean/scenedescription/SDLScene.h"

#include <set>
#include <vector>

namespace Ocean
{

namespace Orca
{

namespace Win
{

extern Config* config;

/**
 * This class implements the main application for the ocean orca viewer.
 * @ingroup orcawin
 */
class Application : public CWinApp
{
	public:

		/**
		 * Definition of different file types.
		 */
		enum FileType
		{
			/// Invalid file type.
			TYPE_INVALID,
			/// Scene file type.
			TYPE_SCENE,
			/// Interaction file type.
			TYPE_INTERACTION,
			/// Project file type.
			TYPE_OCEAN
		};

		/**
		 * Definition of a vector holding filenames.
		 */
		typedef std::vector<std::string> Filenames;

		/**
		 * Definition of a set holding predefined configuration parameters.
		 */
		typedef std::set<std::wstring> Configurations;

	public:

		/**
		 * Creates a new application.
		 */
		Application();

		/**
		 * Destructs the application.
		 */
		virtual ~Application();

		/**
		 * Converts filenames into one single string.
		 * The filesnames will be seperated by a ';'.
		 * @param filenames Filenames to convert
		 * @return Converted single string
		 */
		static std::string convertFilenames(const Filenames& filenames);

		/**
		 * Converts a single string holding several filenames seperated by a ';' into a list of filenames.
		 * @param filenames Single string to convert
		 * @return Converted filenames
		 */
		static Filenames convertFilenames(const std::string& filenames);

		/**
		 * Returns whether the application uses the default plugin directory.
		 * @return True, if so
		 */
		bool usesDefaultPluginDirectory() const;

		/**
		 * Returns the unique application object.
		 * @return Application object
		 */
		static Application& application();

	private:

		/**
		 * Initializes the application instance.
		 * @return True, if succeeded
		 */
		virtual BOOL InitInstance();

		/**
		 * The gui framework calls this function if the application is in an idle state.
		 * @param count Idle iteration counter
		 * @return True, if more idle time is needed
		 */
		virtual BOOL OnIdle(LONG count);

		/**
		 * Opens one or more files.
		 * @param replace True, if the files have will be replace, false if the files will be added
		 */
		void openFiles(const bool replace = true);

		/**
		 * Opens a file dialog and returns a list of files to load.
		 * @param fileExtensions File extentions to create the filter for
		 * @return Selected files
		 */
		Filenames openFileDialog(const IO::FileManager::FileExtensions& fileExtensions);

		/**
		 * The gui framework calls this function if the application has to open a file.
		 */
		afx_msg void OnFileOpen();

		/**
		 * The gui framework calls this function if the application has to add a file.
		 */
		void onFileAdd();

		/**
		 * The gui framework calls this function if the application has to reload the most recent file.
		 */
		void onFileReload();

		/**
		 * The gui framework calls this function if the application has to close all loaded file.
		 */
		void onFileClose();

		/**
		 * The gui framework calls this function if the application has to show the about window.
		 */
		void onAboutWindow();

		/**
		 * The gui framework calls this function if the application has to fit the scene to the current screen.
		 */
		void onNavigationFitToScreen();

		/**
		 * Called by the gui framework from within the Run member function to exit this instance of the application.
		 * @return The application's exit code; 0 indicates no errors
		 */
		virtual int ExitInstance();

		/**
		 * Returns the filter for an open file dialog.
		 * @param fileExtensions File extentions to create the filter for
		 * @return File filter
		 */
		static std::string fileFilter(const IO::FileManager::FileExtensions& fileExtensions);

		/**
		 * Runtime event message function declaration.
		 */
		DECLARE_MESSAGE_MAP();

	protected:

		/// Configuration object.
		Config* applicationConfig;

		/// Application commands.
		Platform::Utilities::Commands applicationCommands;

		/// Predefined application properties.
		Configurations applicationConfigurations;
};

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_APPLICATION_H
