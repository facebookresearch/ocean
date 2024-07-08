/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_FILE_RECORDER_H
#define META_OCEAN_MEDIA_FILE_RECORDER_H

#include "ocean/media/Media.h"
#include "ocean/media/ExplicitRecorder.h"

#include "ocean/base/SmartObjectRef.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class FileRecorder;

/**
 * Definition of a object reference holding a file recorder.
 * @see SmartObjectRef, FileRecorder.
 * @ingroup media
 */
typedef Ocean::SmartObjectRef<FileRecorder, Recorder> FileRecorderRef;

/**
 * This class is the base class for all file recorders.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT FileRecorder : virtual public ExplicitRecorder
{
	public:

		/**
		 * Returns the filename of this recorder.
		 * @return Recorder filename
		 * @see setFilename().
		 */
		inline const std::string& filename() const;

		/**
		 * Returns whether the filename of this recorder will be suffixed with date and time.
		 * The default value is True.
		 * @return True, if so
		 * @see setFilenameSuffixed().
		 */
		inline bool filenameSuffixed() const;

		/**
		 * Sets the filename of this recorder.
		 * @param filename Filename to set
		 * @return True, if succeeded
		 * @see filename().
		 */
		virtual bool setFilename(const std::string& filename);

		/**
		 * Sets whether the filename will be suffixed with date and time.
		 * @param suffixed Determines whether the filename will be suffixed
		 * @return True, if succeeded
		 * @see filenameSuffixed().
		 */
		virtual bool setFilenameSuffixed(const bool suffixed);

	protected:

		/**
		 * Creates a new file recorder.
		 */
		FileRecorder();

		/**
		 * Destructs a file recorder.
		 */
		~FileRecorder() override;


	// **TODO** Remove public access to the functions below once file recorders expose filenames
	public:

		/**
		 * Determines the filename to be used for the next file.
		 * @param filename Initial name of the file, must be valid
		 * @param addSuffix True, to add a date/time-based suffix to the filename
		 * @return The filename to be used
		 */
		static std::string addOptionalSuffixToFilename(const std::string& filename, const bool addSuffix);

		/**
		 * Determines the filename to be used for the next file.
		 * @param filename Initial name of the file, must be valid
		 * @param index The index of the file, with range [0, infinity)
		 * @param addSuffix True, to add a date/time-based suffix to the filename
		 * @return The filename to be used
		 */
		static std::string addOptionalSuffixToFilename(const std::string& filename, const unsigned int index, const bool addSuffix);

	protected:

		/// Filename of this recorder.
		std::string recorderFilename;

		/// Flag determining whether the filename will be suffixed with date and time.
		bool recorderFilenameSuffixed;
};

inline const std::string& FileRecorder::filename() const
{
	return recorderFilename;
}

inline bool FileRecorder::filenameSuffixed() const
{
	return recorderFilenameSuffixed;
}

}

}

#endif // META_OCEAN_MEDIA_FILE_RECORDER_H
