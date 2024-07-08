/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#if defined(OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)

#include "ocean/base/DebugElements.h"
#include "ocean/base/Singleton.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

/**
 * This class implements debug elements for the development of the QR code detector
 * @ingroup cvdetectorqrcodes
 */
class QRCodeDebugElements :
	public DebugElements,
	public Singleton<QRCodeDebugElements>
{
	friend class Singleton<QRCodeDebugElements>;

	public:

		/**
		 * Definition of available debug elements.
		 */
		enum ElementId : unsigned int
		{
			/// Indicator for an invalid element
			EI_INVALID = 0u,

			/// Indicator for the source image as Y8
			EI_SOURCE_IMAGE_Y8,
			/// Indicator for the source image as RGB24
			EI_SOURCE_IMAGE_RGB24,

			/// Indicator for images with individual finder patterns drawn into it
			EI_FINDER_PATTERNS,
			/// Indicator for images with finder patterns grouped into triplets drawn into it
			EI_FINDER_PATTERN_TRIPLETS,

			/// Indicator for images with all of the initial poses of QR code candidates
			EI_COMPUTE_POSES,

			/// Indicator for images with current detections
			EI_DETECTION,
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

inline QRCodeDebugElements::ScopedHierarchy::ScopedHierarchy(const std::string& hierarchy) :
	ScopedHierarchyBase(get(), hierarchy)
{
	// Nothing to do here.
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED
