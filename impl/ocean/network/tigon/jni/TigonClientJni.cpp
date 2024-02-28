// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/network/tigon/jni/TigonClientJni.h"
#include "ocean/network/tigon/TigonClient.h"

#include <AsyncExecutor/SerialExecutor.h>

#include <TigonIface/TigonServiceHolder.h>

#include <jni.h>
#include <jniexecutors/JExecutor.h>

#include <fbjni/fbjni.h>

using namespace Ocean;

/**
 * This class implements a custom executor forwarding the tasks to the Java side.
 */
class OceanSerialExecutor : public facebook::mobile::xplat::executor::SerialExecutor
{
	public:

		OceanSerialExecutor(facebook::jni::alias_ref<facebook::executor4a::JExecutor> jExecutor) :
			jExecutor_(facebook::jni::make_global(jExecutor)),
			executorName_(facebook::jni::make_global(facebook::jni::make_jstring("OceanSerialExecutor")))
		{
			// nothing to do here
		}

		~OceanSerialExecutor() override
		{
			// nothing to do here
		}

		void add(folly::Func task) override
		{
			// This may be called from native threads created by
			// Omnistore, so make sure we're attached to the JavaVM
			facebook::jni::ThreadScope::WithClassLoader([&] {
				jExecutor_->execute(facebook::executor4a::NativeRunnable::create(executorName_, std::move(task)));
			});
		}

	private:

		facebook::jni::global_ref<facebook::executor4a::JExecutor> jExecutor_;

		facebook::jni::global_ref<jstring> executorName_;
};

jboolean Java_com_facebook_ocean_network_tigon_TigonClientJni_setTigonService(JNIEnv* env, jobject javaThis, jobject tigonServiceHolder, jobject executor)
{
	ocean_assert(env != nullptr && javaThis != nullptr && tigonServiceHolder != nullptr);

	auto jTigonServiceHolder = facebook::jni::wrap_alias(static_cast<facebook::tigon::iface::TigonServiceHolder::jhybridobject>(tigonServiceHolder));
	std::shared_ptr<facebook::tigon::TigonService> tigonService = jTigonServiceHolder->cthis()->getTigonService();

	if (!tigonService)
	{
		return false;
	}

	Network::Tigon::TigonClient::get().setTigonService(std::move(tigonService));

	if (executor != nullptr)
	{
		facebook::jni::alias_ref<facebook::executor4a::JExecutor> jExecutor = facebook::jni::wrap_alias(static_cast<facebook::executor4a::JExecutor::javaobject>(executor));
		std::shared_ptr<OceanSerialExecutor> oceanSerialExecutor = std::make_shared<OceanSerialExecutor>(jExecutor);

		Network::Tigon::TigonClient::get().setExecutor(std::move(oceanSerialExecutor));
	}

	// invoking a dummy http request to ensure that the Tigon Client can be called from any new thread
	Network::Tigon::TigonClient::get().httpRequest("https://facebook.com");

	return true;
}

jboolean Java_com_facebook_ocean_network_tigon_TigonClientJni_releaseTigonService(JNIEnv* env, jobject javaThis)
{
	ocean_assert(env != nullptr && javaThis != nullptr);

	Network::Tigon::TigonClient::get().release();

	return true;
}
