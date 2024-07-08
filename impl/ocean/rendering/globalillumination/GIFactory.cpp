/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIFactory.h"
//#include "ocean/rendering/globalillumination/GIAbsoluteTransform.h"
#include "ocean/rendering/globalillumination/GIAttributeSet.h"
//#include "ocean/rendering/globalillumination/GIBillboard.h"
//#include "ocean/rendering/globalillumination/GIBlendAttribute.h"
#include "ocean/rendering/globalillumination/GIBox.h"
#include "ocean/rendering/globalillumination/GICone.h"
#include "ocean/rendering/globalillumination/GICylinder.h"
//#include "ocean/rendering/globalillumination/GIDepthAttribute.h"
//#include "ocean/rendering/globalillumination/GIDirectionalLight.h"
#include "ocean/rendering/globalillumination/GIGeometry.h"
#include "ocean/rendering/globalillumination/GIGroup.h"
//#include "ocean/rendering/globalillumination/GILOD.h"
#include "ocean/rendering/globalillumination/GIMaterial.h"
#include "ocean/rendering/globalillumination/GIMediaTexture2D.h"
//#include "ocean/rendering/globalillumination/GIParallelView.h"
#include "ocean/rendering/globalillumination/GIPerspectiveView.h"
//#include "ocean/rendering/globalillumination/GIPhantomAttribute.h"
#include "ocean/rendering/globalillumination/GIPointLight.h"
//#include "ocean/rendering/globalillumination/GIPoints.h"
//#include "ocean/rendering/globalillumination/GIPrimitiveAttribute.h"
//#include "ocean/rendering/globalillumination/GIQuads.h"
//#include "ocean/rendering/globalillumination/GIQuadStrips.h"
#include "ocean/rendering/globalillumination/GIScene.h"
//#include "ocean/rendering/globalillumination/GIShaderProgramCg.h"
//#include "ocean/rendering/globalillumination/GISkyBackground.h"
#include "ocean/rendering/globalillumination/GISphere.h"
//#include "ocean/rendering/globalillumination/GISwitch.h"
#include "ocean/rendering/globalillumination/GISpotLight.h"
//#include "ocean/rendering/globalillumination/GIStereoView.h"
#include "ocean/rendering/globalillumination/GITextures.h"
#include "ocean/rendering/globalillumination/GITransform.h"
//#include "ocean/rendering/globalillumination/GITriangleFans.h"
#include "ocean/rendering/globalillumination/GITriangles.h"
//#include "ocean/rendering/globalillumination/GITriangleStrips.h"
//#include "ocean/rendering/globalillumination/GIUndistortedBackground.h"
#include "ocean/rendering/globalillumination/GIVertexSet.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

ObjectRef GIFactory::createAbsoluteTransform() const
{
	return ObjectRef();
	//return createReference(new GIAbsoluteTransform());
}

ObjectRef GIFactory::createAttributeSet() const
{
	return createReference(new GIAttributeSet());
}

NodeRef GIFactory::createBillboard() const
{
	return ObjectRef();
	//return createReference(new GIBillboard());
}

ObjectRef GIFactory::createBlendAttribute() const
{
	return ObjectRef();
	//return createReference(new GIBlendAttribute());
}

ObjectRef GIFactory::createBox() const
{
	return createReference(new GIBox());
}

ObjectRef GIFactory::createCone() const
{
	return createReference(new GICone());
}

ObjectRef GIFactory::createCylinder() const
{
	return createReference(new GICylinder());
}

ObjectRef GIFactory::createDepthAttribute() const
{
	return ObjectRef();
	//return createReference(new GIDepthAttribute());
}

ObjectRef GIFactory::createDirectionalLight() const
{
	return ObjectRef();
	//return createReference(new GIDirectionalLight());
}

NodeRef GIFactory::createGeometry() const
{
	return createReference(new GIGeometry());
}

NodeRef GIFactory::createGroup() const
{
	return createReference(new GIGroup());
}

NodeRef GIFactory::createLOD() const
{
	return ObjectRef();
	//return createReference(new GILOD());
}

ObjectRef GIFactory::createMaterial() const
{
	return createReference(new GIMaterial());
}

ObjectRef GIFactory::createMediaTexture2D() const
{
	return createReference(new GIMediaTexture2D());
}

ObjectRef GIFactory::createParallelView() const
{
	return ObjectRef();
	//return createReference(new GIParallelView());
}

ObjectRef GIFactory::createPerspectiveView() const
{
	return createReference(new GIPerspectiveView());
}

ObjectRef GIFactory::createPhantomAttribute() const
{
	return ObjectRef();
	//return createReference(new GIPhantomAttribute());
}

ObjectRef GIFactory::createPointLight() const
{
	return createReference(new GIPointLight());
}

ObjectRef GIFactory::createPoints() const
{
	return ObjectRef();
	//return createReference(new GIPoints());
}

ObjectRef GIFactory::createPrimitiveAttribute() const
{
	return ObjectRef();
	//return createReference(new GIPrimitiveAttribute());
}

ObjectRef GIFactory::createQuads() const
{
	return ObjectRef();
	//return createReference(new GIQuads());
}

ObjectRef GIFactory::createQuadStrips() const
{
	return ObjectRef();
	//return createReference(new GIQuadStrips());
}

NodeRef GIFactory::createScene() const
{
	return createReference(new GIScene());
}

ObjectRef GIFactory::createShaderProgram() const
{
	return ObjectRef();
}

NodeRef GIFactory::createSkyBackground() const
{
	return ObjectRef();
	//return createReference(new GISkyBackground());
}

ObjectRef GIFactory::createSphere() const
{
	return createReference(new GISphere());
}

NodeRef GIFactory::createSwitch() const
{
	return ObjectRef();
	//return createReference(new GISwitch());
}

ObjectRef GIFactory::createSpotLight() const
{
	return createReference(new GISpotLight());
}

ObjectRef GIFactory::createStereoView() const
{
	return ObjectRef();
	//return createReference(new GIStereoView());
}

ObjectRef GIFactory::createTextures() const
{
	return createReference(new GITextures());
}

NodeRef GIFactory::createTransform() const
{
	return createReference(new GITransform());
}

ObjectRef GIFactory::createTriangleFans() const
{
	return ObjectRef();
	//return createReference(new GITriangleFans());
}

ObjectRef GIFactory::createTriangles() const
{
	return createReference(new GITriangles());
}

ObjectRef GIFactory::createTriangleStrips() const
{
	return ObjectRef();
	//return createReference(new GITriangleStrips());
}

NodeRef GIFactory::createUndistortedBackground() const
{
	return ObjectRef();
	//return createReference(new GIUndistortedBackground());
}

ObjectRef GIFactory::createVertexSet() const
{
	return createReference(new GIVertexSet());
}

}

}

}
