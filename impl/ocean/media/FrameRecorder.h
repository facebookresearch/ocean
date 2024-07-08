/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_FRAME_RECORDER_H
#define META_OCEAN_MEDIA_FRAME_RECORDER_H

#include "ocean/media/Media.h"
#include "ocean/media/Recorder.h"

#include "ocean/base/Frame.h"
#include "ocean/base/SmartObjectRef.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class FrameRecorder;

/**
 * Definition of a object reference holding a frame recorder.
 * @see SmartObjectRef, Recorder.
 * @ingroup media
 */
typedef Ocean::SmartObjectRef<FrameRecorder, Recorder> FrameRecorderRef;

/**
 * This class is the base class for all frame recorders.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT FrameRecorder : virtual public Recorder
{
	public:

		/**
		 * Returns the actual frame type of this recorder.
		 * The actual frame type may be different from the preferred (requested) frametype.
		 * @return Frame type
		 * @see setPreferredFrameType().
		 */
		inline const FrameType& frameType() const;

		/**
		 * Returns the frame frequency of this recorder in Hz.
		 * @return Frame frequency
		 */
		inline double frameFrequency() const;

		/**
		 * Returns the name of the encoder used to encoder the frames.
		 * @return Encoder name
		 */
		inline const std::string& frameEncoder() const;

		/**
		 * Returns a list of possible frame encoders for this recorder.
		 * @return Encoder names
		 */
		virtual Encoders frameEncoders() const;

		/**
		 * Returns whether the selected frame encoder has a configuration possibility.
		 * @return True, if so
		 */
		virtual bool frameEncoderHasConfiguration();

		/**
		 * Sets the preferred frame type of this recorder.
		 * The recorder may not support the specific pixel format, therefore check the actual pixel format of the recorder.
		 * @param type The preferred frame type to record
		 * @return True, if succeeded
		 * @see frameType().
		 */
		virtual bool setPreferredFrameType(const FrameType& type);

		/**
		 * Sets the frame frequency of this recorder.
		 * @param frequency Frame frequency in Hz
		 * @return True, if succeeded
		 */
		virtual bool setFrameFrequency(const double frequency);

		/**
		 * Sets the frame encoder used to encode the frames.
		 * @param encoder Name of the encoder to use
		 * @return True, if succeeded
		 */
		virtual bool setFrameEncoder(const std::string& encoder);

		/**
		 * Starts the frame encoder configuration possibility.
		 * @param data Possible configuration data like gui handles.
		 * @return True, if succeeded
		 */
		virtual bool frameEncoderConfiguration(long long data);

		/**
		 * Locks the most recent frame and returns it so that the image data can be written to this frame.
		 * Beware: Call unlockBufferToFill() once the image data is written to the frame.
		 * @param recorderFrame The resulting frame in which the image data can be copied, the frame type of this frame must not be changed
		 * @param respectFrameFrequency Flag determining that a buffer will be returned if it is time for a new frame only
		 * @see unlockBufferToFill().
		 */
		virtual bool lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency = true) = 0;

		/**
		 * Unlocks the filled buffer.
		 * Beware: The buffer has to be locked by FrameRecorder::lockBufferToFill() before.
		 */
		virtual void unlockBufferToFill() = 0;

	protected:

		/**
		 * Creats a new frame recorder.
		 */
		FrameRecorder();

		/**
		 * Destructs a frame recorder.
		 */
		~FrameRecorder() override;

	protected:

		/// Type of the frame to record.
		FrameType recorderFrameType;

		/// Frame frequency used by the recorder.
		double recorderFrameFrequency;

		/// Name of the frame encoder used to encode the frames.
		std::string recorderFrameEncoder;
};

inline const FrameType& FrameRecorder::frameType() const
{
	return recorderFrameType;
}

inline double FrameRecorder::frameFrequency() const
{
	return recorderFrameFrequency;
}

inline const std::string& FrameRecorder::frameEncoder() const
{
	return recorderFrameEncoder;
}

}

}

#endif // META_OCEAN_MEDIA_FRAME_RECORDER_H
