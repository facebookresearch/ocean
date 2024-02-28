// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_VRS_VRS_H
#define META_OCEAN_MEDIA_VRS_VRS_H

#include "ocean/media/Media.h"
#include "ocean/media/StereoImageProviderInterface.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SampleMap.h"

namespace Ocean
{

namespace Media
{

namespace VRS
{

/**
 * @ingroup media
 * @defgroup mediavrs Ocean Media VRS Library
 * @{
 * The Ocean Media VRS Library provides media functionalities to access media information from VRS files.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Media::VRS Namespace of the Media VRS library.<p>
 * The Namespace Ocean::Media::VRS is used in the entire Ocean Media VRS Library.
 */

// Defines OCEAN_MEDIA_VRS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_MEDIA_VRS_EXPORT
		#define OCEAN_MEDIA_VRS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_MEDIA_VRS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_MEDIA_VRS_EXPORT
#endif

/**
 * Returns the name of this media VRS library.
 * @ingroup mediavrs
 */
std::string nameVRSLibrary();

/**
 * Creates a stereo image provider for a given VRS file.
 * This function is simply a trivial helper function and allows to avoid including the header file with external dependencies.
 * @param vrsFilename The filename of the VRS file from which the stereo images will be provided, must valid
 * @param posesFilename The filename of the poses file which contains the transformations between device and camera, an empty string to try to read the transformation from the VRS file
 * @return The stereo image provider object
 */
std::shared_ptr<StereoImageProviderInterface> createStereoImageProvider(const std::string& vrsFilename, const std::string& posesFilename = std::string());

/**
 * Translates the string of a type id of a recordable to the corresponding value.
 * @param recordableTypeId The string of the type id to translate
 * @return The resulting type id value, -1 if unknown
 */
int translateRecordableTypeid(const std::string& recordableTypeId);

/**
 * Extracts string values associated with a recordable from a VRS file.
 * This function is simply a trivial helper function and allows to avoid including the header file with external dependencies.
 * @param vrsFilename The filename of the VRS file from which the stereo images will be provided, must valid
 * @param recordableTypeId The type id of the recordable
 * @param name The name of the recordable in the VRS stream, must be valid
 * @param values The map mapping timestamps to values
 * @return True, if succeeded
 */
bool extractValuesFromVRS(const std::string& vrsFilename, const int recordableTypeId, const std::string& name, std::map<double, std::string>& values);

/**
 * Extracts HomogenousMatrix4 values associated with a recordable from a VRS file.
 * This function is simply a trivial helper function and allows to avoid including the header file with external dependencies.
 * @param vrsFilename The filename of the VRS file from which the stereo images will be provided, must valid
 * @param recordableTypeId The type id of the recordable
 * @param name The name of the recordable in the VRS stream, must be valid
 * @param values The map mapping timestamps to values
 * @return True, if succeeded
 */
bool extractValuesFromVRS(const std::string& vrsFilename, const int recordableTypeId, const std::string& name, SampleMap<HomogenousMatrixD4>& values);

/**
 * Extracts HomogenousMatrices4 values associated with a recordable from a VRS file.
 * This function is simply a trivial helper function and allows to avoid including the header file with external dependencies.
 * @param vrsFilename The filename of the VRS file from which the stereo images will be provided, must valid
 * @param recordableTypeId The type id of the recordable
 * @param name The name of the recordable in the VRS stream, must be valid
 * @param values The map mapping timestamps to values
 * @return True, if succeeded
 */
bool extractValuesFromVRS(const std::string& vrsFilename, const int recordableTypeId, const std::string& name, std::map<double, HomogenousMatricesD4>& values);

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this media library at the global library manager.
 * This function calls VRSLibrary::registerLibrary() only.
 * @ingroup mediavrs
 */
void registerVRSLibrary();

/**
 * Unregisters this media library at the global library manager.
 * This function calls VRSLibrary::unregisterLibrary() only.
 * @return True, if succeeded
 * @ingroup mediavrs
 */
bool unregisterVRSLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_MEDIA_VRS_VRS_H
