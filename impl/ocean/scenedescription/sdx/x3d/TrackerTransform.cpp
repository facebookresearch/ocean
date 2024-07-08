/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/TrackerTransform.h"

#include "ocean/base/String.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/Tracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/io/FileResolver.h"

#include "ocean/media/Manager.h"
#include "ocean/media/Medium.h"

#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/Transform.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/View.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

TrackerTransform::TrackerTransform(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DBoundedObject(environment),
	X3DGroupingNode(environment),
	SDXUpdateNode(environment),
	insideOut_(false),
	multiInput_(false),
	objectSize_(Vector3(0, 0, 0)),
	alwaysVisible_(false),
	objectId_(Devices::Tracker::invalidObjectId()),
	objectVisible_(false)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createTransform();
}

TrackerTransform::~TrackerTransform()
{
	// nothing to do here
}

TrackerTransform::NodeSpecification TrackerTransform::specifyNode()
{
	NodeSpecification specification("TrackerTransform");

	registerField(specification, "insideOut", insideOut_, ACCESS_NONE);
	registerField(specification, "tracker", tracker_, ACCESS_NONE);
	registerField(specification, "trackerInput", trackerInput_, ACCESS_NONE);
	registerField(specification, "object", object_, ACCESS_NONE);
	registerField(specification, "objectSize", objectSize_, ACCESS_NONE);
	registerField(specification, "alwaysVisible", alwaysVisible_, ACCESS_NONE);

	X3DGroupingNode::registerFields(specification);

	return specification;
}

void TrackerTransform::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DGroupingNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::TransformRef renderingTransform(renderingObject_);

		if (renderingTransform)
		{
			renderingTransform->setVisible(alwaysVisible_.value());

			for (MultiString::Values::const_iterator i = tracker_.values().begin(); i != tracker_.values().end() && trackerRef_.isNull(); ++i)
			{
				if (String::toUpper(*i) == "6DOF_TRACKER")
				{
					trackerRef_ = Devices::Manager::get().device(Devices::Device::DeviceType(Devices::Device::DEVICE_TRACKER, Devices::Tracker::TRACKER_6DOF));
				}
				else if (String::toUpper(*i) == "3DOF_POSITION_TRACKER")
				{
					trackerRef_ = Devices::Manager::get().device(Devices::Device::DeviceType(Devices::Device::DEVICE_TRACKER, Devices::Tracker::TRACKER_POSITION_3DOF));
				}
				else if (String::toUpper(*i) == "3DOF_ORIENTATION_TRACKER")
				{
					trackerRef_ = Devices::Manager::get().device(Devices::Device::DeviceType(Devices::Device::DEVICE_TRACKER, Devices::Tracker::TRACKER_ORIENTATION_3DOF));
				}
				else
				{
					trackerRef_ = Devices::Manager::get().device(*i);
				}
			}

			if (!tracker_.values().empty() && trackerRef_.isNull())
			{
				Log::warning() << "Could not access the following tracker \"" << tracker_.values().front() << "\".";
			}

			if (trackerRef_)
			{
				const Devices::VisualTrackerRef visualTracker(trackerRef_);

				if (visualTracker)
				{
					if (!trackerInput_.values().empty())
					{
						Media::FrameMediumRefs frameMediums;

						for (const std::string& input : trackerInput_.values())
						{
							Media::FrameMediumRef frameMedium;

							if (String::toUpper(input) == "BACKGROUND")
							{
								if (!engine()->framebuffers().empty())
								{
									const Rendering::ViewRef& view = engine()->framebuffers().front()->view();

									if (view && view->background() && view->background()->type() == Rendering::Object::TYPE_UNDISTORTED_BACKGROUND)
									{
										const Rendering::UndistortedBackgroundRef background(view->background());

										frameMedium = background->medium();
									}
								}
							}
							else
							{
								const IO::Files resolvedFiles(IO::FileResolver::get().resolve(IO::File(input), IO::File(filename()), true /*checkExistence*/));

								for (const IO::File& resolvedFile : resolvedFiles)
								{
									frameMedium = Media::Manager::get().newMedium(resolvedFile(), Media::Medium::FRAME_MEDIUM);

									if (frameMedium)
									{
										break;
									}
								}

								if (!frameMedium && resolvedFiles.empty())
								{
									frameMedium = Media::Manager::get().newMedium(input, Media::Medium::LIVE_VIDEO);
								}
							}

							if (!frameMedium && multiInput_.value())
							{
								// for multi-input, each provided input must be valid

								frameMediums.clear();
								break;
							}

							if (frameMedium)
							{
								frameMediums.emplace_back(std::move(frameMedium));
							}

							if (!frameMediums.empty() && !multiInput_.value())
							{
								// we stop when we have the first valid medium in case we have a tracker with single-input
								break;
							}
						}

						if (!frameMediums.empty())
						{
							for (Media::FrameMediumRef& frameMedium : frameMediums)
							{
								if (!frameMedium->start())
								{
									Log::warning() << "Failed to start frame medium '" << frameMedium->url() << "'";
								}
							}

							visualTracker->setInput(std::move(frameMediums));
						}
						else
						{
							Log::warning() << "Could not access the following tracker input \"" << trackerInput_.values().front() << "\".";
						}
					}
				}

				const Devices::ObjectTrackerRef objectTracker(trackerRef_);

				if (objectTracker)
				{
					if (!object_.value().empty())
					{
						objectId_ = objectTracker->registerObject(object_.value(), objectSize_.value());

						if (objectId_ == Devices::Tracker::invalidObjectId())
						{
							const IO::Files resolvedFiles(IO::FileResolver::get().resolve(IO::File(object_.value()), IO::File(filename())));

							for (const IO::File& resolvedFile : resolvedFiles)
							{
								if (resolvedFile.exists())
								{
									objectId_ = objectTracker->registerObject(resolvedFile(), objectSize_.value());

									if (objectId_ != Devices::Tracker::invalidObjectId())
									{
										break;
									}
								}
							}

							if (objectId_ == Devices::Tracker::invalidObjectId())
							{
								Log::warning() << "Could not load the following tracker object '" << object_.value() << "'";
							}
						}
					}
				}

				if (objectId_ == Devices::Tracker::invalidObjectId())
				{
					if (object_.value().empty())
					{
						const Strings objectDescriptions = trackerRef_->objectDescriptions();

						if (objectDescriptions.size() == 1)
						{
							objectId_ = trackerRef_->objectId(objectDescriptions.front());
						}
						else
						{
							Log::warning() << "The tracker '" << trackerRef_->name() << "' has more than on object";
						}
					}
					else
					{
						objectId_ = trackerRef_->objectId(object_.value());

						if (objectId_ == Devices::Tracker6DOF::invalidObjectId())
						{
							Log::warning() << "The tracker '" << trackerRef_->name() << "' does not have an object '" << object_.value() << "'";
						}
					}
				}

				if (objectId_ != Devices::Tracker::invalidObjectId() && trackerRef_->isObjectTracked(objectId_))
				{
					objectVisible_ = true;
				}

				trackerObjectEventSubscription_ = trackerRef_->subscribeTrackerObjectEvent(Devices::Tracker::TrackerObjectCallback::create(*this, &TrackerTransform::onTrackerObjectEvent));

				trackerRef_->start();
			}
		}
	}
	catch (const std::exception& exception)
	{
		Log::warning() << exception.what();
	}
}

void TrackerTransform::onFieldChanged(const std::string& fieldName)
{
	X3DGroupingNode::onFieldChanged(fieldName);
}

Timestamp TrackerTransform::onPreUpdate(const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(view);
	ocean_assert(timestamp.isValid());

	const ScopedLock scopedLock(lock_);

	if (trackerRef_.isNull())
	{
		return timestamp;
	}

	const Rendering::TransformRef renderingTransform(renderingObject_);

	if (renderingTransform.isNull())
	{
		return timestamp;
	}

	if (!objectVisible_)
	{
		if (!alwaysVisible_.value())
		{
			renderingTransform->setVisible(false);
		}

		return timestamp;
	}

	/// Retrieving the sample with identical timestmap or the most recent one
	const Devices::Tracker::TrackerSampleRef sample = trackerRef_->sample(timestamp);

	if (sample.isNull())
	{
		if (!alwaysVisible_.value())
		{
			renderingTransform->setVisible(false);
		}

		return timestamp;
	}

	const Devices::VisualTrackerRef visualTracker(trackerRef_);
	if (visualTracker)
	{
		// Only for visual trackers:
		// Tests whether the frame store of tracker input medium holds still the frame corresponding to the sample

		const Media::FrameMediumRefs trackerMediums(visualTracker->input());

		if (trackerMediums.size() == 1 && !trackerMediums.front()->hasFrame(sample->timestamp()))
		{
			if (!alwaysVisible_.value())
			{
				renderingTransform->setVisible(false);
			}

#ifdef OCEAN_DEBUG
			Log::warning() << "No video background image for the corresponding tracking sample.";
#endif
			return timestamp;
		}
	}

	unsigned int objectIndex = (unsigned int)(-1);

	for (unsigned int n = 0u; n < sample->objectIds().size(); ++n)
	{
		if (sample->objectIds()[n] == objectId_)
		{
			objectIndex = n;
			break;
		}
	}

	// if the sample is not containing the object
	if (objectIndex == (unsigned int)(-1))
	{
		if (!alwaysVisible_.value())
		{
			renderingTransform->setVisible(false);
		}

		return timestamp;
	}

	HomogenousMatrix4 sampleTransformation(true);

	const Devices::Tracker6DOF::Tracker6DOFSampleRef sample6DOF(sample);
	if (sample6DOF)
	{
		ocean_assert(objectIndex < sample6DOF->positions().size());
		ocean_assert(objectIndex < sample6DOF->orientations().size());

		sampleTransformation = HomogenousMatrix4(sample6DOF->positions()[objectIndex], sample6DOF->orientations()[objectIndex]);
	}
	else
	{
		const Devices::PositionTracker3DOF::PositionTracker3DOFSampleRef samplePosition3DOF(sample);

		if (samplePosition3DOF)
		{
			ocean_assert(objectIndex < samplePosition3DOF->positions().size());

			sampleTransformation = HomogenousMatrix4(samplePosition3DOF->positions()[objectIndex]);
		}
		else
		{
			const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef sampleOrientation3DOF(sample);

			if (sampleOrientation3DOF)
			{
				ocean_assert(objectIndex < sampleOrientation3DOF->orientations().size());

				sampleTransformation = HomogenousMatrix4(sampleOrientation3DOF->orientations()[objectIndex]);
			}
			else
			{
				ocean_assert(false && "This should never happen!");
			}
		}
	}

	ocean_assert(sampleTransformation.isValid());

	ocean_assert(sample->referenceSystem() == Devices::Tracker::RS_DEVICE_IN_OBJECT || sample->referenceSystem() == Devices::Tracker::RS_OBJECT_IN_DEVICE);
	const HomogenousMatrix4 device_T_object = sample->referenceSystem() == Devices::Tracker::RS_OBJECT_IN_DEVICE ? sampleTransformation : sampleTransformation.inverted();

	HomogenousMatrix4 view_T_device(true);

	if (view->background() && visualTracker)
	{
		// the tracking result is based on a visual tracker, and the tracking result needs to be displayed wrt the background
		const Quaternion view_Q_background(view->background()->orientation());

		view_T_device = HomogenousMatrix4(view_Q_background); // if the background is rotated wrt view in the same way as the device
	}

	/**
	 * Inside-out tracking determines the position of the device in object coordinate system.
	 *
	 * insideOut == TRUE   <==> DEVICE_IN_OBJECT
	 * insideOut == FALSE  <==> OBJECT_IN_DEVICE
	 */
	if (insideOut_.value())
	{
		renderingTransform->setTransformation(view_T_device * device_T_object);
	}
	else // outside-in
	{
		const HomogenousMatrix4 world_T_view = view->transformation();

		const HomogenousMatrix4 world_T_object = world_T_view * view_T_device * device_T_object;

		renderingTransform->setTransformation(world_T_object);
	}

	renderingTransform->setVisible(true);

	return sample->timestamp();
}

void TrackerTransform::onTrackerObjectEvent(const Devices::Tracker* trackerSender, const bool objectFound, const Devices::Tracker::ObjectIdSet& objectIds, const Timestamp& /*timestamp*/)
{
	ocean_assert_and_suppress_unused(trackerSender != nullptr, trackerSender);

	if (objectIds.find(objectId_) == objectIds.cend())
	{
		return;
	}

	const ScopedLock scopedLock(lock_);

	ocean_assert(objectVisible_ != objectFound);
	objectVisible_ = objectFound;
}

size_t TrackerTransform::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
