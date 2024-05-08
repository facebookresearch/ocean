/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_NODE_H
#define META_OCEAN_RENDERING_NODE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Object.h"
#include "ocean/rendering/ObjectRef.h"
#include "ocean/rendering/Renderable.h"

#include "ocean/math/BoundingBox.h"
#include "ocean/math/BoundingSphere.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Node;

/**
 * Definition of a smart object reference holding a node.
 * @see SmartObjectRef, Node.
 * @ingroup rendering
 */
typedef SmartObjectRef<Node> NodeRef;

/**
 * Definition of a vector holding node references.
 * @ingroup rendering
 */
typedef std::vector<NodeRef> NodeRefs;

/**
 * This is the base class for all rendering nodes.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Node : virtual public Object
{
	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

		/**
		 * Returns whether the node is visible.
		 * As default the node is visible.
		 * @see setVisible().
		 * @return True, if so
		 */
		virtual bool visible() const;

		/**
		 * Returns the bounding box of this node.
		 * @param involveLocalTransformation True, to involve local transformations (e.g., from a Transform node) into the bounding box; False, to skip any local transformation
		 * @return Node bounding box
		 */
		virtual BoundingBox boundingBox(const bool involveLocalTransformation = true) const;

		/**
		 * Returns the bounding sphere of this node.
		 * @param involveLocalTransformation True, to involve local transformations (e.g., from a Transform node) into the bounding box; False, to skip any local transformation
		 * @return Bounding sphere
		 */
		virtual BoundingSphere boundingSphere(const bool involveLocalTransformation = true) const;

		/**
		 * Returns the first possible transformation transforming this node into world coordinates.
		 * The returned transformation is the concatenated transformation result of this node with all parent nodes up to the global root-scene.<br>
		 * @return The first possible world transformation, which is world_T_node
		 * @see worldTransformations().
		 */
		virtual HomogenousMatrix4 worldTransformation() const;

		/**
		 * Returns all possible transformations transforming this node into world coordinates.
		 * The returned transformations are concatenated transformations of this node with all parent nodes up to the global root-scene.<br>
		 * @return All possible world transformations, which is world_T_nodes
		 * @see worldTransformation().
		 */
		virtual HomogenousMatrices4 worldTransformations() const;

		/**
		 * Sets whether the node is visible.
		 * As default the node is visible.
		 * @param visible True, to make the node visible
		 * @see visible().
		 */
		virtual void setVisible(const bool visible);

	protected:

		/**
		 * Creates a new node.
		 */
		Node();

		/**
		 * Destructs a node.
		 */
		~Node() override;
};

}

}

#endif // META_OCEAN_RENDERING_NODE_H
