// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/SceneTrackerExperience.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

SceneTrackerExperience::~SceneTrackerExperience()
{
	// nothing to do here
}

bool SceneTrackerExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
	sceneTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Scene Tracker");
#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
	sceneTracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF Scene Tracker");
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
			}
		}
	}

	if (!sceneTracker6DOF_->start())
	{
		Log::error() << "Scene Tracker Experience could not start the scene tracker";
		return false;
	}

	sceneTrackerSampleEventSubscription_ = sceneTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &SceneTrackerExperience::onSceneTrackerSample));

	if (!anchoredContentManager_.initialize(std::bind(&SceneTrackerExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	return true;
}

bool SceneTrackerExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	sceneTrackerSampleEventSubscription_.release();

	renderingGroup_.release();

	anchoredContentManager_.release();

	sceneTracker6DOF_.release();

	return true;
}

Timestamp SceneTrackerExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedLock(lock_);
		const Devices::SceneTracker6DOF::SharedSceneElements recentSceneElements(std::move(recentSceneElements_));
		const Devices::SceneTracker6DOF::ObjectId objectId(objectId_);
	scopedLock.release();

	if (!renderingGroup_ && objectId != Devices::SceneTracker6DOF::invalidObjectId())
	{
		renderingGroup_ = engine->factory().createGroup();

		constexpr Scalar visibilityRadius = Scalar(1000); // 1km
		constexpr Scalar engagementRadius = Scalar(10000);
		anchoredContentManager_.addContent(renderingGroup_, sceneTracker6DOF_, objectId, visibilityRadius, engagementRadius);
	}

	if (renderingGroup_ && !recentSceneElements.empty())
	{
		if (recentSceneElements.front()->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_OBJECT_POINTS)
		{
			for (const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement : recentSceneElements)
			{
				ocean_assert(sceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_OBJECT_POINTS);
				const Devices::SceneTracker6DOF::SceneElementObjectPoints& sceneElementObjectPoints = dynamic_cast<const Devices::SceneTracker6DOF::SceneElementObjectPoints&>(*sceneElement);

				const Vectors3& objectPoints = sceneElementObjectPoints.objectPoints();
				const Indices64& objectPointIds = sceneElementObjectPoints.objectPointIds();

				if (objectPoints.size() == objectPointIds.size())
				{
					ocean_assert(objectPoints.size() == objectPointIds.size());
					for (size_t n = 0; n < objectPoints.size(); ++n)
					{
						objectPointMap_[objectPointIds[n]] = objectPoints[n]; // setting or updating the 3D location
					}
				}
				else
				{
					Log::warning() << "The scene tracker sample does not contain ids for the object points";
				}
			}

			constexpr double updateInterval = 0.5;

			if (!objectPointMap_.empty() && (renderingObjectTimestamp_.isInvalid() || timestamp >= renderingObjectTimestamp_ + updateInterval))
			{
				ocean_assert(objectId != Devices::SceneTracker6DOF::invalidObjectId());

				Vectors3 vertices;
				vertices.reserve(objectPointMap_.size());

				for (ObjectPointMap::const_iterator i = objectPointMap_.cbegin(); i != objectPointMap_.cend(); ++i)
				{
					vertices.emplace_back(i->second);
				}

				ocean_assert(!vertices.empty());

				renderingGroup_->clear();
				renderingGroup_->addChild(Rendering::Utilities::createPoints(*engine, vertices, RGBAColor(1.0f, 0.0, 0.0), Scalar(7)));

				renderingObjectTimestamp_ = timestamp;
			}
		}
		else if (recentSceneElements.front()->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_MESHES)
		{
			constexpr double updateInterval = 0.1;

			if (renderingObjectTimestamp_.isInvalid() || timestamp >= renderingObjectTimestamp_ + updateInterval)
			{
				const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement = recentSceneElements.back();
				const Devices::SceneTracker6DOF::SceneElementMeshes& sceneElementMeshes = dynamic_cast<const Devices::SceneTracker6DOF::SceneElementMeshes&>(*sceneElement);

				// we do not update all meshes at the same time (due to performance reasons)
				// instead, we apply a round robin update mechanism and skip meshes which have not changed

				Rendering::TriangleFaces reusableTriangleFaces;

				for (size_t nMesh = 0; nMesh < sceneElementMeshes.meshes().size(); ++nMesh)
				{
					const size_t nMeshIndex = (nMesh + roundRobinMeshUpdateIndex) % sceneElementMeshes.meshes().size();

					const Devices::SceneTracker6DOF::SceneElementMeshes::SharedMesh& mesh = sceneElementMeshes.meshes()[nMeshIndex];
					ocean_assert(mesh);

					MeshRenderingObject& meshRenderingObject = meshRenderingObjectMap_[mesh->meshId()];

					if (meshRenderingObject.lastUpdateTimestamp_.isInvalid() || timestamp >= meshRenderingObject.lastUpdateTimestamp_ + updateInterval)
					{
						if (meshRenderingObject.numberVertices_ != mesh->vertices().size())
						{
							const Vectors3& vertices = mesh->vertices();
							const Vectors3& perVertexNormals = mesh->perVertexNormals();
							const Indices32& triangleIndices = mesh->triangleIndices();
							ocean_assert(triangleIndices.size() % 3 == 0);

							reusableTriangleFaces.clear();
							for (size_t n = 0; n < triangleIndices.size(); n += 3)
							{
								const Index32& index0 = triangleIndices[n + 0];
								const Index32& index1 = triangleIndices[n + 1];
								const Index32& index2 = triangleIndices[n + 2];

								ocean_assert(index0 < vertices.size());
								ocean_assert(index1 < vertices.size());
								ocean_assert(index2 < vertices.size());

								reusableTriangleFaces.emplace_back(index0, index1, index2);
							}

							if (meshRenderingObject.color_ == RGBAColor(1.0f, 1.0f, 1.0f))
							{
								// the color has not been set, we set the color based on the initial (mean) mesh normal

								Vector3 planeNormal(0, 0, 0);

								for (const Vector3& normal : perVertexNormals)
								{
									planeNormal += normal;
								}

								planeNormal.normalize(); // we use the normal as the plane's color
								planeNormal = planeNormal * Scalar(0.5) + Vector3(Scalar(0.5), Scalar(0.5), Scalar(0.5));

								meshRenderingObject.color_ = RGBAColor(float(planeNormal.x()), float(planeNormal.y()), float(planeNormal.z()), 0.9f);
							}

							if (meshRenderingObject.renderingTransform_)
							{
								renderingGroup_->removeChild(meshRenderingObject.renderingTransform_);
								meshRenderingObject.renderingTransform_.release();
							}

							meshRenderingObject.renderingTransform_ = Rendering::Utilities::createMesh(engine, vertices, reusableTriangleFaces, meshRenderingObject.color_, perVertexNormals);
							meshRenderingObject.renderingTransform_->setTransformation(mesh->world_T_mesh());

							renderingGroup_->addChild(meshRenderingObject.renderingTransform_);

							meshRenderingObject.numberVertices_ = vertices.size();
							meshRenderingObject.lastUpdateTimestamp_ = timestamp;

							renderingObjectTimestamp_ = timestamp;

							roundRobinMeshUpdateIndex = nMeshIndex + 1u;

							break;
						}
					}
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

std::unique_ptr<XRPlaygroundExperience> SceneTrackerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new SceneTrackerExperience());
}

void SceneTrackerExperience::onSceneTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample)
{
	const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sceneTrackerSample(sample);

	if (!sceneTrackerSample || sceneTrackerSample->sceneElements().empty())
	{
		return;
	}

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	constexpr Devices::SceneTracker6DOF::SceneElement::SceneElementType sceneElementType = Devices::SceneTracker6DOF::SceneElement::SET_OBJECT_POINTS; // on Android we handle object points
#else
	constexpr Devices::SceneTracker6DOF::SceneElement::SceneElementType sceneElementType = Devices::SceneTracker6DOF::SceneElement::SET_MESHES; // on Android we handle object points
#endif

	for (size_t n = 0; n < sceneTrackerSample->objectIds().size(); ++n)
	{
		const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement = sceneTrackerSample->sceneElements()[n];

		if (sceneElement &&  sceneElement->sceneElementType() == sceneElementType)
		{
			const ScopedLock scopedLock(lock_);

			recentSceneElements_.emplace_back(sceneElement);
			objectId_ = sceneTrackerSample->objectIds()[n];

			break;
		}
	}
}

void SceneTrackerExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Should never happen as our engagement radius is very large!");
}

}

}
