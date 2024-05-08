/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_IMAGE_VISUALIZER_H
#define META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_IMAGE_VISUALIZER_H

#include "ocean/platform/meta/quest/application/Application.h"
#include "ocean/platform/meta/quest/application/VRVisualizer.h"

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
 * This class implements a helper function allowing to visualize images in an Ocean-based VR application (e.g., VRNativeApplication).
 * The visualize allows to place images at arbitrary locations in the 3D environment, to update them (e.g., a video), or to remove them again.<br>
 * The `image` coordinate system of the visualization is defined as follows:
 * <pre>
 * Coordinate system of image, the origin is in the center of the image, the image is visualized in the local z=0 plane:
 *
 * (image top-left)
 *  ---------------------------------------------
 * |                                             |
 * |                   ^                         |
 * |                   | y-axis                  |
 * |                   |                         |
 * |                   *------>  x-axis          |
 * |                  /                          |
 * |                 / z-axis                    |
 * |                v                            |
 * |                                             |
 *  ---------------------------------------------
 *                            (image bottom-right)
 *
 * |<-------------- object width --------------->|
 * </pre>
 * @see VRNativeApplication.
 * @ingroup platformmetaquestapplication
 */
class OCEAN_PLATFORM_META_QUEST_APPLICATION_EXPORT VRImageVisualizer : public VRVisualizer
{
	public:

		/**
		 * Default constructor, creates a new invalid visualizer.
		 */
		inline VRImageVisualizer();

		/**
		 * Creates a new image visualizer and initializes the object with a given rendering engine and associated framebuffer.
		 * Rendering engine and framebuffer are necessary so that the rendering objects (like Scene, Transform, Texture2D) can be created and attached to the existing rendering objects.
		 * @param engine The rendering engine to be used, must be valid
		 * @param framebuffer The framebuffer to be used, must be valid
		 */
		explicit inline VRImageVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer);

		/**
		 * Visualizes an image (e.g., for debugging purposes) at a specific location in the virtual environment (defined in relation to the world).
		 * A previous visualization can be updated by specifying the old id and a new frame.
		 * Beware: The visualizer must be created with a valid engine and framebuffer before usage.
		 * @param id The unique id of the visualization, the same id can be used to update/change the visualization
		 * @param world_T_image The transformation at which the image will be displayed, transforming image to world, can be invalid to remove the existing visualization
		 * @param frame The frame to visualize, can be invalid to remove the existing visualization
		 * @param objectSize The size of the visualized image in virtual space (in object space), an invalid object to remove the visualization
		 * @see visualizeImageInView().
		 */
		inline void visualizeImageInWorld(const unsigned int id, const HomogenousMatrix4& world_T_image, Frame&& frame, const ObjectSize& objectSize);

		/**
		 * Visualizes an image (e.g., for debugging purposes) at a specific location in the virtual environment (defined in relation to the world).
		 * A previous visualization can be updated by specifying the old id and a new frame.
		 * Beware: The visualizer must be created with a valid engine and framebuffer before usage.
		 * @param id The unique id of the visualization, the same id can be used to update/change the visualization
		 * @param world_T_image The transformation at which the image will be displayed, transforming image to world, can be invalid to remove the existing visualization
		 * @param frame The frame to visualize, can be invalid to remove the existing visualization
		 * @param objectSize The size of the visualized image in virtual space (in object space), an invalid object to remove the visualization
		 * @see visualizeImageInView().
		 */
		inline void visualizeImageInWorld(const unsigned int id, const HomogenousMatrix4& world_T_image, const Frame& frame, const ObjectSize& objectSize);

		/**
		 * Visualizes an image (e.g., for debugging purposes) at a specific location in the virtual environment (defined in relation to the view).
		 * A previous visualization can be updated by specifying the old id and a new frame.
		 * Beware: The visualizer must be created with a valid engine and framebuffer before usage.
		 * @param id The unique id of the visualization, the same id can be used to update/change the visualization
		 * @param view_T_image The transformation at which the image will be displayed, transforming image to view, can be invalid to remove the existing visualization
		 * @param frame The frame to visualize, can be invalid to remove the existing visualization
		 * @param objectSize The size of the visualized image in virtual space (in object space), an invalid object to remove the visualization
		 * @see visualizeImageInWorld().
		 */
		inline void visualizeImageInView(const unsigned int id, const HomogenousMatrix4& view_T_image, Frame&& frame, const ObjectSize& objectSize);

		/**
		 * Visualizes an image (e.g., for debugging purposes) at a specific location in the virtual environment (defined in relation to the view).
		 * A previous visualization can be updated by specifying the old id and a new frame.
		 * Beware: The visualizer must be created with a valid engine and framebuffer before usage.
		 * @param id The unique id of the visualization, the same id can be used to update/change the visualization
		 * @param view_T_image The transformation at which the image will be displayed, transforming image to view, can be invalid to remove the existing visualization
		 * @param frame The frame to visualize, can be invalid to remove the existing visualization
		 * @param objectSize The size of the visualized image in virtual space (in object space), an invalid object to remove the visualization
		 * @see visualizeImageInWorld().
		 */
		inline void visualizeImageInView(const unsigned int id, const HomogenousMatrix4& view_T_image, const Frame& frame, const ObjectSize& objectSize);

		/**
		 * Visualizes an image (e.g., for debugging purposes) at a specific location in the virtual environment (defined in relation to the world or to the view).
		 * A previous visualization can be updated by specifying the old id and a new frame.
		 * Beware: The visualizer must be created with a valid engine and framebuffer before usage.
		 * @param id The unique id of the visualization, the same id can be used to update/change the visualization
		 * @param reference_T_image The transformation at which the image will be displayed, transforming image to reference (either world or view), can be invalid to remove the existing visualization
		 * @param frame The frame to visualize, can be invalid to remove the existing visualization
		 * @param objectSize The size of the visualized image in virtual space (in object space), an invalid object to remove the visualization
		 * @param referenceIsWorld True, if reference is world; False, if reference is view
		 */
		void visualizeImage(const unsigned int id, const HomogenousMatrix4& reference_T_image, Frame&& frame, const ObjectSize& objectSize, const bool referenceIsWorld = true);

		/**
		 * Visualizes an image (e.g., for debugging purposes) at a specific location in the virtual environment (defined in relation to the world or to the view).
		 * A previous visualization can be updated by specifying the old id and a new frame.
		 * Beware: The visualizer must be created with a valid engine and framebuffer before usage.
		 * @param id The unique id of the visualization, the same id can be used to update/change the visualization
		 * @param reference_T_image The transformation at which the image will be displayed, transforming image to reference (either world or view), can be invalid to remove the existing visualization
		 * @param frame The frame to visualize, can be invalid to remove the existing visualization
		 * @param objectSize The size of the visualized image in virtual space (in object space), an invalid object to remove the visualization
		 * @param referenceIsWorld True, if reference is world; False, if reference is view
		 */
		inline void visualizeImage(const unsigned int id, const HomogenousMatrix4& reference_T_image, const Frame& frame, const ObjectSize& objectSize, const bool referenceIsWorld = true);
};

VRImageVisualizer::VRImageVisualizer()
{
	// nothing to do here
}

VRImageVisualizer::VRImageVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer) :
	VRVisualizer(engine, framebuffer)
{
	// nothing to do here
}

inline void VRImageVisualizer::visualizeImageInWorld(const unsigned int id, const HomogenousMatrix4& world_T_image, Frame&& frame, const ObjectSize& objectSize)
{
	return visualizeImage(id, world_T_image, std::move(frame), objectSize, true);
}

inline void VRImageVisualizer::visualizeImageInWorld(const unsigned int id, const HomogenousMatrix4& world_T_image, const Frame& frame, const ObjectSize& objectSize)
{
	return visualizeImage(id, world_T_image, frame, objectSize, true);
}

inline void VRImageVisualizer::visualizeImageInView(const unsigned int id, const HomogenousMatrix4& world_T_image, Frame&& frame, const ObjectSize& objectSize)
{
	return visualizeImage(id, world_T_image, std::move(frame), objectSize, false);
}

inline void VRImageVisualizer::visualizeImageInView(const unsigned int id, const HomogenousMatrix4& world_T_image, const Frame& frame, const ObjectSize& objectSize)
{
	return visualizeImage(id, world_T_image, frame, objectSize, false);
}

inline void VRImageVisualizer::visualizeImage(const unsigned int id, const HomogenousMatrix4& reference_T_image, const Frame& frame, const ObjectSize& objectSize, const bool referenceIsWorld)
{
	return visualizeImage(id, reference_T_image, Frame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT), objectSize, referenceIsWorld);
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_IMAGE_VISUALIZER_H
