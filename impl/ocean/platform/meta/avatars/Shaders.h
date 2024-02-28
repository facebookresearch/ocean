// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_AVATARS_SHADERS_H
#define META_OCEAN_PLATFORM_META_AVATARS_SHADERS_H

#include "ocean/platform/meta/avatars/Avatars.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/FrameTexture2D.h"
#include "ocean/rendering/ShaderProgram.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

/**
 * This class provides access to shaders for Avatars.
 * @ingroup platformmetaavatars
 */
class OCEAN_PLATFORM_META_AVATARS_EXPORT Shaders
{
	public:

		/**
		 * Returns the default shader program for all avatars.
		 * @param engine The rendering engine to be used
		 * @return The shader program
		 */
		Rendering::ShaderProgramRef shaderProgram(const Rendering::Engine& engine);

		/**
		 * Releases all shaders.
		 */
		void release();

		/**
		 * Updates the skin matrices texture.
		 * @param frameTexture The texture to be updated, must be valid
		 * @param skinMatrices The skin matrices to be used, interleaved with corresponding normal matrices, with range [2, 256 * 2], must be a multiple of two
		 * @return True, if succeeded
		 */
		static bool updateSkinMatricesTexture(Rendering::FrameTexture2D& frameTexture, const HomogenousMatricesF4& skinMatrices);

	protected:

		/// The shader program of the renderer.
		Rendering::ShaderProgramRef shaderProgram_;

		/// The shader part for the platform.
		static const char* partPlatform_;

		/// The shader part for the light definition.
		static const char* partDefinitionLight_;

		/// The shader part for the lighting function.
		static const char* partFunctionLighting_;

		/// The shader part with the skin matrix lookup shader.
		static const char* partSkinMatrixLookup_;

		/// The shader part with the vertex shader.
		static const char* partVertexShaderTexture_;

		/// The shader part with the fragment shader.
		static const char* partFragmentShader_;
};

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_AVATARS_SHADERS_H
