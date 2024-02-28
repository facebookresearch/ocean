// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/metaportation/LegacyMetaportationExperience.h"
#include "application/ocean/xrplayground/common/experiences/metaportation/LegacyMetaportationScannerExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/Tracker6DOF.h"

#include "ocean/geometry/AbsoluteTransformation.h"

#include "ocean/media/Manager.h"

#include "ocean/network/Resolver.h"

#include "ocean/rendering/Utilities.h"
#include "ocean/rendering/FrameTexture2D.h"

#include "ocean/tracking/mapbuilding/RelocalizerStereo.h"
#include "ocean/tracking/mapbuilding/Utilities.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
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

LegacyMetaportationExperience::PortalBase::~PortalBase()
{
	release();
}

bool LegacyMetaportationExperience::PortalBase::initialize(const Rendering::TransformRef& parent, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	ocean_assert(parent);

	const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingText_);
	textTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -2)));

	renderingTransform_ = engine->factory().createTransform();
	renderingTransform_->addChild(textTransform);

	parent->addChild(renderingTransform_);

	return true;
}

void LegacyMetaportationExperience::PortalBase::release()
{
	renderingText_.release();
	renderingTransform_.release();
}

void LegacyMetaportationExperience::PortalBase::onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	if (key == "A" || key == "X")
	{
		// we toggle between mesh and point rendering

		const ScopedLock scopedLock(lock_);

		renderMode_ = RenderMode((renderMode_ + 1u) % RM_END);

		updateRendering_ = true;
	}
}

void LegacyMetaportationExperience::PortalBase::renderPortal(const Rendering::EngineRef& engine)
{
	const ScopedLock scopedLock(lock_);

	if (updateRendering_)
	{
		// we have to remove the old meshes

		for (MeshObjectMap::iterator iMesh = activeMeshObjectMap_.begin(); iMesh != activeMeshObjectMap_.end(); ++iMesh)
		{
			MeshObject& meshObject = iMesh->second;

			if (meshObject.renderingTransform_)
			{
				renderingTransform_->removeChild(meshObject.renderingTransform_);
				meshObject.renderingTransform_.release();
			}
		}

		if (!nextMeshObjectMap_.empty())
		{
			activeMeshObjectMap_ = std::move(nextMeshObjectMap_);
		}

		for (MeshObjectMap::iterator iMesh = activeMeshObjectMap_.begin(); iMesh != activeMeshObjectMap_.end(); ++iMesh)
		{
			MeshObject& meshObject = iMesh->second;

			if (renderMode_ == RM_TEXTURED)
			{
				const Rendering::VertexSetRef vertexSet = engine->factory().createVertexSet();
				vertexSet->setVertices(meshObject.vertices_);
				vertexSet->setColors(meshObject.perVertexColors_);
				vertexSet->setNormals(meshObject.perVertexNormals_);
				vertexSet->setTextureCoordinates(meshObject.textureCoordinates_, 0u);

				const Rendering::TrianglesRef triangles = engine->factory().createTriangles();
				triangles->setVertexSet(vertexSet);
				triangles->setFaces(meshObject.triangleFaces_);

				const Rendering::AttributeSetRef attributeSet = engine->factory().createAttributeSet();

				if (meshObject.textureFrame_.isValid())
				{
					const Rendering::FrameTexture2DRef texture = engine->factory().createFrameTexture2D();
					texture->setTexture(Frame(meshObject.textureFrame_, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT));
					texture->setMinificationFilterMode(Rendering::Texture::MIN_MODE_NEAREST);
					texture->setMagnificationFilterMode(Rendering::Texture::MAG_MODE_LINEAR);
					texture->setUseMipmaps(false);

					const Rendering::TexturesRef textures = engine->factory().createTextures();
					textures->addTexture(texture);
					attributeSet->addAttribute(textures);
				}
				else
				{
					Rendering::MaterialRef material = engine->factory().createMaterial();
					material->setDiffuseColor(RGBAColor(1.0f, 1.0f, 1.0f));
					attributeSet->addAttribute(material);
				}

				const Rendering::GeometryRef geometry = engine->factory().createGeometry();
				geometry->addRenderable(triangles, attributeSet);

				meshObject.renderingTransform_ = engine->factory().createTransform();
				meshObject.renderingTransform_->addChild(geometry);
			}
			else if (renderMode_ == RM_SHADED)
			{
				if (meshObject.perVertexNormals_.empty())
				{
					meshObject.perVertexNormals_.reserve(meshObject.vertices_.size());

					for (const Rendering::TriangleFace& triangleFace : meshObject.triangleFaces_)
					{
						const Vector3& vertex0 = meshObject.vertices_[triangleFace.index(0u)];
						const Vector3& vertex1 = meshObject.vertices_[triangleFace.index(1u)];
						const Vector3& vertex2 = meshObject.vertices_[triangleFace.index(2u)];

						const Vector3 normal = (vertex1 - vertex0).cross(vertex2 - vertex0).normalizedOrZero();

						meshObject.perVertexNormals_.emplace_back(normal);
						meshObject.perVertexNormals_.emplace_back(normal);
						meshObject.perVertexNormals_.emplace_back(normal);
					}
				}

				meshObject.renderingTransform_ = Rendering::Utilities::createMesh(engine, meshObject.vertices_, meshObject.triangleFaces_, RGBAColor(0.7f, 0.7f, 0.7f), meshObject.perVertexNormals_);
			}
			else
			{
				ocean_assert(renderMode_ == RM_POINTS);

				meshObject.renderingTransform_ = Rendering::Utilities::createPoints(*engine, meshObject.vertices_, RGBAColor(0.0f, 0.0f, 1.0f), Scalar(2));
			}

			meshObject.renderingTransform_->setTransformation(meshObject.world_T_mesh_);

			renderingTransform_->addChild(meshObject.renderingTransform_);
		}

		updateRendering_ = false;
	}

	renderingTransform_->setTransformation(headsetWorld_T_meshWorld_);

	if (nextNetworkDataThroughputTimestamp_.isValid() && Timestamp(true) >= nextNetworkDataThroughputTimestamp_)
	{
		const std::string tcpThroughput = " TCP Send: " + String::toAString((unsigned int)(PlatformSDK::Network::get().currentSendThroughput(true) / 1024), 1u)
												+ "KB/s, receive: " + String::toAString((unsigned int)(PlatformSDK::Network::get().currentReceiveThroughput(true) / 1024), 1u) + "KB/s ";

		const std::string udpThroughput = " UDP Send: " + String::toAString((unsigned int)(PlatformSDK::Network::get().currentSendThroughput(false) / 1024), 1u)
												+ "KB/s, receive: " + String::toAString((unsigned int)(PlatformSDK::Network::get().currentReceiveThroughput(false) / 1024), 1u) + "KB/s ";

		renderingText_->setText(" Throughput: \n" + tcpThroughput + "\n" + udpThroughput);
	}
}

bool LegacyMetaportationExperience::PortalBase::onReceiveMesh(IO::InputBitstream& bitstream)
{
	Index32 meshId = Index32(-1);
	unsigned int remainingMeshes = 0u;
	HomogenousMatrix4 world_T_mesh(false);
	Vectors3 vertices;
	Vectors3 perVertexNormals;
	RGBAColors perVertexColors;
	Vectors2 textureCoordinates;
	Rendering::TriangleFaces triangleFaces;
	Frame textureFrame;

	if (LegacyMetaportationScannerExperience::readMeshFromStream(bitstream, meshId, remainingMeshes, world_T_mesh, vertices, perVertexNormals, perVertexColors, textureCoordinates, triangleFaces, textureFrame))
	{
		Log::info() << "Received mesh: " << meshId << " textured: " << int(textureFrame.isValid()) << " remaining: " << remainingMeshes;

		const ScopedLock scopedLock(lock_);

		ocean_assert(pendingMeshObjectMap_.find(meshId) == pendingMeshObjectMap_.cend());

		MeshObject& meshObject = pendingMeshObjectMap_[meshId];

		meshObject.world_T_mesh_ = world_T_mesh;
		meshObject.vertices_ = std::move(vertices);
		meshObject.perVertexNormals_ = std::move(perVertexNormals);
		meshObject.perVertexColors_ = std::move(perVertexColors);
		meshObject.textureCoordinates_ = std::move(textureCoordinates);
		meshObject.triangleFaces_ = std::move(triangleFaces);
		meshObject.textureFrame_ = std::move(textureFrame);

		if (remainingMeshes == 0u)
		{
			// we have received the last mesh, so we can render the updated room

			nextMeshObjectMap_ = std::move(pendingMeshObjectMap_);
			updateRendering_ = true;

			return true;
		}
	}
	else
	{
		Log::error() << "Failed to decode mesh";
	}

	return false;
}

bool LegacyMetaportationExperience::PortalCreator::initialize(const Rendering::TransformRef& parent, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	PortalBase::initialize(parent, engine, timestamp);

	std::string message = " Failed to determine \n the local network address ";

	const Network::Resolver::Addresses4 localAddresses = Network::Resolver::get().localAddresses();

	if (!localAddresses.empty())
	{
		tcpServer_.setPort(Network::Port(6000, Network::Port::TYPE_READABLE));

		tcpServer_.setConnectionRequestCallback(Network::TCPServer::ConnectionRequestCallback::create(*this, &LegacyMetaportationExperience::PortalCreator::onConnectionRequestFromMobileTCP));
		tcpServer_.setDisconnectCallback(Network::TCPServer::DisconnectCallback::create(*this, &LegacyMetaportationExperience::PortalCreator::onConnectionDisconnectedFromMobileTCP));
		tcpServer_.setReceiveCallback(Network::TCPServer::ReceiveCallback::create(*this, &LegacyMetaportationExperience::PortalCreator::onReceiveFromMobileTCP));

		if (tcpServer_.start())
		{
			message = std::string(" Local network address: \n ") + " " + localAddresses.front().readable() + " \n\n Port: " + String::toAString(tcpServer_.port().readable()) + " ";
		}
		else
		{
			message = " Failed to start TCP server ";
		}

		udpServer_.setPort(Network::Port(6000, Network::Port::TYPE_READABLE));
		udpServer_.setReceiveCallback(Network::PackagedUDPServer::ReceiveCallback::create(*this, &LegacyMetaportationExperience::PortalCreator::onReceiveFromMobileUDP));
		udpServer_.start();
	}

	renderingText_->setText(message);

	renderingTransformDevice_ = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.15), Scalar(0.075), Scalar(0.0075)), RGBAColor(0.7, 0.7, 0.7));
	renderingTransformDevice_->setVisible(false);

	renderingTransform_->addChild(renderingTransformDevice_);

	headsetWorld_T_mapWorld_.setTransformation(HomogenousMatrix4(true), timestamp);

	startThread();

	return true;
}

bool LegacyMetaportationExperience::PortalCreator::setRemoteUserId(const uint64_t remoteUserId)
{
	const ScopedLock scopedLock(lock_);

	remoteUserId_ = remoteUserId;

	if (remoteUserId_ != 0ull)
	{
		PlatformSDK::Network::get().acceptConnectionForUser(remoteUserId_);
	}

	if (!receiveSubscription_)
	{
		receiveSubscription_ = PlatformSDK::Network::get().addReceiveCallback(PlatformSDK::Network::CI_CUSTOM, std::bind(&LegacyMetaportationExperience::PortalCreator::onReceiveFromHeadset, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
	}

	return true;
}

void LegacyMetaportationExperience::PortalCreator::release()
{
	receiveSubscription_.release();

	stopThreadExplicitly();

	tcpServer_.stop();

	PortalBase::release();
}

Timestamp LegacyMetaportationExperience::PortalCreator::preUpdate(const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	const ScopedLock scopedLock(lock_);

	const HomogenousMatrix4 world_T_recentCamera(world_T_recentCamera_);
	world_T_recentCamera_.toNull();

	if (world_T_recentCamera.isValid())
	{
		renderingTransformDevice_->setTransformation(world_T_recentCamera * HomogenousMatrix4(Vector3(Scalar(0.065), Scalar(-0.02), 0)));
		renderingTransformDevice_->setVisible(true);
	}

	headsetWorld_T_meshWorld_ = headsetWorld_T_mapWorld_.transformation(timestamp);

	renderPortal(engine);

	return timestamp;
}

bool LegacyMetaportationExperience::PortalCreator::onConnectionRequestFromMobileTCP(const Network::Address4& senderAddress, const Network::Port& senderPort, const Network::ConnectionOrientedServer::ConnectionId connectionId)
{
	Log::info() << "CameraStreamingReceiverExperience: Established connection from " << senderAddress.readable() << ":" << senderPort.readable();

	ocean_assert(renderingText_);
	if (renderingText_)
	{
		renderingText_->setText(" Connection established from \n" + senderAddress.readable() + " ");
	}

	if (nextNetworkDataThroughputTimestamp_.isInvalid())
	{
		nextNetworkDataThroughputTimestamp_ = Timestamp(true) + 5.0;
	}

	return true;
}

void LegacyMetaportationExperience::PortalCreator::onConnectionDisconnectedFromMobileTCP(const Network::ConnectionOrientedServer::ConnectionId connectionId)
{
	Log::info() << "CameraStreamingReceiverExperience: Disconnected stream";

	ocean_assert(renderingText_);
	if (renderingText_)
	{
		renderingText_->setText(" Disconnected stream ");
	}
}

void LegacyMetaportationExperience::PortalCreator::onReceiveFromMobileTCP(const Network::ConnectionOrientedServer::ConnectionId connectionId, const void* data, const size_t size)
{
	if (size == 0)
	{
		return;
	}

	const std::string inputString((const char*)(data), size);

	std::istringstream stringStream(inputString, std::ios::binary);
	IO::InputBitstream bitstream(stringStream);

	unsigned long long tag;
	if (bitstream.look<unsigned long long>(tag))
	{
		switch (tag)
		{
			case LegacyMetaportationScannerExperience::meshTag_:
			{
				const bool lastMeshInSeries = onReceiveMesh(bitstream);

				const uint8_t* data8 = (const uint8_t*)(data);
				Buffer dataCopy(data8, data8 + size);

				TemporaryScopedLock scopedLock(lock_);

				if (pendingMeshBuffers_.empty())
				{
					pendingMeshBuffers_ = Buffers(1, std::move(dataCopy));
				}
				else
				{
					pendingMeshBuffers_.emplace_back(std::move(dataCopy));
				}

				if (lastMeshInSeries)
				{
					if (!completeMeshBuffers_.empty())
					{
						Log::info() << "skipped mesh due to pending packages in network layer";
					}

					completeMeshBuffers_ = std::move(pendingMeshBuffers_);
				}

				// now, we also send the latest alignment between mesh and headset

				const HomogenousMatrixF4 headsetWorld_T_meshWorld(headsetWorld_T_meshWorld_);
				scopedLock.release();

				if (!PlatformSDK::Network::get().sendToUserUDP(remoteUserId_, PlatformSDK::Network::CI_CUSTOM, &headsetWorld_T_meshWorld, sizeof(HomogenousMatrixF4)))
				{
					Log::warning() << "Failed to send headset/mesh transformation to all users in the room";
				}

				break;
			}

			case LegacyMetaportationScannerExperience::mapTag_:
			{
				Vectors3 mapObjectPoints;
				Indices32 mapObjectPointIds;
				std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> unifiedDescriptorMap;

				unsigned long long mapTag = 0u;
				if (bitstream.read<unsigned long long>(mapTag) && mapTag == LegacyMetaportationScannerExperience::mapTag_
						&& LegacyMetaportationScannerExperience::readObjectPointsFromStream(bitstream, mapObjectPoints, mapObjectPointIds)
						&& Tracking::MapBuilding::Utilities::readDescriptorMap(bitstream, unifiedDescriptorMap))
				{
					const ScopedLock scopedLock(lock_);

					mapObjectPoints_ = std::move(mapObjectPoints);
					mapObjectPointIds_ = std::move(mapObjectPointIds);
					unifiedDescriptorMap_ = std::move(unifiedDescriptorMap);
				}
				else
				{
					Log::error() << "Failed to decode map";
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

void LegacyMetaportationExperience::PortalCreator::onReceiveFromMobileUDP(const Network::Address4& address, const Network::Port& port, const void* data, const size_t size, const Network::PackagedUDPServer::MessageId messageId)
{
	const std::string inputString((const char*)(data), size);

	std::istringstream stringStream(inputString, std::ios::binary);
	IO::InputBitstream bitstream(stringStream);

	unsigned long long tag;
	if (bitstream.look<unsigned long long>(tag))
	{
		switch (tag)
		{
			case LegacyMetaportationScannerExperience::transformationTag_:
			{
				HomogenousMatrix4 world_T_camera;
				if (LegacyMetaportationScannerExperience::readTransformationFromStream(bitstream, world_T_camera))
				{
					const ScopedLock scopedLock(lock_);

					world_T_recentCamera_ = world_T_camera;
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

void LegacyMetaportationExperience::PortalCreator::onReceiveFromHeadset(const uint64_t senderUserId, const Platform::Meta::Quest::PlatformSDK::Network::ComponentId componentId, const void* data, const size_t size, const Platform::Meta::Quest::PlatformSDK::Network::ConnectionType connectionType)
{
	ocean_assert(componentId == PlatformSDK::Network::CI_CUSTOM);
	ocean_assert(data != nullptr && size != 0);

	if (connectionType == PlatformSDK::Network::CT_UDP)
	{
		ocean_assert(connectionType == PlatformSDK::Network::CT_UDP);

		if (size == sizeof(HomogenousMatrixF4))
		{
			HomogenousMatrixF4 remoteHeadsetWorld_T_remoteAvatar;
			memcpy(&remoteHeadsetWorld_T_remoteAvatar, data, sizeof(HomogenousMatrixF4));
			ocean_assert(remoteHeadsetWorld_T_remoteAvatar.isValid());

			if (remoteHeadsetWorld_T_remoteAvatar.isValid())
			{
				const ScopedLock scopedLock(lock_);

				remoteHeadsetWorld_T_remoteAvatar_ = HomogenousMatrix4(remoteHeadsetWorld_T_remoteAvatar);
			}
		}
		else
		{
			Log::error() << "Recived invalid UDP data";
		}
	}
}

void LegacyMetaportationExperience::PortalCreator::threadRun()
{
	Tracking::MapBuilding::RelocalizerStereo relocalizer(Tracking::MapBuilding::Relocalizer::detectFreakFeatures);

	const Media::FrameMediumRef frameMediumA = Media::Manager::get().newMedium("StereoCamera0Id:0");
	const Media::FrameMediumRef frameMediumB = Media::Manager::get().newMedium("StereoCamera0Id:1");

	if (frameMediumA.isNull() || frameMediumB.isNull()
		|| !frameMediumA->start() || !frameMediumB->start())
	{
		Log::error() << "Failed to access both cameras";
		return;
	}

	HomogenousMatrices4 headsetWorld_T_correspondingDevices;
	HomogenousMatrices4 mapWorld_T_correspondingDevices;

	const Devices::Tracker6DOFRef headsetTracker = Devices::Manager::get().device("Headset 6DOF Tracker");

	if (!headsetTracker)
	{
		Log::error() << "Failed to access the headset tracker";
		return;
	}

	headsetTracker->start();

	RandomGenerator randomGenerator;

	bool stopRelocalization = false;

	while (!shouldThreadStop())
	{
		if (PlatformSDK::Network::get().sendQueueSizeTCP(remoteUserId_) == 0)
		{
			// the send queue is empty, so we can send the most recent meshes

			TemporaryScopedLock scopedLock(lock_);
				Buffers completeMeshBuffers(std::move(completeMeshBuffers_));
			scopedLock.release();

			if (!completeMeshBuffers.empty())
			{
				for (Buffer& buffer : completeMeshBuffers)
				{
					if (!PlatformSDK::Network::get().sendToUserTCP(remoteUserId_, PlatformSDK::Network::CI_CUSTOM, std::move(buffer)))
					{
						Log::warning() << "Failed to send mesh via network to all users in the room";
					}
				}
			}
		}

		if (stopRelocalization)
		{
			sleep(1u);
			continue;
		}

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

		SharedAnyCamera anyCameraA;
		FrameRef frameA = frameMediumA->frame(&anyCameraA);

		if (!frameA || !anyCameraA)
		{
			sleep(1u);
			continue;
		}

		const Timestamp timestamp = frameA->timestamp();

		SharedAnyCamera anyCameraB;
		FrameRef frameB = frameMediumB->frame(timestamp, &anyCameraB);

		if (!frameB || !anyCameraB || timestamp != frameB->timestamp())
		{
			sleep(1u);
			continue;
		}

		const HomogenousMatrix4 device_T_cameraA(frameMediumA->device_T_camera());
		const HomogenousMatrix4 device_T_cameraB(frameMediumB->device_T_camera());

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
		if (!CV::FrameConverter::Comfort::convert(*frameA, FrameType(*frameA, FrameType::FORMAT_Y8), yFrameA, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE)
				|| !CV::FrameConverter::Comfort::convert(*frameB, FrameType(*frameB, FrameType::FORMAT_Y8), yFrameB, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
		{
			ocean_assert(false && "This should never happen!");
			break;
		}

		HomogenousMatrix4 mapWorld_T_device(false);

		constexpr unsigned int minimalNumberCorrespondences = 20u;
		constexpr Scalar maximalProjectionError = Scalar(2.5);

		if (!relocalizer.relocalize(*anyCameraA, *anyCameraB, device_T_cameraA, device_T_cameraB, yFrameA, yFrameB, mapWorld_T_device, minimalNumberCorrespondences, maximalProjectionError))
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

		if (headsetWorld_T_correspondingDevices.size() > 100)
		{
			/*const size_t index = headsetWorld_T_correspondingDevices.size() - 30; // we keep 30 transfomations

			headsetWorld_T_correspondingDevices = HomogenousMatrices4(headsetWorld_T_correspondingDevices.cbegin() + index, headsetWorld_T_correspondingDevices.cend());
			mapWorld_T_correspondingDevices = HomogenousMatrices4(mapWorld_T_correspondingDevices.cbegin() + index, mapWorld_T_correspondingDevices.cend());*/

			// we stop the alignment process to avoid any further motion sickness

			stopRelocalization = true;
		}
	}
}

bool LegacyMetaportationExperience::PortalReceiver::initialize(const Rendering::TransformRef& parent, const uint64_t senderUserId, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	if (!PortalBase::initialize(parent, engine, timestamp))
	{
		return false;
	}

	ocean_assert(remoteUserId_ == 0ull);
	remoteUserId_ = senderUserId;

	PlatformSDK::Network::get().acceptConnectionForUser(senderUserId);
	receiveSubscription_ = PlatformSDK::Network::get().addReceiveCallback(PlatformSDK::Network::CI_CUSTOM, std::bind(&LegacyMetaportationExperience::PortalReceiver::onReceiveFromHeadset, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));

	return receiveSubscription_.isValid();
}

void LegacyMetaportationExperience::PortalReceiver::release()
{
	receiveSubscription_.release();

	PortalBase::release();
}

Timestamp LegacyMetaportationExperience::PortalReceiver::preUpdate(const HomogenousMatrix4& headsetWorld_T_avatar, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	renderPortal(engine);

	if (remoteUserId_ != 0ull)
	{
		const HomogenousMatrixF4 headsetWorld_T_avatarF(headsetWorld_T_avatar);

		if (!PlatformSDK::Network::get().sendToUserUDP(remoteUserId_, PlatformSDK::Network::CI_CUSTOM, &headsetWorld_T_avatarF, sizeof(HomogenousMatrixF4)))
		{
			Log::warning() << "Failed to send headset/mesh transformation to all users in the room";
		}
	}

	return timestamp;
}

void LegacyMetaportationExperience::PortalReceiver::onReceiveFromHeadset(const uint64_t senderUserId, const PlatformSDK::Network::ComponentId componentId, const void* data, const size_t size, const PlatformSDK::Network::ConnectionType connectionType)
{
	ocean_assert(componentId == PlatformSDK::Network::CI_CUSTOM);
	ocean_assert(data != nullptr && size != 0);

	if (nextNetworkDataThroughputTimestamp_.isInvalid())
	{
		nextNetworkDataThroughputTimestamp_.toNow();
	}

	if (connectionType == PlatformSDK::Network::CT_TCP)
	{
		const std::string inputString((const char*)(data), size);

		std::istringstream stringStream(inputString, std::ios::binary);
		IO::InputBitstream bitstream(stringStream);

		onReceiveMesh(bitstream);
	}
	else
	{
		ocean_assert(connectionType == PlatformSDK::Network::CT_UDP);

		if (size == sizeof(HomogenousMatrixF4))
		{
			HomogenousMatrixF4 headsetWorld_T_meshWorld;
			memcpy(&headsetWorld_T_meshWorld, data, sizeof(HomogenousMatrixF4));

			ocean_assert(headsetWorld_T_meshWorld.isValid());

			if (headsetWorld_T_meshWorld.isValid())
			{
				const ScopedLock scopedLock(lock_);

				headsetWorld_T_meshWorld_ = HomogenousMatrix4(headsetWorld_T_meshWorld);
			}
		}
		else
		{
			Log::error() << "Recived invalid UDP data";
		}
	}
}

LegacyMetaportationExperience::~LegacyMetaportationExperience()
{
	// nothing to do here
}

bool LegacyMetaportationExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
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

			avatarScopedSubscriptions_.emplace_back(Avatars::Manager::get().createAvatar(userId_, std::bind(&LegacyMetaportationExperience::onAvatarCreated, this, std::placeholders::_1, std::placeholders::_2)));
		}
	}
	else
	{
		Log::error() << "Failed to determine local user";
	}

	// this experience uses avatars, so the hand meshes don't need to be displayed.
	PlatformSpecific::get().vrHandVisualizer().hide();

	return true;
}

bool LegacyMetaportationExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	avatarScopedSubscriptions_.clear();
	zoneScopedSubscription_.release();

	if (portalCreator_)
	{
		portalCreator_->release();
		portalCreator_ = nullptr;
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

Timestamp LegacyMetaportationExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	if (applicationState_ == AS_IDLE)
	{
		VRTableMenu::Entries menuEntries =
		{
			VRTableMenu::Entry("Create and share your room", "SHARE_ROOM"),
			VRTableMenu::Entry("Wait to join a room", "JOIN_ROOM"),
		};

		const VRTableMenu::Group menuGroup("What do you want to do?", std::move(menuEntries));

		vrTableMenu_.setMenuEntries(menuGroup);
		vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, -1)));

		applicationState_ = AS_USER_SELECTING_MODE;
	}
	else if (applicationState_ == AS_USER_SELECTING_MODE)
	{
		ocean_assert(vrTableMenu_.isShown());

		std::string entryUrl;
		if (vrTableMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
		{
			vrTableMenu_.hide();

			if (entryUrl == "SHARE_ROOM")
			{
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

			portalCreator_ = std::make_shared<PortalCreator>();
			portalCreator_->initialize(renderingTransformMesh_, engine, timestamp);
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

						portalCreator_->setRemoteUserId(remoteUserId_);

						zoneScopedSubscription_ = Avatars::Manager::get().joinZone(roomZoneName(localRoomId_));

						avatarScopedSubscriptions_.emplace_back(Avatars::Manager::get().createAvatar(remoteUserId_, std::bind(&LegacyMetaportationExperience::onAvatarCreated, this, std::placeholders::_1, std::placeholders::_2)));

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

				avatarScopedSubscriptions_.emplace_back(Avatars::Manager::get().createAvatar(remoteUserId_, std::bind(&LegacyMetaportationExperience::onAvatarCreated, this, std::placeholders::_1, std::placeholders::_2)));

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

		if (renderingTransformRemoteAvatar_)
		{
			// the remote user may have adjusted the local position (manually with the joystick), so we need to reflect this location

			const HomogenousMatrix4 remoteHeadsetWorld_T_remoteAvatar = portalCreator_->remoteHeadsetWorld_T_remoteAvatar();
			ocean_assert(remoteHeadsetWorld_T_remoteAvatar.isValid());

			renderingTransformRemoteAvatar_->setTransformation(remoteHeadsetWorld_T_remoteAvatar);
		}
	}

	if (portalReceiver_)
	{
		handleUserMovement(timestamp);

		const HomogenousMatrix4 avatar_T_headsetWorld = renderingTransformMesh_->transformation();

		portalReceiver_->preUpdate(avatar_T_headsetWorld.inverted(), engine, timestamp);
	}

	return timestamp;
}

void LegacyMetaportationExperience::onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	if (portalCreator_)
	{
		portalCreator_->onKeyPress(userInterface, engine, key, timestamp);
	}

	if (portalReceiver_)
	{
		portalReceiver_->onKeyPress(userInterface, engine, key, timestamp);
	}
}

std::unique_ptr<XRPlaygroundExperience> LegacyMetaportationExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new LegacyMetaportationExperience());
}

void LegacyMetaportationExperience::handleUserMovement(const Timestamp& timestamp)
{
	// let's see whether the user want's to move

	const Vector2 joystickTiltLeft = PlatformSpecific::get().trackedRemoteDevice().joystickTilt(TrackedRemoteDevice::RT_LEFT);
	const Vector2 joystickTiltRight = PlatformSpecific::get().trackedRemoteDevice().joystickTilt(TrackedRemoteDevice::RT_RIGHT);

	if (joystickTiltLeft.sqr() > joystickTiltRight.sqr())
	{
		// left joystick supports smoothly flying through the space

		const Vector2& joystickTilt = joystickTiltLeft;

		const HomogenousMatrix4 world_T_device = HeadsetPoses::world_T_device(PlatformSpecific::get().ovr(), timestamp);

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

void LegacyMetaportationExperience::onAvatarCreated(const uint64_t userId, Rendering::TransformRef avatarTransform)
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

std::string LegacyMetaportationExperience::roomZoneName(const uint64_t roomId)
{
	ocean_assert(roomId != 0ull);

	return "XRPlayground://LEGACY_METAPORTATION_ROOM_" + String::toAString(roomId);
}

#else // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

LegacyMetaportationExperience::~LegacyMetaportationExperience()
{
	// nothing to do here
}

std::unique_ptr<XRPlaygroundExperience> LegacyMetaportationExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new LegacyMetaportationExperience());
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

}

}
