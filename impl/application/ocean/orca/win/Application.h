/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_APPLICATION_H
#define FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_APPLICATION_H

#include "application/ocean/orca/win/OrcaWin.h"
#include "application/ocean/orca/win/Resource.h"

#include "ocean/base/Config.h"

#include "ocean/io/FileManager.h"

#include "ocean/platform/Utilities.h"

#include "ocean/scenedescription/SDLScene.h"

namespace Ocean
{

namespace Orca
{

namespace Win
{

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
		enum FileType : uint32_t
		{
			/// Invalid file type.
			TYPE_INVALID = 0u,
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
		typedef std::unordered_set<std::wstring> ConfigurationSet;

	public:

		/**
		 * Creates a new application.
		 */
		Application();

		/**
		 * Destructs the application.
		 */
		~Application() override;

		/**
		 * Returns whether the application uses the default plugin directory.
		 * @return True, if so
		 */
		bool usesDefaultPluginDirectory() const;

		/**
		 * Returns the configuration of the application.
		 * @return The application's configuration
		 */
		inline Config& config();

		/**
		 * Converts filenames into one single string.
		 * The filenames will be separated by a ';'.
		 * @param filenames The filenames to convert
		 * @return Converted single string
		 */
		static std::string convertFilenames(const Filenames& filenames);

		/**
		 * Converts a single string holding several filenames separated by a ';' into a list of filenames.
		 * @param filenames Single string to convert
		 * @return Converted filenames
		 */
		static Filenames convertFilenames(const std::string& filenames);

		/**
		 * Returns the unique application object.
		 * @return Application object
		 */
		static Application& get();

	private:

		/**
		 * Initializes the application instance.
		 * @return True, if succeeded
		 */
		BOOL InitInstance() override;

		/**
		 * The GUI framework calls this function if the application is in an idle state.
		 * @param count Idle iteration counter
		 * @return True, if more idle time is needed
		 */
		BOOL OnIdle(LONG count) override;

		/**
		 * Opens one or more files.
		 * @param replace True, if the files have will be replace, false if the files will be added
		 */
		void openFiles(const bool replace = true);

		/**
		 * Opens a file dialog and returns a list of files to load.
		 * @param fileExtensions File extensions to create the filter for
		 * @return Selected files
		 */
		Filenames openFileDialog(const IO::FileManager::FileExtensions& fileExtensions);

		/**
		 * The GUI framework calls this function if the application has to open a file.
		 */
		afx_msg void OnFileOpen();

		/**
		 * The GUI framework calls this function if the application has to add a file.
		 */
		void onFileAdd();

		/**
		 * The GUI framework calls this function if the application has to reload the most recent file.
		 */
		void onFileReload();

		/**
		 * The GUI framework calls this function if the application has to close all loaded file.
		 */
		void onFileClose();

		/**
		 * The GUI framework calls this function if the application has to show the about window.
		 */
		void onAboutWindow();

		/**
		 * The GUI framework calls this function if the application has to fit the scene to the current screen.
		 */
		void onNavigationFitToScreen();

		/**
		 * Called by the GUI framework from within the Run member function to exit this instance of the application.
		 * @return The application's exit code; 0 indicates no errors
		 */
		int ExitInstance() override;

		/**
		 * Returns the filter for an open file dialog.
		 * @param fileExtensions File extensions to create the filter for
		 * @return File filter
		 */
		static std::string fileFilter(const IO::FileManager::FileExtensions& fileExtensions);

		/**
		 * Runtime event message function declaration.
		 */
		DECLARE_MESSAGE_MAP();

	protected:

		/// Configuration object.
		std::unique_ptr<Config> config_;

		/// Application commands.
		Platform::Utilities::Commands commands_;

		/// Predefined application properties.
		ConfigurationSet configurationSet_;
};

inline Config& Application::config()
{
	ocean_assert(config_);

	return *config_.get();
}

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_ORCA_WIN_APPLICATION_H
