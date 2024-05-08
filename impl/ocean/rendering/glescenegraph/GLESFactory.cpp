/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESFactory.h"
#include "ocean/rendering/glescenegraph/GLESAbsoluteTransform.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"
#include "ocean/rendering/glescenegraph/GLESBlendAttribute.h"
#include "ocean/rendering/glescenegraph/GLESBox.h"
#include "ocean/rendering/glescenegraph/GLESCone.h"
#include "ocean/rendering/glescenegraph/GLESCylinder.h"
#include "ocean/rendering/glescenegraph/GLESDepthAttribute.h"
#include "ocean/rendering/glescenegraph/GLESDirectionalLight.h"
#include "ocean/rendering/glescenegraph/GLESFrameTexture2D.h"
#include "ocean/rendering/glescenegraph/GLESGeometry.h"
#include "ocean/rendering/glescenegraph/GLESGroup.h"
#include "ocean/rendering/glescenegraph/GLESLines.h"
#include "ocean/rendering/glescenegraph/GLESLineStrips.h"
#include "ocean/rendering/glescenegraph/GLESLOD.h"
#include "ocean/rendering/glescenegraph/GLESMaterial.h"
#include "ocean/rendering/glescenegraph/GLESMediaTexture2D.h"
#include "ocean/rendering/glescenegraph/GLESPerspectiveView.h"
#include "ocean/rendering/glescenegraph/GLESPhantomAttribute.h"
#include "ocean/rendering/glescenegraph/GLESPointLight.h"
#include "ocean/rendering/glescenegraph/GLESPoints.h"
#include "ocean/rendering/glescenegraph/GLESPrimitiveAttribute.h"
#include "ocean/rendering/glescenegraph/GLESScene.h"
#include "ocean/rendering/glescenegraph/GLESSphere.h"
#include "ocean/rendering/glescenegraph/GLESSpotLight.h"
#include "ocean/rendering/glescenegraph/GLESStereoAttribute.h"
#include "ocean/rendering/glescenegraph/GLESStereoView.h"
#include "ocean/rendering/glescenegraph/GLESSwitch.h"
#include "ocean/rendering/glescenegraph/GLESText.h"
#include "ocean/rendering/glescenegraph/GLESTextureFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESTextures.h"
#include "ocean/rendering/glescenegraph/GLESTransform.h"
#include "ocean/rendering/glescenegraph/GLESTriangleFans.h"
#include "ocean/rendering/glescenegraph/GLESTriangles.h"
#include "ocean/rendering/glescenegraph/GLESTriangleStrips.h"
#include "ocean/rendering/glescenegraph/GLESUndistortedBackground.h"
#include "ocean/rendering/glescenegraph/GLESVertexSet.h"
#include "ocean/rendering/glescenegraph/GLESView.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

ObjectRef GLESFactory::createAbsoluteTransform() const
{
	return createReference(new GLESAbsoluteTransform());
}

ObjectRef GLESFactory::createAttributeSet() const
{
	return createReference(new GLESAttributeSet());
}

NodeRef GLESFactory::createBillboard() const
{
	throw NotSupportedException();
}

ObjectRef GLESFactory::createBlendAttribute() const
{
	return createReference(new GLESBlendAttribute());
}

ObjectRef GLESFactory::createBox() const
{
	return createReference(new GLESBox());
}

ObjectRef GLESFactory::createCone() const
{
	return createReference(new GLESCone());
}

ObjectRef GLESFactory::createCylinder() const
{
	return createReference(new GLESCylinder());
}

ObjectRef GLESFactory::createDepthAttribute() const
{
	return createReference(new GLESDepthAttribute());
}

ObjectRef GLESFactory::createDirectionalLight() const
{
	return createReference(new GLESDirectionalLight());
}

ObjectRef GLESFactory::createFrameTexture2D() const
{
	return createReference(new GLESFrameTexture2D());
}

NodeRef GLESFactory::createGeometry() const
{
	return createReference(new GLESGeometry());
}

NodeRef GLESFactory::createGroup() const
{
	return createReference(new GLESGroup());
}

ObjectRef GLESFactory::createLines() const
{
	return createReference(new GLESLines());
}

ObjectRef GLESFactory::createLineStrips() const
{
	return createReference(new GLESLineStrips());
}

NodeRef GLESFactory::createLOD() const
{
	return createReference(new GLESLOD());
}

ObjectRef GLESFactory::createMaterial() const
{
	return createReference(new GLESMaterial());
}

ObjectRef GLESFactory::createMediaTexture2D() const
{
	return createReference(new GLESMediaTexture2D());
}

ObjectRef GLESFactory::createPerspectiveView() const
{
	return createReference(new GLESPerspectiveView());
}

ObjectRef GLESFactory::createPhantomAttribute() const
{
	return createReference(new GLESPhantomAttribute());
}

ObjectRef GLESFactory::createPointLight() const
{
	return createReference(new GLESPointLight());
}

ObjectRef GLESFactory::createPoints() const
{
	return createReference(new GLESPoints());
}

ObjectRef GLESFactory::createPrimitiveAttribute() const
{
	return createReference(new GLESPrimitiveAttribute());
}

NodeRef GLESFactory::createScene() const
{
	return createReference(new GLESScene());
}

ObjectRef GLESFactory::createShaderProgram() const
{
	return createReference(new GLESShaderProgram());
}

ObjectRef GLESFactory::createSphere() const
{
	return createReference(new GLESSphere());
}

NodeRef GLESFactory::createSwitch() const
{
	return createReference(new GLESSwitch());
}

ObjectRef GLESFactory::createStereoAttribute() const
{
	return createReference(new GLESStereoAttribute());
}

ObjectRef GLESFactory::createSpotLight() const
{
	return createReference(new GLESSpotLight());
}

ObjectRef GLESFactory::createStereoView() const
{
	return createReference(new GLESStereoView());
}

ObjectRef GLESFactory::createText() const
{
	return createReference(new GLESText());
}

ObjectRef GLESFactory::createTextureFramebuffer() const
{
	return createReference(new GLESTextureFramebuffer());
}

ObjectRef GLESFactory::createTextures() const
{
	return createReference(new GLESTextures());
}

NodeRef GLESFactory::createTransform() const
{
	return createReference(new GLESTransform());
}

ObjectRef GLESFactory::createTriangleFans() const
{
	return createReference(new GLESTriangleFans());
}

ObjectRef GLESFactory::createTriangles() const
{
	return createReference(new GLESTriangles());
}

ObjectRef GLESFactory::createTriangleStrips() const
{
	return createReference(new GLESTriangleStrips());
}

NodeRef GLESFactory::createUndistortedBackground() const
{
	return createReference(new GLESUndistortedBackground());
}

ObjectRef GLESFactory::createVertexSet() const
{
	return createReference(new GLESVertexSet());
}

}

}

}
