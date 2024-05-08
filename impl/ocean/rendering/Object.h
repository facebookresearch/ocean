/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_OBJECT_H
#define META_OCEAN_RENDERING_OBJECT_H

#include "ocean/rendering/Rendering.h"

#include "ocean/base/Lock.h"
#include "ocean/base/ObjectRef.h"

#include <map>
#include <set>

namespace Ocean
{

namespace Rendering
{

// Forward declaration.
class Engine;
// Forward declaration.
class Object;

/**
 * Definition of a rendering object reference with an internal reference counter.
 * @see Object.
 * @ingroup rendering
 */
typedef Ocean::ObjectRef<Object> ObjectRef;

/**
 * Definition of a vector holding rendering object references.
 * @ingroup rendering
 */
typedef std::vector<ObjectRef> ObjectRefs;

/**
 * Definition of a vector holding rendering object references.
 * @ingroup rendering
 */
typedef std::set<ObjectRef> ObjectRefSet;

/**
 * This class is the base class for all rendering objects.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Object
{
	friend class Ocean::ObjectRef<Object>;

	public:

		/**
		 * Definition of different object type.
		 */
		enum ObjectType
		{
			/// Unknown type.
			TYPE_UNKNOWN,
			/// Absolute transform type.
			TYPE_ABSOLUTE_TRANSFORM,
			/// Attribute type.
			TYPE_ATTRIBUTE,
			/// AttributeSet type.
			TYPE_ATTRIBUTE_SET,
			/// Background type.
			TYPE_BACKGROUND,
			/// Billboard type.
			TYPE_BILLBOARD,
			/// BitmapFramebuffer type.
			TYPE_BITMAP_FRAMEBUFFER,
			/// BlendAttribute type.
			TYPE_BLEND_ATTRIBUTE,
			/// Box type.
			TYPE_BOX,
			/// Cone type.
			TYPE_CONE,
			/// Cylinder type.
			TYPE_CYLINDER,
			/// DepthAttribute type.
			TYPE_DEPTH_ATTRIBUTE,
			/// DirectionalLight type.
			TYPE_DIRECTIONAL_LIGHT,
			/// Framebuffer type.
			TYPE_FRAMEBUFFER,
			/// FrameTexture2D type.
			TYPE_FRAME_TEXTURE_2D,
			/// Geometry type.
			TYPE_GEOMETRY,
			/// Group type.
			TYPE_GROUP,
			/// IndependentPrimitive type.
			TYPE_INDEPENDENT_PRIMITIVE,
			/// LightSource type.
			TYPE_LIGHT_SOURCE,
			/// Lines type.
			TYPE_LINES,
			/// Line strips type.
			TYPE_LINE_STRIPS,
			/// LOD type.
			TYPE_LOD,
			/// Material type.
			TYPE_MATERIAL,
			/// Node type.
			TYPE_NODE,
			/// Object type.
			TYPE_OBJECT,
			/// ParallelView type.
			TYPE_PARALLEL_VIEW,
			/// PerspectiveView type.
			TYPE_PERSPECTIVE_VIEW,
			/// PhantomAttribute type.
			TYPE_PHANTOM_ATTRIBUTE,
			/// PointLight type.
			TYPE_POINT_LIGHT,
			/// Points type.
			TYPE_POINTS,
			/// Primitive type.
			TYPE_PRIMITIVE,
			/// PrimitiveAttribute type.
			TYPE_PRIMITIVE_ATTRIBUTE,
			/// QuadStrips type.
			TYPE_QUAD_STRIPS,
			/// Quads type.
			TYPE_QUADS,
			/// Renderable type.
			TYPE_RENDERABLE,
			/// Scene type.
			TYPE_SCENE,
			/// ShaderProgram type.
			TYPE_SHADER_PROGRAM,
			/// Shape type.
			TYPE_SHAPE,
			/// SkyBackground type.
			TYPE_SKY_BACKGROUND,
			/// Sphere type.
			TYPE_SPHERE,
			/// SpotLight type.
			TYPE_SPOT_LIGHT,
			/// StereoAttribute type.
			TYPE_STEREO_ATTRIBUTE,
			/// StereoView type.
			TYPE_STEREO_VIEW,
			/// StripPrimitive type.
			TYPE_STRIP_PRIMITIVE,
			/// Switch type.
			TYPE_SWITCH,
			/// Text type.
			TYPE_TEXT,
			/// Texture type.
			TYPE_TEXTURE,
			/// Texture2D type.
			TYPE_TEXTURE_2D,
			/// MediaTexture2D type.
			TYPE_MEDIA_TEXTURE_2D,
			/// TextureFramebuffer type.
			TYPE_TEXTURE_FRAMEBUFFER,
			/// Textures type.
			TYPE_TEXTURES,
			/// Transform type.
			TYPE_TRANSFORM,
			/// TriangleFans type.
			TYPE_TRIANGLE_FANS,
			/// Triangles type.
			TYPE_TRIANGLES,
			/// TriangleStrips type.
			TYPE_TRIANGLE_STRIPS,
			/// UndistortedBackground type.
			TYPE_UNDISTORTED_BACKGROUND,
			/// VertexSet type.
			TYPE_VERTEX_SET,
			/// View type.
			TYPE_VIEW,
			/// WindowFramebuffer type.
			TYPE_WINDOW_FRAMEBUFFER
		};

		/**
		 * Definition of a map holding object ids to reference counters.
		 */
		typedef std::unordered_map<ObjectId, unsigned int> ObjectIdMap;

	public:

		/**
		 * Returns the unique object id of this object.
		 * @return Unique object id
		 */
		inline ObjectId id() const;

		/**
		 * Returns the name of this object.
		 * @return Object name
		 */
		inline const std::string& name() const;

		/**
		 * Sets or changes the name of this object.
		 * @param name New object name
		 */
		virtual void setName(const std::string& name);

		/**
		 * Returns the name of the owner engine.
		 * @return Owner engine
		 */
		virtual const std::string& engineName() const = 0;

		/**
		 * Returns the type of this object.
		 * @return Object type
		 */
		virtual ObjectType type() const;

		/**
		 * Returns all parent objects.
		 * @return Vector holding all parent objects
		 */
		ObjectRefs parentObjects() const;

		/**
		 * Returns all parent nodes.
		 * @return Set holding all parent nodes
		 */
		ObjectRefSet parentNodes() const;

		/**
		 * Returns descriptive information about the object as string.
		 * The descriptive information can be used during debugging.
		 * @return The object's descriptive information, if any
		 */
		virtual std::string descriptiveInformation() const;

		/**
		 * Translates an object type to a readable string.
		 * @param objectType The object type to translate
		 * @return The resulting readable string of the object type
		 */
		static std::string translateObjectType(const ObjectType objectType);

	protected:

		/**
		 * Creates a new object.
		 */
		Object();

		/**
		 * Disabled copy constructor.
		 * @param object Object which would be copied
		 */
		Object(const Object& object) = delete;

		/**
		 * Destructs an object.
		 */
		virtual ~Object();

		/**
		 * Returns the render engine which is owner of this object.
		 * @return Rendering engine
		 */
		Engine& engine() const;

		/**
		 * Registers a parent node for this (child) node.
		 * @param parentId Id of the parent node to register
		 */
		inline void registerParent(const ObjectId parentId);

		/**
		 * Unregisters a parent node for this (child) node.
		 * @param parentId Id of the parent node to unregister
		 */
		inline void unregisterParent(const ObjectId parentId);

		/**
		 * Registers this object at a child as parent object.
		 * @param child New child object for this object
		 */
		void registerThisObjectAsParent(const ObjectRef& child);

		/**
		 * Unregisters this object from a child as parent.
		 * @param child Child object to unregister
		 */
		void unregisterThisObjectAsParent(const ObjectRef& child);

		/**
		 * Disabled copy operator.
		 * @param object Object which would be copied
		 * @return Reference to this object
		 */
		Object& operator=(const Object& object) = delete;

	protected:

		/// Lock for the object
		mutable Lock objectLock;

	private:

		/// Unique object id.
		ObjectId objectId;

		/// Object name.
		std::string objectName;

		/// Object ids of parent objects.
		ObjectIdMap objectParents;
};

inline ObjectId Object::id() const
{
	return objectId;
}

inline const std::string& Object::name() const
{
	return objectName;
}

inline void Object::registerParent(const ObjectId parentId)
{
	const ScopedLock scopedLock(objectLock);

	++objectParents.insert(std::make_pair(parentId, 0)).first->second;
}

inline void Object::unregisterParent(const ObjectId parentId)
{
	const ScopedLock scopedLock(objectLock);

	ObjectIdMap::iterator i = objectParents.find(parentId);
	ocean_assert(i != objectParents.end());

	ocean_assert(i->second != 0);

	if (--i->second == 0)
	{
		objectParents.erase(i);
	}
}

/**
 * Write the descriptive information of an object to a stream.
 * @param stream The stream to which the object's description will be written
 * @param object The object to be written to the stream
 * @ingroup rendering
 */
inline std::ostream& operator<<(std::ostream& stream, const Object& object)
{
	stream << object.descriptiveInformation();

	return stream;
}

/**
 * Write the descriptive information of an object to a message object.
 * @param messageObject The message object to which the object's description will be written
 * @param object The object to be written to the message object
 * @tparam tActive True, if the messenger object is active; False, if the messenger object is disabled
 * @ingroup rendering
 */
template <bool tActive>
inline MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const Object& object)
{
	return messageObject << object.descriptiveInformation();
}

/**
 * Write the descriptive information of an object to a message object.
 * @param messageObject The message object to which the object's description will be written
 * @param object The object to be written to the message object
 * @tparam tActive True, if the messenger object is active; False, if the messenger object is disabled
 * @ingroup rendering
 */
template <bool tActive>
inline MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const Object& object)
{
	return messageObject << object.descriptiveInformation();
}

}

}

#endif // META_OCEAN_RENDERING_OBJECT_H
