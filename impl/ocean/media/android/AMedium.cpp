/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/AMedium.h"

#include "ocean/base/String.h"

#include <SLES/OpenSLES_Android.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

AMedium::EffectManager::EngineEffect::EngineEffect(const SLInterfaceID slEffectTypeId, const SLInterfaceID slEffectImplementationId) :
	slEffectTypeId_(slEffectTypeId),
	slEffectImplementationId_(slEffectImplementationId)
{
	ocean_assert(slEffectTypeId_ != nullptr);
	ocean_assert(slEffectImplementationId_ != nullptr);

#ifdef OCEAN_DEBUG
	if (areInterfaceIdsIdentical(slEffectTypeId, &slEffectTypeNoiseSuppressorId_))
	{
		Log::debug() << "Effect is a Noise Suppressor (NS)";
	}
	else if (areInterfaceIdsIdentical(slEffectTypeId, &slEffectTypeAcousticEchoCancelerId_))
	{
		Log::debug() << "Effect is an Acoustic Echo Canceler (AEC)";
	}
#endif
}

bool AMedium::EffectManager::effectIds(const std::string& effectName, SLInterfaceID& slEffectTypeId, SLInterfaceID& slEffectImplementationId) const
{
	ocean_assert(!effectName.empty());

	const ScopedLock scopedLock(lock_);

	EngineEffectMap::const_iterator iEffect = engineEffectMap_.find(effectName);

	if (iEffect == engineEffectMap_.cend())
	{
		return false;
	}

	slEffectTypeId = iEffect->second.slEffectTypeId_;
	slEffectImplementationId = iEffect->second.slEffectImplementationId_;

	return true;
}

void AMedium::EffectManager::initialize(const SLObjectItf& slEngine)
{
	ocean_assert(slEngine != nullptr);

	const ScopedLock scopedLock(lock_);

	if (!engineEffectMap_.empty())
	{
		ocean_assert(false && "Already initialized!");
		return;
	}

	SLAndroidEffectCapabilitiesItf slAndroidEffectCapabilities = nullptr;
	if ((*slEngine)->GetInterface(slEngine, SL_IID_ANDROIDEFFECTCAPABILITIES, &slAndroidEffectCapabilities) == SL_RESULT_SUCCESS)
	{
		SLuint32 numberSupportedEffects = 0u;
		if ((*slAndroidEffectCapabilities)->QueryNumEffects(slAndroidEffectCapabilities, &numberSupportedEffects) == SL_RESULT_SUCCESS)
		{
			Log::debug() << "OpenSL engine has " << numberSupportedEffects << " effects:";

			SLchar effectName[257];

			for (SLuint32 n = 0u; n < numberSupportedEffects; ++n)
			{
				SLInterfaceID slEffectTypeId = nullptr;
				SLInterfaceID slEffectImplementationId = nullptr;

				SLuint16 effectNameSize = 256u;

				if ((*slAndroidEffectCapabilities)->QueryEffect(slAndroidEffectCapabilities, n, &slEffectTypeId, &slEffectImplementationId, effectName, &effectNameSize) == SL_RESULT_SUCCESS)
				{
					ocean_assert(effectNameSize != 0u && effectNameSize <= 256u);

					if (effectNameSize > 0u && effectNameSize <= 256u && slEffectTypeId != nullptr && slEffectImplementationId != nullptr)
					{
						std::string name((const char*)(effectName), effectNameSize);

						Log::debug() << String::toAString(n, 2u) << ": " << name << ", " << toAString(slEffectTypeId);

						ocean_assert(engineEffectMap_.find(name) == engineEffectMap_.cend());
						engineEffectMap_.emplace(std::move(name), EngineEffect(slEffectTypeId, slEffectImplementationId));
					}
				}
			}
		}
	}
}

AMedium::AMedium(const std::string& url) :
	Medium(url)
{
	libraryName_ = nameAndroidLibrary();
}

bool AMedium::areInterfaceIdsIdentical(const SLInterfaceID& interfaceId0, const SLInterfaceID& interfaceId1)
{
	ocean_assert(interfaceId0 != nullptr && interfaceId1 != nullptr);

	static_assert(sizeof(SLInterfaceID_) > sizeof(size_t), "Invalid data type!");

	const bool result = memcmp(interfaceId0, interfaceId1, sizeof(SLInterfaceID_)) == 0;

#ifdef OCEAN_DEBUG
	{
		const bool debugResult = interfaceId0->time_low == interfaceId1->time_low && interfaceId0->time_mid == interfaceId1->time_mid
							&& interfaceId0->time_hi_and_version == interfaceId1->time_hi_and_version && interfaceId0->clock_seq == interfaceId1->clock_seq
							&& interfaceId0->node[0] == interfaceId1->node[0] && interfaceId0->node[1] == interfaceId1->node[1] && interfaceId0->node[2] == interfaceId1->node[2]
							&& interfaceId0->node[3] == interfaceId1->node[3] && interfaceId0->node[4] == interfaceId1->node[4] && interfaceId0->node[5] == interfaceId1->node[5];
		ocean_assert(result == debugResult);
	}
#endif

	return result;
}

std::string AMedium::toAString(const SLInterfaceID& interfaceId)
{
	ocean_assert(interfaceId != nullptr);

	return String::toAStringHex(interfaceId->time_low, false) + "-" + String::toAStringHex(interfaceId->time_mid, false) + "-"
				+ String::toAStringHex(interfaceId->time_hi_and_version, false) + "-" + String::toAStringHex(interfaceId->clock_seq, false) + "-"
				+ String::toAStringHex(interfaceId->node, 6, false);
}

}

}

}
