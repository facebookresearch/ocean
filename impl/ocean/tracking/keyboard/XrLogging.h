// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_KEYBOARD_XRLOGGING_H
#define META_OCEAN_TRACKING_KEYBOARD_XRLOGGING_H

#if defined(OCEAN_ENABLE_XRLOGGING)
	#define DEFAULT_LOG_CHANNEL "KEYBOARD"
	#include <logging/Log.h>
#else
	// Provide empty macro definitions in case xr logging is not available (e.g., visual studio).
	#define XR_LOGE(msg, ...) do{} while(0)
	#define XR_LOGW(msg, ...) do{} while(0)
	#define XR_LOGI(msg, ...) do{} while(0)
	#define XR_LOGD(msg, ...) do{} while(0)
	#define XR_LOGT(msg, ...) do{} while(0)
	#define XR_LOGE_IF(msg, ...) do{} while(0)
	#define XR_LOGW_IF(msg, ...) do{} while(0)
	#define XR_LOGI_IF(msg, ...) do{} while(0)
	#define XR_LOGD_IF(msg, ...) do{} while(0)
	#define XR_LOGT_IF(msg, ...) do{} while(0)
#endif // defined(OCEAN_ENABLE_XRLOGGING)

#endif // META_OCEAN_TRACKING_KEYBOARD_XRLOGGING_H
