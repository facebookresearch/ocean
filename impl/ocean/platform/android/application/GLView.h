/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_APPLICATION_GL_VIEW_H
#define META_OCEAN_PLATFORM_ANDROID_APPLICATION_GL_VIEW_H

#include "ocean/platform/android/application/Application.h"

#include "ocean/base/Singleton.h"

#include "ocean/math/Math.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

namespace Application
{

/**
 * This class implements the base class for all OpenGL-ES-based views.
 * The view is implemented as singleton object.
 * @ingroup platformandroidapplication
 */
class OCEAN_PLATFORM_ANDROID_APPLICATION_EXPORT GLView
{
	public:

		/**
		 * Definition of a function pointer creating an instance of the GLView object.
		 */
		typedef GLView* (*InstanceFunction)();

	protected:

		/**
		 * This class implements a simple helper class providing the instance of this GLView object.
		 */
		class Instance : public Singleton<Instance>
		{
			friend class Singleton<Instance>;

			public:

				/**
				 * Sets the instance function.
				 * @param instanceFunction The instance function to be set
                 * @param isBaseClass True, if the provided instance function is for the base classes GLView or GLFrameView; False, otherwise.
				 */
				inline void setInstanceFunction(const InstanceFunction& instanceFunction, const bool isBaseClass);

				/**
				 * Returns the instance.
				 * @return The instance
				 */
				GLView& instance();

			protected:

				/**
				 * Creates a new instance.
				 */
				inline Instance();

				/**
				 * Destructs an instance.
				 */
				virtual ~Instance();

			protected:

				/// The instance object.
				GLView* view_;

				/// The instance function.
				InstanceFunction baseInstanceFunction_;

                /// The instance function.
				InstanceFunction derivedInstanceFunction_;

				/// The lock of this object.
				Lock lock_;
		};

	public:

		/**
		 * Initializes the view.
		 * @return True, if succeeded
		 */
		virtual bool initialize();

		/**
		 * Releases the view.
		 * @return True, if succeeded
		 */
		virtual bool release();

		/**
		 * View resize event function.
		 * @param width New view width, with range [1, infinity)
		 * @param height New view height, with range [1, infinity)
		 * @return True, if succeeded
		 */
		virtual bool resize(const int width, const int height);

		/**
		 * Renders the next frame.
		 * @return True, if succeeded
		 */
		virtual bool render();

		/**
		 * Converts the given screen positions into frame positions.
		 * @param xScreen Horizontal screen position, with range [0, infinity)
		 * @param yScreen Vertical screen position, with range [0, infinity)
		 * @param xFrame Resulting horizontal frame position, with range [0, infinity)
		 * @param yFrame Resulting vertical frame position, with range [0, infinity)
		 * @return True, if succeeded
		 */
		virtual bool screen2frame(const Scalar xScreen, const Scalar yScreen, Scalar& xFrame, Scalar& yFrame);

		/**
		 * Touch down event function.
		 * @param x Horizontal touch position
		 * @param y Vertical touch position
		 */
		virtual void onTouchDown(const float x, const float y);

		/**
		 * Touch move event function.
		 * @param x Horizontal touch position
		 * @param y Vertical touch position
		 */
		virtual void onTouchMove(const float x, const float y);

		/**
		 * Touch move event function.
		 * @param x Horizontal touch position
		 * @param y Vertical touch position
		 */
		virtual void onTouchUp(const float x, const float y);

		/**
		 * The resume event function for events sent from the owning activity.
		 */
		virtual void onResume();

		/**
		 * The pause event function for events sent from the owning activity.
		 */
		virtual void onPause();

		/**
		 * The stop event function for events sent from the owning activity.
		 */
		virtual void onStop();

		/**
		 * The destroy event function for events sent from the owning activity.
		 */
		virtual void onDestroy();

		/**
		 * The event function for granted permissions.
		 * @param permission The permission which has been granted
		 */
		virtual void onPermissionGranted(const std::string& permission);

		/**
		 * Returns the instance of this frame view object.
		 * @return The frame view object
		 */
		static GLView& get();

		/**
		 * Returns the instance of a derived object from this frame view object.
		 * @return The derived frame view object
		 * @tparam T The data type of the derived view object
		 */
		template <typename T>
		static T& get();

		/**
		 * Sets the instance function for an optional derived class.
		 * @param instanceFunction The instance function to set
         * @param isBaseClass True, if the provided instance function is for the base classes GLView or GLFrameView; False, otherwise.
		 * @return Always True
		 */
		static inline bool registerInstanceFunction(const InstanceFunction& instanceFunction, const bool isBaseClass = false);

        /**
		 * Creates an instance of this object.
		 * @return The new instance
		 */
		static inline GLView* createInstance();

	protected:

		/**
		 * Creates a new view object.
		 */
		GLView() = default;

		/**
		 * Destructs a view object.
		 */
		virtual ~GLView() = default;
};

inline GLView::Instance::Instance() :
	view_(nullptr),
	baseInstanceFunction_(nullptr),
    derivedInstanceFunction_(nullptr)
{
	// nothing to do here
}

inline void GLView::Instance::setInstanceFunction(const InstanceFunction& instanceFunction, const bool isBaseClass)
{
    if (isBaseClass)
    {
        baseInstanceFunction_ = instanceFunction;
    }
    else
    {
	    derivedInstanceFunction_ = instanceFunction;
    }
}

template <typename T>
T& GLView::get()
{
	ocean_assert(dynamic_cast<T*>(&Instance::get().instance()) != nullptr);
	return dynamic_cast<T&>(Instance::get().instance());
}

inline bool GLView::registerInstanceFunction(const InstanceFunction& instanceFunction, const bool isBaseClass)
{
	Instance::get().setInstanceFunction(instanceFunction, isBaseClass);

	return true;
}

inline Ocean::Platform::Android::Application::GLView* GLView::createInstance()
{
	return new GLView();
}

}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_APPLICATION_GL_VIEW_H
