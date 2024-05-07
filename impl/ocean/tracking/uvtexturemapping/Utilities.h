// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/tracking/uvtexturemapping/MeshUVTextureMapping.h"

namespace Ocean
{

namespace Tracking
{

namespace UVTextureMapping
{

/**
 * Utility functions for creating texture mappings on the fly.
 * @ingroup trackinguvtexturemapping
 */
class OCEAN_TRACKING_UVTEXTUREMAPPING_EXPORT Utilities
{
	public:

		/**
		* Creates a MeshUVTextureMapping from a given cubemap (box) texture.
		* The output mesh is given in a right-hand coordinate system, centered in the center of the box.
		* (x axis: right. y axis: up. z axis: out of the box surface through its front face.)
		* The tracking texture is assumed to have a cubemap-type layout:
		* <pre>
		* ---------------------
		* |      |     |      |
		* |      |  H  |      |                     Y
		* |      |     |      |                     ^
		* ---------------------                     |
		* |      |     |      |                .____|_______.
		* |  L   |  T  |   R  |               /     |      /|
		* |      |     |      |              /      T     / |
		* ---------------------             .____________.  |
		* |      |     |      |             |            | R-----> X
		* |      |  F  |      |             |            |  |
		* |      |     |      |             |     F      |  /
		* |      |-----|      |             |    /       | /
		* |      |     |      |             |___/________|/
		* |      |  B  |      |                /
		* |      |     |      |               Z
		* ---------------------
		* </pre>
		* @param width The width of the box along the x axis
		* @param height The height of the box along the y axis
		* @param depth Length of the box along the z axis
		* @param imageWidth Width of the cubemap texture in pixels
		* @param imageHeight Height of the cubemap texture in pixels
		* @return A new mesh UV texture mapping with geometry defined for the box
		*/
		static MeshUVTextureMappingRef createCubemapUVTextureMapping(Scalar width, Scalar height, Scalar depth, Scalar imageWidth, Scalar imageHeight);
};

} // namespace UVTextureMapping

} // namespace Tracking

} // namespace Ocean
