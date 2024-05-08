/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_NODE_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/Node.h"

#include "ocean/rendering/Object.h"

namespace Ocean
{

namespace SceneDescription
{

// Forward declaration.
class SDLNode;
// Forward declaration.
class SDLScene;

/**
 * Definition of a smart object reference for loader scene description nodes.
 * @see SDLNode, Node.
 * @ingroup scenedescription
 */
typedef SmartObjectRef<SDLNode, Node> SDLNodeRef;

/**
 * This class implements the base class for all scene description nodes.
 * @ingroup scenedescription
 */
class OCEAN_SCENEDESCRIPTION_EXPORT SDLNode : virtual public Node
{
	public:

		/**
		 * Returns the scene description type of this node.
		 * @see Node::descriptionType().
		 */
		DescriptionType descriptionType() const override;

		/**
		 * Applies this node to the rendering engine.
		 * @param engine Rendering engine to apply to
		 * @param scene Scene object holding all nodes
		 * @param parentDescription Parent scene description node
		 * @param parentRendering Parent rendering node
		 * @return Resulting rendering object
		 */
		virtual Rendering::ObjectRef apply(const Rendering::EngineRef& engine, const SDLScene& scene, SDLNode& parentDescription, const Rendering::ObjectRef& parentRendering) = 0;
};

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_NODE_H
