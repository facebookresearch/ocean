/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_DEBUG_ELEMENTS_H
#define META_OCEAN_BASE_DEBUG_ELEMENTS_H

#include "ocean/base/Base.h"
#include "ocean/base/Callback.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

#include <map>
#include <unordered_set>

namespace Ocean
{

/**
 * This class implements the base class for a container for debug elements.
 * To use this class, create an own singleton class and derive from this object.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT DebugElements
{
	public:

		/**
		 * This class implements a scoped hierarchy.
		 * The hierarchy exists as long as this object exists.
		 */
		class OCEAN_BASE_EXPORT ScopedHierarchyBase
		{
			public:

				/**
				 * Releases the hierarchy and disposes this object.
				 */
				inline ~ScopedHierarchyBase();

				/**
				 * Explicitly exchanges the hierarchy instead of releasing it and creating a new one.
				 * Actually pops the current hierarchy and pushes the new one.
				 * @param newHierarchyItem The new hierarchy item to be pushed, must be valid
				 */
				void change(const std::string& newHierarchyItem);

				/**
				 * Explicitly releases the hierarchy (already before the destructor is called).
				 */
				void release();

			protected:

				/**
				 * Disabled copy constructor.
				 * @param scopedHierarchy The hierarchy object which would have been copied
				 */
				ScopedHierarchyBase(const ScopedHierarchyBase& scopedHierarchy) = delete;

				/**
				 * Creates a new scoped object and pushes the given hierarchy.
				 * @param owner The owner of the scoped object
				 * @param hierarchyItem The hierarchy item to be pushed, must be valid
				 */
				inline ScopedHierarchyBase(DebugElements& owner, const std::string& hierarchyItem);

				/**
				 * Disabled copy operator.
				 * @param scopedHierarchy The hierarchy object which would have been copied
				 * @return Reference to this object
				 */
				ScopedHierarchyBase& operator=(const ScopedHierarchyBase& scopedHierarchy) = delete;

			protected:

				/// The owner of this object.
				DebugElements& owner_;

				/// The hierarchy item of this object.
				std::string hierarchyItem_;
		};

		/**
		 * Definition of a vector holding strings (a hierarchy).
		 */
		typedef std::vector<std::string> Hierarchy;

		/**
		 * Definition of a callback function for an updated debug element.
		 */
		typedef Callback<void, const uint32_t, const Frame*, const Hierarchy*> ElementUpdateCallback;

	protected:

		/**
		 * Definition of a map mapping hierarchies to frames.
		 */
		typedef std::map<Hierarchy, Frame> HierarchyMap;

		/**
		 * Definition of a map mapping element ids to hierarchy maps.
		 */
		typedef std::map<uint32_t, HierarchyMap> ElementMap;

		/**
		 * Definition of a set holding element ids.
		 */
		typedef std::unordered_set<uint32_t> ElementSet;

	public:

		/**
		 * Returns whether a specific debug elements is activated.
		 * Call this function before preparing the debug information (and updating via updateElement) to reduce the performance overhead when debugging.
		 * @param elementId The id for which the activation state is returned
		 * @return True, if the debug element is activate; False, if the debug element is deactivated
		 */
		bool isElementActive(const uint32_t elementId) const;

		/**
		 * Activates a specific debug element.
		 * @param elementId The id of the element to activate
		 * @see isElementActivated(), deactivateElement().
		 */
		void activateElement(const uint32_t elementId);

		/**
		 * Deactivates a specific debug element.
		 * @param elementId The id of the element to activate
		 * @see activateElement().
		 */
		void deactivateElement(const uint32_t elementId);

		/**
		 * Explicitly activates all elements (to avoid defining all active elements individually).
		 * Beware: Activating a couple of debug elements (and preparing the debug data in particular) can reduce the performance.
		 */
		void activateAllElements();

		/**
		 * Pushes a new hierarchy.
		 * @param hierarchyItem The hierarchy item to be pushed
		 */
		void pushHierarchyItem(const std::string& hierarchyItem);

		/**
		 * Pops the most recent hierarchy item.
		 */
		void popHierarchyItem();

		/**
		 * Updates the debug information of a specific debug element.
		 * In case one (or several) sub-element bits are set, the element with matching sub-element bit mask will be updated only.
		 * Beware: Due to performance reasons, the specific debug element must be activated before it can be updated,<br>
		 * as the preparation of debug information for disabled elements can be skipped.
		 * @param elementId The id of the debug element, should be a value from `DebugId`
		 * @param frame The new debug frame of the debug element, will be moved
		 * @see setSubElementBit(), activateElement().
		 */
		void updateElement(const uint32_t elementId, Frame&& frame);

		/**
		 * Updates the debug information of a specific debug element.
		 * In case one (or several) sub-element bits are set, the element with matching sub-element bit mask will be updated only.
		 * Beware: Due to performance reasons, the specific debug element must be activated before it can be updated,<br>
		 * as the preparation of debug information for disabled elements can be skipped.
		 * @param elementId The id of the debug element, should be a value from `DebugId`
		 * @param frame The new debug frame of the debug element, will be moved
		 * @param explicitHierarchy The explicit hierarchy which will be used instead of the actual internal known hierarchy
		 * @see setSubElementBit(), activateElement().
		 */
		void updateElement(const uint32_t elementId, Frame&& frame, const Hierarchy& explicitHierarchy);

		/**
		 * Updates the debug information of a specific debug element.
		 * Beware: Due to performance reasons, the specific debug element must be activated before it can be updated,<br>
		 * as the preparation of debug information for disabled elements can be skipped.
		 * @param elementId The id of the debug element, should be a value from `DebugId`
		 * @param frame The new debug frame of the debug element, will be copied
		 * @see activateElement(), activateAllElements().
		 */
		void updateElement(const uint32_t elementId, const Frame& frame);

		/**
		 * Updates the debug information of a specific debug element.
		 * Beware: Due to performance reasons, the specific debug element must be activated before it can be updated,<br>
		 * as the preparation of debug information for disabled elements can be skipped.
		 * @param elementId The id of the debug element, should be a value from `DebugId`
		 * @param frame The new debug frame of the debug element, will be copied
		 * @param explicitHierarchy The explicit hierarchy which will be used instead of the actual internal known hierarchy
		 * @see activateElement(), activateAllElements().
		 */
		void updateElement(const uint32_t elementId, const Frame& frame, const Hierarchy& explicitHierarchy);

		/**
		 * Returns the most recent debug frame of a specific debug element.
		 * If more than one debug element with individual hierarchies exist, one of the elements will be returned.<br>
		 * Beware: Due to performance reasons, the specific debug element must be activated before calling this function.
		 * @param elementId The id for which the frame will be returned
		 * @param popElement True, to remove the debug element; False, to keep the debug element until it gets updated again
		 * @return The copy of the frame which is associated with the id, an invalid frame in case the id does not exist
		 * @see isElementActive().
		 */
		Frame element(const uint32_t elementId, const bool popElement = false);

		/**
		 * Returns the most recent debug frame of a specific debug element.
		 * Beware: Due to performance reasons, the specific debug element must be activated before calling this function.
		 * @param elementId The id for which the frame will be returned
		 * @param hierarchy The hierarchy for which the element will be returned
		 * @param popElement True, to remove the debug element; False, to keep the debug element until it gets updated again
		 * @return The copy of the frame which is associated with the id, an invalid frame in case the id does not exist
		 * @see isElementActive().
		 */
		Frame element(const uint32_t elementId, const Hierarchy& hierarchy, const bool popElement = false);

		/**
		 * Returns the most recent debug frame of a specific debug element if the debug element is activate and exists.
		 * If more than one debug element with individual hierarchies exist, one of the elements will be returned.
		 * @param elementId The id for which the frame will be returned
		 * @param frame The resulting copy of the debug frame which is associated with the id
		 * @param popElement True, to remove the debug element; False, to keep the debug element until it gets updated again
		 * @return True, if the debug element is activate and exists
		 */
		bool elementIfActivate(const uint32_t elementId, Frame& frame, const bool popElement = false);

		/**
		 * Returns the most recent debug frame of a specific debug element if the debug element is activate and exists.
		 * @param elementId The id for which the frame will be returned
		 * @param frame The resulting copy of the debug frame which is associated with the id
		 * @param hierarchy The hierarchy for which the element will be returned
		 * @param popElement True, to remove the debug element; False, to keep the debug element until it gets updated again
		 * @return True, if the debug element is activate and exists
		 */
		bool elementIfActivate(const uint32_t elementId, Frame& frame, const Hierarchy& hierarchy, const bool popElement = false);

		/**
		 * Returns all existing hierarchies for a given element id.
		 * @param elementId The id for which all hierarchies will be returned
		 * @return The hierarchies of the element, an empty vector if the element does not exist
		 */
		std::vector<Hierarchy> hierarchies(const uint32_t elementId) const;

		/**
		 * Sets the optional callback function for updated debug element.
		 * The callback function will be called whenever a debug element is updated.
		 * @param callback The callback function to be set, can be invalid to remove a previously set function
		 */
		inline void setElementUpdateCallback(const ElementUpdateCallback& callback);

	protected:

		/**
		 * Protected default constructor.
		 */
		inline DebugElements();

	protected:

		/// The set holding activate debug element ids.
		ElementSet activeElements_;

		/// True, if all debug elements are activated (this state overrides the individual ids of `activeIds_`).
		bool allElementsActivate_;

		/// The map mapping ids to frames.
		ElementMap elementMap_;

		/// The current hierarchy.
		Hierarchy hierarchy_;

		/// Optional callback function for updated debug elements.
		ElementUpdateCallback elementUpdateCallback_;

		/// The data lock.
		mutable Lock lock_;
};

DebugElements::ScopedHierarchyBase::ScopedHierarchyBase(DebugElements& owner, const std::string& hierarchyItem) :
	owner_(owner),
	hierarchyItem_(hierarchyItem)
{
	ocean_assert(!hierarchyItem_.empty());

	owner_.pushHierarchyItem(hierarchyItem_);
}

DebugElements::ScopedHierarchyBase::~ScopedHierarchyBase()
{
	if (!hierarchyItem_.empty())
	{
		owner_.popHierarchyItem();
	}
}

DebugElements::DebugElements() :
	allElementsActivate_(false)
{
	// nothing to do here
}

inline void DebugElements::setElementUpdateCallback(const ElementUpdateCallback& callback)
{
	const ScopedLock scopedLock(lock_);

	elementUpdateCallback_ = callback;
}

}

#endif // META_OCEAN_BASE_DEBUG_ELEMENTS_H
