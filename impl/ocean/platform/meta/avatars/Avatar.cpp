// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/avatars/Avatar.h"
#include "ocean/platform/meta/avatars/Shaders.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Geometry.h"

#include <ovrAvatar2/Entity.h>
#include <ovrAvatar2/Render.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

Avatar::Avatar(const UserType userType, const uint64_t userId, const bool isLocal)
{
	if (userType == UT_UNKNOWN || userId == 0ull)
	{
		ocean_assert(false && "Invalid user id!");
		return;
	}

	userType_ = userType;

	isLocal_ = isLocal;

	ocean_assert(entityId_ == ovrAvatar2EntityId_Invalid);

	ovrAvatar2EntityCreateInfo createInfo;
	memset(&createInfo, 0, sizeof(createInfo));

	if (isLocal)
	{
		createInfo.features = ovrAvatar2EntityFeatures(ovrAvatar2EntityFeature_Preset_Default | ovrAvatar2EntityFeatures_ShowControllers);
	}
	else
	{
		// HACK: workaround to ensure that the local user's controller do not get 'mirrored' when rendering remote users with controllers
		createInfo.features = ovrAvatar2EntityFeatures(ovrAvatar2EntityFeature_Preset_Default & ~ovrAvatar2EntityFeatures_ShowControllers);
	}

	createInfo.renderFilters.lodFlags = ovrAvatar2EntityLODFlag_0;
	createInfo.renderFilters.manifestationFlags = ovrAvatar2EntityManifestationFlag_Half;
	createInfo.renderFilters.viewFlags = ovrAvatar2EntityViewFlag_All;

	ovrAvatar2EntityId entityId = ovrAvatar2EntityId_Invalid;
	if (ovrAvatar2Entity_Create(&createInfo, &entityId) != ovrAvatar2Result_Success)
	{
		Log::error() << "Failed to create entity";
		return;
	}

	ocean_assert(entityId != ovrAvatar2EntityId_Invalid);

	ocean_assert(bodyTrackingProvider_ == nullptr);
	if (ovrAvatar2Body_CreateProvider(ovrAvatar2BodyProviderCreateFlags_RunAsync, &bodyTrackingProvider_) != ovrAvatar2Result_Success)
	{
		Log::error() << "Failed to create body tracking context";
	}

	ovrAvatar2InputControlContext inputControlContext = {this, onDetermineInputControlState};
	if (ovrAvatar2Body_SetInputControlContext(bodyTrackingProvider_, &inputControlContext) != ovrAvatar2Result_Success)
	{
		Log::error() << "Failed to set input controller context";
	}

	ovrAvatar2InputTrackingContext inputTrackingContext = {this, onDetermineInputTrackingState};
	if (ovrAvatar2Body_SetInputTrackingContext(bodyTrackingProvider_, &inputTrackingContext) != ovrAvatar2Result_Success)
	{
		Log::error() << "Failed to set input tracking context";
	}

	ovrAvatar2HandTrackingDataContext handTrackingDataContext = {this, onDetermineHandTrackingState};
	if (ovrAvatar2Body_SetHandTrackingContext(bodyTrackingProvider_, &handTrackingDataContext) != ovrAvatar2Result_Success)
	{
		Log::error() << "Failed to set hand tracking context";
	}

	ovrAvatar2TrackingDataContext trackingDataContext;
	if (ovrAvatar2Body_InitializeDataContext(bodyTrackingProvider_, &trackingDataContext) == ovrAvatar2Result_Success)
	{
		if (ovrAvatar2Tracking_SetBodyTrackingContext(entityId, &trackingDataContext) == ovrAvatar2Result_Success)
		{
			ovrAvatar2EntityLoadSettings loadSettings = ovrAvatar2Entity_DefaultLoadSettings();
			loadSettings.loadFilters = createInfo.renderFilters;

			ovrAvatar2LoadRequestId loadRequestId;

			std::string readableGraphType;
			const ovrAvatar2Graph graphType = translateUserType(userType_, &readableGraphType);

			Log::debug() << "Avatars: Loading user " << userId << " with graph type '" << readableGraphType << "'";

			const ovrAvatar2Result result = ovrAvatar2Entity_LoadUserFromGraph(entityId, userId, graphType, loadSettings, &loadRequestId);

			if (result == ovrAvatar2Result_MissingAccessToken)
			{
				Log::error() << "Avatars: Missing access token for user: " << userId;
			}

			ocean_assert_and_suppress_unused(result == ovrAvatar2Result_Pending, result);

			userId_ = userId;
			entityId_ = entityId;

			if (isLocal)
			{
				const ovrAvatar2LipSyncProviderConfig lipSyncProviderConfig = ovrAvatar2LipSync_DefaultProviderConfig();

				ocean_assert(localLipSyncProvider_ == nullptr);
				if (ovrAvatar2LipSync_CreateProvider(&lipSyncProviderConfig, &localLipSyncProvider_) == ovrAvatar2Result_Success)
				{
					ocean_assert(!microphone_);
					microphone_ = Media::Manager::get().newMedium("Gained Voice Microphone", Media::Medium::MICROPHONE);

					if (microphone_)
					{
						microphoneSubscription_ = microphone_->addSamplesCallback(std::bind(&Avatar::onMicrophoneSamples, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
						ocean_assert(microphoneSubscription_.isValid());

						ocean_assert(localLipSyncContext_.context == nullptr && localLipSyncContext_.lipsyncCallback == nullptr);
						if (ovrAvatar2LipSync_InitializeContext(localLipSyncProvider_, &localLipSyncContext_) != ovrAvatar2Result_Success)
						{
							Log::error() << "Failed to initialize lip sync context";
						}

						if (!microphone_->start())
						{
							Log::error() << "Failed to start microphone";
						}
					}
				}
				else
				{
					Log::error() << "Failed to create viseme context";
				}
			}

			// we create a custom lip sync context (it's a proxy context for local avatars) to support local and remote avatars

			ovrAvatar2LipSyncContext customLipSyncContext;
			customLipSyncContext.context = this;
			customLipSyncContext.lipsyncCallback = onDetermineLipSyncState;

			if (ovrAvatar2Tracking_SetLipSyncContext(entityId, &customLipSyncContext) != ovrAvatar2Result_Success)
			{
				Log::error() << "Failed to set lip sync context";
			}

			return;
		}
		else
		{
			Log::error() << "Failed to set body tracking context";
		}
	}
	else
	{
		Log::error() << "Failed to create body tracking data context";
	}

	ovrAvatar2Entity_Destroy(entityId);
}

Avatar::~Avatar()
{
	release();
}

void Avatar::updateBodyTrackingData(Input::SharedBodyTrackingData&& remoteBodyTrackingData, const Scalar yGroundPlaneElevation)
{
	yGroundPlaneElevation_ = yGroundPlaneElevation;

	bodyTrackingData_ = std::move(remoteBodyTrackingData);

	if (bodyTrackingData_)
	{
		if (isLocal_)
		{
			// normalizing the body tracking data with the local ground plane elevation
			bodyTrackingData_->avatarInputTrackingState_.headset.position.y -= float(yGroundPlaneElevation_);

			for (unsigned int sideIndex = 0u; sideIndex < ovrAvatar2Side_Count; ++sideIndex)
			{
				bodyTrackingData_->avatarInputTrackingState_.controller[sideIndex].position.y -= float(yGroundPlaneElevation_);
				bodyTrackingData_->avatarHandTrackingState_.wristPose[sideIndex].position.y -= float(yGroundPlaneElevation_);
			}
		}
		else
		{
			remoteHeadsetWorld_T_remoteAvatar_ = HomogenousMatrix4(bodyTrackingData_->remoteHeadsetWorld_T_remoteAvatar_);
		}

		if (renderingTransform_ && !hasUpdatedBodyTrackingData_)
		{
			hasUpdatedBodyTrackingData_ = true;

			// HACK: for the very first requested tracking state, we need claim an active controller, otherwise the Avatars SDK will not work as expected

			ovrAvatar2InputTrackingState& avatarInputTrackingState = bodyTrackingData_->avatarInputTrackingState_;

			avatarInputTrackingState.controllerActive[0] = true;
			avatarInputTrackingState.controllerActive[1] = true;
		}
	}
}

bool Avatar::updateRendering(const Primitives& primitives, Shaders& shaders, const Rendering::Engine& engine, const bool renderRemoteAvatars, const ovrAvatar2EntityId overwriteEntityId)
{
	if (!isValid())
	{
		return false;
	}

	const ovrAvatar2EntityId useEntityId = overwriteEntityId != ovrAvatar2EntityId_Invalid ? overwriteEntityId : entityId_;
	ocean_assert(useEntityId != ovrAvatar2EntityId_Invalid);

	const uint32_t numberAssets = ovrAvatar2Entity_GetNumLoadedAssets(useEntityId);

	if (numberAssets != 0u)
	{
		bool defaultModelLoaded = false;
		bool userModelLoaded = false;

		std::vector<ovrAvatar2EntityAssetType> assetTypes(numberAssets);

		if (ovrAvatar2Entity_GetLoadedAssetTypes(useEntityId, assetTypes.data(), numberAssets * sizeof(ovrAvatar2EntityAssetType)) == ovrAvatar2Result_Success)
		{
			for (const ovrAvatar2EntityAssetType& assetType : assetTypes)
			{
				if (assetType == ovrAvatar2EntityAssetType_SystemDefaultModel)
				{
					defaultModelLoaded = true;
				}
				else if (assetType == ovrAvatar2EntityAssetType_Other)
				{
					userModelLoaded = true;
				}
			}

			if (defaultModelLoaded && userModelLoaded)
			{
				ocean_assert(isDefaultSystemModel_);
				isDefaultSystemModel_ = false;

				// we have the user model, so that we now can unload the default model

				if (ovrAvatar2Entity_UnloadDefaultModel(useEntityId) != ovrAvatar2Result_Success)
				{
					Log::error() << "Failed to unload default model";
				}
			}
		}
	}

	ovrAvatar2EntityRenderState entityRenderState;
	if (ovrAvatar2Render_QueryRenderState(useEntityId, &entityRenderState) != ovrAvatar2Result_Success)
	{
		Log::error() << "Failed to determine render state";
		return false;
	}

	for (uint32_t nPrimitive = 0u; nPrimitive < entityRenderState.primitiveCount; ++nPrimitive)
	{
		ovrAvatar2PrimitiveRenderState primitiveRenderState;
		if (ovrAvatar2Render_GetPrimitiveRenderStateByIndex(useEntityId, nPrimitive, &primitiveRenderState) != ovrAvatar2Result_Success)
		{
			Log::error() << "Failed to determine primitive render state";
			continue;
		}

		ocean_assert(primitiveRenderState.meshNodeId != ovrAvatar2NodeId_Invalid);
		NodeMap::iterator iNode = nodeMap_.find(primitiveRenderState.meshNodeId);
		if (iNode == nodeMap_.cend())
		{
			ocean_assert(primitiveRenderState.primitiveId != ovrAvatar2PrimitiveId_Invalid);
			iNode = nodeMap_.emplace(primitiveRenderState.meshNodeId, Node(primitiveRenderState.primitiveId)).first;
		}
		else
		{
			ocean_assert(iNode->second.primitiveId_ == primitiveRenderState.primitiveId);
		}

		Node& node = iNode->second;

		const Primitives::PrimitiveObject* primitiveObject = primitives.primitiveObject(primitiveRenderState.primitiveId);
		if (primitiveObject == nullptr)
		{
			// the primitive has not been loaded (e.g., because the primitive has a view flag not of interest)
			continue;
		}

		const unsigned int jointCount = primitiveObject->jointCount_;

		static_assert(sizeof(ovrAvatar2Matrix4f) == sizeof(SquareMatrixF4), "Invalid data type!");

		skinMatrices_.resize(jointCount * 2u);
		if (ovrAvatar2Render_GetSkinTransforms(useEntityId, primitiveRenderState.id, (ovrAvatar2Matrix4f*)(skinMatrices_.data()), skinMatrices_.size() * sizeof(HomogenousMatrixF4), true /*interleaveNormalMatrix*/) != ovrAvatar2Result_Success)
		{
			Log::error() << "Failed to determine skin transformations";
			continue;
		}

		if (primitiveObject->skinMatricesTexture_ && !Shaders::updateSkinMatricesTexture(*primitiveObject->skinMatricesTexture_, skinMatrices_))
		{
			Log::error() << "Failed to update skin matrices texture";
		}

		if (node.geometry_.isNull())
		{
			Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();
			attributeSet->addAttribute(shaders.shaderProgram(engine));

			ocean_assert(primitiveObject->textures_);
			attributeSet->addAttribute(primitiveObject->textures_);

			node.geometry_ = engine.factory().createGeometry();
			node.geometry_->setVisible(false);

			ocean_assert(primitiveObject->triangles_);
			node.geometry_->addRenderable(primitiveObject->triangles_, attributeSet);
		}
	}

	if (renderingTransform_.isNull())
	{
		renderingTransform_ = engine.factory().createTransform();

		// we hide the group until we have the first valid body tracking data
		renderingTransform_->setVisible(false);
	}

	if (isLocal_)
	{
		if (!renderingTransform_->visible())
		{
			// workaround due to issue in Avatars, we do not render the Avatar before either controller or hand tracking have been active once

			if (bodyTrackingData_ && bodyTrackingData_->avatarInputTrackingState_.headsetActive)
			{
				const bool controllerActive = bodyTrackingData_->avatarInputTrackingState_.controllerActive[0] || bodyTrackingData_->avatarInputTrackingState_.controllerActive[1];
				const bool handsTracked = bodyTrackingData_->avatarHandTrackingState_.isTracked[0] || bodyTrackingData_->avatarHandTrackingState_.isTracked[1];

				renderingTransform_->setVisible(controllerActive || handsTracked);
			}
		}
	}
	else
	{
		// this avatar is an avatar of a remote user, so we apply the custom transformation between avatar and world

		renderingTransform_->setVisible(renderRemoteAvatars && remoteHeadsetWorld_T_remoteAvatar_.isValid());

		if (remoteHeadsetWorld_T_remoteAvatar_.isValid())
		{
			renderingTransform_->setTransformation(remoteHeadsetWorld_T_remoteAvatar_);
		}
	}

	if (entityRenderState.allNodesVersion != nodesVersion_)
	{
		renderingTransform_->clear();

		for (uint32_t nNode = 0u; nNode < entityRenderState.allMeshNodesCount; ++nNode)
		{
			const ovrAvatar2NodeId nodeId = entityRenderState.allMeshNodes[nNode];

			const NodeMap::const_iterator iNode = nodeMap_.find(nodeId);

			if (iNode != nodeMap_.cend())
			{
				// ensure that the geometry is already loaded

				if (iNode->second.geometry_)
				{
					renderingTransform_->addChild(iNode->second.geometry_);
				}
			}
		}

		nodesVersion_ = entityRenderState.allNodesVersion;
	}

	if (entityRenderState.visibleNodesVersion != visibilityVersion_)
	{
		NodeSet visibleNodeIds;

		if (entityRenderState.visibleMeshNodesCount != 0u)
		{
			visibleNodeIds = NodeSet(entityRenderState.visibleMeshNodes, entityRenderState.visibleMeshNodes + entityRenderState.visibleMeshNodesCount);
		}

		for (NodeMap::const_iterator iNode = nodeMap_.cbegin(); iNode != nodeMap_.cend(); ++iNode)
		{
			// ensure that the geometry is already loaded

			if (iNode->second.geometry_)
			{
				const bool nodeIsVisible = visibleNodeIds.find(iNode->first) != visibleNodeIds.cend();
				iNode->second.geometry_->setVisible(nodeIsVisible);
			}
		}

		visibilityVersion_ = entityRenderState.visibleNodesVersion;
	}

	return true;
}

Rendering::TransformRef Avatar::createAvatar(const Rendering::Engine& engine) const
{
	if (!isValid() || renderingTransform_.isNull())
	{
		return Rendering::TransformRef();
	}

	Rendering::TransformRef externalTransform = engine.factory().createTransform();

	externalTransform->addChild(renderingTransform_);

	return externalTransform;
}

void Avatar::release()
{
	if (entityId_ != ovrAvatar2EntityId_Invalid)
	{
		microphoneSubscription_.release();
		microphone_.release();

		if (localLipSyncProvider_ != nullptr)
		{
			if (ovrAvatar2LipSync_DestroyProvider(localLipSyncProvider_) != ovrAvatar2Result_Success)
			{
				Log::error() << "Failed to destroy viseme context";
			}

			localLipSyncProvider_ = nullptr;
		}

		if (bodyTrackingProvider_ != nullptr)
		{
			if (ovrAvatar2Body_DestroyProvider(bodyTrackingProvider_) != ovrAvatar2Result_Success)
			{
				Log::error() << "Failed to destroy tracking context";
			}

			bodyTrackingProvider_ = nullptr;
		}

		ovrAvatar2Entity_Destroy(entityId_);
		entityId_ = ovrAvatar2EntityId_Invalid;

		nodesVersion_ = ovrAvatar2EntityRenderStateVersion_Invalid;
		visibilityVersion_ = ovrAvatar2EntityRenderStateVersion_Invalid;

		nodeMap_.clear();

		renderingTransform_.release();
	}

	userId_ = 0ull;
}

void Avatar::onMicrophoneSamples(const Media::Microphone::SamplesType samplesType, const void* data, const size_t size)
{
	ocean_assert(isLocal_);
	ocean_assert(samplesType != Media::Microphone::ST_INVALID);
	ocean_assert(data != nullptr && size != 0);

	if (localLipSyncProvider_ != nullptr)
	{
		ovrAvatar2AudioDataFormat audioDataFormat = ovrAvatar2AudioDataFormat(-1);
		uint32_t numberSamples = 0u;

		switch (samplesType)
		{
			case Media::Microphone::ST_INVALID:
				ocean_assert(false && "Invalid samples type!");
				return;

			case Media::Microphone::ST_INTEGER_16_MONO_48:
				audioDataFormat = ovrAvatar2AudioDataFormat_S16_Mono;

				ocean_assert(size % sizeof(int16_t) == 0);
				numberSamples = size / sizeof(int16_t);
				break;
		}

		if (audioDataFormat != ovrAvatar2AudioDataFormat(-1) && numberSamples != 0u)
		{
			if (ovrAvatar2LipSync_FeedAudio(localLipSyncProvider_, ovrAvatar2AudioDataFormat_S16_Mono, data, numberSamples) != ovrAvatar2Result_Success)
			{
				Log::error() << "Failed to feed audio";
			}
		}
	}
}

bool Avatar::onDetermineInputControlState(ovrAvatar2InputControlState* inputControlState, void* userContext)
{
	ocean_assert(userContext != nullptr);

	if (inputControlState == nullptr)
	{
		return false;
	}

	Avatar* avatar = reinterpret_cast<Avatar*>(userContext);
	ocean_assert(avatar != nullptr);

	if (avatar->bodyTrackingData_)
	{
		*inputControlState = avatar->bodyTrackingData_->avatarInputControlState_;
		return true;
	}

	return false;
}

bool Avatar::onDetermineInputTrackingState(ovrAvatar2InputTrackingState* inputTrackingState, void* userContext)
{
	ocean_assert(userContext != nullptr);

	if (inputTrackingState == nullptr)
	{
		return false;
	}

	Avatar* avatar = reinterpret_cast<Avatar*>(userContext);
	ocean_assert(avatar != nullptr);

	if (avatar->bodyTrackingData_)
	{
		*inputTrackingState = avatar->bodyTrackingData_->avatarInputTrackingState_;

		ovrAvatar2Vector3f& headsetPosition = inputTrackingState->headset.position;
		const ovrAvatar2Quatf& headsetOrientation = inputTrackingState->headset.orientation;

		headsetPosition.y += float(avatar->yGroundPlaneElevation_);

		const Vector3 position(Scalar(headsetPosition.x), Scalar(headsetPosition.y), Scalar(headsetPosition.z));
		const Quaternion orientation(Scalar(headsetOrientation.w), Scalar(headsetOrientation.x), Scalar(headsetOrientation.y), Scalar(headsetOrientation.z));

		if (orientation.isValid())
		{
			avatar->world_T_head_ = HomogenousMatrix4(position, orientation);

			for (unsigned int controllerIndex = 0u; controllerIndex < ovrAvatar2Side_Count; ++controllerIndex)
			{
				inputTrackingState->controller[controllerIndex].position.y += float(avatar->yGroundPlaneElevation_);
			}

			return true;
		}
		else
		{
			Log::debug() << "Avatar::onDetermineInputTrackingState(): Invalid rotation";
		}
	}

	return false;
}

bool Avatar::onDetermineHandTrackingState(ovrAvatar2HandTrackingState* handTrackingState, void* userContext)
{
	ocean_assert(userContext != nullptr);

	if (handTrackingState == nullptr)
	{
		return false;
	}

	Avatar* avatar = reinterpret_cast<Avatar*>(userContext);
	ocean_assert(avatar != nullptr);

	if (avatar->bodyTrackingData_)
	{
		*handTrackingState = avatar->bodyTrackingData_->avatarHandTrackingState_;

		for (unsigned int handIndex = 0u; handIndex < ovrAvatar2Side_Count; ++handIndex)
		{
			handTrackingState->wristPose[handIndex].position.y += float(avatar->yGroundPlaneElevation_);
		}

		return true;
	}

	return false;
}

bool Avatar::onDetermineLipSyncState(ovrAvatar2LipSyncState* lipSyncState, void* userContext)
{
	ocean_assert(userContext != nullptr);

	if (lipSyncState == nullptr)
	{
		return false;
	}

	Avatar* avatar = reinterpret_cast<Avatar*>(userContext);
	ocean_assert(avatar != nullptr);

	if (avatar->isLocal_)
	{
		// we are a local avatar, so we determine the lip sync state via the real lip sync context

		if (avatar->localLipSyncContext_.lipsyncCallback != nullptr && avatar->localLipSyncContext_.lipsyncCallback(lipSyncState, avatar->localLipSyncContext_.context))
		{
			if (avatar->bodyTrackingData_)
			{
				// we also make a copy so that it can be send via network
				avatar->bodyTrackingData_->avatarLipSyncState_ = *lipSyncState;
			}

			return true;
		}
	}
	else
	{
		// we are a remote avatar, so we forward the lip sync data which we have received via network

		if (avatar->bodyTrackingData_)
		{
			// we also make a copy so that it can be send via network
			*lipSyncState = avatar->bodyTrackingData_->avatarLipSyncState_;

			return true;
		}
	}

	return false;
}

ovrAvatar2Graph Avatar::translateUserType(const UserType userType, std::string* readable)
{
	switch (userType)
	{
		case UT_UNKNOWN:
			break;

		case UT_META:
		{
			if (readable != nullptr)
			{
				*readable = "ovrAvatar2Graph_Meta";
			}
			return ovrAvatar2Graph_Meta;
		}

		case UT_FACEBOOK:
		{
			if (readable != nullptr)
			{
				*readable = "ovrAvatar2Graph_Facebook";
			}
			return ovrAvatar2Graph_Facebook;
		}

		case UT_INSTAGRAM:
		{
			if (readable != nullptr)
			{
				*readable = "ovrAvatar2Graph_Instagram";
			}
			return ovrAvatar2Graph_Instagram;
		}

		case UT_OCULUS:
		{
			if (readable != nullptr)
			{
				*readable = "ovrAvatar2Graph_Oculus";
			}
			return ovrAvatar2Graph_Oculus;
		}

#if defined(AVATAR2_RELEASE_VERSION) && AVATAR2_RELEASE_VERSION >= 23
		case UT_WHATSAPP:
		{
			if (readable != nullptr)
			{
				*readable = "ovrAvatar2Graph_WhatsApp";
			}
			return ovrAvatar2Graph_WhatsApp;
		}
#endif

		case UT_END:
			break;
	}

	ocean_assert(false && "This must never happen!");

	if (readable != nullptr)
	{
		*readable = "ovrAvatar2Graph_Invalid";
	}

	return ovrAvatar2Graph_Invalid;
}

Avatar::UserType Avatar::translateUserType(const ovrAvatar2Graph graphType)
{
	switch (graphType)
	{
		case ovrAvatar2Graph_Invalid:
			return UT_UNKNOWN;

		case ovrAvatar2Graph_Meta:
			return UT_META;

		case ovrAvatar2Graph_Facebook:
			return UT_FACEBOOK;

		case ovrAvatar2Graph_Instagram:
			return UT_INSTAGRAM;

		case ovrAvatar2Graph_Oculus:
			return UT_OCULUS;

#if defined(AVATAR2_RELEASE_VERSION) && AVATAR2_RELEASE_VERSION >= 23
		case ovrAvatar2Graph_WhatsApp:
			return UT_WHATSAPP;
#endif

		case ovrAvatar2Graph_EnumSize:
			break;
	}

	ocean_assert(false && "This must never happen!");
	return UT_UNKNOWN;
}

} // namespace Avatars

} // namespace Meta

} // namespace Platform

} // namespace Ocean
