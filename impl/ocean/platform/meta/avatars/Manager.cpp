// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/avatars/Manager.h"

#include "ocean/io/Compression.h"
#include "ocean/io/File.h"
#include "ocean/io/FileResolver.h"

#include "metaonly/ocean/network/verts/Manager.h"
#include "metaonly/ocean/network/verts/NodeSpecification.h"

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	#include "ocean/platform/android/Resource.h"
#endif

#include <ovrAvatar2/Entity.h>

#ifdef OCEAN_PLATFORM_META_AVATARS_NOTQUEST
	#include <ovrAvatar2/internal/Graphs.h>
#endif

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

Manager::ScopedLocalFile::ScopedLocalFile(const std::string& filename) :
	filename_(filename)
{
	// nothing to do here
}

const void* Manager::ScopedLocalFile::data()
{
	if (!memory_)
	{
		if (!readFile())
		{
			Log::error() << "Avatars: Failed to open resource '" << filename_ << "'";
			return nullptr;
		}
	}

	return memory_.data();
}

size_t Manager::ScopedLocalFile::size()
{
	if (!memory_)
	{
		if (!readFile())
		{
			Log::error() << "Avatars: Failed to open resource '" << filename_ << "'";
			return 0;
		}
	}

	return memory_.size();
}

bool Manager::ScopedLocalFile::readFile()
{
	std::ifstream stream(filename_.c_str(), std::ios::binary);

	if (stream.fail())
	{
		return false;
	}

	stream.seekg(0, std::ios_base::end);
	size_t size = stream.tellg();
	stream.seekg(0, std::ios_base::beg);

	if (size == 0)
	{
		Log::error() << "Avatars: The resource '" << filename_ << "' is empty";
		return false;
	}

	memory_ = Memory(size);

	stream.read((char*)memory_.data(), size);

	return stream.good();
}

std::unique_ptr<Manager::ScopedLocalFile> Manager::ScopedLocalFile::create(const char* filename)
{
	if (filename == nullptr)
	{
		ocean_assert(false && "Invalid input!");
		return nullptr;
	}

	const IO::Files files = IO::FileResolver::get().resolve(IO::File(filename), true);

	if (files.empty())
	{
		Log::warning() << "Avatars: Could not resolve resource '" << filename << "'";
		return nullptr;
	}

	return std::unique_ptr<ScopedLocalFile>(new ScopedLocalFile(files.front()()));
}

inline Manager::Zone::Zone(Network::Verts::SharedDriver driver, Network::Verts::Driver::NewEntityScopedSubscription newEntityScopedSubscription, bool useVoip) :
	usageCounter_(1u),
	driver_(std::move(driver)),
	newEntityScopedSubscription_(std::move(newEntityScopedSubscription))
{
	if (useVoip)
	{
		voipScopedSubscription_ = Network::Verts::Voip::get().startVoip(driver_->zoneName());
	}
}

bool Manager::initialize(const ovrAvatar2Platform ovrPlatform, const UserType userType, const uint64_t userId, const std::string& accessToken, const std::string& clientName, const std::string& clientVersion, const bool useVoip)
{
	if (userType == Avatar::UT_UNKNOWN || userId == 0ull || accessToken.empty() || clientVersion.empty())
	{
		ocean_assert(false && "Invalid input!");
		return false;
	}

#ifdef OCEAN_PLATFORM_META_AVATARS_QUEST
	if (userType != Avatar::UT_OCULUS)
	{
		Log::error() << "Avatars: Invalid user type, must be Oculus";
		return false;
	}
#endif

	const ScopedLock scopedLock(managerLock_);

	if (initialized_)
	{
		ocean_assert(false && "Already initialized!");
		return false;
	}

	ovrAvatar2InitializeInfo initializeInfo = ovrAvatar2_DefaultInitInfo(clientName.c_str(), clientVersion.c_str(), ovrPlatform);

	initializeInfo.loggingLevel = ovrAvatar2LogLevel_Info; // ovrAvatar2LogLevel_Verbose
	initializeInfo.loggingCallback = onLogging;
	initializeInfo.loggingContext = this;

	initializeInfo.fileOpenCallback = onFileOpen;
	initializeInfo.fileReadCallback = onFileRead;
	initializeInfo.fileCloseCallback = onFileClose;
	initializeInfo.fileReaderContext = this;

	initializeInfo.fileOpenCallback = onFileOpen;
	initializeInfo.fileReadCallback = onFileRead;
	initializeInfo.fileCloseCallback = onFileClose;
	initializeInfo.fileReaderContext = this;

	initializeInfo.resourceCallback = onResource;
	initializeInfo.resourceContext = this;

	initializeInfo.requestCallback = onRequest;

	const ovrAvatar2Result result = ovrAvatar2_Initialize(&initializeInfo);
	if (result != ovrAvatar2Result_Success)
	{
		Log::error() << "Avatars: Failed to initialize Avatar2: " << int(result);
		return false;
	}

	initialized_ = true;
	userType_ = userType;
	userId_ = userId;

	if (!accessToken.empty())
	{
		std::string readableGraphType;
		const ovrAvatar2Graph graphType = Avatar::translateUserType(userType, &readableGraphType);

		if (ovrAvatar2_UpdateAccessTokenForGraph(accessToken.c_str(), graphType) == ovrAvatar2Result_Success)
		{
			Log::debug() << "Set access token for graph type '" << readableGraphType << "'";
		}
		else
		{
			Log::error() << "Avatars: Could not update graph access token";
		}
	}

	// we specify the node which we use to store the user data

	Network::Verts::NodeSpecification& bodyTrackingNodeSpecification = Network::Verts::NodeSpecification::newNodeSpecification(vertsAvatarNodeType_);
	bodyTrackingNodeSpecification.registerField<uint64_t>("userId");
	bodyTrackingNodeSpecification.registerField<int32_t>("userType");
	bodyTrackingNodeSpecification.registerField<std::string>("data");

	useVoip_ = useVoip;

	return true;
}

bool Manager::setAccessToken(const UserType userType, const std::string& accessToken)
{
	ocean_assert(userType != Avatar::UT_UNKNOWN);

	const ScopedLock scopedLock(managerLock_);

	std::string readableGraphType;
	const ovrAvatar2Graph graphType = Avatar::translateUserType(userType, &readableGraphType);

	if (ovrAvatar2_UpdateAccessTokenForGraph(accessToken.c_str(), graphType) == ovrAvatar2Result_Success)
	{
		Log::debug() << "Set access token for graph type '" << readableGraphType << "'";
		return true;
	}

	Log::error() << "Avatars: Could not set/update graph access token";

	return false;
}

Manager::~Manager()
{
	release();
}

void Manager::updateLocalBodyTrackingData(Input::SharedBodyTrackingData&& bodyTrackingData)
{
	const ScopedLock scopedLock(managerLock_);

	recentLocalBodyTrackerData_ = std::move(bodyTrackingData);
}

void Manager::update(const Rendering::EngineRef& engine, const double timestamp)
{
	TemporaryScopedLock scopedLock(managerLock_);

	if (!initialized_)
	{
		return;
	}

	const float delta = lastTimestamp_.isInvalid() ? 0.0f : float(timestamp - double(lastTimestamp_));
	lastTimestamp_ = timestamp;

	UserPairs addedAvatarPairs;
	Indices64 removedAvatarIds;

	for (ZoneMap::iterator iZone = zoneMap_.begin(); iZone != zoneMap_.end(); ++iZone)
	{
		const std::string& zoneName = iZone->first;
		Zone& zone = iZone->second;

		addedAvatarPairs.clear();
		removedAvatarIds.clear();

		for (size_t nNode = 0; nNode < zone.remoteAvatarNodes_.size(); /*noop*/)
		{
			Network::Verts::SharedNode& remoteAvatarNode = zone.remoteAvatarNodes_[nNode];

			if (remoteAvatarNode->hasChanged())
			{
				const uint64_t remoteUserId = remoteAvatarNode->field<uint64_t>(0u);
				const Network::Verts::Node::StringPointer trackingData = remoteAvatarNode->field<Network::Verts::Node::StringPointer>(2u);

				IO::Compression::Buffer decompressedBuffer;
				if (IO::Compression::gzipDecompress(trackingData.data(), trackingData.size(), decompressedBuffer))
				{
					if (decompressedBuffer.size() == sizeof(Input::BodyTrackingData))
					{
						Input::SharedBodyTrackingData remoteBodyTrackingData = std::make_shared<Input::BodyTrackingData>();
						memcpy(remoteBodyTrackingData.get(), decompressedBuffer.data(), sizeof(Input::BodyTrackingData));

						remoteBodyTrackingMap_[remoteUserId] = std::move(remoteBodyTrackingData);
					}
					else
					{
						Log::warning() << "Avatars: Invalid body tracking data, got " << trackingData.size() << " bytes but need " << sizeof(Input::BodyTrackingData) << " bytes";
					}
				}
				else
				{
					Log::error() << "Avatars: Failed to unpack body tracking data";
				}

				if (zone.remoteUserIds_.find(remoteUserId) == zone.remoteUserIds_.cend())
				{
					const UserType userType = UserType(remoteAvatarNode->field<int32_t>(1u));

					ocean_assert(userType != Avatar::UT_UNKNOWN && userType < Avatar::UT_END);
					if (userType != Avatar::UT_UNKNOWN && userType < Avatar::UT_END)
					{
						zone.remoteUserIds_.emplace(remoteUserId);

						addedAvatarPairs.emplace_back(remoteUserId, userType);
					}
				}
			}
			else if (remoteAvatarNode->hasBeenDeleted())
			{
				const uint64_t remoteUserId = remoteAvatarNode->field<uint64_t>(0u);

				const UnorderedIndexSet64::const_iterator i = zone.remoteUserIds_.find(remoteUserId);

				if (i != zone.remoteUserIds_.cend())
				{
					removedAvatarIds.emplace_back(remoteUserId);

					zone.remoteUserIds_.erase(i);
				}

				zone.remoteAvatarNodes_[nNode] = zone.remoteAvatarNodes_.back();
				zone.remoteAvatarNodes_.pop_back();

				continue;
			}

			++nNode;
		}

		if (!addedAvatarPairs.empty() || !removedAvatarIds.empty())
		{
			changedAvatarsCallbackHandler_.callCallbacks(zoneName, addedAvatarPairs, removedAvatarIds);
		}
	}

	// first, we update the Avatars' bodies

	for (AvatarMap::iterator iAvatar = avatarMap_.begin(); iAvatar != avatarMap_.end(); ++iAvatar)
	{
		ocean_assert(iAvatar->second.first);
		Avatar& avatar = *iAvatar->second.first;

		if (avatar.isLocal())
		{
			avatar.updateBodyTrackingData(std::move(recentLocalBodyTrackerData_), yGroundPlaneElevation_);
			recentLocalBodyTrackerData_ = nullptr;
		}
		else
		{
			const RemoteBodyTrackingMap::iterator iRemoteBody = remoteBodyTrackingMap_.find(iAvatar->first);

			if (iRemoteBody != remoteBodyTrackingMap_.cend())
			{
				avatar.updateBodyTrackingData(std::move(iRemoteBody->second), yGroundPlaneElevation_);
				remoteBodyTrackingMap_.erase(iRemoteBody);
			}
			else
			{
				avatar.updateBodyTrackingData(nullptr, yGroundPlaneElevation_);
			}
		}
	}

	ovrAvatar2Result result = ovrAvatar2_Update(delta);
	if (result != ovrAvatar2Result_Success)
	{
		Log::error() << "Avatars: ovrAvatar2_Update error: " << int(result);
		return;
	}

	if (!zoneMap_.empty())
	{
		// we send the body tracking data of the local avatar via network

		const AvatarMap::const_iterator iAvatar = avatarMap_.find(userId_);

		if (iAvatar != avatarMap_.cend())
		{
			ocean_assert(iAvatar->second.first);
			const Avatar& avatar = *iAvatar->second.first;

			if (avatar.isLocal_ && avatar.bodyTrackingData_)
			{
				Input::BodyTrackingData& localBodyTrackingData = *avatar.bodyTrackingData_;

				localBodyTrackingData.remoteHeadsetWorld_T_remoteAvatar_ = HomogenousMatrixF4(avatar.remoteHeadsetWorld_T_remoteAvatar_);

				IO::Compression::Buffer compressedBuffer;
				if (IO::Compression::gzipCompress(&localBodyTrackingData, sizeof(localBodyTrackingData), compressedBuffer))
				{
					reusableNetworkBuffer_.resize(compressedBuffer.size());
					memcpy(reusableNetworkBuffer_.data(), compressedBuffer.data(), compressedBuffer.size());

					for (ZoneMap::iterator iZone = zoneMap_.begin(); iZone != zoneMap_.end(); ++iZone)
					{
						Zone& zone = iZone->second;

						if (!zone.localAvatarNode_ && zone.driver_->isInitialized())
						{
							Network::Verts::SharedEntity entity = zone.driver_->newEntity({vertsAvatarNodeType_});
							ocean_assert(entity);

							if (entity)
							{
								zone.localAvatarNode_ = entity->node(vertsAvatarNodeType_);
								ocean_assert(zone.localAvatarNode_);
							}
						}

						if (zone.localAvatarNode_)
						{
							zone.localAvatarNode_->setField(0u, userId_);
							zone.localAvatarNode_->setField(1u, int32_t(userType_));
							zone.localAvatarNode_->setField(2u, reusableNetworkBuffer_);
						}
					}
				}
				else
				{
					ocean_assert(false && "This should never happen!");
					Log::error() << "Avatars: Failed to compress body tracking data";
				}
			}
		}
	}

	scopedLock.release();
	scopedLock.relock(resourceLock_);

	while (!resourceEventQueue_.empty())
	{
		const ResourceEvent& resourceEvent = resourceEventQueue_.front();

		if (resourceEvent.status_ == ovrAvatar2AssetStatus_Loaded)
		{
			images_.loadImages(*engine, resourceEvent.resourceId_);
			primitives_.loadPrimitives(engine, resourceEvent.resourceId_, images_);

			if (ovrAvatar2Asset_ResourceReadyToRender(resourceEvent.resourceId_) != ovrAvatar2Result_Success)
			{
				Log::error() << "Avatars: Failed to set resource ready to render";
			}

			if (ovrAvatar2Asset_ReleaseResource(resourceEvent.resourceId_) != ovrAvatar2Result_Success)
			{
				Log::error() << "Avatars: Failed to release resource";
			}
		}
		else
		{
			ocean_assert(resourceEvent.status_ == ovrAvatar2AssetStatus_Unloaded);

			images_.unloadImages(resourceEvent.resourceId_);
			primitives_.unloadPrimitives(resourceEvent.resourceId_);
		}

		resourceEventQueue_.pop();
	}

	scopedLock.release();
	scopedLock.relock(managerLock_);

	// now, we can update the Avatars' rendering objects

	bool allAvatarsHaveDefaultSystemModels = true;

	for (AvatarMap::const_iterator iAvatar = avatarMap_.cbegin(); iAvatar != avatarMap_.cend(); ++iAvatar)
	{
		if (!iAvatar->second.first->isDefaultSystemModel())
		{
			allAvatarsHaveDefaultSystemModels = false;
			break;
		}
	}

	ovrAvatar2EntityId overwriteEntityId = ovrAvatar2EntityId_Invalid;

	if (allAvatarsHaveDefaultSystemModels && userId_ != 0ull)
	{
		// HACK: workaround to ensure that in case all avatars are system models, at least the remote avatar is rendered

		if (avatarMap_.size() > 2)
		{
			Log::warning() << "Avatars: Currently only one remote avatar can be rendered properly";
		}

		for (AvatarMap::iterator iAvatar = avatarMap_.begin(); iAvatar != avatarMap_.end(); ++iAvatar)
		{
			if (iAvatar->first != userId_)
			{
				overwriteEntityId = iAvatar->second.first->entityId();
			}
		}
	}

	for (AvatarMap::iterator iAvatar = avatarMap_.begin(); iAvatar != avatarMap_.end(); ++iAvatar)
	{
		iAvatar->second.first->updateRendering(primitives_, shaders_, *engine, renderRemoteAvatars_, overwriteEntityId);
	}

	// let's see whether new instances of avatars are requested

	for (size_t nCreate = 0; nCreate < createAvatarCallbacks_.size(); /*noop*/)
	{
		const uint64_t userId = createAvatarCallbacks_[nCreate].first;
		const CreateAvatarCallback& createAvatarCallback = createAvatarCallbacks_[nCreate].second;

		const AvatarMap::const_iterator iAvatar = avatarMap_.find(userId);
		if (iAvatar == avatarMap_.cend())
		{
			// the user id does not exist (e.g., anymore)
			createAvatarCallback(userId, Rendering::TransformRef());
		}
		else
		{
			Rendering::TransformRef transform = iAvatar->second.first->createAvatar(*engine);

			if (transform)
			{
				createAvatarCallback(userId, std::move(transform));
			}
			else
			{
				// the avatar has not yet been loaded

				++nCreate;
				continue;
			}
		}

		createAvatarCallbacks_[nCreate] = createAvatarCallbacks_.back();
		createAvatarCallbacks_.pop_back();
	}
}

Manager::AvatarScopedSubscription Manager::loadUser(const uint64_t userId, const UserType userType)
{
	if (userId == 0ull)
	{
		return AvatarScopedSubscription();
	}

	const ScopedLock scopedLock(managerLock_);

	if (!initialized_)
	{
		return AvatarScopedSubscription();
	}

	ocean_assert(userType_ != Avatar::UT_UNKNOWN);

	const bool isLocal = userId == userId_;

	AvatarMap::iterator iAvatar = avatarMap_.find(userId);

	if (iAvatar != avatarMap_.cend())
	{
		AvatarUsagePair& avatarUsagePair = iAvatar->second;

		ocean_assert(avatarUsagePair.first);
		ocean_assert(avatarUsagePair.second >= 1u);

		++avatarUsagePair.second;

		return AvatarScopedSubscription(userId, std::bind(&Manager::unloadUser, this, std::placeholders::_1));
	}

	std::shared_ptr<Avatar> avatar(new Avatar(userType == Avatar::UT_UNKNOWN ? userType_ : userType, userId, isLocal));
	if (!avatar->isValid())
	{
		return AvatarScopedSubscription();
	}

	avatarMap_.emplace(userId, AvatarUsagePair(std::move(avatar), 1u));

	return AvatarScopedSubscription(userId, std::bind(&Manager::unloadUser, this, std::placeholders::_1));
}

Manager::AvatarScopedSubscription Manager::createAvatar(const uint64_t userId, CreateAvatarCallback createAvatarCallback, const UserType userType)
{
	if (userId == 0ull || !createAvatarCallback)
	{
		return AvatarScopedSubscription();
	}

	const ScopedLock scopedLock(managerLock_);

	AvatarScopedSubscription avatarScopedSubscription = loadUser(userId, userType);

	if (!avatarScopedSubscription)
	{
		AvatarScopedSubscription();
	}

	createAvatarCallbacks_.emplace_back(userId, std::move(createAvatarCallback));

	return avatarScopedSubscription;
}

Manager::ZoneScopedSubscription Manager::joinZone(const std::string& zoneName)
{
	ocean_assert(!zoneName.empty());
	if (zoneName.empty())
	{
		return ZoneScopedSubscription();
	}

	ZoneMap::iterator iZone = zoneMap_.find(zoneName);

	if (iZone == zoneMap_.cend())
	{
		Network::Verts::SharedDriver driver = Network::Verts::Manager::get().driver(zoneName);

		if (driver)
		{
			Network::Verts::Driver::NewEntityScopedSubscription subscription = driver->addNewEntityCallback(std::bind(&Manager::onNewVertsEntity, this, std::placeholders::_1, std::placeholders::_2), vertsAvatarNodeType_);
			zoneMap_.emplace(zoneName, Zone(std::move(driver), std::move(subscription), useVoip_));
		}
	}
	else
	{
		ocean_assert(iZone->second.usageCounter_ >= 1u);
		++iZone->second.usageCounter_;
	}

	return ZoneScopedSubscription(zoneName, std::bind(&Manager::leaveZone, this, std::placeholders::_1));
}

bool Manager::setRemoteHeadsetWorld_T_remoteAvatar(const HomogenousMatrix4& remoteHeadsetWorld_T_remoteAvatar)
{
	const ScopedLock scopedLock(managerLock_);

	if (userId_ == 0ull)
	{
		return false;
	}

	const AvatarMap::iterator iAvatar = avatarMap_.find(userId_);

	if (iAvatar == avatarMap_.cend())
	{
		ocean_assert(false && "This must never happen!");
		return false;
	}

	iAvatar->second.first->setRemoteHeadsetWorld_T_remoteAvatar(remoteHeadsetWorld_T_remoteAvatar);

	if (remoteHeadsetWorld_T_remoteAvatar.isValid())
	{
		Network::Verts::Voip::get().updatePosition(remoteHeadsetWorld_T_remoteAvatar.translation());
	}

	renderRemoteAvatars_ = remoteHeadsetWorld_T_remoteAvatar.isValid();

	return true;
}

HomogenousMatrix4 Manager::remoteHeadsetWorld_T_remoteAvatar(const uint64_t userId) const
{
	ocean_assert(userId != 0ull);

	const ScopedLock scopedLock(managerLock_);

	const AvatarMap::const_iterator iAvatar = avatarMap_.find(userId_);

	if (iAvatar == avatarMap_.cend())
	{
		return HomogenousMatrix4(false);
	}

	return iAvatar->second.first->remoteHeadsetWorld_T_remoteAvatar();
}

HomogenousMatrix4 Manager::world_T_head(const uint64_t userId) const
{
	ocean_assert(userId != 0ull);

	const ScopedLock scopedLock(managerLock_);

	const AvatarMap::const_iterator iAvatar = avatarMap_.find(userId);

	if (iAvatar == avatarMap_.cend())
	{
		return HomogenousMatrix4(false);
	}

	return iAvatar->second.first->world_T_head();
}

void Manager::release()
{
	const ScopedLock scopedLock(managerLock_);

	zoneMap_.clear();

	avatarMap_.clear();
	shaders_.release();
	images_.release();
	primitives_.release();

	if (initialized_)
	{
		if (ovrAvatar2_Shutdown() != ovrAvatar2Result_Success)
		{
			Log::error() << "Avatars: Failed to shutdown Avatar2";
		}

		initialized_ = false;
	}
}

void Manager::leaveZone(const std::string& zoneName)
{
	ocean_assert(!zoneName.empty());

	ZoneMap::iterator iZone = zoneMap_.find(zoneName);
	ocean_assert(iZone != zoneMap_.cend());

	if (iZone != zoneMap_.cend())
	{
		ocean_assert(iZone->second.usageCounter_ >= 1u);
		--iZone->second.usageCounter_;

		if (iZone->second.usageCounter_ == 0u)
		{
			zoneMap_.erase(iZone);
		}
	}
}

void Manager::unloadUser(const uint64_t& userId)
{
	const ScopedLock scopedLock(managerLock_);

	for (size_t n = 0; n < createAvatarCallbacks_.size(); ++n)
	{
		if (createAvatarCallbacks_[n].first == userId)
		{
			createAvatarCallbacks_[n] = createAvatarCallbacks_.back();
			createAvatarCallbacks_.pop_back();
		}
	}

	ocean_assert(userId != 0ull);

	const AvatarMap::iterator iAvatar = avatarMap_.find(userId);
	ocean_assert(iAvatar != avatarMap_.cend());

	if (iAvatar != avatarMap_.cend())
	{
		AvatarUsagePair& avatarUsagePair = iAvatar->second;

		ocean_assert(avatarUsagePair.second >= 1u);
		if (--avatarUsagePair.second == 0u)
		{
			avatarMap_.erase(iAvatar);
		}
	}
}

void Manager::onResource(const ovrAvatar2Asset_Resource* resource)
{
	ocean_assert(resource != nullptr);

	switch (resource->status)
	{
		case ovrAvatar2AssetStatus_LoadFailed:
			Log::error() << "Avatars: Failed to load resource with id " << int(resource->resourceID);
			return;

		case ovrAvatar2AssetStatus_Loaded:
			Log::info() << "Avatars: Loading resource with id " << int(resource->resourceID);
			break;

		case ovrAvatar2AssetStatus_Unloaded:
			Log::info() << "Avatars: Unloading resource with id " << int(resource->resourceID);
			break;

		case ovrAvatar2AssetStatus_Updated:
			Log::warning() << "Avatars: Updating resource with id " << int(resource->resourceID) << " currently not supported";
			return;

		case ovrAvatar2AssetStatus_EnumSize:
			ocean_assert(false && "This should never happen!");
			return;
	}

	const ScopedLock scopedLock(resourceLock_);

	resourceEventQueue_.emplace(resource->resourceID, resource->status);
}

void Manager::onRequest(const ovrAvatar2RequestId requestId, const ovrAvatar2Result status)
{
	if (status == ovrAvatar2Result_Success)
	{
		bool result;
		if (ovrAvatar2_GetRequestBool(requestId, &result) == ovrAvatar2Result_Success)
		{
			Log::info() << "Avatars: Received status result for avatar request: " << (result ? "succeeded" : "failed");
		}
		else
		{
			Log::error() << "Avatars: Failed to determine request result";
		}
	}
	else
	{
		Log::error() << "Avatars: Avatar request failed";
	}
}

void Manager::onNewVertsEntity(Network::Verts::Driver& driver, const Network::Verts::SharedEntity& entity)
{
	const ScopedLock scopedLock(managerLock_);

	Network::Verts::SharedNode node = entity->node(vertsAvatarNodeType_);
	ocean_assert(node);

	if (node)
	{
		const ZoneMap::iterator iZone = zoneMap_.find(driver.zoneName());

		if (iZone != zoneMap_.cend())
		{
			iZone->second.remoteAvatarNodes_.emplace_back(std::move(node));
		}
		else
		{
			Log::error() << "Missing VERTS zone object";
			ocean_assert(false && "Missing zone!");
		}
	}
}

void Manager::onLogging(ovrAvatar2LogLevel priority, const char* message, void* userContext)
{
	ocean_assert(message != nullptr);

	if (priority >= ovrAvatar2LogLevel_Error)
	{
		Log::error() << "Avatars: " << message;
	}
	else if (priority >= ovrAvatar2LogLevel_Warn)
	{
		Log::warning() << "Avatars: " << message;
	}
	else if (priority >= ovrAvatar2LogLevel_Info)
	{
#ifdef OCEAN_DEBUG // currently only in debug mode as the Avatars SDK creates too much noise
		Log::info() << "Avatars: " << message;
#endif
	}
	else
	{
		Log::debug() << "Avatars: " << message;
	}
}

void Manager::onResource(const ovrAvatar2Asset_Resource* resource, void* userContext)
{
	ocean_assert(resource != nullptr);

	Manager* manager = reinterpret_cast<Manager*>(userContext);
	ocean_assert(manager != nullptr);

	manager->onResource(resource);
}

void Manager::onRequest(ovrAvatar2RequestId requestId, ovrAvatar2Result status, void* /*requestContext*/)
{
	Manager::get().onRequest(requestId, status);
}

void* Manager::onFileOpen(void* fileReaderContext, const char* filename)
{
	ocean_assert(filename != nullptr);

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	std::unique_ptr<Android::ResourceManager::ScopedResource> scopedResource(Android::ResourceManager::get().accessAsset(filename));

	if (!scopedResource)
	{
		scopedResource = Android::ResourceManager::get().accessAsset(std::string("ocean_meta_avatars/") + filename);
	}
#else
	std::unique_ptr<ScopedLocalFile> scopedResource(ScopedLocalFile::create(filename));
#endif

	if (!scopedResource)
	{
		Log::error() << "Avatars: Failed to load " << filename;
		return nullptr;
	}

	return scopedResource.release();
}

bool Manager::onFileRead(void* fileReaderContext, void* fileHandle, const void** fileData, uint64_t* fileSize)
{
	if (fileHandle != nullptr)
	{
#ifdef OCEAN_PLATFORM_BUILD_ANDROID
		Android::ResourceManager::ScopedResource* scopedResource = reinterpret_cast<Android::ResourceManager::ScopedResource*>(fileHandle);
#else
		ScopedLocalFile* scopedResource = reinterpret_cast<ScopedLocalFile*>(fileHandle);
#endif

		ocean_assert(scopedResource != nullptr);
		ocean_assert(fileData != nullptr && fileSize != nullptr);

		*fileData = scopedResource->data();
		*fileSize = scopedResource->size();

		return true;
	}

	return false;
}

bool Manager::onFileClose(void* fileReaderContext, void* fileHandle)
{
	if (fileHandle != nullptr)
	{
#ifdef OCEAN_PLATFORM_BUILD_ANDROID
		Android::ResourceManager::ScopedResource* scopedResource = reinterpret_cast<Android::ResourceManager::ScopedResource*>(fileHandle);
#else
		ScopedLocalFile* scopedResource = reinterpret_cast<ScopedLocalFile*>(fileHandle);
#endif

		ocean_assert(scopedResource != nullptr);
		delete scopedResource;
	}

	return true;
}

} // namespace Avatars

} // namespace Meta

} // namespace Platform

} // namespace Ocean
