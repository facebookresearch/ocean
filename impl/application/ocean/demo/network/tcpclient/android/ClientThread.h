/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_NETWORK_TCP_CLIENT_ANDROID_CLIENT_THREAD_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_NETWORK_TCP_CLIENT_ANDROID_CLIENT_THREAD_H

#include "application/ocean/demo/network/tcpclient/android/DemoNetworkTCPClient.h"

#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

/**
 * Simple thread class implemented as singleton.
 */
class ClientThread :
	public Ocean::Singleton<ClientThread>,
	protected Ocean::Thread
{
	friend class Ocean::Singleton<ClientThread>;

	public:

		/**
		 * Connects the client with a specified server.
		 * @param hostAddress The address of the server
		 * @param readablePort The post of the server (readable)
		 * @return True, if succeeded
		 */
		bool connect(const std::string& hostAddress, const unsigned short readablePort);

	protected:

		/**
		 * Creates a new object.
		 */
		ClientThread();

		/**
		 * Destructs a new object.
		 */
		virtual ~ClientThread();

		/**
		 * Thread run function.
		 * @see Thread::threadRun().
		 */
		virtual void threadRun();

		/**
		 * Event function for new data received from the TCP server.
		 * @param data The data which has been received
		 * @param size The number of bytes which have been received
		 */
		void onReceiveData(const void* data, const size_t size);

		/**
		 * Event function for a disconnect from the server.
		 */
		void onDisconnect();

	protected:

		/// The host address.
		std::string threadHostAddress;

		/// The host readable port.
		unsigned short threadHostPort;

		/// The number of received bytes.
		size_t threadReceivedBytes;

		/// The timestamp of the first received byte.
		Ocean::Timestamp threadReceivedTimestamp;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_NETWORK_TCP_CLIENT_ANDROID_CLIENT_THREAD_H
