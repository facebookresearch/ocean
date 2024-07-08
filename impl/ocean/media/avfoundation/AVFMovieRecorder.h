/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_AVF_MOVIE_RECORDER_H
#define META_OCEAN_MEDIA_AVF_MOVIE_RECORDER_H

#include "ocean/media/avfoundation/AVFoundation.h"
#include "ocean/media/avfoundation/PixelBufferAccessor.h"

#include "ocean/media/MovieRecorder.h"

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

/**
 * This class implements a AVFoundation movie recorder.
 * The recorder can record mp4 files and mov files.
 * @ingroup mediaavf
 */
class AVFMovieRecorder : virtual public MovieRecorder
{
	friend class AVFLibrary;

	public:

		/**
		 * Sets the filename of this recorder.
		 * The filename must be set before recordings starts.
		 * @see MovieRecorder::setFilename().
		 */
		bool setFilename(const std::string& filename) override;

		/**
		 * Sets the recorder.
		 * @see ExplicitRecorder::start().
		 */
		bool start() override;

		/**
		 * Stops the recorder.
		 * @see ExplicitRecorder::stop().
		 */
		bool stop() override;

		/**
		 * Returns whether this recorder is currently recording.
		 * @return True, if so
		 */
		bool isRecording() const override;

		/**
		 * Returns a list of possible frame encoders for this recorder.
		 * @return Encoder names
		 */
		Encoders frameEncoders() const override;

		/**
		 * Returns a pointer to the most recent buffer to be filled immediately and locks it.
		 * Beware: Call unlockBufferToFill() once the image data is written to the frame.
		 * @param recorderFrame The resulting frame in which the image data can be copied, the frame type of this frame must not be changed
		 * @param respectFrameFrequency Flag determining that a buffer will be returned if it is time for a new frame only
		 */
		bool lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency = true) override;

		/**
		 * Unlocks the filled buffer.
		 * Beware: The buffer has to be locked by lockBufferToFill() before.
		 */
		void unlockBufferToFill() override;

		/**
		 * Translates a frame encoder to a corresponding AVVideoCodecType.
		 * @param frameEncoder The name of the encoder
		 * @return The corresponding AVVideoCodecType value, if existing, invalid otherwise
		 * @see frameEncoders().
		 */
		static AVVideoCodecType frameEncoderToVideoCodecType(const std::string& frameEncoder);

		/**
		 * Translates the file extension of a movie to a corresponding AVFileType.
		 * The following extensions are supported: mp4, mov.
		 * @param fileExtension The extension of the movie's filename
		 * @return The corresponding AVFileType value, if existing, invalid otherwise
		 */
		static AVFileType fileExtensionToFileType(const std::string& fileExtension);

	protected:

		/**
		 * Creates a new movie recorder object.
		 */
		AVFMovieRecorder();

		/**
		 * Destructs a movie recorder object.
		 */
		~AVFMovieRecorder() override;

		/**
		 * Creates a new asset writer and all associated resources.
		 * @return True, if succeeded
		 */
		bool createNewAssetWriter();

		/**
		 * Releases all resources.
		 */
		void release();

		/**
		 * Returns the time for a given timestamp.
		 * @param timestamp Playback timestamp in seconds, with range [0, infinity)
		 */
		inline CMTime time(const double timestamp);

		/**
		 * Returns the best matching pixel format for a preferred pixel format.
		 * Depending on the platform, not each preferred pixel format may be supported.
		 * This function allows to determine the best matching pixel format closes to the preferred pixel format.
		 * @param preferredPixelFormat The preferred pixel format, can be invalid
		 * @return The supported pixel format best matching with the preferred pixel format
		 */
		static FrameType::PixelFormat bestMatchingPixelFormat(const FrameType::PixelFormat preferredPixelFormat);

	protected:

		/// The asset writer.
		AVAssetWriter* assetWriter_ = nullptr;

		/// The input for the asset writer.
		AVAssetWriterInput* assetWriterInput_ = nullptr;

		/// The pixel buffer adaptor for the input asset writer.
		AVAssetWriterInputPixelBufferAdaptor* assetWriterInputPixelBufferAdaptor_ = nullptr;

		/// The pixel buffer to be filled.
		CVPixelBufferRef pixelBuffer_ = nullptr;

		/// The pixel buffer accessor.
		PixelBufferAccessor pixelBufferAccessor_;

		/// The timestamp of the next frame.
		double nextFrameTimestamp_ = 0.0;

		/// The timestamp of the previous frame.
		double previousFrameTimestamp_ = -1.0;

		/// True, if the recoder is actively recording frames; False, otherwise.
		bool isRecording_ = false;

		/// True, if this recorder is stopped.
		bool isStopped_ = true;
};

inline CMTime AVFMovieRecorder::time(const double timestamp)
{
	ocean_assert(timestamp >= 0.0);

	return CMTimeMakeWithSeconds(timestamp, 600);
}

}

}

}

#endif // META_OCEAN_MEDIA_AVF_MOVIE_RECORDER_H
