/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_LINE_MANAGER_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_LINE_MANAGER_H

#include "application/ocean/demo/misc/imageannotator/wxw/ImageAnnotator.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

#include "ocean/math/Box2.h"
#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Vector2.h"

#include <unordered_set>

/**
 * This class implements a manager for lines, the manager is implemented as a singleton
 * @ingroup applicationdemomiscimageannoatoarwxw
 */
class LineManager : public Singleton<LineManager>
{
	friend class Singleton<LineManager>;

	public:

		/**
		 * Definition of an id for a line.
		 */
		typedef unsigned int LineId;

		/**
		 * Definition of a pair combining a line with a group index.
		 */
		typedef std::pair<FiniteLineD2, unsigned int> LinePair;

		/**
		 * Definition of a map mapping line ids to line pairs.
		 */
		typedef std::map<LineId, LinePair> LineMap;

		/**
		 * Definition of a set holding line ids.
		 */
		typedef std::unordered_set<LineId> LineIdSet;

		/**
		 * Definition of a vector holding line ids.
		 */
		typedef std::vector<LineId> LineIds;

		/**
		 * Definition of an invalid point id.
		 */
		static constexpr LineId invalidId = LineId(-1);

	public:

		/**
		 * Adds a new line to this manager.
		 * @param line The new line to add, must be valid
		 * @return The id of the new line
		 */
		LineId addLine(const FiniteLineD2& line);

		/**
		 * Adds a new lines to this manager.
		 * @param lines The new lines to add, each must be valid
		 * @return The ids of the new lines
		 */
		std::vector<LineId> addLines(const FiniteLinesD2& lines);

		/**
		 * Updates a line already existing in this manager.
		 * @param lineId The id of the line to update, must be valid
		 * @param line The new line, must be valid
		 * @return True, if succeeded
		 */
		bool updateLine(const LineId lineId, const FiniteLineD2& line);

		/**
		 * Removes a line from this manager.
		 * @param lineId The id of the line to remove, must be valid
		 * @return True, if the provided line existed and could be removed
		 * @see removeLines().
		 */
		bool removeLine(const LineId lineId);

		/**
		 * Removes lines from this manager.
		 * @param lineIds The ids of the lines to remove
		 * @see removeLine().
		 */
		void removeLines(const LineIdSet& lineIds);

		/**
		 * Returns a line of this manager.
		 * @param lineId The id of the line to return, must be valid
		 * @return The requested line
		 */
		FiniteLineD2 line(const LineId lineId) const;

		/**
		 * Returns a set of specified lines of this manager.
		 * @param lineIds The ids of all lines to return
		 * @return The lines as specified
		 */
		FiniteLinesD2 lines(const LineIdSet& lineIds) const;

		/**
		 * Returns all lines of this manager.
		 * @param excludeLineId Optional id of a line that will be excluded from the returning set of lines
		 * @return The manager's lines
		 */
		FiniteLinesD2 allLines(const LineId excludeLineId = invalidId) const;

		/**
		 * Returns all lines of this manager except lines which are explicitly specified.
		 * @param excludeLineIds The ids of all lines that are excluded
		 * @param additionalExcludeLineId Optional one explicit additional id of a line that also should be excluded
		 * @return The manager's lines with ids not specified in 'excludeLinedIds'
		 */
		FiniteLinesD2 allLines(const LineIdSet& excludeLineIds, const LineId additionalExcludeLineId = invalidId) const;

		/**
		 * Returns all lines with a specified group of this manager.
		 * @param groupIndex The index of the group, -1 to get all lines not associated with a group
		 * @param excludeLineId Optional id of a line that will be excluded from the returning set of lines
		 * @param lineIds Optional resulting ids of all resulting lines, nullptr if not of interest
		 * @return The manager's lines
		 */
		FiniteLinesD2 allLinesInGroup(const unsigned int groupIndex, const LineId excludeLineId = invalidId, LineIds* lineIds = nullptr) const;

		/**
		 * Returns all lines with a specified group of this manager except lines which are explicitly specified.
		 * @param groupIndex The index of the group, -1 to get all lines not associated with a group
		 * @param excludeLineIds The ids of all lines that are excluded
		 * @param additionalExcludeLineId Optional one explicit additional id of a line that also should be excluded
		 * @param lineIds Optional resulting ids of all resulting lines, nullptr if not of interest
		 * @return The manager's lines with ids not specified in 'excludeLinedIds'
		 */
		FiniteLinesD2 allLinesInGroup(const unsigned int groupIndex, const LineIdSet& excludeLineIds, const LineId additionalExcludeLineId = invalidId, LineIds* lineIds = nullptr) const;

		/**
		 * Adds a set of lines to a specified group.
		 * @param lineIds The ids of the lines to be added to the specified group, each id must be valid
		 * @param groupIndex The index of the group to which all lines will be added, -1 to not associate each line with any group
		 */
		void addLinesToGroup(const LineIdSet& lineIds, const unsigned int groupIndex);

		/**
		 * Adds a set of lines to a specified group.
		 * @param lineIds The ids of the lines to be added to the specified group, each id must be valid, can be nullptr if size is 0
		 * @param size The number of provided line ids, with range [0, infinity)
		 * @param groupIndex The index of the group to which all lines will be added, -1 to not associate each line with any group
		 */
		void addLinesToGroup(const LineId* lineIds, const size_t size, const unsigned int groupIndex);

		/**
		 * Returns whether a specific line is part of a specific group.
		 * @param lineId The id of the line to check, must be valid
		 * @param groupIndex The index of the group to check, with range [0, groups())
		 * @return True, if so
		 */
		bool isLineInGroup(const LineId& lineId, const unsigned int groupIndex) const;

		/**
		 * Finds the closest line to a given coordinate.
		 * @param location The location for which the closest line will be determined
		 * @param maximalDistance The maximal distance between the location and a close line in pixel, with range [0, infinity)
		 * @param pointIndex Optional resulting index of the end point of the line which is within the specified distance, -1 if the location is e.g., in the center of the line
		 * @return The id of the line, 'invalidId' if no line could be found
		 */
		LineId findLine(const VectorD2& location, const double maximalDistance = 10.0, unsigned int* pointIndex = nullptr) const;

		/**
		 * Find all lines inside or intersecting a given 2D axis aligned bounding box.
		 * @param boundingBox The 2D bounding box for which the lines will be determined, must be valid
		 * @return The ids of all lines intersecting the given bounding box
		 */
		LineIdSet findLines(const BoxD2& boundingBox) const;

		/**
		 * Adds a new group of lines.
		 * @return The index of the new group, with range [0, infinity)
		 */
		unsigned int addGroup();

		/**
		 * Return the number of currently existing groups of lines.
		 * @return The group count, with range [0, infinity)
		 */
		inline unsigned int groups() const;

		/**
		 * Returns whether this manager currently is not storying any lines.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Returns whether the content of the manager has been changed since the last call of saveLines().
		 * @return True, if so
		 * @see saveLines().
		 */
		inline bool hasContentChanged() const;

		/**
		 * Removes all lines and groups from this manager.
		 * @see isEmpty().
		 */
		void clear();

		/**
		 * Adds lines from a file to this manager.
		 * The new lines will be added to the existing lines.
		 * @param filename The name of the file from which the lines will be loaded
		 * @return True, if successful
		 * @see saveLine(), clear().
		 */
		bool loadLines(const std::string& filename);

		/**
		 * Adds groups and the connection between lines and groups from a file to this manager.
		 * Ensure that the file matches with the currently existing lines.
		 * @param filename The name of the file from which the information will be loaded
		 * @return True, if successful
		 * @see saveGroups(), clear().
		 */
		bool loadGroups(const std::string& filename);

		/**
		 * Saves all lines of this manager to a file.
		 * @param filename The name of the file to which the lines will be written
		 * @return True, if successful
		 * @see loadLines(), hasContentChanged().
		 */
		bool saveLines(const std::string& filename);

		/**
		 * Saves the connection between lines and groups to a file.
		 * The resulting file will have exactly one group information for each line, -1 if the line is not connected with a group.
		 * @param filename The name of the file to which the information will be written
		 * @return True, if successful
		 * @see loadGroups(), hasContentChanged().
		 */
		bool saveGroups(const std::string& filename);

	protected:

		/**
		 * Creates a new line manager object.
		 */
		LineManager() = default;

	protected:

		/// The lines the manager currently knows.
		LineMap lines_;

		/// The id of the next line to add.
		LineId nextLineId_ = LineId(0);

		/// The number of groups of lines currently exist.
		unsigned int lineGroupCount_ = 0u;

		/// True, if content of this manager has changed since the last save call.
		bool contentHasChanged_ = false;

		/// The lock of this manager.
		mutable Lock lock_;
};

inline unsigned int LineManager::groups() const
{
	const ScopedLock scopedLock(lock_);

	return lineGroupCount_;
}

bool LineManager::isEmpty() const
{
	const ScopedLock scopedLock(lock_);

	return lines_.empty();
}

inline bool LineManager::hasContentChanged() const
{
	const ScopedLock scopedLock(lock_);

	return contentHasChanged_;
}

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_MISC_IMAGEANNOTATOR_WXW_LINE_MANAGER_H
