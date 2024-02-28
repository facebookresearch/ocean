// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/network/jni/NetworkJni.h"

#include "ocean/network/Resolver.h"

using namespace Ocean;

jboolean Java_com_facebook_ocean_network_NetworkJni_setCurrentLocalIPAddress(JNIEnv* env, jobject javaThis, jint localIPAddress)
{
	ocean_assert(env != nullptr);

    const Network::Address4 localAddress((unsigned int)(localIPAddress), Network::Address4::TYPE_BIG_ENDIAN);

	Network::Resolver::get().addLocalAddress(localAddress);

    Log::info() << "Added local IP address: " << localAddress.readable();

	return true;
}
