// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_NETWORK_VERTS_VOIP_H
#define FACEBOOK_NETWORK_VERTS_VOIP_H

#include "ocean/network/verts/Verts.h"

#include "ocean/base/ScopedSubscription.h"
#include "ocean/base/Singleton.h"

#include "ocean/media/LiveAudio.h"
#include "ocean/media/Microphone.h"

#include "ocean/network/verts/Driver.h"

namespace Ocean
{

namespace Network
{

namespace Verts
{

/**
 * This class implements Voip functionalities.
 * @ingroup networkverts
 */
class OCEAN_NETWORK_VERTS_EXPORT Voip : public Singleton<Voip>
{
	friend class Singleton<Voip>;

	public:

		/**
		 * Definition of a scoped subscription object for voip.
		 */
		typedef ScopedSubscription<std::string, Voip> VoipScopedSubscription;

	protected:

		/**
		 * This class holds the relevant information for a voip zone.
		 */
		class Zone
		{
			friend class Voip;

			public:

				/**
				 * Creates a new zone object.
				 * @param vertsDriver The VERTS driver associated with the zone, must be valid
				 */
				explicit Zone(SharedDriver vertsDriver);

				/**
				 * Increments the usage of this zone.
				 */
				void incrementUsage();

				/**
				 * Decrements the usage of this zone.
				 * @return True, if the zone is still in use
				 */
				bool decrementUsage();

			protected:

				/**
				 * Event function for voip samples with 16bit signed integer precision with mono channel and 48kHz.
				 * @param driver The VERTS driver sending the samples
				 * @param samples The voip samples, must be valid
				 * @param size The number of samples, with range [1, infinity)
				 */
				void onVoipSamples(Driver& driver, const int16_t* samples, const size_t size);

			protected:

				/// The counter how often this zone is used.
				unsigned int usageCounter_ = 0u;

				/// The live audio object which will play the voip data.
				Media::LiveAudioRef liveAudio_;

				/// The VERTS driver representing the zone.
				SharedDriver vertsDriver_;

				/// The subscription object for voip samples events.
				Driver::VoipSamplesScopedSubscription voipSamplesScopedSubscription_;
		};

		/**
		 * Definition of a map mapping zone names to zone objects.
		 */
		typedef std::unordered_map<std::string, std::unique_ptr<Zone>> ZoneMap;

	public:

		/**
		 * Starts the voip connection with a zone.
		 * The voip connection will be active for the zone as long as the resulting subscription object exists.
		 * @param zoneName The name of the zone, must be valid
		 * @return The subscription object associated with the zone, an invalid object in case of a failure
		 */
		[[nodiscard]] VoipScopedSubscription startVoip(const std::string& zoneName);

		/**
		 * Updates the location of the local user.
		 * The position in defined in a coordinate system shared across all users/avatars.
		 * @param position The position of the local user
		 */
		void updatePosition(const Vector3& position);

	protected:

		/**
		 * Creates a new object.
		 */
		Voip();

		/**
		 * Stopps the voip connection with a zone.
		 * @param zoneName The name of the zone
		 */
		void stopVoip(const std::string& zoneName);

		/**
		 * Event function for new microphone samples.
		 * @param samplesType The type of the samples
		 * @param data The samples data, must be valid
		 * @param size The size of the data, in bytes, with range [1, infinity)
		 */
		void onMicrophoneSample(const Media::Microphone::SamplesType samplesType, const void* data, const size_t size);

	protected:

		/// The map mapping zone names to zone objects.
		ZoneMap zoneMap_;

		/// The microphone to be used.
		Media::MicrophoneRef microphone_;

		/// The subscription for microphone samples.
		Media::Microphone::SamplesScopedSubscription microphoneSubscription_;

		/// The position of the local user.
		Vector3 position_ = Vector3(0, 0, 0);

		/// The lock for all voip connections.
		Lock lock_;
};

}

}

}

#endif // FACEBOOK_NETWORK_VERTS_VOIP_H
