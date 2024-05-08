/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_UVTEXTUREMAPPING_CYLINDER_H
#define META_OCEAN_TRACKING_UVTEXTUREMAPPING_CYLINDER_H

#include "ocean/tracking/uvtexturemapping/UVTextureMapping.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/math/Cylinder3.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Lookup2.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace Tracking
{

namespace UVTextureMapping
{

/**
 * Implementation allowing for the conversion of a 2D coordinate in a cylinder feature map into a point on the associated 3D cylinder.
 * @ingroup trackinguvtexturemapping
 */
class OCEAN_TRACKING_UVTEXTUREMAPPING_EXPORT CylinderUVTextureMapping
{
	public:

		/**
		 * Creates an invalid texture mapping.
		 */
		CylinderUVTextureMapping() = default;

		/**
		 * Defines a mapping from the surface of a 3D cylinder to a 2D UV texture space.
		 * We unwrap the cylinder by making a straight cut down its side and flattening:
		 * \code
		 *          wrap-around for
		 *        /  edge features  \
		 *  <-  ->                   <-  ->
		 *        <- pattern width ->
		 *  ------A--------B--------C------                   __AC___
		 *  |     |                 |     |                 /        \
		 *  |     |                 |     |            ^   |          |
		 *  |     |                 |     |            |   \          /
		 *  |     |     cylinder    |     |            |   |\___B___/ |
		 *  |     |     surface     |     | <=> height |   |          |      .__X
		 *  |     |                 |     |            |   |  __DF___ |     /|
		 *  |     |                 |     |            |   |/        \|    Y Z
		 *  |     |                 |     |            |   |          |
		 *  |     |                 |     |            V   \          /
		 *  ------D--------E--------F------                 \___E___/
		 *        ^
		 *        pattern x-axis offset
		 *
		 * \endcode
		 * Cuts have been made at A=C and D=F. Since features on the edge require context
		 * across the cut, the pattern may be repeated on the left and right.
		 *
		 * The 3D coordinate frame is located in the center of the cylinder, with
		 * the -Y axis passing through line AD and the +Z axis passing along the
		 * vertical axis of the cylinder.
		 *
		 * If patternYAxisIntersection is provided by the user, the coordinate frame is
		 * rotated and shifted around the +z axis so that the specified point
		 * corresponds to the intersection of the +y axis with the cylinder's surface.
		 * Otherwise, the +y intersection is assumed to be in the center of the pattern.
		 * Further, if originOnCylinderSurface is true, the coordinate frame for the cylinder
		 * will be shifted along the +y axis so that the origin lies on the cylinder's surface.
		 *
		 * @param patternWidth Width of the non-repeated region of the texture (see figure) in, e.g., pixels, with range (0, infinity)
		 * @param patternHeight Height of the texture in, e.g., pixels, with range (0, infinity)
		 * @param metricHeight Height of the cylinder in, e.g., meters, with range (0, infinity)
		 * @param patternXAxisOffset For patterns that have repeat, the amount of padding to the left of the cylinder surface region (see figure)
		 * @param patternYAxisIntersection If non-negative, the coordinate frame is rotated and shifted around the +z axis so that the specified 2D point in the image corresponds to the intersection of the +y axis with the cylinder's surface. Otherwise, the +y intersection is assumed to be in the center of the pattern.
		 * @param originOnCylinderSurface If true, the 3D coordinate system will be shifted down the +y axis so that the origin lies on the surface of the cylinder; otherwise, the origin will lie in the center of the cylinder
		 */
		CylinderUVTextureMapping(const Scalar patternWidth, const Scalar patternHeight, const Scalar metricHeight = Scalar(1), const Scalar patternXAxisOffset = Scalar(0), const Vector2& patternYAxisIntersection = Vector2(-1, -1), const bool originOnCylinderSurface = false);

		/**
		 * Returns whether an instantiated mapping is valid. Internally, we check that height > 0
		 * @return True if the map was created without errors, otherwise false
		 */
		inline bool isValid() const;

		/**
		 * Computes the associated 3D point on a cylinder for the given 2D point in the space of the cylinder feature map.
		 * @param point2D UV coordinates of the point in the mapping; note that we allow wrapping around the cylinder, so no check is performed to ensure that the coordinate lies between the left and right boundaries of the patter -- however, we do check that the point is within the vertical span of the cylinder
		 * @param point3D Output 3D location of the associated point in the established coordinate frame of the cylinder
		 * @return True if the z-coordinate of the point is in the range [z_min, z_max], false otherwise, where z_min and z_max are the z coordinates for two bases of the cylinder
		 */
		bool textureCoordinateTo3DCoordinate(const Vector2& point2D, Vector3& point3D) const;

		/**
		 * Creates a new texture mapping object that corresponds to this object but with the UV coordinates have been rescaled by the given factor.
		 * @param scalingFactor Scaling factor for the 2D coordinates, with range (0, infinity)
		 * @return A new mapping instance
		 */
		inline CylinderUVTextureMapping rescale(const Scalar scalingFactor) const;

		/**
		 * Warps an image of a cylinder into the UV texture space.
		 * @param frame The frame that captures the cylinder
		 * @param pixelOrigin The pixel origin of the given frame (and the resulting reference frame)
		 * @param pinholeCamera The pinhole camera object that defines the projection and has the same dimension as the given frame
		 * @param poseIF Inverted and flipped camera pose corresponding to the given frame
		 * @param cylinder The cylinder defining the 3D object, must be valid
		 * @param reference Resulting reference image
		 * @param referenceMask Resulting reference mask
		 * @param referenceWidth Width of the reference image in pixel, with range [1, infinity)
		 * @param referenceHeight Height of the reference image in pixel, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param maskValue 8 bit mask values for reference pixels lying inside the given camera frame, reference pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @param approximationBinSize Width (and height) of a bin in a lookup table to speedup the in interpolation in pixel; note that this is currently a *required* parameter and must be in the range [1u, infinity)
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		inline void warpImageMaskIF8bitPerChannel(const unsigned char* frame, const FrameType::PixelOrigin pixelOrigin, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& poseIF, const Cylinder3& cylinder, unsigned char* reference, unsigned char* referenceMask, const unsigned int referenceWidth, const unsigned int referenceHeight, Worker* worker = nullptr, const unsigned char maskValue = 0xFF, const unsigned int approximationBinSize = 0u) const;

		/**
		 * Returns the distance between the top and bottom bases of the cylinder along its axis.
		 * @return Height, with range (0, infinity)
		 */
		inline Scalar height() const;

		/**
		 * Returns the y-axis intersection point provided by the user in the constructor.
		 * @return 2D location of the intersection point; (-1, -1) if invalid or not supplied
		 */
		inline const Vector2& yAxisIntersection() const;

		/**
		 * Returns the radius of the larger base for the cylinder.
		 * @return Larger radius, with range (smallerRadius, infinity)
		 */
		inline Scalar radius() const;

		/**
		 * Returns the y offset of the cylinder's 3D coordinates that is induced by the origin being place on the cylinder's surface, instead of exactly in the center of the cylinder pattern. If the origin is not placed on the surface, yOffset = 0.
		 * @return The signed y offset with range (-infinity, infinity); add this value to the y-coordinate of points expressed in a coordinate frame exactly in the center of the truncated cylinder
		 */
		inline Scalar yOffset() const;

		/**
		 * Returns the z offset of the cylinder's 3D coordinates that is induced by the y-axis intersection not being exactly in the center of the cylinder pattern. If the y-axis intersection is exactly in the center, zOffset = 0.
		 * @return The signed z offset with range (-infinity, infinity); add this value to the z-coordinate of points expressed in a coordinate frame exactly in the center of the truncated cylinder
		 */
		inline Scalar zOffset() const;

	private:

		/**
		 * Creates an interpolation lookup table for image warping.
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param poseIF The inverted and flipped camera matrix of the standard camera matrix defining the camera pose
		 * @param cylinder The cylinder defining the 3D object, must be valid
		 * @param lookupTable The resulting lookup table, must be defined (size and number of bins) before this function can be called
		 * @tparam tPixelOriginUpperLeft True, if the pixel origin of the frame (and the reference frame) is FrameType::ORIGIN_UPPER_LEFT, False otherwise
		 */
		template <bool tPixelOriginUpperLeft>
		void warpLookupTableIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& poseIF, const Cylinder3& cylinder, LookupCorner2<Vector2>& lookupTable) const;

	private:

		/// Width of the 2D pattern, e.g. in pixels; range = (0, infinity).
		Scalar patternWidth_ = Scalar(0);

		/// Height of the 2D pattern, e.g. in pixels; range = (0, infinity).
		Scalar patternHeight_ = Scalar(0);

		/// Height of the cylinder along its axis, = 0 for an invalid cylinder.
		Scalar height_ = Scalar(0);

		/// Radius of the cylinder, with range (0, infinity).
		Scalar radius_ = Scalar(0);

		/// X-coordinate at which the trackable region of the pattern actually begins (for patterns
		/// that have padded wrap-around); range = [0, infinity).
		Scalar patternXAxisOffset_ = Scalar(0);

		/// 2D location in the feature map of the y-axis intersection point specified by the user.
		Vector2 patternYAxisIntersection_ = Vector2(-1, -1);

		/// The flattened pattern horizontally spans 2*pi degrees, with the y-axis intersection at
		/// theta = pi/2. Greater values of theta are to the left of this point, and smaller values
		/// are to the right. `thetaMin_` is the rightmost value of theta for the pattern; it will
		/// lie in the range [-3pi/2,pi/2] when the y-axis point is on the left and right edges,
		/// respectively.
		Scalar thetaMin_ = -Numeric::pi_2();

		/// Y offset of the cylinder's 3D coordinates. The y-axis intersection point has coordinate (0, -yOffset, -zOffset_) within a coordinate frame centered exactly in the middle of the 3D cylinder.
		Scalar yOffset_ = Scalar(0);

		/// Z offset of the cylinder's 3D coordinates. The y-axis intersection point has coordinate (0, -yOffset, -zOffset_) within a coordinate frame centered exactly in the middle of the 3D cylinder.
		Scalar zOffset_ = Scalar(0);
};

inline bool CylinderUVTextureMapping::isValid() const
{
	return height_ > Scalar(0);
}

inline CylinderUVTextureMapping CylinderUVTextureMapping::rescale(const Scalar scalingFactor) const
{
	ocean_assert(isValid());
	ocean_assert(scalingFactor > Scalar(0));

	if (!isValid() || scalingFactor <= Scalar(0))
	{
		return CylinderUVTextureMapping();
	}

	CylinderUVTextureMapping newMapping(*this);
	newMapping.patternWidth_ *= scalingFactor;
	newMapping.patternHeight_ *= scalingFactor;
	newMapping.patternXAxisOffset_ *= scalingFactor;
	newMapping.patternYAxisIntersection_ *= scalingFactor;

	return newMapping;
}

template <unsigned int tChannels>
inline void CylinderUVTextureMapping::warpImageMaskIF8bitPerChannel(const unsigned char* frame, const FrameType::PixelOrigin pixelOrigin, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& poseIF, const Cylinder3& cylinder, unsigned char* reference, unsigned char* referenceMask, const unsigned int referenceWidth, const unsigned int referenceHeight, Worker* worker, const unsigned char maskValue, const unsigned int approximationBinSize) const
{
	ocean_assert(isValid() && pinholeCamera.isValid() && poseIF.isValid() && cylinder.isValid());
	ocean_assert(approximationBinSize > 0u);

	if (!isValid() || approximationBinSize == 0u)
	{
		return;
	}

	const unsigned int binsX = minmax(1u, referenceWidth / approximationBinSize, referenceWidth / 4u);
	const unsigned int binsY = minmax(1u, referenceHeight / approximationBinSize, referenceHeight / 4u);
	LookupCorner2<Vector2> lookupTable(referenceWidth, referenceHeight, binsX, binsY);

	if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
	{
		warpLookupTableIF<true>(pinholeCamera, poseIF, cylinder, lookupTable);
	}
	else
	{
		warpLookupTableIF<false>(pinholeCamera, poseIF, cylinder, lookupTable);
	}

	constexpr unsigned int framePaddingElements = 0u;
	constexpr unsigned int referencePaddingElements = 0u;
	constexpr unsigned int refernceMaskPaddingElements = 0u;

	CV::FrameInterpolatorBilinear::lookupMask8BitPerChannel<tChannels>(frame, pinholeCamera.width(), pinholeCamera.height(), lookupTable, false, reference, referenceMask, framePaddingElements, referencePaddingElements, refernceMaskPaddingElements, worker, maskValue);
}

inline Scalar CylinderUVTextureMapping::height() const
{
	return height_;
}

inline const Vector2& CylinderUVTextureMapping::yAxisIntersection() const
{
	return patternYAxisIntersection_;
}

inline Scalar CylinderUVTextureMapping::radius() const
{
	return radius_;
}

inline Scalar CylinderUVTextureMapping::yOffset() const
{
	return yOffset_;
}

inline Scalar CylinderUVTextureMapping::zOffset() const
{
	return zOffset_;
}

template <bool tPixelOriginUpperLeft>
void CylinderUVTextureMapping::warpLookupTableIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& poseIF, const Cylinder3& cylinder, LookupCorner2<Vector2>& lookupTable) const
{
	ocean_assert(pinholeCamera.isValid() && poseIF.isValid() && cylinder.isValid());

	const bool hasDistortionParameters = pinholeCamera.hasDistortionParameters();

	const Scalar frameHeight1 = Scalar(pinholeCamera.height() - 1u);

	const Line3 cylinderAxis(cylinder.origin(), cylinder.axis());
	const HomogenousMatrix4 pose(PinholeCamera::invertedFlipped2Standard(poseIF));

	for (unsigned int yBin = 0u; yBin <= lookupTable.binsY(); ++yBin)
	{
		const Scalar y = lookupTable.binTopLeftCornerPositionY(yBin);
		const Scalar yCorrected = tPixelOriginUpperLeft ? static_cast<Scalar>(y) : (static_cast<Scalar>(lookupTable.sizeY() - 1) - static_cast<Scalar>(y));

		for (unsigned int xBin = 0u; xBin <= lookupTable.binsX(); ++xBin)
		{
			const Scalar x = lookupTable.binTopLeftCornerPositionX(xBin);

			bool setDummy = true;
			Vector3 objectPoint;
			if (textureCoordinateTo3DCoordinate(Vector2(x, yCorrected), objectPoint))
			{
				const Vector3 pointOnAxis = cylinderAxis.nearestPoint(objectPoint);
				const Vector3 pointDirection = objectPoint - pointOnAxis;

				const Vector3 viewingRay = objectPoint - pose.translation();

				if (viewingRay * pointDirection < Numeric::eps())
				{
					const Vector2 imagePoint(pinholeCamera.projectToImageIF<true>(poseIF, objectPoint, hasDistortionParameters));
					const Vector2 correctedImagePoint(imagePoint.x(), tPixelOriginUpperLeft ? imagePoint.y() : (frameHeight1 - imagePoint.y()));
					lookupTable.setBinTopLeftCornerValue(xBin, yBin, correctedImagePoint);
					setDummy = false;
				}
			}

			if (setDummy)
			{
				const Vector2 dummyPointOutsideOfImage(Scalar(10u * pinholeCamera.width()), Scalar(10u * pinholeCamera.height()));
				lookupTable.setBinTopLeftCornerValue(xBin, yBin, dummyPointOutsideOfImage);
			}
		}
	}
}

} // namespace UVTextureMapping

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TRACKING_UVTEXTUREMAPPING_CYLINDER_H
