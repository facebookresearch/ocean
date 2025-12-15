/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_IO_SERIALIZATION_PREVIEW_WIN_PREVIEW_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_IO_SERIALIZATION_PREVIEW_WIN_PREVIEW_MAIN_WINDOW_H

#include "application/ocean/demo/io/serialization/preview/win/PreviewMain.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/serialization/InputDataSerializer.h"
#include "ocean/io/serialization/MediaSerializer.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

/**
 * This class implements a main window.
 * @ingroup applicationdemoioserializationpreviewwin
 */
class PreviewMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param filename The filename of the serialization file to preview, empty to open file dialog
		 */
		PreviewMainWindow(HINSTANCE instance, const std::wstring& name, const std::wstring& filename);

		/**
		 * Destructs the main window.
		 */
		~PreviewMainWindow() override;

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		void onIdle() override;

		/**
		 * Function for keyboard button down events.
		 * @see Window::onKeyDown().
		 */
		void onKeyDown(const int key) override;

		/**
		 * Event function for drag&drop events for files.
		 * @see Window::onDragAndDrop().
		 */
		void onDragAndDrop(const Files& files) override;

		/**
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

		/**
		 * Loads and starts playing a serialization file.
		 * @param filename The filename of the serialization file to load
		 * @return True, if succeeded
		 */
		bool loadFile(const std::wstring& filename);

		/**
		 * Stops the serializer and releases resources.
		 */
		void stopSerializer();

		/**
		 * Opens a file dialog to select a serialization file.
		 * @return The selected filename, empty if canceled
		 */
		std::wstring openFileDialog();

	protected:

		/// The input serializer.
		std::unique_ptr<IO::Serialization::FileInputDataSerializer> serializer_;

		/// The timestamp of the most recent input frame.
		Timestamp frameTimestamp_;

		/// The filename to open.
		std::wstring filename_;

		/// The current rotation angle in degrees (0, 90, 180, 270).
		int rotationAngle_ = 0;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_IO_SERIALIZATION_PREVIEW_WIN_PREVIEW_MAIN_WINDOW_H
