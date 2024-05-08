/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_ATTRIBUTE_H
#define META_OCEAN_RENDERING_GLES_ATTRIBUTE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESObject.h"

#include "ocean/rendering/Attribute.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

// Forward declaration.
class GLESFramebuffer;
class GLESShaderProgram;

/**
 * This class wraps a GLESceneGraph attribute object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESAttribute :
	virtual public GLESObject,
	virtual public Attribute
{
	public:

		/**
		 * Definition of different shader functionalities.
		 */
		enum ProgramType : uint32_t
		{
			/// Not specified shader program.
			PT_UNKNOWN = 0u,
			/// Shader using a static color.
			PT_STATIC_COLOR = (1u << 0u),
			/// Shader using a static color and rendering points.
			PT_POINTS = (1u << 1u),
			/// Shader using a default gray material color.
			PI_DEBUG_GRAY = (1u << 2u),
			/// Shader using material.
			PT_MATERIAL = (1u << 3u),
			/// Shader using light.
			PT_LIGHT = (1u << 4u),
			/// Shader using one lower left oriented texture.
			PT_TEXTURE_LOWER_LEFT = (1u << 5u),
			/// Shader using one upper left oriented texture.
			PT_TEXTURE_UPPER_LEFT = (1u << 6u),
			/// Shader using one Y texture.
			PT_TEXTURE_Y = (1u << 7u),
			/// Shader using one RGBA texture.
			PT_TEXTURE_RGBA = (1u << 8u),
			/// Shader using one BGRA texture.
			PT_TEXTURE_BGRA = (1u << 9u),
			/// Shader using one YUV24 texture.
			PT_TEXTURE_YUV24 = (1u << 10u),
			/// Shader using one YVU24 texture.
			PT_TEXTURE_YVU24 = (1u << 11u),
			/// Shader using one Y_UV12 texture.
			PT_TEXTURE_Y_UV12 = (1u << 12u),
			/// Shader using one Y_VU12 texture.
			PT_TEXTURE_Y_VU12 = (1u << 13u),
			/// Shader using one Y_U_V12 texture.
			PT_TEXTURE_Y_U_V12 = (1u << 14u),
			/// Shader using multiple textures only.
			PT_TEXTURES = (1u << 15u),
			/// Shader for phantom objects using video see-through Augmented Reality.
			PT_PHANTOM_VIDEO_FAST = (1u << 16u),
			/// Shader for phantom video textures used in video see-through Augmented Reality.
			PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST = (1u << 17u),
			/// Shader for text using a transparent grayscale texture.
			PT_TEXT_TRANSPARENT_Y = (1u << 18u),
			/// Shader for text using an opaque grayscale texture.
			PT_TEXT_OPAQUE_Y = (1u << 19u),
			/// Shader for color ids.
			PT_COLOR_ID = (1u << 20u),
			/// Shader waiting for more specific information.
			PT_PENDING = (1u << 21u),
			/// The shader is a custom shader.
			PT_CUSTOM = (1u << 22u)
		};

	public:

		/**
		 * Binds this attribute.
		 * @param framebuffer Framebuffer initiated the rendering process
		 * @param shaderProgram Shader program to be applied
		 */
		virtual void bindAttribute(const GLESFramebuffer& framebuffer, GLESShaderProgram& shaderProgram) = 0;

		/**
		 * Unbinds this attribute.
		 */
		virtual void unbindAttribute();

		/**
		 * Returns the shader type necessary to render an object with this attribute.
		 * @return Shader program type
		 */
		virtual ProgramType necessaryShader() const;

		/**
		 * Translates the program value type into a string containing the individual sub program type.
		 * @param type The program type(s) to translate
		 * @return The program type as string
		 */
		static std::string translateProgramType(const GLESAttribute::ProgramType type);

	protected:

		/**
		 * Creates a new GLESceneGraph attribute object.
		 */
		GLESAttribute();

		/**
		 * Destructs a GLESceneGraph attribute object.
		 */
		~GLESAttribute() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_ATTRIBUTE_H
