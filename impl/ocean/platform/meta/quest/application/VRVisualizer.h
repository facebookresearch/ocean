/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_VISUALIZER_H
#define META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_VISUALIZER_H

#include "ocean/platform/meta/quest/application/Application.h"

#include "ocean/base/Timestamp.h"

#include "ocean/math/Numeric.h"
#include "ocean/math/Vector2.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace Application
{

/**
 * This class implements the base class for all VR visualizers allowing to visualize e.g., images or text in an Ocean-based VR application (e.g., VRNativeApplication).
 * The visualize allows to place the visuals at arbitrary locations in the 3D environment, to update the visuals, or to remove them again.
 * @see VRNativeApplication.
 * @ingroup platformmetaquestapplication
 */
class VRVisualizer
{
	public:

		/**
		 * Definition of a size object allowing to specify either width and height, or only width, or only height.
		 */
		class ObjectSize
		{
			public:

				/**
				 * Creates a new size object with invalid value.
				 */
				inline ObjectSize();

				/**
				 * Creates a new size object
				 * The size object can be specified with either width and height, or only width, or only height.<br>
				 * In case width and height is specified, the resulting visualization will have the desired width and height in object space (regardless of the actual aspect ratio of the object to visualize).<br>
				 * In case either width or height is specified, the resulting visualization will have either the desired width or height in object space (and will preserve the aspect ratio of the object to visualize).
				 * @param width The width of the object, with range [0, infinity)
				 * @param height The height of the object, with range [0, infinity)
				 */
				inline ObjectSize(const Scalar width, const Scalar height = 0);

				/**
				 * Returns the width value.
				 * @return The object width
				 */
				inline Scalar width() const;

				/**
				 * Returns the width value.
				 * @return The object width
				 */
				inline Scalar height() const;

				/**
				 * Returns the desired extent in object space for a given reference width and height (mainly for the reference aspect ratio).
				 * The resulting extent will preserve the aspect ratio of the reference values as long as either width or height is specified in this size object.
				 * @param referenceWidth The width of the reference object for which the desired extent will be determined, with range (0, infinity)
				 * @param referenceHeight The height of the reference object for which the desired extent will be determined, with range (0, infinity)
				 */
				inline Vector2 size(const Scalar referenceWidth, const Scalar referenceHeight) const;

				/**
				 * Returns whether this object has at least one value size value.
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/// The width, if any.
				Scalar width_;

				/// the object height, if any.
				Scalar height_;
		};

	public:

		/**
		 * Returns whether the visualizer has been initialized correctly and thus can be used.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether the visualizer has been initialized correctly and thus can be used.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/**
		 * Default constructor, creates a new invalid visualizer.
		 */
		inline VRVisualizer() = default;

		/**
		 * Default destructor.
		 */
		virtual ~VRVisualizer() = default;

		/**
		 * Creates a new text visualizer and initializes the object with a given rendering engine and associated framebuffer.
		 * Rendering engine and framebuffer are necessary so that the rendering objects (like Scene, Transform, Texture2D) can be created and attached to the existing rendering objects.
		 * @param engine The rendering engine to be used, must be valid
		 * @param framebuffer The framebuffer to be used, must be valid
		 */
		inline VRVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer);

		/**
		 * Disable the copy constructor
		 * @param vrVisualizer The other instance that would have been copied to this instance
		 */
		VRVisualizer(const VRVisualizer& vrVisualizer) = delete;

		/**
		 * Move constructor
		 * @param vrVisualizer The other instance that will be moved to this instance
		 */
		inline VRVisualizer(VRVisualizer&& vrVisualizer);

		/**
		 * Disable the copy assignment operator
		 * @param vrVisualizer The other instance that would have been copied to this instance
		 */
		VRVisualizer& operator=(const VRVisualizer& vrVisualizer) = delete;

		/**
		 * The move assignment operator
		 * @param vrVisualizer The other instance that will be moved to this instance
		 */
		inline VRVisualizer& operator=(VRVisualizer&& vrVisualizer);

	protected:

		/// The rendering engine to be used for visualization
		Rendering::EngineRef engine_;

		/// The rendering framebuffer to be used for visualization.
		Rendering::WindowFramebufferRef framebuffer_;

		/// The visualizer's lock.
		mutable Lock lock_;
};

inline VRVisualizer::ObjectSize::ObjectSize() :
	width_(0),
	height_(0)
{
	ocean_assert(!isValid());
}

inline VRVisualizer::ObjectSize::ObjectSize(const Scalar width, const Scalar height) :
	width_(width),
	height_(height)
{
	ocean_assert(width_ >= 0);
	ocean_assert(height_ >= 0);
}

inline Scalar VRVisualizer::ObjectSize::width() const
{
	return width_;
}

inline Scalar VRVisualizer::ObjectSize::height() const
{
	return height_;
}

inline Vector2 VRVisualizer::ObjectSize::size(const Scalar referenceWidth, const Scalar referenceHeight) const
{
	ocean_assert(isValid());
	ocean_assert(referenceWidth > Numeric::eps() && referenceHeight > Numeric::eps());

	if (Numeric::isNotEqualEps(width_) && Numeric::isNotEqualEps(height_))
	{
		return Vector2(width_, height_);
	}

	const Scalar referenceAspectRatio = Numeric::ratio(referenceWidth, referenceHeight);

	if (Numeric::isNotEqualEps(width_))
	{
		ocean_assert(Numeric::isNotEqualEps(referenceAspectRatio));
		return Vector2(width_, width_ / referenceAspectRatio);
	}
	else
	{
		return Vector2(height_ * referenceAspectRatio, height_);
	}
}

inline bool VRVisualizer::ObjectSize::isValid() const
{
	return Numeric::isNotEqualEps(width_) || Numeric::isNotEqualEps(height_);
}

inline VRVisualizer::VRVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer) :
	engine_(engine),
	framebuffer_(framebuffer)
{
	ocean_assert(engine_ && framebuffer_);
}

inline VRVisualizer::VRVisualizer(VRVisualizer&& vrVisualizer) :
	VRVisualizer()
{
	*this = std::move(vrVisualizer);
}

inline VRVisualizer& VRVisualizer::operator=(VRVisualizer&& vrVisualizer)
{
	if (this != &vrVisualizer)
	{
		engine_ = std::move(vrVisualizer.engine_);
		framebuffer_ = std::move(vrVisualizer.framebuffer_);
	}

	return *this;
};

inline bool VRVisualizer::isValid() const
{
	return engine_ && framebuffer_;
}

inline VRVisualizer::operator bool() const
{
	return isValid();
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_VISUALIZER_H
