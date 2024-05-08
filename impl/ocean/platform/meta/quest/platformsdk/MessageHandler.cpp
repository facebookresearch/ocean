/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/platformsdk/MessageHandler.h"
#include "ocean/platform/meta/quest/platformsdk/Manager.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace PlatformSDK
{

bool MessageHandler::invokeRequest(const ovrRequest requestId, ResponseCallback responseCallback)
{
	return Manager::get().invokeRequest(requestId, std::move(responseCallback));
}

MessageHandler::MessageScopedSubscription MessageHandler::subscribeForMessageResponse(const ovrMessageType& messageType, ResponseCallback responseCallback)
{
	return Manager::get().subscribeForMessageResponse(messageType, std::move(responseCallback));
}

Manager::MessageScopedSubscription MessageHandler::createMessageScopedSubscription(const unsigned int subscriptionId)
{
    return MessageScopedSubscription(subscriptionId, std::bind(&MessageHandler::unsubscribeForMessageResponse, std::placeholders::_1));
}

void MessageHandler::unsubscribeForMessageResponse(const unsigned int& subscriptionId)
{
	return Manager::get().unsubscribeForMessageResponse(subscriptionId);
}

} // namespace PlatformSDK

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
