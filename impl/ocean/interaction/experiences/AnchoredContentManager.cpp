/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/interaction/experiences/AnchoredContentManager.h"

namespace Ocean
{

namespace Interaction
{

namespace Experiences
{

AnchoredContentManager::ContentObject::ContentObject(const Rendering::NodeRef& renderingObjectNode, const Devices::Tracker6DOFRef& devicesTracker, const Devices::Tracker6DOF::ObjectId& devicesObjectId, const ContentId contentId, const Scalar visibilityRadius, const Scalar engagementRadius) :
	contentId_(contentId),
	renderingObjectNode_(renderingObjectNode),
	devicesTracker_(devicesTracker),
	devicesObjectId_(devicesObjectId),
	visibilityRadius_(visibilityRadius),
	engagementRadius_(engagementRadius)
{
	ocean_assert(renderingObjectNode_ && devicesTracker_);
	ocean_assert(devicesObjectId_ != Devices::Tracker6DOF::invalidObjectId());
	ocean_assert(contentId_ != invalidContentId());
	ocean_assert(visibilityRadius_ > Numeric::eps());
	ocean_assert(engagementRadius_ >= visibilityRadius_);
}

HomogenousMatrix4 AnchoredContentManager::ContentObject::world_T_object() const
{
	ocean_assert(isValid());

	if (renderingAnchorTransform_)
	{
		return renderingAnchorTransform_->transformation();
	}

	return HomogenousMatrix4(false);
}

void AnchoredContentManager::ContentObject::setRenderingObjectNode(Rendering::NodeRef renderingObjectNode)
{
	ocean_assert(renderingObjectNode);

	renderingAnchorTransform_->removeChild(renderingObjectNode_);

	renderingObjectNode_ = std::move(renderingObjectNode);

	renderingAnchorTransform_->addChild(renderingObjectNode_);
}

AnchoredContentManager::AnchoredContentManager()
{
	// nothing to do here
}

AnchoredContentManager::~AnchoredContentManager()
{
	release();
}

bool AnchoredContentManager::initialize(RemovedContentCallbackFunction removedContentCallbackFunction, const Rendering::SceneRef& scene)
{
	ocean_assert(removedContentCallbackFunction && scene);

	if (removedContentCallbackFunction_ || renderingScene_ || !removedContentCallbackFunction || !scene)
	{
		return false;
	}

	removedContentCallbackFunction_ = std::move(removedContentCallbackFunction);
	renderingScene_ = scene;

	return true;
}

void AnchoredContentManager::release()
{
	const ScopedLock scopedLock(lock_);

	removedContentCallbackFunction_ = RemovedContentCallbackFunction();
	renderingScene_.release();

	subscriptionMap_.clear();

	contentMap_.clear();
	objectIdToContentObjectMultiMap_.clear();
	trackerToContentObjectMap_.clear();
}

AnchoredContentManager::ContentId AnchoredContentManager::addContent(const Rendering::NodeRef& renderingObjectNode, const Devices::Tracker6DOFRef& devicesTracker, const Devices::Tracker6DOF::ObjectId& devicesObjectId, const Scalar visibilityRadius, const Scalar engagementRadius)
{
	ocean_assert(renderingScene_);
	ocean_assert(renderingObjectNode && devicesTracker);
	ocean_assert(devicesObjectId != Devices::Tracker6DOF::invalidObjectId());
	ocean_assert(visibilityRadius > 0 && engagementRadius >= visibilityRadius);

	if (!renderingScene_ || !renderingObjectNode || !devicesTracker)
	{
		return invalidContentId();
	}

	if (renderingScene_->type() == Rendering::Object::TYPE_SCENE && renderingScene_.pointer() == Rendering::SceneRef(renderingObjectNode).pointer())
	{
		Log::error() << "The provided rendering object node is already defined as the anchored content manager's scene";
		ocean_assert(false && "The provided rendering object node is already defined as the anchored content manager's scene");

		return invalidContentId();
	}

	const ScopedLock scopedLock(lock_);

	// we create a unique content id

	const ContentId newContentId = ++contentIdCounter_;

	SharedContentObject contentObject(new ContentObject(renderingObjectNode, devicesTracker, devicesObjectId, newContentId, visibilityRadius, engagementRadius));

	// we handle event subscription

	SubscriptionMap::iterator iSubscription = subscriptionMap_.find(&*devicesTracker);

	if (iSubscription == subscriptionMap_.cend())
	{
		Devices::Tracker::TrackerObjectEventSubscription eventSubscription = devicesTracker->subscribeTrackerObjectEvent(Devices::Tracker::TrackerObjectCallback::create(*this, &AnchoredContentManager::onTrackerObjects));

		iSubscription = subscriptionMap_.emplace(&*devicesTracker, SubscriptionPair(std::move(eventSubscription), 0u)).first;
	}

	ocean_assert(iSubscription != subscriptionMap_.cend());
	++iSubscription->second.second; // increase reference counter

	contentObject->isTracked_ = devicesTracker->isObjectTracked(contentObject->devicesObjectId_); // we need to know whether the object is currently tracked or not (e.g., not visible)

	ocean_assert(contentMap_.find(newContentId) == contentMap_.cend());
	contentMap_.emplace(newContentId, contentObject);

	objectIdToContentObjectMultiMap_.emplace(devicesObjectId, std::move(contentObject));

	return newContentId;
}

bool AnchoredContentManager::removeAllContent()
{
	ocean_assert(renderingScene_);

	while (contentMap_.size() > 0)
	{
		ContentMap::const_iterator iContent = contentMap_.cbegin();
		ContentId content_id = iContent->second->contentId_;
		removeContent(content_id);
	}

	return true;
}

bool AnchoredContentManager::removeContent(const ContentId contentId)
{
	ocean_assert(renderingScene_);
	ocean_assert(contentId != invalidContentId());

	const ScopedLock scopedLock(lock_);

	ContentMap::iterator iContent = contentMap_.find(contentId);

	if (iContent == contentMap_.cend())
	{
		ocean_assert(false && "The content does not exist!");
		return false;
	}

	Devices::Tracker6DOF& tracker = *iContent->second->devicesTracker_;

	SubscriptionMap::iterator iSubscription = subscriptionMap_.find(&tracker);

	ocean_assert(iSubscription != subscriptionMap_.cend());
	SubscriptionPair& subscriptionPair = iSubscription->second;

	ocean_assert(subscriptionPair.second >= 1u);
	--subscriptionPair.second;// decrease reference counter

	if (subscriptionPair.second == 0u)
	{
		subscriptionMap_.erase(iSubscription);
	}

	const std::pair<ObjectIdToContentObjectMultiMap::iterator, ObjectIdToContentObjectMultiMap::iterator> objectRange = objectIdToContentObjectMultiMap_.equal_range(iContent->second->devicesObjectId_);
	ocean_assert(objectRange.first != objectRange.second);

	for (ObjectIdToContentObjectMultiMap::iterator iObject = objectRange.first; iObject != objectRange.second; ++iObject)
	{
		if (iObject->second->contentId() == contentId)
		{
			objectIdToContentObjectMultiMap_.erase(iObject);
			break;
		}
	}

	if (iContent->second->renderingAnchorTransform_)
	{
		renderingScene_->removeChild(iContent->second->renderingAnchorTransform_);
		iContent->second->renderingAnchorTransform_.release();
	}

	contentMap_.erase(iContent);

	return true;
}

AnchoredContentManager::SharedContentObject AnchoredContentManager::content(const ContentId contentId) const
{
	ocean_assert(contentId != invalidContentId());

	const ScopedLock scopedLock(lock_);

	const ContentMap::const_iterator iContent = contentMap_.find(contentId);

	if (iContent == contentMap_.cend())
	{
		return nullptr;
	}

	return iContent->second;
}

AnchoredContentManager::SharedContentObjectSet AnchoredContentManager::visibleContents() const
{
	const ScopedLock scopedLock(lock_);

	SharedContentObjectSet result;

	for (ContentMap::const_iterator iContent = contentMap_.cbegin(); iContent != contentMap_.cend(); ++iContent)
	{
		if (iContent->second->renderingObjectNode_->visible())
		{
			result.emplace(iContent->second);
		}
	}

	return result;
}

AnchoredContentManager::SharedContentObjectSet AnchoredContentManager::closeContents(const Scalar maxSqrDistance) const
{
	ocean_assert(maxSqrDistance >= 0);

	SharedContentObjectSet result;

	for (ContentMap::const_iterator iContent = contentMap_.cbegin(); iContent != contentMap_.cend(); ++iContent)
	{
		if (iContent->second->sqrDistance_ >= 0 && iContent->second->sqrDistance_ <= maxSqrDistance)
		{
			result.emplace(iContent->second);
		}
	}

	return result;
}

Timestamp AnchoredContentManager::preUpdate(const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(renderingScene_);
	ocean_assert(engine && view);

	const ScopedLock scopedLock(lock_);

	const HomogenousMatrix4 world_T_view = view->transformation();

	HomogenousMatrix4 view_T_device(true); // for visual tracker

	if (view->background())
	{
		// the tracking result is based on a visual tracker, and the tracking result needs to be displayed wrt the background
		const Quaternion view_Q_background(view->background()->orientation());

		view_T_device = HomogenousMatrix4(view_Q_background); // if the background is rotated wrt view in the same way as the device
	}

	SharedContentObjectSet removedContentObjects; // all content objects outside of the engagement radius

	for (SubscriptionMap::const_iterator iSubscription = subscriptionMap_.cbegin(); iSubscription != subscriptionMap_.cend(); ++iSubscription)
	{
		const Devices::Tracker6DOF* tracker = iSubscription->first;

		const Devices::Tracker6DOF::Tracker6DOFSampleRef trackerSample(tracker->sample(timestamp));

		if (trackerSample)
		{
			for (size_t n = 0; n < trackerSample->objectIds().size(); ++n)
			{
				const Devices::Tracker::ObjectId objectId = trackerSample->objectIds()[n];

				const std::pair<ObjectIdToContentObjectMultiMap::const_iterator, ObjectIdToContentObjectMultiMap::const_iterator> range = objectIdToContentObjectMultiMap_.equal_range(objectId);

				for (ObjectIdToContentObjectMultiMap::const_iterator iContent = range.first; iContent != range.second; ++iContent)
				{
					ocean_assert(iContent->second);
					ContentObject& contentObject = *iContent->second;

					bool isVisible = false;

					if (contentObject.isTracked_)
					{
						const Quaternion& orientation = trackerSample->orientations()[n];
						const Vector3& position = trackerSample->positions()[n];

						const HomogenousMatrix4 sampleTransformation(position, orientation);

						ocean_assert(trackerSample->referenceSystem() == Devices::Tracker::RS_DEVICE_IN_OBJECT || trackerSample->referenceSystem() == Devices::Tracker::RS_OBJECT_IN_DEVICE);

						HomogenousMatrix4 device_T_object;

						if (trackerSample->referenceSystem() == Devices::Tracker::RS_OBJECT_IN_DEVICE)
						{
							device_T_object = sampleTransformation;
						}
						else
						{
							device_T_object = sampleTransformation.inverted();
						}

						if (contentObject.renderingAnchorTransform_.isNull())
						{
							// the anchor transform has not been created yet (as this is the first time the object is tracked), so we create the anchor transform (between scene and object node)

							contentObject.renderingAnchorTransform_ = engine->factory().createTransform();
							contentObject.renderingAnchorTransform_->addChild(contentObject.renderingObjectNode_);

							renderingScene_->addChild(contentObject.renderingAnchorTransform_);
						}

						Scalar sqrDistance = Numeric::minValue();

						ocean_assert(contentObject.devicesTracker_);
						if (contentObject.devicesTracker_->type().minorType() & Devices::Tracker::TRACKER_VISUAL)
						{
							const HomogenousMatrix4 view_T_object(view_T_device * device_T_object);
							const HomogenousMatrix4 world_T_object = world_T_view * view_T_object;

							sqrDistance = world_T_view.translation().sqrDistance(world_T_object.translation());

							contentObject.renderingAnchorTransform_->setTransformation(world_T_object);
						}
						else
						{
							sqrDistance = world_T_view.translation().sqrDistance(device_T_object.translation());

							contentObject.renderingAnchorTransform_->setTransformation(device_T_object);
						}

						if (sqrDistance <= Numeric::sqr(contentObject.visibilityRadius_))
						{
							isVisible = true;
						}
						else if (sqrDistance > Numeric::sqr(contentObject.engagementRadius_))
						{
							removedContentObjects.emplace(std::move(iContent->second));
						}

						contentObject.sqrDistance_ = sqrDistance;
					}

					if (contentObject.renderingAnchorTransform_)
					{
						ocean_assert(contentObject.renderingAnchorTransform_);
						contentObject.renderingAnchorTransform_->setVisible(isVisible);
					}
				}
			}
		}
		else
		{
			// no sample for the given timestamp, all associated content objects are invisible

			for (TrackerToContentObjectMap::const_iterator iContent = trackerToContentObjectMap_.cbegin(); iContent != trackerToContentObjectMap_.cend(); ++iContent)
			{
				ocean_assert(iContent->second);
				ContentObject& contentObject = *iContent->second;

				ocean_assert(contentObject.renderingAnchorTransform_);
				contentObject.renderingAnchorTransform_->setVisible(false);
			}
		}
	}

	for (const SharedContentObject& contentObject : removedContentObjects)
	{
		removeContent(contentObject->contentId());
	}

	if (!removedContentObjects.empty() && removedContentCallbackFunction_)
	{
		removedContentCallbackFunction_(std::move(removedContentObjects));
	}

	return timestamp;
}

void AnchoredContentManager::onTrackerObjects(const Devices::Tracker* tracker, const bool found, const Devices::Measurement::ObjectIdSet& objectIds, const Timestamp& /*timestamp*/)
{
	ocean_assert_and_suppress_unused(tracker != nullptr, tracker);
	ocean_assert(!objectIds.empty());

	const ScopedLock scopedLock(lock_);

	for (const Devices::Measurement::ObjectId& objectId : objectIds)
	{
		const std::pair<ObjectIdToContentObjectMultiMap::const_iterator, ObjectIdToContentObjectMultiMap::const_iterator> range = objectIdToContentObjectMultiMap_.equal_range(objectId);

		for (ObjectIdToContentObjectMultiMap::const_iterator iContent = range.first; iContent != range.second; ++iContent)
		{
			ocean_assert(iContent->second);

			ocean_assert(iContent->second->devicesTracker_);
			ocean_assert(&*iContent->second->devicesTracker_ == tracker);

			iContent->second->isTracked_ = found;
		}
	}
}

} // namespace Experiences

} // namespace Interaction

} // namespace Ocean
