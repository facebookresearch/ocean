/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_PROGRAM_MANAGER_H
#define META_OCEAN_RENDERING_GLES_PROGRAM_MANAGER_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESAttribute.h"
#include "ocean/rendering/glescenegraph/GLESShader.h"
#include "ocean/rendering/glescenegraph/GLESShaderProgram.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

#include "ocean/rendering/Engine.h"

#include <map>

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a manager for OpenGL ES shader programs.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESProgramManager : public Singleton<GLESProgramManager>
{
	friend class Singleton<GLESProgramManager>;

	protected:

		/**
		 * Definition of a vector holding pointers to shader codes.
		 */
		typedef std::vector<const char*> ShaderCodes;

		/**
		 * Definition of a map mapping shader codes to compiled shader objects.
		 */
		typedef std::map<ShaderCodes, GLESShaderRef> ShaderMap;

		/**
		 * Definition of an unordered map mapping program types to program objects.
		 */
		typedef std::unordered_map<GLESAttribute::ProgramType, GLESShaderProgramRef> ProgramMap;

	public:

		/**
		 * Returns a specified shader with the specified functionalities.
		 * @param engine The rendering engine to be used
		 * @param programType The necessary shader program type
		 * @return The specified shader object, invalid if the program could not be created
		 */
		GLESShaderProgramRef program(const Engine& engine, const GLESAttribute::ProgramType programType);

		/**
		 * Releases the shader manager.
		 * This function should be called once before program termination.
		 */
		void release();

	protected:

		/**
		 * Creates an shader manager.
		 */
		GLESProgramManager();

		/**
		 * Destructs a shader manager.
		 */
		~GLESProgramManager();

		/**
		 * Returns the vertex shader code with a specified functionality.
		 * @param programType Shader program functionalities
		 * @return Vertex shader code which must not be released
		 */
		ShaderCodes vertexShaderCodes(const GLESAttribute::ProgramType programType) const;

		/**
		 * Returns the fragment shader code with a specified functionality.
		 * @param programType Shader program functionalities
		 * @return Fragment shader code which must not be released
		 */
		ShaderCodes fragmentShaderCodes(const GLESAttribute::ProgramType programType) const;

	protected:

		/// Map mapping vertex shader codes to compiled shader objects.
		ShaderMap vertexShaders_;

		/// Map mapping fragment shader codes to compiled shader objects.
		ShaderMap fragmentShaders_;

		/// Map mapping program types to shader program objects.
		ProgramMap programMap_;

		/// Lock for the program manager.
		Lock lock_;

#ifdef OCEAN_DEBUG

		/// State determining whether this manager has been released.
		bool debugReleased_;

#endif // OCEAN_DEBUG

		/// The code part containing platform specific information e.g., shader version.
		static const char* partPlatform_;

		/// The code part containing the macro to convert a 1-texture Y8 lookup to a RGBA32 value
		static const char* partOneTextureLookupY8ToRGBA32_;

		/// The code part containing the macro to convert a 1-texture RGBA32 lookup to a RGBA32 value
		static const char* partOneTextureLookupRGBA32ToRGBA32_;

		/// The code part containing the macro to convert a 1-texture BGRA32 lookup to a RGBA32 value
		static const char* partOneTextureLookupBGRA32ToRGBA32_;

		/// The code part containing the macro to convert the 1-texture YUV24 lookup to a RGBA32 value
		static const char* partOneTextureLookupYUV24ToRGBA32_;

		/// The code part containing the macro to convert the 1-texture YUV24 lookup to a RGBA32 value
		static const char* partOneTextureLookupYVU24ToRGBA32_;

		/// The code part containing the macro to convert the 2-texture Y_UV12 lookup to a RGBA32 value
		static const char* partTwoTexturesLookupY_UV12ToRGBA32_;

		/// The code part containing the macro to convert the 2-texture Y_VU12 lookup to a RGBA32 value
		static const char* partTwoTexturesLookupY_VU12ToRGBA32_;

		/// The code part containing the macro to convert the 2-texture Y_U_V12 lookup to a RGBA32 value
		static const char* partTwoTexturesLookupY_U_V12ToRGBA32_;

		/// The code part defining the Material struct.
		static const char* partDefinitionMaterial_;

		/// The code part defining the Light struct.
		static const char* partDefinitionLight_;

		/// The code part defining the function to determine the light for a vertex based on up to 8 lights
		static const char* partFunctionLighting_;

		/// Vertex shader code: PT_STATIC_COLOR.
		static const char* programVertexShaderStaticColor_;

		/// Vertex shader code: PT_COLOR_ID.
		static const char* programVertexShaderColorId_;

		/// Vertex shader code: PT_POINTS.
		static const char* programVertexShaderPoints_;

		/// Vertex shader code: PT_POINTS | PT_MATERIAL.
		static const char* programVertexShaderPointsMaterial_;

		/// Vertex shader code: PT_POINTS | PT_MATERIAL | PT_LIGHT.
		static const char* programVertexShaderPointsMaterialLight_;

		/// Vertex shader code: PT_DEBUG_GRAY.
		static const char* programVertexShaderDebugGray_;

		/// Vertex shader code: PT_MATERIAL.
		static const char* programVertexShaderMaterial_;

		/// Vertex shader code: PT_MATERIAL | PT_LIGHT.
		static const char* programVertexShaderMaterialLight_;

		/// Vertex shader code: PT_TEXTURE_LOWER/UPPLER_LEFT | PT_TEXTURE_BGRA/RGBA.
		static const char* programVertexShaderTexture_;

		/// Vertex shader code: PT_MATERIAL | PT_LIGHT | PT_TEXTURE_LOWER/UPPER_LEFT | PT_TEXTURE_BGRA/RGBA:
		static const char* programVertexShaderMaterialLightTexture_;

		/// Vertex shader code: PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST:
		static const char* programVertexShaderPhantomVideoTextureCoordinatesFast_;

		/// Vertex shader code: PT_OPAQUE_TEXT_Y | PT_MATERIAL | PT_LIGHT.
		static const char* programVertexShaderOpaqueTextMaterialLight_;

		/// Vertex shader code: PT_TRANSPARENT_TEXT_Y | PT_MATERIAL | PT_LIGHT.
		static const char* programVertexShaderTransparentTextMaterialLight_;

		/// Fragment shader code: PT_STATIC_COLOR.
		static const char* programFragmentShaderStaticColor_;

		/// Fragment shader code: PT_DEBUG_GRAY.
		static const char* programFragmentShaderOneSidedColor_;

		/// Fragment shader code: PT_MATERIAL.
		static const char* programFragmentShaderTwoSidedColor_;

		/// Fragment shader code for color ids.
		static const char* programFragmentShaderColorId_;

		/// Fragment shader code for one texture.
		static const char* programFragmentShaderOneTexture_;

		/// Fragment shader code for two textures.
		static const char* programFragmentShaderTwoTextures_;

		/// Fragment shader code: PT_MATERIAL | PT_TEXTURE_LOWER/UPPER_LEFT.
		static const char* programFragmentShaderMaterialTexture_;

		/// Fragment shader code: PT_PHANTOM_VIDEO_FAST | PT_TEXTURE_LOWER/UPPER_LEFT.
		static const char* programFragmentShaderPhantomVideoFastOneTexture_;

		/// Fragment shader code: PT_PHANTOM_VIDEO_FAST | PT_TEXTURE_LOWER/UPPER_LEFT.
		static const char* programFragmentShaderPhantomVideoFastTwoTextures_;

		/// Fragment shader code: PT_OPAQUE_TEXT_Y.
		static const char* programFragmentShaderOpaqueTextY_;

		/// Fragment shader code: PT_TRANSPARENT_TEXT_Y.
		static const char* programFragmentShaderTransparentTextY_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_PROGRAM_MANAGER_H
