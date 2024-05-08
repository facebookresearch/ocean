/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/openxr/ActionSet.h"
#include "ocean/platform/openxr/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace OpenXR
{

ActionSet::~ActionSet()
{
	release();
}

bool ActionSet::initialize(const XrInstance& xrInstance, const std::string& name, const std::string& description, const uint32_t priority)
{
	ocean_assert(xrInstance != XR_NULL_HANDLE);
	ocean_assert(!name.empty() || !description.empty());

	if (xrInstance == XR_NULL_HANDLE || name.empty() || description.empty())
	{
		return false;
	}

	if (name.size() + 1 >= XR_MAX_ACTION_SET_NAME_SIZE || description.size() + 1 >= XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE)
	{
		ocean_assert(false && "Invalid name or description!");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (xrActionSet_ != XR_NULL_HANDLE)
	{
		ocean_assert(false && "This action set has already been initialized");
		return true;
	}

	XrActionSetCreateInfo xrActionSetCreateInfo{XR_TYPE_ACTION_SET_CREATE_INFO};
	strcpy(xrActionSetCreateInfo.actionSetName, name.c_str());
	strcpy(xrActionSetCreateInfo.localizedActionSetName, description.c_str());
	xrActionSetCreateInfo.priority = priority;

	const XrResult xrResult = xrCreateActionSet(xrInstance, &xrActionSetCreateInfo, &xrActionSet_);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to create action set: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	ocean_assert(xrActionSet_ != XR_NULL_HANDLE);

	xrInstance_ = xrInstance;

	return true;
}

ActionSet::ActionId ActionSet::createAction(const XrActionType xrActionType, const std::string& name, const std::string& description, const ActionId actionId)
{
	return createAction(xrActionType, name, description, XrPaths(), actionId);
}

ActionSet::ActionId ActionSet::createAction(const XrActionType xrActionType, const std::string& name, const std::string& description, const Paths& paths, const ActionId actionId)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid());

	XrPaths xrPaths;
	xrPaths.reserve(paths.size());

	for (const std::string& path : paths)
	{
		const XrPath xrPath = Utilities::translatePath(xrInstance_, path);

		if (xrPath == XR_NULL_PATH)
		{
			ocean_assert(false && "Invalid path");
			return invalidActionId_;
		}

		xrPaths.emplace_back(xrPath);
	}

	return createAction(xrActionType, name, description, xrPaths, actionId);
}

ActionSet::ActionId ActionSet::createAction(const XrActionType xrActionType, const std::string& name, const std::string& description, const XrPaths& xrPaths, const ActionId actionId)
{
	ocean_assert(isValid());

	if (name.empty() || description.empty() || name.size() + 1 >= XR_MAX_ACTION_NAME_SIZE || description.size() + 1 >= XR_MAX_LOCALIZED_ACTION_NAME_SIZE)
	{
		ocean_assert(false && "Invalid name or description!");
		return invalidActionId_;
	}

	const ScopedLock scopedLock(lock_);

	if (xrActionSet_ == XR_NULL_HANDLE)
	{
		return invalidActionId_;
	}

	if (actionId != invalidActionId_)
	{
		if (actionIdMap_.find(actionId) != actionIdMap_.cend())
		{
			ocean_assert(false && "Action id exists already");
			return invalidActionId_;
		}
	}

	XrActionCreateInfo xrActionCreateInfo{XR_TYPE_ACTION_CREATE_INFO};
	xrActionCreateInfo.actionType = xrActionType;
	strcpy(xrActionCreateInfo.actionName, name.c_str());
	strcpy(xrActionCreateInfo.localizedActionName, description.c_str());

	xrActionCreateInfo.countSubactionPaths = uint32_t(xrPaths.size());
	xrActionCreateInfo.subactionPaths = xrPaths.empty() ? nullptr : xrPaths.data();

	XrAction xrAction = XR_NULL_HANDLE;
	const XrResult xrResult = xrCreateAction(xrActionSet_, &xrActionCreateInfo, &xrAction);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to create action '" << name << "': " << Utilities::translateResult(xrInstance_, xrResult);
		return invalidActionId_;
	}

	ActionId usedActionId = actionId;

	if (usedActionId == invalidActionId_)
	{
		while (actionIdMap_.find(++actionIdCounter_) != actionIdMap_.cend())
		{
			// nothing to do here
		}

		usedActionId = actionIdCounter_;
	}

	ocean_assert(actionIdMap_.find(usedActionId) == actionIdMap_.cend());

	actionIdMap_.emplace(usedActionId, xrAction);

	return usedActionId;
}

bool ActionSet::addActionBinding(const ActionId actionId, const std::string& path)
{
	ocean_assert(isValid());
	ocean_assert(!path.empty());

	return addActionBinding(actionId, Utilities::translatePath(xrInstance_, path));
}

bool ActionSet::addActionBinding(const ActionId actionId, const XrPath& xrPath)
{
	ocean_assert(actionId != invalidActionId_);
	ocean_assert(xrPath != XR_NULL_PATH);

	const ScopedLock scopedLock(lock_);

	const ActionIdMap::const_iterator iAction = actionIdMap_.find(actionId);

	if (iAction == actionIdMap_.cend())
	{
		ocean_assert(false && "Unknown action id");
		return false;
	}

#ifdef OCEAN_DEBUG
	for (const XrActionSuggestedBinding& xrActionSuggestedBinding : xrActionSuggestedBindings_)
	{
		ocean_assert(xrActionSuggestedBinding.action != iAction->second || xrActionSuggestedBinding.binding != xrPath);
	}
#endif

	xrActionSuggestedBindings_.emplace_back(XrActionSuggestedBinding{iAction->second, xrPath});

	return true;
}

bool ActionSet::suggestActionBindings(const XrInstance& xrInstance, const XrPath& xrInteractionProfilePath)
{
	ocean_assert(xrInstance != XR_NULL_HANDLE);
	ocean_assert(xrInteractionProfilePath != XR_NULL_PATH);

	const ScopedLock scopedLock(lock_);

	if (actionBindingsSuggested_)
	{
		ocean_assert(false && "Actions have been suggested already!");
		return false;
	}

	if (xrActionSuggestedBindings_.empty())
	{
		ocean_assert(false && "No bound actions to attach");
		return false;
	}

	XrInteractionProfileSuggestedBinding xrInteractionProfileSuggestedBinding{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
	xrInteractionProfileSuggestedBinding.interactionProfile = xrInteractionProfilePath;
	xrInteractionProfileSuggestedBinding.suggestedBindings = xrActionSuggestedBindings_.data();
	xrInteractionProfileSuggestedBinding.countSuggestedBindings = uint32_t(xrActionSuggestedBindings_.size());

	const XrResult xrResult = xrSuggestInteractionProfileBindings(xrInstance, &xrInteractionProfileSuggestedBinding);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to suggest interaction profile bindings: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	actionBindingsSuggested_ = true;

	return true;
}

XrAction ActionSet::action(const ActionId actionId) const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(actionId != invalidActionId_);

	const ActionIdMap::const_iterator iAction = actionIdMap_.find(actionId);

	if (iAction == actionIdMap_.cend())
	{
		ocean_assert(false && "Unknown action id");
		return XR_NULL_HANDLE;
	}

	ocean_assert(iAction->second != XR_NULL_HANDLE);
	return iAction->second;
}

void ActionSet::release()
{
	const ScopedLock scopedLock(lock_);

	if (xrActionSet_ != XR_NULL_HANDLE)
	{
		const XrResult xrResult = xrDestroyActionSet(xrActionSet_);
		ocean_assert_and_suppress_unused(xrResult == XR_SUCCESS, xrResult);

		xrActionSet_ = XR_NULL_HANDLE;
	}

	xrInstance_ = XR_NULL_HANDLE;

	// When an action set handle is destroyed, all handles of actions in that action set are also destroyed.
	actionIdMap_.clear();

	actionIdCounter_ = 0u;

	xrActionSuggestedBindings_.clear();
	actionBindingsSuggested_ = false;
}

bool ActionSet::actionBindingsSuggested() const
{
	const ScopedLock scopedLock(lock_);

	return actionBindingsSuggested_;
}

bool ActionSet::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return xrActionSet_ != XR_NULL_HANDLE;
}

ActionSet& ActionSet::operator=(ActionSet&& actionSet) noexcept
{
	if (this != &actionSet)
	{
		release();

		xrInstance_ = actionSet.xrInstance_;
		actionSet.xrInstance_ = XR_NULL_HANDLE;

		xrActionSet_ = actionSet.xrActionSet_;
		actionSet.xrActionSet_ = XR_NULL_HANDLE;

		actionIdMap_ = std::move(actionSet.actionIdMap_);

		actionIdCounter_ = actionSet.actionIdCounter_;
		actionSet.actionIdCounter_ = 0u;

		xrActionSuggestedBindings_ = std::move(actionSet.xrActionSuggestedBindings_);

		actionBindingsSuggested_ = actionSet.actionBindingsSuggested_;
		actionSet.actionBindingsSuggested_ = false;
	}

	return *this;
}

}

}

}
