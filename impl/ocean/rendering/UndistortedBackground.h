/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_UNDISTORTED_BACKGROUND_H
#define META_OCEAN_RENDERING_UNDISTORTED_BACKGROUND_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Background.h"
#include "ocean/rendering/DynamicObject.h"
#include "ocean/rendering/MediaTexture2D.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class UndistortedBackground;

/**
 * Definition of a smart object reference holding an undistorted background node.
 * @see SmartObjectRef, UndistortedBackground.
 * @ingroup rendering
 */
typedef SmartObjectRef<UndistortedBackground> UndistortedBackgroundRef;

/**
 * This class is the base class for all undistorted backgrounds.<br>
 * The undistorted background uses a frame medium object as background image<br>
 * and uses corresponding camera calibration parameters for correct visualization.<br>
 * @see FrameMedium, PinholeCamera, CameraCalibrationManager
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT UndistortedBackground :
	virtual public Background,
	virtual public DynamicObject
{
	public:

		/**
		 * Definition of different distortion display types.
		 */
		enum DisplayType
		{
			/// Fastest display type.
			DT_FASTEST,
			/// Undistorted display type.
			DT_UNDISTORTED
		};

	public:

		/**
		 * Returns the image source used as background image.
		 * @return Frame medium background object
		 */
		virtual const Media::FrameMediumRef& medium() const;

		/**
		 * Returns the camera object related to the background image.
		 * @return Current camera object
		 */
		virtual const PinholeCamera& camera() const;

		/**
		 * Returns the display type of this background object.
		 * Default is DT_FASTEST.
		 * @return Distortion display type
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual DisplayType displayType() const;

		/**
		 * Sets the distance between viewer and background.
		 * @see Background::setDistance().
		 */
		bool setDistance(const Scalar distance) override;

		/**
		 * Sets the image source for the undistorted background.
		 * @param medium Frame medium providing the background image
		 */
		virtual void setMedium(const Media::FrameMediumRef& medium);

		/**
		 * Sets the display type of this background object.
		 * @param type Distortion display type to be set
		 * @return True, if succceeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setDisplayType(const DisplayType type);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates an undistorted background object.
		 */
		UndistortedBackground();

		/**
		 * Destructs an undistorted background object.
		 */
		~UndistortedBackground() override;

		/**
		 * Update function called by the framebuffer.
		 * @see DynamicObject::onDynamicUpdate().
		 */
		void onDynamicUpdate(const ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Event function if the camera of the used medium has changed.
		 * @param timestamp Event timestamp
		 */
		virtual void onMediumCameraChanged(const Timestamp timestamp) = 0;

	protected:

		/// Frame medium object providing the image data for the background.
		Media::FrameMediumRef medium_;

		/// The camera profile of the medium for the last update timestamp.
		PinholeCamera mediumCamera_;

		/// True, if the camera has changed since the last update.
		bool cameraChanged_ = false;

		/// 2D texture object holding the background image.
		MediaTexture2DRef texture_;
};

}

}

#endif // META_OCEAN_RENDERING_UNDISTORTED_BACKGROUND_H
