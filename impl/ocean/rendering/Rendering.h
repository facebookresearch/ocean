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
 * Implemented (not abstract) rendering plugins are e.g. the NvidiaSceniX or GLESceneGraph rendering pludings.<br>
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
typedef size_t ObjectId;

/**
 * Definition of an invalid object id.
 * @ingroup rendering
 */
const ObjectId invalidObjectId = ObjectId(-1);

/**
 * Definition of a vertex index.
 * @ingroup rendering
 */
typedef unsigned int VertexIndex;

/**
 * Definition of an invalid vertex index.
 * @ingroup rendering
 */
constexpr VertexIndex invalidIndex = VertexIndex(-1);

/**
 * Definition of a 3D normal.
 * @ingroup rendering
 */
typedef Vector3 Normal;

/**
 * Definition of a 2D texture coordinate.
 * @ingroup rendering
 */
typedef Vector2 TextureCoordinate;

/**
 * Definition of a 3D vertex.
 * @ingroup rendering
 */
typedef Vector3 Vertex;

/**
 * Definition of a vector holding vertex indices.
 * @ingroup rendering
 */
typedef std::vector<VertexIndex> VertexIndices;

/**
 * Definition of a vector holding normals.
 * @ingroup rendering
 */
typedef std::vector<Normal> Normals;

/**
 * Definition of a vector holding texture coordinates.
 * @ingroup rendering
 */
typedef std::vector<TextureCoordinate> TextureCoordinates;

/**
 * Definition of a vector holding vertices.
 * @ingroup rendering
 */
typedef std::vector<Vertex> Vertices;

/**
 * Definition of a vector holding vertex indices.
 * @ingroup rendering
 */
typedef std::vector<VertexIndices> VertexIndexGroups;

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
