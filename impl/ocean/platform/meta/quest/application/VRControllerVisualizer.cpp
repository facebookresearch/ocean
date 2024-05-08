/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/application/VRControllerVisualizer.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/Cylinder.h"
#include "ocean/rendering/Manager.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/scenedescription/Manager.h"
#include "ocean/scenedescription/Scene.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace Application
{

VRControllerVisualizer::ScopedState::ScopedState(VRControllerVisualizer& vrControllerVisualizer)
{
	if (vrControllerVisualizer.isValid())
	{
		vrControllerVisualizer_ = &vrControllerVisualizer;

		wasShown_ = vrControllerVisualizer.isShown();
	}
}

VRControllerVisualizer::ScopedState::ScopedState(ScopedState&& scopedState)
{
	*this = std::move(scopedState);
}

VRControllerVisualizer::ScopedState::~ScopedState()
{
	release();
}

void VRControllerVisualizer::ScopedState::release()
{
	if (vrControllerVisualizer_ != nullptr)
	{
		if (wasShown_)
		{
			vrControllerVisualizer_->show();
		}
		else
		{
			vrControllerVisualizer_->hide();
		}

		vrControllerVisualizer_->setControllerRayLength(controllerRayLength_);

		vrControllerVisualizer_ = nullptr;
	}

	wasShown_ = false;
}

VRControllerVisualizer::ScopedState& VRControllerVisualizer::ScopedState::operator=(ScopedState&& scopedState)
{
	if (this != &scopedState)
	{
		release();

		vrControllerVisualizer_ = scopedState.vrControllerVisualizer_;
		scopedState.vrControllerVisualizer_ = nullptr;

		wasShown_ = scopedState.wasShown_;
		scopedState.wasShown_ = false;

		controllerRayLength_ = scopedState.controllerRayLength_;
		scopedState.controllerRayLength_ = Scalar(0);
	}

	return *this;
}

VRControllerVisualizer::~VRControllerVisualizer()
{
	release();
}

void VRControllerVisualizer::visualizeControllerInWorld(const ControllerType controllerType, const HomogenousMatrix4& world_T_controller, const Scalar controllerRayLength)
{
	ocean_assert(isValid());

	if (!isValid())
	{
		return;
	}

	const Rendering::SceneRef& controllerScene = controllerType == CT_LEFT ? leftControllerScene_ : rightControllerScene_;

	if (controllerScene.isNull())
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	if (!isShown_ || !world_T_controller.isValid())
	{
		controllerScene->setVisible(false);
		return;
	}

	controllerScene->setTransformation(world_T_controller);

	Rendering::TransformRef& controllerRayTransform = controllerType == CT_LEFT ? leftControllerRayTransform_ : rightControllerRayTransform_;

	// Additional translation along the z-axis to prevent the ray from intersecting with the controller model
	constexpr Scalar zOffset = Scalar(-0.04);

	const Scalar explicitControllerRayLength = controllerRayLength >= 0 ? controllerRayLength : this->controllerRayLength();

	const Scalar controllerRayLengthAfterOffset = explicitControllerRayLength + zOffset;
	constexpr Scalar controllerRayDiameter = Scalar(0.005);

	if (controllerRayLengthAfterOffset > 0)
	{
		const HomogenousMatrix4 cylinderScale(Vector3(0, 0, 0), Vector3(controllerRayDiameter, controllerRayLengthAfterOffset, controllerRayDiameter));
		const HomogenousMatrix4 cylinderRotate(Rotation(Vector3(Scalar(1), 0, 0), -Numeric::pi_2()));
		const HomogenousMatrix4 cylinderTranslation(Vector3(0, 0, zOffset - controllerRayLengthAfterOffset * Scalar(0.5)));

		controllerRayTransform->setTransformation(cylinderTranslation * cylinderRotate * cylinderScale);
	}

	controllerRayTransform->setVisible(controllerRayLengthAfterOffset > 0);

	controllerScene->setVisible(true);
}

void VRControllerVisualizer::release()
{
	if (leftControllerScene_)
	{
		if (framebuffer_)
		{
			framebuffer_->removeScene(leftControllerScene_);
		}

		leftControllerScene_.release();
	}

	if (rightControllerScene_)
	{
		if (framebuffer_)
		{
			framebuffer_->removeScene(rightControllerScene_);
		}

		rightControllerScene_.release();
	}

	leftControllerRayTransform_.release();
	rightControllerRayTransform_.release();

	framebuffer_.release();
	engine_.release();
}

VRControllerVisualizer& VRControllerVisualizer::operator=(VRControllerVisualizer&& vrControllerVisualizer)
{
	if (this != &vrControllerVisualizer)
	{
		release();

		deviceType_ = vrControllerVisualizer.deviceType_;
		vrControllerVisualizer.deviceType_ = Device::DT_UNKNOWN;

		leftControllerScene_ = std::move(vrControllerVisualizer.leftControllerScene_);
		rightControllerScene_ = std::move(vrControllerVisualizer.rightControllerScene_);

		leftControllerRayTransform_ = std::move(vrControllerVisualizer.leftControllerRayTransform_);
		rightControllerRayTransform_ = std::move(vrControllerVisualizer.rightControllerRayTransform_);

		isShown_ = bool(vrControllerVisualizer.isShown_);
		vrControllerVisualizer.isShown_ = true;

		controllerRayLength_ = Scalar(vrControllerVisualizer.controllerRayLength_);
		vrControllerVisualizer.controllerRayLength_ = 0;

		VRVisualizer::operator=(std::move(vrControllerVisualizer));
	}

	return *this;
}

bool VRControllerVisualizer::loadModels(const std::string& leftRenderModelFilename, const std::string& rightRenderModelFilename)
{
	if (!engine_ || !framebuffer_)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	// Load the controller models from file

	for (const bool isLeftController : {true, false})
	{
		Rendering::SceneRef& controllerScene = isLeftController ? leftControllerScene_ : rightControllerScene_;
		const std::string renderModelFilename = isLeftController ? leftRenderModelFilename : rightRenderModelFilename;

		ocean_assert(controllerScene.isNull());

		const Timestamp currentTimestamp(0.0); // controllers do not contain animation so that any timestamp will work

		const SceneDescription::SceneRef sceneDescriptionScene = SceneDescription::Manager::get().load(renderModelFilename, engine_, currentTimestamp, SceneDescription::TYPE_TRANSIENT);

		if (sceneDescriptionScene)
		{
			const SceneDescription::SDLSceneRef sdlScene(sceneDescriptionScene);
			ocean_assert(sdlScene);

			controllerScene = sdlScene->apply(engine_);

			if (controllerScene)
			{
#ifdef OCEAN_DEBUG
				controllerScene->setName("(debug) VRControllerVisualizer scene " + (isLeftController ? std::string("left") : std::string("right")));
#endif

				framebuffer_->addScene(controllerScene);
			}
		}
	}

	if (leftControllerScene_.isNull() || rightControllerScene_.isNull())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	// Initialize the controller rays

	for (size_t c = 0; c < 2; ++c)
	{
		const bool isLeftController = c == 0;

		const RGBAColor color(1.0f, 1.0f, 1.0f, 0.4f);

		const Rendering::MaterialRef material(engine_->factory().createMaterial());
		material->setDiffuseColor(color);
		material->setTransparency(color.alpha());

		const Rendering::AttributeSetRef attributeSet(engine_->factory().createAttributeSet());
		attributeSet->addAttribute(material);

		if (material->transparency() != 0.0f)
		{
			attributeSet->addAttribute(engine_->factory().createBlendAttribute());
		}

		const Rendering::CylinderRef cylinderAxis(engine_->factory().createCylinder());
		cylinderAxis->setRadius(Scalar(0.5)); // unit diameter
		cylinderAxis->setHeight(Scalar(1)); // unit length

		const Rendering::GeometryRef geometryAxis(engine_->factory().createGeometry());
		geometryAxis->addRenderable(cylinderAxis, attributeSet);

		Rendering::TransformRef& controllerRayTransform = isLeftController ? leftControllerRayTransform_ : rightControllerRayTransform_;
		ocean_assert(controllerRayTransform.isNull());

		controllerRayTransform = Rendering::TransformRef(engine_->factory().createTransform());
		controllerRayTransform->addChild(geometryAxis);

		Rendering::SceneRef& controllerScene = isLeftController ? leftControllerScene_ : rightControllerScene_;

		controllerScene->setVisible(false);
		controllerScene->addChild(controllerRayTransform);
	}

	return true;
}

bool VRControllerVisualizer::loadModels(const Device::DeviceType deviceType, const std::string& renderModelDirectoryName)
{
	if (deviceType == Device::DT_UNKNOWN || renderModelDirectoryName.empty())
	{
		ocean_assert(false && "Invalid input");
		return false;
	}

	const IO::Directory renderModelDirectory(renderModelDirectoryName);

	if (!renderModelDirectory.exists())
	{
		Log::error() << "The directory containing the controller models does not exist";
		return false;
	}

	IO::File leftRenderModelFile;
	IO::File rightRenderModelFile;

	switch (deviceType)
	{
		case Device::DT_QUEST:
			leftRenderModelFile = renderModelDirectory + IO::File("quest_controller_left.obj");
			rightRenderModelFile = renderModelDirectory + IO::File("quest_controller_right.obj");
			break;

		case Device::DT_QUEST_2:
			leftRenderModelFile = renderModelDirectory + IO::File("quest2_controller_left.obj");
			rightRenderModelFile = renderModelDirectory + IO::File("quest2_controller_right.obj");
			break;

		case Device::DT_QUEST_PRO:
		case Device::DT_QUEST_3:
			leftRenderModelFile = renderModelDirectory + IO::File("quest3_controller_left.obj");
			rightRenderModelFile = renderModelDirectory + IO::File("quest3_controller_right.obj");
			break;

		case Device::DT_UNKNOWN:
		case Device::DT_QUEST_END:
			ocean_assert(false && "Never be here!");
			return false;
	}

#ifdef OCEAN_PLATFORM_META_QUEST_OPENXR_USE_EXTERNAL_CONTROLLER_MODEL_FILE

	if (deviceType > Device::DT_QUEST_END)
	{
		ocean_assert(!leftRenderModelFile.isValid());
		ocean_assert(!rightRenderModelFile.isValid());

		std::string leftModelFile;
		std::string rightModelFile;
		if (VRControllerVisualizer_externalControllerModelFiles(deviceType, leftModelFile, rightModelFile))
		{
			leftRenderModelFile = renderModelDirectory + IO::File(leftModelFile);
			rightRenderModelFile = renderModelDirectory + IO::File(rightModelFile);
		}
	}
#endif // OCEAN_PLATFORM_META_QUEST_OPENXR_USE_EXTERNAL_CONTROLLER_MODEL_FILE

	if (!leftRenderModelFile.isValid())
	{
		Log::error() << "No valid controller model for device '" << Platform::Meta::Quest::Device::deviceName(deviceType) << "'";
		return false;
	}

	if (!leftRenderModelFile.exists())
	{
		Log::error() << "The controller model '" << leftRenderModelFile() << "' does not exist";
		return false;
	}

	if (!rightRenderModelFile.exists())
	{
		Log::error() << "The controller model '" << rightRenderModelFile() << "' does not exist";
		return false;
	}

	return loadModels(leftRenderModelFile(), rightRenderModelFile());
}

}

}

}

}

}
