// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/network/verts/Manager.h"
#include "ocean/network/verts/NodeSpecification.h"

#include <verts/client/capi.h>

#include "verts/shared/ctypes.h"

namespace Ocean
{

namespace Network
{

namespace Verts
{

namespace
{

void vertsCustomLogger(VertsLogLevel vertsLogLevel, const verts_string message, void* /*context*/)
{
	switch (vertsLogLevel)
	{
		case VertsLogLevel::VertsLogLevel_Fatal:
		case VertsLogLevel::VertsLogLevel_Error:
			Log::error() << "Verts: " << message.content;
			return;

		case VertsLogLevel::VertsLogLevel_Warn:
			Log::warning() << "Verts: " << message.content;
			return;

		case VertsLogLevel::VertsLogLevel_Info:
			Log::info() << "Verts: " << message.content;
			return;
	}

	ocean_assert(false && "Unknown log level!");
	Log::error() << "Verts: " << message.content;
}

}

Manager::Manager()
{
	// nothing to do here
}

Manager::~Manager()
{
	release();
}

bool Manager::initialize(const std::string& appName, const std::string& accessToken)
{
	ocean_assert(!appName.empty());
	ocean_assert(!accessToken.empty());

	if (appName.empty() || accessToken.empty())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (isInitialized_)
	{
		ocean_assert(false && "Verts already initialize");
		return false;
	}

	verts_init_options* vertsInitOptions = verts_get_default_init_options();

	verts_init_options_set_app_name(vertsInitOptions, verts_make_string(appName.c_str()));
	verts_init_options_set_app_version(vertsInitOptions, verts_make_string("1"));
	verts_init_options_set_access_token(vertsInitOptions, verts_make_string(accessToken.c_str()));
	verts_init_options_set_log_to_stderr(vertsInitOptions, false);
	verts_init_options_set_init_glog(vertsInitOptions, false);
	verts_init_options_set_soft_errors(vertsInitOptions, false);

#ifdef OCEAN_DEBUG_INTENSIVE
	verts_set_verbose_log_level(1);
#endif

	verts_init(vertsInitOptions);

	verts_init_options_destroy(vertsInitOptions);
	verts_set_custom_logger(vertsCustomLogger, nullptr);

	if (verts_is_initialized())
	{
		const verts_string vertsLibVersion = verts_lib_version();
		const verts_string vertsProtocolVersion = verts_protocol_version();

		Log::info() << "VERTS initialized for app '" << appName << "', with access token '" << accessToken.substr(0, 8) << "...', library version " << vertsLibVersion.content << ", protocol version " << vertsProtocolVersion.content;

		isInitialized_ = true;
		appName_ = appName;
	}
	else
	{
		Log::error() << "VERTS: Failed to initialized manager";
	}

	NodeSpecification::registerInternalNodeSpecifications();

	return isInitialized_;
}

SharedDriver Manager::driver(const std::string& zoneName, const std::string& host, const unsigned int timeout)
{
	const ScopedLock scopedLock(lock_);

	if (!isInitialized_)
	{
		ocean_assert(false && "VERTS: The manager is not initialized");
		return nullptr;
	}

	DriverMap::const_iterator iDriver = driverMap_.find(zoneName);

	if (iDriver != driverMap_.cend())
	{
		SharedDriver sharedDriver(iDriver->second.lock());

		if (sharedDriver)
		{
			ocean_assert(sharedDriver->host() == host);
			if (sharedDriver->host() != host)
			{
				Log::error() << "VERTS: Driver for zone '" << zoneName << "' exists already using different host";
			}

			if (sharedDriver->timeout() != timeout)
			{
				Log::error() << "VERTS: Driver for zone '" << zoneName << "' exists already using different timeout";
			}

			return sharedDriver;
		}

		driverMap_.erase(iDriver);
	}

	SharedDriver driver(new Driver(zoneName, host, timeout));

	if (!driver->isValid())
	{
		return nullptr;
	}

	driverMap_.emplace(zoneName, driver);

	return driver;
}

void Manager::update(const Timestamp& timestamp)
{
	const ScopedLock scopedLock(lock_);

	updateRateCalculator_.addOccurance(timestamp);

	if (!isInitialized_)
	{
		return;
	}

	for (DriverMap::iterator iDriver = driverMap_.begin(); iDriver != driverMap_.end(); /*noop*/)
	{
		const SharedDriver sharedDriver(iDriver->second.lock());

		if (sharedDriver)
		{
			sharedDriver->update(timestamp);
			++iDriver;
		}
		else
		{
			iDriver = driverMap_.erase(iDriver);
		}
	}

#ifdef OCEAN_DEBUG
	double rate;
	if (updateRateCalculator_.rateEveryNSeconds(timestamp, rate, 10.0, Timestamp(true)))
	{
		Log::debug() << "Verts update rate: " << rate << " Hz";
	}
#endif
}

void Manager::release()
{
	const ScopedLock scopedLock(lock_);

	driverMap_.clear();
}

}

}

}
