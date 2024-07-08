/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/imageannotator/wxw/LineManager.h"

LineManager::LineId LineManager::addLine(const FiniteLineD2& line)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(lines_.find(nextLineId_) == lines_.cend());

	lines_.insert(std::make_pair(nextLineId_, LinePair(line, invalidId)));

	contentHasChanged_ = true;

	return nextLineId_++;
}

std::vector<LineManager::LineId> LineManager::addLines(const FiniteLinesD2& lines)
{
	const ScopedLock scopedLock(lock_);

	std::vector<LineManager::LineId> lineIds;
	lineIds.reserve(lines.size());

	for (const FiniteLineD2& line : lines)
	{
		ocean_assert(lines_.find(nextLineId_) == lines_.cend());

		lines_.insert(std::make_pair(nextLineId_, LinePair(line, invalidId)));

		lineIds.push_back(nextLineId_++);
	}

	contentHasChanged_ = !lines.empty();

	return lineIds;
}

bool LineManager::updateLine(const LineId lineId, const FiniteLineD2& line)
{
	ocean_assert(line.isValid());

	const ScopedLock scopedLock(lock_);

	LineMap::iterator i = lines_.find(lineId);

	if (i == lines_.cend())
	{
		return false;
	}

	if (i->second.first == line)
	{
		// actually, the line has not changed
		return true;
	}

	i->second = LinePair(line, i->second.second);

	contentHasChanged_ = true;

	return true;
}

bool LineManager::removeLine(const LineId lineId)
{
	ocean_assert(lineId != invalidId);

	const ScopedLock scopedLock(lock_);

	const LineMap::iterator i = lines_.find(lineId);

	if (i == lines_.end())
	{
		return false;
	}

	lines_.erase(i);

	contentHasChanged_ = true;

	return true;
}

void LineManager::removeLines(const LineIdSet& lineIds)
{
	if (lineIds.empty())
	{
		return;
	}

	const ScopedLock scopedLock(lock_);

	for (const LineId& lineId : lineIds)
	{
		lines_.erase(lineId);
	}

	contentHasChanged_ = true;
}

FiniteLineD2 LineManager::line(const LineId lineId) const
{
	ocean_assert(lineId != invalidId);

	const ScopedLock scopedLock(lock_);

	const LineMap::const_iterator i = lines_.find(lineId);

	ocean_assert(i != lines_.end());
	if (i == lines_.end())
	{
		return FiniteLineD2();
	}

	return i->second.first;
}

FiniteLinesD2 LineManager::lines(const LineIdSet& lineIds) const
{
	const ScopedLock scopedLock(lock_);

	std::vector<FiniteLineD2> result;
	result.reserve(lineIds.size());

	for (const LineId& lineId : lineIds)
	{
		const LineMap::const_iterator i = lines_.find(lineId);
		ocean_assert(i != lines_.end());

		result.push_back(i->second.first);
	}

	return result;
}

FiniteLinesD2 LineManager::allLines(const LineId excludeLineId) const
{
	const ScopedLock scopedLock(lock_);

	std::vector<FiniteLineD2> result;
	result.reserve(lines_.size());

	for (LineMap::const_iterator i = lines_.cbegin(); i != lines_.cend(); ++i)
	{
		if (i->first != excludeLineId)
		{
			result.emplace_back(i->second.first);
		}
	}

	return result;
}

FiniteLinesD2 LineManager::allLines(const LineIdSet& excludeLineIds, const LineId additionalExcludeLineId) const
{
#ifdef OCEAN_DEBUG
	for (const LineId& lineId : excludeLineIds)
	{
		ocean_assert(lineId != invalidId);
	}
#endif

	const ScopedLock scopedLock(lock_);

	std::vector<FiniteLineD2> result;
	result.reserve(lines_.size());

	for (LineMap::const_iterator i = lines_.cbegin(); i != lines_.cend(); ++i)
	{
		if (i->first != additionalExcludeLineId && excludeLineIds.find(i->first) == excludeLineIds.end())
		{
			result.emplace_back(i->second.first);
		}
	}

	return result;
}

FiniteLinesD2 LineManager::allLinesInGroup(const unsigned int groupIndex, const LineId excludeLineId, LineIds* lineIds) const
{
	const ScopedLock scopedLock(lock_);

	std::vector<FiniteLineD2> result;
	result.reserve(lines_.size());

	if (lineIds)
	{
		lineIds->clear();
		lineIds->reserve(lines_.size());
	}

	for (LineMap::const_iterator i = lines_.cbegin(); i != lines_.cend(); ++i)
	{
		if (i->second.second == groupIndex && i->first != excludeLineId)
		{
			result.emplace_back(i->second.first);

			if (lineIds)
			{
				lineIds->push_back(i->first);
			}
		}
	}

	return result;
}

FiniteLinesD2 LineManager::allLinesInGroup(const unsigned int groupIndex, const LineIdSet& excludeLineIds, const LineId additionalExcludeLineId, LineIds* lineIds) const
{
#ifdef OCEAN_DEBUG
	for (const LineId& lineId : excludeLineIds)
	{
		ocean_assert(lineId != invalidId);
	}
#endif

	const ScopedLock scopedLock(lock_);

	std::vector<FiniteLineD2> result;
	result.reserve(lines_.size());

	if (lineIds)
	{
		lineIds->clear();
		lineIds->reserve(lines_.size());
	}

	for (LineMap::const_iterator i = lines_.cbegin(); i != lines_.cend(); ++i)
	{
		if (i->second.second == groupIndex && i->first != additionalExcludeLineId && excludeLineIds.find(i->first) == excludeLineIds.end())
		{
			result.emplace_back(i->second.first);

			if (lineIds)
			{
				lineIds->push_back(i->first);
			}
		}
	}

	return result;
}

void LineManager::addLinesToGroup(const LineIdSet& lineIds, const unsigned int groupIndex)
{
	const ScopedLock scopedLock(lock_);

	for (const LineId& lineId : lineIds)
	{
		LineMap::iterator i = lines_.find(lineId);
		ocean_assert(i != lines_.end());

		i->second.second = groupIndex;
	}

	contentHasChanged_ = true;
}

void LineManager::addLinesToGroup(const LineId* lineIds, const size_t size, const unsigned int groupIndex)
{
	ocean_assert(size == 0 || lineIds != nullptr);

	const ScopedLock scopedLock(lock_);

	for (size_t n = 0; n < size; ++n)
	{
		LineMap::iterator i = lines_.find(lineIds[n]);
		ocean_assert(i != lines_.end());

		i->second.second = groupIndex;
	}

	contentHasChanged_ = true;
}

bool LineManager::isLineInGroup(const LineId& lineId, const unsigned int groupIndex) const
{
	const ScopedLock scopedLock(lock_);

	const LineMap::const_iterator i = lines_.find(lineId);

	ocean_assert(i != lines_.cend());
	if (i == lines_.cend())
	{
		return false;
	}

	return i->second.second == groupIndex;
}

LineManager::LineId LineManager::findLine(const VectorD2& location, const double maximalDistance, unsigned int* pointIndex) const
{
	if (pointIndex != nullptr)
	{
		*pointIndex = (unsigned int)(-1);
	}

	ocean_assert(maximalDistance >= 0.0);
	if (maximalDistance < 0.0)
	{
		return invalidId;
	}

	const ScopedLock scopedLock(lock_);

	const double maximalDistanceSqr = NumericD::sqr(maximalDistance);

	LineId closestId = invalidId;
	double closestSqrDistance = maximalDistanceSqr + Numeric::eps();

	for (LineMap::const_iterator i = lines_.cbegin(); i != lines_.cend(); ++i)
	{
		const double sqrDistance = i->second.first.sqrDistance(location);

		if (sqrDistance < closestSqrDistance)
		{
			closestSqrDistance = sqrDistance;
			closestId = i->first;
		}
	}

	if (closestId != invalidId && pointIndex != nullptr)
	{
		const FiniteLineD2& line = lines_.find(closestId)->second.first;

		const double sqrDistance0 = location.sqrDistance(line.point0());
		const double sqrDistance1 = location.sqrDistance(line.point1());

		if (sqrDistance0 < sqrDistance1 && sqrDistance0 <= Numeric::sqr(maximalDistance))
		{
			*pointIndex = 0u;
		}
		else if (sqrDistance1 < sqrDistance0 && sqrDistance1 <= Numeric::sqr(maximalDistance))
		{
			*pointIndex = 1u;
		}
	}

	return closestId;
}

LineManager::LineIdSet LineManager::findLines(const BoxD2& boundingBox) const
{
	ocean_assert(boundingBox.isValid());

	if (boundingBox.isPoint())
	{
		return LineIdSet();
	}

	const FiniteLineD2 edge0 = FiniteLineD2(boundingBox.corner(0), boundingBox.corner(1));
	const FiniteLineD2 edge1 = FiniteLineD2(boundingBox.corner(1), boundingBox.corner(2));
	const FiniteLineD2 edge2 = FiniteLineD2(boundingBox.corner(2), boundingBox.corner(3));
	const FiniteLineD2 edge3 = FiniteLineD2(boundingBox.corner(3), boundingBox.corner(0));

	const ScopedLock scopedLock(lock_);

	LineIdSet lineIds;

	for (LineMap::const_iterator i = lines_.cbegin(); i != lines_.cend(); ++i)
	{
		const FiniteLineD2& line = i->second.first;

		if (boundingBox.isInside(line.point0()) || boundingBox.isInside(line.point1()))
		{
			lineIds.insert(i->first);
		}
		else
		{
			// the bounding box can still be a simple line only, so that an edge can be invalid

			if ((edge0.isValid() && edge0.intersects(line))
					|| (edge1.isValid() && edge1.intersects(line))
					|| (edge2.isValid() && edge2.intersects(line))
					|| (edge3.isValid() && edge3.intersects(line)))
			{
				lineIds.insert(i->first);
			}
		}
	}

	return lineIds;
}

unsigned int LineManager::addGroup()
{
	const ScopedLock scopedLock(lock_);

	return lineGroupCount_++;
}

void LineManager::clear()
{
	const ScopedLock scopedLock(lock_);

	lines_.clear();
	nextLineId_ = LineId(0);
	lineGroupCount_ = 0u;

	contentHasChanged_ = true;
}

bool LineManager::loadLines(const std::string& filename)
{
	std::ifstream stream(filename.c_str(), std::ios::binary);

	if (stream.is_open() == false)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	while (stream.eof() == false)
	{
		double xPoint0 = Numeric::minValue();
		double yPoint0 = Numeric::minValue();
		double xPoint1 = Numeric::minValue();
		double yPoint1 = Numeric::minValue();

		stream >> xPoint0;

		if (stream.eof())
		{
			break;
		}

		stream >> yPoint0;
		stream >> xPoint1;
		stream >> yPoint1;

		if (stream.bad() || xPoint0 == Numeric::minValue() || yPoint0 == Numeric::minValue() || xPoint1 == Numeric::minValue() || yPoint1 == Numeric::minValue())
		{
			return false;
		}

		addLine(FiniteLineD2(VectorD2(xPoint0, yPoint0), VectorD2(xPoint1, yPoint1)));
	}

	contentHasChanged_ = false;

	return true;
}

bool LineManager::loadGroups(const std::string& filename)
{
	std::ifstream stream(filename.c_str(), std::ios::binary);

	if (stream.is_open() == false)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	unsigned int lineIndex = 0u;

	while (stream.eof() == false)
	{
		int groupIndex = NumericT<int>::minValue();

		stream >> groupIndex;

		if (stream.eof())
		{
			break;
		}

		if (stream.bad() || groupIndex < -1 || groupIndex > 1000)
		{
			ocean_assert(groupIndex <= 1000);
			return false;
		}

		if (groupIndex >= 0)
		{
			ocean_assert(groupIndex >= 0u);
			while ((unsigned int)groupIndex >= lineGroupCount_)
			{
				addGroup();
			}

			LineMap::iterator iLine = lines_.find(LineId(lineIndex));

			// the index of the line and the id of the line may not match to each other
			// e.g., when the user has removed lines in the meantime, this is a drawback of not storing line indices in the file

			if (iLine == lines_.end())
			{
				return false;
			}

			iLine->second.second = (unsigned int)groupIndex;
		}

		lineIndex++;
	}

	if (size_t(lineIndex) != lines_.size())
	{
		// the group file provided not the exact number of connection
		return false;
	}

	contentHasChanged_ = false;

	return true;
}

bool LineManager::saveLines(const std::string& filename)
{
	std::ofstream stream(filename.c_str(), std::ios::binary);

	if (stream.is_open() == false)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	for (LineMap::const_iterator i = lines_.cbegin(); i != lines_.cend(); ++i)
	{
		const FiniteLineD2& finiteLine = i->second.first;

		stream << String::toAString(finiteLine.point0().x(), 5u);
		stream << " ";
		stream << String::toAString(finiteLine.point0().y(), 5u);
		stream << " ";
		stream << String::toAString(finiteLine.point1().x(), 5u);
		stream << " ";
		stream << String::toAString(finiteLine.point1().y(), 5u);

		stream << std::endl;
	}

	if (stream.good())
	{
		contentHasChanged_ = false;
		return true;
	}

	return false;
}

bool LineManager::saveGroups(const std::string& filename)
{
	std::ofstream stream(filename.c_str(), std::ios::binary);

	if (stream.is_open() == false)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	for (LineMap::const_iterator i = lines_.cbegin(); i != lines_.cend(); ++i)
	{
		const unsigned int groupIndex = i->second.second;

		if (groupIndex < lineGroupCount_)
		{
			stream << groupIndex;
		}
		else
		{
			stream << "-1";
		}

		stream << std::endl;
	}

	if (stream.good())
	{
		contentHasChanged_ = false;
		return true;
	}

	return false;



}
