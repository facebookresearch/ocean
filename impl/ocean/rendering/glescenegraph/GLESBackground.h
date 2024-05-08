/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_BACKGROUND_H
#define META_OCEAN_RENDERING_GLES_BACKGROUND_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESNode.h"

#include "ocean/rendering/Background.h"

#include "ocean/math/Quaternion.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements the base class for all GLESceneGraph background nodes.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESBackground :
	virtual public GLESNode,
	virtual public Background
{
	public:

		/**
		 * Returns the position used for this background.
		 * @see Background::position().
		 */
		Vector3 position() const override;

		/**
		 * Returns the orientation used for this background.
		 * @see Background::orientation().
		 */
		Quaternion orientation() const override;

		/**
		 * Sets the position used for this background.
		 * @see Background::setPosition().
		 */
		void setPosition(const Vector3& position) override;

		/**
		 * Sets the orientation used for this background.
		 * @see Background::setOrientation().
		 */
		void setOrientation(const Quaternion& orientation) override;

	protected:

		/**
		 * Creates a new GLESceneGraph background object.
		 */
		GLESBackground();

		/**
		 * Destructs a GLESceneGraph background object.
		 */
		~GLESBackground() override;

	protected:

		/// Background position.
		Vector3 backgroundPosition;

		/// Background orientation.
		Quaternion backgroundOrientation;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_BACKGROUND_H
