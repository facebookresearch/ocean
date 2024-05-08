/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAGDEBUGELEMENTS_H
#define META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAGDEBUGELEMENTS_H

#include "ocean/tracking/oculustags/OculusTags.h"

#include "ocean/base/DebugElements.h"

namespace Ocean
{

namespace Tracking
{

namespace OculusTags
{

#ifdef OCN_OCULUSTAG_DEBUGGING_ENABLED

/**
 * This class implements a container for debug elements.
 * @ingroup trackingoculustags
 */
class OculusTagDebugElements :
	public DebugElements,
	public Singleton<OculusTagDebugElements>
{
	friend class Singleton<OculusTagDebugElements>;

	public:

		/**
		 * Definition of several debug elements.
		 */
		enum ElementId : uint32_t
		{
			/// An invalid element id.
			EI_INVALID = 0u,

			/// Line segments that are extracted for detection of boundary patterns
			EI_BOUNDARY_PATTERN_LINE_SEGMENTS,
			/// Raw L-shapes that are used for detection of boundary patterns
			EI_BOUNDARY_PATTERN_LSHAPES_INITIAL,
			/// Final L-shapes that are used for the detection of boundary patterns
			EI_BOUNDARY_PATTERN_LSHAPES_FINAL,
			/// Detected boundary patterns
			EI_BOUNDARY_PATTERN_DETECTIONS,

			/// Rectified images of detected tags in the tracker, must be used in conjunction with a tag ID (i.e. as a hierarchy)
			EI_DETECTOR_RECTIFIED_TAG,

			/// Rectified images of detected tags in the tracker, must be used in conjunction with a tag ID (i.e. as a hierarchy)
			EI_TRACKER_CORNER_TRACKING,
			/// Rectified images of detected tags in the tracker
			EI_TRACKER_RECTIFIED_TAG
		};

		/**
		 * This class implements a scoped hierarchy.
		 * The hierarchy exists as long as this object exists.
		 */
		class ScopedHierarchy : public ScopedHierarchyBase
		{
			public:

				/**
				 * Creates a new scoped object and pushes the given hierarchy.
				 * @param hierarchy The hierarchy to be pushed, must be valid
				 */
				inline ScopedHierarchy(const std::string& hierarchy);
		};
};

inline OculusTagDebugElements::ScopedHierarchy::ScopedHierarchy(const std::string& hierarchy) :
	ScopedHierarchyBase(get(), hierarchy)
{
	// nothing to do here
}

#endif // OCN_OCULUSTAG_DEBUGGING_ENABLED

} // namespace OculusTags

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAGDEBUGELEMENTS_H
