/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_A_MEDIUM_H
#define META_OCEAN_MEDIA_ANDROID_A_MEDIUM_H

#include "ocean/media/android/Android.h"

#include "ocean/base/Singleton.h"

#include "ocean/media/Medium.h"

#include <jni.h>

#include <SLES/OpenSLES.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

/**
 * This class implements the base class for all Medium objects in the Android library.
 * @ingroup mediaandroid
 */
class OCEAN_MEDIA_A_EXPORT AMedium : virtual public Medium
{
	friend class ALibrary;

	public:

		/**
		 * Definition of the interface for Noise Suppressor (NS).
		 */
		static constexpr SLInterfaceID_ slEffectTypeNoiseSuppressorId_ = {0x58b4b260, 0x8e06, 0x11e0, 0xaa8e, {0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};

		/**
		 * Definition of the interface for Acoustic Echo Canceler (AEC).
		 */
		static constexpr SLInterfaceID_ slEffectTypeAcousticEchoCancelerId_ = {0x7b491460, 0x8d4d, 0x11e0, 0xbd61, {0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}};

	protected:

		/**
		 * This manager holds all effects available in the engine.
		 */
		class EffectManager final : public Singleton<EffectManager>
		{
			friend class Singleton<EffectManager>;
			friend class ALibrary;

			protected:

				/**
				 * This class holds the two interfaces of a OpenSL engine effect.
				 */
				class EngineEffect
				{
					public:

						/**
						 * Creates a new effect object.
						 * @param slEffectTypeId The id of the effect type
						 * @param slEffectImplementationId The id of the effect implementation
						 */
						EngineEffect(const SLInterfaceID slEffectTypeId, const SLInterfaceID slEffectImplementationId);

					public:

						/// The id of the effect type.
						SLInterfaceID slEffectTypeId_ = nullptr;

						/// The id of the effect implementation.
						SLInterfaceID slEffectImplementationId_ = nullptr;
				};

				/**
				 * Definition of a map mapping effect names to effect objects.
				 */
				typedef std::unordered_map<std::string, EngineEffect> EngineEffectMap;

			public:

				/**
				 * Returns whether the engine has a specifc effect.
				 * @param effectName The name of the effect to check.
				 * @param slEffectTypeId The resulting type id of the requested effect
				 * @param slEffectImplementationId The resulting implementation id of the requested effect
				 * @return True, if so
				 */
				bool effectIds(const std::string& effectName, SLInterfaceID& slEffectTypeId, SLInterfaceID& slEffectImplementationId) const;

			protected:

				/**
				 * Default constructor.
				 */
				EffectManager() = default;

				/**
				 * Initializes the manager.
				 * @param slEngine The SL engine to be used, must be valid
				 */
				void initialize(const SLObjectItf& slEngine);

			protected:

				/// The map mapping effect names to effect interfaces.
				EngineEffectMap engineEffectMap_;

				/// The manager's lock.
				mutable Lock lock_;
		};

	public:

		/**
		 * Returns whether two interface ids are identical.
		 * @param interfaceId0 The first interface id, must be valid
		 * @param interfaceId1 The second interface id, must be valid
		 * @return True, if so
		 */
		static bool areInterfaceIdsIdentical(const SLInterfaceID& interfaceId0, const SLInterfaceID& interfaceId1);

		/**
		 * Returns the hex string of an interface id,
		 * @param interfaceId The id of the interface
		 * @return The resulting string
		 */
		static std::string toAString(const SLInterfaceID& interfaceId);

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url The URL of the medium
		 */
		explicit AMedium(const std::string& url);
};

}

}

}

#endif // META_OCEAN_MEDIA_ANDROID_A_MEDIUM_H
