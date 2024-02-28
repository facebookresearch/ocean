// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/base/Scheduler.h"

namespace Ocean
{

Scheduler::Scheduler() :
	Thread("Scheduler thread")
{
	// nothing to do here
}

Scheduler::~Scheduler()
{
	ocean_assert(callbacks.isEmpty());

	stopThreadExplicitly();
}

void Scheduler::registerFunction(const Callback& callback)
{
	ocean_assert(callback);

	callbacks.addCallback(callback);

	if (!isThreadInvokedToStart())
		startThread();
}

void Scheduler::unregisterFunction(const Callback& callback)
{
	ocean_assert(callback);

	callbacks.removeCallback(callback);
}

void Scheduler::threadRun()
{
	while (!shouldThreadStop())
	{
		callbacks();
		sleep(1);
	}
}

}
