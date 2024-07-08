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
	ocean_assert(!frameEncoderFilter_.isValid());
	ocean_assert(!sampleSourceFilter_.isValid());
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

	ocean_assert(frameEncoderFilter_.isValid());

	ScopedIAMVfwCompressDialogs dialogs;
	if (S_OK == frameEncoderFilter_->QueryInterface(IID_IAMVfwCompressDialogs, (void**)(&dialogs.resetObject())))
	{
		if (S_OK == dialogs->ShowDialog(VfwCompressDialog_QueryConfig, nullptr))
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

	ocean_assert(frameEncoderFilter_.isValid());

	ScopedIAMVfwCompressDialogs dialogs;
	if (S_OK == frameEncoderFilter_->QueryInterface(IID_IAMVfwCompressDialogs, (void**)(&dialogs.resetObject())))
	{
		if (S_OK == dialogs->ShowDialog(VfwCompressDialog_Config, HWND(data)))
		{
			return true;
		}
	}

	return false;
}

bool DSFrameRecorder::lockBufferToFill(Frame& recorderFrame, const bool respectFrameFrequency)
{
	if (sampleSourceFilter_.isValid())
	{
		void* buffer = nullptr;
		size_t bufferSize = 0u;

		if (sampleSourceFilter_->lockBufferToFill(buffer, bufferSize, respectFrameFrequency))
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
	if (sampleSourceFilter_.isValid())
	{
		sampleSourceFilter_->unlockBufferToFill();
	}
}

bool DSFrameRecorder::insertSourceFilter(ScopedIPin& sourceOutputPin)
{
	ocean_assert(filterGraph_.isValid());

	sourceOutputPin.release();
	if (sampleSourceFilter_.isValid())
	{
		sourceOutputPin = firstPin(*sampleSourceFilter_, PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);

		if (!sourceOutputPin.isValid())
		{
			Log::error() << "Could not get a free output pin from the frame source filter.";

			return false;
		}

		return true;
	}

	sampleSourceFilter_ = ScopedDSSampleSourceFilter(new DSSampleSourceFilter(nullptr));

	if (sampleSourceFilter_.isValid())
	{
		sampleSourceFilter_->AddRef();
	}
	else
	{
		Log::error() << "Could not create the sample source filter.";

		return false;
	}

	ScopedFunctionVoid scopedReleaseFrameSourceFilterFunction(std::bind(&DSFrameRecorder::releaseFrameSourceFilter, this)); // scoped function which will be invoked in case we don't reach the both of this function

	if (!sampleSourceFilter_->setFormat(recorderFrameType, recorderFrameFrequency))
	{
		Log::error() << "Could not set the defined frame type.";

		return false;
	}

	if (S_OK != filterGraph_->AddFilter(*sampleSourceFilter_, L"Ocean frame source filter"))
	{
		Log::error() << "Could not add the source filter the to the filter graph.";

		return false;
	}

	sourceOutputPin = firstPin(*sampleSourceFilter_, PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);
	if (!sourceOutputPin.isValid())
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
	ocean_assert(filterGraph_.isValid());

	encoderOutputPin.release();

	if (frameEncoderFilter_.isValid())
	{
		encoderOutputPin = firstPin(*frameEncoderFilter_, PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);

		if (!encoderOutputPin.isValid())
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
	if (!encoderMoniker.isValid())
	{
		Log::error() << "A frame encoder with name \"" << recorderFrameEncoder << "\" does not exist.";

		return false;
	}

	if (S_OK != encoderMoniker->BindToObject(nullptr, nullptr, IID_IBaseFilter, (void**)(&frameEncoderFilter_.resetObject())))
	{
		Log::error() << "Could not create the frame encoder filter \"" << recorderFrameEncoder << "\".";

		return false;
	}

	ScopedFunctionVoid scopedReleaseFrameEncoderFilterFunction(std::bind(&DSFrameRecorder::releaseFrameEncoderFilter, this)); // scoped function which will be invoked in case we don't reach the both of this function

	if (S_OK != filterGraph_->AddFilter(*frameEncoderFilter_, String::toWString(recorderFrameEncoder).c_str()))
	{
		Log::error() << "Could not add the encoder filter to the filter graph.";

		return false;
	}

	const ScopedIPin frameEncoderInputPin = firstPin(*frameEncoderFilter_, PTYPE_INPUT, CTYPE_NOT_CONNECTED);
	if (!frameEncoderInputPin.isValid())
	{
		Log::error() << "Could not get a free input pin from the frame encoder filter.";

		return false;
	}

	if (S_OK != filterGraph_->Connect(outputPin, *frameEncoderInputPin))
	{
		Log::error() << "Could not connect the given output pin with the frame encoder input pin.";

		return false;
	}

	encoderOutputPin = firstPin(*frameEncoderFilter_, PTYPE_OUTPUT, CTYPE_NOT_CONNECTED);
	if (!encoderOutputPin.isValid())
	{
		Log::error() << "Could not get a free output pin from the frame encoder filter.";

		return false;
	}

	scopedReleaseFrameEncoderFilterFunction.revoke(); // we don't call the releaseFrameEncoderFilter() function

	return true;
}

void DSFrameRecorder::releaseFrameSourceFilter()
{
	if (sampleSourceFilter_.isValid())
	{
		ocean_assert(filterGraph_.isValid());

		filterGraph_->RemoveFilter(*sampleSourceFilter_);
	}

	sampleSourceFilter_.release();
}

void DSFrameRecorder::releaseFrameEncoderFilter()
{
	if (frameEncoderFilter_.isValid())
	{
		ocean_assert(filterGraph_.isValid());

		filterGraph_->RemoveFilter(*frameEncoderFilter_);
	}

	frameEncoderFilter_.release();
}

}

}

}
