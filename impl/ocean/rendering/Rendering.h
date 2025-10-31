/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_RENDERING_H
#define META_OCEAN_RENDERING_RENDERING_H

#include "ocean/base/Base.h"
#include "ocean/base/Exception.h"
#include "ocean/base/Messenger.h"

#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include <vector>

namespace Ocean
{

namespace Rendering
{

/**
 * @defgroup rendering Ocean Rendering Abstraction Library
 * @{
 * The Ocean Rendering Library is the base library and organizer of all rendering libraries / plugins.<br>
 * In major, this library holds abstract objects only and therefore is used as an interface to provide access to different rendering plugins.<br>
 * Each rendering plugin provides a framebuffer as well as a scenegraph.<br>
 *
 * Two different framebuffers can be chosen: a standard window framebuffer or a bitmap framebuffer.<br>
 * The scenegraph provides standard scenegraph elements like e.g. nodes, groups, transform nodes, geometry nodes, shapes, materials and textures.<br>
 * Derived rendering libraries use this abstract base library to implement rendering plugins with e.g. different 3rdparty frameworks.<br>
 * Use the manager to get access to a specific rendering plugin.<br>
 * Beware: Never use the specific rendering plugin directly.<br>
 * Using the abstract rendering objects guarantees that different rendering plugins can be used by the same code.<br>
 *
 * Each engine provides an object factory to create object and nodes necessary for scene graph building.<br>
 * Implemented (not abstract) rendering plugins are e.g. GLESceneGraph rendering plugins.<br>
 *
 * However, a rendering plugin is allowed to implement a subset of the entire functionality only.<br>
 * Therefore, not supported functions will throw an exception.<br>
 * The library is platform independent.<br>
 * @see Manager, Engine, Factory, Framebuffer, Object, Node, NotSupportedException.
 * @}
 */

/**
 * @namespace Ocean::Rendering Namespace of the Rendering library.<p>
 * The Namespace Ocean::Rendering is used in the entire Ocean Rendering Library.
 */

/**
 * Definition of a object id.
 * @ingroup rendering
 */
using ObjectId = size_t;

/**
 * Definition of an invalid object id.
 * @ingroup rendering
 */
const ObjectId invalidObjectId = ObjectId(-1);

/**
 * Definition of a vertex index.
 * @ingroup rendering
 */
using VertexIndex = unsigned int;

/**
 * Definition of an invalid vertex index.
 * @ingroup rendering
 */
constexpr VertexIndex invalidIndex = VertexIndex(-1);

/**
 * Definition of a 3D normal.
 * @ingroup rendering
 */
using Normal = Vector3;

/**
 * Definition of a 2D texture coordinate.
 * @ingroup rendering
 */
using TextureCoordinate = Vector2;

/**
 * Definition of a 3D vertex.
 * @ingroup rendering
 */
using Vertex = Vector3;

/**
 * Definition of a vector holding vertex indices.
 * @ingroup rendering
 */
using VertexIndices = std::vector<VertexIndex>;

/**
 * Definition of a vector holding normals.
 * @ingroup rendering
 */
using Normals = std::vector<Normal>;

/**
 * Definition of a vector holding texture coordinates.
 * @ingroup rendering
 */
using TextureCoordinates = std::vector<TextureCoordinate>;

/**
 * Definition of a vector holding vertices.
 * @ingroup rendering
 */
using Vertices = std::vector<Vertex>;

/**
 * Definition of a vector holding vertex indices.
 * @ingroup rendering
 */
using VertexIndexGroups = std::vector<VertexIndices>;

// Defines OCEAN_RENDERING_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_RENDERING_EXPORT
		#define OCEAN_RENDERING_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_RENDERING_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_RENDERING_EXPORT
#endif

}

}

#endif // META_OCEAN_RENDERING_RENDERING_H
