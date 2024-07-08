/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/DebugElements.h"

namespace Ocean
{

void DebugElements::ScopedHierarchyBase::change(const std::string& newHierarchyItem)
{
	ocean_assert(!hierarchyItem_.empty());
	ocean_assert(!newHierarchyItem.empty());

	if (!hierarchyItem_.empty())
	{
		owner_.popHierarchyItem();
	}

	if (!newHierarchyItem.empty())
	{
		hierarchyItem_ = newHierarchyItem;

		owner_.pushHierarchyItem(hierarchyItem_);
	}
}

void DebugElements::ScopedHierarchyBase::release()
{
	ocean_assert(!hierarchyItem_.empty());

	if (!hierarchyItem_.empty())
	{
		owner_.popHierarchyItem();
		hierarchyItem_.clear();
	}
}

bool DebugElements::isElementActive(const uint32_t elementId) const
{
	const ScopedLock scopedLock(lock_);

	return allElementsActivate_ || activeElements_.find(elementId) != activeElements_.cend();
}

void DebugElements::activateElement(const uint32_t elementId)
{
	const ScopedLock scopedLock(lock_);

	activeElements_.insert(elementId);
}

void DebugElements::deactivateElement(const uint32_t elementId)
{
	const ScopedLock scopedLock(lock_);

	activeElements_.erase(elementId);
}

void DebugElements::activateAllElements()
{
	allElementsActivate_ = true;
}

void DebugElements::pushHierarchyItem(const std::string& hierarchyItem)
{
	ocean_assert(!hierarchyItem.empty());

	hierarchy_.push_back(hierarchyItem);
}

void DebugElements::popHierarchyItem()
{
	ocean_assert(!hierarchy_.empty());

	hierarchy_.pop_back();
}

void DebugElements::updateElement(const uint32_t elementId, Frame&& frame)
{
	ocean_assert(isElementActive(elementId) && "The debug element should be activate");

	updateElement(elementId, std::move(frame), hierarchy_);
}

void DebugElements::updateElement(const uint32_t elementId, Frame&& frame, const Hierarchy& explicitHierarchy)
{
	ocean_assert(isElementActive(elementId) && "The debug element should be activate");

	const ScopedLock scopedLock(lock_);

	if (elementUpdateCallback_)
	{
		elementUpdateCallback_(elementId, &frame, &explicitHierarchy);
	}

	elementMap_[elementId][explicitHierarchy] = std::move(frame);
}

void DebugElements::updateElement(const uint32_t elementId, const Frame& frame)
{
	ocean_assert(isElementActive(elementId) && "The debug element should be activate");

	const ScopedLock scopedLock(lock_);

	updateElement(elementId, frame, hierarchy_);
}

void DebugElements::updateElement(const uint32_t elementId, const Frame& frame, const Hierarchy& explicitHierarchy)
{
	ocean_assert(isElementActive(elementId) && "The debug element should be activate");

	const ScopedLock scopedLock(lock_);

	if (elementUpdateCallback_)
	{
		elementUpdateCallback_(elementId, &frame, &explicitHierarchy);
	}

	elementMap_[elementId][explicitHierarchy] = Frame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
}

Frame DebugElements::element(const uint32_t elementId, const bool popElement)
{
	ocean_assert(isElementActive(elementId) && "The debug element should be activate");

	const ScopedLock scopedLock(lock_);

	const ElementMap::iterator iElement = elementMap_.find(elementId);

	if (iElement == elementMap_.cend())
	{
		return Frame();
	}

	HierarchyMap& hierarchyMap = iElement->second;

	if (hierarchyMap.empty())
	{
		return Frame();
	}

	Frame frameCopy(hierarchyMap.begin()->second, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	if (popElement)
	{
		hierarchyMap.erase(hierarchyMap.begin());
	}

	return frameCopy;
}

Frame DebugElements::element(const uint32_t elementId, const Hierarchy& hierarchy, const bool popElement)
{
	ocean_assert(isElementActive(elementId) && "The debug element should be activate");

	const ScopedLock scopedLock(lock_);

	const ElementMap::iterator iElement = elementMap_.find(elementId);

	if (iElement == elementMap_.cend())
	{
		return Frame();
	}

	HierarchyMap& hierarchyMap = iElement->second;

	const HierarchyMap::const_iterator iHierarchy = hierarchyMap.find(hierarchy);

	if (iHierarchy == hierarchyMap.cend())
	{
		return Frame();
	}

	Frame frameCopy(iHierarchy->second, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	if (popElement)
	{
		hierarchyMap.erase(iHierarchy);
	}

	return frameCopy;
}

bool DebugElements::elementIfActivate(const uint32_t elementId, Frame& frame, const bool popElement)
{
	if (!isElementActive(elementId))
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	const ElementMap::iterator iElement = elementMap_.find(elementId);

	if (iElement == elementMap_.cend())
	{
		return false;
	}

	HierarchyMap& hierarchyMap = iElement->second;

	if (hierarchyMap.empty())
	{
		return false;
	}

	frame.copy(hierarchyMap.begin()->second);

	if (popElement)
	{
		hierarchyMap.erase(hierarchyMap.begin());
	}

	return true;
}

bool DebugElements::elementIfActivate(const uint32_t elementId, Frame& frame, const Hierarchy& hierarchy, const bool popElement)
{
	if (!isElementActive(elementId))
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	const ElementMap::iterator iElement = elementMap_.find(elementId);

	if (iElement == elementMap_.cend())
	{
		return false;
	}

	HierarchyMap& hierarchyMap = iElement->second;

	HierarchyMap::const_iterator iHierarchy = hierarchyMap.find(hierarchy);

	if (iHierarchy == hierarchyMap.cend())
	{
		return false;
	}

	frame.copy(iHierarchy->second);

	if (popElement)
	{
		hierarchyMap.erase(iHierarchy);
	}

	return true;
}

std::vector<DebugElements::Hierarchy> DebugElements::hierarchies(const uint32_t elementId) const
{
	const ScopedLock scopedLock(lock_);

	const ElementMap::const_iterator iElement = elementMap_.find(elementId);

	if (iElement == elementMap_.cend())
	{
		return std::vector<Hierarchy>();
	}

	const HierarchyMap& hierarchyMap = iElement->second;

	std::vector<Hierarchy> hierarchies;
	hierarchies.reserve(hierarchyMap.size());

	for (HierarchyMap::const_iterator iHierarchy = hierarchyMap.cbegin(); iHierarchy != hierarchyMap.cend(); ++iHierarchy)
	{
		hierarchies.emplace_back(iHierarchy->first);
	}

	return hierarchies;
}

}
