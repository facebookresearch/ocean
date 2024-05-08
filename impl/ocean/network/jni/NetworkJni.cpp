/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/jni/NetworkJni.h"

#include "ocean/network/Resolver.h"

using namespace Ocean;

jboolean Java_com_meta_ocean_network_NetworkJni_setCurrentLocalIPAddress(JNIEnv* env, jobject javaThis, jint localIPAddress)
{
	ocean_assert(env != nullptr);

    const Network::Address4 localAddress((unsigned int)(localIPAddress), Network::Address4::TYPE_BIG_ENDIAN);

	Network::Resolver::get().addLocalAddress(localAddress);

    Log::info() << "Added local IP address: " << localAddress.readable();

	return true;
}
