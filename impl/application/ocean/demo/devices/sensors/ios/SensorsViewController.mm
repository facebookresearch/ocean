/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "SensorsViewController.h"

#include "ocean/base/Lock.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"

#include "ocean/cv/fonts/FontManager.h"

#include "ocean/devices/AccelerationSensor3DOF.h"
#include "ocean/devices/GravityTracker3DOF.h"
#include "ocean/devices/Manager.h"
#include "ocean/devices/Measurement.h"
#include "ocean/devices/OrientationTracker3DOF.h"

#include "ocean/math/Euler.h"

#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Text.h"
#include "ocean/rendering/Utilities.h"

#include <atomic>
#include <deque>

using namespace Ocean;

namespace
{

/**
 * Definition of the different application states.
 */
enum ApplicationState : uint32_t
{
	/// The state showing the acceleration sensor data.
	AS_ACCELERATION = 0u,
	/// The state showing the linear acceleration sensor data.
	AS_LINEAR_ACCELERATION,
	/// The state showing the orientation tracker data.
	AS_ORIENTATION,
	/// The state showing the rotation data.
	AS_ROTATION,
	/// The state showing the heading and gravity data.
	AS_HEADING_AND_GRAVITY,

	/// The number of application states.
	AS_SIZE
};

/**
 * This class implements a callback handler for sensor samples.
 * It calculates the sample delay and maintains a running average.
 */
class SampleCallbackHandler
{
	public:

		/**
		 * Creates a new sample callback handler.
		 */
		SampleCallbackHandler() = default;

		/**
		 * Event callback for new measurement samples.
		 * This function calculates the sample delay and updates the running average.
		 * @param measurement The measurement object sending the sample
		 * @param sample The new sample
		 */
		void onSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample);

		/**
		 * Returns the averaged sample delay in milliseconds.
		 * @return The averaged sample delay in milliseconds, NumericD::minValue() if no samples have been received yet
		 */
		double averageSampleDelayMs() const;

		/**
		 * Resets the sample delay measurements.
		 */
		void reset();

	protected:

		/// The number of delay samples to average.
		static constexpr size_t numberDelaySamples_ = 10;

		/// The lock for the delay samples.
		mutable Lock delayLock_;

		/// The recent sample delay measurements in milliseconds for averaging.
		std::deque<double> recentSampleDelaysMs_;

		/// The running sum of the delay samples.
		double delaySumMs_ = 0.0;

		/// The averaged sample delay in milliseconds.
		std::atomic<double> averageSampleDelayMs_{NumericD::minValue()};
};

void SampleCallbackHandler::onSample(const Devices::Measurement* /*measurement*/, const Devices::Measurement::SampleRef& sample)
{
	ocean_assert(sample);

	if (!sample)
	{
		return;
	}

	const double delayMs = double(Timestamp(true) - sample->timestamp()) * 1000.0;

	const ScopedLock scopedLock(delayLock_);

	recentSampleDelaysMs_.push_back(delayMs);
	delaySumMs_ += delayMs;

	if (recentSampleDelaysMs_.size() > numberDelaySamples_)
	{
		delaySumMs_ -= recentSampleDelaysMs_.front();
		recentSampleDelaysMs_.pop_front();
	}

	averageSampleDelayMs_ = delaySumMs_ / double(recentSampleDelaysMs_.size());
}

double SampleCallbackHandler::averageSampleDelayMs() const
{
	return averageSampleDelayMs_.load();
}

void SampleCallbackHandler::reset()
{
	const ScopedLock scopedLock(delayLock_);

	recentSampleDelaysMs_.clear();
	delaySumMs_ = 0.0;
	averageSampleDelayMs_ = NumericD::minValue();
}

}

@interface SensorsViewController ()
{
	Rendering::SceneRef scene_;

	Devices::AccelerationSensor3DOFRef accelerationSensor_;
	Devices::AccelerationSensor3DOFRef linearAccelerationSensor_;
	Devices::OrientationTracker3DOFRef orientationTracker_;
	Devices::OrientationTracker3DOFRef headingTracker_;
	Devices::GravityTracker3DOFRef gravityTracker_;

	Devices::Measurement::SampleEventSubscription accelerationSensorSubscription_;
	Devices::Measurement::SampleEventSubscription linearAccelerationSensorSubscription_;
	Devices::Measurement::SampleEventSubscription orientationTrackerSubscription_;
	Devices::Measurement::SampleEventSubscription headingTrackerSubscription_;
	Devices::Measurement::SampleEventSubscription gravityTrackerSubscription_;

	Rendering::TextRef renderingTextState_;
	Rendering::TextRef renderingTextDelay_;

	Rendering::TransformRef renderingTransformAcceleration_;
	Rendering::TransformRef renderingTransformAccelerationX_;
	Rendering::TransformRef renderingTransformAccelerationY_;
	Rendering::TransformRef renderingTransformAccelerationZ_;
	Rendering::TextRef renderingTextAcceleration_;

	Rendering::TransformRef renderingTransformOrientation_;

	Rendering::TransformRef renderingTransformRotation_;
	Rendering::TextRef renderingTextRotation_;

	Rendering::TransformRef renderingTransformHeadingAndGravity_;
	Rendering::TransformRef renderingTransformHeading_;
	Rendering::TransformRef renderingTransformGravity_;

	unsigned int applicationState_;

	bool switchApplicationState_;

	SampleCallbackHandler sampleCallbackHandler_;
}

@end

@implementation SensorsViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

	RandomI::initialize();

	const size_t registeredFonts = CV::Fonts::FontManager::get().registerSystemFonts();
	Log::debug() << "Registered fonts: " << registeredFonts;

	applicationState_ = 0u;
	switchApplicationState_ = false;

	if (renderingEngine_.isNull() || renderingFramebuffer_.isNull())
	{
		return;
	}

	[self setViewInteractionEnabled:false];

	scene_ = renderingEngine_->factory().createScene();
	renderingFramebuffer_->addScene(scene_);

	Rendering::TransformRef textTransform = Rendering::Utilities::createText(*renderingEngine_, "", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false, 0, 0, Scalar(1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingTextState_);
	textTransform->setTransformation(HomogenousMatrix4(Vector3(0, 3.8, 4)));

	scene_->addChild(textTransform);

	Rendering::TransformRef delayTextTransform = Rendering::Utilities::createText(*renderingEngine_, "Delay: -- ms", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.5f), false, 0, 0, Scalar(0.4), Rendering::Text::AM_CENTER, Rendering::Text::HA_LEFT, Rendering::Text::VA_BOTTOM, std::string(), std::string(), &renderingTextDelay_);
	delayTextTransform->setTransformation(HomogenousMatrix4(Vector3(-3.5, -3.5, 4)));

	scene_->addChild(delayTextTransform);

	UITapGestureRecognizer* tapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTap:)];
	[self.view addGestureRecognizer:tapRecognizer];
}

- (void)handleTap:(UITapGestureRecognizer *)recognizer
{
	switchApplicationState_ = true;
}

- (void)showAcceleration
{
	if (!accelerationSensor_)
	{
		accelerationSensor_ = Devices::Manager::get().device("IOS 3DOF Acceleration Sensor");

		if (!accelerationSensor_)
		{
			Log::error() << "SensorsViewController: Failed to access the acceleration sensor";
			return;
		}

		accelerationSensorSubscription_ = accelerationSensor_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(sampleCallbackHandler_, &SampleCallbackHandler::onSample));
	}

	if (!accelerationSensor_->start())
	{
		Log::error() << "SensorsViewController: Failed to start the acceleration sensor";
		return;
	}

	const Devices::AccelerationSensor3DOF::Acceleration3DOFSampleRef sample(accelerationSensor_->sample());

	if (sample && sample->measurements().size() >= 1)
	{
		const Vector3 acceleration = sample->measurements().front();

		[self showAccelerationWithValue:acceleration scaledValue:acceleration * Scalar(0.5)];
	}
}

- (void)showLinearAcceleration
{
	if (!linearAccelerationSensor_)
	{
		linearAccelerationSensor_ = Devices::Manager::get().device("IOS 3DOF Linear Acceleration Sensor");

		if (!linearAccelerationSensor_)
		{
			Log::error() << "SensorsViewController: Failed to access the linear acceleration sensor";
			return;
		}

		linearAccelerationSensorSubscription_ = linearAccelerationSensor_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(sampleCallbackHandler_, &SampleCallbackHandler::onSample));
	}

	if (!linearAccelerationSensor_->start())
	{
		Log::error() << "SensorsViewController: Failed to start the linear acceleration sensor";
		return;
	}

	const Devices::AccelerationSensor3DOF::Acceleration3DOFSampleRef sample(linearAccelerationSensor_->sample());

	if (sample && sample->measurements().size() == 1)
	{
		const Vector3 acceleration = sample->measurements().front();

		static std::deque<Vector3> accelerationQueue;
		accelerationQueue.push_back(acceleration);

		while (accelerationQueue.size() >= 100)
		{
			accelerationQueue.pop_front();
		}

		Vector3 filteredAcceleration(0, 0, 0);
		for (const Vector3& v : accelerationQueue)
		{
			filteredAcceleration += v;
		}

		filteredAcceleration *= Numeric::ratio(1, accelerationQueue.size(), 1);

		[self showAccelerationWithValue:filteredAcceleration scaledValue:filteredAcceleration * Scalar(5)];
	}
}

- (void)showAccelerationWithValue:(const Vector3&)acceleration scaledValue:(const Vector3&)scaledAcceleration
{
	ocean_assert(scene_ && renderingEngine_);

	if (!renderingTransformAcceleration_)
	{
		renderingTransformAcceleration_ = renderingEngine_->factory().createTransform();
		renderingTransformAcceleration_->setVisible(false);
		scene_->addChild(renderingTransformAcceleration_);

		Rendering::TransformRef transformAxis_ = renderingEngine_->factory().createTransform();
		transformAxis_->setTransformation(HomogenousMatrix4(Quaternion(Vector3(1, 0, 0), Numeric::deg2rad(20))));
		renderingTransformAcceleration_->addChild(transformAxis_);

		renderingTransformAccelerationX_ = Rendering::Utilities::createCylinder(renderingEngine_, Scalar(0.1), 1, RGBAColor(1, 0, 0));
		renderingTransformAccelerationY_ = Rendering::Utilities::createCylinder(renderingEngine_, Scalar(0.1), 1, RGBAColor(0, 1, 0));
		renderingTransformAccelerationZ_ = Rendering::Utilities::createCylinder(renderingEngine_, Scalar(0.1), 1, RGBAColor(0, 0, 1));

		transformAxis_->addChild(renderingTransformAccelerationX_);
		transformAxis_->addChild(renderingTransformAccelerationY_);
		transformAxis_->addChild(renderingTransformAccelerationZ_);

		ocean_assert(!renderingTextAcceleration_);
		Rendering::TransformRef textTransform = Rendering::Utilities::createText(*renderingEngine_, "", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false, 0, 0, Scalar(0.5), Rendering::Text::AM_CENTER, Rendering::Text::HA_LEFT, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingTextAcceleration_);
		textTransform->setTransformation(HomogenousMatrix4(Vector3(1, 1.5, 1)));

		renderingTransformAcceleration_->addChild(textTransform);
	}

	renderingTextAcceleration_->setText(String::toAString(acceleration.x(), 1u) + ", " + String::toAString(acceleration.y(), 1u) + ", " + String::toAString(acceleration.z(), 1u) + " m/s^2");

	const Scalar absAccelerationX = minmax(Scalar(0.001), Numeric::abs(scaledAcceleration.x()), Scalar(4));
	const Scalar absAccelerationY = minmax(Scalar(0.001), Numeric::abs(scaledAcceleration.y()), Scalar(4));
	const Scalar absAccelerationZ = minmax(Scalar(0.001), Numeric::abs(scaledAcceleration.z()), Scalar(4));

	HomogenousMatrix4 device_T_accelerationX(false);
	HomogenousMatrix4 device_T_accelerationY(false);
	HomogenousMatrix4 device_T_accelerationZ(false);

	if (scaledAcceleration.x() >= 0)
	{
		device_T_accelerationX = HomogenousMatrix4(Vector3(absAccelerationX * Scalar(0.5), 0, 0), Quaternion(Vector3(0, 0, 1), -Numeric::pi_2()), Vector3(1, absAccelerationX, 1));
	}
	else
	{
		device_T_accelerationX = HomogenousMatrix4(Vector3(-absAccelerationX * Scalar(0.5), 0, 0), Quaternion(Vector3(0, 0, 1), Numeric::pi_2()), Vector3(1, absAccelerationX, 1));
	}

	if (scaledAcceleration.y() >= 0)
	{
		device_T_accelerationY = HomogenousMatrix4(Vector3(0, absAccelerationY * Scalar(0.5), 0), Quaternion(Vector3(1, 0, 0), 0), Vector3(1, absAccelerationY, 1));
	}
	else
	{
		device_T_accelerationY = HomogenousMatrix4(Vector3(0, -absAccelerationY * Scalar(0.5), 0), Quaternion(Vector3(1, 0, 0), Numeric::pi()), Vector3(1, absAccelerationY, 1));
	}

	if (scaledAcceleration.z() >= 0)
	{
		device_T_accelerationZ = HomogenousMatrix4(Vector3(0, 0, absAccelerationZ * Scalar(0.5)), Quaternion(Vector3(1, 0, 0), Numeric::pi_2()), Vector3(1, absAccelerationZ, 1));
	}
	else
	{
		device_T_accelerationZ = HomogenousMatrix4(Vector3(0, 0, -absAccelerationZ * Scalar(0.5)), Quaternion(Vector3(1, 0, 0), -Numeric::pi_2()), Vector3(1, absAccelerationZ, 1));
	}

	renderingTransformAccelerationX_->setTransformation(device_T_accelerationX);
	renderingTransformAccelerationY_->setTransformation(device_T_accelerationY);
	renderingTransformAccelerationZ_->setTransformation(device_T_accelerationZ);

	renderingTransformAcceleration_->setVisible(true);
}

- (void)showOrientation
{
	if (!renderingTransformOrientation_)
	{
		renderingTransformOrientation_ = Rendering::Utilities::createCoordinateSystem(renderingEngine_, Scalar(3), Scalar(0.3), Scalar(0.2));
		renderingTransformOrientation_->setVisible(false);
		scene_->addChild(renderingTransformOrientation_);
	}

	if (!orientationTracker_)
	{
		orientationTracker_ = Devices::Manager::get().device("IOS 3DOF Orientation Tracker");

		if (!orientationTracker_)
		{
			Log::error() << "SensorsViewController: Failed to access the orientation tracker";
			return;
		}

		orientationTrackerSubscription_ = orientationTracker_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(sampleCallbackHandler_, &SampleCallbackHandler::onSample));
	}

	if (!orientationTracker_->start())
	{
		Log::error() << "SensorsViewController: Failed to start the orientation tracker";
		return;
	}

	const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef sample(orientationTracker_->sample());

	if (sample && sample->orientations().size() == 1)
	{
		Quaternion device_R_rotation(false);
		if (sample->referenceSystem() == Devices::OrientationTracker3DOF::RS_OBJECT_IN_DEVICE)
		{
			device_R_rotation = sample->orientations().front();
		}
		else
		{
			device_R_rotation = sample->orientations().front().inverted();
		}

		renderingTransformOrientation_->setTransformation(HomogenousMatrix4(Vector3(0, 0, 0), device_R_rotation));
		renderingTransformOrientation_->setVisible(true);
	}
}

- (void)showRotationWithReset:(bool)reset
{
	if (!renderingTransformRotation_)
	{
		renderingTransformRotation_ = renderingEngine_->factory().createTransform();
		renderingTransformRotation_->setVisible(false);
		scene_->addChild(renderingTransformRotation_);

		ocean_assert(!renderingTextRotation_);
		Rendering::TransformRef textTransform = Rendering::Utilities::createText(*renderingEngine_, "", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false, 0, 0, Scalar(0.5), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingTextRotation_);
		textTransform->setTransformation(HomogenousMatrix4(Vector3(0, 1.5, 1)));

		renderingTransformRotation_->addChild(textTransform);
	}

	ocean_assert(orientationTracker_ && "showOrientation should have been called before");

	if (!orientationTracker_)
	{
		return;
	}

	const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef sample(orientationTracker_->sample());

	if (sample && sample->orientations().size() >= 1)
	{
		const Quaternion world_R_device = sample->orientations().front();

		static Quaternion resetDevice_R_world(world_R_device.inverted());

		if (reset)
		{
			resetDevice_R_world = world_R_device.inverted();
		}

		const Quaternion resetDevice_R_device = resetDevice_R_world * world_R_device;

		const Euler euler(resetDevice_R_device);

		renderingTextRotation_->setText("Yaw: " + String::toAString(Numeric::rad2deg(euler.yaw()), 1u) + ", pitch: " + String::toAString(Numeric::rad2deg(euler.pitch()), 1u) + ", roll: " + String::toAString(Numeric::rad2deg(euler.roll()), 1u));

		renderingTransformRotation_->setVisible(true);
	}
}

- (void)showHeadingAndGravity
{
	if (!headingTracker_)
	{
		headingTracker_ = Devices::Manager::get().device("IOS 3DOF Heading Tracker");

		if (!headingTracker_)
		{
			Log::error() << "SensorsViewController: Failed to access the heading tracker";
			return;
		}

		headingTrackerSubscription_ = headingTracker_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(sampleCallbackHandler_, &SampleCallbackHandler::onSample));
	}

	if (!headingTracker_->start())
	{
		Log::error() << "SensorsViewController: Failed to start the heading tracker";
		return;
	}

	if (!gravityTracker_)
	{
		gravityTracker_ = Devices::Manager::get().device("IOS 3DOF Gravity Tracker");

		if (!gravityTracker_)
		{
			Log::error() << "SensorsViewController: Failed to access the gravity tracker";
			return;
		}

		gravityTrackerSubscription_ = gravityTracker_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(sampleCallbackHandler_, &SampleCallbackHandler::onSample));
	}

	if (!gravityTracker_->start())
	{
		Log::error() << "SensorsViewController: Failed to start the gravity tracker";
		return;
	}

	if (!renderingTransformHeading_)
	{
		renderingTransformHeadingAndGravity_ = renderingEngine_->factory().createTransform();
		renderingTransformHeadingAndGravity_->setVisible(false);
		scene_->addChild(renderingTransformHeadingAndGravity_);

		renderingTransformHeading_ = Rendering::Utilities::createArrow(renderingEngine_, Scalar(3), Scalar(0.3), Scalar(0.2), RGBAColor(1.0f, 1.0f, 0.0f));
		renderingTransformHeadingAndGravity_->addChild(renderingTransformHeading_);

		renderingTransformGravity_ = Rendering::Utilities::createArrow(renderingEngine_, Scalar(3), Scalar(0.3), Scalar(0.2), RGBAColor(0.0f, 1.0f, 1.0f));
		renderingTransformHeadingAndGravity_->addChild(renderingTransformGravity_);
	}

	const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef headingSample(headingTracker_->sample());

	if (headingSample && headingSample->orientations().size() == 1)
	{
		const Devices::GravityTracker3DOF::GravityTracker3DOFSampleRef gravitySample = gravityTracker_->sample(headingSample->timestamp(), Devices::GravityTracker3DOF::IS_TIMESTAMP_INTERPOLATE);
		if (gravitySample && gravitySample->gravities().size() == 1)
		{
			ocean_assert(gravitySample->referenceSystem() == Devices::OrientationTracker3DOF::RS_OBJECT_IN_DEVICE);

			const Vector3 gravityVector = gravitySample->gravities().front();

			const Quaternion deviceGravity_Q_yAxisNegative = Quaternion::left_Q_right(gravityVector, Vector3(0, -1, 0));

			{
				const Quaternion yAxisNegative_R_yAxisPositive(Vector3(1, 0, 0), Numeric::pi());

				const HomogenousMatrix4 deviceGravity_T_yAxisPositive = HomogenousMatrix4(deviceGravity_Q_yAxisNegative * yAxisNegative_R_yAxisPositive);

				ocean_assert(renderingTransformGravity_);
				renderingTransformGravity_->setTransformation(deviceGravity_T_yAxisPositive);
			}

			{
				Quaternion device_R_heading(false);
				if (headingSample->referenceSystem() == Devices::OrientationTracker3DOF::RS_OBJECT_IN_DEVICE)
				{
					device_R_heading = headingSample->orientations().front();
				}
				else
				{
					device_R_heading = headingSample->orientations().front().inverted();
				}

				const Quaternion heading_R_yAxis(Vector3(1, 0, 0), -Numeric::pi_2());

				const HomogenousMatrix4 device_T_yAxis = HomogenousMatrix4(device_R_heading * heading_R_yAxis);

				ocean_assert(renderingTransformHeading_);
				renderingTransformHeading_->setTransformation(device_T_yAxis);
			}

			renderingTransformHeadingAndGravity_->setVisible(true);
		}
	}
}

- (void)update
{
	if (renderingFramebuffer_.isNull() || renderingEngine_.isNull())
	{
		return;
	}

	bool reset = false;

	if (switchApplicationState_)
	{
		switch (applicationState_)
		{
			case AS_ACCELERATION:
			{
				if (renderingTransformAcceleration_)
				{
					renderingTransformAcceleration_->setVisible(false);
				}

				if (accelerationSensor_)
				{
					accelerationSensor_->stop();
				}
				break;
			}

			case AS_LINEAR_ACCELERATION:
			{
				if (renderingTransformAcceleration_)
				{
					renderingTransformAcceleration_->setVisible(false);
				}

				if (linearAccelerationSensor_)
				{
					linearAccelerationSensor_->stop();
				}
				break;
			}

			case AS_ORIENTATION:
			{
				if (renderingTransformOrientation_)
				{
					renderingTransformOrientation_->setVisible(false);
				}

				// Not stopping orientationTracker_ here as AS_ROTATION also uses it
				break;
			}

			case AS_ROTATION:
			{
				if (renderingTransformRotation_)
				{
					renderingTransformRotation_->setVisible(false);
				}

				if (orientationTracker_)
				{
					orientationTracker_->stop();
				}
				break;
			}

			case AS_HEADING_AND_GRAVITY:
			{
				if (renderingTransformHeadingAndGravity_)
				{
					renderingTransformHeadingAndGravity_->setVisible(false);
				}

				if (headingTracker_)
				{
					headingTracker_->stop();
				}

				if (gravityTracker_)
				{
					gravityTracker_->stop();
				}
				break;
			}

			case AS_SIZE:
				ocean_assert(false && "This should never happen!");
				break;
		}

		applicationState_ = (applicationState_ + 1u) % AS_SIZE;

		switchApplicationState_ = false;
		reset = true;
	}

	switch (applicationState_)
	{
		case AS_ACCELERATION:
			[self showAcceleration];
			if (renderingTextState_)
			{
				renderingTextState_->setText("Acceleration");
			}
			break;

		case AS_LINEAR_ACCELERATION:
			[self showLinearAcceleration];
			if (renderingTextState_)
			{
				renderingTextState_->setText("Linear Acceleration");
			}
			break;

		case AS_ORIENTATION:
			[self showOrientation];
			if (renderingTextState_)
			{
				renderingTextState_->setText("Orientation");
			}
			break;

		case AS_ROTATION:
			[self showRotationWithReset:reset];
			if (renderingTextState_)
			{
				renderingTextState_->setText("Rotation");
			}
			break;

		case AS_HEADING_AND_GRAVITY:
			[self showHeadingAndGravity];
			if (renderingTextState_)
			{
				renderingTextState_->setText("Heading & Gravity");
			}
			break;

		case AS_SIZE:
			ocean_assert(false && "This should never happen!");
			break;
	}

	if (renderingTextDelay_)
	{
		const double averageDelayMs = sampleCallbackHandler_.averageSampleDelayMs();

		if (averageDelayMs != NumericD::minValue())
		{
			renderingTextDelay_->setText("Delay: " + String::toAString(averageDelayMs, 1u) + " ms");
		}
	}

	[super update];
}

@end
