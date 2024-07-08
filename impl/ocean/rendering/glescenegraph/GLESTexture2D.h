/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_TEXTURE_2D_H
#define META_OCEAN_RENDERING_GLES_TEXTURE_2D_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESTexture.h"

#include "ocean/rendering/Texture2D.h"
#include "ocean/rendering/DynamicObject.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class wraps a GLESceneGraph texture object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESTexture2D :
	virtual public GLESTexture,
	virtual public Texture2D,
	virtual protected DynamicObject
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the texture wrap type in s direction.
		 * @see Texture2D::wrapTypeS().
		 */
		WrapType wrapTypeS() const override;

		/**
		 * Returns the texture wrap type in t direction.
		 * @see Texture2D::wrapTypeT().
		 */
		WrapType wrapTypeT() const override;

		/**
		 * Sets the texture wrap type in s direction.
		 * @see Texture2D::setWrapTypeS().
		 */
		bool setWrapTypeS(const WrapType type) override;

		/**
		 * Sets the texture wrap type in t direction.
		 * @see Texture2D::setWrapTypeT().
		 */
		bool setWrapTypeT(const WrapType type) override;

	protected:

		/**
		 * Creates a new GLESceneGraph texture 2D object.
		 */
		GLESTexture2D();

		/**
		 * Destructs a GLESceneGraph texture 2D object.
		 */
		~GLESTexture2D() override;

		/**
		 * Determines the alignment for a plane.
		 * @param planeStrideBytes The stride of the plane in bytes, with range [1, infinity)
		 * @param rowLength The resulting row length, with range [1, infinity)
		 * @param byteAlignment The resulting byte alignment, either 1, 2, or 4
		 * @return True, if succeeded
		 */
		static bool determineAlignment(const unsigned int planeStrideBytes, unsigned int& rowLength, unsigned int& byteAlignment);

	protected:

		/// The texture wrap s type.
		WrapType wrapTypeS_;

		/// The texture wrap t type.
		WrapType wrapTypeT_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_TEXTURE_2D_H
