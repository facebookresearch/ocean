// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/GravityAndHeadingNativeExperience.h"

#include "ocean/devices/Manager.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

GravityAndHeadingNativeExperience::~GravityAndHeadingNativeExperience()
{
	// nothing to do here
}

bool GravityAndHeadingNativeExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

#ifdef OCEAN_PLATFORM_BUILD_ANDROID // **TODO** avoid platform specific tracker names
	gravityTracker3DOF_ = Devices::Manager::get().device("Android 3DOF Gravity Tracker");
	headingTracker3DOF_ = Devices::Manager::get().device("Android 3DOF Heading Tracker");
#else
	gravityTracker3DOF_ = Devices::Manager::get().device("IOS 3DOF Gravity Tracker");
	headingTracker3DOF_ = Devices::Manager::get().device("IOS 3DOF Heading Tracker");
#endif

	if (headingTracker3DOF_.isNull() || gravityTracker3DOF_.isNull())
	{
		Log::error() << "Failed to access Gravity or Heading tracker";
		return false;
	}

	if (!gravityTracker3DOF_->start() || !headingTracker3DOF_->start())
	{
		Log::error() << "Failed to start Gravity or Heading tracker";
		return false;
	}

	const Rendering::AbsoluteTransformRef absoluteTransformation = engine->factory().createAbsoluteTransform();
	absoluteTransformation->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP); // the head-up transformation allows to place content as "head-up display"
	absoluteTransformation->setTransformation(HomogenousMatrix4(Vector3(0, 0, -1)));

	experienceScene()->addChild(absoluteTransformation);

	renderingGravityTransform_ = Rendering::Utilities::createArrow(engine, Scalar(0.45), Scalar(0.05), Scalar(0.025), RGBAColor(0.0f, 1.0f, 1.0f));
	renderingHeadingTransform_ = Rendering::Utilities::createArrow(engine, Scalar(0.45), Scalar(0.05), Scalar(0.025), RGBAColor(1.0f, 1.0f, 0.0f));

	renderingGravityTransform_->setVisible(false); // we hide the arrows until we have valid tracking samples
	renderingHeadingTransform_->setVisible(false);

	absoluteTransformation->addChild(renderingGravityTransform_);
	absoluteTransformation->addChild(renderingHeadingTransform_);

	return true;
}

bool GravityAndHeadingNativeExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	gravityTracker3DOF_.release();
	headingTracker3DOF_.release();

	renderingGravityTransform_.release();
	renderingHeadingTransform_.release();

	return true;
}

Timestamp GravityAndHeadingNativeExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(gravityTracker3DOF_ && headingTracker3DOF_);

	const Vector3 offsetVector(0, Scalar(0.03), 0);

	// let's get the gravity and heading samples for the current frame timestamp (linear interpolated)

	const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef gravitySample = gravityTracker3DOF_->sample(timestamp, Devices::OrientationTracker3DOF::IS_TIMESTAMP_INTERPOLATE);
	if (gravitySample && !gravitySample->orientations().empty())
	{
		ocean_assert(renderingGravityTransform_);

		Quaternion device_R_gravity(false);
		if (gravitySample->referenceSystem() == Devices::OrientationTracker3DOF::RS_OBJECT_IN_DEVICE)
		{
			// the object (gravity) is defined in the device coordinate system
			device_R_gravity = gravitySample->orientations().front();
		}
		else
		{
			// the device is defined in the object (gravity) coordinate system
			device_R_gravity = gravitySample->orientations().front().inverted();
		}

		ocean_assert(device_R_gravity.isValid());
		const Quaternion gravity_R_yAxis(Vector3(1, 0, 0), Numeric::pi()); // the arrow is defined along the positive y-axis, gravity is pointing into the opposite direction

		const HomogenousMatrix4 device_T_yAxis = HomogenousMatrix4(device_R_gravity * gravity_R_yAxis);
		const HomogenousMatrix4 device_T_yAxisOffset = device_T_yAxis * HomogenousMatrix4(offsetVector);

		renderingGravityTransform_->setTransformation(device_T_yAxisOffset);
		renderingGravityTransform_->setVisible(true);
	}

	const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef headingSample = headingTracker3DOF_->sample(timestamp, Devices::OrientationTracker3DOF::IS_TIMESTAMP_INTERPOLATE);
	if (headingSample && !headingSample->orientations().empty())
	{
		ocean_assert(renderingHeadingTransform_);

		 Quaternion device_R_heading(false);
		if (headingSample->referenceSystem() == Devices::OrientationTracker3DOF::RS_OBJECT_IN_DEVICE)
		{
			// the object (heading) is defined in the device coordinate system
			device_R_heading = headingSample->orientations().front();
		}
		else
		{
			// the device is defined in the object (heading) coordinate system
			device_R_heading = headingSample->orientations().front().inverted();
		}

		const Quaternion heading_R_yAxis(Vector3(1, 0, 0), -Numeric::pi_2()); // the arrow is defined along the positive y-axis, heading is pointing along the positive z-axis

		const HomogenousMatrix4 device_T_yAxis = HomogenousMatrix4(device_R_heading * heading_R_yAxis);
		const HomogenousMatrix4 device_T_yAxisOffset = device_T_yAxis * HomogenousMatrix4(offsetVector);

		renderingHeadingTransform_->setTransformation(device_T_yAxisOffset);
		renderingHeadingTransform_->setVisible(true);
	}

	return timestamp;
}

std::unique_ptr<XRPlaygroundExperience> GravityAndHeadingNativeExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new GravityAndHeadingNativeExperience());
}

}

}
