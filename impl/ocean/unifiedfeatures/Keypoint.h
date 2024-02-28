// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"

#include "ocean/math/SquareMatrix2.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace UnifiedFeatures
{

/// Forward declaration
class Keypoint;

/**
 * Definition of a vector of keypoints
 * @ingroup unifiedfeatures
 */
typedef std::vector<Keypoint> Keypoints;

/**
 * Definition of a keypoint class
 * @ingroup unifiedfeatures
 */
class Keypoint
{
	public:

		/**
		 * Default constructor
		 */
		inline Keypoint();

		/**
		 * Constructor
		 * @param observation The location in pixels where this keypoint has been detected
		 * @param strength The strength of this keypoint, range: [0, infinity)
		 * @param orientation The orientation of this keypoint in radian, range: [0, 2*PI)
		 * @param octave The octave in a image pyramid in which this keypoint has been detected, range: [0, infinity)
		 * @param scale The scale factor (e.g. inside an image pyramid) at which a keypoint has been detected, range: (0, infinity)
		 * @param affineTransformImageFromCanonical The affine shape of the keypoint as a transform from the canonical frame (a unit circle) into the image at its original resolution, if computed by the detector
		 */
		inline Keypoint(const VectorF2& observation, const float strength = -1.0f, const float orientation = -1.0f, const unsigned int octave = 0u, const float scale = -1.0f, std::shared_ptr<SquareMatrixF2> affineTransformImageFromCanonical = nullptr);

		/**
		 * Returns the observation of the keypoint
		 * @return The observation
		 */
		inline const VectorF2& observation() const;

		/**
		 * Returns the strength of the this keypoint
		 * @return The strength value
		 */
		inline float strength() const;

		/**
		 * Returns the orientation of this keypoint
		 * @return The orientation value
		 */
		inline float orientation() const;

		/**
		 * Returns the octave of this keypoint
		 * @return The value of the octave
		 */
		inline unsigned int octave() const;

		/**
		 * Returns the scale of this keypoint
		 * @return The scale value
		 */
		inline float scale() const;

		/**
		 * Returns the affine shape of the keypoint as a transform from the canonical frame (e.g., a unit circle) into the image at its original resolution, if computed by the detector
		 * @return The affine shape, which may be std::nullopt if the detector did not compute it
		 */
		inline std::shared_ptr<const SquareMatrixF2> affineTransformImageFromCanonical() const;

		/**
		 * Returns whether this instance has a strength value (in the allowed range)
		 * @return True, if so
		 */
		inline bool hasStrength() const;

		/**
		 * Returns whether this instance has a strength value (in the allowed range)
		 * @return True, if so
		 */
		inline bool hasOrientation() const;

		/**
		 * Returns whether this instance has a strength value (in the allowed range)
		 * @return True, if so
		 */
		inline bool hasScale() const;

	protected:

		/// The observation of the keypoint
		VectorF2 observation_;

		/// The strength of the this keypoint, range: [0, infinity)
		float strength_;

		/// The orientation of this keypoint in radian, range [0, 2*PI)
		float orientation_;

		/// The octave of this keypoint, range: [0, infinity)
		unsigned int octave_;

		/// The scale of this keypoint, range: (0, infinity)
		float scale_;

		/// Affine shape of the keypoint as a transform from the canonical frame (a unit circle) into the image at its original resolution, if computed by the detector
		std::shared_ptr<SquareMatrixF2> affineTransformImageFromCanonical_;
};

Keypoint::Keypoint() :
	Keypoint(/* observation */ VectorF2(0, 0))
{
	// Nothing else to do.
}

inline Keypoint::Keypoint(const VectorF2& observation, const float strength, const float orientation, const unsigned int octave, const float scale, std::shared_ptr<SquareMatrixF2> affineTransformImageFromCanonical) :
	observation_(observation),
	strength_(strength),
	orientation_(orientation),
	octave_(octave),
	scale_(scale),
	affineTransformImageFromCanonical_(affineTransformImageFromCanonical)
{
	// Nothing else to do
}

inline const VectorF2& Keypoint::observation() const
{
	return observation_;
}

inline float Keypoint::strength() const
{
	return strength_;
}

inline float Keypoint::orientation() const
{
	return orientation_;
}

inline unsigned int Keypoint::octave() const
{
	ocean_assert(octave_ < (unsigned int)(-1));
	return octave_;
}

inline float Keypoint::scale() const
{
	return scale_;
}

inline std::shared_ptr<const SquareMatrixF2> Keypoint::affineTransformImageFromCanonical() const
{
	return std::shared_ptr<const SquareMatrixF2>(affineTransformImageFromCanonical_);
}

inline bool Keypoint::hasStrength() const
{
	return strength_ >= 0.0f;
}

inline bool Keypoint::hasOrientation() const
{
	return orientation_ >= 0.0f && orientation_ < NumericF::pi2();
}

inline bool Keypoint::hasScale() const
{
	return scale_ > 0.0f;
}

} // namespace UnifiedFeatures

} // namespace Ocean
