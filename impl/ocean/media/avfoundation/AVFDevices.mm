/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/avfoundation/AVFDevices.h"

#include "ocean/base/StringApple.h"

#include <AVFoundation/AVFoundation.h>

#import <Foundation/Foundation.h>

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

AVFDevices::Devices AVFDevices::videoDevices()
{
#if (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR == 1) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1)

	// [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo] is deprecated since ios 10.0
	// the new AVCaptureDeviceDiscoverySession is only available for ios

	NSArray<AVCaptureDeviceType>* deviceTypes = @[AVCaptureDeviceTypeBuiltInWideAngleCamera, AVCaptureDeviceTypeBuiltInUltraWideCamera, AVCaptureDeviceTypeBuiltInTelephotoCamera, AVCaptureDeviceTypeBuiltInDualCamera, AVCaptureDeviceTypeBuiltInDualWideCamera, AVCaptureDeviceTypeBuiltInTripleCamera];

	const AVCaptureDeviceDiscoverySession* session = [AVCaptureDeviceDiscoverySession discoverySessionWithDeviceTypes:deviceTypes mediaType:AVMediaTypeVideo position:AVCaptureDevicePositionUnspecified];

	const NSArray<AVCaptureDevice*>* captureDevices = session.devices;

#else

	// for mac os we still use the old method to get the capture devices
	const NSArray<AVCaptureDevice*>* captureDevices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];

#endif

	const size_t numberDevices = [captureDevices count];

	Devices result;
	result.reserve(numberDevices);

	for (size_t n = 0; n < numberDevices; ++n)
	{
		const AVCaptureDevice* device = [captureDevices objectAtIndex:n];

		result.emplace_back(Device(StringApple::toWString(device.localizedName), StringApple::toWString(device.uniqueID)));
	}

	return result;
}

}

}

}
