/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSMediaObject.h"

#include "ocean/devices/Tracker6DOF.h"

#include "ocean/media/Audio.h"
#include "ocean/media/Image.h"
#include "ocean/media/LiveVideo.h"
#include "ocean/media/Movie.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

void JSMediaObject::createFunctionTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::FunctionTemplate> functionTemplate(v8::FunctionTemplate::New(isolate, constructor<Media::MediumRef>));
	functionTemplate->SetClassName(newString(objectName(), isolate));

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::Local<v8::ObjectTemplate>::New(isolate, functionTemplate->InstanceTemplate()));
	objectTemplate->SetInternalFieldCount(1);

	objectTemplate->SetAccessor(newString("url", isolate), propertyGetter<NativeType, AI_URL>);

	objectTemplate->Set(newString("duration", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_DURATION>));
	objectTemplate->Set(newString("loop", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_LOOP>));
	objectTemplate->Set(newString("frameHeight", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FRAME_HEIGHT>));
	objectTemplate->Set(newString("frameWidth", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FRAME_WIDTH>));
	objectTemplate->Set(newString("framePixelFormat", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FRAME_PIXEL_FORMAT>));
	objectTemplate->Set(newString("framePixelOrigin", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FRAME_PIXEL_ORIGIN>));
	objectTemplate->Set(newString("frameFrequency", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FRAME_FREQUENCY>));
	objectTemplate->Set(newString("frameTimestamp", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_FRAME_TIMESTAMP>));
	objectTemplate->Set(newString("hasFrame", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_HAS_FRAME>));
	objectTemplate->Set(newString("preferredFrameHeight", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PREFERRED_FRAME_HEIGHT>));
	objectTemplate->Set(newString("preferredFrameWidth", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PREFERRED_FRAME_WIDTH>));
	objectTemplate->Set(newString("preferredFrameFrequency", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PREFERRED_FRAME_FREQUENCY>));
	objectTemplate->Set(newString("preferredFramePixelFormat", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PREFERRED_FRAME_PIXEL_FORMAT>));
	objectTemplate->Set(newString("normalDuration", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_NORMAL_DURATION>));
	objectTemplate->Set(newString("position", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_POSITION>));
	objectTemplate->Set(newString("speed", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SPEED>));

	objectTemplate->Set(newString("hasSound", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_HAS_SOUND>));
	objectTemplate->Set(newString("soundChannels", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SOUND_CHANNELS>));
	objectTemplate->Set(newString("soundFrequency", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SOUND_FREQUENCY>));
	objectTemplate->Set(newString("soundBitsPerSample", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SOUND_BITS_PER_SAMPLE>));
	objectTemplate->Set(newString("soundVolume", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SOUND_VOLUME>));
	objectTemplate->Set(newString("soundMute", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SOUND_MUTE>));
	objectTemplate->Set(newString("preferredSoundChannels", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PREFERRED_SOUND_CHANNELS>));
	objectTemplate->Set(newString("preferredSoundFrequency", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PREFERRED_SOUND_FREQUENCY>));
	objectTemplate->Set(newString("preferredSoundBitsPerSample", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PREFERRED_SOUND_BITS_PER_SAMPLE>));


	objectTemplate->Set(newString("setLoop", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_LOOP>));
	objectTemplate->Set(newString("setPosition", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_POSITION>));
	objectTemplate->Set(newString("setSpeed", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_SPEED>));
	objectTemplate->Set(newString("setPreferredFrameDimension", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_PREFERRED_FRAME_DIMENSION>));
	objectTemplate->Set(newString("setPreferredFrameFrequency", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_PREFERRED_FRAME_FREQUENCY>));
	objectTemplate->Set(newString("setPreferredFramePixelFormat", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_PREFERRED_FRAME_PIXEL_FORMAT>));
	objectTemplate->Set(newString("setPreferredSoundBitsPerSample", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_PREFERRED_SOUND_BITS_PER_SAMPLE>));
	objectTemplate->Set(newString("setPreferredSoundChannels", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_PREFERRED_SOUND_CHANNELS>));
	objectTemplate->Set(newString("setPreferredSoundFrequency", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_PREFERRED_SOUND_FREQUENCY>));

	objectTemplate->Set(newString("setSoundVolume", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_SOUND_VOLUME>));
	objectTemplate->Set(newString("setSoundMute", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_SET_SOUND_MUTE>));

	objectTemplate->Set(newString("isValid", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_VALID>));
	objectTemplate->Set(newString("isExclusive", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_EXCLUSIVE>));
	objectTemplate->Set(newString("isInvalid", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_INVALID>));

	objectTemplate->Set(newString("start", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_START>));
	objectTemplate->Set(newString("restart", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_RESTART>));
	objectTemplate->Set(newString("pause", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_PAUSE>));
	objectTemplate->Set(newString("stop", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_STOP>));

	objectTemplate->Set(newString("isStarted", isolate), v8::FunctionTemplate::New(isolate, function<NativeType, FI_IS_STARTED>));

	functionTemplate_.Reset(isolate, functionTemplate);
	objectTemplate_.Reset(isolate, objectTemplate);
}

template <>
void JSBase::propertyGetter<Media::MediumRef, JSMediaObject::AI_URL>(Media::MediumRef& thisValue, v8::Local<v8::String>& /*property*/, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	if (thisValue)
	{
		info.GetReturnValue().Set(newString(thisValue->url(), v8::Isolate::GetCurrent()));
	}
	else
	{
		Log::warning() << "The MediumObject is invalid.";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_DURATION>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FiniteMediumRef finiteMedium(thisValue);

	if (finiteMedium)
	{
		info.GetReturnValue().Set(double(finiteMedium->duration()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'duration()'";
	}
}

template <>
void JSBase::constructor<Media::MediumRef>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	if (info.Length() == 0)
	{
		return;
	}

	if (hasValue(info, 0u, thisValue))
	{
		return;
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_LOOP>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FiniteMediumRef finiteMedium(thisValue);

	if (finiteMedium)
	{
		info.GetReturnValue().Set(finiteMedium->loop());
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'loop()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_FRAME_HEIGHT>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FrameMediumRef frameMedium(thisValue);

	if (frameMedium)
	{
		const FrameRef frame = frameMedium->frame();

		info.GetReturnValue().Set(frame ? int(frame->height()) : 0);
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'frameHeight()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_FRAME_WIDTH>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FrameMediumRef frameMedium(thisValue);

	if (frameMedium)
	{
		const FrameRef frame = frameMedium->frame();

		info.GetReturnValue().Set(frame ? int(frame->width()) : 0);
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'frameWidth()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_FRAME_PIXEL_FORMAT>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FrameMediumRef frameMedium(thisValue);

	if (frameMedium)
	{
		const FrameRef frame = frameMedium->frame();

		const Frame::PixelFormat pixelFormat = frame ? frame->pixelFormat() : FrameType::FORMAT_UNDEFINED;

		info.GetReturnValue().Set(newString(Frame::translatePixelFormat(pixelFormat), v8::Isolate::GetCurrent()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'framePixelFormat()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_FRAME_PIXEL_ORIGIN>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FrameMediumRef frameMedium(thisValue);

	if (frameMedium)
	{
		const FrameRef frame = frameMedium->frame();

		const Frame::PixelOrigin pixelOrigin = frame ? frame->pixelOrigin() : FrameType::ORIGIN_INVALID;

		info.GetReturnValue().Set(newString(Frame::translatePixelOrigin(pixelOrigin), v8::Isolate::GetCurrent()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'framePixelOrigin()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_FRAME_FREQUENCY>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FrameMediumRef frameMedium(thisValue);

	if (frameMedium)
	{
		info.GetReturnValue().Set(double(frameMedium->frameFrequency()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'frameFrequency()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_FRAME_TIMESTAMP>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FrameMediumRef frameMedium(thisValue);

	if (frameMedium)
	{
		const FrameRef frame = frameMedium->frame();

		info.GetReturnValue().Set(frame ? double(frame->timestamp()) : -1.0);
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'frameTimestamp()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_HAS_FRAME>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FrameMediumRef frameMedium(thisValue);

	info.GetReturnValue().Set(frameMedium && frameMedium->frame() && !frameMedium->frame().isNull());
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_PREFERRED_FRAME_HEIGHT>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FrameMediumRef frameMedium(thisValue);

	if (frameMedium)
	{
		info.GetReturnValue().Set(int(frameMedium->preferredFrameHeight()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'preferredFrameHeight()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_PREFERRED_FRAME_WIDTH>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FrameMediumRef frameMedium(thisValue);

	if (frameMedium)
	{
		info.GetReturnValue().Set(int(frameMedium->preferredFrameWidth()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'preferredFrameWidth()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_PREFERRED_FRAME_FREQUENCY>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FrameMediumRef frameMedium(thisValue);

	if (frameMedium)
	{
		info.GetReturnValue().Set(double(frameMedium->preferredFrameFrequency()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'preferredFrameFrequency()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_PREFERRED_FRAME_PIXEL_FORMAT>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FrameMediumRef frameMedium(thisValue);

	if (frameMedium)
	{
		info.GetReturnValue().Set(newString(Frame::translatePixelFormat(frameMedium->preferredFramePixelFormat()), v8::Isolate::GetCurrent()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'preferredFrameFrequency()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_NORMAL_DURATION>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FiniteMediumRef finiteMedium(thisValue);

	if (finiteMedium)
	{
		info.GetReturnValue().Set(double(finiteMedium->normalDuration()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'normalDuration()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_POSITION>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FiniteMediumRef finiteMedium(thisValue);

	if (finiteMedium)
	{
		info.GetReturnValue().Set(double(finiteMedium->position()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'position()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SPEED>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::FiniteMediumRef finiteMedium(thisValue);

	if (finiteMedium)
	{
		info.GetReturnValue().Set(double(finiteMedium->speed()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'speed()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_HAS_SOUND>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::SoundMediumRef soundMedium(thisValue);

	if (soundMedium)
	{
		info.GetReturnValue().Set(soundMedium->hasSound());
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'hasSound()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SOUND_CHANNELS>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::SoundMediumRef soundMedium(thisValue);

	if (soundMedium)
	{
		info.GetReturnValue().Set(int(soundMedium->soundChannels()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'soundChannels()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SOUND_FREQUENCY>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::SoundMediumRef soundMedium(thisValue);

	if (soundMedium)
	{
		info.GetReturnValue().Set(double(soundMedium->soundFrequency()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'soundFrequency()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SOUND_BITS_PER_SAMPLE>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::SoundMediumRef soundMedium(thisValue);

	if (soundMedium)
	{
		info.GetReturnValue().Set(int(soundMedium->soundBitsPerSample()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'soundBitsPerSample()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SOUND_VOLUME>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::SoundMediumRef soundMedium(thisValue);

	if (soundMedium)
	{
		info.GetReturnValue().Set(double(soundMedium->soundVolume()));
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'soundVolume()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SOUND_MUTE>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::SoundMediumRef soundMedium(thisValue);

	if (soundMedium)
	{
		info.GetReturnValue().Set(soundMedium->soundMute());
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'soundMute()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_PREFERRED_SOUND_CHANNELS>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::SoundMediumRef soundMedium(thisValue);

	if (soundMedium)
	{
		info.GetReturnValue().Set(soundMedium->preferredSoundChannels());
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'preferredSoundChannels()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_PREFERRED_SOUND_FREQUENCY>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::SoundMediumRef soundMedium(thisValue);

	if (soundMedium)
	{
		info.GetReturnValue().Set(soundMedium->preferredSoundFrequency());
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'preferredSoundFrequency()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_PREFERRED_SOUND_BITS_PER_SAMPLE>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	const Media::SoundMediumRef soundMedium(thisValue);

	if (soundMedium)
	{
		info.GetReturnValue().Set(soundMedium->soundBitsPerSample());
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'soundBitsPerSample()'";
	}
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SET_LOOP>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	bool result = false;

	const Media::FiniteMediumRef finiteMedium(thisValue);

	if (finiteMedium)
	{
		bool loopValue;
		if (hasValue<bool>(info, 0u, loopValue))
		{
			result = finiteMedium->setLoop(loopValue);
		}
		else
		{
			Log::warning() << "The MediumObject::setLoop() needs Boolean value as parameter";
		}
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'setLoop()'";
	}

	info.GetReturnValue().Set(result);
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SET_POSITION>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	bool result = false;

	const Media::FiniteMediumRef finiteMedium(thisValue);

	if (finiteMedium)
	{
		double positionValue;
		if (hasValue<double>(info, 0u, positionValue) && positionValue >= 0.0)
		{
			result = finiteMedium->setPosition(positionValue);
		}
		else
		{
			Log::warning() << "The MediumObject::setPosition() needs a positive Number value as parameter";
		}
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'setLoop()'";
	}

	info.GetReturnValue().Set(result);
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SET_SPEED>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	bool result = false;

	const Media::FiniteMediumRef finiteMedium(thisValue);

	if (finiteMedium)
	{
		float speedValue;
		if (hasValue<float>(info, 0u, speedValue) && speedValue > 0.0f)
		{
			result = finiteMedium->setSpeed(speedValue);
		}
		else
		{
			Log::warning() << "The MediumObject::setSpeed() needs positive Number value as parameter";
		}
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'setSpeed()'";
	}

	info.GetReturnValue().Set(result);
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SET_PREFERRED_FRAME_DIMENSION>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	bool result = false;

	const Media::FrameMediumRef frameMedium(thisValue);

	if (frameMedium)
	{
		int widthValue;
		int heightValue;

		if (hasValue<int>(info, 0u, widthValue) && hasValue<int>(info, 1u, heightValue) && widthValue > 0 && heightValue > 0)
		{
			result = frameMedium->setPreferredFrameDimension((unsigned int)(widthValue), (unsigned int)(heightValue));
		}
		else
		{
			Log::warning() << "The MediumObject::setPreferredFrameDimension() two positive Integer values as parameter";
		}
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'setPreferredFrameDimension()'";
	}

	info.GetReturnValue().Set(result);
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SET_PREFERRED_FRAME_FREQUENCY>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	bool result = false;

	const Media::FrameMediumRef frameMedium(thisValue);

	if (frameMedium)
	{
		double value;
		if (hasValue<double>(info, 0u, value) && value > 0.0)
		{
			result = frameMedium->setPreferredFrameFrequency(value);
		}
		else
		{
			Log::warning() << "The MediumObject::setPreferredFrameFrequency() a positive Number value as parameter";
		}
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'setPreferredFrameFrequency()'";
	}

	info.GetReturnValue().Set(result);
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SET_PREFERRED_FRAME_PIXEL_FORMAT>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	bool result = false;

	const Media::FrameMediumRef frameMedium(thisValue);

	if (frameMedium)
	{
		std::string value;
		if (hasValue<std::string>(info, 0u, value))
		{
			result = frameMedium->setPreferredFramePixelFormat(FrameType::translatePixelFormat(value));
		}
		else
		{
			Log::warning() << "The MediumObject::setPreferredFramePixelFormat() a positive Number value as parameter";
		}
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'setPreferredFramePixelFormat()'";
	}

	info.GetReturnValue().Set(result);
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SET_PREFERRED_SOUND_BITS_PER_SAMPLE>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	bool result = false;

	const Media::SoundMediumRef soundMedium(thisValue);

	if (soundMedium)
	{
		int value;
		if (hasValue<int>(info, 0u, value) && value > 0)
		{
			result = soundMedium->setPreferredSoundBitsPerSample((unsigned int)(value));
		}
		else
		{
			Log::warning() << "The MediumObject::setPreferredSoundBitsPerSample() a positive Integer value as parameter";
		}
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'setPreferredSoundBitsPerSample()'";
	}

	info.GetReturnValue().Set(result);
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SET_PREFERRED_SOUND_CHANNELS>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	bool result = false;

	const Media::SoundMediumRef soundMedium(thisValue);

	if (soundMedium)
	{
		int value;
		if (hasValue<int>(info, 0u, value) && value > 0)
		{
			result = soundMedium->setPreferredSoundChannels((unsigned int)(value));
		}
		else
		{
			Log::warning() << "The MediumObject::setPreferredSoundChannels() a positive Integer value as parameter";
		}
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'setPreferredSoundChannels()'";
	}

	info.GetReturnValue().Set(result);
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SET_PREFERRED_SOUND_FREQUENCY>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	bool result = false;

	const Media::SoundMediumRef soundMedium(thisValue);

	if (soundMedium)
	{
		double value;
		if (hasValue<double>(info, 0u, value) && value > 0.0)
		{
			result = soundMedium->setPreferredSoundFrequency(Media::SoundMedium::SoundFrequency(value));
		}
		else
		{
			Log::warning() << "The MediumObject::setPreferredSoundFrequency() a positive Number value as parameter";
		}
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'setPreferredSoundFrequency()'";
	}

	info.GetReturnValue().Set(result);
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SET_SOUND_VOLUME>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	bool result = false;

	const Media::SoundMediumRef soundMedium(thisValue);

	if (soundMedium)
	{
		float value;
		if (hasValue<float>(info, 0u, value) && value > 0.0f)
		{
			result = soundMedium->setSoundVolume(value);
		}
		else
		{
			Log::warning() << "The MediumObject::setSoundVolume() a positive Number value as parameter";
		}
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'setSoundVolume()'";
	}

	info.GetReturnValue().Set(result);
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_SET_SOUND_MUTE>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	bool result = false;

	const Media::SoundMediumRef soundMedium(thisValue);

	if (soundMedium)
	{
		bool value;
		if (hasValue<bool>(info, 0u, value))
		{
			result = soundMedium->setSoundMute(value);
		}
		else
		{
			Log::warning() << "The MediumObject::setSoundMute() a Boolean value as parameter";
		}
	}
	else
	{
		Log::warning() << "The MediumObject does not support the function 'setSoundMute()'";
	}

	info.GetReturnValue().Set(result);
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_IS_VALID>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(!thisValue.isNull());
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_IS_EXCLUSIVE>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue && thisValue->isExclusive());
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_IS_INVALID>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue.isNull());
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_START>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue && thisValue->start());
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_RESTART>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue && thisValue->stop() && thisValue->start());
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_PAUSE>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue && thisValue->pause());
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_STOP>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue && thisValue->stop());
}

template <>
void JSBase::function<Media::MediumRef, JSMediaObject::FI_IS_STARTED>(Media::MediumRef& thisValue, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	info.GetReturnValue().Set(thisValue && thisValue->isStarted());
}

}

}

}
