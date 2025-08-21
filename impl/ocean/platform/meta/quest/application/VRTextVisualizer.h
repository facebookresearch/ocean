/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_TEXT_VISUALIZER_H
#define META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_TEXT_VISUALIZER_H

#include "ocean/platform/meta/quest/application/Application.h"
#include "ocean/platform/meta/quest/application/VRVisualizer.h"

#include "ocean/base/Timestamp.h"
#include "ocean/math/RGBAColor.h"

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
 * This class implements a helper function allowing to visualize text in an Ocean-based VR application (e.g., VRNativeApplication).
 * The visualize allows to place text at arbitrary locations in the 3D environment, to update the text, or to remove the text again.<br>
 * The `text` coordinate system of the visualization is defined as follows:
 * <pre>
 * Coordinate system of text, the origin is in the center of the text, the text is visualized in the local z=0 plane:
 *
 * (text field top-left)
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
 *                       (text field bottom-right)
 *
 * |<-------------- object width --------------->|
 * </pre>
 * The visualizer uses `CV::Fonts` to rendering the text.
 * Custom fonts can be registered via `CV::Fonts::FontManager::get().registerFont()`.
 * @see VRNativeApplication.
 * @ingroup platformmetaquestapplication
 */
class OCEAN_PLATFORM_META_QUEST_APPLICATION_EXPORT VRTextVisualizer : public VRVisualizer
{
	public:

		/**
		 * Default constructor, creates a new invalid visualizer.
		 */
		inline VRTextVisualizer();

		/**
		 * Creates a new text visualizer and initializes the object with a given rendering engine and associated framebuffer.
		 * Rendering engine and framebuffer are necessary so that the rendering objects (like Scene, Transform, Texture2D) can be created and attached to the existing rendering objects.
		 * @param engine The rendering engine to be used, must be valid
		 * @param framebuffer The framebuffer to be used, must be valid
		 */
		inline VRTextVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer);

		/**
		 * Visualizes a text at a specific location in the virtual environment (defined in relation to the world).
		 * A previous visualization can be updated by specifying the old id and a new text.
		 * Beware: The visualizer must be created with a valid engine and framebuffer before usage.
		 * @param id The unique id of the visualization, the same id can be used to update/change the visualization
		 * @param world_T_text The transformation at which the text will be displayed, transforming text to world, can be invalid to remove the existing visualization
		 * @param text The text to visualize, can contain end-of-line characters for multiple text lines, can be empty to remove the existing visualization
		 * @param objectSize The size of the visualized image in virtual space (in object space), an invalid object to remove the visualization
		 * @param workaroundTimestamp Current workaround: Timestamp of the rendering engine
		 * @param fontName The name of the font to be used to render the text
		 * @param backgroundColor The background color with alpha, default is opaque gray (0.25, 0.25, 0.25, 1.0)
		 * @see visualizeTextnView().
		 */
		inline void visualizeTextInWorld(const unsigned int id, const HomogenousMatrix4& world_T_text, const std::string& text, const ObjectSize& objectSize, const Timestamp& workaroundTimestamp = Timestamp(false), const std::string& fontName = "Roboto", const RGBAColor& backgroundColor = RGBAColor(0.25f, 0.25f, 0.25f, 1.0f));

		/**
		 * Visualizes a text at a specific location in the virtual environment (defined in relation to the view).
		 * A previous visualization can be updated by specifying the old id and a new text.
		 * Beware: The visualizer must be created with a valid engine and framebuffer before usage.
		 * @param id The unique id of the visualization, the same id can be used to update/change the visualization
		 * @param view_T_text The transformation at which the text will be displayed, transforming text to view, can be invalid to remove the existing visualization
		 * @param text The text to visualize, can contain end-of-line characters for multiple text lines, can be empty to remove the existing visualization
		 * @param objectSize The size of the visualized image in virtual space (in object space), an invalid object to remove the visualization
		 * @param workaroundTimestamp Current workaround: Timestamp of the rendering engine
		 * @param fontName The name of the font to be used to render the text
		 * @param backgroundColor The background color with alpha, default is opaque gray (0.25, 0.25, 0.25, 1.0)
		 * @see visualizeTextInWorld().
		 */
		inline void visualizeTextInView(const unsigned int id, const HomogenousMatrix4& view_T_text, const std::string& text, const ObjectSize& objectSize, const Timestamp& workaroundTimestamp = Timestamp(false), const std::string& fontName = "Roboto", const RGBAColor& backgroundColor = RGBAColor(0.25f, 0.25f, 0.25f, 1.0f));

		/**
		 * Visualizes a text at a specific location in the virtual environment (defined in relation to the world or to the view).
		 * A previous visualization can be updated by specifying the old id and a new text.
		 * Beware: The visualizer must be created with a valid engine and framebuffer before usage.
		 * @param id The unique id of the visualization, the same id can be used to update/change the visualization
		 * @param reference_T_text The transformation at which the text will be displayed, transforming text to reference (either world or view), can be invalid to remove the existing visualization
		 * @param text The text to visualize, can contain end-of-line characters for multiple text lines, can be empty to remove the existing visualization
		 * @param objectSize The size of the visualized image in virtual space (in object space), an invalid object to remove the visualization
		 * @param workaroundTimestamp Current workaround: Timestamp of the rendering engine
		 * @param referenceIsWorld True, if reference is world; False, if reference is view
		 * @param fontName The name of the font to be used to render the text
		 * @param backgroundColor The background color with alpha, default is opaque gray (0.25, 0.25, 0.25, 1.0)
		 */
		void visualizeText(const unsigned int id, const HomogenousMatrix4& reference_T_text, const std::string& text, const ObjectSize& objectSize, const Timestamp& workaroundTimestamp = Timestamp(false), const bool referenceIsWorld = true, const std::string& fontName = "Roboto", const RGBAColor& backgroundColor = RGBAColor(0.25f, 0.25f, 0.25f, 1.0f));
};

inline VRTextVisualizer::VRTextVisualizer()
{
	// nothing to do here
}

inline VRTextVisualizer::VRTextVisualizer(const Rendering::EngineRef& engine, const Rendering::FramebufferRef framebuffer) :
	VRVisualizer(engine, framebuffer)
{
	// nothing to do here
}

inline void VRTextVisualizer::visualizeTextInWorld(const unsigned int id, const HomogenousMatrix4& world_T_text, const std::string& text, const ObjectSize& objectSize, const Timestamp& workaroundTimestamp, const std::string& fontName, const RGBAColor& backgroundColor)
{
	return visualizeText(id, world_T_text, text, objectSize, workaroundTimestamp, true, fontName, backgroundColor);
}

inline void VRTextVisualizer::visualizeTextInView(const unsigned int id, const HomogenousMatrix4& world_T_text, const std::string& text, const ObjectSize& objectSize, const Timestamp& workaroundTimestamp, const std::string& fontName, const RGBAColor& backgroundColor)
{
	return visualizeText(id, world_T_text, text, objectSize, workaroundTimestamp, false, fontName, backgroundColor);
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_TEXT_VISUALIZER_H
