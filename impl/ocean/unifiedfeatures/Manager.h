// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"
#include "ocean/unifiedfeatures/UnifiedObject.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Singleton.h"

namespace Ocean
{

namespace UnifiedFeatures
{

/**
 * This class is the manager for all unified objects.
 * As unified objects cannot be created directly this manager is necessary to create individual unified objects.<br>
 * @see UnifiedObject
 * @ingroup unifiedfeatures
 */
class OCEAN_UNIFIEDFEATURES_EXPORT Manager : public Singleton<Manager>
{
	friend class Singleton<Manager>;

	public:

		/**
		 * Definition of a callback function to create new unified objects
		 */
		typedef Callback<std::shared_ptr<UnifiedObject>, const std::shared_ptr<Parameters>&> CreateUnifiedObjectCallback;

	private:

		/**
		 * Definition of a pair combining a callback function with a reference counter.
		 */
		typedef std::pair<CreateUnifiedObjectCallback, unsigned int> CreateUnifiedObjectCallbackCounterPair;

		/**
		 * Definition of a map holding pairs of callback and reference counters.
		 */
		typedef std::unordered_map<std::string, CreateUnifiedObjectCallbackCounterPair> UnifiedObjectMap;

	public:

		/**
		 * Creates an instance of the specified unified object
		 * @param name The name of the unified object, must be valid, cf. UnifiedObject::name()
		 * @param parameters Optional parameters that can be used to initialize the object.
		 * @return A pointer to the newly created object. Will be `nullptr` if no class with the specified object has been found.
		 * @sa registerUnifiedObject(), unregisterUnifiedObject()
		 */
		std::shared_ptr<UnifiedObject> create(const std::string& name, const std::shared_ptr<Parameters>& parameters = nullptr);

		/**
		 * Returns the names currently registered libraries.
		 * @return Registered unified objects
		 */
		std::vector<std::string> unifiedObjectNames() const;

		/**
		 * Releases all registered libraries.
		 * This function should be called once before the application is shutting down.
		 * However this function should be called after all unified objects have been released.
		 */
		void release();

		/**
		 * Registers a new unified object.
		 * With each register call, the reference counter for a specific unified object will be incremented.
		 * Each call to registerUnifiedObject() needs to be balanced with a corresponding call of unregisterUnifiedObject() before shutting down.
		 * @param name The name of the unified object to register, must be valid
		 * @param createUnifiedObjectCallback The callback function that is able to create the unified object called `name`
		 * @return True, if the unified object has not been registered before
		 * @see unregisterUnifiedObject().
		 */
		bool registerUnifiedObject(const std::string& name, const CreateUnifiedObjectCallback& createUnifiedObjectCallback);

		/**
		 * Unregisters an unified object.
		 * With each unregister call, the reference counter for a specific unified object will be decremented and removed from the system if the counter reaches zero.
		 * Each call to registerUnifiedObject() needs to be balanced with a corresponding call of unregisterUnifiedObject() before shutting down.
		 * @param name The name of the unified object to unregister, must be valid
		 * @return True, if the unified object was actually removed from the system (as the reference counter reached zero); False, if the unified object is still used by someone else
		 * @see registerUnifiedObject().
		 */
		bool unregisterUnifiedObject(const std::string& name);

	protected:

		/**
		 * Creates a new manager object.
		 */
		Manager();

		/**
		 * Destructs the manager, called by the singleton object.
		 * Beware: The release function should be called before the singleton invokes the destructor.
		 * In common cases the singleton is released to late for internal unified object objects.
		 */
		virtual ~Manager();

	protected:

		/// Registered unified objects.
		UnifiedObjectMap unifiedObjectMap_;

		/// Lock for the libraries.
		mutable Lock lock_;
};

} // namespace UnifiedFeatures

} // namespace Ocean
