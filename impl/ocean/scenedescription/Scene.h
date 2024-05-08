/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SCENE_H
#define META_OCEAN_SCENEDESCRIPTION_SCENE_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/Node.h"

namespace Ocean
{

namespace SceneDescription
{

// Forward declaration
class Scene;

/**
 * Definition of a smart object reference for scene desciption scenes.
 * @see Scene, Node.
 * @ingroup scenedescription
 */
typedef SmartObjectRef<Scene, Node> SceneRef;

/**
 * This class implements the base class for all scene objects providing access to all elements of a scene.
 * A new scene object can be created by the scene description Manager object.
 * @ingroup scenedescription
 */
class OCEAN_SCENEDESCRIPTION_EXPORT Scene : virtual public Node
{
	public:

		/**
		 * Creates a new scene object.
		 * @param filename Scene filename
		 */
		explicit Scene(const std::string& filename);

		/**
		 * Returns the filename of this scene.
		 * @return Scene filename
		 */
		inline const std::string& filename() const;

	protected:

		/// Filename of this scene.
		std::string filename_;
};

inline const std::string& Scene::filename() const
{
	return filename_;
}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SCENE_H
