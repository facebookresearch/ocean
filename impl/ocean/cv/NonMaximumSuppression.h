/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_NON_MAXIMUM_SUPPRESSION_H
#define META_OCEAN_CV_NON_MAXIMUM_SUPPRESSION_H

#include "ocean/cv/CV.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/base/Callback.h"
#include "ocean/base/ShiftVector.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

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
class NonMaximumSuppression
{
	public:

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

		/**
		 * Definition of a callback function used to determine the precise sub-pixel position of a specific point.
		 * The first parameter provides the horizontal position.<br>
		 * The second parameter provides the vertical position.<br>
		 * The third parameter provides the strength value.<br>
		 * The fourth parameter receives the precise horizontal position.<br>
		 * The fifth parameter receives the precise vertical position.<br>
		 * The sixth parameter receives the precise strength value.<br>
		 * The return parameter should be True if the precise position could be determined
		 */
		template <typename TCoordinate, typename TStrength>
		using PositionCallback = Callback<bool, const unsigned int, const unsigned int, const T, TCoordinate&, TCoordinate&, TStrength&>;

	private:

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
		typedef std::vector<StrengthCandidate> StrengthCandidateRow;

		/**
		 * Definition of a vector holding a vector of strength candidates.
		 */
		typedef ShiftVector<StrengthCandidateRow> StrengthCandidateRows;

	public:

		/**
		 * Move constructor.
		 * @param nonMaximumSuppression The object to be moved
		 */
		NonMaximumSuppression(NonMaximumSuppression<T>&& nonMaximumSuppression) noexcept;

		/**
		 * Copy constructor.
		 * @param nonMaximumSuppression The object to be moved
		 */
		NonMaximumSuppression(const NonMaximumSuppression<T>& nonMaximumSuppression) noexcept;

		/**
		 * Creates a new maximum suppression object with a predefined size.
		 * @param width The width of this object in pixel, with range [3, infinity)
		 * @param height The height of this object in pixel, with range [3, infinity)
		 * @param yOffset Optional offset in the vertical direction moving the suppression region by the specified number of rows, with range [0, infinity)
		 */
		NonMaximumSuppression(const unsigned int width, const unsigned int height, const unsigned int yOffset = 0u) noexcept;

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
		 * Applies a non-maximum-suppression search on a given 2D frame in a 3x3 neighborhood (eight neighbors).
		 * This function allows to determine the precise position of the individual maximum value positions by application of a callback function determining the individual positions.<br>
		 * @param firstColumn First column to be handled, with range [1, width() - 1)
		 * @param numberColumns Number of columns to be handled
		 * @param firstRow First row to be handled, with range [yOffset() + 1, height() - 1)
		 * @param numberRows Number of rows to be handled
		 * @param worker Optional worker object to distribute the computation
		 * @param positionCallback Optional callback function allowing to determine the precise position of the individual maximum value positions
		 * @return Resulting non maximum suppressed positions including the strength parameters
		 * @tparam TCoordinate The data type of a scalar coordinate
		 * @tparam TStrength The data type of the strength parameter
		 * @tparam tStrictMaximum True, to search for a strict maximum (larger than all eight neighbors); False, to allow equal values in the upper left neighborhood
		 */
		template <typename TCoordinate, typename TStrength, bool tStrictMaximum = true>
		StrengthPositions<TCoordinate, TStrength> suppressNonMaximum(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows, Worker* worker = nullptr, const PositionCallback<TCoordinate, TStrength>* positionCallback = nullptr) const;

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
		NonMaximumSuppression<T>& operator=(NonMaximumSuppression<T>&& nonMaximumSuppression);

		/**
		 * Copy operator.
		 * @param nonMaximumSuppression The object to be copied
		 * @return Reference to this object
		 */
		NonMaximumSuppression<T>& operator=(const NonMaximumSuppression<T>& nonMaximumSuppression);

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
		 * @param lock The lock object that must be defined if this function is executed in parallel on several threads
		 * @param positionCallback Optional callback function allowing to determine the precise position of the individual maximum value positions
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tStrictMaximum True, to search for a strict maximum (larger than all eight neighbors); False, to allow equal values in the upper left neighborhood
		 */
		template <typename TCoordinate, typename TStrength, bool tStrictMaximum>
		void suppressNonMaximumSubset(StrengthPositions<TCoordinate, TStrength>* strengthPositions, const unsigned int firstColumn, const unsigned int firstRow, Lock* lock, const PositionCallback<TCoordinate, TStrength>* positionCallback, const unsigned int numberColumns, const unsigned int numberRows) const;

	private:

		/// Width of this object.
		unsigned int width_ = 0u;

		/// All candidate rows.
		StrengthCandidateRows rows_;
};

template <typename T>
template <typename TCoordinate, typename TStrength>
inline NonMaximumSuppression<T>::StrengthPosition<TCoordinate, TStrength>::StrengthPosition(const TCoordinate x, const TCoordinate y, const TStrength& strength) :
	VectorT2<TCoordinate>(x, y),
	strength_(strength)
{
	// nothing to do here
}

template <typename T>
template <typename TCoordinate, typename TStrength>
inline const TStrength& NonMaximumSuppression<T>::StrengthPosition<TCoordinate, TStrength>::strength() const
{
	return strength_;
}

template <typename T>
template <typename TCoordinate, typename TStrength>
template <bool tLeftLargerThanRight>
inline bool NonMaximumSuppression<T>::StrengthPosition<TCoordinate, TStrength>::compareStrength(const StrengthPosition<TCoordinate, TStrength>& left, const StrengthPosition<TCoordinate, TStrength>& right)
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
inline NonMaximumSuppression<T>::StrengthCandidate::StrengthCandidate() :
	positionX_(-1),
	strength_(T())
{
	// nothing to do here
}

template <typename T>
inline NonMaximumSuppression<T>::StrengthCandidate::StrengthCandidate(const unsigned int x, const T& strength) :
	positionX_(x),
	strength_(strength)
{
	// nothing to do here
}

template <typename T>
inline unsigned int NonMaximumSuppression<T>::StrengthCandidate::x() const
{
	return positionX_;
}

template <typename T>
inline const T& NonMaximumSuppression<T>::StrengthCandidate::strength() const
{
	return strength_;
}

template <typename T>
NonMaximumSuppression<T>::NonMaximumSuppression(NonMaximumSuppression<T>&& nonMaximumSuppression) noexcept
{
	*this = std::move(nonMaximumSuppression);
}

template <typename T>
NonMaximumSuppression<T>::NonMaximumSuppression(const NonMaximumSuppression<T>& nonMaximumSuppression) noexcept :
	width_(nonMaximumSuppression.width_),
	rows_(nonMaximumSuppression.rows_)
{
	// nothing to do here
}

template <typename T>
NonMaximumSuppression<T>::NonMaximumSuppression(const unsigned int width, const unsigned int height, const unsigned int yOffset) noexcept :
	width_(width),
	rows_(yOffset, height, StrengthCandidateRow())
{
	// nothing to do here
}

template <typename T>
inline unsigned int NonMaximumSuppression<T>::width() const
{
	return width_;
}

template <typename T>
inline unsigned int NonMaximumSuppression<T>::height() const
{
	return (unsigned int)rows_.size();
}

template <typename T>
inline unsigned int NonMaximumSuppression<T>::yOffset() const
{
	ocean_assert(rows_.firstIndex() >= 0);

	return (unsigned int)(rows_.firstIndex());
}

template <typename T>
inline void NonMaximumSuppression<T>::addCandidate(const unsigned int x, const unsigned int y, const T& strength)
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
void NonMaximumSuppression<T>::addCandidates(const T* values, const unsigned int valuesPaddingElements, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows, const T& minimalThreshold, Worker* worker)
{
	ocean_assert(values != nullptr);

	ocean_assert(firstColumn + numberColumns <= width_);
	ocean_assert(typename StrengthCandidateRows::Index(firstRow) >= rows_.firstIndex());
	ocean_assert(typename StrengthCandidateRows::Index(firstRow + numberRows) <= rows_.endIndex());

	const unsigned int valuesStrideElements = width_ + valuesPaddingElements;

	if (worker)
	{
		worker->executeFunction(Worker::Function::create(*this, &NonMaximumSuppression<T>::addCandidatesSubset, values, valuesStrideElements, firstColumn, numberColumns, &minimalThreshold, 0u, 0u), firstRow, numberRows, 5u, 6u, 20u);
	}
	else
	{
		addCandidatesSubset(values, valuesStrideElements, firstColumn, numberColumns, &minimalThreshold, firstRow, numberRows);
	}
}

template <typename T>
inline void NonMaximumSuppression<T>::removeCandidatesRightFrom(const unsigned int x, const unsigned int y)
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
template <typename TCoordinate, typename TStrength, bool tStrictMaximum>
typename NonMaximumSuppression<T>::template StrengthPositions<TCoordinate, TStrength> NonMaximumSuppression<T>::suppressNonMaximum(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows, Worker* worker, const PositionCallback<TCoordinate, TStrength>* positionCallback) const
{
	ocean_assert(firstColumn + numberColumns <= width_);
	ocean_assert(firstRow >= (unsigned int)rows_.firstIndex() && firstRow + numberRows <= (unsigned int)rows_.endIndex());

	StrengthPositions<TCoordinate, TStrength> result;
	result.reserve(100);

	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::create(*this, &NonMaximumSuppression<T>::suppressNonMaximumSubset<TCoordinate, TStrength, tStrictMaximum>, &result, firstColumn, numberColumns, &lock, positionCallback, 0u, 0u), firstRow, numberRows, 5u, 6u, 3u);
	}
	else
	{
		suppressNonMaximumSubset<TCoordinate, TStrength, tStrictMaximum>(&result, firstColumn, numberColumns, nullptr, positionCallback, firstRow, numberRows);
	}

	return result;
}

template <typename T>
void NonMaximumSuppression<T>::reset()
{
	for (ptrdiff_t n = rows_.firstIndex(); n < rows_.endIndex(); ++n)
	{
		rows_[n].clear();
	}
}

template <typename T>
NonMaximumSuppression<T>& NonMaximumSuppression<T>::operator=(NonMaximumSuppression<T>&& nonMaximumSuppression)
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
NonMaximumSuppression<T>& NonMaximumSuppression<T>::operator=(const NonMaximumSuppression<T>& nonMaximumSuppression)
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
typename NonMaximumSuppression<T>::template StrengthPositions<TCoordinate, TStrength> NonMaximumSuppression<T>::suppressNonMaximum(const unsigned int width, const unsigned int height, const StrengthPositions<TCoordinate, TStrength>& strengthPositions, const TCoordinate radius, Indices32* validIndices)
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
bool NonMaximumSuppression<T>::determinePrecisePeakLocation1(const T& leftValue, const T& middleValue, const T& rightValue, TFloat& location)
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
bool NonMaximumSuppression<T>::determinePrecisePeakLocation2(const T* const topValues, const T* const centerValues, const T* const bottomValues, VectorT2<TFloat>& location)
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
	const TFloat dxx = TFloat(value12 + value10 - value11 * TFloat(2));
	const TFloat dyy = TFloat(value21 + value01 - value11 * TFloat(2));

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
void NonMaximumSuppression<T>::addCandidatesSubset(const T* values, const unsigned int valuesStrideElements, const unsigned int firstColumn, const unsigned int numberColumns, const T* minimalThreshold, const unsigned int firstRow, const unsigned int numberRows)
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
template <typename TCoordinate, typename TStrength, bool tStrictMaximum>
void NonMaximumSuppression<T>::suppressNonMaximumSubset(StrengthPositions<TCoordinate, TStrength>* strengthPositions, const unsigned int firstColumn, const unsigned int numberColumns, Lock* lock, const PositionCallback<TCoordinate, TStrength>* positionCallback, const unsigned int firstRow, const unsigned int numberRows) const
{
	ocean_assert(strengthPositions);

	ocean_assert(firstColumn + numberColumns <= width_);
	ocean_assert(firstRow >= (unsigned int)rows_.firstIndex());
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

		typename StrengthCandidateRow::const_iterator i0 = row0.begin();
		typename StrengthCandidateRow::const_iterator i2 = row2.begin();

		typename StrengthCandidateRow::const_iterator i1Minus = row1.end();
		typename StrengthCandidateRow::const_iterator i1Plus = row1.size() > 1 ? row1.begin() + 1 : row1.end();

		for (typename StrengthCandidateRow::const_iterator i1 = row1.begin(); i1 != row1.end(); ++i1)
		{
			ocean_assert(i1->x() >= 0u && i1->x() + 1u <= width_);

			// check left candidate (west)
			if (i1->x() >= firstCenterColumn && i1->x() < endCenterColumn && (i1Minus == row1.end() || i1Minus->x() + 1u != i1->x() || (tStrictMaximum && i1Minus->strength() < i1->strength()) || (!tStrictMaximum && i1Minus->strength() <= i1->strength())))
			{
				// check right candidate (east)
				if (i1Plus == row1.end() || i1Plus->x() != i1->x() + 1u || i1Plus->strength() < i1->strength())
				{
					// set the top row iterator to the right position
					while (i0 != row0.end())
					{
						if (i0->x() + 1u < i1->x())
						{
							++i0;
						}
						else
						{
							break;
						}
					}

					// now i0 should point at least to the north west pixel position (or more far east)
					ocean_assert(i0 == row0.end() || i0->x() + 1u >= i1->x());

					if (i0 != row0.end() && i0->x() <= i1->x() + 1u)
					{
						ocean_assert(i0->x() + 1u == i1->x() || i0->x() == i1->x() || i0->x() - 1u == i1->x());

						if ((tStrictMaximum && i0->strength() >= i1->strength()) || (!tStrictMaximum && i0->strength() > i1->strength()))
						{
							goto next;
						}

						// check if there is a further candidate in the north row

						const typename StrengthCandidateRow::const_iterator i0Plus = i0 + 1;

						if (i0Plus != row0.end() && i0Plus->x() <= i1->x() + 1u)
						{
							if ((tStrictMaximum && i0Plus->strength() >= i1->strength()) || (!tStrictMaximum && i0Plus->strength() > i1->strength()))
							{
								goto next;
							}

							// check if there is a further candidate in the north row

							const typename StrengthCandidateRow::const_iterator i0PlusPlus = i0Plus + 1;

							if (i0PlusPlus != row0.end() && i0PlusPlus->x() <= i1->x() + 1u)
							{
								ocean_assert(i0PlusPlus->x() == i1->x() + 1u);

								if ((tStrictMaximum && i0PlusPlus->strength() >= i1->strength()) || (!tStrictMaximum && i0PlusPlus->strength() > i1->strength()))
								{
									goto next;
								}
							}
						}
					}


					// set the bottom row iterator to the right position
					while (i2 != row2.end())
					{
						if (i2->x() + 1u < i1->x())
						{
							++i2;
						}
						else
						{
							break;
						}
					}

					// now i2 should point at least to the south west pixel position (or more far east)
					ocean_assert(i2 == row2.end() || i2->x() + 1u >= i1->x());

					if (i2 != row2.end() && i2->x() <= i1->x() + 1u)
					{
						ocean_assert(i2->x() + 1u == i1->x() || i2->x() == i1->x() || i2->x() - 1u == i1->x());

						if (i2->x() + 1u == i1->x())
						{
							// i2 points to the south west pixel

							if ((tStrictMaximum && i2->strength() >= i1->strength()) || (!tStrictMaximum && i2->strength() > i1->strength()))
							{
								goto next;
							}
						}
						else
						{
							if (i2->strength() >= i1->strength())
							{
								goto next;
							}
						}

						// check if there is a further candidate in the south row

						const typename StrengthCandidateRow::const_iterator i2Plus = i2 + 1;

						if (i2Plus != row2.end() && i2Plus->x() <= i1->x() + 1u)
						{
							if (i2Plus->strength() >= i1->strength())
								goto next;

							// check if there is a further candidate in the south row

							const typename StrengthCandidateRow::const_iterator i2PlusPlus = i2Plus + 1;

							if (i2PlusPlus != row2.end() && i2PlusPlus->x() <= i1->x() + 1u)
							{
								ocean_assert(i2PlusPlus->x() == i1->x() + 1u);

								if (i2PlusPlus->strength() >= i1->strength())
									goto next;
							}
						}
					}

					if (positionCallback)
					{
						TCoordinate preciseX, preciseY;
						TStrength preciseStrength;
						if ((*positionCallback)(i1->x(), y, i1->strength(), preciseX, preciseY, preciseStrength))
						{
							localStrengthPositions.emplace_back(preciseX, preciseY, preciseStrength);
						}
					}
					else
					{
						localStrengthPositions.emplace_back(TCoordinate(i1->x()), TCoordinate(y), i1->strength());
					}
				}
			}

next:

			i1Minus = i1;

			if (i1Plus != row1.end())
				++i1Plus;
		}
	}

	const OptionalScopedLock scopedLock(lock);

	strengthPositions->insert(strengthPositions->end(), localStrengthPositions.begin(), localStrengthPositions.end());
}

}

}

#endif // META_OCEAN_CV_NON_MAXIMUM_SUPPRESSION_H
