/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MICROPHONE_H
#define META_OCEAN_MEDIA_MICROPHONE_H

#include "ocean/media/Media.h"
#include "ocean/media/LiveMedium.h"

#include "ocean/base/ScopedSubscription.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class Microphone;

/**
 * Definition of a smart medium reference holding a microphone object.
 * @see SmartMediumRef, Microphone.
 * @ingroup media
 */
typedef SmartMediumRef<Microphone> MicrophoneRef;

/**
 * This class is the base class for all microphones.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT Microphone : public virtual LiveMedium
{
	public:

		/**
		 * Definition of individual microphone types.
		 */
		enum MicrophoneTypes : uint32_t
		{
			/// An unknown microphone type.
			MT_UNKNOWN = 0u,
			/// A built-in microphone (e.g., in a mobile phone).
			MT_BUILTIN = 1u << 0u,
			/// An external microphone (e.g., as part of headphones).
			MT_EXTERNAL = 1u << 1u,
			/// Any microphone either built-in or external.
			MT_ANY = MT_BUILTIN | MT_EXTERNAL
		};

		/**
		 * Definition of individual microphone configurations.
		 */
		enum MicrophoneConfigurations : uint32_t
		{
			/// A default microphone configuration.
			MC_DEFAULT = 0u,
			/// The microphone is configured for voice communication and can include features like an Acoustic Echo Canceler, may have some latency.
			MC_VOICE_COMMUNICATION = 1u << 0u,
			/// The microphone is configured for performance with low latency.
			MC_PERFORMANCE = 1u << 1u,
			/// The microphone is gained.
			MC_GAINED = 1u << 2u
		};

		/**
		 * Definition of individual samples types.
		 */
		enum SamplesType : uint32_t
		{
			/// An invalid samples type
			ST_INVALID = 0u,
			/// Samples with 16bit signed integer precision as mono channel and 48kHz
			ST_INTEGER_16_MONO_48
		};

		/**
		 * Definition of a callback function for microphone samples.
		 * @param samplesType The type of the samples
		 * @param data The samples data, must be valid
		 * @param size The size of the data, in bytes, with range [1, infinity)
		 */
		using SamplesCallbackFunction = std::function<void(const SamplesType samplesType, const void* data, const size_t size)>;

		/**
		 * Definition of a subscription object for microphone samples.
		 */
		using SamplesScopedSubscription = ScopedSubscriptionHandler::ScopedSubscriptionType;

	protected:

		/**
		 * Definition of a subscription handler for voip samples events.
		 */
		using SamplesCallbackHandler = ScopedSubscriptionCallbackHandlerT<SamplesCallbackFunction, Microphone, true>;

	public:

		/**
		 * Returns the type of this microphone.
		 * @return The microphone's type
		 */
		virtual MicrophoneTypes microphoneTypes() const;

		/**
		 * Returns the configuration of this microphone.
		 * @return The microphone's configuration
		 */
		virtual MicrophoneConfigurations microphoneConfigurations() const;

		/**
		 * Adds a new callback function for samples events.
		 * @param samplesCallbackFunction The callback function to add, must be valid
		 * @return The subscription object, the callback function will exist as long as the subscription object exists
		 */
		[[nodiscard]] virtual SamplesScopedSubscription addSamplesCallback(SamplesCallbackFunction samplesCallbackFunction);

	protected:

		/**
		 * Creates a new microphone by a given url.
		 * @param url Url of the microphone
		 */
		explicit Microphone(const std::string& url);

		/**
		 * Sends samples to all subscribed callback functions.
		 * @param samplesType The type of the samples
		 * @param data The samples data, must be valid
		 * @param size The size of the data, in bytes, with range [1, infinity)
		 */
		inline void sendSamples(const SamplesType samplesType, const void* data, const size_t size);

	protected:

		/// The microphone's types.
		MicrophoneTypes microphoneTypes_ = MT_UNKNOWN;

		/// The microphone's configurations.
		MicrophoneConfigurations microphoneConfigurations_ = MC_DEFAULT;

		/// The handler for samples callback functions.
		SamplesCallbackHandler samplesCallbackHandler_;
};

inline void Microphone::sendSamples(const SamplesType samplesType, const void* data, const size_t size)
{
	samplesCallbackHandler_.callCallbacks(samplesType, data, size);
}

}

}

#endif // META_OCEAN_MEDIA_MICROPHONE_H
