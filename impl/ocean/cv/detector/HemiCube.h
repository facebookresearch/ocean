/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_HEMI_CUBE_H
#define META_OCEAN_CV_DETECTOR_HEMI_CUBE_H

#include "ocean/cv/detector/Detector.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/PixelPosition.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * Data structure used for efficient grouping to 2D line segments
 * This data structure is inspired by and derived from the HemiCube of Rick Szeliski and Daniel Scharstein.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT HemiCube
{
	public:

		/// The location of a line in the cube map is defined as the triple (binX, binY, faceIndex), faceIndex = {0, 1, 2}
		typedef VectorT3<unsigned int> MapIndex;

		/**
		 * Helper to compute hash values for map indices
		 */
		struct MapIndexHash
		{
			/**
			 * Compute hash value for a map index
			 * @param mapIndex The map index to be hashed
			 * @return Hash value of the input map index
			 */
			size_t operator()(const MapIndex& mapIndex) const noexcept;
		};

		/// The actual cube map: maps a line to a bin (set of line indices)
		typedef std::unordered_map<MapIndex, IndexSet32, MapIndexHash> Map;

	public:

		/**
		 * Default constructor
		 */
		HemiCube() = default;

		/**
		 * Constructor
		 * @param bins Number of bins along one dimension (cube), range: [1, infinity) (suggested range: [1, 20])
		 * @param imageWidth Width of the image in which the above lines were found, range: [1, infinity)
		 * @param imageHeight Height of the image in which the above lines were found, range: [1, infinity)
		 * @param focalLength Focal length of the original camera; it's used to convert 2D image points into 3D rays; if unknown use something like 1.0; range: (0, infinity)
		 */
		HemiCube(const unsigned int bins, const unsigned int imageWidth, const unsigned int imageHeight, const Scalar focalLength);

		/**
		 * Check if the Hemi cube is fully initialized
		 * @return True if the Hemi cube has been fully initialized, otherwise false
		 */
		inline bool isValid() const;

		/**
		 * Add a line to the Hemi cube
		 * The line will be added as-is, it will not be merged with similar lines in the Hemi cube
		 * @note There are no checks for duplicate lines.
		 * @param newLine Line to be added to the map, line must be valid
		 */
		inline void insert(const FiniteLine2& newLine);

		/**
		 * Add multiple lines to the Hemi cube
		 * The lines will be added as-is, they will not be merged with similar lines in the Hemi cube
		 * The are no checks for duplicate lines.
		 * @param lines Lines to be added to the map, must be valid
		 */
		inline void insert(const FiniteLines2& lines);

		/**
		 * Merge a similar lines in set of line segments (brute-force search/no use of Hemi cubes, slow)
		 * Lines that cannot be merged will be added to the output as-is.
		 * @param lines Lines to be merged
		 * @param maxLineDistance Maximum allowed distance of the end-points of one line segment to infinite line of another line segment in order to be considered as collinear, range: [0, infinity)
		 * @param maxLineGap Maximum allowed distance between a pair of collinear line segments in order to be considered mergeable, range: [0, infinity)
		 * @param mapping If specified, this will be the mapping of indices of the lines in parameter `lines` to lines in the return value (i.e. input line i merged into output line j), identical size as parameter `lines`, will be ignored if set to `nullptr`
		 * @param cosAngle Cosine of the maximum angle that is allowed in order for the two line segments to be considered parallel; default: cos(weakEps()), i.e., approx. one, range: [0, 1]
		 * @return The merged lines
		 */
		static FiniteLines2 mergeGreedyBruteForce(const FiniteLines2& lines, const Scalar maxLineDistance, const Scalar maxLineGap, Indices32* mapping = nullptr, const Scalar cosAngle = Numeric::cos(Numeric::weakEps()));

		/**
		 * Merge a similar lines in set of line segments
		 * Lines will be added to a Hemi cube internally. For each new input line, retrieve similar lines from the Hemi cube and try to merge it. Then update the Hemi cube.
		 * Lines that cannot be merged will be added to the output as-is.
		 * @param lines Lines to be merged
		 * @param maxLineDistance Maximum allowed distance of the end-points of one line segment to infinite line of another line segment in order to be considered as collinear, range: [0, infinity)
		 * @param maxLineGap Maximum allowed distance between a pair of collinear line segments in order to be considered mergeable
		 * @param mapping If specified, this will be the mapping of indices of the lines in parameter `lines` to lines in the return value (i.e. input line i merged into output line j), identical size as parameter `lines`, will be ignored if set to `nullptr`
		 * @param cosAngle Cosine of the maximum angle that is allowed in order for the two line segments to be considered parallel; default: cos(weakEps()), i.e., approx. one, range: [0, 1]
		 */
		void merge(const FiniteLines2& lines, const Scalar maxLineDistance, const Scalar maxLineGap, Indices32* mapping = nullptr, const Scalar cosAngle = Numeric::cos(Numeric::weakEps()));

		/**
		 * Compute line segment that minimizes the distances to the endpoints of the input line segments
		 * Computes the infinite line that minimizes the weighted distances to the endpoints of the input line segments, `line0` and `line1`.
		 * The weights are computed as `w_i = len(line_i) / (len(line0) + len(line1))`
		 * The endpoints of the input lines are then projected on the infinite line to generate a new line segments such that the resulting length is maximized.
		 * @param line0 First line segment
		 * @param line1 Second line segment
		 * @return Merged line segment
		 */
		static FiniteLine2 fuse(const FiniteLine2& line0, const FiniteLine2& line1);

		/**
		 * Find similar lines in Hemi cube
		 * @param line Line for which similar lines are searched, must be valid
		 * @param radius Search radius in the Hemi cube, does not search across different faces of the Hemi cube, range: [1, infinity)
		 * @return Indices of the lines in `linesInMap` that are similar to input parameter `line`.
		 * @sa lines(), operator[]()
		 */
		IndexSet32 find(const FiniteLine2& line, const Scalar radius = Scalar(1)) const;

		/**
		 * Return the number of lines stored in the Hemi cube
		 * @return Number of lines stored in the Hemi cube
		 */
		inline size_t size() const;

		/**
		 * Return the number of bins in the Hemi cube which actually contain data
		 * @return Number of non-empty bins
		 */
		inline size_t nonEmptyBins() const;

		/**
		 * Clear this Hemi cube
		 */
		inline void clear();

		/**
		 * Get a reference to the lines stored in the Hemi cube
		 * @return All lines in the Hemi cube
		 * @sa find()
		 */
		inline const FiniteLines2& lines() const;

		/**
		 * Returns a reference to the internal map of lines indices
		 * @return The map with line indices
		 */
		const Map& map() const;

		/**
		 * Get a const reference to a line in the Hemi cube
		 * @param index Index of a line in the Hemi cube, range: [0, HemiCube::size())
		 * @return Const reference a line in the Hemi cube
		 * @sa find()
		 */
		inline const FiniteLine2& operator[](const unsigned int index) const;

		/**
		 * Get a reference to a line in the Hemi cube
		 * @param index Index of a line in the Hemi cube, range: [0, HemiCube::size())
		 * @return Reference a line in the Hemi cube
		 * @sa find()
		 */
		inline FiniteLine2& operator[](const unsigned int index);

	protected:

		/**
		 * Given the map index of a line, compute its pixel location in a image representation of the cube map
		 * @param mapIndex The map index of a line segment with range [0, bins - 1]x[0, bins - 1]x[0, 2]
		 * @return The pixel location of the line segment in the cube map
		 * @sa computeMapIndex()
		 */
		inline PixelPosition hemiCubeCoordinatesFrom(const MapIndex& mapIndex) const;

		/**
		 * Given a 2D line segment, compute its location (map index) in the cube map
		 * @param line The line for which its map index is computed
		 * @return The map index of the input line
		 */
		MapIndex mapIndexFrom(const FiniteLine2& line) const;

		/**
		 * For a given 2D line segment compute its representation as a line equation (3D vector)
		 * The line equation is in normalized coordinates rather than image coordinates
		 * The line equation is the normal of the plane that intersects the line segment and the camera's center of projection
		 * @param line Line for which a line equation is computed
		 * @return The line equation represention of input line
		 * @tparam tScale If true, the line equation (3D vector) will be scaled to point to a face of the cube map, otherwise no scaling will be applied
		 */
		template <bool tScale>
		Vector3 lineEquationFrom(const FiniteLine2& line) const;

		/**
		 * Compute the point on the ray at a specific depth (3D ray which points from the center of projection to the image point on the project plane)
		 * @param point Point that is converted into a 3D ray
		 * @return The 3D ray corresponding to the input point
		 */
		inline Vector3 rayFrom(const Vector2& point) const;

		/**
		 * Update a line segment stored in the Hemi cube
		 * Updates the line segment at index `index` and its map index in the Hemi cube.
		 * @param index Index of a line in the Hemi cube, range: [0, HemiCube::size())
		 * @param updatedLine Updated line segment that will be stored at index `index` in the Hemi cube
		 */
		void updateLine(unsigned int index, const FiniteLine2& updatedLine);

	protected:

		/// All lines which are represented by their indices in the map
		FiniteLines2 linesInMap_;

		/// Width of the image in which the lines in the cube map have been found; used to convert 2D image points into 3D rays
		unsigned int imageWidth_ = 0u;

		/// Height of the image in which the lines in the cube map have been found; used to convert 2D image points into 3D rays
		unsigned int imageHeight_ = 0u;

		/// Principal point of the image in which the lines have have been found; using the image center should work just as fine
		Vector2 principalPoint_ = Vector2(Scalar(0), Scalar(0));

		/// Focal length of the original camera; used convert 2D image points into 3D rays.
		Scalar focalLength_ = Scalar(0);

		/// The actual map data structure
		Map map_;

		/// Number of bins along one dimension (cube)
		unsigned int numberBins_ = 0u;
};

inline bool HemiCube::isValid() const
{
	return numberBins_ != 0u && imageWidth_ != 0u && imageHeight_ != 0u && Numeric::isNotEqualEps(focalLength_) && principalPoint_ != Vector2(Scalar(0), Scalar(0));
}

inline void HemiCube::insert(const FiniteLine2& newLine)
{
	ocean_assert(isValid());
	ocean_assert(newLine.isValid());

	// Find or create the bin into which the new line will be placed
	IndexSet32& bin = map_[mapIndexFrom(newLine)];

	const unsigned int newLineIndex = (unsigned int)linesInMap_.size();
	ocean_assert(bin.find(newLineIndex) == bin.end());

	bin.insert(newLineIndex);
	linesInMap_.emplace_back(newLine);
}

inline void HemiCube::insert(const FiniteLines2& lines)
{
	for (const FiniteLine2& line : lines)
	{
		insert(line);
	}
}

inline size_t HemiCube::size() const
{
	return linesInMap_.size();
}

inline size_t HemiCube::nonEmptyBins() const
{
	return map_.size();
}

inline void HemiCube::clear()
{
	linesInMap_.clear();
	map_.clear();
}

inline const FiniteLines2& HemiCube::lines() const
{
	return linesInMap_;
}

inline const HemiCube::Map& HemiCube::map() const
{
	return map_;
}

inline const FiniteLine2& HemiCube::operator[](const unsigned int index) const
{
	ocean_assert(index < (unsigned int)linesInMap_.size());
	return linesInMap_[index];
}

inline FiniteLine2& HemiCube::operator[](const unsigned int index)
{
	ocean_assert(index < (unsigned int)linesInMap_.size());
	return linesInMap_[index];
}

inline CV::PixelPosition HemiCube::hemiCubeCoordinatesFrom(const MapIndex& mapIndex) const
{
	ocean_assert(isValid());
	ocean_assert(mapIndex[0] < numberBins_ && mapIndex[1] < numberBins_ && mapIndex[2] <= 2u);
	return CV::PixelPosition(mapIndex[2] * numberBins_ + mapIndex[0], mapIndex[1]);
}

template <bool tScale>
Vector3 HemiCube::lineEquationFrom(const FiniteLine2& line) const
{
	ocean_assert(line.isValid());
	const Vector3 ray0 = rayFrom(line.point0());
	const Vector3 ray1 = rayFrom(line.point1());
	ocean_assert(Numeric::isNotEqualEps(ray0.length()));
	ocean_assert(Numeric::isNotEqualEps(ray1.length()));

	const Vector3 lineEquation = ray0.cross(ray1);
	ocean_assert(Numeric::isNotEqualEps(lineEquation.length()));

	if constexpr (tScale)
	{
		const Scalar maxValue = std::max(std::abs(lineEquation[0]), std::max(std::abs(lineEquation[1]), std::abs(lineEquation[2])));
		ocean_assert(Numeric::isNotEqualEps(maxValue));
		return lineEquation * (Scalar(1.0) / maxValue);
	}

	return lineEquation;
}

Vector3 HemiCube::rayFrom(const Vector2& point) const
{
	const Scalar inPixels = Scalar(0.5) * Scalar(std::max(imageWidth_, imageHeight_));
	return Vector3(point - principalPoint_, focalLength_ * inPixels);
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_HEMI_CUBE_H
