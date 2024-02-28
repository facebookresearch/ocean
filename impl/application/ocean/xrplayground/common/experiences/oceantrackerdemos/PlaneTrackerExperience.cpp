// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/PlaneTrackerExperience.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

PlaneTrackerExperience::~PlaneTrackerExperience()
{
	// nothing to do here
}

bool PlaneTrackerExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
	planeTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Plane Tracker");
#else
	planeTracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF Plane Tracker");
#endif

	if (!planeTracker6DOF_)
	{
		Log::error() << "Plane Tracker Experience could not access the plane tracker";
		return false;
	}

	const Devices::VisualTrackerRef visualTracker(planeTracker6DOF_);
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

	if (!planeTracker6DOF_->start())
	{
		Log::error() << "Plane Tracker Experience could not start the scene tracker";
		return false;
	}

	sceneTrackerSampleEventSubscription_ = planeTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &PlaneTrackerExperience::onSceneTrackerSample));

	if (!anchoredContentManager_.initialize(std::bind(&PlaneTrackerExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	return true;
}

bool PlaneTrackerExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	sceneTrackerSampleEventSubscription_.release();

	renderingGroup_.release();

	anchoredContentManager_.release();

	planeTracker6DOF_.release();

	return true;
}

Timestamp PlaneTrackerExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedLock(lock_);
		const Devices::SceneTracker6DOF::SharedSceneElement recentSceneElement(recentSceneElement_);
		const Devices::SceneTracker6DOF::ObjectId objectId(objectId_);
	scopedLock.release();

	if (recentSceneElement && objectId != Devices::SceneTracker6DOF::invalidObjectId())
	{
		const Devices::SceneTracker6DOF::SceneElementPlanes& sceneElementPlanes = dynamic_cast<const Devices::SceneTracker6DOF::SceneElementPlanes&>(*recentSceneElement);
		ocean_assert(!sceneElementPlanes.planes().empty());

		if (!renderingGroup_)
		{
			renderingGroup_ = engine->factory().createGroup();

			constexpr Scalar visibilityRadius = Scalar(1000); // 1km
			constexpr Scalar engagementRadius = Scalar(10000);
			anchoredContentManager_.addContent(renderingGroup_, planeTracker6DOF_, objectId, visibilityRadius, engagementRadius);
		}

		renderingGroup_->clear();

		for (const Devices::SceneTracker6DOF::SceneElementPlanes::Plane& plane : sceneElementPlanes.planes())
		{
			Vectors3 normals;
			Rendering::TriangleFaces triangleFaces;

			if (!plane.vertices().empty() && !plane.triangleIndices().empty() && plane.triangleIndices().size() % 3 == 0)
			{
				const Vectors3& vertices = plane.vertices();

				Vector3 planeNormal(0, 0, 0);

				triangleFaces.clear();
				normals.clear();
				for (size_t n = 0; n < plane.triangleIndices().size() / 3; ++n)
				{
					const Index32& index0 = plane.triangleIndices()[n * 3 + 0];
					const Index32& index1 = plane.triangleIndices()[n * 3 + 1];
					const Index32& index2 = plane.triangleIndices()[n * 3 + 2];

					ocean_assert(index0 < vertices.size());
					ocean_assert(index1 < vertices.size());
					ocean_assert(index2 < vertices.size());

					triangleFaces.emplace_back(index0, index1, index2);

					const Vector3 normal = (vertices[index1] - vertices[index0]).cross((vertices[index2] - vertices[index0])).normalizedOrZero();

					normals.emplace_back(normal);
					normals.emplace_back(normal);
					normals.emplace_back(normal);

					planeNormal += normal;
				}

				planeNormal.normalize(); // we use the normal as the plane's color
				planeNormal = planeNormal * Scalar(0.5) + Vector3(Scalar(0.5), Scalar(0.5), Scalar(0.5));

				const Rendering::TransformRef planeTransform = Rendering::Utilities::createMesh(engine, vertices, triangleFaces, RGBAColor(float(planeNormal.x()), float(planeNormal.y()), float(planeNormal.z()), 0.95f), normals);
				planeTransform->setTransformation(plane.world_T_plane());
				renderingGroup_->addChild(planeTransform);

				const Rendering::TransformRef verticesTransform = Rendering::Utilities::createPoints(*engine, vertices, RGBAColor(1.0f, 0.0f, 0.0f), Scalar(20));
				verticesTransform->setTransformation(plane.world_T_plane());
				renderingGroup_->addChild(verticesTransform);

				if (!plane.boundaryVertices().empty())
				{
					const Rendering::TransformRef boundaryVerticesTransform = Rendering::Utilities::createPoints(*engine, plane.boundaryVertices(), RGBAColor(0.0f, 1.0f, 0.0f), Scalar(10));
					boundaryVerticesTransform->setTransformation(plane.world_T_plane());

					renderingGroup_->addChild(boundaryVerticesTransform);
				}
			}
		}
	}

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	return anchoredContentManager_.preUpdate(engine, view, timestamp);
}

std::unique_ptr<XRPlaygroundExperience> PlaneTrackerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new PlaneTrackerExperience());
}

void PlaneTrackerExperience::onSceneTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample)
{
	const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sceneTrackerSample(sample);

	if (!sceneTrackerSample || sceneTrackerSample->sceneElements().empty())
	{
		return;
	}

	const Devices::SceneTracker6DOF::ObjectId objectId = sceneTrackerSample->objectIds().front();

	const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement = sceneTrackerSample->sceneElements().front();

	if (sceneElement && sceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_PLANES)
	{
		const Devices::SceneTracker6DOF::SceneElementPlanes& sceneElementPlanes = dynamic_cast<const Devices::SceneTracker6DOF::SceneElementPlanes&>(*sceneElement);

		if (!sceneElementPlanes.planes().empty())
		{
			const ScopedLock scopedLock(lock_);

			recentSceneElement_ = sceneElement;
			objectId_ = objectId;
		}
	}
}

void PlaneTrackerExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Should never happen as our engagement radius is very large!");
}

}

}
