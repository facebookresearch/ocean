/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_UTILITIES_H
#define META_OCEAN_RENDERING_UTILITIES_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Box.h"
#include "ocean/rendering/Cone.h"
#include "ocean/rendering/Cylinder.h"
#include "ocean/rendering/Engine.h"
#include "ocean/rendering/FrameTexture2D.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Lines.h"
#include "ocean/rendering/LineStrips.h"
#include "ocean/rendering/Material.h"
#include "ocean/rendering/MediaTexture2D.h"
#include "ocean/rendering/Points.h"
#include "ocean/rendering/Sphere.h"
#include "ocean/rendering/Text.h"
#include "ocean/rendering/Transform.h"
#include "ocean/rendering/TriangleFace.h"
#include "ocean/rendering/VertexSet.h"

#include "ocean/base/Frame.h"

#include "ocean/math/RGBAColor.h"

#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Rendering
{

/**
 * This class implements utility functions for the rendering library.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Utilities
{
	public:

		/**
		 * Returns a Transform object holding a visible box, no material is applied.
		 * @param engine Rendering engine to be used
		 * @param dimension The dimension of the box in x, y and z direction, with range [0, infinity)x[0, infinity)x[0, infinity)
		 * @param box Optional resulting box object which is attached with the geometry
		 * @param attributeSet Optional resulting AttributeSet object which is attached with the geometry
		 * @param geometry Optional resulting geometry object which is encapsulating the 3D object
		 */
		static TransformRef createBox(const EngineRef& engine, const Vector3& dimension, BoxRef* box = nullptr, AttributeSetRef* attributeSet = nullptr, GeometryRef* geometry = nullptr);

		/**
		 * Returns a Transform object holding a visible box, further a material is applied.
		 * @param engine Rendering engine to be used
		 * @param dimension The dimension of the box in x, y and z direction
		 * @param color Diffuse color to be used
		 * @param box Optional resulting box object which is attached with the geometry
		 * @param attributeSet Optional resulting AttributeSet object which is attached with the geometry
		 * @param material Optional resulting material object which is attached with the resulting box
		 * @param geometry Optional resulting geometry object which is encapsulating the 3D object
		 */
		static TransformRef createBox(const EngineRef& engine, const Vector3& dimension, const RGBAColor& color, BoxRef* box = nullptr, AttributeSetRef* attributeSet = nullptr, MaterialRef* material = nullptr, GeometryRef* geometry = nullptr);

		/**
		 * Returns a Transform object holding a visible box, further a texture is applied.
		 * @param engine Rendering engine to be used
		 * @param dimension The dimension of the box in x, y and z direction
		 * @param textureMedium The frame medium which is used for the texture
		 * @param texture Optional resulting texture object which is attached with the resulting box
		 * @param createMipmaps True, to create a texture with mipmaps; False, to create a texture without mipmaps
		 * @param attributeSet Optional resulting AttributeSet object which is attached with the geometry
		 * @param geometry Optional resulting geometry object which is encapsulating the 3D object
		 * @param color Optional color to be used, nullptr to create a box without material
		 */
		static TransformRef createBox(const EngineRef& engine, const Vector3& dimension, const Media::FrameMediumRef textureMedium, MediaTexture2DRef* texture = nullptr, const bool createMipmaps = true, AttributeSetRef* attributeSet = nullptr, GeometryRef* geometry = nullptr, const RGBAColor* color = nullptr);

		/**
		 * Returns a Transform object holding a visible box, further a texture is applied.
		 * @param engine Rendering engine to be used
		 * @param dimension The dimension of the box in x, y and z direction
		 * @param frame The frame which is used for the texture, must own the image memory, must be valid
		 * @param texture Optional resulting texture object which is attached with the resulting box
		 * @param createMipmaps True, to create a texture with mipmaps; False, to create a texture without mipmaps
		 * @param attributeSet Optional resulting AttributeSet object which is attached with the geometry
		 * @param geometry Optional resulting geometry object which is encapsulating the 3D object
		 * @param color Optional color to be used, nullptr to create a box without material
		 */
		static TransformRef createBox(const EngineRef& engine, const Vector3& dimension, Frame&& frame, FrameTexture2DRef* texture = nullptr, const bool createMipmaps = true, AttributeSetRef* attributeSet = nullptr, GeometryRef* geometry = nullptr, const RGBAColor* color = nullptr);

		/**
		 * Returns a Transform object holding a visible sphere, no material is applied.
		 * @param engine Rendering engine to be used
		 * @param radius The radius of the sphere, with range [0, infinity)
		 * @param sphere Optional resulting sphere object which is attached with the geometry
		 * @param attributeSet Optional resulting AttributeSet object which is attached with the geometry
		 * @param geometry Optional resulting geometry object which is encapsulating the 3D object
		 */
		static TransformRef createSphere(const EngineRef& engine, const Scalar radius, SphereRef* sphere = nullptr, AttributeSetRef* attributeSet = nullptr, GeometryRef* geometry = nullptr);

		/**
		 * Returns a Transform object holding a visible sphere, further a material is applied.
		 * @param engine Rendering engine to be used
		 * @param radius The radius of the sphere, with range [0, infinity)
		 * @param color Diffuse color to be used
		 * @param sphere Optional resulting sphere object which is attached with the geometry
		 * @param attributeSet Optional resulting AttributeSet object which is attached with the geometry
		 * @param material Optional resulting material object which is attached with the geometry
		 * @param geometry Optional resulting geometry object which is encapsulating the 3D object
		 */
		static TransformRef createSphere(const EngineRef& engine, const Scalar radius, const RGBAColor& color, SphereRef* sphere = nullptr, AttributeSetRef* attributeSet = nullptr, MaterialRef* material = nullptr, GeometryRef* geometry = nullptr);

		/**
		 * Returns a Transform object holding a visible sphere, further a texture is applied.
		 * @param engine Rendering engine to be used
		 * @param radius The radius of the sphere, with range [0, infinity)
		 * @param frame The frame which is used for the texture, must own the image memory, must be valid
		 * @param texture Optional resulting texture object which is attached with the resulting sphere
		 * @param createMipmaps True, to create a texture with mipmaps; False, to create a texture without mipmaps
		 * @param attributeSet Optional resulting AttributeSet object which is attached with the geometry
		 * @param geometry Optional resulting geometry object which is encapsulating the 3D object
		 * @param color Optional color to be used, nullptr to create a sphere without material
		 */
		static TransformRef createSphere(const EngineRef& engine, const Scalar radius, Frame&& frame, FrameTexture2DRef* texture = nullptr, const bool createMipmaps = true, AttributeSetRef* attributeSet = nullptr, GeometryRef* geometry = nullptr, const RGBAColor* color = nullptr);

		/**
		 * Returns a Transform object holding a visible cylinder, further a material is applied.
		 * @param engine Rendering engine to be used
		 * @param radius The radius of the cylinder, with range (0, infinity)
		 * @param height The height of the cylinder, with range (0, infinity)
		 * @param color Diffuse color to be used
		 * @param cylinder Optional resulting cylinder object which is attached with the geometry
		 * @param attributeSet Optional resulting AttributeSet object which is attached with the geometry
		 * @param material Optional resulting material object which is attached with the geometry
		 * @param geometry Optional resulting geometry object which is encapsulating the 3D object
		 */
		static TransformRef createCylinder(const EngineRef& engine, const Scalar radius, const Scalar height, const RGBAColor& color, CylinderRef* cylinder = nullptr, AttributeSetRef* attributeSet = nullptr, MaterialRef* material = nullptr, GeometryRef* geometry = nullptr);

		/**
		 * Returns a Transform object holding a visible cone, further a material is applied.
		 * @param engine Rendering engine to be used
		 * @param radius The radius of the cone, with range (0, infinity)
		 * @param height The height of the cone, with range (0, infinity)
		 * @param color Diffuse color to be used
		 * @param cone Optional resulting cone object which is attached with the geometry
		 * @param attributeSet Optional resulting AttributeSet object which is attached with the geometry
		 * @param material Optional resulting material object which is attached with the geometry
		 * @param geometry Optional resulting geometry object which is encapsulating the 3D object
		 */
		static TransformRef createCone(const EngineRef& engine, const Scalar radius, const Scalar height, const RGBAColor& color, ConeRef* cone = nullptr, AttributeSetRef* attributeSet = nullptr, MaterialRef* material = nullptr, GeometryRef* geometry = nullptr);

		/**
		 * Returns a Transform object holding a visible coordinate system rendered as cylinders with cones.
		 * @param engine Rendering engine to be used
		 * @param length The length of the arrow, with range (0, infinity)
		 * @param topLength Length of the arrow top, with range (0, infinity)
		 * @param radius of the arrow, with range (0, infinity)
		 * @return Transform node
		 */
		static TransformRef createCoordinateSystem(const EngineRef& engine, const Scalar length, const Scalar topLength, const Scalar radius);

		/**
		 * Returns a Transform object holding several individual visible coordinate system rendered as thin lines.
		 * @param engine Rendering engine to be used
		 * @param world_T_coordinateSystems The individual coordinate systems, at least one
		 * @param length The length of each axis, with range (0, infinity)
		 * @param vertexSet Optional result vertex set object which holds the vertices of the 3D object, six vertices for each coordinate system, with order (origin, x-axis, origin, y-axis, origin, z-axis)
		 * @return The Transform node holding the coordinate system
		 */
		static TransformRef createCoordinateSystems(const Engine& engine, const HomogenousMatrices4& world_T_coordinateSystems, const Scalar length, VertexSetRef* vertexSet = nullptr);

		/**
		 * Returns an arrow with specified length and radius.
		 * @param engine Rendering engine to be used
		 * @param length The length of the arrow, with range [0, infinity)
		 * @param topLength Length of the arrow top, with range [0, infinity)
		 * @param radius of the arrow, with range [0, infinity)
		 * @param color The color to be applied, must be valid
		 * @return Transform node holding a visible arrow
		 */
		static TransformRef createArrow(const EngineRef& engine, const Scalar length, const Scalar topLength, const Scalar radius, const RGBAColor& color);

		/**
		 * Returns a mesh by a given set of vertices and their corresponding triangulation.
		 * @param engine Rendering engine to be used
		 * @param vertices The vertices defining the mesh
		 * @param triangleFaces Triangulation defining the topology
		 * @param color The color to be applied to the entire mesh, invalid to create an unlit mesh
		 * @param normals Optional normals of each vertex
		 * @param textureCoordinates Optional texture coordinates, one for each given vertex
		 * @param textureFrame Optional frame to be used as texture, most be owner of the memory if provided, invalid to use no texture or to use the textureMedium instead
		 * @param textureMedium Optional texture medium, invalid to use no texture or to use the textureFrame instead
		 * @param attributeSet Optional resulting AttributeSet object which is attached with the geometry
		 * @param geometry Optional resulting geometry object which is encapsulating the 3D object
		 * @param vertexSet Optional result vertex set object which holds the vertices of the 3D object
		 * @param texture Optional resulting texture object
		 * @return Transform node holding the mesh object
		 */
		static TransformRef createMesh(const EngineRef& engine, const Vectors3& vertices, const TriangleFaces& triangleFaces, const RGBAColor& color, const Normals& normals = Normals(), const TextureCoordinates& textureCoordinates = TextureCoordinates(), Frame&& textureFrame = Frame(), const Media::FrameMediumRef& textureMedium = Media::FrameMediumRef(), AttributeSetRef* attributeSet = nullptr, GeometryRef* geometry = nullptr, VertexSetRef* vertexSet = nullptr, Texture2DRef* texture = nullptr);

		/**
		 * Returns a textured plane with z-axis a normal.
		 * @param engine Rendering engine to be used, must be valid
		 * @param width The width of the plane in object domain (the size of the plane in x-direction), with range (0, infinity)
		 * @param height The height of the plane in object domain (the size of the plane, in y-direction), with range (0, infinity)
		 * @param textureMedium The frame medium which is used for the texture
		 * @param texture Optional resulting texture object which is attached with the resulting box
		 * @param createMipmaps True, to create a texture with mipmaps; False, to create a texture without mipmaps
		 * @param attributeSet Optional resulting AttributeSet object which is attached with the geometry
		 * @param geometry Optional resulting geometry object which is encapsulating the 3D object
		 * @return Transform node holding the plane
		 */
		static TransformRef createTexturedPlane(const EngineRef& engine, const Scalar width, const Scalar height, const Media::FrameMediumRef textureMedium, MediaTexture2DRef* texture = nullptr, const bool createMipmaps = true, AttributeSetRef* attributeSet = nullptr, GeometryRef* geometry = nullptr);

		/**
		 * Returns a planar text within the x,y plane.
		 * @param engine The rendering engine to be used
		 * @param textString The actual text to show
		 * @param foregroundColor The foreground color to be used
		 * @param backgroundColor The background color to be used, a transparent color to create a text without background
		 * @param shaded True, to create a text with shading/lighting; False, to create a text constant color
		 * @param fixedWidth The optional fixed width, with range [0, infinity)
		 * @param fixedHeight The optional fixed height, with range [0, infinity)
		 * @param fixedLineHeight The optional fixed line height, with range [0, infinity)
		 * @param alignmentMode The text's alignment mode
		 * @param horizontalAnchor The text's horizontal anchor
		 * @param verticalAnchor The text's vertical anchor
		 * @param fontFamily The font family to be used, empty to use a default font family
		 * @param styleName The font style to be used, empty to use the default font style
		 * @param text Optional resulting text node which will be part of the resulting Transform node, nullptr of not of interest
		 * @param foregroundMaterial Optional resulting foreground material of the new text, nullptr if not of interest
		 * @param backgroundMaterial Optional resulting background material of the new text, nullptr if not of interest
		 */
		static TransformRef createText(const Engine& engine, const std::string& textString, const RGBAColor& foregroundColor, const RGBAColor& backgroundColor, const bool shaded, const Scalar fixedWidth, const Scalar fixedHeight, const Scalar fixedLineHeight, const Text::AlignmentMode alignmentMode = Text::AM_LEFT, const Text::HorizontalAnchor horizontalAnchor = Text::HA_LEFT, const Text::VerticalAnchor verticalAnchor = Text::VA_TOP, const std::string& fontFamily = std::string(), const std::string& styleName = std::string(), TextRef* text = nullptr, MaterialRef* foregroundMaterial = nullptr, MaterialRef* backgroundMaterial = nullptr);

		/**
		 * Returns a Transform holding a geometry with points.
		 * @param engine The rendering engine to be used
		 * @param vertices The vertices of all points to be rendered, at least one
		 * @param emissiveColor The emissiveColor of all vertices in case 'colorsPerVertex' is empty, can be transparent
		 * @param pointSize The size of the points, with range [1, infinity)
		 * @param colorsPerVertex The optional colors of the individual vertices, one for each vertex or empty to use the 'emissiveColor'
		 * @param material Optional resulting Material node; will be invalid if colorPerVertex is not empty
		 * @param attributeSet Optional resulting AttributeSet node
		 * @param points Optional resulting Points node
		 * @param vertexSet Optional resulting VertexSet node
		 * @return The resulting transform node holding the geometry node
		 */
		static TransformRef createPoints(const Engine& engine, const Vectors3& vertices, const RGBAColor& emissiveColor, const Scalar pointSize = Scalar(5), const RGBAColors& colorsPerVertex = RGBAColors(), MaterialRef* material = nullptr, AttributeSetRef* attributeSet = nullptr, PointsRef* points = nullptr, VertexSetRef* vertexSet = nullptr);

		/**
		 * Returns a Transform holding a geometry with lines.
		 * @param engine The rendering engine to be used
		 * @param vertices The vertices of all points to be rendered, at least one
		 * @param lineIndexGroups The group of vertex indices defining several individual lines, at least one
		 * @param emissiveColor The emissiveColor of all vertices in case 'colorsPerVertex' is empty, can be transparent
		 * @param colorsPerVertex The optional colors of the individual vertices, one for each vertex or empty to use the 'emissiveColor'
		 * @param material Optional resulting Material node; will be invalid if colorPerVertex is not empty
		 * @param attributeSet Optional resulting AttributeSet node
		 * @param lineStrips Optional resulting LineStrips node
		 * @param vertexSet Optional resulting VertexSet node
		 * @return The resulting transform node holding the geometry node
		 */
		static TransformRef createLines(const Engine& engine, const Vectors3& vertices, const VertexIndexGroups& lineIndexGroups, const RGBAColor& emissiveColor, const RGBAColors& colorsPerVertex = RGBAColors(), MaterialRef* material = nullptr, AttributeSetRef* attributeSet = nullptr, LineStripsRef* lineStrips = nullptr, VertexSetRef* vertexSet = nullptr);

		/**
		 * Returns a Transform holding a geometry with lines (a wireframe of a mesh).
		 * @param engine The rendering engine to be used
		 * @param vertices The vertices of all points to be rendered, at least one
		 * @param triangleFaces The indices of triangle vertices for which lines will be created, at least one
		 * @param emissiveColor The emissiveColor of all vertices in case 'colorsPerVertex' is empty, can be transparent
		 * @param colorsPerVertex The optional colors of the individual vertices, one for each vertex or empty to use the 'emissiveColor'
		 * @param material Optional resulting Material node; will be invalid if colorPerVertex is not empty
		 * @param attributeSet Optional resulting AttributeSet node
		 * @param lines Optional resulting Lines node
		 * @param vertexSet Optional resulting VertexSet node
		 * @return The resulting transform node holding the geometry node
		 */
		static TransformRef createLines(const Engine& engine, const Vectors3& vertices, const TriangleFaces& triangleFaces, const RGBAColor& emissiveColor, const RGBAColors& colorsPerVertex = RGBAColors(), MaterialRef* material = nullptr, AttributeSetRef* attributeSet = nullptr, LinesRef* lines = nullptr, VertexSetRef* vertexSet = nullptr);

		/**
		 * Returns a Transform holding a geometry with lines (normals for given points).
		 * @param engine Rendering engine to be used
		 * @param points The starting points where the normals will start, at least one
		 * @param normals The normal vectors, one for each point
		 * @param size The number of points (and normals), with range [1, infinity)
		 * @param scale The scale which will be applied to each normal (allowing to control the length of the normals), with range (-infinity, infinity)
		 * @param emissiveColor The emissiveColor of all normals, can be transparent
		 * @param material Optional resulting Material node
		 * @param vertexSet Optional result vertex set object which holds the vertices of the 3D normals, two vertices for each normal
		 * @return The Transform node holding the normals
		 */
		static TransformRef createLines(const Engine& engine, const Vector3* points, const Vector3* normals, const size_t size, const Scalar scale, const RGBAColor& emissiveColor, MaterialRef* material = nullptr, VertexSetRef* vertexSet = nullptr);

		/**
		 * Returns the first attribute set object that is located in a given node tree.
		 * @param node The node defining the root of the tree in that the attribute set is located
		 * @return First attribute set object that can be found
		 */
		static AttributeSetRef findAttributeSet(const NodeRef& node);

	protected:

		/**
		 * Returns a 64-bit index based on two 32-bit indices.
		 * @param indexA The first 32-bit index
		 * @param indexB The second 32-bit index
		 * @return The resulting 64-bit index
		 */
		static inline uint64_t makeSortedIndex64(const uint32_t indexA, const uint32_t indexB);
};

inline uint64_t Utilities::makeSortedIndex64(const uint32_t indexA, const uint32_t indexB)
{
	if (indexA < indexB)
	{
		return uint64_t(indexA) | (uint64_t(indexB) << 32ull);
	}

	return uint64_t(indexB) | (uint64_t(indexA) << 32ull);
}

}

}

#endif // META_OCEAN_RENDERING_UTILITIES_H
