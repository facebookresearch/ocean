// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/UserProfileExperience.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/io/JSONConfig.h"

#include "ocean/io/image/Image.h"

#include "ocean/media/Manager.h"
#include "ocean/media/PixelImage.h"

#include "ocean/rendering/Scene.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

UserProfileExperience::~UserProfileExperience()
{
	// nothing to do here
}

bool UserProfileExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	assert(engine);

	// first, we invoke the WhoAmI query

	const std::string query = "4670246276321700"; // "query WhoAmI { viewer { actor { id, name, profile_picture { uri } } } }"
	constexpr bool queryIsPersistId = true;
	const std::string parameters = "{}";

	futureGraphQLResponse_ = Network::Tigon::TigonClient::get().graphQLRequest(query, queryIsPersistId, parameters);

	if (!futureGraphQLResponse_.valid())
	{
		Log::error() << "Failed to invoke the WhoAmI query";
		return false;
	}

#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	// now we can access a SLAM world tracker

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	slamTracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF World Tracker");
#else
	slamTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF World Tracker");
#endif

	if (slamTracker6DOF_.isNull())
	{
		Log::error() << "Failed to access World tracker";
		return false;
	}

	const Devices::VisualTrackerRef visualTracker(slamTracker6DOF_);
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
			}
		}
	}

	slamTracker6DOF_->start();

	if (!anchoredContentManager_.initialize(std::bind(&UserProfileExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	return true;
}

bool UserProfileExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	ocean_assert(engine);

	anchoredContentManager_.release();

	slamTracker6DOF_.release();

	return true;
}

Timestamp UserProfileExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	if (futureGraphQLResponse_.valid() &&  futureGraphQLResponse_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		Network::Tigon::TigonRequest::TigonResponse response = futureGraphQLResponse_.get();

		if (response.succeeded() && !response.response().empty())
		{
			IO::JSONConfig config(std::shared_ptr<std::istream>(new std::istringstream(response.response())));

			if (config.exist("data"))
			{
				std::string name = config["data"]["viewer"]["actor"]["name"](std::string());
				std::string uri = config["data"]["viewer"]["actor"]["profile_picture"]["uri"](std::string());

				if (!name.empty())
				{
#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

					// we have a valid name, let's create a rendering 3D text and anchor it in the world of the SLAM tracker

					const Devices::Tracker6DOF::ObjectId trackerObjectId = slamTracker6DOF_->objectId("World");

					if (trackerObjectId != Devices::Tracker6DOF::invalidObjectId())
					{
						const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, name, RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.5f), true, Scalar(0), Scalar(0.1), Scalar(0), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE);
						textTransform->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.1), Scalar(-0.5))));

						constexpr Scalar visibilityRadius = Scalar(2.5);
						constexpr Scalar engagementRadius = Scalar(1000);
						anchoredContentManager_.addContent(textTransform, slamTracker6DOF_, trackerObjectId, visibilityRadius, engagementRadius);
					}

#else

					const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, name, RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.5f), true, Scalar(0), Scalar(0.1), Scalar(0), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE);
					textTransform->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.1), Scalar(-0.5))));
					experienceScene()->addChild(textTransform);

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

				}
				else
				{
					Log::info() << "No username available";
				}

				if (!uri.empty())
				{
					// removing backlash from URI
					uri = String::replace(std::move(uri), "\\", "", false);

					futureHTTPResponse_ = Network::Tigon::TigonClient::get().httpRequest(uri);
				}
				else
				{
					Log::info() << "Warning no profile picture available";
				}
			}
		}
		else
		{
			Log::error() << "GraphQL query failed";
		}

		futureGraphQLResponse_ = std::future<Network::Tigon::TigonRequest::TigonResponse>();
		ocean_assert(!futureGraphQLResponse_.valid());
	}
	else if (futureHTTPResponse_.valid() &&  futureHTTPResponse_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		Network::Tigon::TigonRequest::TigonResponse response = futureHTTPResponse_.get();

		if (response.succeeded() && !response.response().empty())
		{
			const std::string& content = response.response();

			Frame frame = IO::Image::decodeImage(content.c_str(), content.size());

			if (frame.isValid())
			{
				frame.setTimestamp(timestamp);

#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

				// we have a valid frame, let's create a rendering 3D box and anchor it in the world of the SLAM tracker

				const Devices::Tracker6DOF::ObjectId trackerObjectId = slamTracker6DOF_->objectId("World");

				if (trackerObjectId != Devices::Tracker6DOF::invalidObjectId())
				{
					const Scalar aspectRatio = Scalar(frame.width()) / Scalar(frame.height());
					const Vector3 size = Vector3(Scalar(0.25) * aspectRatio, Scalar(0.255), 0);

					const Media::PixelImageRef pixelImage = Media::Manager::get().newMedium("USER_PROFILE_EXPERIENCE_IMAGE", Media::Medium::PIXEL_IMAGE, true);

					pixelImage->start();
					pixelImage->setPixelImage(std::move(frame));

					const Rendering::TransformRef boxTransform = Rendering::Utilities::createBox(engine, size, pixelImage);
					boxTransform->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(-0.1), Scalar(-0.5))));

					constexpr Scalar visibilityRadius = Scalar(2.5);
					constexpr Scalar engagementRadius = Scalar(1000);
					anchoredContentManager_.addContent(boxTransform, slamTracker6DOF_, trackerObjectId, visibilityRadius, engagementRadius);
				}

#else

				const Scalar aspectRatio = Scalar(frame.width()) / Scalar(frame.height());
				const Vector3 size = Vector3(Scalar(0.25) * aspectRatio, Scalar(0.255), 0);

				const Media::PixelImageRef pixelImage = Media::Manager::get().newMedium("USER_PROFILE_EXPERIENCE_IMAGE", Media::Medium::PIXEL_IMAGE, true);

				pixelImage->start();
				pixelImage->setPixelImage(std::move(frame));

				const Rendering::TransformRef boxTransform = Rendering::Utilities::createBox(engine, size, pixelImage);
				boxTransform->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(-0.1), Scalar(-0.5))));
				experienceScene()->addChild(boxTransform);

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

			}
		}
		else
		{
			Log::error() << "HTTP request failed";
		}

		futureHTTPResponse_ = std::future<Network::Tigon::TigonRequest::TigonResponse>();
		ocean_assert(!futureHTTPResponse_.valid());
	}

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	const Timestamp updatedTimestamp = anchoredContentManager_.preUpdate(engine, view, timestamp);

	return updatedTimestamp;
}

std::unique_ptr<XRPlaygroundExperience> UserProfileExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new UserProfileExperience());
}

void UserProfileExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects)
{
	ocean_assert(false && "Objects should never be removed by the anchor manager, as we have a very large engagement radius");
}

}

}
