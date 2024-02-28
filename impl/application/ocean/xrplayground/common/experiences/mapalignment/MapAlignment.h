// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MAP_ALIGNMENT_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MAP_ALIGNMENT_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/io/Bitstream.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements the base class for all map alignment experiences.
 * @ingroup xrplayground
 */
class MapAlignment
{
	protected:

		/// The unique tag for a reloc-based transformation.
		static constexpr uint64_t transformationRelocTag_ = IO::Tag::string2tag("_RLOCTM_");

		/// The unique tag for a slam-based transformation.
		static constexpr uint64_t transformationSlamTag_ = IO::Tag::string2tag("_SLAMTM_");

		/// The unique tag for a map.
		static constexpr uint64_t mapTag_ = IO::Tag::string2tag("_OCNMAP_");

		/// The unique tag for object points.
		static constexpr uint64_t objectPointsTag_ = IO::Tag::string2tag("_OCNOPT_");

		/// The unique tag for a network port.
		static constexpr uint64_t portTag_ = IO::Tag::string2tag("_OCNPRT_");

	protected:

		/**
		 * Writes a 6-DOF transformation to a bitstream.
		 * @param transformation The transformation to write, must be valid
		 * @param slamBased True, if the given transformation is SLAM-based; False, if the transformation is reloc-based
		 * @param bitstream The output stream to which the transformation will be written
		 * @return True, if succeeded
		 */
		static bool writeTransformationToStream(const HomogenousMatrix4& transformation, const bool slamBased, IO::OutputBitstream& bitstream);

		/**
		 * Writes 3D object points to a bitstream.
		 * @param objectPoints The 3D object points
		 * @param objectPointIds The ids of the 3D object points, one for each object point
		 * @param bitstream The input stream to which the points will be written
		 * @return True, if succeeded
		 */
		static bool writeObjectPointsToStream(const Vectors3& objectPoints, const Indices32& objectPointIds, IO::OutputBitstream& bitstream);

		/**
		 * Reads a 6-DOF transformation from a bitstream.
		 * @param bitstream The input stream from which the transformation will be read
		 * @param transformation The resulting transformation
		 * @param slamBased True, if the resulting transformation is SLAM-based; False, if the transformation is reloc-based
		 * @return True, if succeeded
		 */
		static bool readTransformationFromStream(IO::InputBitstream& bitstream, HomogenousMatrix4& transformation, bool& slamBased);

		/**
		 * Reads 3D object points from a bitstream.
		 * @param bitstream The input stream from which the points will be read
		 * @param objectPoints The resulting 3D object points
		 * @param objectPointIds The resulting ids of the object points, one for each object point
		 * @return True, if succeeded
		 */
		static bool readObjectPointsFromStream(IO::InputBitstream& bitstream, Vectors3& objectPoints, Indices32& objectPointIds);
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MAP_ALIGNMENT_H
