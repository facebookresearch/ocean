/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/ios/Utilities.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/StringApple.h"
#include "ocean/base/Thread.h"

#include "ocean/platform/apple/Utilities.h"

#include <Foundation/Foundation.h>

#include <UIKit/UIKit.h>

using namespace Ocean;
using namespace Ocean::Platform::Apple::IOS;

/**
 * Definition of a image picker delegate object.
 */
@interface ImagePickerDelegate : NSObject<UIImagePickerControllerDelegate, UINavigationControllerDelegate>
{
	/// The callback function for picked image
	@private Ocean::Platform::Apple::IOS::Utilities::OnPickedImageCallback onPickedCallback_;
}

/**
 * Initialize the delegate object by a given callback function for picked images.
 * @param onPickedCallback Callback function for a picked image
 * @return The instance of the delegate object
 */
- (id) initWithCallback:(Ocean::Platform::Apple::IOS::Utilities::OnPickedImageCallback)onPickedCallback;

@end

namespace
{

/**
 * This helper class allows to prevent the usage of image picker capabilities in parallel.
 */
class ImagePickerManager : public Singleton<ImagePickerManager>
{
	friend class Singleton<ImagePickerManager>;

	public:

		/**
		 * Picks an image from the photo album.
		 * Each call must be balanced with a call of disposeDelegate().
		 * @param callback The callback function which will be called providing the picked image
		 */
		void pickImage(const Utilities::OnPickedImageCallback& callback);

		/**
		 * Destructs the delegate of this manager.
		 */
		void disposeDelegate();

	protected:

		/**
		 * Protected constructor.
		 */
		ImagePickerManager();

		/**
		 * Protected desctrutor.
		 */
		virtual ~ImagePickerManager();

	protected:

		/// The lock object of this manager.
		Lock lock_;

		/// The delegate of the image picker.
		ImagePickerDelegate* imagePickerDelegate_;
};

ImagePickerManager::ImagePickerManager() :
	imagePickerDelegate_(nullptr)
{
	// nothing to do here
}

ImagePickerManager::~ImagePickerManager()
{
	ocean_assert(imagePickerDelegate_ == nullptr);
}

void ImagePickerManager::pickImage(const Utilities::OnPickedImageCallback& callback)
{
	ocean_assert(callback);

	while (true)
	{
		TemporaryScopedLock scopedLock(lock_);

		if (imagePickerDelegate_ == nullptr)
		{
			imagePickerDelegate_ = [[ImagePickerDelegate alloc] initWithCallback:callback];

			dispatch_async(dispatch_get_main_queue(), ^{
				UIImagePickerController* picker = [[UIImagePickerController alloc] init];
				picker.delegate = imagePickerDelegate_;
				picker.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;

				UIViewController* rootController = [[[[UIApplication sharedApplication]delegate] window] rootViewController];
				[rootController presentViewController:picker animated:YES completion:nil];
			});

			break;
		}

		scopedLock.release();

		Thread::sleep(1u);
	}
}

void ImagePickerManager::disposeDelegate()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(imagePickerDelegate_ != nullptr);
	imagePickerDelegate_ = nullptr;
}

}

@implementation ImagePickerDelegate

- (id)initWithCallback:(Ocean::Platform::Apple::IOS::Utilities::OnPickedImageCallback)onPickedCallback
{
	if (self = [super init])
	{
		ocean_assert(onPickedCallback);
		onPickedCallback_ = onPickedCallback;
	}

	return self;
}

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
	ocean_assert(onPickedCallback_);

    UIImage* image = info[UIImagePickerControllerOriginalImage];

    [picker dismissViewControllerAnimated:YES completion:nullptr];

	onPickedCallback_((__bridge void*)image);

	ImagePickerManager::get().disposeDelegate();
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
	ImagePickerManager::get().disposeDelegate();

	[picker dismissViewControllerAnimated:NO completion:nil];
}

@end

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace IOS
{

namespace
{

/**
 * This helper class allows to prevent the usage of iOS's sharing capabilities in parallel.
 */
class ActivityManager : public Singleton<ActivityManager>
{
	friend class Singleton<ActivityManager>;

	public:

		/**
		 * Locks the usage of the sharing activity.
		 * So that no other thread can use it until it is unlocked.
		 * Beware: The caller must call unlock() when the activity is free for use again.
		 * @see unlock().
		 */
		void lock();

		/**
		 * Unlocks the usage of the sharing activity.
		 * Beware: The caller must have locked the activity before calling this function.
		 * @see lock().
		 */
		void unlock();

	protected:

		/**
		 * Protected constructor.
		 */
		ActivityManager();

		/**
		 * Protected desctrutor.
		 */
		virtual ~ActivityManager();

	protected:

		/// The lock object of this manager.
		Lock lock_;

		/// The state whether the activity is locked (True) or not (False).
		bool locked_;
};

ActivityManager::ActivityManager() :
	locked_(false)
{
	// nothing to do here
}

ActivityManager::~ActivityManager()
{
	// nothing to do here
}

void ActivityManager::lock()
{
	while (true)
	{
		TemporaryScopedLock scopedLock(lock_);

		if (!locked_)
		{
			locked_ = true;
			break;
		}

		scopedLock.release();

		Thread::sleep(1u);
	}
}

void ActivityManager::unlock()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(locked_);
	locked_ = false;
}

/**
 * Shares some objcts using iOS' Activity view controller.
 * We define this function here in the cpp file as we do not want to have Objc code in the header files,<br>
 * as this header files would need to be called from an Objc source file.<br>
 * The controller allows to copy or to share the provided objects.<br>
 * By calling this function, you will see iPhone's default sharing functionality offering to share the provided objects.
 * @param objects The objects to be shared, must be valid
 */
void shareObjects(NSArray* objects)
{
	ocean_assert(objects != nullptr);

	if (objects == nullptr)
	{
		return;
	}

	// we need to lock the usage of the Activity view,
	// as this function will return immediately and thus a successive call to shareObjects() would not have any impact
	// therefore, we will wait here if another sharing activity is currently visible already
	ActivityManager::get().lock();

	dispatch_async(dispatch_get_main_queue(), ^{
		UIActivityViewController* controller = [[UIActivityViewController alloc] initWithActivityItems:objects applicationActivities:nil];

		controller.completionWithItemsHandler = ^(NSString *activityType, BOOL completed, NSArray *returnedItems, NSError *activityError) {
			// we unlock the Activity so that we can share new content or pending content
			ActivityManager::get().unlock();
		};

		UIViewController* rootController = [[[[UIApplication sharedApplication]delegate] window] rootViewController];

		if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad)
		{
			// On iPad we have to use a popover

			UIView* view = rootController.viewIfLoaded;

			if (view && view.bounds.size.width > 0 && view.bounds.size.height > 0)
			{
				controller.popoverPresentationController.sourceView = view;

				CGSize size = view.bounds.size;

				controller.popoverPresentationController.sourceRect = CGRectMake(size.width / CGFloat(8), size.height / CGFloat(8), size.width / CGFloat(4), size.height / CGFloat(4));
			}
			else
			{
				Log::error() << "Did not find non-empty sourceView for popover";

				ActivityManager::get().unlock();

				return;
			}

		}

		[rootController presentViewController:controller animated:YES completion:nil];
	});
}

}

void Utilities::loadImageFromPhotoAlbum(const OnPickedImageCallback& callback)
{
	ImagePickerManager::get().pickImage(callback);
}

bool Utilities::saveImageToPhotosAlbum(const Frame& frame)
{
	ocean_assert(frame);

	const Frame internalFrame = castToSupportedPixelFormat(frame);

	const ScopedCGImageRef imageRef(Apple::Utilities::toCGImage(internalFrame, true));

	UIImage* uiImage = [[UIImage alloc] initWithCGImage:imageRef.object()];

	if (uiImage == nullptr)
	{
		return false;
	}

	UIImageWriteToSavedPhotosAlbum(uiImage, nil, nil, nil);

	return true;
}

bool Utilities::shareImage(const Frame& frame)
{
	ocean_assert(frame);

	const Frame internalFrame = castToSupportedPixelFormat(frame);

	ocean_assert(internalFrame);

	const ScopedCGImageRef imageRef(Apple::Utilities::toCGImage(internalFrame, true));

	UIImage* uiImage = [[UIImage alloc] initWithCGImage:imageRef.object()];

	if (uiImage == nullptr)
	{
		return false;
	}

	shareObjects(@[uiImage]);

	return true;
}

bool Utilities::shareImages(const Frames& frames)
{
	ocean_assert(!frames.empty());

	NSMutableArray* objects = [[NSMutableArray alloc] init];

	for (Frames::const_iterator i = frames.begin(); i != frames.end(); ++i)
	{
		ocean_assert(*i);

		const ScopedCGImageRef imageRef(Apple::Utilities::toCGImage(*i, true));

		if (imageRef.object() != nullptr)
		{
			UIImage* uiImage = [[UIImage alloc] initWithCGImage:imageRef.object()];

			if (uiImage != nullptr)
			{
				[objects addObject:uiImage];
			}
		}
	}

	if (objects.count == 0)
	{
		return false;
	}

	shareObjects(objects);

	return true;
}

bool Utilities::shareText(const std::string& text)
{
	ocean_assert(!text.empty());

	NSString* nsText = StringApple::toNSString(text);

	if (nsText == nullptr)
	{
		return false;
	}

	shareObjects(@[nsText]);

	return true;
}

bool Utilities::shareFile(const std::string& filename)
{
	ocean_assert(!filename.empty());

	NSURL* nsUrl = [NSURL fileURLWithPath: StringApple::toNSString(filename)];

	if (nsUrl == nullptr)
	{
		return false;
	}

	shareObjects(@[nsUrl]);

	return true;
}

bool Utilities::shareFiles(const Strings& filenames)
{
	ocean_assert(!filenames.empty());

	NSMutableArray* objects = [[NSMutableArray alloc] init];

	for (const std::string& filename : filenames)
	{
		NSURL* nsUrl = [NSURL fileURLWithPath: StringApple::toNSString(filename)];

		if (nsUrl == nullptr)
		{
			return false;
		}

		[objects addObject:nsUrl];
	}

	shareObjects(objects);

	return true;

}

Frame Utilities::castToSupportedPixelFormat(const Frame& frame)
{
	ocean_assert(frame);

	if (FrameType::formatIsPureGeneric(frame.pixelFormat()))
	{
		// in case, we have a generic pixel format we will use a pixel format with valid representation
		const FrameType::PixelFormat bestPixelFormat = FrameType::findPixelFormat(frame.dataType(), frame.channels());

		if (bestPixelFormat == FrameType::FORMAT_UNDEFINED)
		{
			ocean_assert(false && "Not sure how to share the given frame!");
			return Frame();
		}

		return Frame(FrameType(frame, bestPixelFormat), frame.constdata<void>(), Frame::CM_USE_KEEP_LAYOUT);
	}

	return Frame(frame.frameType(), frame.constdata<void>(), Frame::CM_USE_KEEP_LAYOUT);
}

}

}

}

}
