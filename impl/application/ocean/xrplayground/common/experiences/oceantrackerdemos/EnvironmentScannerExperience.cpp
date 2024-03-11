// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/EnvironmentScannerExperience.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/io/Compression.h"

#include "ocean/media/Utilities.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/tracking/mapbuilding/PatchTracker.h"
#include "ocean/tracking/mapbuilding/Utilities.h"

#include "metaonly/ocean/network/tigon/TigonClient.h"

#include "metaonly/ocean/network/verts/Manager.h"

namespace Ocean
{

using namespace Network;

namespace XRPlayground
{

bool EnvironmentScannerExperience::MeshObject::update(const SceneElementMeshes::SharedMesh& mesh, const Timestamp& currentTimestamp)
{
	bool makeUpdate = false;

	if (lastUpdateTimestamp_.isInvalid())
	{
		makeUpdate = true;
	}
	else
	{
		ocean_assert(mesh_ && mesh_->meshId() == mesh->meshId());

		if (mesh_->vertices().size() != mesh->vertices().size()
			|| mesh_->triangleIndices().size() != mesh->triangleIndices().size()
			|| mesh_->world_T_mesh() != mesh->world_T_mesh())
		{
			makeUpdate = true;
		}
	}

	if (makeUpdate)
	{
		mesh_ = mesh;

		if (color_ == RGBAColor(1.0f, 1.0f, 1.0f))
		{
			// the color has not been set, we set the color based on the initial (mean) mesh normal

			Vector3 planeNormal(0, 0, 0);

			for (const Vector3& normal : mesh_->perVertexNormals())
			{
				planeNormal += normal;
			}

			planeNormal.normalize(); // we use the normal as the plane's color
			planeNormal = planeNormal * Scalar(0.5) + Vector3(Scalar(0.5), Scalar(0.5), Scalar(0.5));

			color_ = RGBAColor(float(planeNormal.x()), float(planeNormal.y()), float(planeNormal.z()), 0.65f);
		}

		lastUpdateTimestamp_ = currentTimestamp;
	}

	return makeUpdate;
}

EnvironmentScannerExperience::MapCreatorWorker::MapCreatorWorker(const Devices::Tracker6DOFRef& tracker6DOF, const Media::FrameMediumRef& frameMedium) :
	tracker6DOF_(tracker6DOF),
	frameMedium_(frameMedium)
{
	ocean_assert(tracker6DOF_ && frameMedium_);

	if (tracker6DOF_ && frameMedium_)
	{
		constexpr size_t maximalFeaturesPerFrame = 400;
		constexpr double newFeaturesInterval = 0.1;

		constexpr bool keepUnlocatedFeatures = false;
		constexpr size_t minimalNumberObservationsPerFeature = 15;
		constexpr Scalar minimalBoxDiagonalForLocatedFeature = Scalar(0.05); // 5cm

		const Tracking::MapBuilding::PatchTracker::Options options(maximalFeaturesPerFrame, newFeaturesInterval, keepUnlocatedFeatures, minimalNumberObservationsPerFeature, minimalBoxDiagonalForLocatedFeature);

		patchTracker_ = std::make_shared<Tracking::MapBuilding::PatchTracker>(std::make_shared<Tracking::MapBuilding::UnifiedDescriptorExtractorFreakMultiDescriptor256>(), options);

		startThread();
	}
}

EnvironmentScannerExperience::MapCreatorWorker::~MapCreatorWorker()
{
	stopThreadExplicitly();
}

bool EnvironmentScannerExperience::MapCreatorWorker::latestMap(Vectors3& objectPoints, Indices32& objectPointIds, std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap>& unifiedDescriptorMap) const
{
	const ScopedLock scopedLock(lock_);

	if (!patchTracker_)
	{
		return false;
	}

	objectPoints = patchTracker_->latestObjectPoints(&objectPointIds);

	unifiedDescriptorMap = patchTracker_->unifiedDescriptorMap()->clone();

	return true;
}

void EnvironmentScannerExperience::MapCreatorWorker::threadRun()
{
	ocean_assert(tracker6DOF_ && frameMedium_ && patchTracker_);

	Index32 frameIndex = 0u;
	Timestamp lastTimestamp(false);

	constexpr unsigned int pyramidLayers = 5u;

	Tracking::MapBuilding::PatchTracker::SharedFramePyramid yPreviousFramePyramid;
	Tracking::MapBuilding::PatchTracker::SharedFramePyramid yCurrentFramePyramid;

	while (!shouldThreadStop())
	{
		const Devices::Tracker6DOF::Tracker6DOFSampleRef sample(tracker6DOF_->sample()); // get the latest tracker sample

		if (!sample || sample->timestamp() <= lastTimestamp)
		{
			sleep(1u);
			continue;
		}

		SharedAnyCamera frameAnyCamera;
		const FrameRef frameRef = frameMedium_->frame(sample->timestamp(), &frameAnyCamera);

		if (!frameRef || !frameAnyCamera || frameRef->timestamp() != sample->timestamp())
		{
			sleep(1u);
			continue;
		}

		lastTimestamp = sample->timestamp();

		if (sample->objectIds().empty())
		{
			continue;
		}

		const HomogenousMatrix4 world_T_camera(sample->positions().front(), sample->orientations().front());

		Frame yFrame;
		if (!CV::FrameConverter::Comfort::convert(*frameRef, FrameType(*frameRef, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		while (yFrame.width() * yFrame.height() > 640u * 480u / 2u)
		{
			CV::FrameShrinker::downsampleByTwo11(yFrame);
		}

		SharedAnyCamera anyCamera = frameAnyCamera->clone();

		if (yFrame.width() != anyCamera->width() || yFrame.height() != anyCamera->height())
		{
			anyCamera = frameAnyCamera->clone(yFrame.width(), yFrame.height());

			if (!anyCamera)
			{
				ocean_assert(false && "Failed to scale camera profile!");
				break;
			}
		}

		if (!yCurrentFramePyramid)
		{
			yCurrentFramePyramid = std::make_shared<CV::FramePyramid>();
		}

		yCurrentFramePyramid->replace8BitPerChannel(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.channels(), yFrame.pixelOrigin(), pyramidLayers, yFrame.paddingElements(), nullptr, yFrame.timestamp());

		TemporaryScopedLock scopedLock(lock_);
			patchTracker_->trackFrame(frameIndex++, *anyCamera, world_T_camera, yCurrentFramePyramid, sample->timestamp());
		scopedLock.release();

		if (yPreviousFramePyramid.use_count() == 1)
		{
			// nobody is using the previous pyramid anymore
			std::swap(yPreviousFramePyramid, yCurrentFramePyramid);
		}
		else
		{
			yCurrentFramePyramid = nullptr;
		}
	}
}

EnvironmentScannerExperience::~EnvironmentScannerExperience()
{
	// nothing to do here
}

bool EnvironmentScannerExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
	sceneTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Scene Tracker");
#endif

	if (!sceneTracker6DOF_)
	{
		Log::error() << "Scene Tracker Experience could not access the scene tracker";
		return false;
	}

	const Devices::VisualTrackerRef visualTracker(sceneTracker6DOF_);
	if (visualTracker && !engine->framebuffers().empty())
	{
		const Rendering::ViewRef view = engine->framebuffers().front()->view();

		if (view && view->background())
		{
			const Rendering::UndistortedBackgroundRef undistortedBackground(view->background());

			const Media::FrameMediumRef frameMedium(undistortedBackground->medium());

			if (frameMedium)
			{
				visualTracker->setInput(frameMedium);

				frameMedium_ = frameMedium;
			}
		}
	}

	sceneTrackerSampleEventSubscription_ = sceneTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &EnvironmentScannerExperience::onSceneTrackerSample));

	if (!anchoredContentManager_.initialize(std::bind(&EnvironmentScannerExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	const Rendering::AbsoluteTransformRef absoluteTransform = engine->factory().createAbsoluteTransform();
	absoluteTransform->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP); // the head-up transformation allows to place content as "head-up display"
	absoluteTransform->setHeadUpRelativePosition(Vector2(Scalar(0.5), Scalar(0.065)));
	experienceScene()->addChild(absoluteTransform);

	const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, " Start Environment Renderer \n experience on your Quest ", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, Scalar(0.005), 0, 0, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingText_);

	absoluteTransform->addChild(textTransform);

	if (frameMedium_.isNull())
	{
		ocean_assert(false && "This should never happen!");

		renderingText_->setText("Failed to access camera stream");
	}

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
		changedUsersScopedSubscription_ = vertsDriver_->addChangedUsersCallback(std::bind(&EnvironmentScannerExperience::onChangedUsers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

		Log::info() << "Created VERTS driver with zone name '" << vertsZoneName << "'";
	}

	Verts::NodeSpecification& devicePoseSpecification = Verts::NodeSpecification::newNodeSpecification("DevicePose");
	devicePoseSpecification.registerField<std::string>("world_T_camera");

	return true;
}

bool EnvironmentScannerExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	stopScanning();
	joinThread();

	changedUsersScopedSubscription_.release();

	sceneTrackerSampleEventSubscription_.release();

	renderingGroup_.release();

	anchoredContentManager_.release();

	sceneTracker6DOF_.release();

	return true;
}

Timestamp EnvironmentScannerExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock temporaryScopedLock(lock_);
		const Devices::SceneTracker6DOF::ObjectId objectId(objectId_);
	temporaryScopedLock.release();

	if (!renderingGroup_ && objectId != Devices::SceneTracker6DOF::invalidObjectId())
	{
		renderingGroup_ = engine->factory().createGroup();

		constexpr Scalar visibilityRadius = Scalar(1000); // 1km
		constexpr Scalar engagementRadius = Scalar(10000);
		anchoredContentManager_.addContent(renderingGroup_, sceneTracker6DOF_, objectId, visibilityRadius, engagementRadius);
	}

	const Verts::SharedNode vertsDevicePoseNode(vertsDevicePoseNode_);
	if (vertsDevicePoseNode)
	{
		const ScopedLock scopedLock(lock_);

		if (world_T_recentCamera_.isValid())
		{
			const HomogenousMatrixD4 matrix(world_T_recentCamera_);

			vertsDevicePoseNode->setField<Verts::Node::StringPointer>("world_T_camera", Verts::Node::StringPointer(matrix.data(), sizeof(HomogenousMatrixD4)));
			world_T_recentCamera_.toNull();
		}
	}

	if (vertsDriver_)
	{
		const ScopedLock scopedLock(lock_);

		if (!mapBuffer_.empty())
		{
			static unsigned int counter = 0u;
			vertsDriver_->sendContainer("map", counter++, mapBuffer_.data(), mapBuffer_.size());

			mapBuffer_.clear();
		}

		if (!meshesBuffer_.empty())
		{
			static unsigned int counter = 0u;
			vertsDriver_->sendContainer("mesh", counter++, meshesBuffer_.data(), meshesBuffer_.size());

			meshesBuffer_.clear();
		}
	}

	if (renderingGroup_)
	{
		const Timestamp currentTimestamp(true);

		constexpr double updateInterval = 0.1; // 100ms

		const ScopedLock scopedLock(lock_);

		for (MeshObjectMap::iterator iMesh = meshObjectMap_.begin(); iMesh != meshObjectMap_.end(); ++iMesh)
		{
			MeshObject& meshObject = iMesh->second;

			ocean_assert(meshObject.lastUpdateTimestamp_.isValid());

			if (meshObject.lastReneringTimestamp_.isInvalid() || (meshObject.lastUpdateTimestamp_ > meshObject.lastReneringTimestamp_ && meshObject.lastReneringTimestamp_ + updateInterval < currentTimestamp))
			{
				ocean_assert(meshObject.mesh_);
				const SceneElementMeshes::Mesh& mesh = *meshObject.mesh_;

				Rendering::TriangleFaces triangleFaces;
				triangleFaces.reserve(mesh.triangleIndices().size() / 3);
				for (size_t n = 0; n < mesh.triangleIndices().size() / 3; ++n)
				{
					const Index32& index0 = mesh.triangleIndices()[n * 3 + 0];
					const Index32& index1 = mesh.triangleIndices()[n * 3 + 1];
					const Index32& index2 = mesh.triangleIndices()[n * 3 + 2];

					ocean_assert(index0 < mesh.vertices().size());
					ocean_assert(index1 < mesh.vertices().size());
					ocean_assert(index2 < mesh.vertices().size());

					triangleFaces.emplace_back(index0, index1, index2);
				}

				if (meshObject.renderingTransform_)
				{
					renderingGroup_->removeChild(meshObject.renderingTransform_);
					meshObject.renderingTransform_.release();
				}

				meshObject.renderingTransform_ = Rendering::Utilities::createMesh(engine, mesh.vertices(), triangleFaces, meshObject.color_, mesh.perVertexNormals());
				meshObject.renderingTransform_->setTransformation(mesh.world_T_mesh());

				renderingGroup_->addChild(meshObject.renderingTransform_);

				meshObject.lastReneringTimestamp_ = currentTimestamp;

				// we do not upate more than one mesh per frame
				break;
			}
		}
	}

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	return anchoredContentManager_.preUpdate(engine, view, timestamp);
}

bool EnvironmentScannerExperience::startScanning()
{
	const ScopedLock scopedLock(lock_);

	if (!vertsDriver_->isInitialized())
	{
		Log::error() << "VERTS is not yet initialized";
		return false;
	}

	Verts::SharedEntity entity = vertsDriver_->newEntity({"DevicePose"});

	if (entity)
	{
		vertsDevicePoseNode_ = entity->node("DevicePose");
	}

	if (!vertsDevicePoseNode_)
	{
		renderingText_->setText("Networking failure");
		return false;
	}

	if (sceneTracker6DOF_.isNull() || !sceneTracker6DOF_->start())
	{
		Log::error() << "Scene Tracker Experience could not start the scene tracker";
		return false;
	}

	renderingText_->setText("Scanning started");

	startThread();

	return true;
}

bool EnvironmentScannerExperience::stopScanning()
{
	const ScopedLock scopedLock(lock_);

	vertsDevicePoseNode_ = nullptr;

	stopThreadExplicitly();

	if (sceneTracker6DOF_ && !sceneTracker6DOF_->stop())
	{
		Log::error() << "Scene Tracker Experience could not stop the scene tracker";
		return false;
	}

	renderingText_->setText("Scanning stopped");

	return true;
}

std::unique_ptr<XRPlaygroundExperience> EnvironmentScannerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new EnvironmentScannerExperience());
}

void EnvironmentScannerExperience::onSceneTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample)
{
	const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sceneTrackerSample(sample);

	if (!sceneTrackerSample || sceneTrackerSample->sceneElements().empty())
	{
		return;
	}

	HomogenousMatrix4 world_T_camera(sceneTrackerSample->positions().front(), sceneTrackerSample->orientations().front());

	constexpr Devices::SceneTracker6DOF::SceneElement::SceneElementType sceneElementType = Devices::SceneTracker6DOF::SceneElement::SET_MESHES;

	for (size_t n = 0; n < sceneTrackerSample->objectIds().size(); ++n)
	{
		const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement = sceneTrackerSample->sceneElements()[n];

		if (sceneElement && sceneElement->sceneElementType() == sceneElementType)
		{
			const ScopedLock scopedLock(lock_);

			recentSceneElements_.emplace_back(sceneElement);
			objectId_ = sceneTrackerSample->objectIds()[n];

			world_T_recentCamera_ = world_T_camera;

			return;
		}
	}

	const ScopedLock scopedLock(lock_);

	world_T_recentCamera_ = world_T_camera;
}

void EnvironmentScannerExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Should never happen as our engagement radius is very large!");
}

void EnvironmentScannerExperience::threadRun()
{
	MapCreatorWorker mapCreatorWorker(sceneTracker6DOF_, frameMedium_);

	Timestamp nextMapTimestamp = Timestamp(Timestamp(true) + 2.0);
	double mapTimestampInterval = 0.5;

	while (!shouldThreadStop())
	{
		TemporaryScopedLock scopedLock(lock_);
			const Devices::SceneTracker6DOF::SharedSceneElements recentSceneElements(std::move(recentSceneElements_));
		scopedLock.release();

		const Timestamp currentTimestamp(true);

		if (currentTimestamp >= nextMapTimestamp)
		{
			Vectors3 objectPoints;
			Indices32 objectPointIds;
			std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> unifiedDescriptorMap;

			if (mapCreatorWorker.latestMap(objectPoints, objectPointIds, unifiedDescriptorMap) && !objectPoints.empty() && unifiedDescriptorMap)
			{
				std::ostringstream stringStream(std::ios::binary);
				IO::OutputBitstream bitstream(stringStream);

				if (bitstream.write<unsigned long long>(mapTag_)
						&& writeObjectPointsToStream(objectPoints, objectPointIds, bitstream)
						&& Tracking::MapBuilding::Utilities::writeDescriptorMap(*unifiedDescriptorMap, bitstream))
				{
					const std::string data = stringStream.str();
					ocean_assert(!data.empty());

					std::vector<uint8_t> mapBuffer;
					if (IO::Compression::gzipCompress(data.c_str(), data.size(), mapBuffer))
					{
						scopedLock.relock(lock_);
							mapBuffer_ = std::move(mapBuffer);
						scopedLock.release();
					}
				}

				mapTimestampInterval = std::min(mapTimestampInterval * 1.25, 10.0);
			}

			nextMapTimestamp = currentTimestamp + mapTimestampInterval;
		}

		std::vector<SceneElementMeshes::SharedMesh> meshesToSend;
		meshesToSend.reserve(recentSceneElements.size() / 2);

		for (const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement : recentSceneElements)
		{
			ocean_assert(sceneElement->sceneElementType() == SceneElementMeshes::SET_MESHES);

			const SceneElementMeshes& sceneElementMeshes = dynamic_cast<const SceneElementMeshes&>(*sceneElement);

			for (const SceneElementMeshes::SharedMesh& mesh : sceneElementMeshes.meshes())
			{
				ocean_assert(mesh);

				TemporaryScopedLock internalScopedLock(lock_);

					MeshObject& meshObject = meshObjectMap_[mesh->meshId()];
					if (!meshObject.update(mesh, currentTimestamp))
					{
						continue;
					}

					bool sendMesh = false;

					if (meshObject.lastSentTimestamp_.isInvalid() || currentTimestamp >= meshObject.lastSentTimestamp_ + 0.5)
					{
						meshObject.lastSentTimestamp_ = currentTimestamp;
						sendMesh = true;
					}

				internalScopedLock.release();

				if (sendMesh)
				{
					meshesToSend.emplace_back(mesh);
				}
			}
		}

		if (!meshesToSend.empty())
		{
			std::ostringstream stringStream(std::ios::binary);
			IO::OutputBitstream bitstream(stringStream);

			if (bitstream.write<unsigned int>((unsigned int)(meshesToSend.size())))
			{
				for (const SceneElementMeshes::SharedMesh& mesh : meshesToSend)
				{
					if (!writeMeshToStream(mesh->meshId(), mesh->world_T_mesh(), mesh->vertices(), mesh->perVertexNormals(), mesh->triangleIndices(), bitstream))
					{
						Log::error() << "Failed to write mesh";
					}
				}

				const std::string data = stringStream.str();
				ocean_assert(!data.empty());

				std::vector<uint8_t> meshesBuffer;
				if (IO::Compression::gzipCompress(data.c_str(), data.size(), meshesBuffer))
				{
					scopedLock.relock(lock_);
						meshesBuffer_ = std::move(meshesBuffer);
					scopedLock.release();
				}
			}
		}

		sleep(1u);
	}
}

void EnvironmentScannerExperience::onChangedUsers(Network::Verts::Driver& driver, const UnorderedIndexSet64& addedUsers, const UnorderedIndexSet64& removedUsers)
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
		startScanning();
	}
	else if (usersBefore != 0 && userIds_.empty())
	{
		stopScanning();
	}
}

bool EnvironmentScannerExperience::writeTransformationToStream(const HomogenousMatrix4& world_T_camera, IO::OutputBitstream& bitstream)
{
	if (!bitstream.write<unsigned long long>(transformationTag_))
	{
		return false;
	}

	const HomogenousMatrixD4 worldD_T_cameraD(world_T_camera);

	constexpr unsigned long long version = 1ull;

	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	if (!bitstream.write(worldD_T_cameraD.data(), sizeof(double) * 16))
	{
		return false;
	}

	return true;
}

bool EnvironmentScannerExperience::writeMeshToStream(const Index32 meshId, const HomogenousMatrix4& world_T_mesh, const Vectors3& vertices, const Vectors3& perVertexNormals, const Indices32& triangleIndices, IO::OutputBitstream& bitstream)
{
	ocean_assert(vertices.size() == perVertexNormals.size());
	ocean_assert(triangleIndices.size() >= 3 && triangleIndices.size() % 3 == 0);

	if (vertices.empty() || vertices.size() != perVertexNormals.size())
	{
		return false;
	}

	if (triangleIndices.size() < 3 || triangleIndices.size() % 3 != 0)
	{
		return false;
	}

	if (!bitstream.write<unsigned long long>(meshTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 1ull;

	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	if (!bitstream.write<Index32>(meshId))
	{
		return false;
	}

	if (!writeTransformationToStream(world_T_mesh, bitstream))
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		const unsigned int numberVertices = (unsigned int)(vertices.size());
		if (!bitstream.write<unsigned int>(numberVertices))
		{
			return false;
		}

		if (!bitstream.write(vertices.data(), vertices.size() * sizeof(VectorF3)))
		{
			return false;
		}

		const unsigned int numberNormals = (unsigned int)(perVertexNormals.size());
		if (!bitstream.write<unsigned int>(numberNormals))
		{
			return false;
		}

		if (!bitstream.write(perVertexNormals.data(), perVertexNormals.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF3 verticesF;
		verticesF.reserve(vertices.size());
		for (const Vector3& vertex : vertices)
		{
			verticesF.emplace_back(float(vertex.x()), float(vertex.y()), float(vertex.z()));
		}

		VectorsF3 perVertexNormalsF;
		perVertexNormalsF.reserve(perVertexNormals.size());
		for (const Vector3& perVertexNormal : perVertexNormals)
		{
			perVertexNormalsF.emplace_back(float(perVertexNormal.x()), float(perVertexNormal.y()), float(perVertexNormal.z()));
		}

		const unsigned int numberVertices = (unsigned int)(verticesF.size());
		if (!bitstream.write<unsigned int>(numberVertices))
		{
			return false;
		}

		if (!bitstream.write(verticesF.data(), verticesF.size() * sizeof(VectorF3)))
		{
			return  false;
		}

		const unsigned int numberNormals = (unsigned int)(perVertexNormalsF.size());
		if (!bitstream.write<unsigned int>(numberNormals))
		{
			return false;
		}

		if (!bitstream.write(perVertexNormalsF.data(), perVertexNormalsF.size() * sizeof(VectorF3)))
		{
			return  false;
		}
	}

	const unsigned int numberTriangleIndices = (unsigned int)(triangleIndices.size());
	if (!bitstream.write<unsigned int>(numberTriangleIndices))
	{
		return false;
	}

	if (!bitstream.write(triangleIndices.data(), triangleIndices.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

bool EnvironmentScannerExperience::writeObjectPointsToStream(const Vectors3& objectPoints, const Indices32& objectPointIds, IO::OutputBitstream& bitstream)
{
	ocean_assert(objectPoints.size() == objectPointIds.size());

	if (objectPoints.empty() || objectPoints.size() != objectPointIds.size())
	{
		return false;
	}

	if (!bitstream.write<unsigned long long>(objectPointsTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 1ull;

	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	if (!bitstream.write<unsigned int>((unsigned int)(objectPoints.size())))
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		if (!bitstream.write(objectPoints.data(), objectPoints.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF3 objectPointsF;
		objectPointsF.reserve(objectPoints.size());
		for (const Vector3& objectPoint : objectPoints)
		{
			objectPointsF.emplace_back(float(objectPoint.x()), float(objectPoint.y()), float(objectPoint.z()));
		}

		if (!bitstream.write(objectPointsF.data(), objectPointsF.size() * sizeof(VectorF3)))
		{
			return  false;
		}
	}

	if (!bitstream.write<unsigned int>((unsigned int)(objectPointIds.size())))
	{
		return false;
	}

	if (!bitstream.write(objectPointIds.data(), objectPointIds.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

bool EnvironmentScannerExperience::readTransformationFromStream(IO::InputBitstream& bitstream, HomogenousMatrix4& world_T_camera)
{
	unsigned long long tag;
	if (!bitstream.read<unsigned long long>(tag) || tag != transformationTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	HomogenousMatrixD4 worldD_T_cameraD;
	if (!bitstream.read(worldD_T_cameraD.data(), sizeof(double) * 16))
	{
		return false;
	}

	world_T_camera = HomogenousMatrix4(worldD_T_cameraD);

	return world_T_camera.isValid();
}

bool EnvironmentScannerExperience::readMeshFromStream(IO::InputBitstream& bitstream, Index32& meshId, HomogenousMatrix4& world_T_mesh, Vectors3& vertices, Vectors3& perVertexNormals, Indices32& triangleIndices)
{
	unsigned long long tag;
	if (!bitstream.read<unsigned long long>(tag) || tag != meshTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	if (!bitstream.read<Index32>(meshId))
	{
		return false;
	}

	if (!readTransformationFromStream(bitstream, world_T_mesh))
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		unsigned int numberVertices = 0u;
		if (!bitstream.read<unsigned int>(numberVertices) || numberVertices > 100000u)
		{
			return false;
		}

		vertices.resize(numberVertices);
		if (!bitstream.read(vertices.data(), vertices.size() * sizeof(VectorF3)))
		{
			return false;
		}

		unsigned int numberNormals = 0u;
		if (!bitstream.read<unsigned int>(numberNormals) || numberNormals > 100000u)
		{
			return false;
		}

		if (numberVertices != numberNormals)
		{
			return false;
		}

		perVertexNormals.resize(numberNormals);
		if (!bitstream.read(perVertexNormals.data(), perVertexNormals.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		unsigned int numberVertices = 0u;
		if (!bitstream.read<unsigned int>(numberVertices) || numberVertices > 100000u)
		{
			return false;
		}

		VectorsF3 verticesF(numberVertices);
		if (!bitstream.read(verticesF.data(), verticesF.size() * sizeof(VectorF3)))
		{
			return false;
		}

		unsigned int numberNormals = 0u;
		if (!bitstream.read<unsigned int>(numberNormals) || numberNormals > 100000u)
		{
			return false;
		}

		if (numberVertices != numberNormals)
		{
			return false;
		}

		VectorsF3 perVertexNormalsF(numberNormals);
		if (!bitstream.read(perVertexNormalsF.data(), perVertexNormalsF.size() * sizeof(VectorF3)))
		{
			return false;
		}

		vertices.reserve(verticesF.size());
		for (const VectorF3& vertexF : verticesF)
		{
			vertices.emplace_back(Scalar(vertexF.x()), Scalar(vertexF.y()), Scalar(vertexF.z()));
		}

		perVertexNormals.reserve(perVertexNormalsF.size());
		for (const VectorF3& perVertexNormalF : perVertexNormalsF)
		{
			perVertexNormals.emplace_back(Scalar(perVertexNormalF.x()), Scalar(perVertexNormalF.y()), Scalar(perVertexNormalF.z()));
		}
	}

	unsigned int numberTriangleIndices = 0u;
	if (!bitstream.read<unsigned int>(numberTriangleIndices) || numberTriangleIndices == 0u || numberTriangleIndices > 300000u || numberTriangleIndices % 3u != 0u)
	{
		return false;
	}

	triangleIndices.resize(numberTriangleIndices);
	if (!bitstream.read(triangleIndices.data(), triangleIndices.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

bool EnvironmentScannerExperience::readObjectPointsFromStream(IO::InputBitstream& bitstream, Vectors3& objectPoints, Indices32& objectPointIds)
{
	unsigned long long tag = 0ull;
	if (!bitstream.read<unsigned long long>(tag) || tag != objectPointsTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	unsigned int numberObjectPoints = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPoints))
	{
		return false;
	}

	if (numberObjectPoints > 100000u)
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		objectPoints.resize(numberObjectPoints);

		if (!bitstream.read(objectPoints.data(), objectPoints.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF3 objectPointsF(numberObjectPoints);
		if (!bitstream.read(objectPointsF.data(), objectPointsF.size() * sizeof(VectorF3)))
		{
			return false;
		}

		objectPoints.clear();
		objectPoints.reserve(objectPointsF.size());

		for (VectorF3& objectPointF : objectPointsF)
		{
			objectPoints.emplace_back(Scalar(objectPointF.x()), Scalar(objectPointF.y()), Scalar(objectPointF.z()));
		}
	}

	unsigned int numberObjectPointIds = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPointIds))
	{
		return false;
	}

	if (numberObjectPoints != numberObjectPointIds)
	{
		return false;
	}

	objectPointIds.resize(numberObjectPointIds);

	if (!bitstream.read(objectPointIds.data(), objectPointIds.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

}

}
