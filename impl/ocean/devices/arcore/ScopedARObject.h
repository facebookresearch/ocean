/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARCORE_AC_SCOPED_AR_OBJECT_H
#define META_OCEAN_DEVICES_ARCORE_AC_SCOPED_AR_OBJECT_H

#include "ocean/devices/arcore/ARCore.h"

#include <arcore_c_api.h>

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

// Forward declaration.
template <typename T, void (*tDestroyFunction)(T*)>
class ScopedARObject;

/**
 * Definition of a scoped object for ArSession.
 * @see ScopedARObject
 * @ingroup devicesarcore
 */
typedef ScopedARObject<ArSession, ArSession_destroy> ScopedARSession;

/**
 * Definition of a scoped object for ArConfig.
 * @see ScopedARObject
 * @ingroup devicesarcore
 */
typedef ScopedARObject<ArConfig, ArConfig_destroy> ScopedARConfig;

/**
 * Definition of a scoped object for ArPose.
 * @see ScopedARObject
 * @ingroup devicesarcore
 */
typedef ScopedARObject<ArPose, ArPose_destroy> ScopedARPose;

/**
 * Definition of a scoped object for ArImage.
 * @see ScopedARObject
 * @ingroup devicesarcore
 */
typedef ScopedARObject<ArImage, ArImage_release> ScopedARImage;

/**
 * Definition of a scoped object for ArCameraConfig.
 * @see ScopedARObject
 * @ingroup devicesarcore
 */
typedef ScopedARObject<ArCameraConfig, ArCameraConfig_destroy> ScopedARCameraConfig;

/**
 * Definition of a scoped object for ArCameraConfigList.
 * @see ScopedARObject
 * @ingroup devicesarcore
 */
typedef ScopedARObject<ArCameraConfigList, ArCameraConfigList_destroy> ScopedARCameraConfigList;

/**
 * Definition of a scoped object for ArCameraConfigFilter.
 * @see ScopedARObject
 * @ingroup devicesarcore
 */
typedef ScopedARObject<ArCameraConfigFilter, ArCameraConfigFilter_destroy> ScopedARCameraConfigFilter;

/**
 * Definition of a scoped object for ArCameraIntrinsics.
 * @see ScopedARObject
 * @ingroup devicesarcore
 */
typedef ScopedARObject<ArCameraIntrinsics, ArCameraIntrinsics_destroy> ScopedARCameraIntrinsics;

/**
 * Definition of a scoped object for ArTrackable.
 * @see ScopedARObject
 * @ingroup devicesarcore
 */
typedef ScopedARObject<ArTrackable, ArTrackable_release> ScopedARTrackable;

/**
 * Definition of a scoped object for ArTrackableList.
 * @see ScopedARObject
 * @ingroup devicesarcore
 */
typedef ScopedARObject<ArTrackableList, ArTrackableList_destroy> ScopedARTrackableList;

/**
 * Definition of a scoped object for ArPointCloud.
 * @see ScopedARObject
 * @ingroup devicesarcore
 */
typedef ScopedARObject<ArPointCloud, ArPointCloud_release> ScopedARPointCloud;

/**
 * This class implements a scoped object for ARCore objects.
 * The class is a helper class to ensure that objects are always destoyed after usage.
 * @tparam T The data type of the object
 * @tparam tDestroyFunction The static function to destroy the object
 * @ingroup devicesarcore
 */
template <typename T, void (*tDestroyFunction)(T*)>
class ScopedARObject
{
	public:

		/**
		 * Definition of a function pointer to a function creating the object.
		 * @param arSession The ARCore session for which the object will be created
		 * @param object The resulting created object
		 */
		typedef void(*CreateFunction)(const ArSession* arSession, T** object);

	public:

		/**
		 * Default constructor creating an invalid object.
		 */
		ScopedARObject() = default;

		/**
		 * Creates a new scoped ARCore object.
		 * @param object The actual object which will be destroyed once this object is disposed, must be valid
		 */
		explicit inline ScopedARObject(T* object);

		/**
		 * Creates a new scoped ARCore object.
		 * @param arSession The ARCore session for which the object will be created
		 * @param createFunction The create function which will be used to create the new object, must be valid
		 */
		inline ScopedARObject(const ArSession* arSession, const CreateFunction& createFunction);

		/**
		 * Move constructor.
		 * @param arScopedObject The scoped object to be moved
		 */
		inline ScopedARObject(ScopedARObject<T, tDestroyFunction>&& arScopedObject);

		/**
		 * Destructs the object and destroys it.
		 */
		inline ~ScopedARObject();

		/**
		 * Allows to ingest an object via an external function.
		 * In case this scoped object already holds an existing object, the existing object will be released first.
		 * @return The pointer to the internal object's pointer.
		 */
		inline T** ingest();

		/**
		 * Explicitly releases the scoped object.
		 */
		inline void release();

		/**
		 * Returns the actual object.
		 * @return The object of this object
		 */
		inline operator T*() const;

		/**
		 * Returns whether this scoped object holds an actual object.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Move operator.
		 * @param arScopedObject The object to be moved
		 * @return Reference to this object
		 */
		inline ScopedARObject<T, tDestroyFunction>& operator=(ScopedARObject<T, tDestroyFunction>&& arScopedObject);

	protected:

		/**
		 * Disabled copy constructor.
		 */
		ScopedARObject(const ScopedARObject<T, tDestroyFunction>&) = delete;

		/**
		 * Disabled assign operator.
		 */
		ScopedARObject<T, tDestroyFunction>& operator=(const ScopedARObject<T, tDestroyFunction>&) = delete;

	protected:

		/// The actual object.
		T* object_ = nullptr;
};

template <typename T, void (*tDestroyFunction)(T*)>
inline ScopedARObject<T, tDestroyFunction>::ScopedARObject(T* object) :
	object_(object)
{
	static_assert(tDestroyFunction != nullptr, "Invalid destroy function!");
}

template <typename T, void (*tDestroyFunction)(T*)>
inline ScopedARObject<T, tDestroyFunction>::ScopedARObject(const ArSession* arSession, const CreateFunction& createFunction)
{
	ocean_assert(arSession != nullptr);
	ocean_assert(createFunction != nullptr);

	createFunction(arSession, &object_);
	ocean_assert(object_ != nullptr);
}

template <typename T, void (*tDestroyFunction)(T*)>
inline ScopedARObject<T, tDestroyFunction>::ScopedARObject(ScopedARObject<T, tDestroyFunction>&& arScopedObject)
{
	*this = std::move(arScopedObject);
}

template <typename T, void (*tDestroyFunction)(T*)>
inline ScopedARObject<T, tDestroyFunction>::~ScopedARObject()
{
	release();
}

template <typename T, void (*tDestroyFunction)(T*)>
inline T** ScopedARObject<T, tDestroyFunction>::ingest()
{
	release();

	return &object_;
}

template <typename T, void (*tDestroyFunction)(T*)>
inline void ScopedARObject<T, tDestroyFunction>::release()
{
	if (object_ != nullptr)
	{
		tDestroyFunction(object_);
		object_ = nullptr;
	}
}

template <typename T, void (*tDestroyFunction)(T*)>
inline ScopedARObject<T, tDestroyFunction>::operator T*() const
{
	return object_;
}

template <typename T, void (*tDestroyFunction)(T*)>
inline ScopedARObject<T, tDestroyFunction>::operator bool() const
{
	return object_ != nullptr;
}

template <typename T, void (*tDestroyFunction)(T*)>
inline ScopedARObject<T, tDestroyFunction>& ScopedARObject<T, tDestroyFunction>::operator=(ScopedARObject<T, tDestroyFunction>&& arScopedObject)
{
	if (this != &arScopedObject)
	{
		release();

		object_ = arScopedObject.object_;
		arScopedObject.object_ = nullptr;
	}

	return *this;
}

}

}

}

#endif // META_OCEAN_DEVICES_ARCORE_AC_SCOPED_AR_OBJECT_H
