// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
	ocean_assert(aviMuxFilter_.object() == nullptr);
	ocean_assert(fileWriterFilter_.object() == nullptr);
	ocean_assert(fileSinkInterface_.object() == nullptr);
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

	if (fileSinkInterface_.object() != nullptr)
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

		if (S_OK != fileSinkInterface_.object()->SetFileName(String::toWString(filename).c_str(), nullptr))
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
	ocean_assert(filterGraph_.object() != nullptr);

	if (fileWriterFilter_.object() != nullptr)
	{
		return true;
	}

	ocean_assert(fileSinkInterface_.object() == nullptr);

	ScopedFunctionVoid scopedReleaseFileWriterFilterFunction(std::bind(&DSFileRecorder::releaseFileWriterFilter, this)); // scoped function which will be invoked in case we don't reach the both of this function

	if (S_OK != CoCreateInstance(CLSID_AviDest, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)(&aviMuxFilter_.resetObject())))
	{
		Log::error() << "Could not create an AVI MUX filter for \"" << recorderFilename << "\".";

		return false;
	}

	if (S_OK != filterGraph_.object()->AddFilter(aviMuxFilter_.object(), L"AVI MUX filter"))
	{
		Log::error() << "Could not add the AVI MUX filter to the filter graph.";

		return false;
	}

	const ScopedIPin aviMuxFilterInputPin = firstPin(aviMuxFilter_.object(), PTYPE_INPUT, CTYPE_NOT_CONNECTED);
	if (aviMuxFilterInputPin.object() == nullptr)
	{
		Log::error() << "Could not find a free input pin of the just created AVI MUX filter.";

		return false;
	}

	const ScopedIPin aviMuxFilterOutputPin = firstPin(aviMuxFilter_.object(), PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);
	if (aviMuxFilterOutputPin.object() == nullptr)
	{
		Log::error() << "Could not find a free output pin of the just created AVI MUX filter.";

		return false;
	}

	if (S_OK != CoCreateInstance(CLSID_FileWriter, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)(&fileWriterFilter_.resetObject())))
	{
		Log::error() << "Could not create a file writer filter for \"" << recorderFilename << "\".";

		return false;
	}

	if (S_OK != filterGraph_.object()->AddFilter(fileWriterFilter_.object(), String::toWString(recorderFilename).c_str()))
	{
		Log::error() << "Could not add the file writer filter to the filter graph.";

		return false;
	}

	const ScopedIPin fileWriterFilterInputPin = firstPin(fileWriterFilter_.object(), PTYPE_INPUT, CTYPE_NOT_CONNECTED);
	if (fileWriterFilterInputPin.object() == nullptr)
	{
		Log::error() << "Could not find a free input pin of the just created file writer filter.";

		return false;
	}

	if (S_OK != filterGraph_.object()->Connect(outputPin, aviMuxFilterInputPin.object()))
	{
		Log::error() << "Could not connect the given output pin with the AVI MUX input pin.";

		return false;
	}

	if (S_OK != filterGraph_.object()->Connect(aviMuxFilterOutputPin.object(), fileWriterFilterInputPin.object()))
	{
		Log::error() << "Could not connect the AVI MUX output pin with the file writer input pin.";

		return false;
	}

	if (S_OK != fileWriterFilter_.object()->QueryInterface(IID_IFileSinkFilter, (void**)(&fileSinkInterface_.resetObject())))
	{
		Log::error() << "Could not receive a file sink interface from the file writer filter.";

		return false;
	}

	if (S_OK != fileSinkInterface_.object()->SetFileName(String::toWString(recorderFilename).c_str(), nullptr))
	{
		Log::error() << "Could not set the filename \"" << recorderFilename << "\" of the file writer filter.";

		return false;
	}

	scopedReleaseFileWriterFilterFunction.revoke(); // we don't call the releaseFileWriterFilter() function

	return true;
}

void DSFileRecorder::releaseFileWriterFilter()
{
	if (aviMuxFilter_.object() != nullptr)
	{
		ocean_assert(filterGraph_.object() != nullptr);

		filterGraph_.object()->RemoveFilter(aviMuxFilter_.object());
	}

	if (fileWriterFilter_.object() != nullptr)
	{
		ocean_assert(filterGraph_.object() != nullptr);

		filterGraph_.object()->RemoveFilter(fileWriterFilter_.object());
	}

	fileSinkInterface_.release();
	aviMuxFilter_.release();
	fileWriterFilter_.release();
}

}

}

}
