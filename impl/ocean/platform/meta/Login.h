// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_LOGIN_H
#define META_OCEAN_PLATFORM_META_LOGIN_H

#include "ocean/platform/meta/Meta.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

#include <functional>

#ifdef __OBJC__
	// Forward declaration.
	@class FBUserSession;
#endif

namespace Ocean
{

namespace Platform
{

namespace Meta
{

/**
 * This class provides a singleton-based access to Meta first party login.
 * The login information can be provided as userId/userToken pair or as FBUserSession.
 * @ingroup platformmeta
 */
class OCEAN_PLATFORM_META_EXPORT Login : public Singleton<Login>
{
	friend class Singleton<Login>;

	public:

		/**
		 * Definition of individual login types.
		 */
		enum LoginType : uint32_t
		{
			/// Unknown login type.
			LT_UNKNOWN,
			/// Login for Meta account.
			LT_META,
			/// Login for Facebook account.
			LT_FACEBOOK,
			/// Login for Instagram account.
			LT_INSTAGRAM,
			/// Login for Oculus account.
			LT_OCULUS
		};

		/**
		 * Definition of a vector holding login types.
		 */
		typedef std::vector<LoginType> LoginTypes;

		/**
		 * Definition of an unordered set holding login types.
		 */
		typedef std::unordered_set<LoginType> LoginTypeSet;

		/**
		 * This class implements a wrapper around a FBUserSession object.
		 * The wrapper is empty on non-ObjC platforms.
		 */
		class FBUserSessionWrapper
		{
			public:

				/**
				 * Default constructor.
				 */
				FBUserSessionWrapper() = default;
#ifdef __OBJC__

				/**
				 * Creates a new object with given valid FBUserSession object.
				 * @param fbUserSession The FBUserSession object to wrap, must be valid
				 */
				explicit inline FBUserSessionWrapper(FBUserSession* fbUserSession);

			public:

				/// The wrapped FBUserSession object.
				FBUserSession* fbUserSession_ = nullptr;
#endif
		};

		/**
		 * Definition of a callback function returning a FBUserSession object.
		 */
		typedef std::function<FBUserSessionWrapper()> FBUserSessionFunction;

	protected:

		/**
		 * This class holds the relevant information for a login.
		 */
		class LoginData
		{
			public:

				/// The user id, if known.
				std::string userId_;

				/// The user token, if known.
				std::string userToken_;

				/// The callback function returning the FBUserSession object.
				FBUserSessionFunction fbUserSessionFunction_;
		};

		/**
		 * Definition of an unordered map mapping login types to login information.
		 */
		typedef std::unordered_map<LoginType, LoginData> LoginMap;

	public:

		/**
		 * Sets or updates a login.
		 * @param loginType The type of the login
		 * @param userId The id of the user, if known
		 * @param userToken The access token, if known
		 * @param fbUserSessionFunction The callback function to return the FBUserSession object, if known
		 */
		void setLogin(const LoginType loginType, std::string&& userId, std::string&& userToken, FBUserSessionFunction&& fbUserSessionFunction = FBUserSessionFunction());

		/**
		 * Returns whether login data exists for a specific login type.
		 * @param loginType The type of the login to check
		 * @return True, if so
		 */
		bool hasLogin(const LoginType loginType) const;

		/**
		 * Returns the login data for a specified login type.
		 * @param loginType The login type for which the login data will be returned, must be valid
		 * @param userId The resulting id of the user, if known
		 * @param userToken The resulting access token of the user, if known
		 * @return True, if the login type exists
		 */
		bool login(const LoginType loginType, std::string& userId, std::string& userToken) const;

		/**
		 * Returns the user id for a specified login type.
		 * @param loginType The login type for which the user id will be returned, must be valid
		 * @return The resulting id of the user, empty if unknown or if the login does not exist
		 */
		std::string userId(const LoginType loginType) const;

		/**
		 * Returns the access token for a specified login type.
		 * @param loginType The login type for which the token will be returned, must be valid
		 * @return The resulting access token, empty if unknown or if the login does not exist
		 */
		std::string userToken(const LoginType loginType) const;

		/**
		 * Returns the types of all logins currently available.
		 * @return The login types
		 */
		LoginTypeSet loginTypes() const;

#ifdef __OBJC__

		/**
		 * Returns the FBUserSession object for a specified login type.
		 * @param loginType The login type for which the session will be returned, must be valid
		 * @return The FBUserSession object, nullptr if unknown
		 */
		inline FBUserSession* userSession(const LoginType loginType) const;

#endif

	protected:

		/**
		 * Protected default constructor.
		 */
		Login();

	protected:

		/// The map holding all logins.
		LoginMap loginMap_;

		/// The lock for this login object.
		mutable Lock lock_;
};

#ifdef __OBJC__

inline Login::FBUserSessionWrapper::FBUserSessionWrapper(FBUserSession* fbUserSession) :
	fbUserSession_(fbUserSession)
{
	ocean_assert(fbUserSession_ != nullptr);
}

inline FBUserSession* Login::userSession(const LoginType loginType) const
{
	ocean_assert(loginType != LT_UNKNOWN);

	const ScopedLock scopedLock(lock_);

	LoginMap::const_iterator i = loginMap_.find(loginType);
	if (i != loginMap_.cend())
	{
		const FBUserSessionFunction& fbUserSessionFunction = i->second.fbUserSessionFunction_;

		if (fbUserSessionFunction)
		{
			FBUserSessionWrapper fbUserSessionWrapper = fbUserSessionFunction();

			return fbUserSessionWrapper.fbUserSession_;
		}
	}

	return nullptr;
}

#endif // __OBJC__

}

}

}

#endif // META_OCEAN_PLATFORM_META_LOGIN_H
