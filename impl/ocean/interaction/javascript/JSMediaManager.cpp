/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/javascript/JSMediaManager.h"
#include "ocean/interaction/javascript/JSMediaObject.h"

#include "ocean/base/String.h"

#include "ocean/media/Manager.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

v8::Persistent<v8::ObjectTemplate> JSMediaManager::objectTemplate_;

void JSMediaManager::createObjectTemplate()
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();

	v8::Local<v8::ObjectTemplate> objectTemplate(v8::ObjectTemplate::New(isolate));

	objectTemplate->Set(newString("create", isolate), v8::FunctionTemplate::New(isolate, functionCreate));
	objectTemplate->Set(newString("createAudio", isolate), v8::FunctionTemplate::New(isolate, functionCreateAudio));
	objectTemplate->Set(newString("createImage", isolate), v8::FunctionTemplate::New(isolate, functionCreateImage));
	objectTemplate->Set(newString("createLiveVideo", isolate), v8::FunctionTemplate::New(isolate, functionCreateLiveVideo));
	objectTemplate->Set(newString("createMovie", isolate), v8::FunctionTemplate::New(isolate, functionCreateMovie));

	objectTemplate_.Reset(isolate, objectTemplate);
}

void JSMediaManager::functionCreate(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	createMediaObject(Media::Medium::MEDIUM, info);
}

void JSMediaManager::functionCreateAudio(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	createMediaObject(Media::Medium::AUDIO, info);
}

void JSMediaManager::functionCreateImage(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	createMediaObject(Media::Medium::IMAGE, info);
}

void JSMediaManager::functionCreateLiveVideo(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	createMediaObject(Media::Medium::LIVE_VIDEO, info);
}

void JSMediaManager::functionCreateMovie(const v8::FunctionCallbackInfo<v8::Value>& info)
{
	createMediaObject(Media::Medium::MOVIE, info);
}

void JSMediaManager::createMediaObject(const Media::Medium::Type type, const v8::FunctionCallbackInfo<v8::Value>& info)
{
	std::string url;
	if (hasValue(info, 0u, url))
	{
		bool exclusive = false;
		hasValue(info, 1u, exclusive);

		if ((type & Media::Medium::LIVE_MEDIUM) == Media::Medium::LIVE_MEDIUM)
		{
			info.GetReturnValue().Set(createObject<JSMediaObject>(Media::Manager::get().newMedium(url, type, exclusive), JSContext::currentContext()));
			return;
		}
		else
		{
			const std::shared_ptr<JSContext> context(JSContext::currentJSContext());
			ocean_assert(context);

			if (context)
			{
				for (const IO::File& file : context->resolveFile(IO::File(url)))
				{
					const Media::MediumRef medium(Media::Manager::get().newMedium(file(), type, exclusive));

					if (medium)
					{
						info.GetReturnValue().Set(createObject<JSMediaObject>(medium, JSContext::currentContext()));
						return;
					}
				}
			}
		}
	}

	int liveVideoId;
	if (hasValue(info, 0u, liveVideoId))
	{
		bool exclusive = false;
		hasValue(info, 1u, exclusive);

		if ((type & Media::Medium::LIVE_MEDIUM) == Media::Medium::LIVE_MEDIUM)
		{
			const std::string videoId = std::string("LiveVideoId:") + Ocean::String::toAString(liveVideoId);
			info.GetReturnValue().Set(createObject<JSMediaObject>(Media::Manager::get().newMedium(videoId, type, exclusive), JSContext::currentContext()));
			return;
		}
	}

	if (info.Length() == 0)
	{
		if ((type & Media::Medium::LIVE_MEDIUM) == Media::Medium::LIVE_MEDIUM)
		{
			info.GetReturnValue().Set(createObject<JSMediaObject>(Media::Manager::get().newMedium("LiveVideoId:0", type, false), JSContext::currentContext()));
			return;
		}
	}

	Log::warning() << "MediaManager::create() failed";

	info.GetReturnValue().Set(createObject<JSMediaObject>(Media::MediumRef(), JSContext::currentContext()));
}

}

}

}
