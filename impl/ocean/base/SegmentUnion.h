/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_SEGMENT_UNION_H
#define META_OCEAN_BASE_SEGMENT_UNION_H

#include "ocean/base/Base.h"

#include <limits>
#include <map>

namespace Ocean
{

/**
 * This class implements a functionality to determine the union of individual segments.
 * Each segment is defined by a start point and an end point.<br>
 * The result of such a uion is depicted below:
 * <pre>
 *       Segment A:          S---------P
 *       Segment B:   S--P
 *       Segment C:                S-----------P
 *       Segment D:                                S----P
 *          ...
 *       Segment Z:                                     S-P
 * Resulting Union:   S--P   S-----------------P   S------P
 *
 * (with 's' as including start point, and 'p' as including end point)
 * </pre>
 * @tparam T The data type of the start and end point of a segment, should be 'float', 'double', or 'int'
 * @ingroup base
 */
template <typename T>
class SegmentUnion
{
	public:

		/**
		 * Definition of a map mapping a start point to the corresponding end point of a segment.
		 */
		typedef std::map<T, T> SegmentMap;

	public:

		/**
		 * Adds a new segment to this union object.
		 * @param startPosition The (including) start position of the segment, with range (-infinity, startPosition)
		 * @param stopPosition The (including) stop position of the segment, with range (stopPosition, infinity)
		 */
		void addSegment(const T& startPosition, const T& stopPosition);

		/**
		 * Returns the intersection of this union with a given range (an additional segment).
		 * An intersection between this object an a given range/segment is depicted below:
		 * <pre>
		 * Segments of this object:    X------X    X---X  X-----------X
		 *    Intersection Segment:        S--------------------------------P
		 *        Resulting object:        X--X    X---X  X-----------X
		 * </pre>
		 * @param startPosition The (including) start position of the segment, with range (-infinity, startPosition)
		 * @param stopPosition The (including) stop position of the segment, with range (stopPosition, infinity)
		 * @return The new object holding the intersection of this object and the specified segment, may be empty if no intersection exists
		 */
		SegmentUnion<T> intersection(const T& startPosition, const T& stopPosition) const;

		/**
		 * Returns the maximal gap between all successive segments.
		 * @return The maximal gap, 0 if this object is composed of zero or one segments, with range [0, infinity)
		 */
		T maximalGap() const;

		/**
		 * Returns the segments of this object.
		 * @return The individual segments defining the union
		 */
		inline const SegmentMap& segments() const;

		/**
		 * Returns the sum of all segment sizes.
		 * @return The size of all segments, with range [0, infinity)
		 */
		T unionSize() const;

		/**
		 * Returns whether the union is composed of at least one segment.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/**
		 * Returns whether the data structure of this object is correct.
		 * @return True, if so
		 */
		bool isCorrect() const;

	protected:

		/// The map holding all segments.
		SegmentMap segmentMap_;
};

template <typename T>
void SegmentUnion<T>::addSegment(const T& startPosition, const T& stopPosition)
{
	ocean_assert(isCorrect());

	ocean_assert(startPosition < stopPosition);

	//   existing segments:      X----X    X------X    X-X       X------------X
	// start/stop position:                                   S----P

	if (segmentMap_.empty())
	{
		segmentMap_.insert(std::make_pair(startPosition, stopPosition));
		ocean_assert(isCorrect());

		return;
	}

	typename SegmentMap::iterator i = segmentMap_.lower_bound(startPosition); // segment with 'startPosition' <= start position (of found segment)

	if (i == segmentMap_.end())
	{
		ocean_assert(segmentMap_.rbegin() != segmentMap_.rend());

		typename SegmentMap::reverse_iterator lastSegment = segmentMap_.rbegin();

		if (startPosition > lastSegment->second)
		{
			//   existing segments:  X---X    X--------X
			// start/stop position:                        S--------P

			segmentMap_.insert(std::make_pair(startPosition, stopPosition));
			ocean_assert(isCorrect());

			return;
		}

		if (startPosition == lastSegment->second)
		{
			//   existing segments:  X---X    X--------X
			// start/stop position:                    S--------P

			lastSegment->second = stopPosition;
			ocean_assert(isCorrect());

			return;
		}

		ocean_assert(startPosition > lastSegment->first);
		ocean_assert(startPosition < lastSegment->second);
		
		if (stopPosition <= lastSegment->second)
		{
			//   existing segments:  X---X    X------------X
			// start/stop position:              S----P
			// start/stop position:              S---------P

			return;
		}

		ocean_assert(stopPosition > lastSegment->second);

		//   existing segments:  X---X    X-------X
		// start/stop position:              S--------P

		lastSegment->second = stopPosition;
		ocean_assert(isCorrect());

		return;
	}

	ocean_assert(startPosition <= i->first);

	if (i == segmentMap_.begin())
	{
		if (stopPosition < i->first)
		{
			//   existing segments:           X------------X       X--------X
			// start/stop position:  S----P

			segmentMap_.insert(std::make_pair(startPosition, stopPosition));
			ocean_assert(isCorrect());

			return;
		}

		if (stopPosition == i->first)
		{
			//   existing segments:       X------------X       X--------X
			// start/stop position:  S----P

			const T newStopPosition = i->second;

			segmentMap_.erase(i);

			segmentMap_.insert(std::make_pair(startPosition, newStopPosition));

			ocean_assert(isCorrect());

			return;		
		}

		ocean_assert(stopPosition > i->first);

		typename SegmentMap::iterator iEnd = i;

		while (iEnd != segmentMap_.end())
		{
			if (stopPosition < iEnd->first)
			{
				//   existing segments:      X--------X      X--------X    X------X
				// start/stop position:   S--------------P
				// start/stop position:   S------------------------------P

				segmentMap_.erase(i, iEnd); // remove [i, iEnd - 1]

				segmentMap_.insert(std::make_pair(startPosition, stopPosition));

				ocean_assert(isCorrect());

				return;
			}
			else if (stopPosition < iEnd->second)
			{
				//   existing segments:      X------------X     X--------X    X------X
				// start/stop position:   S------------P
				// start/stop position:    S--------------------------P

				const T newStopPostion = iEnd->second;

				segmentMap_.erase(i, ++iEnd); // remove [i, iEnd]

				segmentMap_.insert(std::make_pair(startPosition, newStopPostion));

				ocean_assert(isCorrect());

				return;
			}

			++iEnd;
		}

		//   existing segments:      X------------X     X--------X    X------X
		// start/stop position:    S--------------------------------------------P

		segmentMap_.erase(i, segmentMap_.end()); // remove [i, end)

		segmentMap_.insert(std::make_pair(startPosition, stopPosition));

		ocean_assert(isCorrect());

		return;
	}

	--i;

	//   existing segments:   X------------X       X--------X
	// start/stop position:       S----P
	// start/stop position:       S--------P
	// start/stop position:       S------------P
	// start/stop position:                S------------P
	// start/stop position:                S---------------------P
	// start/stop position:                    S-P
	// start/stop position:                    S------P

	if (stopPosition <= i->second)
	{
		//   existing segments:  X---X      X------------X
		// start/stop position:                 S----P

		return;
	}

	if (startPosition <= i->second)
	{
		typename SegmentMap::iterator iEnd = i;

		while (++iEnd != segmentMap_.end())
		{
			if (stopPosition < iEnd->first)
			{
				//   existing segments:   X------------X      X--------X    X------X
				// start/stop position:       S------------P
				// start/stop position:       S--------------------------P

				i->second = stopPosition;

				segmentMap_.erase(++i, iEnd); // remove [i + 1, iEnd - 1]

				ocean_assert(isCorrect());

				return;
			}
			else if (stopPosition < iEnd->second)
			{
				//   existing segments:   X------------X     X--------X    X------X
				// start/stop position:       S------------------P
				// start/stop position:       S-------------------------------P

				i->second = iEnd->second;

				segmentMap_.erase(++i, ++iEnd); // remove [i + 1, iEnd]

				ocean_assert(isCorrect());

				return;
			}
		}

		//   existing segments:  X------------X     X--------X
		// start/stop position:      S---------------------------P

		i->second = stopPosition;

		segmentMap_.erase(++i, segmentMap_.end()); // remove [i + 1, end)

		ocean_assert(isCorrect());

		return;
	}

	ocean_assert(startPosition > i->second);

	typename SegmentMap::iterator iEnd = i;

	while (++iEnd != segmentMap_.end())
	{
		if (stopPosition < iEnd->first)
		{
			//   existing segments:        X------X     X--------X    X------X
			// start/stop position:   S--P
			// start/stop position:   S---------------P
			// start/stop position:   S-----------------------------P

			segmentMap_.erase(++i, iEnd); // remove [i + 1, iEnd - 1]

			segmentMap_.insert(std::make_pair(startPosition, stopPosition));

			ocean_assert(isCorrect());

			return;
		}
		else if (stopPosition < iEnd->second)
		{
			//   existing segments:      X------------X     X--------X
			// start/stop position:   S----------P
			// start/stop position:   S------------------------P

			const T newStopPosition = iEnd->second;

			segmentMap_.erase(++i, ++iEnd); // remove [i + 1, iEnd]

			segmentMap_.insert(std::make_pair(startPosition, newStopPosition));

			ocean_assert(isCorrect());

			return;
		}
	}

	//   existing segments:      X------------X     X--------X
	// start/stop position:   S----------------------------------P

	segmentMap_.erase(++i, segmentMap_.end()); // remove [i + 1, end)

	segmentMap_.insert(std::make_pair(startPosition, stopPosition));

	ocean_assert(isCorrect());
}

template <typename T>
SegmentUnion<T> SegmentUnion<T>::intersection(const T& startPosition, const T& stopPosition) const
{
	ocean_assert(isCorrect());
	ocean_assert(startPosition < stopPosition);

	// Segments of this object:    X------X    X---X  X-----------X
	//    Intersection Segment:        S--------------------------------P
	//        Resulting object:        X--X    X---X  X-----------X

	if (segmentMap_.empty())
	{
		return SegmentUnion<T>();
	}

	typename SegmentMap::const_iterator i = segmentMap_.lower_bound(startPosition); // segment with 'startPosition' <= start position (of found segment)

	if (i == segmentMap_.end())
	{
		ocean_assert(segmentMap_.rbegin() != segmentMap_.rend());

		typename SegmentMap::const_reverse_iterator lastSegment = segmentMap_.crbegin();

		if (startPosition >= lastSegment->second)
		{
			//   existing segments:  X---X    X--------X
			// start/stop position:                        S--------P
			// start/stop position:                    S--------P

			return SegmentUnion<T>();
		}

		ocean_assert(startPosition > lastSegment->first);
		ocean_assert(startPosition < lastSegment->second);

		//   existing segments:  X---X    X------------X
		// start/stop position:              S----P
		// start/stop position:              S---------P
		// start/stop position:                   S--------P

		const T newStartPosition = std::max(lastSegment->first, startPosition);
		const T newStopPosition = std::min(stopPosition, lastSegment->second);

		SegmentUnion<T> result;
		result.segmentMap_.insert(std::make_pair(newStartPosition, newStopPosition));

		ocean_assert(result.isCorrect());

		return result;
	}

	ocean_assert(startPosition <= i->first);

	if (i == segmentMap_.begin())
	{
		if (stopPosition <= i->first)
		{
			//   existing segments:           X------------X       X--------X
			// start/stop position:  S----P
			// start/stop position:  S--------P

			return SegmentUnion<T>();
		}

		ocean_assert(stopPosition > i->first);
	}
	else
	{
		--i;
	}

	//   existing segments:   X------------X       X--------X
	// start/stop position: S---------P
	// start/stop position:  S------------------P
	// start/stop position:       S----P
	// start/stop position:       S--------P
	// start/stop position:       S------------P
	// start/stop position:                S------------P
	// start/stop position:                S---------------------P
	// start/stop position:                    S-P
	// start/stop position:                    S------P

	if (stopPosition <= i->second)
	{
		//   existing segments:     X------------X
		// start/stop position:  S----P
		// start/stop position:     S----------P
		// start/stop position:     S------------P

		const T newStartPosition = std::max(i->first, startPosition);
		const T newStopPosition = std::min(stopPosition, i->second);

		SegmentUnion<T> result;
		result.segmentMap_.insert(std::make_pair(newStartPosition, newStopPosition));

		ocean_assert(result.isCorrect());

		return result;
	}

	if (startPosition >= i->second)
	{
		i++;
	}

	ocean_assert(startPosition < i->second);

	if (stopPosition <= i->first)
	{
		//   existing segments:   X------X         X--------X    X------X
		// start/stop position:              S--P

		return SegmentUnion<T>();
	}
	
	typename SegmentMap::const_iterator iEnd = i;

	while (++iEnd != segmentMap_.end())
	{
		if (stopPosition <= iEnd->first)
		{
			//   existing segments:        X------X     X--------X    X------X
			// start/stop position:   S--P
			// start/stop position:   S---------------P
			// start/stop position:   S-----------------------------P

			break;
		}
		else if (stopPosition <= iEnd->second)
		{
			//   existing segments:      X------------X     X--------X
			// start/stop position:   S----------P
			// start/stop position:   S------------------------P

			iEnd++;
			break;
		}
	}

	SegmentUnion<T> result;
			
	const T newStartPosition = std::max(startPosition, i->first);
	result.segmentMap_.insert(std::make_pair(newStartPosition, i->second));

	i++;

	for (typename SegmentMap::const_iterator iN = i; iN != iEnd; ++iN)
	{
		result.segmentMap_.insert(std::make_pair(iN->first, iN->second));
	}

	result.segmentMap_.rbegin()->second = std::min(stopPosition, result.segmentMap_.rbegin()->second);

	ocean_assert(result.isCorrect());

	return result;
}

template <typename T>
T SegmentUnion<T>::maximalGap() const
{
	ocean_assert(isCorrect());

	if (segmentMap_.size() <= 1)
	{
		return T(0);
	}

	typename SegmentMap::const_iterator left = segmentMap_.cbegin();
	typename SegmentMap::const_iterator right = left;
	++right;

	T maximalValue = right->first - left->second;
	ocean_assert(maximalValue > T(0));

	while (++right != segmentMap_.cend())
	{
		++left;

		const T value = right->first - left->second;
		ocean_assert(value > T(0));

		if (value > maximalValue)
		{
			maximalValue = value;
		}
	}

	return maximalValue;
}

template <typename T>
inline const typename SegmentUnion<T>::SegmentMap& SegmentUnion<T>::segments() const
{
	ocean_assert(isCorrect());

	return segmentMap_;
}

template <typename T>
T SegmentUnion<T>::unionSize() const
{
	ocean_assert(isCorrect());

	T sum = (0);

	for (typename SegmentMap::const_iterator i = segmentMap_.cbegin(); i != segmentMap_.cend(); ++i)
	{
		ocean_assert(i->first < i->second);

		sum += i->second - i->first;
	}

	return sum;
}

template <typename T>
inline SegmentUnion<T>::operator bool() const
{
	ocean_assert(isCorrect());

	return !segmentMap_.empty();
}

template <typename T>
bool SegmentUnion<T>::isCorrect() const
{
	T previousEnd = std::numeric_limits<T>::lowest();

	for (typename SegmentMap::const_iterator i = segmentMap_.cbegin(); i != segmentMap_.cend(); ++i)
	{
		if (i->first <= previousEnd)
		{
			return false;
		}

		previousEnd = i->second;
	}

	return true;
}

}

#endif // META_OCEAN_BASE_RING_MAP_H
