/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_NETWORK_NETWORK_RESOURCE_H
#define FACEBOOK_NETWORK_NETWORK_RESOURCE_H

#include "ocean/network/Network.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

namespace Ocean
{

namespace Network
{

/**
 * This class implements a network resource manager object for platforms that rely on specific network/socket resource management.
 * @ingroup network
 */
class OCEAN_NETWORK_EXPORT NetworkResource
{
	protected:

		/**
		 * The manager of the parent resource object.
		 */
		class Manager : public Singleton<Manager>
		{
			friend class Singleton<Manager>;

			public:

				/**
				 * Increases the resource.
				 * @return True, if the source is use for the first time
				 */
				bool increase();

				/**
				 * Decreases the resource.
				 * @return True, if the resource is not used anymore
				 */
				bool decrease();

			protected:

				/**
				 * Creates a new manager.
				 */
				Manager();

			protected:

				/// The resource counter.
				unsigned int resourceCounter_ = 0u;

				/// The lock of the manager.
				Lock lock_;
		};

	public:

		/**
		 * Creates a new resource management object.
		 */
		NetworkResource();

		/**
		 * Destructs an resource management object.
		 */
		~NetworkResource();
};

}

}

#endif // FACEBOOK_NETWORK_NETWORK_RESOURCE_H
