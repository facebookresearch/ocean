// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_UVTEXTUREMAPPING_CONE_H
#define META_OCEAN_TRACKING_UVTEXTUREMAPPING_CONE_H

#include "ocean/tracking/uvtexturemapping/UVTextureMapping.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/math/Cone3.h"
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
 * Implementation allowing for the conversion of a 2D coordinate in a cone feature map into a point on the associated 3D cone.
 * @ingroup trackinguvtexturemapping
 */
class OCEAN_TRACKING_UVTEXTUREMAPPING_EXPORT ConeUVTextureMapping
{
	public:
		/**
		 * Creates an invalid texture mapping.
		 */
		ConeUVTextureMapping();

		/**
		 * Defines a mapping from the surface of a 3D cone to a 2D UV texture space.
		 * We unwrap the cone by making a straight cut down its side and flattening:
		 * \code
		 *
		 *  ---------------------                ^     .--AC--.      <- larger diameter
		 *  |                   |                |    /        \
		 *  |    _.---B---._    |        segment |   |          |           .__X
		 *  |  /             \  |         height |   '.        .'          /|
		 *  | A     cone      C |                |     \__B___/           Y Z
		 *  |  \   surface   /  |  <=>           |
		 *  |   \           /   |                v       .DF.        <- smaller diameter >= 0
		 *  |    \   .E.   /    |                        \E_/
		 *  |     D./   \.F     |
		 *  |                   |                         O          <- tip of cone
		 *  ---------------------
		 *            O  <- apex of the flattened cone (may be outside of the image)
		 * \endcode
		 * Cuts have been made at A=C and D=F. The user should specify the points C and F,
		 * as well as the larger and smaller diameters and the height of the cone segment.
		 * Note that AC and DF in the left image are arcs of concentric circles with
		 * center O.
		 *
		 * The 3D coordinate frame is located in the center of the cone segment, with
		 * the -Y axis passing through line AD and the +Z axis passing along the
		 * vertical axis of the cone through its tip. So, for a cone segment with
		 * height H, large radius R and small radius r, the 3D coordinates are:
		 * <pre>
		 *   A = C = (0, -R, -H/2)
		 *       B = (0,  R, -H/2)
		 *   D = F = (0, -r,  H/2)
		 *       E = (0,  r,  H/2)
		 * </pre>
		 * (assuming B and E are diametrically opposite A and D).
		 *
		 * Note that the "cone segment height" is the distance between the larger and
		 * smaller circles along the axis of the cone, not the distance along the
		 * surface of the cone.
		 *
		 * If yAxisIntersection is provided by the user, the coordinate frame is
		 * rotated and shifted around the +z axis so that the specified point
		 * corresponds to the intersection of the +y axis with the cone's surface.
		 * Otherwise, the +y intersection is assumed to be in the center of the cone.
		 * Further, if originOnConeSurface is true, the coordinate frame for the cone
		 * will be shifted along the +y axis so that the origin lies on the cone's surface.
		 *
		 * @param height The height of the cone; for example, the vertical distance from a table to the opening of a cup, with range (0, infinity)
		 * @param largerDiameter Diameter of the larger slice of the cone segment, with range (smallerDiameter, infinity); must be strictly larger than the smaller diameter
		 * @param smallerDiameter Diameter of the smaller slice of the cone segment, with range [0, largerDiameter); can be zero, in which case the smaller segment represents the cone tip
		 * @param largerArcEnd 2D coordinate of the ending point for the larger arc (point C in the diagram above)
		 * @param smallerArcEnd 2D coordinate of the ending point for the smaller arc (point F in the diagram above; if smallerDiameter=0, D=F=O)
		 * @param yAxisIntersection If non-negative, the coordinate frame is rotated and shifted around the +z axis so that the specified 2D point in the image corresponds to the intersection of the +y axis with the cone's surface. Otherwise, the +y intersection is assumed to be in the center of the cone.
		 * @param originOnConeSurface If true, the 3D coordinate system will be shifted down the +y axis so that the origin lies on the surface of the cone; otherwise, the origin will lie in the center of the cone
		 */
		ConeUVTextureMapping(const Scalar height, const Scalar largerDiameter, const Scalar smallerDiameter, const Vector2& largerArcEnd, const Vector2& smallerArcEnd, const Vector2& yAxisIntersection = Vector2(Scalar(-1.), Scalar(-1.)), const bool originOnConeSurface = false);

		/**
		 * Returns whether an instantiated mapping is valid. Internally, we check that height > 0.
		 * @return True if the map was created without errors, otherwise false
		 */
		inline bool isValid() const;

		/**
		 * Computes the associated 3D point on a cone for the given 2D point in the space of the cone feature map.
		 * @param point2D UV coordinates of the point in the mapping; note that we allow wrapping around the cone, so no check is performed to ensure that the coordinate lies between the lines AD and CF -- however, we do check that the point is within the vertical span of the cone
		 * @param point3D Output 3D location of the associated point in the established coordinate frame of the cone
		 * @return True if the z-coordinate of the point is in the range (z_min, z_max], false otherwise, where z_min and z_max are the z coordinates for the smaller and larger bases of the cone, respectively
		 */
		bool textureCoordinateTo3DCoordinate(const Vector2& point2D, Vector3& point3D) const;

		/**
		 * Creates a new texture mapping object that corresponds to this object but with the UV coordinates have been rescaled by the given factor.
		 * @param scalingFactor Scaling factor for the 2D coordinates, with range (0, infinity)
		 * @return A new mapping instance
		 */
		inline ConeUVTextureMapping rescale(const Scalar scalingFactor) const;

		/**
		 * Warps an image of a cone into the UV texture space.
		 * @param frame The frame that captures the cone
		 * @param pixelOrigin The pixel origin of the given frame (and the resulting reference frame)
		 * @param pinholeCamera The pinhole camera object that defines the projection and has the same dimension as the given frame
		 * @param poseIF Inverted and flipped camera pose corresponding to the given frame
		 * @param cone The cone defining the 3D object, must be valid
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
		inline void warpImageMaskIF8bitPerChannel(const unsigned char* frame, const FrameType::PixelOrigin pixelOrigin, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& poseIF, const Cone3& cone, unsigned char* reference, unsigned char* referenceMask, const unsigned int referenceWidth, const unsigned int referenceHeight, Worker* worker = nullptr, const unsigned char maskValue = 0xFF, const unsigned int approximationBinSize = 0u) const;

		/**
		 * Returns the distance between the top and bottom bases of the cone along its axis.
		 * @return Height, with range (0, infinity)
		 */
		inline Scalar height() const;

		/**
		 * Returns the y-axis intersection point provided by the user in the constructor.
		 * @return 2D location of the intersection point; (-1, -1) if invalid or not supplied
		 */
		inline const Vector2& yAxisIntersection() const;

		/**
		 * Returns the radius of the larger base for the cone.
		 * @return Larger radius, with range (smallerRadius, infinity)
		 */
		inline Scalar largerRadius() const;

		/**
		 * Returns the radius of the smaller base for the cone.
		 * @return Smaller radius, with range [0, largerRadius)
		 */
		inline Scalar smallerRadius() const;

		/**
		 * Returns the distance of FC in the UV space.
		 * @return Distance of segment FC if the mapping is valid, otherwise 0
		 */
		inline Scalar flattenedConeLength() const;

		/**
		 * Returns the y offset of the cone's 3D coordinates that is induced by the origin being place on the cone's surface, instead of exactly in the center of the cone pattern. If the origin is not placed on the surface, yOffset = 0.
		 * @return The signed y offset with range (-infinity, infinity); add this value to the y-coordinate of points expressed in a coordinate frame exactly in the center of the truncated cone
		 */
		inline Scalar yOffset() const;

		/**
		 * Returns the z offset of the cone's 3D coordinates that is induced by the y-axis intersection not being exactly in the center of the cone pattern. If the y-axis intersection is exactly in the center, zOffset = 0.
		 * @return The signed z offset with range (-infinity, infinity); add this value to the z-coordinate of points expressed in a coordinate frame exactly in the center of the truncated cone
		 */
		inline Scalar zOffset() const;

	private:

		/**
		 * Creates an interpolation lookup table for image warping.
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param poseIF The inverted and flipped camera matrix of the standard camera matrix defining the camera pose
		 * @param cone The cone defining the 3D object, must be valid
		 * @param lookupTable The resulting lookup table, must be defined (size and number of bins) before this function can be called
		 * @tparam tPixelOriginUpperLeft True, if the pixel origin of the frame (and the reference frame) is FrameType::ORIGIN_UPPER_LEFT, False otherwise
		 */
		template <bool tPixelOriginUpperLeft>
		void warpLookupTableIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& poseIF, const Cone3& cone, LookupCorner2<Vector2>& lookupTable) const;

		/// Height of the cone along its axis between its larger and smaller bases, = 0 for an invalid cone.
		Scalar height_;

		/// Radius of the smaller diameter of the cone, with range [0, infinity).
		Scalar smallerRadius_;

		/// Difference between the larger and smaller radii of the cone's bases, with range (0, infinity).
		Scalar radiusDifference_;

		/// 2D location in the feature map of the y-axis intersection point specified by the user.
		Vector2 yAxisIntersection_;

		/// Projected 2D position of the cone's apex, also denoted as point O.
		Vector2 apex2d_;

		/// Distance in pixels between the cone's projected apex and point F in the UV space.
		Scalar lengthOF_;

		/// Inverse distance, in pixels, between points C and F in the UV space.
		Scalar invLengthFC_;

		/// Matrix that rotates vector OC so that it aligns with the x-axis.
		SquareMatrix2 rotationFrameOC_;

		/// Inverse of the 2D angle AOC in the diagram above, in radians.
		Scalar invAngleAOC_;

		/// 3D coordinates of the cone can be expressed as a function of an angle, theta, and the distance, z, along the cone's axis. `theta_min` denotes the minimum value of theta in the UV space, for points along line FC. At the y-axis intersection, theta=pi/2. Points on line AD map to `theta = theta_min + 2 * pi`.
		Scalar thetaMin_;

		/// Y offset of the cone's 3D coordinates. The y-axis intersection point has coordinate (0, -yOffset, -zOffset_) within a coordinate frame centered exactly in the middle of the 3D cone.
		Scalar yOffset_;

		/// Z offset of the cone's 3D coordinates. The y-axis intersection point has coordinate (0, -yOffset, -zOffset_) within a coordinate frame centered exactly in the middle of the 3D cone.
		Scalar zOffset_;
};

inline bool ConeUVTextureMapping::isValid() const
{
	return height_ > Scalar(0.);
}

inline ConeUVTextureMapping ConeUVTextureMapping::rescale(const Scalar scalingFactor) const
{
	ocean_assert(isValid());
	ocean_assert(scalingFactor > Scalar(0));

	if (!isValid() || scalingFactor <= Scalar(0))
	{
		return ConeUVTextureMapping();
	}

	ConeUVTextureMapping newMapping(*this);
	newMapping.apex2d_ *= scalingFactor;
	newMapping.lengthOF_ *= scalingFactor;
	newMapping.invLengthFC_ /= scalingFactor;
	newMapping.yAxisIntersection_ *= scalingFactor;

	return newMapping;
}

template <unsigned int tChannels>
inline void ConeUVTextureMapping::warpImageMaskIF8bitPerChannel(const unsigned char* frame, const FrameType::PixelOrigin pixelOrigin, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& poseIF, const Cone3& cone, unsigned char* reference, unsigned char* referenceMask, const unsigned int referenceWidth, const unsigned int referenceHeight, Worker* worker, const unsigned char maskValue, const unsigned int approximationBinSize) const
{
	ocean_assert(isValid() && pinholeCamera.isValid() && poseIF.isValid() && cone.isValid());
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
		warpLookupTableIF<true>(pinholeCamera, poseIF, cone, lookupTable);
	}
	else
	{
		warpLookupTableIF<false>(pinholeCamera, poseIF, cone, lookupTable);
	}

	constexpr unsigned int framePaddingElements = 0u;
	constexpr unsigned int referencePaddingElements = 0u;
	constexpr unsigned int refernceMaskPaddingElements = 0u;

	CV::FrameInterpolatorBilinear::lookupMask8BitPerChannel<tChannels>(frame, pinholeCamera.width(), pinholeCamera.height(), lookupTable, false, reference, referenceMask, framePaddingElements, referencePaddingElements, refernceMaskPaddingElements, worker, maskValue);
}

inline Scalar ConeUVTextureMapping::height() const
{
	return height_;
}

inline const Vector2& ConeUVTextureMapping::yAxisIntersection() const
{
	return yAxisIntersection_;
}

inline Scalar ConeUVTextureMapping::largerRadius() const
{
	return smallerRadius_ + radiusDifference_;
}

inline Scalar ConeUVTextureMapping::smallerRadius() const
{
	return smallerRadius_;
}

inline Scalar ConeUVTextureMapping::flattenedConeLength() const
{
	ocean_assert(isValid());
	return isValid() ? Scalar(1.) / invLengthFC_ : Scalar(0.);
}

inline Scalar ConeUVTextureMapping::yOffset() const
{
	return yOffset_;
}

inline Scalar ConeUVTextureMapping::zOffset() const
{
	return zOffset_;
}

template <bool tPixelOriginUpperLeft>
void ConeUVTextureMapping::warpLookupTableIF(const PinholeCamera& pinholeCamera, const HomogenousMatrix4& poseIF, const Cone3& cone, LookupCorner2<Vector2>& lookupTable) const
{
	ocean_assert(pinholeCamera.isValid() && poseIF.isValid() && cone.isValid());

	const bool hasDistortionParameters = pinholeCamera.hasDistortionParameters();

	const Scalar frameHeight1 = Scalar(pinholeCamera.height() - 1u);

	const Line3 coneAxis(cone.apex(), cone.axis());
	const HomogenousMatrix4 pose(PinholeCamera::invertedFlipped2Standard(poseIF));

	for (unsigned int yBin = 0u; yBin <= lookupTable.binsY(); ++yBin)
	{
		const Scalar y = lookupTable.binTopLeftCornerPositionY(yBin);
		const Scalar yCorrected = tPixelOriginUpperLeft ? static_cast<Scalar>(y) : (static_cast<Scalar>(lookupTable.sizeY() - 1) - static_cast<Scalar>(y));

		for (unsigned int xBin = 0u; xBin <= lookupTable.binsX(); ++xBin)
		{
			const Scalar x = lookupTable.binTopLeftCornerPositionX(xBin);

			Vector3 objectPoint;
			if (textureCoordinateTo3DCoordinate(Vector2(x, yCorrected), objectPoint))
			{
				const Vector3 pointOnAxis = coneAxis.nearestPoint(objectPoint);
				const Vector3 pointDirection = objectPoint - pointOnAxis;

				const Vector3 viewingRay = objectPoint - pose.translation();

				if (viewingRay * pointDirection < Numeric::eps())
				{
					const Vector2 imagePoint(pinholeCamera.projectToImageIF<true>(poseIF, objectPoint, hasDistortionParameters));
					const Vector2 correctedImagePoint(imagePoint.x(), tPixelOriginUpperLeft ? imagePoint.y() : (frameHeight1 - imagePoint.y()));
					lookupTable.setBinTopLeftCornerValue(xBin, yBin, correctedImagePoint);
				}
				else
				{
					const Vector2 dummyPointOutsideOfImage(Scalar(10u * pinholeCamera.width()), Scalar(10u * pinholeCamera.height()));
					lookupTable.setBinTopLeftCornerValue(xBin, yBin, dummyPointOutsideOfImage);
				}
			}
			else
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

#endif // META_OCEAN_TRACKING_UVTEXTUREMAPPING_CONE_H
