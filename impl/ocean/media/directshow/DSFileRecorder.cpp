/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSFileRecorder.h"

#include "ocean/base/DateTime.h"
#include "ocean/base/ScopedFunction.h"
#include "ocean/base/String.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSFileRecorder::~DSFileRecorder()
{
	ocean_assert(!aviMuxFilter_.isValid());
	ocean_assert(!fileWriterFilter_.isValid());
	ocean_assert(!fileSinkInterface_.isValid());
}

bool DSFileRecorder::setFilename(const std::string& filename)
{
	if (graphIsRunning_)
	{
		return false;
	}

	return FileRecorder::setFilename(filename);
}

bool DSFileRecorder::isRecording() const
{
	return graphIsRunning_;
}

bool DSFileRecorder::start()
{
	if (graphIsRunning_)
	{
		return true;
	}

	if (fileSinkInterface_.isValid())
	{
		std::string filename = recorderFilename;

		if (recorderFilenameSuffixed)
		{
			std::string::size_type dot = recorderFilename.rfind('.');

			std::string dateTime(DateTime::localString());

			if (dot != std::string::npos && dot != 0 && dot != recorderFilename.length() - 1)
			{
				std::string front = recorderFilename.substr(0, dot);
				std::string back = recorderFilename.substr(dot + 1);

				filename = front + std::string(" ") + dateTime + std::string(".") + back;
			}
			else
			{
				filename += std::string(" ") + dateTime;
			}
		}

		if (S_OK != fileSinkInterface_->SetFileName(String::toWString(filename).c_str(), nullptr))
		{
			return false;
		}
	}

	return startGraph();
}

bool DSFileRecorder::stop()
{
	return stopGraph();
}

bool DSFileRecorder::insertFileWriterFilter(IPin* outputPin)
{
	ocean_assert(outputPin != nullptr);
	ocean_assert(filterGraph_.isValid());

	if (fileWriterFilter_.isValid())
	{
		return true;
	}

	ocean_assert(!fileSinkInterface_.isValid());

	ScopedFunctionVoid scopedReleaseFileWriterFilterFunction(std::bind(&DSFileRecorder::releaseFileWriterFilter, this)); // scoped function which will be invoked in case we don't reach the both of this function

	if (S_OK != CoCreateInstance(CLSID_AviDest, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)(&aviMuxFilter_.resetObject())))
	{
		Log::error() << "Could not create an AVI MUX filter for \"" << recorderFilename << "\".";

		return false;
	}

	if (S_OK != filterGraph_->AddFilter(*aviMuxFilter_, L"AVI MUX filter"))
	{
		Log::error() << "Could not add the AVI MUX filter to the filter graph.";

		return false;
	}

	const ScopedIPin aviMuxFilterInputPin = firstPin(*aviMuxFilter_, PTYPE_INPUT, CTYPE_NOT_CONNECTED);
	if (!aviMuxFilterInputPin.isValid())
	{
		Log::error() << "Could not find a free input pin of the just created AVI MUX filter.";

		return false;
	}

	const ScopedIPin aviMuxFilterOutputPin = firstPin(*aviMuxFilter_, PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);
	if (!aviMuxFilterOutputPin.isValid())
	{
		Log::error() << "Could not find a free output pin of the just created AVI MUX filter.";

		return false;
	}

	if (S_OK != CoCreateInstance(CLSID_FileWriter, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)(&fileWriterFilter_.resetObject())))
	{
		Log::error() << "Could not create a file writer filter for \"" << recorderFilename << "\".";

		return false;
	}

	if (S_OK != filterGraph_->AddFilter(*fileWriterFilter_, String::toWString(recorderFilename).c_str()))
	{
		Log::error() << "Could not add the file writer filter to the filter graph.";

		return false;
	}

	const ScopedIPin fileWriterFilterInputPin = firstPin(*fileWriterFilter_, PTYPE_INPUT, CTYPE_NOT_CONNECTED);
	if (!fileWriterFilterInputPin.isValid())
	{
		Log::error() << "Could not find a free input pin of the just created file writer filter.";

		return false;
	}

	if (S_OK != filterGraph_->Connect(outputPin, *aviMuxFilterInputPin))
	{
		Log::error() << "Could not connect the given output pin with the AVI MUX input pin.";

		return false;
	}

	if (S_OK != filterGraph_->Connect(*aviMuxFilterOutputPin, *fileWriterFilterInputPin))
	{
		Log::error() << "Could not connect the AVI MUX output pin with the file writer input pin.";

		return false;
	}

	if (S_OK != fileWriterFilter_->QueryInterface(IID_IFileSinkFilter, (void**)(&fileSinkInterface_.resetObject())))
	{
		Log::error() << "Could not receive a file sink interface from the file writer filter.";

		return false;
	}

	if (S_OK != fileSinkInterface_->SetFileName(String::toWString(recorderFilename).c_str(), nullptr))
	{
		Log::error() << "Could not set the filename \"" << recorderFilename << "\" of the file writer filter.";

		return false;
	}

	scopedReleaseFileWriterFilterFunction.revoke(); // we don't call the releaseFileWriterFilter() function

	return true;
}

void DSFileRecorder::releaseFileWriterFilter()
{
	if (aviMuxFilter_.isValid())
	{
		ocean_assert(filterGraph_.isValid());

		filterGraph_->RemoveFilter(*aviMuxFilter_);
	}

	if (fileWriterFilter_.isValid())
	{
		ocean_assert(filterGraph_.isValid());

		filterGraph_->RemoveFilter(*fileWriterFilter_);
	}

	fileSinkInterface_.release();
	aviMuxFilter_.release();
	fileWriterFilter_.release();
}

}

}

}
