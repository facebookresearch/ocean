/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_FRAME_RECORDER_H
#define META_OCEAN_MEDIA_DS_FRAME_RECORDER_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSSampleSourceFilter.h"
#include "ocean/media/directshow/DSRecorder.h"

#include "ocean/media/FrameRecorder.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class implements a DirectShow frame recorder.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSFrameRecorder :
	virtual public DSRecorder,
	virtual public FrameRecorder
{
	public:

		/**
		 * Returns a list of possible frame encoders for this recorder.
		 * @see FrameRecorder::frameEncoders().
		 */
		Encoders frameEncoders() const override;

		/**
		 * Returns whether the selected frame encoder has a configuration possibility.
		 * @see FrameRecorder::frameEncoderHasConfiguration().
		 */
		bool frameEncoderHasConfiguration() override;

		/**
		 * Sets the preferred frame type of this recorder.
		 * @see FrameRecorder::setPreferredFrameType().
		 */
		bool setPreferredFrameType(const FrameType& type) override;

		/**
		 * Sets the frame frequency of this recorder.
		 * @see FrameRecorder::setFrameFrequency().
		 */
		bool setFrameFrequency(const double frequency) override;

		/**
		 * Sets the frame encoder used to encode the frames.
		 * @see FrameRecorder::setFrameEncoder().
		 */
		bool setFrameEncoder(const std::string& encoder) override;

		/**
		 * Starts the frame encoder configuration possibility.
		 * @see FrameRecorder::frameEncoderConfiguration().
		 */
		bool frameEncoderConfiguration(long long data) override;

		/**
		 * Locks the most recent frame and returns it so that the image data can be written to this frame.
		 * @see FrameRecorder::lockBufferToFill().
		 */
		bool lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency = true) override;

		/**
		 * Unlocks the filled buffer.
		 * @see FrameRecorder::unlockBufferToFill().
		 */
		void unlockBufferToFill() override;

	protected:

		/**
		 * Creates a new DirectShow frame recorder.
		 */
		DSFrameRecorder() = default;

		/**
		 * Destructs a DirectShow frame recorder.
		 */
		~DSFrameRecorder() override;

		/**
		 * Inserts the frame source filter.
		 * @param sourceOutputPin Resulting source filter output pin which can be used for further connections
		 * @return True, if succeeded
		 */
		bool insertSourceFilter(ScopedIPin& sourceOutputPin);

		/**
		 * Inserts the frame encoder filter.
		 * @param outputPin Output pin which will be connected with the encoder filter input pin
		 * @param encoderOutputPin Resulting encoder output pin which can be used for further connections
		 * @return True, if succeeded
		 */
		bool insertFrameEncoderFilter(IPin* outputPin, ScopedIPin& encoderOutputPin);

		/**
		 * Releases the frame source filter.
		 */
		void releaseFrameSourceFilter();

		/**
		 * Releases the frame encoder filter.
		 */
		void releaseFrameEncoderFilter();

	protected:

		/// DirectShow frame encoder filter.
		ScopedIBaseFilter frameEncoderFilter_;

		/// DirectShow sample source filter.
		ScopedDSSampleSourceFilter sampleSourceFilter_;
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_FRAME_RECORDER_H
