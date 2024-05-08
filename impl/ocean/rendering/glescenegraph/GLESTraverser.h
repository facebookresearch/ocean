/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_TRAVERSER_H
#define META_OCEAN_RENDERING_GLES_TRAVERSER_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"
#include "ocean/rendering/glescenegraph/GLESLightSource.h"
#include "ocean/rendering/glescenegraph/GLESObject.h"
#include "ocean/rendering/glescenegraph/GLESRenderable.h"
#include "ocean/rendering/glescenegraph/GLESShaderProgram.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Node.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a traverser for renderables.
 * Each renderable which is supposed to be renderd in the next frame is managed inside this traverser.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESTraverser
{
	protected:

		/**
		 * This class stores the data which is necessary to render one renderable.
		 */
		class TraverserObject
		{
			public:

				/**
				 * Creates a new traverser object.
				 * @param renderable The renderable to be added, must be valid
				 * @param attributeSet The attribute set which is connected with the renderable, must be valid
				 * @param camera_T_renderable The transformation between the renderable and the camera, must be valid
				 * @param normalMatrix The normal transformation matrix with is the transposed inverse of the upper 3x3 model view matrix
				 * @param lights The lights used the render this renderable, can be empty
				 */
				inline TraverserObject(const RenderableRef& renderable, const AttributeSetRef& attributeSet, const HomogenousMatrix4& camera_T_renderable, const SquareMatrix3& normalMatrix, const Lights& lights);

				/**
				 * Renders all elements which have been gathered.
				 * @param framebuffer The framebuffer in which the objects are rendered
				 * @param projection The projection matrix to be used, must be valid
				 * @param camera_T_world The transformation between world and camera, must be valid
				 */
				inline void render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projection, const HomogenousMatrix4& camera_T_world) const;

				/**
				 * Renders all elements which have been gathered.
				 * @param projection The projection matrix to be used, must be valid
				 * @param camera_T_world The transformation between world and camera, must be valid
				 * @param shaderProgram The shader program to use for all objects
				 */
				inline void render(const SquareMatrix4& projection, const HomogenousMatrix4& camera_T_world, GLESShaderProgram& shaderProgram) const;

				/**
				 * Returns the renderable of this traverser object.
				 * @return The object's renderable
				 */
				inline const SmartObjectRef<GLESRenderable>& renderable() const;

				/**
				 * Returns whether the distance of the left object is closer to the camera than the right object.
				 * @param left The left traverser object to compare
				 * @param right The right traverser object to compare
				 * @return True, if so
				 */
				static inline bool compareDistance(const TraverserObject& left, const TraverserObject& right);

			protected:

				/// The renderable object.
				SmartObjectRef<GLESRenderable> renderable_;

				/// The attribute set which is connected with the renderable.
				SmartObjectRef<GLESAttributeSet> attributeSet_;

				/// The transformation between the renderable and the camera.
				HomogenousMatrix4 camera_T_renderable_;

				/// The normal transformation matrix with is the transposed inverse of the upper 3x3 model view matrix.
				SquareMatrix3 normalMatrix_;

				/// The lights used the render this renderable.
				Lights lights_;
		};

		/**
		 * Definition of a vector holding traverser objects.
		 */
		typedef std::vector<TraverserObject> TraverserObjects;

	public:

		/**
		 * Creates a traverser object.
		 */
		GLESTraverser() = default;

		/**
		 * Renders all gathered elements.
		 * @param framebuffer The framebuffer in which the objects are rendered
		 * @param projection The projection matrix to be used, must be valid
		 * @param camera_T_world The transformation between world and camera, must be valid
		 */
		void render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projection, const HomogenousMatrix4& camera_T_world);

		/**
		 * Renders all gathered elements with an individual color id.
		 * @param engine The rendering object to be used
		 * @param projection The projection matrix to be used, must be valid
		 * @param camera_T_world The transformation between world and camera, must be valid
		 */
		void renderColorIds(const Engine& engine, const SquareMatrix4& projection, const HomogenousMatrix4& camera_T_world);

		/**
		 * Adds a new renderable to this traverser.
		 * @param renderable The renderable to be added, must be valid
		 * @param attributeSet The attribute set which is connected with the renderable, must be valid
		 * @param camera_T_renderable The transformation between the renderable and the camera, must be valid
		 * @param normalMatrix The normal transformation matrix with is the transposed inverse of the upper 3x3 model view matrix
		 * @param lights The lights used the render this renderable, can be empty
		 */
		void addRenderable(const RenderableRef& renderable, const AttributeSetRef& attributeSet, const HomogenousMatrix4& camera_T_renderable, const SquareMatrix3& normalMatrix, const Lights& lights);

		/**
		 * Lookups the renderable which has been rendered with a given color id.
		 * This function needs to be called after renderColorIds().
		 * @param colorId The id for which the renderable will be returned
		 * @return The renderable, invalid if not corresponding renderable exists
		 */
		RenderableRef renderableFromColorId(const uint32_t colorId) const;

		/**
		 * Removes all gathered renderables from this traverser.
		 */
		void clear();

	protected:

		/// The renderable object with depth attribute.
		TraverserObjects depthTraverserObjects_;

		/// The renderable object with depth and blend attributes.
		TraverserObjects defaultTraverserObjects_;

		/// The renderable object with blend attribute.
		TraverserObjects blendTraverserObjects_;

		/// The shader able to render objects with individual color ids.
		GLESShaderProgramRef shaderProgramColorId_;
};

inline GLESTraverser::TraverserObject::TraverserObject(const RenderableRef& renderable, const AttributeSetRef& attributeSet, const HomogenousMatrix4& camera_T_renderable, const SquareMatrix3& normalMatrix, const Lights& lights) :
	renderable_(renderable),
	attributeSet_(attributeSet),
	camera_T_renderable_(camera_T_renderable),
	normalMatrix_(normalMatrix),
	lights_(lights)
{
	// nothing to do here
}

inline void GLESTraverser::TraverserObject::render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projection, const HomogenousMatrix4& camera_T_world) const
{
	ocean_assert(!projection.isSingular());
	ocean_assert(renderable_ && attributeSet_);

	renderable_->render(framebuffer, projection, camera_T_renderable_, camera_T_world, normalMatrix_, *attributeSet_, lights_);
}

inline void GLESTraverser::TraverserObject::render(const SquareMatrix4& projection, const HomogenousMatrix4& camera_T_world, GLESShaderProgram& shaderProgram) const
{
	ocean_assert(!projection.isSingular());
	ocean_assert(renderable_);

	renderable_->render(projection, camera_T_renderable_, camera_T_world, normalMatrix_, shaderProgram);
}

inline const SmartObjectRef<GLESRenderable>& GLESTraverser::TraverserObject::renderable() const
{
	return renderable_;
}

inline bool GLESTraverser::TraverserObject::compareDistance(const TraverserObject& left, const TraverserObject& right)
{
	return left.camera_T_renderable_.translation().sqr() < right.camera_T_renderable_.translation().sqr();
}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_NODE_H
