/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#if defined(OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#include "ocean/base/DebugElements.h"
#include "ocean/base/Singleton.h"

namespace Ocean
{

namespace Tracking
{

namespace QRCodes
{

/**
 * This class implements debug elements for the development of the 6DOF QR code tracker
 * @ingroup trackingqrcodes
 */
class QRCodesDebugElements :
	public DebugElements,
	public Singleton<QRCodesDebugElements>
{
	friend class Singleton<QRCodesDebugElements>;

	public:

		/**
		 * Definition of available debug elements.
		 */
		enum ElementId : unsigned int
		{
			/// Indicator for an invalid element
			EI_INVALID = 0u,

			/// Frame from the input cameras
			EI_CAMERA_FRAMES,

			/// Frame from the input cameras
			EI_TRACKING_FINDER_PATTERNS,

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

inline QRCodesDebugElements::ScopedHierarchy::ScopedHierarchy(const std::string& hierarchy) :
	ScopedHierarchyBase(get(), hierarchy)
{
	// Nothing to do here.
}

} // namespace QRCodes

} // namespace Tracking

} // namespace Ocean

#endif // OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED
