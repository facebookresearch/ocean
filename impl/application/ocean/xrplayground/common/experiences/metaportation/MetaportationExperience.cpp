// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/metaportation/MetaportationExperience.h"

#include "application/ocean/xrplayground/common/ContentManager.h"
#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/detector/qrcodes/QRCodeDetector2D.h"
#include "ocean/cv/detector/qrcodes/QRCodeDecoder.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/Tracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/io/Compression.h"
#include "ocean/io/File.h"
#include "ocean/io/FileResolver.h"

#include "ocean/media/Manager.h"

#include "ocean/network/Resolver.h"

#include "ocean/rendering/Utilities.h"
#include "ocean/rendering/FrameTexture2D.h"

#include "ocean/tracking/mapbuilding/MultiViewMapCreator.h"
#include "ocean/tracking/mapbuilding/TrackerStereo.h"
#include "ocean/tracking/mapbuilding/Utilities.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/android/Resource.h"

	#include "ocean/platform/meta/quest/vrapi/HeadsetPoses.h"

	#include "ocean/platform/meta/quest/platformsdk/Manager.h"
	#include "ocean/platform/meta/quest/platformsdk/Room.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

using namespace Platform::Meta;
using namespace Platform::Meta::Quest;
using namespace Platform::Meta::Quest::VrApi;
using namespace Platform::Meta::Quest::VrApi::Application;

using namespace Tracking::MapTexturing;

void MetaportationExperience::InputData::updateInputData(std::shared_ptr<Frames> yFrames, SharedAnyCameras cameras, const HomogenousMatrix4& world_T_device, HomogenousMatrices4 device_T_cameras, const Timestamp& timestamp)
{
	ocean_assert(yFrames && yFrames->size() >= 1);
	ocean_assert(cameras.size() == yFrames->size());
	ocean_assert(timestamp.isValid());

	const ScopedLock scopedLock(lock_);

	timestamp_ = timestamp;
	yFrames_ = std::move(yFrames);
	cameras_ = std::move(cameras);
	world_T_device_ = world_T_device;
	device_T_cameras_ = std::move(device_T_cameras);
}

bool MetaportationExperience::InputData::latestInputData(Timestamp& lastTimestamp, std::shared_ptr<Frames>& yFrames, SharedAnyCameras& cameras, HomogenousMatrix4& world_T_device, HomogenousMatrices4& device_T_cameras) const
{
	const ScopedLock scopedLock(lock_);

	if (lastTimestamp >= timestamp_) // last timestamp can also be invalid
	{
		return false;
	}

	if (yFrames_ == nullptr)
	{
		return false;
	}

	ocean_assert(timestamp_.isValid());
	lastTimestamp = timestamp_;

	yFrames = yFrames_;
	cameras = cameras_;
	world_T_device = world_T_device_;
	device_T_cameras = device_T_cameras_;

	return true;
}

void MetaportationExperience::MapData::updateMapData(Vectors3&& objectPoints, Scalars&& objectPointStabilityFactors, std::vector<CV::Detector::FREAKDescriptors32>&& multiDescriptors)
{
	ocean_assert(objectPoints.size() == objectPointStabilityFactors.size());
	ocean_assert(objectPoints.size() == multiDescriptors.size());

	const ScopedLock scopedLock(lock_);

	objectPoints_ = std::move(objectPoints);
	objectPointStabilityFactors_ = std::move(objectPointStabilityFactors);
	multiDescriptors_ = std::move(multiDescriptors);
}

bool MetaportationExperience::MapData::latestMapData(Vectors3& objectPoints, Scalars& objectPointStabilityFactors, std::vector<CV::Detector::FREAKDescriptors32>& multiDescriptors) const
{
	const ScopedLock scopedLock(lock_);

	if (objectPoints_.empty())
	{
		return false;
	}

	objectPoints = std::move(objectPoints_);
	objectPointStabilityFactors = std::move(objectPointStabilityFactors_);
	multiDescriptors = std::move(multiDescriptors_);

	return true;
}

void MetaportationExperience::MapData::updateFeatureMap(Tracking::MapBuilding::SharedUnifiedFeatureMap unifiedFeatureMap)
{
	const ScopedLock scopedLock(lock_);

	unifiedFeatureMap_ = std::move(unifiedFeatureMap);
}

bool MetaportationExperience::MapData::latestFeatureMap(Tracking::MapBuilding::SharedUnifiedFeatureMap& unifiedFeatureMap) const
{
	const ScopedLock scopedLock(lock_);

	if (!unifiedFeatureMap_)
	{
		return false;
	}

	unifiedFeatureMap = std::move(unifiedFeatureMap_);

	return true;
}

MetaportationExperience::Game::Game(const bool isHost) :
	isHost_(isHost)
{
	userId_ = Avatars::Manager::get().userId();
	ocean_assert(userId_ != 0ull);
}

MetaportationExperience::Game::~Game()
{
	if (renderingTransformParent_headset_ && renderingGroup_headset_)
	{
		renderingTransformParent_headset_->removeChild(renderingGroup_headset_);
		renderingGroup_headset_.release();
	}

	if (renderingTransformParent_headset_T_relocalizer_ && renderingGroup_relocalizer_)
	{
		renderingTransformParent_headset_T_relocalizer_->removeChild(renderingGroup_relocalizer_);
		renderingGroup_relocalizer_.release();
	}

	if (renderingTransformParent_relocalizer_T_meshes_ && renderingGroup_meshes_)
	{
		renderingTransformParent_relocalizer_T_meshes_->removeChild(renderingGroup_meshes_);
		renderingGroup_meshes_.release();
	}
}

bool MetaportationExperience::Game::initialize(const Rendering::TransformRef& renderingTransform_headset, const Rendering::TransformRef& renderingTransform_headset_T_relocalizer, Rendering::TransformRef& renderingTransform_relocalizer_T_meshes, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	renderingTransformParent_headset_ = renderingTransform_headset;
	renderingTransformParent_headset_T_relocalizer_ = renderingTransform_headset_T_relocalizer;
	renderingTransformParent_relocalizer_T_meshes_ = renderingTransform_relocalizer_T_meshes;

	renderingGroup_headset_ = engine->factory().createGroup();
	renderingTransformParent_headset_->addChild(renderingGroup_headset_);

	renderingGroup_relocalizer_ = engine->factory().createGroup();
	renderingTransformParent_headset_T_relocalizer_->addChild(renderingGroup_relocalizer_);

	renderingGroup_meshes_ = engine->factory().createGroup();
	renderingTransformParent_relocalizer_T_meshes_->addChild(renderingGroup_meshes_);

	return true;
}

MetaportationExperience::GameVirus::Virus::Virus(Rendering::TransformRef renderingTransform) :
	renderingTransform_(std::move(renderingTransform))
{
	// nothing to do here
}

MetaportationExperience::GameVirus::GameVirus(const bool isHost, const Box3& boundingBox) :
	Game(isHost),
	boundingBox_(boundingBox)
{
	const IO::Files laserSoundFiles = IO::FileResolver::get().resolve(IO::File("laser_sound.mp3"), true);

	if (!laserSoundFiles.empty())
	{
		Media::AudioRef audio = Media::Manager::get().newMedium(laserSoundFiles.front()(), Media::Medium::AUDIO);

		if (audio)
		{
			while (audiosLaser_.size() < 3)
			{
				audiosLaser_.emplace(audio->clone());
			}
			audiosLaser_.emplace(std::move(audio));
		}
		else
		{
			Log::error() << "Failed to load laser sound '" << laserSoundFiles.front()() << "'";
		}
	}
	else
	{
		Log::error() << "Failed to load laser sound";
	}

	const IO::Files hitSoundFiles = IO::FileResolver::get().resolve(IO::File("hit_sound.mp3"), true);

	if (!hitSoundFiles.empty())
	{
		Media::AudioRef audio = Media::Manager::get().newMedium(hitSoundFiles.front()(), Media::Medium::AUDIO);

		if (audio)
		{
			while (audiosHit_.size() < 3)
			{
				audiosHit_.emplace(audio->clone());
			}
			audiosHit_.emplace(std::move(audio));
		}
		else
		{
			Log::error() << "Failed to load hit sound '" << hitSoundFiles.front()() << "'";
		}
	}
	else
	{
		Log::error() << "Failed to load hit sound";
	}
}

MetaportationExperience::GameVirus::~GameVirus()
{
	renderingTransformVirus_.release();
	virusMap_.clear();
}

bool MetaportationExperience::GameVirus::initialize(const Rendering::TransformRef& renderingTransform_headset, const Rendering::TransformRef& renderingTransform_headset_T_relocalizer, Rendering::TransformRef& renderingTransform_relocalizer_T_meshes, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	if (!Game::initialize(renderingTransform_headset, renderingTransform_headset_T_relocalizer, renderingTransform_relocalizer_T_meshes, engine, timestamp))
	{
		return false;
	}

	ContentManager::get().loadContent("corona.obj", ContentManager::LM_LOAD_ADD, std::bind(&MetaportationExperience::GameVirus::onContentHandled, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	ocean_assert(renderingGroup_headset_);

	Rendering:: MaterialRef material ;
	Rendering::TransformRef transformLaser = Rendering::Utilities::createCylinder(engine, Scalar(0.004), 10, RGBAColor(1.0f, 0.0f, 0.0f, 0.75f), nullptr, nullptr, &material);
	material->setEmissiveColor(RGBAColor(1.0f, 0.0f, 0.0f));

	renderingTransformLaserLocal_ = engine->factory().createTransform();
	renderingTransformLaserLocal_->setVisible(false);
	renderingTransformLaserLocal_->addChild(transformLaser);
	renderingTransformLaserRemote_ = engine->factory().createTransform();
	renderingTransformLaserRemote_->setVisible(false);
	renderingTransformLaserRemote_->addChild(transformLaser);

	renderingGroup_headset_->addChild(renderingTransformLaserLocal_);
	renderingGroup_headset_->addChild(renderingTransformLaserRemote_);

	virusPlacementTimestamp_ = timestamp + 5.0;

	Rendering::MaterialRef foregroundMaterial;
	renderingTransformScore_ = Rendering::Utilities::createText(*engine, "", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), true, 0, 0, Scalar(0.025), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingTextScore_, &foregroundMaterial);
	renderingTransformScore_->setVisible(false);
	foregroundMaterial->setEmissiveColor(RGBAColor(1.0f, 1.0f, 1.0f));

	renderingGroup_headset_->addChild(renderingTransformScore_);

	return true;
}

void MetaportationExperience::GameVirus::preUpdateGame(const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	if (virusMap_.empty() && renderingTransformVirus_)
	{
		for (unsigned int nVirus = 0u; nVirus < numberVirus_; ++nVirus)
		{
			Rendering::TransformRef transform = engine->factory().createTransform();
			transform->setVisible(false);
			transform->addChild(renderingTransformVirus_);

			virusMap_.emplace(EI_VIRUS_POSITIONS_START + nVirus, transform);

			renderingGroup_meshes_->addChild(transform);
		}
	}

	const HomogenousMatrix4 headsetWorld_T_meshes = renderingTransformParent_headset_->transformation() * renderingTransformParent_headset_T_relocalizer_->transformation() * renderingTransformParent_relocalizer_T_meshes_->transformation();

	const HomogenousMatrix4 meshes_T_headsetWorld = headsetWorld_T_meshes.inverted();

	if (virusPlacementTimestamp_.isValid())
	{
		if (timestamp >= virusPlacementTimestamp_)
		{
			virusPlacementTimestamp_.toInvalid();
			gameOverTimestamp_ = timestamp + 60.0;

			if (isHost_)
			{
				for (VirusMap::iterator iVirus = virusMap_.begin(); iVirus != virusMap_.end(); ++iVirus)
				{
					Virus& virus = iVirus->second;
					virus.isPlaced_ = true;

					Scalar xLower = Scalar(-5);
					Scalar xHigher = Scalar(5);

					Scalar zLower = Scalar(-5);
					Scalar zHigher = Scalar(5);

					Scalar yLower = Scalar(1);
					Scalar yHigher = Scalar(3);

					if (boundingBox_.isValid() && boundingBox_.xDimension() >= 2)
					{
						xLower = boundingBox_.lower().x();
						xHigher = boundingBox_.higher().x();
					}

					if (boundingBox_.isValid() && boundingBox_.yDimension() >= 2)
					{
						zHigher = std::max(Scalar(3), boundingBox_.higher().y());
					}

					if (boundingBox_.isValid() && boundingBox_.zDimension() >= 2)
					{
						zLower = boundingBox_.lower().z();
						zHigher = boundingBox_.higher().z();
					}

					const Vector3 translation = Vector3(Random::scalar(xLower, xHigher), Random::scalar(yLower, yHigher), Random::scalar(zLower, zHigher));
					const Quaternion rotation = Random::quaternion();

					virus.renderingTransform_->setVisible(true);
					virus.renderingTransform_->setTransformation(HomogenousMatrix4(translation, rotation));

					float translationValue[3] = {float(translation.x()), float(translation.y()), float(translation.z())};
					distributedStates_.updateState(iVirus->first, Value(translationValue, sizeof(float) * 3), Timestamp(true));
				}
			}
		}
		else
		{
			const int delay = int(double(virusPlacementTimestamp_ - timestamp));

			renderingTextScore_->setText(" Start in " + String::toAString(delay) + " seconds ");
		}
	}

	// we check whethere new virus locations have arrived, or changed

	for (VirusMap::iterator iVirus = virusMap_.begin(); iVirus != virusMap_.end(); ++iVirus)
	{
		Virus& virus = iVirus->second;

		Value valueVirus;
		Timestamp stateTimestamp;
		if (distributedStates_.changedState(iVirus->first, valueVirus, stateTimestamp, 0.0))
		{
			if (valueVirus.isBuffer())
			{
				size_t bufferSize = 0;
				const void* buffer = valueVirus.bufferValue(bufferSize);

				if (bufferSize == sizeof(VectorF3))
				{
					VectorF3 translation;
					memcpy(&translation, buffer, sizeof(VectorF3));

					if (virus.isPlaced_ && translation.x() == NumericF::minValue())
					{
						// the remote person killed the virus

						virus.renderingTransform_->setVisible(false);

						playAudioHit();
					}
					else if (!virus.isPlaced_)
					{
						virus.isPlaced_ = true;
						virus.renderingTransform_->setVisible(true);

						const Quaternion rotation = Random::quaternion();
						virus.renderingTransform_->setTransformation(HomogenousMatrix4(Vector3(translation), rotation));
					}
				}
			}
		}
	}

	if (virusPlacementTimestamp_.isInvalid())
	{
		int32_t counterHost = 0;
		int32_t counterGuest = 0;

		Value valueCounterHost;
		Timestamp stateTimestamp;
		if (distributedStates_.state(EI_COUNTER_HOST, valueCounterHost, stateTimestamp, 0.0) && valueCounterHost.isInt())
		{
			counterHost = valueCounterHost.intValue();
		}

		Value valueCounterGuest;
		if (distributedStates_.state(EI_COUNTER_GUEST, valueCounterGuest, stateTimestamp, 0.0) && valueCounterGuest.isInt())
		{
			counterGuest = valueCounterGuest.intValue();
		}

		std::string scoreString;

		if (gameOverTimestamp_.isValid())
		{
			if (timestamp >= gameOverTimestamp_)
			{
				scoreString += " --- Game Over --- \n\n";
			}
			else
			{
				const int timeLeft = int(double(gameOverTimestamp_ - timestamp));

				scoreString += " Time left: " + String::toAString(timeLeft) + " sec \n\n";
			}
		}

		if (isHost_)
		{
			scoreString += " You: " + String::toAString(counterHost) + " \n Friend: " + String::toAString(counterGuest) + " ";
		}
		else
		{
			scoreString += " You: " + String::toAString(counterGuest) + " \n Friend: " + String::toAString(counterHost) + " ";
		}

		renderingTextScore_->setText(scoreString);
	}

	const bool gameIsOver = gameOverTimestamp_.isValid() && timestamp >= gameOverTimestamp_;

	const TrackedRemoteDevice& trackedRemoteDevice = PlatformSpecific::get().trackedRemoteDevice();

	for (const TrackedRemoteDevice::RemoteType remoteType : {TrackedRemoteDevice::RT_LEFT, TrackedRemoteDevice::RT_RIGHT})
	{
		HomogenousMatrix4 headsetWorld_T_remoteDevice(false);
		if (trackedRemoteDevice.pose(remoteType, &headsetWorld_T_remoteDevice, nullptr, timestamp))
		{
			const HomogenousMatrix4 remoteHeadsetWorld_T_remoteAvatarWorld(Avatars::Manager::get().remoteHeadsetWorld_T_remoteAvatar(userId_));

			if (remoteHeadsetWorld_T_remoteAvatarWorld.isValid())
			{
				headsetWorld_T_remoteDevice = remoteHeadsetWorld_T_remoteAvatarWorld * headsetWorld_T_remoteDevice;
			}

			if (!gameIsOver && virusPlacementTimestamp_.isInvalid() && trackedRemoteDevice.buttonsPressed(remoteType) & ovrButton_Trigger)
			{
				const HomogenousMatrix4 remoteDevice_T_laser(Quaternion(Vector3(1, 0, 0), -1));
				const HomogenousMatrix4 headsetWorld_T_laser(headsetWorld_T_remoteDevice * remoteDevice_T_laser);

				renderingTransformLaserLocal_->setTransformation(headsetWorld_T_laser * HomogenousMatrix4(Vector3(0, Scalar(5.1), 0)));
				renderingTransformLaserLocal_->setVisible(true);
				hideTimestampLaserLocal_ = timestamp + 0.01;

				playAudioLaser();

				if (isHost_)
				{
					distributedStates_.updateState(EI_LASER_HOST, Value(HomogenousMatrixF4(headsetWorld_T_laser).data(), sizeof(HomogenousMatrixF4)), Timestamp(true));
				}
				else
				{
					distributedStates_.updateState(EI_LASER_GUEST, Value(HomogenousMatrixF4(headsetWorld_T_laser).data(), sizeof(HomogenousMatrixF4)), Timestamp(true));
				}

				const HomogenousMatrix4 meshes_T_laser = meshes_T_headsetWorld * headsetWorld_T_laser;

				ocean_assert(meshes_T_laser.yAxis().isUnit());
				if (!meshes_T_laser.yAxis().isUnit())
				{
					Log::warning() << "Not unit vector";
				}

				const Line3 laserRay = Line3(meshes_T_laser.translation(), meshes_T_laser.yAxis());

				for (VirusMap::iterator iVirus = virusMap_.begin(); iVirus != virusMap_.end(); ++iVirus)
				{
					Virus& virus = iVirus->second;

					if (virus.isPlaced_ && virus.renderingTransform_->visible() && laserRay.sqrDistance(virus.renderingTransform_->transformation().translation()) <= Scalar(0.05 * 0.05))
					{
						virus.renderingTransform_->setVisible(false);

						playAudioHit();

						++hitCounter_;

						distributedStates_.updateState(isHost_ ? EI_COUNTER_HOST : EI_COUNTER_GUEST, Value(int32_t(hitCounter_)), Timestamp(true));

						const VectorF3 invalidTranslation(NumericF::minValue(), NumericF::minValue(), NumericF::minValue());
						distributedStates_.updateState(iVirus->first, Value(invalidTranslation.data(), sizeof(VectorF3)), Timestamp(true));
					}
				}
			}

			renderingTransformScore_->setVisible(true);
			renderingTransformScore_->setTransformation(headsetWorld_T_remoteDevice * HomogenousMatrix4(Vector3(0, Scalar(0.1), Scalar(-0.05))));
		}
		else
		{
			renderingTransformScore_->setVisible(false);
		}
	}

	if (gameIsOver)
	{
		for (VirusMap::iterator iVirus = virusMap_.begin(); iVirus != virusMap_.end(); ++iVirus)
		{
			iVirus->second.renderingTransform_->setVisible(false);
		}

		return;
	}

	Value valueLaserRemote;
	Timestamp stateTimestamp;
	if (distributedStates_.changedState(isHost_ ? EI_LASER_GUEST : EI_LASER_HOST, valueLaserRemote, stateTimestamp, 0.0))
	{
		size_t bufferSize = 0;
		const void* buffer = valueLaserRemote.bufferValue(bufferSize);

		if (bufferSize == sizeof(HomogenousMatrixF4))
		{
			float matrixValues[16];
			memcpy(matrixValues, buffer, sizeof(HomogenousMatrixF4));

			renderingTransformLaserRemote_->setTransformation(HomogenousMatrix4(matrixValues) * HomogenousMatrix4(Vector3(0, Scalar(5.1), 0)));
			renderingTransformLaserRemote_->setVisible(true);
			hideTimestampLaserRemote_ = timestamp + 0.01;
		}

		playAudioLaser();
	}

	if (hideTimestampLaserLocal_.isValid() && timestamp >= hideTimestampLaserLocal_)
	{
		renderingTransformLaserLocal_->setVisible(false);
		hideTimestampLaserLocal_.toInvalid();
	}

	if (hideTimestampLaserRemote_.isValid() && timestamp >= hideTimestampLaserRemote_)
	{
		renderingTransformLaserRemote_->setVisible(false);
		hideTimestampLaserRemote_.toInvalid();
	}

	for (VirusMap::iterator iVirus = virusMap_.begin(); iVirus != virusMap_.end(); ++iVirus)
	{
		Virus& virus = iVirus->second;

		if (virus.isPlaced_ && virus.renderingTransform_->visible())
		{
			const Vector3 translation = Random::vector3(Scalar(-0.01), Scalar(0.01));
			const Euler rotation = Random::euler(Scalar(0), Scalar(0.1));

			virus.renderingTransform_->setTransformation(virus.renderingTransform_->transformation() * HomogenousMatrix4(translation, rotation));
		}
	}
}

void MetaportationExperience::GameVirus::playAudioLaser()
{
	if (!audiosLaser_.empty())
	{
		Media::AudioRef audio = audiosLaser_.front();
		audiosLaser_.pop();

		audio->start();
		audiosLaser_.emplace(std::move(audio));
	}
}

void MetaportationExperience::GameVirus::playAudioHit()
{
	if (!audiosHit_.empty())
	{
		Media::AudioRef audio = audiosHit_.front();
		audiosHit_.pop();

		audio->start();
		audiosHit_.emplace(std::move(audio));
	}
}

void MetaportationExperience::GameVirus::onContentHandled(const std::string& content, const bool succeeded, const std::vector<Rendering::SceneRef>& scenes)
{
	if (!succeeded || scenes.size() != 1)
	{
		Log::error() << "Failed to handle content!";
		return;
	}

	renderingTransformVirus_ = scenes.front();

	ContentManager::get().unloadContent(content);

	if (renderingTransformVirus_)
	{
		renderingTransformVirus_->setTransformation(HomogenousMatrix4(Vector3(0, 0, 0), Vector3(Scalar(0.25), Scalar(0.25), Scalar(0.25))));
	}
}

MetaportationExperience::GameMovie::GameMovie(const bool isHost) :
	Game(isHost)
{
	// nothing to do here
}

MetaportationExperience::GameMovie::~GameMovie()
{
	renderingTransformMount_.release();
	movie_.release();
}

bool MetaportationExperience::GameMovie::initialize(const Rendering::TransformRef& renderingTransform_headset, const Rendering::TransformRef& renderingTransform_headset_T_relocalizer, Rendering::TransformRef& renderingTransform_relocalizer_T_meshes, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	if (!Game::initialize(renderingTransform_headset, renderingTransform_headset_T_relocalizer, renderingTransform_relocalizer_T_meshes, engine, timestamp))
	{
		return false;
	}

	renderingTransformMount_ = engine->factory().createTransform();
	renderingTransformMount_->setVisible(false);

	Rendering::TransformRef transform = Rendering::Utilities::createBox(engine, Vector3(3, Scalar(0.2), 1), RGBAColor(0.7f, 0.7f, 0.7f));
	transform->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.1), 0)));
	renderingTransformMount_->addChild(transform);

	transform = Rendering::Utilities::createBox(engine, Vector3(Scalar(2.2), Scalar(1.8), Scalar(0.1)), RGBAColor(0.7f, 0.7f, 0.7f));
	transform->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(1), Scalar(0.2))));
	renderingTransformMount_->addChild(transform);

	transform = Rendering::Utilities::createBox(engine, Vector3(Scalar(2.04), Scalar(1.165), Scalar(0.001)), RGBAColor(0.0f, 0.0f, 0.0f));
	transform->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(1.25), Scalar(0.11))));
	renderingTransformMount_->addChild(transform);

	const IO::Files files = IO::FileResolver::get().resolve(IO::File("movie.mp4"), true);

	if (!files.empty())
	{
		movie_ = Media::Manager::get().newMedium(files.front()(), Media::Medium::MOVIE);
	}
	else
	{
		Log::error() << "Failed to load movie";
	}

	if (movie_)
	{
		transform = Rendering::Utilities::createBox(engine, Vector3(Scalar(2), Scalar(1.125), Scalar(0.001)), movie_);
	}
	else
	{
		transform = Rendering::Utilities::createBox(engine, Vector3(Scalar(2), Scalar(1.125), Scalar(0.001)), RGBAColor(1.0f, 0.0f, 0.0f));
	}

	transform->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(1.25), Scalar(0.1))));
	renderingTransformMount_->addChild(transform);

	renderingGroup_meshes_->addChild(renderingTransformMount_);

	return true;
}

void MetaportationExperience::GameMovie::preUpdateGame(const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	const HomogenousMatrix4 headsetWorld_T_meshes = renderingTransformParent_headset_->transformation() * renderingTransformParent_headset_T_relocalizer_->transformation() * renderingTransformParent_relocalizer_T_meshes_->transformation();

	const HomogenousMatrix4 meshes_T_headsetWorld = headsetWorld_T_meshes.inverted();

	if (isHost_)
	{
		const TrackedRemoteDevice& trackedRemoteDevice = PlatformSpecific::get().trackedRemoteDevice();

		if (definingMountLocation_)
		{
			const HomogenousMatrix4 world_T_floor = HeadsetPoses::world_T_floor(PlatformSpecific::get().ovr());

			for (const TrackedRemoteDevice::RemoteType remoteType : {TrackedRemoteDevice::RT_RIGHT})
			{
				HomogenousMatrix4 headsetWorld_T_remoteDevice(false);
				if (trackedRemoteDevice.pose(remoteType, &headsetWorld_T_remoteDevice, nullptr, timestamp))
				{
					const HomogenousMatrix4 remoteHeadsetWorld_T_remoteAvatarWorld(Avatars::Manager::get().remoteHeadsetWorld_T_remoteAvatar(userId_));

					if (remoteHeadsetWorld_T_remoteAvatarWorld.isValid())
					{
						headsetWorld_T_remoteDevice = remoteHeadsetWorld_T_remoteAvatarWorld * headsetWorld_T_remoteDevice;
					}

					const HomogenousMatrix4 remoteDevice_T_laser(Quaternion(Vector3(1, 0, 0), -1));
					const HomogenousMatrix4 headsetWorld_T_laser(headsetWorld_T_remoteDevice * remoteDevice_T_laser);

					const Line3 laserRay = Line3(headsetWorld_T_laser.translation(), headsetWorld_T_laser.yAxis());

					const Plane3 floorPlane(world_T_floor.translation(), Vector3(0, 1, 0));

					Vector3 floorIntersection;
					if (floorPlane.intersection(laserRay, floorIntersection) && ((floorIntersection - laserRay.point()) * laserRay.direction() > Scalar(0))) // intersection in front of user
					{
						const Vector3 controllerOnFloorPoint = floorPlane.projectOnPlane(laserRay.point());

						if (controllerOnFloorPoint.distance(floorIntersection) >= 6)
						{
							floorIntersection = controllerOnFloorPoint + (floorIntersection - controllerOnFloorPoint).normalizedOrZero() * 6;
						}
						else if (controllerOnFloorPoint.distance(floorIntersection) < 1)
						{
							floorIntersection = controllerOnFloorPoint + (floorIntersection - controllerOnFloorPoint).normalizedOrZero();
						}

						Vector3 zAxis = floorIntersection - controllerOnFloorPoint;

						if (zAxis.normalize())
						{
							const Vector3 yAxis = Vector3(0, 1, 0);
							const Vector3 xAxis = yAxis.cross(zAxis).normalizedOrZero();

							const HomogenousMatrix4 world_T_mount(xAxis, yAxis, zAxis, floorIntersection);
							const HomogenousMatrix4 meshes_T_mount = meshes_T_headsetWorld * world_T_mount;

							renderingTransformMount_->setTransformation(meshes_T_mount);
							renderingTransformMount_->setVisible(true);

							distributedStates_.updateState(EI_TRANSFORMATION_MOUNT, Value(HomogenousMatrixF4(meshes_T_mount).data(), sizeof(HomogenousMatrixF4)), Timestamp(true));

							if (trackedRemoteDevice.buttonsPressed(remoteType) & ovrButton_Trigger)
							{
								definingMountLocation_ = false;

								if (movie_)
								{
									movie_->start();

									distributedStates_.updateState(EI_PLAY_PAUSE, Value(true), Timestamp(true));
								}
							}
						}
					}
				}
			}
		}
	}

	if (!isHost_)
	{
		Value mountValue;
		Timestamp stateTimestamp;
		if (distributedStates_.changedState(EI_TRANSFORMATION_MOUNT, mountValue, stateTimestamp, 0.0))
		{
			size_t bufferSize = 0;
			const void* buffer = mountValue.bufferValue(bufferSize);

			if (bufferSize == sizeof(HomogenousMatrixF4))
			{
				float matrixValues[16];
				memcpy(matrixValues, buffer, sizeof(HomogenousMatrixF4));

				renderingTransformMount_->setTransformation(HomogenousMatrix4(matrixValues));
				renderingTransformMount_->setVisible(true);
			}
		}

		Value playPauseValue;
		if (distributedStates_.changedState(EI_PLAY_PAUSE, playPauseValue, stateTimestamp, 0.0))
		{
			if (playPauseValue.isBool())
			{
				if (playPauseValue.boolValue())
				{
					if (movie_)
					{
						movie_->start();
					}
				}
			}
		}
	}
}

MetaportationExperience::PortalBase::~PortalBase()
{
	release();
}

bool MetaportationExperience::PortalBase::initialize(const Rendering::TransformRef& parent, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	ocean_assert(parent);

	renderingTransform_headset_ = engine->factory().createTransform();
	parent->addChild(renderingTransform_headset_);

	renderingTransformText_headset_ = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingText_);
	renderingTransformText_headset_->setTransformation(HomogenousMatrix4(Vector3(0, 0, -2)));
	renderingTransform_headset_->addChild(renderingTransformText_headset_);

	renderingTransform_headset_T_relocalizer_ = engine->factory().createTransform();
	renderingTransform_headset_->addChild(renderingTransform_headset_T_relocalizer_);

	renderingTransform_relocalizer_T_meshes_ = engine->factory().createTransform();
	renderingTransform_headset_T_relocalizer_->addChild(renderingTransform_relocalizer_T_meshes_);

	renderingGroup_meshes_ = engine->factory().createGroup();
	renderingTransform_relocalizer_T_meshes_->addChild(renderingGroup_meshes_);

	vrTableMenu_ = VRTableMenu(engine, engine->framebuffers().front());

	return true;
}

void MetaportationExperience::PortalBase::release()
{
	gameMap_.clear();

	vrTableMenu_.release();

	renderingGroup_meshes_.release();
	renderingTransform_relocalizer_T_meshes_.release();
	renderingTransform_headset_T_relocalizer_.release();

	renderingText_.release();
	renderingTransformText_headset_.release();

	renderingTransform_headset_.release();
}

void MetaportationExperience::PortalBase::onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	if (key == "X")
	{
		// we toggle between mesh and point rendering

		const ScopedLock scopedLock(lock_);

		renderMode_ = RenderMode((renderMode_ + 1u) % RM_END);
	}

	if (key == "A")
	{
		timestampPressedButtonA_ = timestamp;

		if (vrTableMenu_.isShown())
		{
			vrTableMenu_.hide();
		}
	}
}

void MetaportationExperience::PortalBase::onKeyRelease(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	if (key == "A")
	{
		timestampPressedButtonA_.toInvalid();
	}
}

void MetaportationExperience::PortalBase::renderPortal(const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	TemporaryScopedLock scopedLock(lock_);

		Meshes meshes;
		if (!meshesQueue_.empty())
		{
			meshes = std::move(meshesQueue_.front());
			meshesQueue_.pop();
		}

	scopedLock.release();

	if (timestampPressedButtonA_.isValid() && timestamp > timestampPressedButtonA_ + 1.5) // long press
	{
		if (!vrTableMenu_.isShown())
		{
			const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(timestamp);

			if (world_T_device.isValid())
			{
				onShowPortalMenu(world_T_device);
			}
		}
	}

	if (vrTableMenu_.isShown())
	{
		std::string entryUrl;
		if (vrTableMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
		{
			if (!entryUrl.empty())
			{
				onPortalMenuEntrySelected(entryUrl);
			}

			vrTableMenu_.hide();
		}
	}

	if (!meshes.texturedMeshMap_.empty())
	{
		Rendering::FrameTexture2DRef frameTexture = engine->factory().createFrameTexture2D();
		frameTexture->setTexture(std::move(meshes.textureFrame_));
		frameTexture->setMinificationFilterMode(Rendering::Texture::MIN_MODE_LINEAR_MIPMAP_LINEAR);
		frameTexture->setMagnificationFilterMode(Rendering::Texture::MAG_MODE_LINEAR);
		frameTexture->setUseMipmaps(true);

		Rendering::TexturesRef textures = engine->factory().createTextures();
		textures->addTexture(frameTexture);

		Rendering::AttributeSetRef attributeSet = engine->factory().createAttributeSet();
		attributeSet->addAttribute(textures);

		for (NewTextureGenerator::TexturedMeshMap::const_iterator iTexturedMesh = meshes.texturedMeshMap_.cbegin(); iTexturedMesh != meshes.texturedMeshMap_.cend(); ++iTexturedMesh)
		{
			const NewTextureGenerator::TexturedMesh& texturedMesh = iTexturedMesh->second;

			const Vectors3& vertices = texturedMesh.vertices_;
			const Vectors2& textureCoordinates = texturedMesh.textureCoordinates_;

			Rendering::VertexSetRef vertexSet = engine->factory().createVertexSet();
			vertexSet->setVertices(vertices);
			vertexSet->setTextureCoordinates(textureCoordinates, 0u);

			Rendering::TrianglesRef triangles = engine->factory().createTriangles();
			triangles->setFaces((unsigned int)(vertices.size()) / 3u);

			triangles->setVertexSet(vertexSet);

			Rendering::GeometryRef geometry = engine->factory().createGeometry();
			geometry->addRenderable(triangles, attributeSet);

			Rendering::TransformRef transform = engine->factory().createTransform();
			transform->addChild(geometry);

			const Box3 boundingBox(vertices);

			renderingBlockMap_[iTexturedMesh->first] = std::make_pair(transform, boundingBox);
		}

		boundingBox_ = Box3();

		renderingGroup_meshes_->clear();
		for (RenderingBlockMap::const_iterator iBlock = renderingBlockMap_.cbegin(); iBlock != renderingBlockMap_.cend(); ++iBlock)
		{
			boundingBox_ += iBlock->second.second;

			renderingGroup_meshes_->addChild(iBlock->second.first);
		}

		world_T_mesh_.setTransformation(meshes.world_T_meshes_, timestamp);
	}

	const HomogenousMatrix4 world_T_mesh(world_T_mesh_.transformation(timestamp));

	if (world_T_mesh.isValid())
	{
		renderingTransform_relocalizer_T_meshes_->setTransformation(HomogenousMatrix4(world_T_mesh.translation(), world_T_mesh.rotation())); // 7-DOF to 6-DOF
	}

	if (nextNetworkDataThroughputTimestamp_.isValid() && Timestamp(true) >= nextNetworkDataThroughputTimestamp_)
	{
		const std::string tcpThroughput = " TCP Send: " + String::toAString((unsigned int)(PlatformSDK::Network::get().currentSendThroughput(true) / 1024), 1u)
												+ "KB/s, receive: " + String::toAString((unsigned int)(PlatformSDK::Network::get().currentReceiveThroughput(true) / 1024), 1u) + "KB/s ";

		const std::string udpThroughput = " UDP Send: " + String::toAString((unsigned int)(PlatformSDK::Network::get().currentSendThroughput(false) / 1024), 1u)
												+ "KB/s, receive: " + String::toAString((unsigned int)(PlatformSDK::Network::get().currentReceiveThroughput(false) / 1024), 1u) + "KB/s ";

		renderingText_->setText(" Throughput: \n" + tcpThroughput + "\n" + udpThroughput);
	}

	scopedLock.relock(lock_);
		const GameMap gameMap(gameMap_);
	scopedLock.release();

	for (GameMap::const_iterator iGame = gameMap_.cbegin(); iGame != gameMap_.cend(); ++iGame)
	{
		const SharedGame& game = iGame->second;

		if (!game->isInitialized())
		{
			game->initialize(renderingTransform_headset_, renderingTransform_headset_T_relocalizer_, renderingTransform_relocalizer_T_meshes_, engine, timestamp);
		}

		game->preUpdateGame(engine, timestamp);
	}
}

void MetaportationExperience::PortalBase::onShowPortalMenu(const HomogenousMatrix4& /*world_T_device*/)
{
	// nothing to do here
}

void MetaportationExperience::PortalBase::onPortalMenuEntrySelected(const std::string& entryUrl)
{
	if (entryUrl == "HIDE_DEBUG")
	{
		renderingTransformText_headset_->setVisible(false);
	}
	else if (entryUrl == "SHOW_DEBUG")
	{
		renderingTransformText_headset_->setVisible(true);
	}
	else if (entryUrl == "PLAY_VIRUS")
	{
		Log::info() << "Arena bounding box: " << boundingBox_.xDimension() << "x" << boundingBox_.yDimension() << "x" << boundingBox_.zDimension();

		TemporaryScopedLock scopedLock(lock_);
			gameMap_["game_virus"] = nullptr;
			gameMap_["game_virus"] = std::make_shared<GameVirus>(true, boundingBox_);
		scopedLock.release();

		if (remoteUserId_ != 0ull)
		{
			std::ostringstream stringStream(std::ios::binary);
			IO::OutputBitstream bitstream(stringStream);

			if (bitstream.write<unsigned long long>(gameTag_) && bitstream.write<std::string>("game_virus"))
			{
				const std::string gameData = stringStream.str();
				ocean_assert(!gameData.empty());

				std::vector<uint8_t> buffer((const uint8_t*)(gameData.c_str()), (const uint8_t*)(gameData.c_str() + gameData.size()));

				if (PlatformSDK::Network::get().sendToUserTCP(remoteUserId_, PlatformSDK::Network::CI_CUSTOM, std::move(buffer)))
				{
					Log::info() << "Sent command to start game 'game_virus'";
				}
				else
				{
					Log::warning() << "Failed to send the game name";
				}
			}
		}
	}
	else if (entryUrl == "WATCH_MOVIE")
	{
		TemporaryScopedLock scopedLock(lock_);
			gameMap_["game_movie"] = nullptr;
			gameMap_["game_movie"] = std::make_shared<GameMovie>(true);
		scopedLock.release();

		if (remoteUserId_ != 0ull)
		{
			std::ostringstream stringStream(std::ios::binary);
			IO::OutputBitstream bitstream(stringStream);

			if (bitstream.write<unsigned long long>(gameTag_) && bitstream.write<std::string>("game_movie"))
			{
				const std::string gameData = stringStream.str();
				ocean_assert(!gameData.empty());

				std::vector<uint8_t> buffer((const uint8_t*)(gameData.c_str()), (const uint8_t*)(gameData.c_str() + gameData.size()));

				if (PlatformSDK::Network::get().sendToUserTCP(remoteUserId_, PlatformSDK::Network::CI_CUSTOM, std::move(buffer)))
				{
					Log::info() << "Sent command to start game 'game_movie'";
				}
				else
				{
					Log::warning() << "Failed to send the game name";
				}
			}
		}
	}
}

bool MetaportationExperience::PortalBase::onReceiveMesh(IO::InputBitstream& bitstream)
{
	Meshes meshes;
	if (readMeshesFromStream(bitstream, meshes))
	{
		Log::info() << "Received " << meshes.texturedMeshMap_.size() << " meshes with texture " << meshes.textureFrame_.width() << "x" << meshes.textureFrame_.height();

		onNewMesh(meshes);

		const ScopedLock scopedLock(lock_);

		meshesQueue_.emplace(std::move(meshes));

		return true;
	}
	else
	{
		Log::error() << "Failed to decode mesh";
	}

	return false;
}

void MetaportationExperience::PortalBase::onNewMesh(const Meshes& meshes)
{
	// nothing to do here
}

bool MetaportationExperience::PortalCreator::initialize(const Rendering::TransformRef& parent, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	PortalBase::initialize(parent, engine, timestamp);

	const Network::Resolver::Addresses4 localAddresses = Network::Resolver::get().localAddresses();

	if (!localAddresses.empty())
	{
		tcpClient_.setReceiveCallback(Network::TCPClient::ReceiveCallback::create(*this, &MetaportationExperience::PortalCreator::onReceiveFromMobileTCP));

		udpServer_.setPort(Network::Port(6000, Network::Port::TYPE_READABLE));
		udpServer_.setReceiveCallback(Network::PackagedUDPServer::ReceiveCallback::create(*this, &MetaportationExperience::PortalCreator::onReceiveFromMobileUDP));
		udpServer_.start();
	}

	renderingText_->setText(" Start 'Metaportation Scanner' \n experience on your phone \n\n and scan the QR code ");

	renderingTransformDevice_headset_ = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.15), Scalar(0.075), Scalar(0.0075)), RGBAColor(0.7, 0.7, 0.7));
	renderingTransformDevice_headset_->setVisible(false);

	renderingTransform_headset_->addChild(renderingTransformDevice_headset_);

	startThread();

	return true;
}

bool MetaportationExperience::PortalCreator::setRemoteUserId(const uint64_t remoteUserId)
{
	const ScopedLock scopedLock(lock_);

	remoteUserId_ = remoteUserId;

	if (remoteUserId_ != 0ull)
	{
		PlatformSDK::Network::get().acceptConnectionForUser(remoteUserId_);
	}

	if (!receiveSubscription_)
	{
		receiveSubscription_ = PlatformSDK::Network::get().addReceiveCallback(PlatformSDK::Network::CI_CUSTOM, std::bind(&MetaportationExperience::PortalCreator::onReceiveFromHeadset, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
	}

	return true;
}

void MetaportationExperience::PortalCreator::release()
{
	receiveSubscription_.release();

	stopThreadExplicitly();

	udpServer_.stop();

	PortalBase::release();
}

Timestamp MetaportationExperience::PortalCreator::preUpdate(const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	renderPortal(engine, timestamp);

	const Timestamp lastDeviceTimestamp(lastDeviceTimestamp_.load());

	if (Timestamp(true) > lastDeviceTimestamp + 2.5) // hiding the box of the phone if we haven't received new poses in a while
	{
		renderingTransformDevice_headset_->setVisible(false);
	}

	return timestamp;
}

void MetaportationExperience::PortalCreator::onShowPortalMenu(const HomogenousMatrix4& world_T_device)
{
	VRTableMenu::Entries menuEntries;
	menuEntries.reserve(8);

	if (renderingTransformText_headset_->visible())
	{
		menuEntries.emplace_back("Hide debug information", "HIDE_DEBUG");
	}
	else
	{
		menuEntries.emplace_back("Show debug information", "SHOW_DEBUG");
	}

	if (renderingGroup_meshes_)
	{
		if (renderingGroup_meshes_->visible())
		{
			menuEntries.emplace_back("Hide scan", "HIDE_SCAN");
		}
		else
		{
			menuEntries.emplace_back("Show scan", "SHOW_SCAN");
		}
	}

	if (mapData_ && meshesManager_.numberMeshes() != 0)
	{
		menuEntries.emplace_back("Save room", "SAVE_ROOM");
	}

	VRTableMenu::Groups menuGroups;
	menuGroups.emplace_back("What do you want to do?", std::move(menuEntries));

	menuEntries = VRTableMenu::Entries();

	menuEntries.emplace_back("Play Virus", "PLAY_VIRUS");
	menuEntries.emplace_back("Watch Movie", "WATCH_MOVIE");

	if (!menuEntries.empty())
	{
		menuGroups.emplace_back("Or some fun?", std::move(menuEntries));
	}

	vrTableMenu_.setMenuEntries(menuGroups);
	vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, -1)), world_T_device);
}

void MetaportationExperience::PortalCreator::onPortalMenuEntrySelected(const std::string& entryUrl)
{
	if (entryUrl == "HIDE_SCAN")
	{
		renderingGroup_meshes_->setVisible(false);
	}
	else if (entryUrl == "SHOW_SCAN")
	{
		renderingGroup_meshes_->setVisible(true);
	}
	else if (entryUrl == "SAVE_ROOM")
	{
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			ocean_assert(Platform::Android::ResourceManager::get().isValid());
			const IO::Directory directory = IO::Directory(Platform::Android::ResourceManager::get().externalFilesDirectory()) + IO::Directory("metaportation/rooms/room_" + String::toAString(n, 3u));

			if (!directory.exists())
			{
				if (!directory.create() || !saveRoom(directory))
				{
					renderingText_->setText(" Failed to save room ");
				}

				break;
			}
		}
	}
	else
	{
		PortalBase::onPortalMenuEntrySelected(entryUrl);
	}
}

void MetaportationExperience::PortalCreator::onNewMesh(const Meshes& meshes)
{
	meshesManager_.updateMeshes(meshes.world_T_meshes_, TexturedMeshMap(meshes.texturedMeshMap_), Frame(meshes.textureFrame_, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT));
}

void MetaportationExperience::PortalCreator::onReceiveFromMobileTCP(const void* data, const size_t size)
{
	if (size == 0)
	{
		return;
	}

	const ScopedUncompressedMemory scopedUncompressedMemory(data, size);

	const std::string inputString((const char*)(scopedUncompressedMemory.data()), scopedUncompressedMemory.size());

	std::istringstream stringStream(inputString, std::ios::binary);
	IO::InputBitstream bitstream(stringStream);

	unsigned long long tag;
	if (bitstream.look<unsigned long long>(tag))
	{
		switch (tag)
		{
			case meshTag_:
			{
				onReceiveMesh(bitstream);

				const uint8_t* data8 = (const uint8_t*)(data);
				Buffer dataCopy(data8, data8 + size);

				TemporaryScopedLock scopedLock(lock_);
					meshBufferQueue_.push(std::move(dataCopy));
				scopedLock.release();

				break;
			}

			default:
			{
				Log::error() << "Unknown tag";
				break;
			}
		}
	}
}

void MetaportationExperience::PortalCreator::onReceiveFromMobileUDP(const Network::Address4& address, const Network::Port& port, const void* data, const size_t size, const Network::PackagedUDPServer::MessageId messageId)
{
	const std::string inputString((const char*)(data), size);

	std::istringstream stringStream(inputString, std::ios::binary);
	IO::InputBitstream bitstream(stringStream);

	unsigned long long tag;
	if (bitstream.look<unsigned long long>(tag))
	{
		switch (tag)
		{
			case transformationTag_:
			{
				HomogenousMatrix4 world_T_camera;
				if (readTransformationFromStream(bitstream, world_T_camera))
				{
					ocean_assert(world_T_camera.isValid());
					ocean_assert(renderingTransformDevice_headset_);

					renderingTransformDevice_headset_->setTransformation(world_T_camera * HomogenousMatrix4(Vector3(Scalar(0.065), Scalar(-0.02), 0)));
					renderingTransformDevice_headset_->setVisible(true);

					lastDeviceTimestamp_ = Timestamp(true);
				}
				else
				{
					Log::error() << "Failed to decode camera pose";
				}

				break;
			}

			default:
			{
				Log::error() << "Unknown tag";
				break;
			}
		}
	}
}

void MetaportationExperience::PortalCreator::onReceiveFromHeadset(const uint64_t senderUserId, const Platform::Meta::Quest::PlatformSDK::Network::ComponentId componentId, const void* data, const size_t size, const Platform::Meta::Quest::PlatformSDK::Network::ConnectionType connectionType)
{
	ocean_assert(componentId == PlatformSDK::Network::CI_CUSTOM);
	ocean_assert(data != nullptr && size != 0);

	if (connectionType == PlatformSDK::Network::CT_UDP)
	{
		Log::warning() << "Received unexpected UDP data from user " << senderUserId;
	}
	else
	{
		Log::warning() << "Received unexpected TCP data from user " << senderUserId;
	}
}

void MetaportationExperience::PortalCreator::threadRun()
{
	std::vector<Media::FrameMediumRef> frameMediums;

	for (size_t cameraId = 0; cameraId < 4; ++cameraId)
	{
		Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium("LiveVideoId:" + String::toAString(cameraId));

		if (frameMedium.isNull())
		{
			Log::error() << "Failed to access headset camera " << cameraId << ", see https://fburl.com/access_cameras";

			renderingText_->setText(" Failed to access the cameras \n see https://fburl.com/access_cameras ");

			return;
		}

		frameMedium->start();
		frameMediums.emplace_back(std::move(frameMedium));
	}

	HomogenousMatrix4 world_T_floor(false);

	Network::Address4 phoneAddress;
	Network::Port phonePort;

	Timestamp lastFrametimestamp(false);

	InputData inputData;
	mapData_ = std::make_shared<MapData>();

	MapCreatorThread mapCreatorThread(inputData, *mapData_);
	MapHandlingThread mapHandlingThread(*this, *mapData_);

	while (!shouldThreadStop())
	{
		if (remoteUserId_ != 0ull)
		{
			if (!world_T_floor.isValid())
			{
				world_T_floor = HeadsetPoses::world_T_floor(PlatformSpecific::get().ovr());

				if (world_T_floor.isValid())
				{
					std::ostringstream stringStream(std::ios::binary);
					IO::OutputBitstream bitstream(stringStream);

					if (writeVector3(world_T_floor.translation(), worldFloorTag_, bitstream))
					{
						const std::string worldFloorData = stringStream.str();
						ocean_assert(!worldFloorData.empty());

						std::vector<uint8_t> buffer((const uint8_t*)(worldFloorData.c_str()), (const uint8_t*)(worldFloorData.c_str() + worldFloorData.size()));

						if (!PlatformSDK::Network::get().sendToUserTCP(remoteUserId_, PlatformSDK::Network::CI_CUSTOM, std::move(buffer)))
						{
							Log::warning() << "Failed to send distance between world and floor";
						}
					}
				}
			}

			if (PlatformSDK::Network::get().sendQueueSizeTCP(remoteUserId_) == 0)
			{
				// the send queue is empty, so we can send the most recent meshes

				TemporaryScopedLock scopedLock(lock_);
					Buffer meshBuffer;
					if (!meshBufferQueue_.empty())
					{
						meshBuffer = std::move(meshBufferQueue_.front());
						meshBufferQueue_.pop();
					}
				scopedLock.release();

				if (!meshBuffer.empty())
				{
					if (!PlatformSDK::Network::get().sendToUserTCP(remoteUserId_, PlatformSDK::Network::CI_CUSTOM, std::move(meshBuffer)))
					{
						Log::warning() << "Failed to send mesh via network to all users in the room";
					}
				}
			}
		}

		FrameRefs frames;
		SharedAnyCameras cameras;

		bool timedOut = false;
		if (!Media::FrameMedium::syncedFrames(frameMediums, lastFrametimestamp, frames, cameras, 2u /*waitTime*/, &timedOut))
		{
			if (timedOut)
			{
				Log::warning() << "Failed to access synced camera frames for timestamp";
			}

			continue;
		}

		ocean_assert(frameMediums.size() == frames.size() && frameMediums.size() == cameras.size());

		lastFrametimestamp = frames.front()->timestamp();

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
		const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(lastFrametimestamp);
		if (!world_T_device.isValid())
		{
			Log::debug() << "Failed to determine headset pose for timestamp " << double(lastFrametimestamp);

			continue;
		}
#else
		ocean_assert(false && "This should never happen!");
		const HomogenousMatrix4 world_T_device(false);
		break;
#endif

		HomogenousMatrices4 device_T_cameras(frameMediums.size());
		Frames yFrames(frameMediums.size());

		for (size_t cameraId = 0; cameraId < frameMediums.size(); ++cameraId)
		{
			if (!CV::FrameConverter::Comfort::convert(*frames[cameraId], FrameType(*frames[cameraId], FrameType::FORMAT_Y8), yFrames[cameraId], CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, nullptr, CV::FrameConverter::Options(0.6f, true)))
			{
				ocean_assert(false && "This should never happen!");
				return;
			}

			device_T_cameras[cameraId] = HomogenousMatrix4(frameMediums[cameraId]->device_T_camera());
		}

		inputData.updateInputData(std::make_shared<Frames>(std::move(yFrames)), cameras, world_T_device, device_T_cameras, lastFrametimestamp);

		if (!phoneAddress.isValid())
		{
			if (detectQRCodeWithAddress(inputData, phoneAddress, phonePort))
			{
				Log::info() << "Decoded phone address: " << phoneAddress.readable() << ", " << phonePort.readable();
			}
		}

		if (phoneAddress.isValid())
		{
			ocean_assert(phonePort.isValid());

			if (!tcpClient_.isConnected())
			{
				if (tcpClient_.connect(phoneAddress, phonePort))
				{
					renderingText_->setText("Connection started");

					const Network::Port udpPort = udpServer_.port();

					std::ostringstream stringStream(std::ios::binary);
					IO::OutputBitstream bitstream(stringStream);

					if (bitstream.write<unsigned long long>(portTag_) && bitstream.write<unsigned short>(udpPort))
					{
						const std::string portData = stringStream.str();
						ocean_assert(!portData.empty());

						if (tcpClient_.send(portData.c_str(), portData.size()) == Network::Socket::SR_SUCCEEDED)
						{
							Log::info() << "Sent UDP port: " << udpPort.readable();
							continue;
						}
					}

					renderingText_->setText("Failed to send UDP port");
				}
				else
				{
					renderingText_->setText("Connection failed");
				}
			}
		}
	}
}

bool MetaportationExperience::PortalCreator::saveRoom(const IO::Directory& directory)
{
	ocean_assert(directory.exists());

	TemporaryScopedLock scopedLock(lock_);

		const std::shared_ptr<MapData> mapData(mapData_);

		if (!mapData)
		{
			Log::error() << "Invalid map data";
			return false;
		}

		Vectors3 objectPoints;
		Scalars objectPointStabilityFactors;
		std::vector<CV::Detector::FREAKDescriptors32> multiDescriptors;

		if (!mapData->latestMapData(objectPoints, objectPointStabilityFactors, multiDescriptors))
		{
			Log::info() << "Failed to extract map data";
			return false;
		}

		const Indices32 objectPointIds = createIndices<Index32>(objectPoints.size(), 0u);

		Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap descriptorMap;
		descriptorMap.reserve(objectPoints.size() * 3 / 2);

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			descriptorMap.emplace(objectPointIds[n], std::move(multiDescriptors[n]));
		}

		const std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> unifiedDescriptorMap = std::make_shared<Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256>(std::move(descriptorMap));

		std::vector<Meshes> meshesGroup;
		meshesManager_.latestMeshes(meshesGroup);

	scopedLock.release();

	{
		const IO::File file(directory + IO::File("room.metaportation_map"));

		std::ofstream fileStream(file().c_str(), std::ios::binary);
		IO::OutputBitstream bitstream(fileStream);

		if (!bitstream.write<unsigned long long>(mapTag_)
	 			|| !writeObjectPointsToStream(objectPoints, objectPointIds, bitstream)
				|| !Tracking::MapBuilding::Utilities::writeDescriptorMap(*unifiedDescriptorMap, bitstream))
		{
			Log::error() << "Failed to write map";
			return false;
		}
	}

	for (size_t nMeshes = 0u; nMeshes < meshesGroup.size(); ++nMeshes)
	{
		const std::string filename = "room_" + String::toAString((unsigned int)(nMeshes), 4u) + ".metaportation_meshes";

		const IO::File file(directory + IO::File(filename));

		std::ofstream fileStream(file().c_str(), std::ios::binary);
		IO::OutputBitstream bitstream(fileStream);

		if (!writeMeshesToStream(meshesGroup[nMeshes], bitstream))
		{
			Log::error() << "Failed to write room";
			return false;
		}
	}

	return true;
}

bool MetaportationExperience::PortalLoader::initialize(const Rendering::TransformRef& parent, const Rendering::EngineRef& engine, const Timestamp timestamp, const IO::Directory& directory)
{
	PortalBase::initialize(parent, engine, timestamp);

	directory_ = directory;

	startThread();

	return true;
}

bool MetaportationExperience::PortalLoader::setRemoteUserId(const uint64_t remoteUserId)
{
	const ScopedLock scopedLock(lock_);

	remoteUserId_ = remoteUserId;

	if (remoteUserId_ != 0ull)
	{
		PlatformSDK::Network::get().acceptConnectionForUser(remoteUserId_);
	}

	if (!receiveSubscription_)
	{
		receiveSubscription_ = PlatformSDK::Network::get().addReceiveCallback(PlatformSDK::Network::CI_CUSTOM, std::bind(&MetaportationExperience::PortalLoader::onReceiveFromHeadset, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
	}

	return true;
}

void MetaportationExperience::PortalLoader::release()
{
	receiveSubscription_.release();

	PortalBase::release();
}

Timestamp MetaportationExperience::PortalLoader::preUpdate(const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	if (sceneTracker_)
	{
		const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sample = sceneTracker_->sample(timestamp);

		if (sample)
		{
			if (sample->objectIds().size() == 1)
			{
				if (renderingTransformMapPoints_.isNull() && sample->sceneElements().front())
				{
					const Devices::SceneTracker6DOF::SceneElement& sceneElement = *sample->sceneElements().front();

					if (sceneElement.sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_OBJECT_POINTS)
					{
						const Devices::SceneTracker6DOF::SceneElementObjectPoints& sceneElementObjectPoints = dynamic_cast<const Devices::SceneTracker6DOF::SceneElementObjectPoints&>(sceneElement);

						const Vectors3& objectPoints = sceneElementObjectPoints.objectPoints();

						renderingTransformMapPoints_ = Rendering::Utilities::createPoints(*engine, objectPoints, RGBAColor(0.0f, 1.0f, 0.0f), Scalar(3));
						renderingTransformMapPoints_->setVisible(false);

						renderingTransform_headset_T_relocalizer_->addChild(renderingTransformMapPoints_);
					}
				}

				if (sample->timestamp() == timestamp)
				{
					const HomogenousMatrix4 headsetWorld_T_device = PlatformSpecific::get().world_T_device(timestamp);

					if (headsetWorld_T_device.isValid())
					{
						const HomogenousMatrix4 trackerWorld_T_device(sample->positions().front(), sample->orientations().front());
						const HomogenousMatrix4 headsetWorld_T_trackerWorld = headsetWorld_T_device * trackerWorld_T_device.inverted();

						// the mesh is defined in the tracker's world (as it was scanned in the tracker's world)

						renderingTransform_headset_T_relocalizer_->setTransformation(headsetWorld_T_trackerWorld);

						if (remoteUserId_ != 0ull)
						{
							std::ostringstream stringStream(std::ios::binary);
							IO::OutputBitstream bitstream(stringStream);

							if (writeTransformationToStream(headsetWorld_T_trackerWorld, bitstream))
							{
								const std::string transformationData = stringStream.str();
								ocean_assert(!transformationData.empty());

								if (!PlatformSDK::Network::get().sendToUserUDP(remoteUserId_, PlatformSDK::Network::CI_CUSTOM, transformationData.c_str(), transformationData.size()))
								{
									Log::warning() << "Failed to send relocalization pose";
								}
							}
						}

						if (!firstRelocalizationReceived_)
						{
							// we have relocalized for the first time, now we can show the scan

							renderingTransform_headset_T_relocalizer_->setVisible(true);
							firstRelocalizationReceived_ = true;
						}
					}
				}
			}
		}
	}

	renderPortal(engine, timestamp);

	return timestamp;
}

void MetaportationExperience::PortalLoader::onShowPortalMenu(const HomogenousMatrix4& world_T_device)
{
	VRTableMenu::Entries menuEntries;
	menuEntries.reserve(8);

	if (renderingTransformText_headset_->visible())
	{
		menuEntries.emplace_back("Hide debug information", "HIDE_DEBUG");
	}
	else
	{
		menuEntries.emplace_back("Show debug information", "SHOW_DEBUG");
	}

	if (renderingGroup_meshes_)
	{
		if (renderingGroup_meshes_->visible())
		{
			menuEntries.emplace_back("Hide scan", "HIDE_SCAN");
		}
		else
		{
			menuEntries.emplace_back("Show scan", "SHOW_SCAN");
		}
	}

	if (renderingTransformMapPoints_)
	{
		if (renderingTransformMapPoints_->visible())
		{
			menuEntries.emplace_back("Hide map points", "HIDE_MAP_POINTS");
		}
		else
		{
			menuEntries.emplace_back("Show map points", "SHOW_MAP_POINTS");
		}
	}

	VRTableMenu::Groups menuGroups;
	menuGroups.emplace_back("What do you want to do?", std::move(menuEntries));

	menuEntries = VRTableMenu::Entries();

	menuEntries.emplace_back("Play Virus", "PLAY_VIRUS");
	menuEntries.emplace_back("Watch Movie", "WATCH_MOVIE");

	if (!menuEntries.empty())
	{
		menuGroups.emplace_back("Or some fun?", std::move(menuEntries));
	}

	vrTableMenu_.setMenuEntries(menuGroups);
	vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, -1)), world_T_device);
}

void MetaportationExperience::PortalLoader::onPortalMenuEntrySelected(const std::string& entryUrl)
{
	if (entryUrl == "HIDE_SCAN")
	{
		renderingGroup_meshes_->setVisible(false);
	}
	else if (entryUrl == "SHOW_SCAN")
	{
		renderingGroup_meshes_->setVisible(true);
	}
	else if (entryUrl == "HIDE_MAP_POINTS")
	{
		renderingTransformMapPoints_->setVisible(false);
	}
	else if (entryUrl == "SHOW_MAP_POINTS")
	{
		renderingTransformMapPoints_->setVisible(true);
	}
	else
	{
		PortalBase::onPortalMenuEntrySelected(entryUrl);
	}
}

void MetaportationExperience::PortalLoader::onReceiveFromHeadset(const uint64_t senderUserId, const Platform::Meta::Quest::PlatformSDK::Network::ComponentId componentId, const void* data, const size_t size, const Platform::Meta::Quest::PlatformSDK::Network::ConnectionType connectionType)
{
	ocean_assert(componentId == PlatformSDK::Network::CI_CUSTOM);
	ocean_assert(data != nullptr && size != 0);

	if (connectionType == PlatformSDK::Network::CT_UDP)
	{
		Log::warning() << "Received unexpected UDP data from user " << senderUserId;
	}
	else
	{
		Log::warning() << "Received unexpected TCP data from user " << senderUserId;
	}
}

void MetaportationExperience::PortalLoader::threadRun()
{
	HomogenousMatrix4 world_T_floor(false);

	while (!shouldThreadStop())
	{
		sleep(1u);

		TemporaryScopedLock scopedLock(lock_);
			const IO::Directory directory(directory_);
			directory_ = IO::Directory();
		scopedLock.release();

		if (directory.isValid())
		{
			Log::info() << "Loading room '" << directory.name() << "'";

			loadRoom(directory);
		}

		if (remoteUserId_ != 0ull)
		{
			if (!world_T_floor.isValid())
			{
				world_T_floor = HeadsetPoses::world_T_floor(PlatformSpecific::get().ovr());

				if (world_T_floor.isValid())
				{
					std::ostringstream stringStream(std::ios::binary);
					IO::OutputBitstream bitstream(stringStream);

					if (writeVector3(world_T_floor.translation(), worldFloorTag_, bitstream))
					{
						const std::string worldFloorData = stringStream.str();
						ocean_assert(!worldFloorData.empty());

						std::vector<uint8_t> buffer((const uint8_t*)(worldFloorData.c_str()), (const uint8_t*)(worldFloorData.c_str() + worldFloorData.size()));

						if (!PlatformSDK::Network::get().sendToUserTCP(remoteUserId_, PlatformSDK::Network::CI_CUSTOM, std::move(buffer)))
						{
							Log::warning() << "Failed to send distance between world and floor";
						}
					}
				}
			}

			if (PlatformSDK::Network::get().sendQueueSizeTCP(remoteUserId_) == 0)
			{
				// the send queue is empty, so we can send the most recent meshes

				scopedLock.relock(lock_);
					if (meshesFilesForRemoteUser_.empty())
					{
						continue;
					}
					const IO::File meshesFile(meshesFilesForRemoteUser_.back());
				scopedLock.release();

				ocean_assert(meshesFile.exists());

				std::ifstream stream(meshesFile().c_str(), std::ios::binary);
				stream.seekg(0, std::ifstream::end);
				const uint64_t fileSize = uint64_t(stream.tellg());
				stream.seekg(0, std::ifstream::beg);

				if (fileSize != 0)
				{
					Buffer meshBuffer(fileSize);
					stream.read((char*)(meshBuffer.data()), meshBuffer.size());

					if (stream.good() && PlatformSDK::Network::get().sendToUserTCP(remoteUserId_, PlatformSDK::Network::CI_CUSTOM, std::move(meshBuffer)))
					{
						scopedLock.relock(lock_);
							meshesFilesForRemoteUser_.pop_back();
						scopedLock.release();
					}
					else
					{
						Log::warning() << "Failed to send mesh via network to all users in the room";
					}
				}
			}
		}
	}
}

bool MetaportationExperience::PortalLoader::loadRoom(const IO::Directory& directory)
{
	ocean_assert(directory.exists());

	const IO::File mapFile(directory + IO::File("room.metaportation_map"));

	IO::Files meshesFiles(directory.findFiles("metaportation_meshes"));

	if (!mapFile.exists() || meshesFiles.empty())
	{
		Log::error() << "Room files do not exists";
		renderingText_->setText(" The room could not be loaded ");

		return false;
	}

	TemporaryScopedLock temporaryScopedLock(lock_);

	sceneTracker_ = Devices::Manager::get().device("Quest On-Device Relocalizer 6DOF Tracker");

	if (Devices::VisualTrackerRef visualTracker = sceneTracker_)
	{
		const Media::FrameMediumRef frameMediumA = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);
		const Media::FrameMediumRef frameMediumB = Media::Manager::get().newMedium("LiveVideoId:1", Media::Medium::LIVE_VIDEO);

		if (frameMediumA && frameMediumB && frameMediumA->start() && frameMediumB->start())
		{
			visualTracker->setInput({frameMediumA, frameMediumB});

			const Devices::ObjectTrackerRef objectTracker = sceneTracker_;
			ocean_assert(objectTracker);

			temporaryScopedLock.release();

			if (objectTracker->registerObject(mapFile()) && sceneTracker_->start())
			{
				Log::info() << "On-device relocalization tracker started";
			}
			else
			{
				Log::error() << "Failed to start on-device relocalization tracker";
				return false;
			}
		}
		else
		{
			Log::error() << "Failed to access headset cameras, see https://fburl.com/access_cameras";

			renderingText_->setText(" Failed to access the cameras \n see https://fburl.com/access_cameras ");
			return false;
		}
	}
	else
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	// we do not show the scan until we have the first valid relocalization
	renderingTransform_headset_T_relocalizer_->setVisible(false);

	std::sort(meshesFiles.begin(), meshesFiles.end());

	for (const IO::File& meshesFile : meshesFiles)
	{
		if (shouldThreadStop())
		{
			return false;
		}

		std::ifstream fileStream(meshesFile().c_str(), std::ios::binary);
		IO::InputBitstream bitstream(fileStream);

		Meshes meshes;
		if (!readMeshesFromStream(bitstream, meshes))
		{
			Log::error() << "Failed to read meshs from file";

			renderingText_->setText(" The room could not be loaded ");
			return false;
		}

		const ScopedLock scopedLock(lock_);

		meshesQueue_.emplace(std::move(meshes));
	}

	meshesFilesForRemoteUser_.clear();
	meshesFilesForRemoteUser_.reserve(meshesFiles.size());

	for (size_t n = meshesFiles.size() - 1; n < meshesFiles.size(); --n)
	{
		meshesFilesForRemoteUser_.emplace_back(meshesFiles[n]);
	}

	return true;
}

bool MetaportationExperience::PortalReceiver::initialize(const Rendering::TransformRef& parent, const uint64_t senderUserId, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	if (!PortalBase::initialize(parent, engine, timestamp))
	{
		return false;
	}

	ocean_assert(remoteUserId_ == 0ull);
	remoteUserId_ = senderUserId;

	PlatformSDK::Network::get().acceptConnectionForUser(senderUserId);
	receiveSubscription_ = PlatformSDK::Network::get().addReceiveCallback(PlatformSDK::Network::CI_CUSTOM, std::bind(&MetaportationExperience::PortalReceiver::onReceiveFromHeadset, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));

	return receiveSubscription_.isValid();
}

void MetaportationExperience::PortalReceiver::release()
{
	receiveSubscription_.release();

	PortalBase::release();
}

Timestamp MetaportationExperience::PortalReceiver::preUpdate(const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	TemporaryScopedLock scopedLock(lock_);
		const Vector3 hostWorld_t_hostFloor(hostWorld_t_hostFloor_);
		const HomogenousMatrix4 headsetWorld_T_trackerWorld(headsetWorld_T_trackerWorld_);
	scopedLock.release();

#if 0
	HomogenousMatrix4 customWorld_T_world(true);

	if (!hostWorld_t_hostFloor.isNull())
	{
		const HomogenousMatrix4 world_T_floor = HeadsetPoses::world_T_floor(PlatformSpecific::get().ovr());
		const HomogenousMatrix4 hostWorld_T_floor(hostWorld_t_hostFloor);

		const HomogenousMatrix4 hostWorld_T_world(hostWorld_T_floor * world_T_floor.inverted());
		const Vector3 hostWorld_t_world(hostWorld_T_world.translation());

		customWorld_T_world = HomogenousMatrix4(Vector3(0, hostWorld_t_world.y(), 0));
	}
#endif

	if (headsetWorld_T_trackerWorld.isValid())
	{
		renderingTransform_headset_T_relocalizer_->setTransformation(headsetWorld_T_trackerWorld);
	}

	renderPortal(engine, timestamp);

	return timestamp;
}

void MetaportationExperience::PortalReceiver::onShowPortalMenu(const HomogenousMatrix4& world_T_device)
{
	VRTableMenu::Entries menuEntries;
	menuEntries.reserve(8);

	if (renderingTransformText_headset_->visible())
	{
		menuEntries.emplace_back("Hide debug information", "HIDE_DEBUG");
	}
	else
	{
		menuEntries.emplace_back("Show debug information", "SHOW_DEBUG");
	}

	const VRTableMenu::Group menuGroup("What do you want to do?", std::move(menuEntries));

	vrTableMenu_.setMenuEntries(menuGroup);
	vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, -1)), world_T_device);
}

void MetaportationExperience::PortalReceiver::onPortalMenuEntrySelected(const std::string& entryUrl)
{
	if (entryUrl == "HIDE_DEBUG")
	{
		renderingTransformText_headset_->setVisible(false);
	}
	else if (entryUrl == "SHOW_DEBUG")
	{
		renderingTransformText_headset_->setVisible(true);
	}
}

void MetaportationExperience::PortalReceiver::onReceiveFromHeadset(const uint64_t senderUserId, const PlatformSDK::Network::ComponentId componentId, const void* data, const size_t size, const PlatformSDK::Network::ConnectionType connectionType)
{
	ocean_assert(componentId == PlatformSDK::Network::CI_CUSTOM);
	ocean_assert(data != nullptr && size != 0);

	if (nextNetworkDataThroughputTimestamp_.isInvalid())
	{
		nextNetworkDataThroughputTimestamp_.toNow();
	}

	if (connectionType == PlatformSDK::Network::CT_TCP)
	{
		const ScopedUncompressedMemory scopedUncompressedMemory(data, size);

		const std::string inputString((const char*)(scopedUncompressedMemory.data()), scopedUncompressedMemory.size());

		std::istringstream stringStream(inputString, std::ios::binary);
		IO::InputBitstream bitstream(stringStream);

		unsigned long long tag = 0ull;
		bitstream.look<unsigned long long>(tag);

		if (tag == worldFloorTag_)
		{
			Vector3 world_t_floor;
			if (readVector3(bitstream, worldFloorTag_, world_t_floor))
			{
				const ScopedLock scopedLock(lock_);

				hostWorld_t_hostFloor_ = world_t_floor;
			}
		}
		else if (tag == meshTag_)
		{
			onReceiveMesh(bitstream);
		}
		else if (tag == gameTag_)
		{
			bitstream.read<unsigned long long>(tag);

			std::string gameName;
			if (bitstream.read<std::string>(gameName))
			{
				if (gameName == "game_virus")
				{
					const ScopedLock scopedLock(lock_);

					gameMap_["game_virus"] = nullptr;
					gameMap_["game_virus"] = std::make_shared<GameVirus>(false /*isHost*/, boundingBox_);
				}
				else if (gameName == "game_movie")
				{
					const ScopedLock scopedLock(lock_);

					gameMap_["game_movie"] = nullptr;
					gameMap_["game_movie"] = std::make_shared<GameMovie>(false /*isHost*/);
				}
			}
			else
			{
				Log::error() << "Failed to read the game name";
			}
		}
		else
		{
			Log::error() << "Received invalid TCP data";
		}
	}
	else
	{
		const std::string inputString((const char*)(data), size);

		std::istringstream stringStream(inputString, std::ios::binary);
		IO::InputBitstream bitstream(stringStream);

		unsigned long long tag = 0ull;
		bitstream.look<unsigned long long>(tag);

		if (tag == transformationTag_)
		{
			HomogenousMatrix4 headsetWorld_T_trackerWorld(false);
			if (readTransformationFromStream(bitstream, headsetWorld_T_trackerWorld))
			{
				const ScopedLock scopedLock(lock_);

				headsetWorld_T_trackerWorld_ = headsetWorld_T_trackerWorld;
			}
		}
		else
		{
			Log::error() << "Recived invalid UDP data";
		}
	}
}

MetaportationExperience::MapCreatorThread::MapCreatorThread(const InputData& inputData, MapData& mapData) :
	inputData_(inputData),
	mapData_(mapData)
{
	startThread();
}

void MetaportationExperience::MapCreatorThread::threadRun()
{
	Tracking::MapBuilding::MultiViewMapCreator multiViewMapCreator;

	Timestamp lastTimestamp(false);

	std::shared_ptr<Frames> yFrames;
	SharedAnyCameras cameras;
	HomogenousMatrix4 world_T_device;
	HomogenousMatrices4 device_T_cameras;

	double mapExtractionInterval = 1.0;
	Timestamp nextMapExtractionTimestamp(false);

	HighPerformanceStatistic performance;

	while (!shouldThreadStop())
	{
		if (!inputData_.latestInputData(lastTimestamp, yFrames, cameras, world_T_device, device_T_cameras))
		{
			sleep(1u);
			continue;
		}

		performance.start();

#if 1
		IndexPair32 stereoCameraIndices;
		if (Tracking::MapBuilding::MultiViewMapCreator::determineLowerStereoCameras(device_T_cameras, stereoCameraIndices))
		{
			const Frames yFramesSubset =
			{
				Frame((*yFrames)[stereoCameraIndices.first], Frame::ACM_USE_KEEP_LAYOUT),
				Frame((*yFrames)[stereoCameraIndices.second], Frame::ACM_USE_KEEP_LAYOUT)
			};

			const SharedAnyCameras camerasSubset =
			{
				cameras[stereoCameraIndices.first],
				cameras[stereoCameraIndices.second],
			};

			const HomogenousMatrices4 device_T_camerasSubset =
			{
				device_T_cameras[stereoCameraIndices.first],
				device_T_cameras[stereoCameraIndices.second]
			};

			multiViewMapCreator.processFrame(yFramesSubset, camerasSubset, world_T_device, device_T_camerasSubset);
		}
#else
		multiViewMapCreator.processFrame(yFrames, cameras, world_T_device, device_T_cameras);
#endif

		performance.stop();

		if (performance.measurements() % 100u == 0u)
		{
			Log::info() << "Map creator: " << performance.averageMseconds() << "ms, " << performance.lastMseconds() << "ms";
		}

		if (lastTimestamp >= nextMapExtractionTimestamp)
		{
			Vectors3 objectPoints;
			Scalars objectPointStabilityFactors;
			std::vector<CV::Detector::FREAKDescriptors32> multiDescriptors;

			if (multiViewMapCreator.latestFeatureMap(objectPoints, &multiDescriptors, &objectPointStabilityFactors, 20, 20))
			{
				mapData_.updateMapData(std::move(objectPoints), std::move(objectPointStabilityFactors), std::move(multiDescriptors));
			}

			nextMapExtractionTimestamp = lastTimestamp + mapExtractionInterval;

			mapExtractionInterval = std::min(mapExtractionInterval * 1.15, 5.0);
		}
	}
}

MetaportationExperience::MapHandlingThread::MapHandlingThread(PortalCreator& portalCreator, MapData& mapData) :
	owner_(portalCreator),
	mapData_(mapData)
{
	startThread();
}

void MetaportationExperience::MapHandlingThread::threadRun()
{
	RandomGenerator randomGenerator;

	while (!shouldThreadStop())
	{
		Vectors3 objectPoints;
		Scalars objectPointStabilityFactors;
		std::vector<CV::Detector::FREAKDescriptors32> multiDescriptors;

		if (!mapData_.latestMapData(objectPoints, objectPointStabilityFactors, multiDescriptors))
		{
			sleep(1u);
			continue;
		}

		const Indices32 objectPointIds = createIndices<Index32>(objectPoints.size(), 0u);

		Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap descriptorMap;
		descriptorMap.reserve(objectPoints.size() * 3 / 2);

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			descriptorMap.emplace(objectPointIds[n], std::move(multiDescriptors[n]));
		}

		const std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> unifiedDescriptorMap = std::make_shared<Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256>(std::move(descriptorMap));

		using ImagePointDescriptor = Tracking::MapBuilding::UnifiedDescriptor::FreakMultiDescriptor256;
		using ObjectPointDescriptor = Tracking::MapBuilding::UnifiedDescriptor::FreakMultiDescriptors256;
		using ObjectPointVocabularyDescriptor = Tracking::MapBuilding::UnifiedDescriptor::BinaryDescriptor<256u>;

		using UnifiedFeatureMap = Tracking::MapBuilding::UnifiedFeatureMapT<ImagePointDescriptor, ObjectPointDescriptor, ObjectPointVocabularyDescriptor>;

		mapData_.updateFeatureMap(std::make_shared<UnifiedFeatureMap>(Vectors3(objectPoints), Indices32(objectPointIds), std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap>(unifiedDescriptorMap), randomGenerator, &UnifiedFeatureMap::VocabularyForest::TVocabularyTree::determineClustersMeanForBinaryDescriptor<256u>, &Tracking::MapBuilding::UnifiedHelperFreakMultiDescriptor256::extractVocabularyDescriptorsFromMap));

		if (owner_.tcpClient_.isConnected())
		{
			std::ostringstream stringStream(std::ios::binary);
			IO::OutputBitstream bitstream(stringStream);

			if (bitstream.write<unsigned long long>(mapTag_)
					&& writeObjectPointsToStream(objectPoints, objectPointIds, bitstream)
					&& Tracking::MapBuilding::Utilities::writeDescriptorMap(*unifiedDescriptorMap, bitstream))
			{
				const std::string mapData = stringStream.str();
				ocean_assert(!mapData.empty());

				IO::Compression::Buffer compressedMapData;
				if (IO::Compression::gzipCompress(mapData.c_str(), mapData.size(), compressedMapData))
				{
					if (owner_.tcpClient_.send(compressedMapData.data(), compressedMapData.size()) == Network::Socket::SR_SUCCEEDED)
					{
						owner_.renderingText_->setText("Connection started");

						continue;
					}
				}

				owner_.renderingText_->setText("Failed to send mesh");
			}
		}
	}
}

MetaportationExperience::~MetaportationExperience()
{
	// nothing to do here
}

bool MetaportationExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	renderingTransformMesh_ = engine->factory().createTransform();
	experienceScene()->addChild(renderingTransformMesh_);

	renderingTextGroup_ = engine->factory().createGroup();
	renderingTransformMesh_->addChild(renderingTextGroup_);

	vrTableMenu_ = VRTableMenu(engine, engine->framebuffers().front());

	userId_ = PlatformSDK::Manager::get().userId(); // id of the currently logged in user

	if (userId_ != 0ull)
	{
		if (Avatars::Manager::get().loadUser(userId_))
		{
			Log::info() << "Local user loaded: " << userId_;

			avatarScopedSubscriptions_.emplace_back(Avatars::Manager::get().createAvatar(userId_, std::bind(&MetaportationExperience::onAvatarCreated, this, std::placeholders::_1, std::placeholders::_2)));
		}
	}
	else
	{
		Log::error() << "Failed to determine local user";
	}

	return true;
}

bool MetaportationExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	avatarScopedSubscriptions_.clear();
	zoneScopedSubscription_.release();

	if (portalCreator_)
	{
		portalCreator_->release();
		portalCreator_ = nullptr;
	}

	if (portalLoader_)
	{
		portalLoader_->release();
		portalLoader_ = nullptr;
	}

	if (portalReceiver_)
	{
		portalReceiver_->release();
		portalReceiver_ = nullptr;
	}

	vrTableMenu_.release();
	renderingTextGroup_.release();
	renderingTransformRemoteAvatar_.release();
	renderingTransformMesh_.release();

	return true;
}

Timestamp MetaportationExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	if (applicationState_ == AS_IDLE)
	{
		VRTableMenu::Entries menuEntries =
		{
			VRTableMenu::Entry("Create and share your room", "CREATE_AND_SHARE_ROOM"),
			VRTableMenu::Entry("Wait to join a room", "JOIN_ROOM"),
		};

		VRTableMenu::Groups menuGroups(1, VRTableMenu::Group("What do you want to do?", std::move(menuEntries)));

		ocean_assert(Platform::Android::ResourceManager::get().isValid());
		const IO::Directories roomDirectories = (IO::Directory(Platform::Android::ResourceManager::get().externalFilesDirectory()) + IO::Directory("metaportation/rooms/")).findDirectories();

		if (!roomDirectories.empty())
		{
			menuEntries = VRTableMenu::Entries();

			for (const IO::Directory& roomDirectory : roomDirectories)
			{
				menuEntries.emplace_back(roomDirectory.name(), "LOAD_AND_SHARE_ROOM_" + roomDirectory.name());
			}

			menuGroups.emplace_back("Or load and share a stored room?", std::move(menuEntries));
		}

		vrTableMenu_.setMenuEntries(menuGroups);

		const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(timestamp);

		if (world_T_device.isValid())
		{
			vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, -1)), world_T_device);

			applicationState_ = AS_USER_SELECTING_MODE;
		}
	}
	else if (applicationState_ == AS_USER_SELECTING_MODE)
	{
		ocean_assert(vrTableMenu_.isShown());

		std::string entryUrl;
		if (vrTableMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
		{
			vrTableMenu_.hide();

			if (entryUrl == "CREATE_AND_SHARE_ROOM")
			{
				portalCreator_ = std::make_shared<PortalCreator>();
				portalCreator_->initialize(renderingTransformMesh_, engine, timestamp);

				PlatformSpecific::get().mrPassthroughVisualizer().resumePassthrough();

				PlatformSDK::Room::get().createAndJoin();

				applicationState_ = AS_SHARE_ROOM_INVITE;
			}
			else if (entryUrl.find("LOAD_AND_SHARE_ROOM_") == 0)
			{
				const std::string roomName = entryUrl.substr(20);

				ocean_assert(Platform::Android::ResourceManager::get().isValid());
				const IO::Directory roomDirectory = IO::Directory(Platform::Android::ResourceManager::get().externalFilesDirectory()) + IO::Directory("metaportation/rooms/" + roomName);
				ocean_assert(roomDirectory.exists());

				portalLoader_ = std::make_shared<PortalLoader>();
				portalLoader_->initialize(renderingTransformMesh_, engine, timestamp, roomDirectory);

				PlatformSpecific::get().mrPassthroughVisualizer().resumePassthrough();

				PlatformSDK::Room::get().createAndJoin();

				applicationState_ = AS_SHARE_ROOM_INVITE;
			}
			else
			{
				ocean_assert(entryUrl == "JOIN_ROOM");

				renderingTextGroup_->clear();

				const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, " Wait for an invite ", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE);
				textTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -2)));
				renderingTextGroup_->addChild(textTransform);

				applicationState_ = AS_JOIN_ROOM_WAIT_FOR_INVITE;
			}
		}
	}
	else if (applicationState_ == AS_SHARE_ROOM_INVITE)
	{
		if (PlatformSDK::Room::get().hasLatestCreateAndJoinResult(localRoomId_))
		{
			if (localRoomId_ != 0ull)
			{
				Log::info() << "Created room id: " << localRoomId_;

				PlatformSDK::Room::get().invitableUsers(localRoomId_);
			}
			else
			{
				Log::error() << "Failed to create room";
			}
		}

		PlatformSDK::Room::Users invitableUsers;
		if (PlatformSDK::Room::get().hasLatestInvitableUsersResult(invitableUsers))
		{
			Log::info() << "Number of invitable users: " << invitableUsers.size();

			PlatformSDK::Room::get().launchInvitableUserFlow(localRoomId_);
		}

		PlatformSDK::Room::Users joinedUsers;
		if (PlatformSDK::Room::get().hasLatestUsersResult(joinedUsers))
		{
			Log::info() << "Users in the room: " << joinedUsers.size();

			static UnorderedIndexSet64 remoteUsersInRoom;

			for (const PlatformSDK::Room::User& joinedUser : joinedUsers)
			{
				Log::info() << joinedUser.userId() << ", " << joinedUser.oculusId();

				if (joinedUser.userId() != userId_)
				{
					if (remoteUsersInRoom.find(joinedUser.userId()) == remoteUsersInRoom.cend())
					{
						ocean_assert(remoteUserId_ == 0ull);
						remoteUserId_ = joinedUser.userId();

						if (portalCreator_)
						{
							portalCreator_->setRemoteUserId(remoteUserId_);
						}

						if (portalLoader_)
						{
							portalLoader_->setRemoteUserId(remoteUserId_);
						}

						zoneScopedSubscription_ = Avatars::Manager::get().joinZone(roomZoneName(localRoomId_));

						avatarScopedSubscriptions_.emplace_back(Avatars::Manager::get().createAvatar(remoteUserId_, std::bind(&MetaportationExperience::onAvatarCreated, this, std::placeholders::_1, std::placeholders::_2)));

						remoteUsersInRoom.emplace(remoteUserId_);

						applicationState_ = AS_SHARE_ROOM_CONNECT_WITH_PHONE; // **TODO**
					}
				}
			}
		}
	}
	else if (applicationState_ == AS_JOIN_ROOM_WAIT_FOR_INVITE)
	{
		PlatformSDK::Room::Invite invite;
		if (PlatformSDK::Room::get().hasLatestInviteResult(invite))
		{
			ocean_assert(invite.userId() != 0ull);
			Log::info() << "Received invite from user: " << invite.userId();

			remoteUserId_ = invite.userId();
			remoteRoomId_ = invite.roomId();

			const VRTableMenu::Group menuGroup("Invite from " + String::toAString(invite.userId()), {VRTableMenu::Entry("Accept invite", "ACCEPT_INVITE"), VRTableMenu::Entry("Reject invite", "REJECT_INVITE")});

			vrTableMenu_.setMenuEntries(menuGroup);
			vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, -1)));
		}

		if (vrTableMenu_.isShown())
		{
			std::string entryUrl;
			if (vrTableMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
			{
				vrTableMenu_.hide();

				if (entryUrl == "ACCEPT_INVITE")
				{
					Log::info() << "User accepted to join the portal: " << remoteRoomId_;

					ocean_assert(remoteUserId_ != 0ull);
					PlatformSDK::Room::get().joinRoom(remoteRoomId_);

					applicationState_ = AS_JOIN_ROOM_ACCEPTED_INVITE;
				}
				else
				{
					Log::info() << "User rejected to join the portal";

					remoteUserId_ = 0ull;
					remoteRoomId_ = 0ull;

					applicationState_ = AS_IDLE;
				}
			}
		}
	}
	else if (applicationState_ == AS_JOIN_ROOM_ACCEPTED_INVITE)
	{
		bool roomJoined;
		if (PlatformSDK::Room::get().hasLatestJoinRoomResult(roomJoined))
		{
			if (roomJoined)
			{
				Log::info() << "User has joind the room " << remoteRoomId_;

				ocean_assert(remoteUserId_ != 0ull);

				zoneScopedSubscription_ = Avatars::Manager::get().joinZone(roomZoneName(remoteRoomId_));

				avatarScopedSubscriptions_.emplace_back(Avatars::Manager::get().createAvatar(remoteUserId_, std::bind(&MetaportationExperience::onAvatarCreated, this, std::placeholders::_1, std::placeholders::_2)));

				renderingTextGroup_->clear();

				ocean_assert(!portalReceiver_);
				portalReceiver_ = std::make_shared<PortalReceiver>();

				portalReceiver_->initialize(renderingTransformMesh_, remoteUserId_, engine, timestamp);

				/// we place the receiver 1.5meter in front of the creator
				renderingTransformMesh_->setTransformation(HomogenousMatrix4(Vector3(0, 0, -1.5), Quaternion(Vector3(0, 1, 0), Numeric::pi())));
			}
			else
			{
				Log::info() << "User failed to join the room";
			}
		}
	}

	if (portalCreator_)
	{
		portalCreator_->preUpdate( engine, timestamp);
	}

	if (portalLoader_)
	{
		portalLoader_->preUpdate( engine, timestamp);
	}

	if (portalReceiver_)
	{
		handleUserMovement(timestamp);

		// we need to tell the Avatar system that we may have moved based on the controller
		const HomogenousMatrix4 avatar_T_headsetWorld = renderingTransformMesh_->transformation();
		Avatars::Manager::get().setRemoteHeadsetWorld_T_remoteAvatar(avatar_T_headsetWorld.inverted());

		portalReceiver_->preUpdate(engine, timestamp);
	}

	return timestamp;
}

void MetaportationExperience::onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	if (portalCreator_)
	{
		portalCreator_->onKeyPress(userInterface, engine, key, timestamp);
	}

	if (portalLoader_)
	{
		portalLoader_->onKeyPress(userInterface, engine, key, timestamp);
	}

	if (portalReceiver_)
	{
		portalReceiver_->onKeyPress(userInterface, engine, key, timestamp);
	}
}

void MetaportationExperience::onKeyRelease(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	if (portalCreator_)
	{
		portalCreator_->onKeyRelease(userInterface, engine, key, timestamp);
	}

	if (portalLoader_)
	{
		portalLoader_->onKeyRelease(userInterface, engine, key, timestamp);
	}

	if (portalReceiver_)
	{
		portalReceiver_->onKeyRelease(userInterface, engine, key, timestamp);
	}
}

std::unique_ptr<XRPlaygroundExperience> MetaportationExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new MetaportationExperience());
}

void MetaportationExperience::handleUserMovement(const Timestamp& timestamp)
{
	// let's see whether the user want's to move

	const Vector2 joystickTiltLeft = PlatformSpecific::get().trackedRemoteDevice().joystickTilt(TrackedRemoteDevice::RT_LEFT);
	const Vector2 joystickTiltRight = PlatformSpecific::get().trackedRemoteDevice().joystickTilt(TrackedRemoteDevice::RT_RIGHT);

	if (joystickTiltLeft.sqr() > joystickTiltRight.sqr())
	{
		// left joystick supports smoothly flying through the space

		const Vector2& joystickTilt = joystickTiltLeft;

		const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(timestamp);

		// create a pitch/roll-free device transformation

		const Vector3 yAxis(0, 1, 0);

		Vector3 zAxis = world_T_device.xAxis().cross(yAxis);
		Vector3 xAxis = yAxis.cross(zAxis);

		if (zAxis.normalize() && xAxis.normalize())
		{
			const Quaternion world_T_deviceYaw = HomogenousMatrix4(xAxis, yAxis, zAxis).rotation();

			const Vector3 translationDevice(-joystickTilt.x(), 0, joystickTilt.y());

			HomogenousMatrix4 worldHeadset_T_worldPortal = renderingTransformMesh_->transformation();
			worldHeadset_T_worldPortal.setTranslation(worldHeadset_T_worldPortal.translation() + world_T_deviceYaw * translationDevice * Scalar(0.01));

			renderingTransformMesh_->setTransformation(worldHeadset_T_worldPortal);
		}
	}
	else if (jumpStartTimestamp_.isInvalid())
	{
		// right controller supports jump movements (to address motion sickness)

		const Vector2& joystickTilt = joystickTiltRight;

		if (Numeric::abs(joystickTilt.x()) > Scalar(0.6) || Numeric::abs(joystickTilt.y()) > Scalar(0.6))
		{
			const HomogenousMatrix4 world_T_device = HeadsetPoses::world_T_device(PlatformSpecific::get().ovr(), timestamp);

			// create a pitch/roll-free device transformation

			const Vector3 yAxis(0, 1, 0);

			Vector3 zAxis = world_T_device.xAxis().cross(yAxis);
			Vector3 xAxis = yAxis.cross(zAxis);

			if (zAxis.normalize() && xAxis.normalize())
			{
				const Quaternion world_T_deviceYaw = HomogenousMatrix4(xAxis, yAxis, zAxis).rotation();

				Vector3 translationDevice(-joystickTilt.x(), 0, joystickTilt.y());
				translationDevice.normalize();

				HomogenousMatrix4 worldHeadset_T_worldPortal = renderingTransformMesh_->transformation();
				worldHeadset_T_worldPortal.setTranslation(worldHeadset_T_worldPortal.translation() + world_T_deviceYaw * translationDevice * Scalar(1)); // make 1 meter jumps

				renderingTransformMesh_->setTransformation(worldHeadset_T_worldPortal);

				jumpStartTimestamp_ = timestamp;

				// for the jump, we will disable mesh rendering for a short time
				experienceScene()->setVisible(false);
			}
		}
	}

	if (jumpStartTimestamp_.isValid() && timestamp > jumpStartTimestamp_ + 0.25)
	{
		experienceScene()->setVisible(true);

		if (joystickTiltLeft.isNull() && joystickTiltRight.isNull())
		{
			// allow a new jump movement
			jumpStartTimestamp_.toInvalid();
		}
	}
}

void MetaportationExperience::onAvatarCreated(const uint64_t userId, Rendering::TransformRef avatarTransform)
{
	if (avatarTransform)
	{
		if (userId == userId_)
		{
			PlatformSpecific::get().vrControllerVisualizer().hide();
			PlatformSpecific::get().vrHandVisualizer().hide();

			experienceScene()->addChild(avatarTransform);
		}
		else
		{
			renderingTransformRemoteAvatar_ = avatarTransform;

			renderingTransformMesh_->addChild(renderingTransformRemoteAvatar_);
		}
	}
	else
	{
		Log::error() << "Failed to create rendering instance for user " << userId;
	}
}

bool MetaportationExperience::detectQRCodeWithAddress(const InputData& inputData, Network::Address4& address, Network::Port& port)
{
	Timestamp timestamp(false);

	std::shared_ptr<Frames> yFrames;
	SharedAnyCameras cameras;
	HomogenousMatrix4 world_T_device;
	HomogenousMatrices4 device_T_cameras;

	if (!inputData.latestInputData(timestamp, yFrames, cameras, world_T_device, device_T_cameras))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	IndexPair32 stereoCameraIndices;
	if (!Tracking::MapBuilding::MultiViewMapCreator::determineLowerStereoCameras(device_T_cameras, stereoCameraIndices))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	const unsigned int index = RandomI::random(1u) == 0u ? stereoCameraIndices.first : stereoCameraIndices.second;

	const Frame& yFrame = (*yFrames)[index];
	const AnyCamera& camera = *cameras[index];

	const CV::Detector::QRCodes::QRCodes codes = CV::Detector::QRCodes::QRCodeDetector2D::detectQRCodes(camera, yFrame);

	for (const CV::Detector::QRCodes::QRCode& code : codes)
	{
		if (code.encodingMode() == CV::Detector::QRCodes::QRCode::EM_BYTE && code.data().size() == 8)
		{
			const std::vector<uint8_t>& payload = code.data();

			if (payload[0] == uint8_t('M') && payload[1] == uint8_t('P'))
			{
				unsigned int addressValue = 0u;
				unsigned short portValue = 0u;

				static_assert(sizeof(address) + sizeof(port) == 6, "Invalid data type!");

				memcpy(&addressValue, payload.data() + 2, sizeof(addressValue));
				memcpy(&portValue, payload.data() + 6, sizeof(portValue));

				address = Network::Address4(addressValue, Network::Address4::TYPE_BIG_ENDIAN);
				port = Network::Port(portValue, Network::Address4::TYPE_BIG_ENDIAN);

				return true;
			}
		}

		Log::info() << "Detected not compatible QR Code.";
	}

	return false;
}

std::string MetaportationExperience::roomZoneName(const uint64_t roomId)
{
	ocean_assert(roomId != 0ull);

	return "XRPlayground://METAPORTATION_ROOM_" + String::toAString(roomId);
}

#else // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

MetaportationExperience::~MetaportationExperience()
{
	// nothing to do here
}

std::unique_ptr<XRPlaygroundExperience> MetaportationExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new MetaportationExperience());
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

}

}
