// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/EnvironmentRendererExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/EnvironmentScannerExperience.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/Tracker6DOF.h"

#include "ocean/geometry/AbsoluteTransformation.h"

#include "ocean/io/Compression.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/network/tigon/TigonClient.h"

#include "ocean/network/verts/Manager.h"

#include "ocean/rendering/Utilities.h"

#include "ocean/tracking/mapbuilding/RelocalizerStereo.h"
#include "ocean/tracking/mapbuilding/Utilities.h"

namespace Ocean
{

using namespace Network;

namespace XRPlayground
{

EnvironmentRendererExperience::~EnvironmentRendererExperience()
{
	// nothing to do here
}

bool EnvironmentRendererExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, " Start Environment Scanner \n experience on your phone ", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 2, 0, 0, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingText_);
	textTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -5)));

	experienceScene()->addChild(textTransform);

	renderingTransform_ = engine->factory().createTransform();
	experienceScene()->addChild(renderingTransform_);

	renderingTransformDevice_ = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.15), Scalar(0.075), Scalar(0.0075)), RGBAColor(0.7f, 0.7f, 0.7f));
	renderingTransformDevice_->setVisible(false);

	renderingTransform_->addChild(renderingTransformDevice_);

	headsetWorld_T_mapWorld_.setTransformation(HomogenousMatrix4(true), timestamp);

	// we need to determine a zone name which is unique for the user (user id does not work as user may be logged in with individual ids like Meta, Facebook, Oculus)

	std::string userName;
	if (!Network::Tigon::TigonClient::get().determineUserName(userName))
	{
		renderingText_->setText(" Failed to determine user name \n User needs to be logged in ");
		return true;
	}

	const std::string vertsZoneName = "XRPlayground://ENVIRONMENT_ZONE_FOR_" + String::toAString(std::hash<std::string>()(userName));

	vertsDriver_ = Verts::Manager::get().driver(vertsZoneName);

	if (vertsDriver_)
	{
		newEntityScopedSubscription_ = vertsDriver_->addNewEntityCallback(std::bind(&EnvironmentRendererExperience::onNewEntity, this, std::placeholders::_1, std::placeholders::_2), "DevicePose");
		changedUsersScopedSubscription_ = vertsDriver_->addChangedUsersCallback(std::bind(&EnvironmentRendererExperience::onChangedUsers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		receiveContainerScopedSubscription_ = vertsDriver_->addReceiveContainerCallback(std::bind(&EnvironmentRendererExperience::onReceiveContainer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

		Log::info() << "Created VERTS driver with zone name '" << vertsZoneName << "'";
	}

	Verts::NodeSpecification& devicePoseSpecification = Verts::NodeSpecification::newNodeSpecification("DevicePose");
	devicePoseSpecification.registerField<std::string>("world_T_camera");

	startThread();

	return true;
}

bool EnvironmentRendererExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	newEntityScopedSubscription_.release();
	changedUsersScopedSubscription_.release();
	receiveContainerScopedSubscription_.release();

	stopThreadExplicitly();

	renderingText_.release();
	renderingTransform_.release();

	return true;
}

Timestamp EnvironmentRendererExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	const ScopedLock scopedLock(lock_);

	for (MeshObjectMap::iterator iMesh = meshObjectMap_.begin(); iMesh != meshObjectMap_.end(); ++iMesh)
	{
		MeshObject& meshObject = iMesh->second;

		if (!meshObject.hasChanged_)
		{
			continue;
		}

		Rendering::TriangleFaces triangleFaces;
		triangleFaces.reserve(meshObject.triangleIndices_.size() / 3);

		for (size_t n = 0; n < meshObject.triangleIndices_.size() / 3; ++n)
		{
			const Index32& index0 = meshObject.triangleIndices_[n * 3 + 0];
			const Index32& index1 = meshObject.triangleIndices_[n * 3 + 1];
			const Index32& index2 = meshObject.triangleIndices_[n * 3 + 2];

			ocean_assert(index0 < meshObject.vertices_.size());
			ocean_assert(index1 < meshObject.vertices_.size());
			ocean_assert(index2 < meshObject.vertices_.size());

			triangleFaces.emplace_back(index0, index1, index2);
		}

		if (meshObject.renderingTransform_)
		{
			renderingTransform_->removeChild(meshObject.renderingTransform_);
			meshObject.renderingTransform_.release();
		}

		if (renderMesh_)
		{
			meshObject.renderingTransform_ = Rendering::Utilities::createMesh(engine, meshObject.vertices_, triangleFaces, RGBAColor(0.7f, 0.7f, 0.7f), meshObject.perVertexNormals_);
		}
		else
		{
			meshObject.renderingTransform_ = Rendering::Utilities::createPoints(*engine, meshObject.vertices_, RGBAColor(0.0f, 0.0f, 1.0f), 2);
		}

		meshObject.renderingTransform_->setTransformation(meshObject.world_T_mesh_);

		renderingTransform_->addChild(meshObject.renderingTransform_);

		meshObject.hasChanged_ = false;
	}

	if (vertsDevicePoseNode_ && vertsDevicePoseNode_->hasChanged())
	{
		const Verts::Node::StringPointer data = vertsDevicePoseNode_->field<Verts::Node::StringPointer>("world_T_camera");

		if (data)
		{
			ocean_assert(data.size() == sizeof(HomogenousMatrixD4));
			if (data.size() == sizeof(HomogenousMatrixD4))
			{
				double values[16];
				memcpy(values, data.data(), sizeof(double) * 16);

				const HomogenousMatrixD4 world_T_recentCamera(values);

				if (world_T_recentCamera.isValid())
				{
					renderingTransformDevice_->setTransformation(HomogenousMatrix4(world_T_recentCamera) * HomogenousMatrix4(Vector3(Scalar(0.065), Scalar(-0.02), 0)));
					renderingTransformDevice_->setVisible(true);
				}
			}
		}
	}

	renderingTransform_->setTransformation(headsetWorld_T_mapWorld_.transformation(timestamp));

	return timestamp;
}

void EnvironmentRendererExperience::onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	const ScopedLock scopedLock(lock_);

	// we toggle between mesh and point rendering

	renderMesh_ = !renderMesh_;

	for (MeshObjectMap::iterator iMesh = meshObjectMap_.begin(); iMesh != meshObjectMap_.end(); ++iMesh)
	{
		iMesh->second.hasChanged_ = true;
	}
}

std::unique_ptr<XRPlaygroundExperience> EnvironmentRendererExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new EnvironmentRendererExperience());
}

void EnvironmentRendererExperience::onNewEntity(Network::Verts::Driver& driver, const Network::Verts::SharedEntity& entity)
{
	vertsDevicePoseNode_ = entity->node("DevicePose");
	ocean_assert(vertsDevicePoseNode_);
}

void EnvironmentRendererExperience::onChangedUsers(Network::Verts::Driver& driver, const UnorderedIndexSet64& addedUsers, const UnorderedIndexSet64& removedUsers)
{
	const size_t usersBefore = userIds_.size();

	for (const Index64 userId : addedUsers)
	{
		userIds_.emplace(userId);
	}

	for (const Index64 userId : removedUsers)
	{
		userIds_.erase(userId);
	}

	if (usersBefore == 0 && !userIds_.empty())
	{
		renderingText_->setText("");
	}
	else if (usersBefore != 0 && userIds_.empty())
	{
		renderingText_->setText(" Start Environment Scanner \n experience on your phone ");
	}
}

void EnvironmentRendererExperience::onReceiveContainer(Network::Verts::Driver& driver, const uint32_t sessionId, const uint64_t userId, const std::string& identifier, const unsigned int version, const Network::Verts::Driver::SharedBuffer& buffer)
{
	bool decodingSucceeded = false;

	std::vector<uint8_t> decompressedBuffer;
	if (IO::Compression::gzipDecompress(buffer->data(), buffer->size(), decompressedBuffer))
	{
		const std::string inputString((const char*)(decompressedBuffer.data()), decompressedBuffer.size());

		std::istringstream stringStream(inputString, std::ios::binary);
		IO::InputBitstream bitstream(stringStream);

		if (identifier == "map")
		{
			Vectors3 mapObjectPoints;
			Indices32 mapObjectPointIds;
			std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> unifiedDescriptorMap;

			unsigned long long mapTag = 0u;
			if (bitstream.read<unsigned long long>(mapTag) && mapTag == EnvironmentScannerExperience::mapTag_
					&& EnvironmentScannerExperience::readObjectPointsFromStream(bitstream, mapObjectPoints, mapObjectPointIds)
					&& Tracking::MapBuilding::Utilities::readDescriptorMap(bitstream, unifiedDescriptorMap))
			{
				decodingSucceeded = true;

				const ScopedLock scopedLock(lock_);

				mapObjectPoints_ = std::move(mapObjectPoints);
				mapObjectPointIds_ = std::move(mapObjectPointIds);
				unifiedDescriptorMap_ = std::move(unifiedDescriptorMap);
			}
			else
			{
				Log::error() << "Failed to decode map";
			}
		}
		else if (identifier == "mesh")
		{
			unsigned int numberMeshes = 0u;
			if (bitstream.read<unsigned int>(numberMeshes))
			{
				for (unsigned int n = 0u; n < numberMeshes; ++n)
				{
					Index32 meshId = Index32(-1);
					HomogenousMatrix4 world_T_mesh(false);
					Vectors3 vertices;
					Vectors3 perVertexNormals;
					Indices32 triangleIndices;

					if (EnvironmentScannerExperience::readMeshFromStream(bitstream, meshId, world_T_mesh, vertices, perVertexNormals, triangleIndices))
					{
						decodingSucceeded = true;

						const ScopedLock scopedLock(lock_);

						MeshObject& meshObject = meshObjectMap_[meshId];

						meshObject.world_T_mesh_ = world_T_mesh;
						meshObject.vertices_ = std::move(vertices);
						meshObject.perVertexNormals_ = std::move(perVertexNormals);
						meshObject.triangleIndices_ = std::move(triangleIndices);
						meshObject.hasChanged_ = true;
					}
					else
					{
						Log::error() << "Failed to decode mesh";
					}
				}
			}
		}
	}

	if (!decodingSucceeded)
	{
		renderingText_->setText("Failed to decode '" + identifier + "'");
	}
}

void EnvironmentRendererExperience::threadRun()
{
	Tracking::MapBuilding::RelocalizerStereo relocalizer(Tracking::MapBuilding::Relocalizer::detectFreakFeatures);

	Media::FrameMediumRef frameMediumA = Media::Manager::get().newMedium("StereoCamera0Id:0");
	Media::FrameMediumRef frameMediumB = Media::Manager::get().newMedium("StereoCamera0Id:1");

	if (frameMediumA.isNull() || frameMediumB.isNull()
		|| !frameMediumA->start() || !frameMediumB->start())
	{
		Log::error() << "Failed to access both cameras";
		return;
	}

	const Media::FrameMediumRefs frameMediums = {frameMediumA, frameMediumB};

	HomogenousMatrices4 headsetWorld_T_correspondingDevices;
	HomogenousMatrices4 mapWorld_T_correspondingDevices;

	const Devices::Tracker6DOFRef headsetTracker = Devices::Manager::get().device("Headset 6DOF Tracker");

	if (!headsetTracker)
	{
		Log::error() << "Failed to access the headset tracker";
		return;
	}

	headsetTracker->start();

	Timestamp lastFrameTimestamp(false);
	RandomGenerator randomGenerator;

	while (!shouldThreadStop())
	{
		TemporaryScopedLock scopedLock(lock_);
			Vectors3 mapObjectPoints = std::move(mapObjectPoints_);
			Indices32 mapObjectPointIds = std::move(mapObjectPointIds_);
			std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> unifiedDescriptorMap = std::move(unifiedDescriptorMap_);
		scopedLock.release();

		if (!mapObjectPoints.empty())
		{
			using ImagePointDescriptor = Tracking::MapBuilding::UnifiedDescriptor::FreakMultiDescriptor256;
			using ObjectPointDescriptor = Tracking::MapBuilding::UnifiedDescriptor::FreakMultiDescriptors256;
			using ObjectPointVocabularyDescriptor = Tracking::MapBuilding::UnifiedDescriptor::BinaryDescriptor<256u>;

			using UnifiedFeatureMap = Tracking::MapBuilding::UnifiedFeatureMapT<ImagePointDescriptor, ObjectPointDescriptor, ObjectPointVocabularyDescriptor>;

			relocalizer.setFeatureMap(std::make_shared<UnifiedFeatureMap>(std::move(mapObjectPoints), std::move(mapObjectPointIds), std::move(unifiedDescriptorMap), randomGenerator, &UnifiedFeatureMap::VocabularyForest::TVocabularyTree::determineClustersMeanForBinaryDescriptor<256u>, &Tracking::MapBuilding::UnifiedHelperFreakMultiDescriptor256::extractVocabularyDescriptorsFromMap));
		}

		if (!relocalizer.isValid())
		{
			sleep(1u);
			continue;
		}

		FrameRefs frames;
		SharedAnyCameras cameras;
		HomogenousMatricesD4 device_T_cameras;
		if (!Media::FrameMedium::syncedFrames(frameMediums, lastFrameTimestamp, frames, cameras, 2u, nullptr, &device_T_cameras))
		{
			continue;
		}

		ocean_assert(frames.size() == 2 && frames.size() == cameras.size() && frames.size() == device_T_cameras.size());

		const Timestamp timestamp = frames.front()->timestamp();
		lastFrameTimestamp = timestamp;

		const Devices::Tracker6DOF::Tracker6DOFSampleRef sample(headsetTracker->sample(timestamp, Devices::Tracker6DOF::IS_TIMESTAMP_INTERPOLATE));

		if (!sample || sample->objectIds().empty())
		{
			sleep(1u);

			Log::warning() << "No valid headset pose";
			continue;
		}

		const HomogenousMatrix4 headsetWorld_T_device(sample->positions().front(), sample->orientations().front());

		Frame yFrameA;
		Frame yFrameB;
		if (!CV::FrameConverter::Comfort::convert(*frames[0], FrameType(*frames[0], FrameType::FORMAT_Y8), yFrameA, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE)
				|| !CV::FrameConverter::Comfort::convert(*frames[1], FrameType(*frames[1], FrameType::FORMAT_Y8), yFrameB, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
		{
			ocean_assert(false && "This should never happen!");
			break;
		}

		constexpr unsigned int minimalNumberCorrespondences = 65u;
		constexpr Scalar maximalProjectionError = Scalar(2.5);

		HomogenousMatrix4 mapWorld_T_device(false);
		if (!relocalizer.relocalize(*cameras[0], *cameras[1], HomogenousMatrix4(device_T_cameras[0]), HomogenousMatrix4(device_T_cameras[1]), yFrameA, yFrameB, mapWorld_T_device, minimalNumberCorrespondences, maximalProjectionError))
		{
			continue;
		}

		headsetWorld_T_correspondingDevices.emplace_back(headsetWorld_T_device);
		mapWorld_T_correspondingDevices.emplace_back(mapWorld_T_device);

		HomogenousMatrix4 headsetWorld_T_mapWorld;
		Scalar scale;
		if (Geometry::AbsoluteTransformation::calculateTransformation(mapWorld_T_correspondingDevices.data(), headsetWorld_T_correspondingDevices.data(), mapWorld_T_correspondingDevices.size(), headsetWorld_T_mapWorld, Geometry::AbsoluteTransformation::ScaleErrorType::Symmetric, &scale) && scale >= Scalar(0.75) && scale <= Scalar(1.25))
		{
			headsetWorld_T_mapWorld.applyScale(Vector3(scale, scale, scale));

			scopedLock.relock(lock_);

			headsetWorld_T_mapWorld_.setTransformation(headsetWorld_T_mapWorld, timestamp);
		}

		if (headsetWorld_T_correspondingDevices.size() > 50)
		{
			const size_t index = headsetWorld_T_correspondingDevices.size() - 30; // we keep 30 transfomations

			headsetWorld_T_correspondingDevices = HomogenousMatrices4(headsetWorld_T_correspondingDevices.cbegin() + index, headsetWorld_T_correspondingDevices.cend());
			mapWorld_T_correspondingDevices = HomogenousMatrices4(mapWorld_T_correspondingDevices.cbegin() + index, mapWorld_T_correspondingDevices.cend());
		}
	}
}

}

}
