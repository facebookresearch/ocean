// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_RENDERING_GI_ATTRIBUTE_SET_H
#define META_OCEAN_RENDERING_GI_ATTRIBUTE_SET_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GILightSource.h"
#include "ocean/rendering/globalillumination/GIObject.h"
#include "ocean/rendering/globalillumination/GITextures.h"

#include "ocean/rendering/AttributeSet.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a Global Illumination attribute set object.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIAttributeSet :
	virtual public GIObject,
	virtual public AttributeSet
{
	friend class GIFactory;

	public:

		/**
		 * Adds a new attribute to this attribute set.
		 * @see AttributeSet::addAttribute().
		 */
		void addAttribute(const AttributeRef& attribute) override;

		/**
		 * Removes a registered attribute from this attribute ses.
		 * @see AttributeSet::removeAttribute().
		 */
		void removeAttribute(const AttributeRef& attribute) override;

	protected:

		/**
		 * Creates a new Global Illumination attribute set object.
		 */
		GIAttributeSet();

		/**
		 * Destructs a Global Illumination attribute set object.
		 */
		~GIAttributeSet() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_ATTRIBUTE_SET_H
