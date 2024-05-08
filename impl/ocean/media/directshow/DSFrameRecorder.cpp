/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSFrameRecorder.h"
#include "ocean/media/directshow/DSDeviceEnumerator.h"

#include "ocean/base/ScopedFunction.h"
#include "ocean/base/String.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSFrameRecorder::~DSFrameRecorder()
{
	ocean_assert(frameEncoderFilter_.object() == nullptr);
	ocean_assert(sampleSourceFilter_.object() == nullptr);
}

DSFrameRecorder::Encoders DSFrameRecorder::frameEncoders() const
{
	return DSEnumerators::get().enumerator(CLSID_VideoCompressorCategory).names();
}

bool DSFrameRecorder::frameEncoderHasConfiguration()
{
	if (recorderFrameEncoder.empty())
	{
		return false;
	}

	if (!buildGraph())
	{
		return false;
	}

	ocean_assert(frameEncoderFilter_.object() != nullptr);

	ScopedIAMVfwCompressDialogs dialogs;
	if (S_OK == frameEncoderFilter_.object()->QueryInterface(IID_IAMVfwCompressDialogs, (void**)(&dialogs.resetObject())))
	{
		if (S_OK == dialogs.object()->ShowDialog(VfwCompressDialog_QueryConfig, nullptr))
		{
			return true;
		}
	}

	return false;
}

bool DSFrameRecorder::setPreferredFrameType(const FrameType& type)
{
	if (!type.isValid())
	{
		return false;
	}

	const unsigned int width4 = (type.width() / 4u) * 4u;
	const unsigned int height4 = (type.height() / 4u) * 4u;

	if (width4 == 0u || height4 == 0u)
	{
		return false;
	}

	FrameType::PixelFormat pixelFormat = type.pixelFormat();

	if (type.pixelFormat() != FrameType::FORMAT_BGR24 && type.pixelFormat() != FrameType::FORMAT_BGR32)
	{
		pixelFormat = FrameType::FORMAT_BGR24;
	}

	FrameType newFrameType(width4, height4, pixelFormat, type.pixelOrigin());

	const ScopedLock scopedLock(recorderLock);

	if (newFrameType == recorderFrameType)
	{
		return true;
	}

	if (graphIsRunning_)
	{
		return false;
	}

	releaseGraph();

	recorderFrameType = newFrameType;

	if (recorderFrameEncoder.empty())
	{
		return true;
	}

	return buildGraph();
}

bool DSFrameRecorder::setFrameFrequency(const double frequency)
{
	if (recorderFrameFrequency == frequency)
	{
		return true;
	}

	if (frequency < 0)
	{
		return false;
	}

	if (graphIsRunning_)
	{
		return false;
	}

	releaseGraph();

	if (!FrameRecorder::setFrameFrequency(frequency))
	{
		return false;
	}

	return buildGraph();
}

bool DSFrameRecorder::setFrameEncoder(const std::string& encoder)
{
	if (recorderFrameEncoder == encoder)
	{
		return true;
	}

	if (graphIsRunning_)
	{
		return false;
	}

	releaseGraph();

	recorderFrameEncoder = encoder;

	if (recorderFrameEncoder.empty())
	{
		return true;
	}

	return buildGraph();
}

bool DSFrameRecorder::frameEncoderConfiguration(long long data)
{
	if (recorderFrameEncoder.empty())
	{
		return false;
	}

	if (!buildGraph())
	{
		return false;
	}

	ocean_assert(frameEncoderFilter_.object() != nullptr);

	ScopedIAMVfwCompressDialogs dialogs;
	if (S_OK == frameEncoderFilter_.object()->QueryInterface(IID_IAMVfwCompressDialogs, (void**)(&dialogs.resetObject())))
	{
		if (S_OK == dialogs.object()->ShowDialog(VfwCompressDialog_Config, HWND(data)))
		{
			return true;
		}
	}

	return false;
}

bool DSFrameRecorder::lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency)
{
	if (sampleSourceFilter_.object() != nullptr)
	{
		void* buffer = nullptr;
		size_t bufferSize = 0u;

		if (sampleSourceFilter_.object()->lockBufferToFill(buffer, bufferSize, respectFrameFrequency))
		{
			const Frame::PlaneInitializers<void> planeInitializer =
			{
				Frame::PlaneInitializer<void>(buffer, Frame::CM_USE_KEEP_LAYOUT, 0u)
			};

			recorderFrame = Frame(frameType(), planeInitializer);
		}
	}

	return false;
}

void DSFrameRecorder::unlockBufferToFill()
{
	if (sampleSourceFilter_.object() != nullptr)
	{
		sampleSourceFilter_.object()->unlockBufferToFill();
	}
}

bool DSFrameRecorder::insertSourceFilter(ScopedIPin& sourceOutputPin)
{
	ocean_assert(filterGraph_.object() != nullptr);

	sourceOutputPin.release();
	if (sampleSourceFilter_.object() != nullptr)
	{
		sourceOutputPin = firstPin(sampleSourceFilter_.object(), PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);

		if (sourceOutputPin.object() == nullptr)
		{
			Log::error() << "Could not get a free output pin from the frame source filter.";

			return false;
		}

		return true;
	}

	sampleSourceFilter_ = ScopedDSSampleSourceFilter(new DSSampleSourceFilter(nullptr));

	if (sampleSourceFilter_.object() != nullptr)
	{
		sampleSourceFilter_.object()->AddRef();
	}
	else
	{
		Log::error() << "Could not create the sample source filter.";

		return false;
	}

	ScopedFunctionVoid scopedReleaseFrameSourceFilterFunction(std::bind(&DSFrameRecorder::releaseFrameSourceFilter, this)); // scoped function which will be invoked in case we don't reach the both of this function

	if (!sampleSourceFilter_.object()->setFormat(recorderFrameType, recorderFrameFrequency))
	{
		Log::error() << "Could not set the defined frame type.";

		return false;
	}

	if (S_OK != filterGraph_.object()->AddFilter(sampleSourceFilter_.object(), L"Ocean frame source filter"))
	{
		Log::error() << "Could not add the source filter the to the filter graph.";

		return false;
	}

	sourceOutputPin = firstPin(sampleSourceFilter_.object(), PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);
	if (sourceOutputPin.object() == nullptr)
	{
		Log::error() << "Could not get a free output pin from the frame source filter.";

		return false;
	}

	scopedReleaseFrameSourceFilterFunction.revoke(); // we don't call the releaseFrameSourceFilter() function

	return true;
}

bool DSFrameRecorder::insertFrameEncoderFilter(IPin* outputPin, ScopedIPin& encoderOutputPin)
{
	ocean_assert(outputPin != nullptr);
	ocean_assert(filterGraph_.object() != nullptr);

	encoderOutputPin.release();

	if (frameEncoderFilter_.object() != nullptr)
	{
		encoderOutputPin = firstPin(frameEncoderFilter_.object(), PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);

		if (encoderOutputPin.object() == nullptr)
		{
			Log::error() << "Could not get a free output pin from the frame encoder filter.";

			return false;
		}

		return true;
	}

	if (recorderFrameEncoder.empty())
	{
		return false;
	}

	ScopedIMoniker encoderMoniker = DSEnumerators::get().enumerator(CLSID_VideoCompressorCategory).moniker(recorderFrameEncoder);
	if (encoderMoniker.object() == nullptr)
	{
		Log::error() << "A frame encoder with name \"" << recorderFrameEncoder << "\" does not exist.";

		return false;
	}

	if (S_OK != encoderMoniker.object()->BindToObject(nullptr, nullptr, IID_IBaseFilter, (void**)(&frameEncoderFilter_.resetObject())))
	{
		Log::error() << "Could not create the frame encoder filter \"" << recorderFrameEncoder << "\".";

		return false;
	}

	ScopedFunctionVoid scopedReleaseFrameEncoderFilterFunction(std::bind(&DSFrameRecorder::releaseFrameEncoderFilter, this)); // scoped function which will be invoked in case we don't reach the both of this function

	if (S_OK != filterGraph_.object()->AddFilter(frameEncoderFilter_.object(), String::toWString(recorderFrameEncoder).c_str()))
	{
		Log::error() << "Could not add the encoder filter to the filter graph.";

		return false;
	}

	const ScopedIPin frameEncoderInputPin = firstPin(frameEncoderFilter_.object(), PTYPE_INPUT, CTYPE_NOT_CONNECTED);
	if (frameEncoderInputPin.object() == nullptr)
	{
		Log::error() << "Could not get a free input pin from the frame encoder filter.";

		return false;
	}

	if (S_OK != filterGraph_.object()->Connect(outputPin, frameEncoderInputPin.object()))
	{
		Log::error() << "Could not connect the given output pin with the frame encoder input pin.";

		return false;
	}

	encoderOutputPin = firstPin(frameEncoderFilter_.object(), PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);
	if (encoderOutputPin.object() == nullptr)
	{
		Log::error() << "Could not get a free output pin from the frame encoder filter.";

		return false;
	}

	scopedReleaseFrameEncoderFilterFunction.revoke(); // we don't call the releaseFrameEncoderFilter() function

	return true;
}

void DSFrameRecorder::releaseFrameSourceFilter()
{
	if (sampleSourceFilter_.object() != nullptr)
	{
		ocean_assert(filterGraph_.object() != nullptr);

		filterGraph_.object()->RemoveFilter(sampleSourceFilter_.object());
	}

	sampleSourceFilter_.release();
}

void DSFrameRecorder::releaseFrameEncoderFilter()
{
	if (frameEncoderFilter_.object() != nullptr)
	{
		ocean_assert(filterGraph_.object() != nullptr);

		filterGraph_.object()->RemoveFilter(frameEncoderFilter_.object());
	}

	frameEncoderFilter_.release();
}

}

}

}
