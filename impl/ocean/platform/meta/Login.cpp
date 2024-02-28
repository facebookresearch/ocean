// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/Login.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

Login::Login()
{
	// nothing to do here
}

void Login::setLogin(const LoginType loginType, std::string&& userId, std::string&& userToken, FBUserSessionFunction&& fbUserSessionFunction)
{
	ocean_assert(loginType != LT_UNKNOWN);

	const ScopedLock scopedLock(lock_);

	if (userId.empty() && userToken.empty() && fbUserSessionFunction == nullptr)
	{
		loginMap_.erase(loginType);
	}
	else
	{
		LoginData& loginData = loginMap_[loginType];

		loginData.userId_ = std::move(userId);
		loginData.userToken_ = std::move(userToken);
		loginData.fbUserSessionFunction_ = std::move(fbUserSessionFunction);
	}
}

bool Login::hasLogin(const LoginType loginType) const
{
	ocean_assert(loginType != LT_UNKNOWN);

	const ScopedLock scopedLock(lock_);

	return loginMap_.find(loginType) != loginMap_.cend();
}

bool Login::login(const LoginType loginType, std::string& userId, std::string& userToken) const
{
	ocean_assert(loginType != LT_UNKNOWN);

	const ScopedLock scopedLock(lock_);

	LoginMap::const_iterator i = loginMap_.find(loginType);
	if (i == loginMap_.cend())
	{
		return false;
	}

	userId = i->second.userId_;
	userToken = i->second.userToken_;

	return true;
}

std::string Login::userId(const LoginType loginType) const
{
	ocean_assert(loginType != LT_UNKNOWN);

	const ScopedLock scopedLock(lock_);

	LoginMap::const_iterator i = loginMap_.find(loginType);
	if (i == loginMap_.cend())
	{
		return std::string();
	}

	return i->second.userId_;
}

std::string Login::userToken(const LoginType loginType) const
{
	ocean_assert(loginType != LT_UNKNOWN);

	const ScopedLock scopedLock(lock_);

	LoginMap::const_iterator i = loginMap_.find(loginType);
	if (i == loginMap_.cend())
	{
		return std::string();
	}

	return i->second.userToken_;
}

Login::LoginTypeSet Login::loginTypes() const
{
	const ScopedLock scopedLock(lock_);

	LoginTypeSet result;
	result.reserve(loginMap_.size());

	for (LoginMap::const_iterator i = loginMap_.cbegin(); i != loginMap_.cend(); ++i)
	{
		result.emplace(i->first);
	}

	return result;
}

} // namespace Meta

} // namespace Platform

} // namespace Ocean
