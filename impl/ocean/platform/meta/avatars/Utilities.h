// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_AVATARS_UTILITIES_H
#define META_OCEAN_PLATFORM_META_AVATARS_UTILITIES_H

#include "ocean/platform/meta/avatars/Avatars.h"

#include <ovrAvatar2/Types.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

/**
 * This class implements utility functions for the Avatars library.
 * @ingroup platformmetaavatars
 */
class Utilities
{
	public:

		/**
		 * Hash function for image ids.
		 * @param imageId The image id to hash
		 */
		inline size_t operator()(const ovrAvatar2ImageId& imageId) const;

		/**
		 * Hash function for primitive ids.
		 * @param primitiveId The primitive id to hash
		 */
		inline size_t operator()(const ovrAvatar2PrimitiveId& primitiveId) const;

		/**
		 * Hash function for vertex buffer ids.
		 * @param vertexBufferId The vertex buffer id to hash
		 */
		inline size_t operator()(const ovrAvatar2VertexBufferId& vertexBufferId) const;

		/**
		 * Hash function for node ids.
		 * @param nodeId The node id to hash
		 */
		inline size_t operator()(const ovrAvatar2NodeId& nodeId) const;
};

inline size_t Utilities::operator()(const ovrAvatar2ImageId& imageId) const
{
	// workaround for older compilers
	static_assert(sizeof(ovrAvatar2ImageId) == sizeof(int), "Invalid data type!");

	return std::hash<int>{}(int(imageId));
}

inline size_t Utilities::operator()(const ovrAvatar2PrimitiveId& primitiveId) const
{
	// workaround for older compilers
	static_assert(sizeof(ovrAvatar2PrimitiveId) == sizeof(int), "Invalid data type!");

	return std::hash<int>{}(int(primitiveId));
}

inline size_t Utilities::operator()(const ovrAvatar2VertexBufferId& vertexBufferId) const
{
	// workaround for older compilers
	static_assert(sizeof(ovrAvatar2VertexBufferId) == sizeof(int), "Invalid data type!");

	return std::hash<int>{}(int(vertexBufferId));
}

inline size_t Utilities::operator()(const ovrAvatar2NodeId& nodeId) const
{
	// workaround for older compilers
	static_assert(sizeof(ovrAvatar2NodeId) == sizeof(int), "Invalid data type!");

	return std::hash<int>{}(int(nodeId));
}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_AVATARS_UTILITIES_H
