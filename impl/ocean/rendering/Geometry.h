/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GEOMETRY_H
#define META_OCEAN_RENDERING_GEOMETRY_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Node.h"

#include <map>

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Geometry;

/**
 * Definition of a smart object reference holding a geometry.
 * @see SmartObjectRef, Geometry.
 * @ingroup rendering
 */
typedef SmartObjectRef<Geometry> GeometryRef;

/**
 * This class is the base class for all rendering geometry nodes.<br>
 * Each geometry node can holds several different renderable objects associated with different appearance attributes.<br>
 * All renderable objects are defined in the same coordinate system.<br>
 * A group node can be used to group several independent geometry nodes.<br>
 * Geometry nodes with different coordinate systems can be defined by the use of transform nodes as father nodes.<br>
 * @see Renderable, AttributeSet, Group, Transform
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Geometry : virtual public Node
{
	protected:

		/**
		 * Definition of a map mapping renderable objects to attributes.
		 */
		typedef std::map<RenderableRef, AttributeSetRef> Renderables;

	public:

		/**
		 * Adds a new renderable object to this geometry.
		 * @see Renderable, AttributeSet
		 * @param renderable New renderable object
		 * @param attributes Attributes associated with the renderable object
		 */
		virtual void addRenderable(const RenderableRef& renderable, const AttributeSetRef& attributes);

		/**
		 * Returns the number of renderable objects connected to this geometry.
		 * @return Number of renderable objects
		 */
		virtual unsigned int numberRenderables() const;

		/**
		 * Returns a specified renderable holds by this geometry object.
		 * @param index Index of the renderable object to return
		 * @return Renderable object
		 */
		virtual RenderableRef renderable(const unsigned int index) const;

		/**
		 * Returns an attribute set connected with a renderable object specified by the index.
		 * @param index Index of the renderable object to return the attribute set for
		 * @return Attribute set object
		 */
		virtual AttributeSetRef attributeSet(const unsigned int index) const;

		/**
		 * Removes a renderable object connected to this geometry.
		 * @param renderable Renderable object to remove
		 */
		virtual void removeRenderable(const RenderableRef& renderable);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new geometry object.
		 */
		Geometry();

		/**
		 * Destructs a geometry object.
		 */
		~Geometry() override;

	protected:

		/// Renderables object connected to this geometry.
		Renderables geometryRenderables;
};

}

}

#endif // META_OCEAN_RENDERING_GEOMETRY_H
