/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_FILE_RECORDER_H
#define META_OCEAN_MEDIA_DS_FILE_RECORDER_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSRecorder.h"

#include "ocean/media/FileRecorder.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class implements a DirectShow file recorder.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSFileRecorder :
	virtual public DSRecorder,
	virtual public FileRecorder
{
	public:

		/**
		 * Sets the filename of this recorder.
		 * @see FileRecroder::setFilename().
		 */
		bool setFilename(const std::string& filename) override;

		/**
		 * Returns whether this recorder is currently recording.
		 * @see FileRecorder::isRecording().
		 */
		bool isRecording() const override;

		/**
		 * Sets the recorder.
		 * @see FileRecorder::start().
		 */
		bool start() override;

		/**
		 * Stops the recorder.
		 * @see FileRecorder::stop().
		 */
		bool stop() override;

	protected:

		/**
		 * Creates a new DirectShow file recorder.
		 */
		DSFileRecorder() = default;

		/**
		 * Destructs a DirectShow file recorder.
		 */
		~DSFileRecorder() override;

		/**
		 * Inserts an AVI MUX and file writer filter to the filter graph.
		 * The file writer filter should be connected with a MUX filter.
		 * @param outputPin Output pin of e.g. encoder filter which will be connected with the input pin of the file writer filter
		 * @return True, if succeeded
		 */
		virtual bool insertFileWriterFilter(IPin* outputPin);

		/**
		 * Releases the filter writer filter.
		 */
		void releaseFileWriterFilter();

	protected:

		/// AVI MUX filter.
		ScopedIBaseFilter aviMuxFilter_;

		/// DirectShow file writer filter.
		ScopedIBaseFilter fileWriterFilter_;

		/// DirectShow file sink interface.
		ScopedIFileSinkFilter fileSinkInterface_;
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_FILE_RECORDER_H
