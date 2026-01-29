/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/base/Lock.h"

#include "ocean/devices/AccelerationSensor3DOF.h"
#include "ocean/devices/GravityTracker3DOF.h"
#include "ocean/devices/Measurement.h"
#include "ocean/devices/OrientationTracker3DOF.h"

#include "ocean/platform/android/application/GLRendererView.h"

#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Text.h"
#include "ocean/rendering/Transform.h"

#include <atomic>
#include <deque>

namespace Ocean
{

/**
 * This class implements the main view of the Sensors demo application for Android.
 * The application visualizes sensor data from the device's built-in sensors.
 * @ingroup applicationdemodevicessensors
 */
class SensorsView : public Platform::Android::Application::GLRendererView
{
	friend class Singleton<SensorsView>;

	protected:

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

		/// The number of delay samples to average.
		static constexpr size_t numberDelaySamples_ = 10;

	public:

		/**
		 * Initializes the view.
		 * @see GLView::initialize().
		 */
		bool initialize() override;

		/**
		 * Releases the view.
		 * @see GLView::release().
		 */
		bool release() override;

		/**
		 * Renders the next frame.
		 * @see GLView::render().
		 */
		bool render() override;

		/**
		 * Shows the acceleration sensor data.
		 * @return True, if succeeded
		 */
		bool showAcceleration();

		/**
		 * Shows the linear acceleration sensor data.
		 * @return True, if succeeded
		 */
		bool showLinearAcceleration();

		/**
		 * Shows the acceleration visualization with the given values.
		 * @param acceleration The acceleration to show
		 * @param scaledAcceleration The scaled acceleration for visualization
		 */
		void showAcceleration(const Vector3& acceleration, const Vector3& scaledAcceleration);

		/**
		 * Shows the orientation tracker data.
		 * @return True, if succeeded
		 */
		bool showOrientation();

		/**
		 * Shows the rotation data.
		 * @param reset True, to reset the rotation reference
		 * @return True, if succeeded
		 */
		bool showRotation(const bool reset);

		/**
		 * Shows the heading and gravity data.
		 * @return True, if succeeded
		 */
		bool showHeadingAndGravity();

		/**
		 * Touch up event function.
		 * @see GLView::onTouchUp().
		 */
		void onTouchUp(const float x, const float y) override;

		/**
		 * Creates an instance of this view.
		 * @return The new instance
		 */
		static inline Platform::Android::Application::GLView* createInstance();

	protected:

		/**
		 * Creates a new sensors view.
		 */
		SensorsView();

		/**
		 * Destructs the sensors view.
		 */
		~SensorsView() override;

		/**
		 * Event callback for new measurement samples.
		 * This function calculates the sample delay and updates the running average.
		 * @param measurement The measurement object sending the sample
		 * @param sample The new sample
		 */
		void onSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample);

	protected:

		/// The current application state.
		ApplicationState applicationState_ = AS_ACCELERATION;

		/// True, if the application state should be switched.
		std::atomic_bool switchApplicationState_ = false;

		/// The 3DOF acceleration sensor.
		Devices::AccelerationSensor3DOFRef accelerationSensor_;

		/// The 3DOF linear acceleration sensor.
		Devices::AccelerationSensor3DOFRef linearAccelerationSensor_;

		/// The 3DOF orientation tracker.
		Devices::OrientationTracker3DOFRef orientationTracker_;

		/// The 3DOF heading tracker.
		Devices::OrientationTracker3DOFRef headingTracker_;

		/// The 3DOF gravity tracker.
		Devices::GravityTracker3DOFRef gravityTracker_;

		/// The sample event subscription for the acceleration sensor.
		Devices::Measurement::SampleEventSubscription accelerationSensorSubscription_;

		/// The sample event subscription for the linear acceleration sensor.
		Devices::Measurement::SampleEventSubscription linearAccelerationSensorSubscription_;

		/// The sample event subscription for the orientation tracker.
		Devices::Measurement::SampleEventSubscription orientationTrackerSubscription_;

		/// The sample event subscription for the heading tracker.
		Devices::Measurement::SampleEventSubscription headingTrackerSubscription_;

		/// The sample event subscription for the gravity tracker.
		Devices::Measurement::SampleEventSubscription gravityTrackerSubscription_;

		/// The rendering scene.
		Rendering::SceneRef scene_;

		/// The text element showing the current application state.
		Rendering::TextRef renderingTextState_;

		/// The text element showing the sample delay.
		Rendering::TextRef renderingTextDelay_;

		/// The lock for the delay samples.
		mutable Lock delayLock_;

		/// The recent sample delay measurements in milliseconds for averaging.
		std::deque<double> recentSampleDelaysMs_;

		/// The running sum of the delay samples.
		double delaySumMs_ = 0.0;

		/// The averaged sample delay in milliseconds.
		std::atomic<double> averageSampleDelayMs_ = NumericD::minValue();

		/// The transform for the acceleration visualization.
		Rendering::TransformRef renderingTransformAcceleration_;

		/// The transform for the X-axis acceleration visualization.
		Rendering::TransformRef renderingTransformAccelerationX_;

		/// The transform for the Y-axis acceleration visualization.
		Rendering::TransformRef renderingTransformAccelerationY_;

		/// The transform for the Z-axis acceleration visualization.
		Rendering::TransformRef renderingTransformAccelerationZ_;

		/// The text element showing the acceleration values.
		Rendering::TextRef renderingTextAcceleration_;

		/// The transform for the orientation visualization.
		Rendering::TransformRef renderingTransformOrientation_;

		/// The transform for the rotation visualization.
		Rendering::TransformRef renderingTransformRotation_;

		/// The text element showing the rotation values.
		Rendering::TextRef renderingTextRotation_;

		/// The transform for the heading and gravity visualization.
		Rendering::TransformRef renderingTransformHeadingAndGravity_;

		/// The transform for the heading visualization.
		Rendering::TransformRef renderingTransformHeading_;

		/// The transform for the gravity visualization.
		Rendering::TransformRef renderingTransformGravity_;

		/// True, if the instance function is registered.
		static const bool instanceRegistered_;
};

inline Platform::Android::Application::GLView* SensorsView::createInstance()
{
	return new SensorsView();
}

}
