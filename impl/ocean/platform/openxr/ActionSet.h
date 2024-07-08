/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_OPENXR_ACTION_SET_H
#define META_OCEAN_PLATFORM_OPENXR_ACTION_SET_H

#include "ocean/platform/openxr/OpenXR.h"
#include "ocean/platform/openxr/Session.h"

#include "ocean/base/Lock.h"

namespace Ocean
{

namespace Platform
{

namespace OpenXR
{

// Foward declaration.
class ActionSet;

/**
 * Definition of a shared pointer holding an ActionSet object.
 * @see ActionSet.
 * @ingroup platformopenxr
 */
typedef std::shared_ptr<ActionSet> SharedActionSet;

/**
 * Definition of a vector holding ActionSet objects.
 * @see ActinoSet
 * @ingroup platformopenxr
 */
typedef std::vector<ActionSet> ActionSets;

/**
 * Definition of a vector holding a SharedActionSet object.
 * @see SharedActionSet.
 * @ingroup platformopenxr
 */
typedef std::vector<SharedActionSet> SharedActionSets;

/**
 * This class implements a wrapper for an OpenXR action set.
 * @ingroup platformopenxr
 */
class OCEAN_PLATFORM_OPENXR_EXPORT ActionSet final
{
	public:

		/**
		 * Definition of an id for an action.
		 */
		typedef uint32_t ActionId;

		/**
		 * Definition of an unordered map mapping action ids to XrAction objects.
		 */
		typedef std::unordered_map<ActionId, XrAction> ActionIdMap;

		/**
		 * Definition of a vector holding strings.
		 */
		typedef std::vector<std::string> Paths;

		/// An invalid action id.
		static constexpr ActionId invalidActionId_ = ActionId(-1);

	protected:

		/**
		 * Definition of a vector holding XrActionSuggestedBinding objects.
		 */
		typedef std::vector<XrActionSuggestedBinding> XrActionSuggestedBindings;

	public:

		/**
		 * Default constructor.
		 */
		ActionSet() noexcept = default;

		/**
		 * Move constructor.
		 * @param actionSet The action set to be moved
		 */
		inline ActionSet(ActionSet&& actionSet) noexcept;

		/**
		 * Destructs this action set and releases all resources.
		 */
		~ActionSet();

		/**
		 * Initializes the action set.
		 * @param xrInstance The OpenXR instance for which the action set will be created, must be valid
		 * @param name The action set's name, must be valid
		 * @param description The action set's description, must be valid
		 * @param priority The action set's priority, with range [0, infinity)
		 * @return True, if succeeded
		 */
		bool initialize(const XrInstance& xrInstance, const std::string& name, const std::string& description, const uint32_t priority = 0u);

		/**
		 * Creates a new action.
		 * In case an external managed id is provided, the id must be unique and must not be associated with an existing id already.
		 * @param xrActionType The type of the action to be created, must be valid
		 * @param name The name of the action, must be valid
		 * @param description The description of the action, must be valid
		 * @param actionId The id of the action, an explicit (externally managed id), or invalidActionId_ to use an internally managed id
		 * @return The id of the created action, invalidActionId_ if the action could not be created
		 */
		ActionId createAction(const XrActionType xrActionType, const std::string& name, const std::string& description, const ActionId actionId = invalidActionId_);

		/**
		 * Creates a new action.
		 * In case an external managed id is provided, the id must be unique and must not be associated with an existing id already.
		 * @param xrActionType The type of the action to be created, must be valid
		 * @param name The name of the action, must be valid
		 * @param description The description of the action, must be valid
		 * @param subactionPaths Optional sub-action paths
		 * @param actionId The id of the action, an explicit (externally managed id), or invalidActionId_ to use an internally managed id
		 * @return The id of the created action, invalidActionId_ if the action could not be created
		 */
		ActionId createAction(const XrActionType xrActionType, const std::string& name, const std::string& description, const Paths& subactionPaths, const ActionId actionId = invalidActionId_);

		/**
		 * Creates a new action.
		 * In case an external managed id is provided, the id must be unique and must not be associated with an existing id already.
		 * @param xrActionType The type of the action to be created, must be valid
		 * @param name The name of the action, must be valid
		 * @param description The description of the action, must be valid
		 * @param xrSubactionPaths Optional sub-action paths
		 * @param actionId The id of the action, an explicit (externally managed id), or invalidActionId_ to use an internally managed id
		 * @return The id of the created action, invalidActionId_ if the action could not be created
		 */
		ActionId createAction(const XrActionType xrActionType, const std::string& name, const std::string& description, const XrPaths& xrSubactionPaths, const ActionId actionId = invalidActionId_);

		/**
		 * Creates a new action and directly adds an action binding.
		 * In case an external managed id is provided, the id must be unique and must not be associated with an existing id already.
		 * @param xrActionType The type of the action to be created, must be valid
		 * @param name The name of the action, must be valid
		 * @param description The description of the action, must be valid
		 * @param path The OpenXR path to which the action will be bound, must be valid
		 * @param actionId The id of the action, an explicit (externally managed id), or invalidActionId_ to use an internally managed id
		 * @return The id of the created action, invalidActionId_ if the action could not be created
		 */
		inline ActionId createActionWithBinding(const XrActionType xrActionType, const std::string& name, const std::string& description, const std::string& path, const ActionId actionId = invalidActionId_);

		/**
		 * Adds an action binding.
		 * @param actionId The id of the action to bind, must be valid
		 * @param path The OpenXR path to which the action will be bound, must be valid
		 * @return True, if succeeded
		 */
		bool addActionBinding(const ActionId actionId, const std::string& path);

		/**
		 * Adds an action binding.
		 * @param actionId The id of the action to bind, must be valid
		 * @param xrPath The OpenXR path to which the action will be bound, must be valid
		 * @return True, if succeeded
		 */
		bool addActionBinding(const ActionId actionId, const XrPath& xrPath);

		/**
		 * Suggests the actions of this set as binding.
		 * @param xrInstance The OpenXR instance for which the action set will be created, must be valid
		 * @param interactionProfile The interaction profile to be used, must be valid
		 * @return True, if succeeded
		 * @see actionBindingsSuggested().
		 */
		inline bool suggestActionBindings(const XrInstance& xrInstance, const std::string& interactionProfile);

		/**
		 * Suggests the actions of this set as binding.
		 * @param xrInstance The OpenXR instance for which the action set will be created, must be valid
		 * @param xrInteractionProfilePath The interaction profile to be used, must be valid
		 * @return True, if succeeded
		 * @see actionBindingsSuggested().
		 */
		bool suggestActionBindings(const XrInstance& xrInstance, const XrPath& xrInteractionProfilePath);

		/**
		 * Returns an action of this set.
		 * @param actionId The id of the action, must be valid
		 * @return The requested action, XR_NULL_HANDLE if unknown
		 */
		XrAction action(const ActionId actionId) const;

		/**
		 * Determines the state of an action.
		 * @param xrSession The session to which the action belongs (is synchronized), must be valid
		 * @param actionId The id of the action, must be valid
		 * @param state The resulting state
		 * @return True, if succeeded
		 * @tparam T The data type of the state, either 'XrActionStateBoolean', 'xrGetActionStateFloat', 'XrActionStateVector2f', or 'xrGetActionStatePose'
		 */
		template <typename T>
		bool actionState(const XrSession& xrSession, const ActionId actionId, T& state) const;

		/**
		 * Explicitly releases the action set and all associated actions and resources.
		 * @see initialize().
		 */
		void release();

		/**
		 * Returns true whether the actions have been set as applicated-suggested bindings.
		 * @return True, if so
		 * @see suggestActionBindings().
		 */
		bool actionBindingsSuggested() const;

		/**
		 * Returns whether this object holds a valid OpenXR action set.
		 * @return True, if so
		 * @see initialize().
		 */
		bool isValid() const;

		/**
		 * Returns the wrapped OpenXR action set.
		 * @return The actual action set, nullptr if not initialized
		 * @see isValid().
		 */
		inline operator XrActionSet() const;

		/**
		 * Returns whether this object holds a valid OpenXR action set.
		 * @return True, if so
		 * @see isValid().
		 */
		explicit inline operator bool() const;

		/**
		 * Move operator.
		 * @param actionSet The action set object to be moved
		 * @return The reference to this object
		 */
		ActionSet& operator=(ActionSet&& actionSet) noexcept;

		/**
		 * Determines the state of an action.
		 * @param xrSession The session to which the action belongs (is synchronized), must be valid
		 * @param xrAction The action for which the state will be returned, must be valid
		 * @param state The resulting state
		 * @return True, if succeeded
		 * @tparam T The data type of the state, either 'XrActionStateBoolean', 'xrGetActionStateFloat', 'XrActionStateVector2f', or 'xrGetActionStatePose'
		 */
		template <typename T>
		static bool actionState(const XrSession& xrSession, const XrAction& xrAction, T& state);

	protected:

		/**
		 * Disabled copy constructor.
		 */
		ActionSet(const ActionSet&) = delete;

		/**
		 * Disabled assign operator.
		 * @return The reference to this object
		 */
		ActionSet& operator=(const ActionSet&) = delete;

	protected:

		/// The OpenXR instance associated with this action set.
		XrInstance xrInstance_ = XR_NULL_HANDLE;

		/// The action OpenXR action set.
		XrActionSet xrActionSet_ = XR_NULL_HANDLE;

		/// The actions of this action set.
		ActionIdMap actionIdMap_;

		/// The counter of unique action ids.
		ActionId actionIdCounter_ = 0u;

		/// The XrActionSuggestedBindings of this action set.
		XrActionSuggestedBindings xrActionSuggestedBindings_;

		/// True, if the actions have been set as applicated-suggested bindings.
		bool actionBindingsSuggested_ = false;

		/// The action set's lock.
		mutable Lock lock_;
};

inline ActionSet::ActionSet(ActionSet&& actionSet) noexcept
{
	*this = std::move(actionSet);
}

inline ActionSet::ActionId ActionSet::createActionWithBinding(const XrActionType xrActionType, const std::string& name, const std::string& description, const std::string& path, const ActionId actionId)
{
	const ActionId id = createAction(xrActionType, name, description, actionId);
	ocean_assert(id != invalidActionId_);

	if (id != invalidActionId_ && addActionBinding(id, path))
	{
		return id;
	}

	return invalidActionId_;
}

inline bool ActionSet::suggestActionBindings(const XrInstance& xrInstance, const std::string& interactionProfile)
{
	ocean_assert(xrInstance != XR_NULL_HANDLE);
	ocean_assert(!interactionProfile.empty());

	return suggestActionBindings(xrInstance, Utilities::translatePath(xrInstance, interactionProfile));
}

template <typename T>
bool ActionSet::actionState(const XrSession& xrSession, const ActionId actionId, T& state) const
{
	ocean_assert(xrSession != XR_NULL_HANDLE);
	ocean_assert(actionId != invalidActionId_);

	const ScopedLock scopedLock(lock_);

	const ActionIdMap::const_iterator iAction = actionIdMap_.find(actionId);

	if (iAction == actionIdMap_.cend())
	{
		ocean_assert(false && "Unknown action id");
		return false;
	}

	return actionState<T>(xrSession, iAction->second, state);
}

template <>
inline bool ActionSet::actionState(const XrSession& xrSession, const XrAction& xrAction, XrActionStateBoolean& state)
{
	ocean_assert(xrSession != XR_NULL_HANDLE);
	ocean_assert(xrAction != XR_NULL_HANDLE);

	state = XrActionStateBoolean{XR_TYPE_ACTION_STATE_BOOLEAN};

	XrActionStateGetInfo xrActionStateGetInfo{XR_TYPE_ACTION_STATE_GET_INFO};
	xrActionStateGetInfo.action = xrAction;

	const XrResult xrResult = xrGetActionStateBoolean(xrSession, &xrActionStateGetInfo, &state);
	ocean_assert(xrResult == XR_SUCCESS);

	return xrResult == XR_SUCCESS;
}

template <>
inline bool ActionSet::actionState(const XrSession& xrSession, const XrAction& xrAction, XrActionStateFloat& state)
{
	ocean_assert(xrSession != XR_NULL_HANDLE);
	ocean_assert(xrAction != XR_NULL_HANDLE);

	state = XrActionStateFloat{XR_TYPE_ACTION_STATE_FLOAT};

	XrActionStateGetInfo xrActionStateGetInfo{XR_TYPE_ACTION_STATE_GET_INFO};
	xrActionStateGetInfo.action = xrAction;

	const XrResult xrResult = xrGetActionStateFloat(xrSession, &xrActionStateGetInfo, &state);
	ocean_assert(xrResult == XR_SUCCESS);

	return xrResult == XR_SUCCESS;
}

template <>
inline bool ActionSet::actionState(const XrSession& xrSession, const XrAction& xrAction, XrActionStateVector2f& state)
{
	ocean_assert(xrSession != XR_NULL_HANDLE);
	ocean_assert(xrAction != XR_NULL_HANDLE);

	state = XrActionStateVector2f{XR_TYPE_ACTION_STATE_VECTOR2F};

	XrActionStateGetInfo xrActionStateGetInfo{XR_TYPE_ACTION_STATE_GET_INFO};
	xrActionStateGetInfo.action = xrAction;

	const XrResult xrResult = xrGetActionStateVector2f(xrSession, &xrActionStateGetInfo, &state);
	ocean_assert(xrResult == XR_SUCCESS);

	return xrResult == XR_SUCCESS;
}

template <>
inline bool ActionSet::actionState(const XrSession& xrSession, const XrAction& xrAction, XrActionStatePose& state)
{
	ocean_assert(xrSession != XR_NULL_HANDLE);
	ocean_assert(xrAction != XR_NULL_HANDLE);

	state = XrActionStatePose{XR_TYPE_ACTION_STATE_POSE};

	XrActionStateGetInfo xrActionStateGetInfo{XR_TYPE_ACTION_STATE_GET_INFO};
	xrActionStateGetInfo.action = xrAction;

	const XrResult xrResult = xrGetActionStatePose(xrSession, &xrActionStateGetInfo, &state);
	ocean_assert(xrResult == XR_SUCCESS);

	return xrResult == XR_SUCCESS;
}

inline ActionSet::operator XrActionSet() const
{
	const ScopedLock scopedLock(lock_);

	return xrActionSet_;
}

inline ActionSet::operator bool() const
{
	return isValid();
}

}

}

}

#endif // META_OCEAN_PLATFORM_OPENXR_ACTION_SET_H
