/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_NON_MAXIMUM_SUPPRESSION_H
#define META_OCEAN_CV_NON_MAXIMUM_SUPPRESSION_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/base/ShiftVector.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Vector2.h"

#include <functional>

namespace Ocean
{

namespace CV
{

/**
 * This class provides base functionality and type definitions for non-maximum-suppression operations.
 * It defines common types like StrengthPosition and StrengthPositions used by the template class NonMaximumSuppressionT.
 * @see NonMaximumSuppressionT
 * @ingroup cv
 */
class NonMaximumSuppression
{
	public:

		/**
		 * Definition of individual suppression modes for extremum search.
		 */
		enum SuppressionMode : uint32_t
		{
			/// Finds the maximum values, any value is allowed.
			SM_MAXIMUM,
			/// Finds the minimum values, any value is allowed.
			SM_MINIMUM,
			/// Finds the maximum values, only positive values are allowed (> 0).
			SM_MAXIMUM_POSITIVE_ONLY,
			/// Finds the minimum values, only negative values are allowed (< 0).
			SM_MINIMUM_NEGATIVE_ONLY
		};

		/**
		 * Definition of individual refinement status values for iterative sub-pixel peak refinement.
		 */
		enum RefinementStatus : uint32_t
		{
			/// Invalid status, no refinement was applied.
			RS_INVALID = 0u,
			/// The fitted offset exceeded the step size, indicating divergence.
			RS_DIVERGED,
			/// The sample position fell outside the frame bounds.
			RS_BORDER,
			/// The refinement did not converge within the maximum number of iterations.
			RS_MAX_ITERATIONS,
			/// The refinement converged, the offset is below the convergence threshold.
			RS_CONVERGED
		};

		/**
		 * This class extends a 2D position by a third parameter storing a strength value.
		 * @tparam TCoordinate The data type of a scalar coordinate
		 * @tparam TStrength The data type of the strength parameter
		 */
		template <typename TCoordinate, typename TStrength>
		class StrengthPosition : public VectorT2<TCoordinate>
		{
			public:

				/**
				 * Creates a new object with default strength parameter.
				 */
				StrengthPosition() = default;

				/**
				 * Creates a new object with explicit position and strength parameter.
				 * @param x Horizontal position
				 * @param y Vertical position
				 * @param strength The strength parameter
				 */
				inline StrengthPosition(const TCoordinate x, const TCoordinate y, const TStrength& strength);

				/**
				 * Returns the strength parameter of this object.
				 * @return Strength parameter
				 */
				inline const TStrength& strength() const;

				/**
				 * Compares the strength value of two objects.
				 * @param left The left object to compare
				 * @param right The right object to compare
				 * @return True, if so
				 * @tparam tLeftLargerThanRight True, to return whether left's strength is larger than right's strength; False, to return whether right is larger than left
				 */
				template <bool tLeftLargerThanRight = true>
				static inline bool compareStrength(const StrengthPosition<TCoordinate, TStrength>& left, const StrengthPosition<TCoordinate, TStrength>& right);

			private:

				/// Strength parameter of this object.
				TStrength strength_ = TStrength();
		};

		/**
		 * Definition of a vector holding strength pixel positions.
		 */
		template <typename TCoordinate, typename TStrength>
		using StrengthPositions = std::vector<StrengthPosition<TCoordinate, TStrength>>;
};

/**
 * This class implements the possibility to find local maximum in a 2D array by applying a non-maximum-suppression search.
 * The search is done within a 3x3 neighborhood (centered around the point of interest).<br>
 * Use this class to determine e.g. reliable feature points.<br>
 * The class supports bin accuracy (pixel accuracy) and sub-bin accuracy (sub-pixel accuracy).
 *
 * The non-maximum-suppression search is implemented by a vertical list holding maps of horizontal array elements.<br>
 * The performance depends on the number of elements inserted into the individual maps.<br>
 * Thus, do not add data elements with negligible value.
 *
 * It should be mentioned that the application of this class should be restricted to situations in which the entire filter response values do not exist already.<br>
 * The performance boost comes with a simultaneous determination of filter responses and the insertion of possible candidates for maximum locations.
 * @tparam T The data type of the individual elements that are applied for the non-maximum-suppression search.
 * @ingroup cv
 */
template <typename T>
class NonMaximumSuppressionT : public NonMaximumSuppression
{
	public:

		/**
		 * Definition of a callback function used to determine the precise sub-pixel position of a specific point.
		 * @param x The horizontal position of the point, with range [0, width - 1]
		 * @param y The vertical position of the point, with range [0, height - 1]
		 * @param strength The strength parameter already known for the position of the point
		 * @param preciseX The resulting precise horizontal position of the point, with range (x - 1, x + 1)
		 * @param preciseY The resulting precise vertical position of the point, with range (y - 1, y + 1)
		 * @param preciseStrength The resulting precise strength parameter of the precise point
		 * @return True, if the precise position could be determined
		 * @tparam TCoordinate The data type of a scalar coordinate
		 * @tparam TStrength The data type of the strength parameter
		 */
		template <typename TCoordinate, typename TStrength>
		using PositionCallback = std::function<bool(const unsigned int x, const unsigned int y, const TStrength strength, TCoordinate& preciseX, TCoordinate& preciseY, TStrength& preciseStrength)>;

	protected:

		/**
		 * This class holds the horizontal position and strength parameter of an interest pixel.
		 */
		class StrengthCandidate
		{
			public:

				/**
				 * Creates a new candidate object.
				 */
				inline StrengthCandidate();

				/**
				 * Creates a new candidate object with horizontal position and strength parameter.
				 * @param x Horizontal position in pixel
				 * @param strength The strength parameter
				 */
				inline StrengthCandidate(const unsigned int x, const T& strength);

				/**
				 * Returns the horizontal position of this candidate object.
				 * @return Horizontal position in pixel
				 */
				inline unsigned int x() const;

				/**
				 * Returns the strength parameter of this object.
				 * @return Strength parameter
				 */
				inline const T& strength() const;

			private:

				/// Horizontal position of this object.
				unsigned int positionX_ = (unsigned int)(-1);

				/// Strength parameter of this object.
				T strength_ = T();
		};

		/**
		 * Definition of a vector holding strength candidate objects.
		 */
		using StrengthCandidateRow = std::vector<StrengthCandidate>;

		/**
		 * Definition of a vector holding a vector of strength candidates.
		 */
		using StrengthCandidateRows = ShiftVector<StrengthCandidateRow>;

	public:

		/**
		 * Move constructor.
		 * @param nonMaximumSuppression The object to be moved
		 */
		NonMaximumSuppressionT(NonMaximumSuppressionT<T>&& nonMaximumSuppression) noexcept;

		/**
		 * Copy constructor.
		 * @param nonMaximumSuppression The object to be moved
		 */
		NonMaximumSuppressionT(const NonMaximumSuppressionT<T>& nonMaximumSuppression) noexcept;

		/**
		 * Creates a new maximum suppression object with a predefined size.
		 * @param width The width of this object in pixel, with range [3, infinity)
		 * @param height The height of this object in pixel, with range [3, infinity)
		 * @param yOffset Optional offset in the vertical direction moving the suppression region by the specified number of rows, with range [0, infinity)
		 */
		NonMaximumSuppressionT(const unsigned int width, const unsigned int height, const unsigned int yOffset = 0u) noexcept;

		/**
		 * Returns the width of this object.
		 * @return Width in pixel
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of this object.
		 * @return Height in pixel
		 */
		inline unsigned int height() const;

		/**
		 * Returns the optional offset in the vertical direction.
		 * @return Optional vertical direction offset, 0 by default
		 */
		inline unsigned int yOffset() const;

		/**
		 * Adds a new candidate to this object.
		 * Beware: Due to performance issues do no add candidates with negligible strength parameter.
		 * @param x Horizontal position in pixel, with range [0, width() - 1]
		 * @param y Vertical position in pixel, with range [yOffset(), yOffset() + height() - 1]
		 * @param strength The strength parameter
		 * @see addCandidates(), removeCandidatesFromRight().
		 */
		inline void addCandidate(const unsigned int x, const unsigned int y, const T& strength);

		/**
		 * Adds new candidates to this object from a given buffer providing one value for each bin/pixel of this object.
		 * Beware: Due to performance reasons, you should use the addCandidate() function to add one single new candidate in the moment the filter response is larger than a specific threshold.<br>
		 * @param values The from which candidates will be added, must be width() * height() elements
		 * @param valuesPaddingElements The number of padding elements at the end of each values row, in elements, with range [0, infinity)
		 * @param firstColumn First column to be handled, with range [0, width() - 1]
		 * @param numberColumns Number of columns to be handled, with range [1, width() - firstColumn]
		 * @param firstRow First row to be handled, with range [yOffset(), height() - 1]
		 * @param numberRows Number of rows to be handled, with range [1, height() - firstRow]
		 * @param minimalThreshold The minimal threshold so that a value counts as candidate
		 * @param worker Optional worker object to distribute the computation
		 * @see addCandidate(), removeCandidatesFromRight().
		 */
		void addCandidates(const T* values, const unsigned int valuesPaddingElements, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows, const T& minimalThreshold, Worker* worker);

		/**
		 * Removes all candidates from a specified row having a horizontal location equal or larger than a specified coordinate.
		 * @param x The horizontal coordinate specifying which candidates will be removed, all candidates with horizontal location >= x will be removed, with range [0, infinity)
		 * @param y The index of the row in which the candidates will be removed, with range [yOffset(), yOffset() + height() - 1]
		 */
		inline void removeCandidatesRightFrom(const unsigned int x, const unsigned int y);

		/**
		 * Returns the strength value of a candidate at a specified position.
		 * The function uses binary search on the x-coordinate within the specified row, as candidates are sorted by horizontal position.
		 * @param x The horizontal position to look up, with range [0, width() - 1]
		 * @param y The vertical position (row) to look up, with range [yOffset(), yOffset() + height() - 1]
		 * @param strength The resulting strength value if a candidate exists at the specified position
		 * @return True, if a candidate exists at the specified position; False, if no candidate was found
		 */
		bool candidate(const unsigned int x, const unsigned int y, T& strength) const;

		/**
		 * Returns all gathered candidates of this object.
		 * The resulting candidates are raw candidates without any suppression.
		 * @param firstColumn The first column from which candidates will be returned, with range [0, width() - 1]
		 * @param numberColumns The number of columns for which candidates will be returned, with range [1, width() - firstColumn]
		 * @param firstRow The first row from which candidates will be returned, with range [yOffset(), height() - 1]
		 * @param numberRows The number of rows for which candidates will be returned, with range [1, height() - firstRow]
		 * @param strengthPositions The resulting strength positions
		 */
		void candidates(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows, StrengthPositions<unsigned int, T>& strengthPositions) const;

		/**
		 * Applies a non-maximum-suppression search on a given 2D frame in a 3x3 neighborhood (eight neighbors).
		 * This function allows to determine the precise position of the individual maximum value positions by application of a callback function determining the individual positions.<br>
		 * @param firstColumn First column to be handled, with range [1, width() - 1)
		 * @param numberColumns Number of columns to be handled
		 * @param firstRow First row to be handled, with range [yOffset() + 1, height() - 1)
		 * @param numberRows Number of rows to be handled
		 * @param strengthPositions The resulting non maximum suppressed positions including the strength parameters, will be appended
		 * @param worker Optional worker object to distribute the computation
		 * @param positionCallback Optional callback function allowing to determine the precise position of the individual maximum value positions
		 * @return True, if succeeded
		 * @tparam TCoordinate The data type of a scalar coordinate
		 * @tparam TStrength The data type of the strength parameter
		 * @tparam tStrictMaximum True, to search for a strict maximum (larger than all eight neighbors); False, to allow equal values in the upper left neighborhood
		 * @tparam tSuppressionMode The suppression mode defining whether to find maximum/minimum and whether to enforce sign constraints
		 */
		template <typename TCoordinate, typename TStrength, bool tStrictMaximum = true, NonMaximumSuppression::SuppressionMode tSuppressionMode = NonMaximumSuppression::SM_MAXIMUM>
		bool suppressNonMaximum(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows, StrengthPositions<TCoordinate, TStrength>& strengthPositions, Worker* worker = nullptr, const PositionCallback<TCoordinate, TStrength>* positionCallback = nullptr) const;

		/**
		 * Removes the gathered non-maximum suppression information so that this object can be reused again (for the same task with same resolution etc.).
		 * The allocated memory will remain so that reusing this object may improve performance.
		 */
		void reset();

		/**
		 * Move operator.
		 * @param nonMaximumSuppression Object to be moved
		 * @return Reference to this object
		 */
		NonMaximumSuppressionT<T>& operator=(NonMaximumSuppressionT<T>&& nonMaximumSuppression);

		/**
		 * Copy operator.
		 * @param nonMaximumSuppression The object to be copied
		 * @return Reference to this object
		 */
		NonMaximumSuppressionT<T>& operator=(const NonMaximumSuppressionT<T>& nonMaximumSuppression);

		/**
		 * Applies a non-maximum-suppression based on already existing strength positions (just with a custom suppression radius) e.g., as a post-processing step.
		 * @param width The width of the image/domain in which the strength positions are located, e.g., in pixel, with range [1, infinity)
		 * @param height The height of the image/domain in which the strength positions are located, e.g., in pixel, with range [1, infinity)
		 * @param strengthPositions The strength positions for which a custom suppression-radius will be applied
		 * @param radius The suppression radius to be applied, with range [1, infinity)
		 * @param validIndices Optional resulting indices of all strength positions which remain after suppression
		 * @return The resulting strength positions
		 * @tparam TCoordinate The data type of a scalar coordinate
		 * @tparam TStrength The data type of the strength parameter
		 * @tparam tStrictMaximum True, to search for a strict maximum (larger than all eight neighbors); False, to allow equal values in the upper left neighborhood
		 */
		template <typename TCoordinate, typename TStrength, bool tStrictMaximum>
		static StrengthPositions<TCoordinate, TStrength> suppressNonMaximum(const unsigned int width, const unsigned int height, const StrengthPositions<TCoordinate, TStrength>& strengthPositions, const TCoordinate radius, Indices32* validIndices = nullptr);

		/**
		 * Determines the precise peak location in 1D space for three discrete neighboring measurements at location x == 0.
		 * The precise peak is determined based on the first and second derivatives of the measurement values.
		 * @param leftValue The left discrete value, e.g., at location x - 1, with range (-infinity, middleValue]
		 * @param middleValue The middle discrete value, e.g., at location x, with range (-infinity, infinity)
		 * @param rightValue The  discrete value, e.g., at location x + 1, with range (-infinity, middleValue]
		 * @param location The location of the precise peak of all values, with range (-1, 1)
		 * @return True, if succeeded
		 * @tparam TFloat The floating point data type to be used for calculation, either 'float' or 'double'
		 */
		template <typename TFloat>
		static bool determinePrecisePeakLocation1(const T& leftValue, const T& middleValue, const T& rightValue, TFloat& location);

		/**
		 * Determines the precise peak location in 2D space for nine discrete neighboring measurements at location x == 0, y == 0.
		 * The precise peak is determined based on the first and second derivatives of the measurement values.
		 * @param topValues The three discrete values in the top row, must be valid
		 * @param centerValues The three discrete values in the center row, must be valid
		 * @param bottomValues The three discrete values in the bottom row, must be valid
		 * @param location The location of the precise peak of all values, with range (-1, 1)
		 * @return True, if succeeded
		 * @tparam TFloat The floating point data type to be used for calculation, either 'float' or 'double'
		 */
		template <typename TFloat>
		static bool determinePrecisePeakLocation2(const T* const topValues, const T* const centerValues, const T* const bottomValues, VectorT2<TFloat>& location);

		/**
		 * Determines the precise peak location in 2D space by fitting a quadratic surface via least-squares to a tSize x tSize grid of values.
		 * Uses all tSize * tSize samples to fit F(x,y) = a + bx + cy + dx^2 + exy + fy^2 and solves for the peak of the fitted surface.<br>
		 * For a 3x3 grid this reduces to central differences; for larger grids the least-squares fit uses all samples for a more robust estimate.<br>
		 * The function validates that the fitted surface has the correct curvature (negative-definite Hessian for maximum, positive-definite for minimum).
		 * @param values The grid values in row-major order, must have at least (tSize - 1) * valuesStrideElements + tSize elements
		 * @param valuesStrideElements The number of elements between the start of two consecutive rows, with range [tSize, infinity)
		 * @param offsetX The resulting horizontal offset of the peak from the grid center, in range (-1, 1)
		 * @param offsetY The resulting vertical offset of the peak from the grid center, in range (-1, 1)
		 * @return True, if the fit succeeded, the curvature is correct, and the offset is within range; False, if the surface is degenerate, has wrong curvature, or the offset is out of range
		 * @tparam TFloat The floating point data type to be used for calculation, either 'float' or 'double'
		 * @tparam tSize The size of the sampling grid, must be odd and >= 3
		 * @tparam tFindMaximum True, to find the maximum of the fitted surface; False, to find the minimum
		 */
		template <typename TFloat, unsigned int tSize, bool tFindMaximum = true>
		static bool determinePrecisePeakLocationNxN(const TFloat* values, const size_t valuesStrideElements, TFloat& offsetX, TFloat& offsetY);

		/**
		 * Determines the precise sub-pixel peak location by iteratively sampling image intensity values at sub-pixel positions.
		 * The function uses bilinear interpolation to sample a tSize x tSize grid of intensity values around the current estimate, calls determinePrecisePeakLocationNxN() to find the sub-pixel offset, and iterates with decreasing step size until convergence.<br>
		 * The valid sampling range is [0, width - 1]x[0, height - 1] for PC_TOP_LEFT or [0, width]x[0, height] for PC_CENTER.
		 * @param frame Pointer to the first pixel of the grayscale frame (single channel, uint8_t), must be valid
		 * @param width The width of the frame in pixels, with range [tSize, infinity)
		 * @param height The height of the frame in pixels, with range [tSize, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param x The horizontal position of the initial peak estimate, in the coordinate system defined by tPixelCenter
		 * @param y The vertical position of the initial peak estimate, in the coordinate system defined by tPixelCenter
		 * @param preciseX The resulting precise horizontal position, in the coordinate system defined by tPixelCenter
		 * @param preciseY The resulting precise vertical position, in the coordinate system defined by tPixelCenter
		 * @param maxIterations The maximum number of refinement iterations, with range [1, infinity)
		 * @param stepSize The initial step size for the sampling grid, with range (0, 1]
		 * @param convergenceThreshold The threshold for the pixel offset magnitude below which convergence is declared, with range (0, infinity)
		 * @return The refinement status
		 * @tparam TFloat The floating point data type to be used for calculation, either 'float' or 'double'
		 * @tparam tSize The size of the sampling grid, must be odd and >= 3
		 * @tparam tFindMaximum True, to find the intensity maximum; False, to find the intensity minimum
		 * @tparam tPixelCenter The pixel center to be used during interpolation, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 */
		template <typename TFloat, unsigned int tSize, bool tFindMaximum = true, PixelCenter tPixelCenter = PC_TOP_LEFT>
		static NonMaximumSuppression::RefinementStatus determinePrecisePeakLocationIterativeNxN(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const TFloat x, const TFloat y, TFloat& preciseX, TFloat& preciseY, const unsigned int maxIterations = 5u, const TFloat stepSize = TFloat(0.75), const TFloat convergenceThreshold = TFloat(0.01));

	private:

		/**
		 * Adds new candidates to this object from a subset of a given buffer providing one value for each bin/pixel of this object.
		 * @param values The from which candidates will be added, must be width() * height() elements
		 * @param valuesStrideElements The number of elements between two values rows, in elements, with range [0, infinity)
		 * @param minimalThreshold The minimal threshold so that a value counts as candidate
		 * @param firstColumn First column to be handled, with range [0, width() - 1]
		 * @param numberColumns Number of columns to be handled, with range [1, width() - firstColumn]
		 * @param firstRow The first row in the buffer from which the candidates will be added, with range [yOffset(), height())
		 * @param numberRows The number of rows to be handled, with range [1, height() - firstRow]
		 * @see addCandidates().
		 */
		void addCandidatesSubset(const T* values, const unsigned int valuesStrideElements, const unsigned int firstColumn, const unsigned int numberColumns, const T* minimalThreshold, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies a non-maximum-suppression search on a subset of a given 2D frame in a 3x3 neighborhood (eight neighbors).
		 * This function allows to determine the precise position of the individual maximum value positions by application of a callback function determining the individual positions.<br>
		 * @param strengthPositions Resulting non maximum suppressed positions including the strength parameters
		 * @param firstColumn First column to be handled
		 * @param numberColumns Number of columns to be handled
		 * @param lock Optional lock when executed in parallel
		 * @param positionCallback Optional callback function allowing to determine the precise position of the individual maximum value positions
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam TCoordinate The data type of a scalar coordinate
		 * @tparam TStrength The data type of the strength parameter
		 * @tparam tStrictMaximum True, to search for a strict maximum (larger than all eight neighbors); False, to allow equal values in the upper left neighborhood
		 * @tparam tOnlyPositive True, to only consider positive values (> 0); False, to consider all values
		 */
		template <typename TCoordinate, typename TStrength, bool tStrictMaximum, bool tOnlyPositive = false>
		void suppressNonMaximumSubset(StrengthPositions<TCoordinate, TStrength>* strengthPositions, const unsigned int firstColumn, const unsigned int numberColumns, Lock* lock, const PositionCallback<TCoordinate, TStrength>* positionCallback, const unsigned int firstRow, const unsigned int numberRows) const;

		/**
		 * Applies a non-minimum-suppression search on a subset of a given 2D frame in a 3x3 neighborhood (eight neighbors).
		 * This function allows to determine the precise position of the individual minimum value positions by application of a callback function determining the individual positions.<br>
		 * @param strengthPositions Resulting non minimum suppressed positions including the strength parameters
		 * @param firstColumn First column to be handled
		 * @param numberColumns Number of columns to be handled
		 * @param lock Optional lock when executed in parallel
		 * @param positionCallback Optional callback function allowing to determine the precise position of the individual minimum value positions
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam TCoordinate The data type of a scalar coordinate
		 * @tparam TStrength The data type of the strength parameter
		 * @tparam tStrictMaximum True, to search for a strict minimum (smaller than all eight neighbors); False, to allow equal values in the upper left neighborhood
		 * @tparam tOnlyNegative True, to only consider negative values (< 0); False, to consider all values
		 */
		template <typename TCoordinate, typename TStrength, bool tStrictMaximum, bool tOnlyNegative = false>
		void suppressNonMinimumSubset(StrengthPositions<TCoordinate, TStrength>* strengthPositions, const unsigned int firstColumn, const unsigned int numberColumns, Lock* lock, const PositionCallback<TCoordinate, TStrength>* positionCallback, const unsigned int firstRow, const unsigned int numberRows) const;

	private:

		/// Width of this object.
		unsigned int width_ = 0u;

		/// All candidate rows.
		StrengthCandidateRows rows_;
};

template <typename TCoordinate, typename TStrength>
inline NonMaximumSuppression::StrengthPosition<TCoordinate, TStrength>::StrengthPosition(const TCoordinate x, const TCoordinate y, const TStrength& strength) :
	VectorT2<TCoordinate>(x, y),
	strength_(strength)
{
	// nothing to do here
}

template <typename TCoordinate, typename TStrength>
inline const TStrength& NonMaximumSuppression::StrengthPosition<TCoordinate, TStrength>::strength() const
{
	return strength_;
}

template <typename TCoordinate, typename TStrength>
template <bool tLeftLargerThanRight>
inline bool NonMaximumSuppression::StrengthPosition<TCoordinate, TStrength>::compareStrength(const StrengthPosition<TCoordinate, TStrength>& left, const StrengthPosition<TCoordinate, TStrength>& right)
{
	if constexpr (tLeftLargerThanRight)
	{
		return left.strength() > right.strength();
	}
	else
	{
		return left.strength() < right.strength();
	}
}

template <typename T>
inline NonMaximumSuppressionT<T>::StrengthCandidate::StrengthCandidate() :
	positionX_(-1),
	strength_(T())
{
	// nothing to do here
}

template <typename T>
inline NonMaximumSuppressionT<T>::StrengthCandidate::StrengthCandidate(const unsigned int x, const T& strength) :
	positionX_(x),
	strength_(strength)
{
	// nothing to do here
}

template <typename T>
inline unsigned int NonMaximumSuppressionT<T>::StrengthCandidate::x() const
{
	return positionX_;
}

template <typename T>
inline const T& NonMaximumSuppressionT<T>::StrengthCandidate::strength() const
{
	return strength_;
}

template <typename T>
NonMaximumSuppressionT<T>::NonMaximumSuppressionT(NonMaximumSuppressionT<T>&& nonMaximumSuppression) noexcept
{
	*this = std::move(nonMaximumSuppression);
}

template <typename T>
NonMaximumSuppressionT<T>::NonMaximumSuppressionT(const NonMaximumSuppressionT<T>& nonMaximumSuppression) noexcept :
	width_(nonMaximumSuppression.width_),
	rows_(nonMaximumSuppression.rows_)
{
	// nothing to do here
}

template <typename T>
NonMaximumSuppressionT<T>::NonMaximumSuppressionT(const unsigned int width, const unsigned int height, const unsigned int yOffset) noexcept :
	width_(width),
	rows_(yOffset, height, StrengthCandidateRow())
{
	// nothing to do here
}

template <typename T>
inline unsigned int NonMaximumSuppressionT<T>::width() const
{
	return width_;
}

template <typename T>
inline unsigned int NonMaximumSuppressionT<T>::height() const
{
	return (unsigned int)rows_.size();
}

template <typename T>
inline unsigned int NonMaximumSuppressionT<T>::yOffset() const
{
	ocean_assert(rows_.firstIndex() >= 0);

	return (unsigned int)(rows_.firstIndex());
}

template <typename T>
inline void NonMaximumSuppressionT<T>::addCandidate(const unsigned int x, const unsigned int y, const T& strength)
{
	ocean_assert(x < width_);

	ocean_assert(rows_.isValidIndex(y) && y >= (unsigned int)(rows_.firstIndex()) && y <= (unsigned int)(rows_.endIndex()));

	if (rows_[y].empty())
	{
		rows_[y].reserve(128);
	}

	rows_[y].emplace_back(x, strength);
}

template <typename T>
void NonMaximumSuppressionT<T>::addCandidates(const T* values, const unsigned int valuesPaddingElements, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows, const T& minimalThreshold, Worker* worker)
{
	ocean_assert(values != nullptr);

	ocean_assert(firstColumn + numberColumns <= width_);
	ocean_assert(typename StrengthCandidateRows::Index(firstRow) >= rows_.firstIndex());
	ocean_assert(typename StrengthCandidateRows::Index(firstRow + numberRows) <= rows_.endIndex());

	const unsigned int valuesStrideElements = width_ + valuesPaddingElements;

	if (worker != nullptr)
	{
		worker->executeFunction(Worker::Function::create(*this, &NonMaximumSuppressionT<T>::addCandidatesSubset, values, valuesStrideElements, firstColumn, numberColumns, &minimalThreshold, 0u, 0u), firstRow, numberRows, 5u, 6u, 20u);
	}
	else
	{
		addCandidatesSubset(values, valuesStrideElements, firstColumn, numberColumns, &minimalThreshold, firstRow, numberRows);
	}
}

template <typename T>
inline void NonMaximumSuppressionT<T>::removeCandidatesRightFrom(const unsigned int x, const unsigned int y)
{
	ocean_assert(rows_.isValidIndex(y) && y >= (unsigned int)(rows_.firstIndex()) && y <= (unsigned int)(rows_.endIndex()));

	StrengthCandidateRow& suppressionRow = rows_[y];

	while (!suppressionRow.empty())
	{
		if (suppressionRow.back().x() >= x)
		{
			suppressionRow.pop_back();
		}
		else
		{
			break;
		}
	}
}

template <typename T>
bool NonMaximumSuppressionT<T>::candidate(const unsigned int x, const unsigned int y, T& strength) const
{
	ocean_assert(x < width_);
	ocean_assert(rows_.isValidIndex(y) && y >= (unsigned int)(rows_.firstIndex()) && y <= (unsigned int)(rows_.endIndex()));

	const StrengthCandidateRow& suppressionRow = rows_[y];

	if (suppressionRow.empty())
	{
		return false;
	}

	size_t left = 0;
	size_t right = suppressionRow.size() - 1;

	while (left <= right)
	{
		const size_t mid = left + (right - left) / 2;

		const unsigned int midX = suppressionRow[mid].x();

		if (midX == x)
		{
			strength = suppressionRow[mid].strength();
			return true;
		}
		else if (midX < x)
		{
			left = mid + 1;
		}
		else
		{
			if (mid == 0)
			{
				break;
			}

			right = mid - 1;
		}
	}

	return false;
}

template <typename T>
void NonMaximumSuppressionT<T>::candidates(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows, NonMaximumSuppressionT<T>::StrengthPositions<unsigned int, T>& strengthPositions) const
{
	ocean_assert(firstColumn + numberColumns <= width_);
	ocean_assert(firstRow + numberRows <= (unsigned int)(rows_.endIndex()));

	const unsigned int endColumn = firstColumn + numberColumns;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		const StrengthCandidateRow& row = rows_[y];

		for (const StrengthCandidate& candidate : row)
		{
			if (candidate.x() < firstColumn)
			{
				continue;
			}

			if (candidate.x() >= endColumn)
			{
				break;
			}

			strengthPositions.emplace_back(candidate.x(), y, candidate.strength());
		}
	}
}

template <typename T>
template <typename TCoordinate, typename TStrength, bool tStrictMaximum, NonMaximumSuppression::SuppressionMode tSuppressionMode>
bool NonMaximumSuppressionT<T>::suppressNonMaximum(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows, StrengthPositions<TCoordinate, TStrength>& strengthPositions, Worker* worker, const PositionCallback<TCoordinate, TStrength>* positionCallback) const
{
	ocean_assert(firstColumn + numberColumns <= width_);
	ocean_assert(firstRow >= (unsigned int)(rows_.firstIndex()) && firstRow + numberRows <= (unsigned int)(rows_.endIndex()));

	if (firstColumn + numberColumns > width_)
	{
		return false;
	}

	if (firstRow >= (unsigned int)(rows_.endIndex()) || firstRow + numberRows <= (unsigned int)(rows_.firstIndex()))
	{
		return false;
	}

	strengthPositions.reserve(strengthPositions.size() + 128);

	if constexpr (tSuppressionMode == NonMaximumSuppression::SM_MAXIMUM || tSuppressionMode == NonMaximumSuppression::SM_MAXIMUM_POSITIVE_ONLY)
	{
		constexpr bool tOnlyPositive = tSuppressionMode == NonMaximumSuppression::SM_MAXIMUM_POSITIVE_ONLY;

		if (worker != nullptr)
		{
			Lock lock;
			worker->executeFunction(Worker::Function::create(*this, &NonMaximumSuppressionT<T>::suppressNonMaximumSubset<TCoordinate, TStrength, tStrictMaximum, tOnlyPositive>, &strengthPositions, firstColumn, numberColumns, &lock, positionCallback, 0u, 0u), firstRow, numberRows, 5u, 6u, 3u);
		}
		else
		{
			suppressNonMaximumSubset<TCoordinate, TStrength, tStrictMaximum, tOnlyPositive>(&strengthPositions, firstColumn, numberColumns, nullptr, positionCallback, firstRow, numberRows);
		}
	}
	else
	{
		static_assert(tSuppressionMode == NonMaximumSuppression::SM_MINIMUM || tSuppressionMode == NonMaximumSuppression::SM_MINIMUM_NEGATIVE_ONLY);

		constexpr bool tOnlyNegative = tSuppressionMode == NonMaximumSuppression::SM_MINIMUM_NEGATIVE_ONLY;

		if (worker != nullptr)
		{
			Lock lock;
			worker->executeFunction(Worker::Function::create(*this, &NonMaximumSuppressionT<T>::suppressNonMinimumSubset<TCoordinate, TStrength, tStrictMaximum, tOnlyNegative>, &strengthPositions, firstColumn, numberColumns, &lock, positionCallback, 0u, 0u), firstRow, numberRows, 5u, 6u, 3u);
		}
		else
		{
			suppressNonMinimumSubset<TCoordinate, TStrength, tStrictMaximum, tOnlyNegative>(&strengthPositions, firstColumn, numberColumns, nullptr, positionCallback, firstRow, numberRows);
		}
	}

	return true;
}

template <typename T>
void NonMaximumSuppressionT<T>::reset()
{
	for (ptrdiff_t n = rows_.firstIndex(); n < rows_.endIndex(); ++n)
	{
		rows_[n].clear();
	}
}

template <typename T>
NonMaximumSuppressionT<T>& NonMaximumSuppressionT<T>::operator=(NonMaximumSuppressionT<T>&& nonMaximumSuppression)
{
	if (this != &nonMaximumSuppression)
	{
		width_ = nonMaximumSuppression.width_;
		nonMaximumSuppression.width_ = 0u;

		rows_ = std::move(nonMaximumSuppression.rows_);
	}

	return *this;
}

template <typename T>
NonMaximumSuppressionT<T>& NonMaximumSuppressionT<T>::operator=(const NonMaximumSuppressionT<T>& nonMaximumSuppression)
{
	if (this != &nonMaximumSuppression)
	{
		width_ = nonMaximumSuppression.width_;
		rows_ = nonMaximumSuppression.rows_;
	}

	return *this;
}

template <typename T>
template <typename TCoordinate, typename TStrength, bool tStrictMaximum>
typename NonMaximumSuppressionT<T>::template StrengthPositions<TCoordinate, TStrength> NonMaximumSuppressionT<T>::suppressNonMaximum(const unsigned int width, const unsigned int height, const StrengthPositions<TCoordinate, TStrength>& strengthPositions, const TCoordinate radius, Indices32* validIndices)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(radius >= TCoordinate(1));

	const unsigned int binSize = std::max(10u, (unsigned int)(NumericT<TCoordinate>::ceil(radius)));

	const unsigned int horizontalBins = std::max(1u, (width + binSize - 1u) / binSize);
	const unsigned int verticalBins = std::max(1u, (height + binSize - 1u) / binSize);

	ocean_assert(binSize * horizontalBins >= width);
	ocean_assert(binSize * verticalBins >= height);

	IndexGroups32 indexGroups(horizontalBins * verticalBins);

	// distributing all strength positions into a regular grid to reduce search space later

	for (size_t n = 0; n < strengthPositions.size(); ++n)
	{
		const VectorT2<TCoordinate>& position = strengthPositions[n];

		ocean_assert((unsigned int)(position.x()) < width);
		ocean_assert((unsigned int)(position.y()) < height);

		const unsigned int xBin = (unsigned int)(position.x()) / binSize;
		const unsigned int yBin = (unsigned int)(position.y()) / binSize;

		ocean_assert(xBin <= horizontalBins);
		ocean_assert(yBin <= verticalBins);

		indexGroups[yBin * horizontalBins + xBin].emplace_back(Index32(n));
	}

	std::vector<uint8_t> validPositions(strengthPositions.size(), 1u);

	const TCoordinate sqrRadius = radius * radius;

	for (size_t nCandidate = 0; nCandidate < strengthPositions.size(); ++nCandidate)
	{
		if (validPositions[nCandidate] == 0u)
		{
			// the positions is already suppressed
			continue;
		}

		const StrengthPosition<TCoordinate, TStrength>& candidatePosition = strengthPositions[nCandidate];

		const unsigned int xCandidateBin = (unsigned int)(candidatePosition.x()) / binSize;
		const unsigned int yCandidateBin = (unsigned int)(candidatePosition.y()) / binSize;

		ocean_assert(xCandidateBin <= horizontalBins);
		ocean_assert(yCandidateBin <= verticalBins);

		bool checkNextCandidate = false;

		for (unsigned int yBin = (unsigned int)(max(0, int(yCandidateBin) - 1)); !checkNextCandidate && yBin < min(yCandidateBin + 2u, verticalBins); ++yBin)
		{
			for (unsigned int xBin = (unsigned int)(max(0, int(xCandidateBin) - 1)); !checkNextCandidate && xBin < min(xCandidateBin + 2u, horizontalBins); ++xBin)
			{
				const Indices32& indices = indexGroups[yBin * horizontalBins + xBin];

				for (const Index32& nTest : indices)
				{
					if (nTest == Index32(nCandidate))
					{
						continue;
					}

					const StrengthPosition<TCoordinate, TStrength>& testPosition = strengthPositions[nTest];

					// we do not check whether test position is suppressed already (as the test position may still be the reason to suppress the candidate position)

					if (candidatePosition.sqrDistance(testPosition) <= sqrRadius)
					{
						if (candidatePosition.strength() > testPosition.strength())
						{
							validPositions[nTest] = 0u;
						}
						else if (candidatePosition.strength() < testPosition.strength())
						{
							validPositions[nCandidate] = 0u;

							checkNextCandidate = true;
							break;
						}
						else
						{
							ocean_assert(candidatePosition.strength() == testPosition.strength());

							if constexpr (tStrictMaximum)
							{
								// we suppress both elements, as we seek a strict maximum element

								validPositions[nCandidate] = 0u;
								validPositions[nTest] = 0u;

								checkNextCandidate = true;
								break;
							}
							else
							{
								// we will suppress one of both elements, as we accept a non-strict maximum element

								if (candidatePosition.y() < testPosition.y() || (candidatePosition.y() == testPosition.y() && candidatePosition.x() < testPosition.x()))
								{
									// the candidate position will be suppressed as the test position is located to the bottom/right of the candidate position

									validPositions[nCandidate] = 0u;

									checkNextCandidate = true;
									break;
								}
								else
								{
									ocean_assert(testPosition.y() < candidatePosition.y() || (testPosition.y() == candidatePosition.y() && testPosition.x() < candidatePosition.x()));

									// the test position will be suppressed as the candidate position is located to the bottom/right of the test position

									validPositions[nTest] = 0u;
								}
							}
						}
					}
				}
			}
		}
	}

	StrengthPositions<TCoordinate, TStrength> remainingPositions;
	remainingPositions.reserve(strengthPositions.size());

	if (validIndices)
	{
		ocean_assert(validIndices->empty());

		validIndices->clear();
		validIndices->reserve(strengthPositions.size());

		for (size_t n = 0; n < validPositions.size(); ++n)
		{
			if (validPositions[n])
			{
				remainingPositions.emplace_back(strengthPositions[n]);
				validIndices->emplace_back(Index32(n));
			}
		}
	}
	else
	{
		for (size_t n = 0; n < validPositions.size(); ++n)
		{
			if (validPositions[n])
			{
				remainingPositions.emplace_back(strengthPositions[n]);
			}
		}
	}

	return remainingPositions;
}

template <typename T>
template <typename TFloat>
bool NonMaximumSuppressionT<T>::determinePrecisePeakLocation1(const T& leftValue, const T& middleValue, const T& rightValue, TFloat& location)
{
	static_assert(std::is_floating_point<TFloat>::value, "Invalid floating point data type!");

	// f(x) = f(a) + f`(a) * (x - a)

	// we expect our middle value to be located at a = 0:
	// f(x) = f(0) + f`(0) * x

	// 0 = f'(x)
	//   = f'(0) + f''(0) * x

	// x = - f'(0) / f''(0)

	//  f`(x) = [-1 0 1] * 1/2
	const TFloat df = (TFloat(rightValue) - TFloat(leftValue)) * TFloat(0.5);

	// f``(x) = [1 -2 1] * 1/1
	const TFloat dff = TFloat(leftValue) + TFloat(rightValue) - TFloat(middleValue) * TFloat(2);

	if (NumericT<TFloat>::isEqualEps(dff))
	{
		location = TFloat(0);
		return true;
	}

	const TFloat x = -df / dff;

	if (x < TFloat(-1) || x > TFloat(1))
	{
		return false;
	}

	location = x;
	return true;
}

template <typename T>
template <typename TFloat>
bool NonMaximumSuppressionT<T>::determinePrecisePeakLocation2(const T* const topValues, const T* const centerValues, const T* const bottomValues, VectorT2<TFloat>& location)
{
	static_assert(std::is_floating_point<TFloat>::value, "Invalid floating point data type!");

	const T& value00 = topValues[0];
	const T& value01 = topValues[1];
	const T& value02 = topValues[2];

	const T& value10 = centerValues[0];
	const T& value11 = centerValues[1];
	const T& value12 = centerValues[2];

	const T& value20 = bottomValues[0];
	const T& value21 = bottomValues[1];
	const T& value22 = bottomValues[2];

#if 0
	// some response values may not perfectly follow the peak criteria so that we do not use the asserts by default
	ocean_assert(value11 >= value00 && value11 >= value01 && value11 >= value02);
	ocean_assert(value11 >= value10 && value11 >= value12);
	ocean_assert(value11 >= value20 && value11 >= value21 && value11 >= value22);
#endif

	// [-1 0 1] * 1/2
	const TFloat dx = TFloat(value12 - value10) * TFloat(0.5);
	const TFloat dy = TFloat(value21 - value01) * TFloat(0.5);

	// [1 -2 1] * 1/1
	const TFloat dxx = TFloat(value12 + value10) - TFloat(value11) * TFloat(2);
	const TFloat dyy = TFloat(value21 + value01) - TFloat(value11) * TFloat(2);

	// [ 1  0 -1 ]
	// [ 0  0  0 ] * 1/4
	// [-1  0  1 ]

	const TFloat dxy = TFloat(value22 + value00 - value20 - value02) * TFloat(0.25);

	const TFloat denominator = dxx * dyy - dxy * dxy;

	if (NumericT<TFloat>::isEqualEps(denominator))
	{
		location = VectorT2<TFloat>(0, 0);
		return true;
	}

	const TFloat factor = TFloat(1) / denominator;

	const TFloat offsetX = -(dyy * dx - dxy * dy) * factor;
	const TFloat offsetY = -(dxx * dy - dxy * dx) * factor;

	if (offsetX < TFloat(-1) || offsetX > TFloat(1) || offsetY < TFloat(-1) || offsetY > TFloat(1))
	{
		return false;
	}

	location = VectorT2<TFloat>(offsetX, offsetY);

	return true;
}

template <typename T>
template <typename TFloat, unsigned int tSize, bool tFindMaximum>
bool NonMaximumSuppressionT<T>::determinePrecisePeakLocationNxN(const TFloat* values, const size_t valuesStrideElements, TFloat& offsetX, TFloat& offsetY)
{
	static_assert(std::is_floating_point<TFloat>::value, "Invalid floating point data type!");
	static_assert(tSize >= 3u && tSize % 2u == 1u, "Grid size must be odd and >= 3");

	ocean_assert(valuesStrideElements >= tSize);

	// Least-squares fit of F(x,y) = a + bx + cy + dx^2 + exy + fy^2 to all NxN grid samples.
	//
	// Grid coordinates: x_i = gx - tSize_2, y_j = gy - tSize_2, for gx,gy in [0, tSize).
	// On a symmetric grid, cross-terms in the normal equations vanish, yielding closed-form coefficients:
	//
	//   gradient:  dx = Sxz / (n * S2),  dy = Syz / (n * S2)
	//   Hessian:   dxx = 2 * (Sxxz - S2 * Sz / n) / (n * S4 - S2^2)
	//              dyy = 2 * (Syyz - S2 * Sz / n) / (n * S4 - S2^2)
	//              dxy = Sxyz / S2^2
	//
	// where S2 = sum(i^2) and S4 = sum(i^4) over i in [-tSize_2, tSize_2].

	constexpr int tSize_2 = int(tSize / 2u);
	constexpr unsigned int n = tSize;

	// Precomputed grid sums for coordinates in [-tSize_2, tSize_2]:
	// sumOfSquaredCoordinates = sum(i^2) = tSize_2 * (tSize_2 + 1) * (2 * tSize_2 + 1) / 3
	// sumOfFourthPowerCoordinates = sum(i^4) = tSize_2 * (tSize_2 + 1) * (2 * tSize_2 + 1) * (3 * tSize_2^2 + 3 * tSize_2 - 1) / 15

	const TFloat sumOfSquaredCoordinates = TFloat(tSize_2 * (tSize_2 + 1) * (2 * tSize_2 + 1)) / TFloat(3);
	const TFloat sumOfFourthPowerCoordinates = TFloat(tSize_2 * (tSize_2 + 1) * (2 * tSize_2 + 1) * (3 * tSize_2 * tSize_2 + 3 * tSize_2 - 1)) / TFloat(15);

	const TFloat normalEquationGradientDenominator = TFloat(n) * sumOfSquaredCoordinates;
	const TFloat squaredSumOfSquaredCoordinates = sumOfSquaredCoordinates * sumOfSquaredCoordinates;
	const TFloat normalEquationHessianDenominator = TFloat(n) * sumOfFourthPowerCoordinates - squaredSumOfSquaredCoordinates;

	if (NumericT<TFloat>::isEqualEps(normalEquationGradientDenominator) || NumericT<TFloat>::isEqualEps(normalEquationHessianDenominator))
	{
		offsetX = TFloat(0);
		offsetY = TFloat(0);

		return true;
	}

	// Accumulate weighted sums in a single pass

	TFloat sumValues = TFloat(0);
	TFloat sumXValues = TFloat(0);
	TFloat sumYValues = TFloat(0);
	TFloat sumXXValues = TFloat(0);
	TFloat sumYYValues = TFloat(0);
	TFloat sumXYValues = TFloat(0);

	for (unsigned int yy = 0u; yy < tSize; ++yy)
	{
		const TFloat yj = TFloat(int(yy) - tSize_2);

		for (unsigned int xx = 0u; xx < tSize; ++xx)
		{
			const TFloat xi = TFloat(int(xx) - tSize_2);
			const TFloat z = values[yy * valuesStrideElements + xx];

			sumValues += z;
			sumXValues += xi * z;
			sumYValues += yj * z;
			sumXXValues += xi * xi * z;
			sumYYValues += yj * yj * z;
			sumXYValues += xi * yj * z;
		}
	}

	// Derivatives at the grid center from the least-squares quadratic fit

	const TFloat dx = sumXValues / normalEquationGradientDenominator;
	const TFloat dy = sumYValues / normalEquationGradientDenominator;
	const TFloat dxy = sumXYValues / squaredSumOfSquaredCoordinates;

	const TFloat meanValue = sumValues / TFloat(n);
	const TFloat dxx = TFloat(2) * (sumXXValues - sumOfSquaredCoordinates * meanValue) / normalEquationHessianDenominator;
	const TFloat dyy = TFloat(2) * (sumYYValues - sumOfSquaredCoordinates * meanValue) / normalEquationHessianDenominator;

	const TFloat hessianDeterminant = dxx * dyy - dxy * dxy;

	if (NumericT<TFloat>::isEqualEps(hessianDeterminant))
	{
		offsetX = TFloat(0);
		offsetY = TFloat(0);

		return true;
	}

	// For a maximum, the Hessian must be negative-definite (dxx < 0 and det > 0)
	// For a minimum, the Hessian must be positive-definite (dxx > 0 and det > 0)
	if constexpr (tFindMaximum)
	{
		if (dxx >= TFloat(0) || hessianDeterminant <= TFloat(0))
		{
			offsetX = TFloat(0);
			offsetY = TFloat(0);

			return false;
		}
	}
	else
	{
		if (dxx <= TFloat(0) || hessianDeterminant <= TFloat(0))
		{
			offsetX = TFloat(0);
			offsetY = TFloat(0);

			return false;
		}
	}

	const TFloat inverseHessianDeterminant = TFloat(1) / hessianDeterminant;

	offsetX = -(dyy * dx - dxy * dy) * inverseHessianDeterminant;
	offsetY = -(dxx * dy - dxy * dx) * inverseHessianDeterminant;

	if (offsetX < TFloat(-1) || offsetX > TFloat(1) || offsetY < TFloat(-1) || offsetY > TFloat(1))
	{
		return false;
	}

	return true;
}

template <typename T>
template <typename TFloat, unsigned int tSize, bool tFindMaximum, PixelCenter tPixelCenter>
NonMaximumSuppression::RefinementStatus NonMaximumSuppressionT<T>::determinePrecisePeakLocationIterativeNxN(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const TFloat x, const TFloat y, TFloat& preciseX, TFloat& preciseY, const unsigned int maxIterations, const TFloat stepSize, const TFloat convergenceThreshold)
{
	static_assert(std::is_floating_point<TFloat>::value, "Invalid floating point data type!");
	static_assert(tSize >= 3u && tSize % 2u == 1u, "Grid size must be odd and >= 3");

	ocean_assert(frame != nullptr);
	ocean_assert(width >= tSize && height >= tSize);
	ocean_assert(maxIterations >= 1u);
	ocean_assert(stepSize > TFloat(0) && stepSize <= TFloat(1));
	ocean_assert(convergenceThreshold > TFloat(0));

	constexpr unsigned int tSize_2 = tSize / 2u;

	const TFloat maxSampleX = (tPixelCenter == PC_TOP_LEFT) ? TFloat(width - 1u) : TFloat(width);
	const TFloat maxSampleY = (tPixelCenter == PC_TOP_LEFT) ? TFloat(height - 1u) : TFloat(height);

	ocean_assert(x >= TFloat(0) && x <= maxSampleX);
	ocean_assert(y >= TFloat(0) && y <= maxSampleY);

	if (x < TFloat(0) || x > maxSampleX || y < TFloat(0) || y > maxSampleY)
	{
		return NonMaximumSuppression::RS_INVALID;
	}

	TFloat iterationX = x;
	TFloat iterationY = y;
	TFloat stepFactor = stepSize;

	const TFloat sqrConvergenceThreshold = convergenceThreshold * convergenceThreshold;

	for (unsigned int iteration = 0u; iteration < maxIterations; ++iteration)
	{
		if (iterationX - TFloat(tSize_2) < TFloat(0) || iterationX + TFloat(tSize_2) > maxSampleX || iterationY - TFloat(tSize_2) < TFloat(0) || iterationY + TFloat(tSize_2) > maxSampleY)
		{
			preciseX = iterationX;
			preciseY = iterationY;

			return NonMaximumSuppression::RS_BORDER;
		}

		TFloat interpolatedValues[tSize * tSize];

		for (unsigned int yy = 0u; yy < tSize; ++yy)
		{
			for (unsigned int xx = 0u; xx < tSize; ++xx)
			{
				const TFloat sampleX = iterationX + TFloat(int(xx) - int(tSize_2));
				const TFloat sampleY = iterationY + TFloat(int(yy) - int(tSize_2));

				ocean_assert(sampleX >= TFloat(0) && sampleX <= maxSampleX);
				ocean_assert(sampleY >= TFloat(0) && sampleY <= maxSampleY);

				const VectorT2<TFloat> samplePos(sampleX, sampleY);

				FrameInterpolatorBilinear::interpolatePixel<uint8_t, TFloat, 1u, tPixelCenter, TFloat>(frame, width, height, framePaddingElements, samplePos, &interpolatedValues[yy * tSize + xx]);
			}
		}

		TFloat fitOffsetX;
		TFloat fitOffsetY;

		if (!determinePrecisePeakLocationNxN<TFloat, tSize, tFindMaximum>(interpolatedValues, tSize, fitOffsetX, fitOffsetY))
		{
			preciseX = iterationX;
			preciseY = iterationY;

			return NonMaximumSuppression::RS_DIVERGED;
		}

		if (fitOffsetX == TFloat(0) && fitOffsetY == TFloat(0))
		{
			preciseX = iterationX;
			preciseY = iterationY;

			return NonMaximumSuppression::RS_CONVERGED;
		}

		const TFloat pixelOffsetX = fitOffsetX * stepFactor;
		const TFloat pixelOffsetY = fitOffsetY * stepFactor;

		if (NumericT<TFloat>::abs(pixelOffsetX) > stepFactor || NumericT<TFloat>::abs(pixelOffsetY) > stepFactor)
		{
			preciseX = iterationX;
			preciseY = iterationY;

			return NonMaximumSuppression::RS_DIVERGED;
		}

		iterationX += pixelOffsetX;
		iterationY += pixelOffsetY;

		stepFactor *= stepSize;

		if (pixelOffsetX * pixelOffsetX + pixelOffsetY * pixelOffsetY < sqrConvergenceThreshold)
		{
			preciseX = iterationX;
			preciseY = iterationY;

			return NonMaximumSuppression::RS_CONVERGED;
		}
	}

	preciseX = iterationX;
	preciseY = iterationY;

	return NonMaximumSuppression::RS_MAX_ITERATIONS;
}

template <typename T>
void NonMaximumSuppressionT<T>::addCandidatesSubset(const T* values, const unsigned int valuesStrideElements, const unsigned int firstColumn, const unsigned int numberColumns, const T* minimalThreshold, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(values != nullptr);
	ocean_assert(valuesStrideElements >= width_);
	ocean_assert(firstColumn + numberColumns <= width_);

	ocean_assert(typename StrengthCandidateRows::Index(firstRow) >= rows_.firstIndex());
	ocean_assert(typename StrengthCandidateRows::Index(firstRow + numberRows) <= rows_.endIndex());

	const T localThreshold = *minimalThreshold;

	values += firstRow * valuesStrideElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if (values[x] >= localThreshold)
			{
				addCandidate(x, y, values[x]);
			}
		}

		values += valuesStrideElements;
	}
}

template <typename T>
template <typename TCoordinate, typename TStrength, bool tStrictMaximum, bool tOnlyPositive>
void NonMaximumSuppressionT<T>::suppressNonMaximumSubset(StrengthPositions<TCoordinate, TStrength>* strengthPositions, const unsigned int firstColumn, const unsigned int numberColumns, Lock* lock, const PositionCallback<TCoordinate, TStrength>* positionCallback, const unsigned int firstRow, const unsigned int numberRows) const
{
	ocean_assert(strengthPositions);

	ocean_assert(firstColumn + numberColumns <= width_);
	ocean_assert(firstRow >= (unsigned int)(rows_.firstIndex()));
	ocean_assert(firstRow + numberRows <= (unsigned int)rows_.endIndex());

	if (numberColumns < 3u || numberRows < 3u)
	{
		return;
	}

	const unsigned int firstCenterColumn = max(1u, firstColumn);
	const unsigned int endCenterColumn = min(firstColumn + numberColumns, width_ - 1u);

	const unsigned int firstCenterRow = max((unsigned int)rows_.firstIndex() + 1u, firstRow);
	const unsigned int endCenterRow = min(firstRow + numberRows, (unsigned int)rows_.lastIndex());

	ocean_assert(firstCenterRow >= 1u);

	StrengthPositions<TCoordinate, TStrength> localStrengthPositions;
	localStrengthPositions.reserve(100);

	for (unsigned int y = firstCenterRow; y < endCenterRow; ++y)
	{
		const StrengthCandidateRow& row0 = rows_[y - 1u];
		const StrengthCandidateRow& row1 = rows_[y + 0u];
		const StrengthCandidateRow& row2 = rows_[y + 1u];

		typename StrengthCandidateRow::const_iterator iRow0 = row0.begin();
		typename StrengthCandidateRow::const_iterator iRow2 = row2.begin();

		typename StrengthCandidateRow::const_iterator iRow1Minus = row1.end();
		typename StrengthCandidateRow::const_iterator iRow1Plus = row1.size() > 1 ? row1.begin() + 1 : row1.end();

		for (typename StrengthCandidateRow::const_iterator iRow1 = row1.begin(); iRow1 != row1.end(); ++iRow1)
		{
			ocean_assert(iRow1->x() >= 0u && iRow1->x() + 1u <= width_);

			// check value constraint if enabled
			if constexpr (tOnlyPositive)
			{
				if (iRow1->strength() <= T(0))
				{
					goto next;
				}
			}

			// check left candidate (west)
			if (iRow1->x() >= firstCenterColumn && iRow1->x() < endCenterColumn && (iRow1Minus == row1.end() || iRow1Minus->x() + 1u != iRow1->x() || (tStrictMaximum && iRow1Minus->strength() < iRow1->strength()) || (!tStrictMaximum && iRow1Minus->strength() <= iRow1->strength())))
			{
				// check right candidate (east)
				if (iRow1Plus == row1.end() || iRow1Plus->x() != iRow1->x() + 1u || iRow1Plus->strength() < iRow1->strength())
				{
					// set the top row iterator to the right position
					while (iRow0 != row0.end())
					{
						if (iRow0->x() + 1u < iRow1->x())
						{
							++iRow0;
						}
						else
						{
							break;
						}
					}

					// now iRow0 should point at least to the north west pixel position (or more far east)
					ocean_assert(iRow0 == row0.end() || iRow0->x() + 1u >= iRow1->x());

					if (iRow0 != row0.end() && iRow0->x() <= iRow1->x() + 1u)
					{
						ocean_assert(iRow0->x() + 1u == iRow1->x() || iRow0->x() == iRow1->x() || iRow0->x() - 1u == iRow1->x());

						if ((tStrictMaximum && iRow0->strength() >= iRow1->strength()) || (!tStrictMaximum && iRow0->strength() > iRow1->strength()))
						{
							goto next;
						}

						// check if there is a further candidate in the north row

						const typename StrengthCandidateRow::const_iterator iRow0Plus = iRow0 + 1;

						if (iRow0Plus != row0.end() && iRow0Plus->x() <= iRow1->x() + 1u)
						{
							if ((tStrictMaximum && iRow0Plus->strength() >= iRow1->strength()) || (!tStrictMaximum && iRow0Plus->strength() > iRow1->strength()))
							{
								goto next;
							}

							// check if there is a further candidate in the north row

							const typename StrengthCandidateRow::const_iterator iRow0PlusPlus = iRow0Plus + 1;

							if (iRow0PlusPlus != row0.end() && iRow0PlusPlus->x() <= iRow1->x() + 1u)
							{
								ocean_assert(iRow0PlusPlus->x() == iRow1->x() + 1u);

								if ((tStrictMaximum && iRow0PlusPlus->strength() >= iRow1->strength()) || (!tStrictMaximum && iRow0PlusPlus->strength() > iRow1->strength()))
								{
									goto next;
								}
							}
						}
					}


					// set the bottom row iterator to the right position
					while (iRow2 != row2.end())
					{
						if (iRow2->x() + 1u < iRow1->x())
						{
							++iRow2;
						}
						else
						{
							break;
						}
					}

					// now iRow2 should point at least to the south west pixel position (or more far east)
					ocean_assert(iRow2 == row2.end() || iRow2->x() + 1u >= iRow1->x());

					if (iRow2 != row2.end() && iRow2->x() <= iRow1->x() + 1u)
					{
						ocean_assert(iRow2->x() + 1u == iRow1->x() || iRow2->x() == iRow1->x() || iRow2->x() - 1u == iRow1->x());

						if (iRow2->x() + 1u == iRow1->x())
						{
							// iRow2 points to the south west pixel

							if ((tStrictMaximum && iRow2->strength() >= iRow1->strength()) || (!tStrictMaximum && iRow2->strength() > iRow1->strength()))
							{
								goto next;
							}
						}
						else
						{
							if (iRow2->strength() >= iRow1->strength())
							{
								goto next;
							}
						}

						// check if there is a further candidate in the south row

						const typename StrengthCandidateRow::const_iterator iRow2Plus = iRow2 + 1;

						if (iRow2Plus != row2.end() && iRow2Plus->x() <= iRow1->x() + 1u)
						{
							if (iRow2Plus->strength() >= iRow1->strength())
							{
								goto next;
							}

							// check if there is a further candidate in the south row

							const typename StrengthCandidateRow::const_iterator iRow2PlusPlus = iRow2Plus + 1;

							if (iRow2PlusPlus != row2.end() && iRow2PlusPlus->x() <= iRow1->x() + 1u)
							{
								ocean_assert(iRow2PlusPlus->x() == iRow1->x() + 1u);

								if (iRow2PlusPlus->strength() >= iRow1->strength())
								{
									goto next;
								}
							}
						}
					}

					if (positionCallback != nullptr)
					{
						TCoordinate preciseX;
						TCoordinate preciseY;
						TStrength preciseStrength;

						if ((*positionCallback)(iRow1->x(), y, iRow1->strength(), preciseX, preciseY, preciseStrength))
						{
							localStrengthPositions.emplace_back(preciseX, preciseY, preciseStrength);
						}
					}
					else
					{
						localStrengthPositions.emplace_back(TCoordinate(iRow1->x()), TCoordinate(y), iRow1->strength());
					}
				}
			}

next:

			iRow1Minus = iRow1;

			if (iRow1Plus != row1.end())
			{
				++iRow1Plus;
			}
		}
	}

	const OptionalScopedLock scopedLock(lock);

	strengthPositions->insert(strengthPositions->end(), localStrengthPositions.begin(), localStrengthPositions.end());
}

template <typename T>
template <typename TCoordinate, typename TStrength, bool tStrictMaximum, bool tOnlyNegative>
void NonMaximumSuppressionT<T>::suppressNonMinimumSubset(StrengthPositions<TCoordinate, TStrength>* strengthPositions, const unsigned int firstColumn, const unsigned int numberColumns, Lock* lock, const PositionCallback<TCoordinate, TStrength>* positionCallback, const unsigned int firstRow, const unsigned int numberRows) const
{
	ocean_assert(strengthPositions);

	ocean_assert(firstColumn + numberColumns <= width_);
	ocean_assert(firstRow >= (unsigned int)(rows_.firstIndex()));
	ocean_assert(firstRow + numberRows <= (unsigned int)rows_.endIndex());

	if (numberColumns < 3u || numberRows < 3u)
	{
		return;
	}

	const unsigned int firstCenterColumn = max(1u, firstColumn);
	const unsigned int endCenterColumn = min(firstColumn + numberColumns, width_ - 1u);

	const unsigned int firstCenterRow = max((unsigned int)rows_.firstIndex() + 1u, firstRow);
	const unsigned int endCenterRow = min(firstRow + numberRows, (unsigned int)rows_.lastIndex());

	ocean_assert(firstCenterRow >= 1u);

	StrengthPositions<TCoordinate, TStrength> localStrengthPositions;
	localStrengthPositions.reserve(100);

	for (unsigned int y = firstCenterRow; y < endCenterRow; ++y)
	{
		const StrengthCandidateRow& row0 = rows_[y - 1u];
		const StrengthCandidateRow& row1 = rows_[y + 0u];
		const StrengthCandidateRow& row2 = rows_[y + 1u];

		typename StrengthCandidateRow::const_iterator iRow0 = row0.begin();
		typename StrengthCandidateRow::const_iterator iRow2 = row2.begin();

		typename StrengthCandidateRow::const_iterator iRow1Minus = row1.end();
		typename StrengthCandidateRow::const_iterator iRow1Plus = row1.size() > 1 ? row1.begin() + 1 : row1.end();

		for (typename StrengthCandidateRow::const_iterator iRow1 = row1.begin(); iRow1 != row1.end(); ++iRow1)
		{
			ocean_assert(iRow1->x() >= 0u && iRow1->x() + 1u <= width_);

			// check value constraint if enabled
			if constexpr (tOnlyNegative)
			{
				if (iRow1->strength() >= T(0))
				{
					goto nextMinimum;
				}
			}

			// check left candidate (west)
			if (iRow1->x() >= firstCenterColumn && iRow1->x() < endCenterColumn && (iRow1Minus == row1.end() || iRow1Minus->x() + 1u != iRow1->x() || (tStrictMaximum && iRow1Minus->strength() > iRow1->strength()) || (!tStrictMaximum && iRow1Minus->strength() >= iRow1->strength())))
			{
				// check right candidate (east)
				if (iRow1Plus == row1.end() || iRow1Plus->x() != iRow1->x() + 1u || iRow1Plus->strength() > iRow1->strength())
				{
					// set the top row iterator to the right position
					while (iRow0 != row0.end())
					{
						if (iRow0->x() + 1u < iRow1->x())
						{
							++iRow0;
						}
						else
						{
							break;
						}
					}

					// now iRow0 should point at least to the north west pixel position (or more far east)
					ocean_assert(iRow0 == row0.end() || iRow0->x() + 1u >= iRow1->x());

					if (iRow0 != row0.end() && iRow0->x() <= iRow1->x() + 1u)
					{
						ocean_assert(iRow0->x() + 1u == iRow1->x() || iRow0->x() == iRow1->x() || iRow0->x() - 1u == iRow1->x());

						if ((tStrictMaximum && iRow0->strength() <= iRow1->strength()) || (!tStrictMaximum && iRow0->strength() < iRow1->strength()))
						{
							goto nextMinimum;
						}

						// check if there is a further candidate in the north row

						const typename StrengthCandidateRow::const_iterator iRow0Plus = iRow0 + 1;

						if (iRow0Plus != row0.end() && iRow0Plus->x() <= iRow1->x() + 1u)
						{
							if ((tStrictMaximum && iRow0Plus->strength() <= iRow1->strength()) || (!tStrictMaximum && iRow0Plus->strength() < iRow1->strength()))
							{
								goto nextMinimum;
							}

							// check if there is a further candidate in the north row

							const typename StrengthCandidateRow::const_iterator iRow0PlusPlus = iRow0Plus + 1;

							if (iRow0PlusPlus != row0.end() && iRow0PlusPlus->x() <= iRow1->x() + 1u)
							{
								ocean_assert(iRow0PlusPlus->x() == iRow1->x() + 1u);

								if ((tStrictMaximum && iRow0PlusPlus->strength() <= iRow1->strength()) || (!tStrictMaximum && iRow0PlusPlus->strength() < iRow1->strength()))
								{
									goto nextMinimum;
								}
							}
						}
					}


					// set the bottom row iterator to the right position
					while (iRow2 != row2.end())
					{
						if (iRow2->x() + 1u < iRow1->x())
						{
							++iRow2;
						}
						else
						{
							break;
						}
					}

					// now iRow2 should point at least to the south west pixel position (or more far east)
					ocean_assert(iRow2 == row2.end() || iRow2->x() + 1u >= iRow1->x());

					if (iRow2 != row2.end() && iRow2->x() <= iRow1->x() + 1u)
					{
						ocean_assert(iRow2->x() + 1u == iRow1->x() || iRow2->x() == iRow1->x() || iRow2->x() - 1u == iRow1->x());

						if (iRow2->x() + 1u == iRow1->x())
						{
							// iRow2 points to the south west pixel

							if ((tStrictMaximum && iRow2->strength() <= iRow1->strength()) || (!tStrictMaximum && iRow2->strength() < iRow1->strength()))
							{
								goto nextMinimum;
							}
						}
						else
						{
							if (iRow2->strength() <= iRow1->strength())
							{
								goto nextMinimum;
							}
						}

						// check if there is a further candidate in the south row

						const typename StrengthCandidateRow::const_iterator iRow2Plus = iRow2 + 1;

						if (iRow2Plus != row2.end() && iRow2Plus->x() <= iRow1->x() + 1u)
						{
							if (iRow2Plus->strength() <= iRow1->strength())
							{
								goto nextMinimum;
							}

							// check if there is a further candidate in the south row

							const typename StrengthCandidateRow::const_iterator iRow2PlusPlus = iRow2Plus + 1;

							if (iRow2PlusPlus != row2.end() && iRow2PlusPlus->x() <= iRow1->x() + 1u)
							{
								ocean_assert(iRow2PlusPlus->x() == iRow1->x() + 1u);

								if (iRow2PlusPlus->strength() <= iRow1->strength())
								{
									goto nextMinimum;
								}
							}
						}
					}

					if (positionCallback != nullptr)
					{
						TCoordinate preciseX;
						TCoordinate preciseY;
						TStrength preciseStrength;

						if ((*positionCallback)(iRow1->x(), y, iRow1->strength(), preciseX, preciseY, preciseStrength))
						{
							localStrengthPositions.emplace_back(preciseX, preciseY, preciseStrength);
						}
					}
					else
					{
						localStrengthPositions.emplace_back(TCoordinate(iRow1->x()), TCoordinate(y), iRow1->strength());
					}
				}
			}

nextMinimum:

			iRow1Minus = iRow1;

			if (iRow1Plus != row1.end())
			{
				++iRow1Plus;
			}
		}
	}

	const OptionalScopedLock scopedLock(lock);

	strengthPositions->insert(strengthPositions->end(), localStrengthPositions.begin(), localStrengthPositions.end());
}

}

}

#endif // META_OCEAN_CV_NON_MAXIMUM_SUPPRESSION_H
