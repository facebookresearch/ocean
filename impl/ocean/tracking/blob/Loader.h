// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_BLOB_LOADER_H
#define META_OCEAN_TRACKING_BLOB_LOADER_H

#include "ocean/tracking/blob/Blob.h"

#include "ocean/cv/detector/blob/BlobFeature.h"

namespace Ocean
{

namespace Tracking
{

namespace Blob
{

/**
 * This class implements a loader for Blob features.
 * @ingroup trackingblob
 */
class OCEAN_TRACKING_BLOB_EXPORT Loader
{
	public:

		/**
		 * Loads features from a given file.
		 * @param file File holding the features
		 * @param features Resulting loaded features
		 * @return True, if succeeded
		 */
		static bool loadFile(const std::string& file, CV::Detector::Blob::BlobFeatures& features);
};

}

}

}

#endif // META_OCEAN_TRACKING_BLOB_LOADER_H
