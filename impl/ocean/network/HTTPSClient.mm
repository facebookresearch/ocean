/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/network/HTTPSClient.h"

#ifdef OCEAN_DEBUG
	#include "ocean/base/Timestamp.h"
#endif

#include "ocean/base/StringApple.h"

#include <Foundation/Foundation.h>

#ifndef __APPLE__
    #error This files contains Apple specific functions only, see HTTPSClient.cpp
#endif

/**
 * Definition of the NSURLSession delegate object.
 */
@interface NSURLSessionDelegate : NSObject<NSURLSessionDataDelegate>
{
	/// The data buffer in which the receiving data is written
	Ocean::Network::HTTPSClient::Buffer* data_;

	/// Optional flag that may be set to True by another thread to abort the request
	bool* abort_;

	/// Optional callback for receiving progress information
	Ocean::Network::HTTPSClient::ProgressCallback callback_;

	/// The current data position of the buffer for the received data
	size_t position_;
}

/**
 * Initializes the delegate object.
 * @param data The data buffer in which the receiving data is written
 * @param abort Optional flag that may be set to True by another thread to abort the request
 * @param callback Optional callback for receiving progress information
 * @return The instance of the delegate object
 */
-(id)initWithData:(Ocean::Network::HTTPSClient::Buffer*)data andAbort:(bool*)abort andCallback:(Ocean::Network::HTTPSClient::ProgressCallback)callback;

/**
 * Delegate function from NSURLSessionDataDelegate protocol that is called when the data task has received some of the expected data.
 * @param session The session containing the data task that provided data
 * @param dataTask The data task that provided data
 * @param data A data object containing the transferred data
 */
-(void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveData:(NSData *)data;

@end

@implementation NSURLSessionDelegate

-(id)initWithData:(Ocean::Network::HTTPSClient::Buffer*)data andAbort:(bool*)abort andCallback:(Ocean::Network::HTTPSClient::ProgressCallback)callback
{
	ocean_assert(data);
	data_ = nullptr;
	abort_ = nullptr;
	position_ = 0;

	if (self = [super init])
	{
		data_ = data;

		if (abort)
		{
			abort_ = abort;
		}

		if (callback)
		{
			callback_ = callback;
		}
	}

	return self;
}

-(void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveData:(NSData *)data
{
	if (abort_ && *abort_)
	{
		[dataTask cancel];
		[session invalidateAndCancel];

		return;
	}

	const size_t bytesAvailable = (size_t)[data length];

	if (position_ + bytesAvailable > data_->size())
	{
		data_->resize(position_ + bytesAvailable);
	}

	memcpy(data_->data() + position_, [data bytes], bytesAvailable);

	position_ += bytesAvailable;

	ocean_assert(position_ == size_t([dataTask countOfBytesReceived]));
	ocean_assert([dataTask countOfBytesExpectedToReceive] == 0 || position_ <= size_t([dataTask countOfBytesExpectedToReceive]));

	if (callback_)
	{
		callback_(position_, (size_t)[dataTask countOfBytesExpectedToReceive]);
	}
}

@end

namespace Ocean
{

namespace Network
{

bool HTTPSClient::httpsGetRequestApple(const std::string& url, Buffer& data, const Port& port, const double timeout, bool* abort, const ProgressCallback& progressCallback)
{
	ocean_assert(data.empty());
	ocean_assert(timeout > 0);

	data.clear();

	NSURL* nsUrl = [NSURL URLWithString:StringApple::toNSString(url)];

	NSURLSessionConfiguration* configuration = [NSURLSessionConfiguration defaultSessionConfiguration];
	[configuration setTimeoutIntervalForRequest:timeout];

	NSURLSessionDelegate* delegate = [[NSURLSessionDelegate alloc] initWithData:&data andAbort:abort andCallback:progressCallback];
	NSOperationQueue* queue = [[NSOperationQueue alloc] init];

	NSURLSession* session = [NSURLSession sessionWithConfiguration:configuration delegate:delegate delegateQueue:queue];
	NSURLSessionDataTask* dataTask = [session dataTaskWithURL:nsUrl];

	[dataTask resume];

	// invalidates the session after all tasks are completed
	// it is important to invalidated the session to avoid memory leaks
	[session finishTasksAndInvalidate];

#ifdef OCEAN_DEBUG
	Timestamp timestamp;
#endif

	while ([dataTask state] == NSURLSessionTaskStateRunning)
	{
#ifdef OCEAN_DEBUG

		// we check that the data task is canceled within 5 seconds after the abort flag is set
		if (abort && *abort)
		{
			if (timestamp.isInvalid())
			{
				timestamp.toNow() += Timestamp(5.0);
			}

			if (Timestamp(true) > timestamp)
			{
				ocean_assert(false && "This should never happen!");
			}
		}

#endif

		sleep(1);
	}

	return [dataTask state] == NSURLSessionTaskStateCompleted && [dataTask error] == nil;
}

bool HTTPSClient::httpsPostRequestApple(const std::string& url, const uint8_t* requestData, const size_t requestDataSize, Buffer& data, const Port& port, const double timeout, const Strings& additionalHeaders)
{
	ocean_assert(data.empty());
	ocean_assert(timeout > 0);

	data.clear();

	NSString* bodyString = [[NSString alloc] initWithBytes:requestData length:requestDataSize encoding:NSUTF8StringEncoding];

	NSMutableDictionary* additionalHeadersDictionary = [NSMutableDictionary dictionaryWithCapacity:additionalHeaders.size() + 1];
	[additionalHeadersDictionary setObject:[NSString stringWithFormat:@"%@",  @([bodyString length])] forKey:@"Content-Length"];

	for (Strings::const_iterator i = additionalHeaders.begin(); i != additionalHeaders.end(); ++i)
	{
		const std::string header = *i;
		const std::string::size_type pos = header.find(": ");

		ocean_assert(pos != std::string::npos);
		if (pos != std::string::npos)
		{
			const NSString* field = StringApple::toNSString(header.substr(0, pos));
			const NSString* value = StringApple::toNSString(header.substr(pos + 2));

			[additionalHeadersDictionary setObject:value forKey:field];
		}
	}

	NSURL* nsUrl = [NSURL URLWithString:StringApple::toNSString(url)];

	NSURLSessionConfiguration* configuration = [NSURLSessionConfiguration defaultSessionConfiguration];
	[configuration setTimeoutIntervalForRequest:timeout];
	[configuration setHTTPAdditionalHeaders:additionalHeadersDictionary];

	NSURLSession* session = [NSURLSession sessionWithConfiguration:configuration];

	NSMutableURLRequest* request = [NSMutableURLRequest requestWithURL:nsUrl];
	[request setHTTPMethod:@"POST"];
	[request setHTTPBody:[bodyString dataUsingEncoding:NSUTF8StringEncoding]];

	NSURLSessionDataTask* dataTask = [session dataTaskWithRequest:request completionHandler:^(NSData* taskRequestData, NSURLResponse* response, NSError* errorValue)
									{
										if (taskRequestData)
										{
											data.resize([taskRequestData length]);
											memcpy(data.data(), [taskRequestData bytes], [taskRequestData length]);
										}
									}];

	[dataTask resume];

	// invalidates the session after all tasks are completed
	// it is important to invalidated the session to avoid memory leaks
	[session finishTasksAndInvalidate];

	while ([dataTask state] == NSURLSessionTaskStateRunning)
	{
		sleep(1u);
	}

	return [dataTask state] == NSURLSessionTaskStateCompleted && [dataTask error] == nil;
}


} // namespace Network

} // namespace Ocean
