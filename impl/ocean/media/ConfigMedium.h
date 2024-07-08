/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_CONFIG_MEDIUM_H
#define META_OCEAN_MEDIA_CONFIG_MEDIUM_H

#include "ocean/media/Media.h"
#include "ocean/media/Medium.h"
#include "ocean/media/MediumRef.h"

#include <vector>

namespace Ocean
{

namespace Media
{

// Forward declaration.
class ConfigMedium;

/**
 * Definition of a smart medium reference holding a config medium object.
 * @see SmartMediumRef, ConfigMedium.
 * @ingroup media
 */
typedef SmartMediumRef<ConfigMedium> ConfigMediumRef;

/**
 * This class is the base class for all mediums with advanced configuration possibilities.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT ConfigMedium : virtual public Medium
{
	public:

		/**
		 * Definition of a vector holding configuration names.
		 */
		typedef std::vector<std::string> ConfigNames;

	public:

		/**
		 * Starts a configuration possibility.
		 * @param name The name of the configuration to start
		 * @param data Possible configuration data like gui handles.
		 * @return True, if succeeded
		 */
		virtual bool configuration(const std::string& name, long long data);

		/**
		 * Returns the names of the configuration possibilities.
		 * @return Names of configurations
		 */
		virtual ConfigNames configs() const;

	protected:

		/**
		 * Creates a new ConfigMedium object by a given url.
		 * @param url Url of medium
		 */
		explicit ConfigMedium(const std::string& url);
};

}

}

#endif // META_OCEAN_MEDIA_CONFIG_MEDIUM_H
