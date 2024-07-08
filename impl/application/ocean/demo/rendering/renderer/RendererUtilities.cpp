/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/renderer/RendererUtilities.h"

#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/Vector3.h"

#include "ocean/rendering/Box.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Material.h"
#include "ocean/rendering/Transform.h"

void RendererUtilities::moveMouseTranslateView(const int currentX, const int currentY, const int previousX, const int previousY, Rendering::View& view, const Scalar interactionSpeed)
{
	ocean_assert(interactionSpeed > 0);

	const Scalar xDifference = Scalar(int(previousX) - int(currentX));
	const Scalar yDifference = Scalar(int(previousY) - int(currentY));

	if (xDifference == 0 && yDifference == 0)
	{
		return;
	}

	const Quaternion orientation = view.transformation().rotation();

	const Vector3 xAxis(1, 0, 0);
	const Vector3 yAxis(0, 1, 0);

	const Quaternion xRotation(orientation * xAxis, Numeric::deg2rad(Scalar(yDifference)) * interactionSpeed);
	const Quaternion yRotation(orientation * yAxis, Numeric::deg2rad(Scalar(xDifference)) * interactionSpeed);

	Quaternion rotation(xRotation * yRotation);
	rotation.normalize();

	const HomogenousMatrix4 newTransform(HomogenousMatrix4(rotation) * view.transformation());

	view.setTransformation(newTransform);
}

void RendererUtilities::moveMouseRotateView(const int currentX, const int currentY, const int previousX, const int previousY, Rendering::View& view, const Scalar interactionSpeed)
{
	ocean_assert(interactionSpeed > 0);

	const Scalar xDifference = Scalar(int(previousX) - int(currentX));
	const Scalar yDifference = Scalar(int(previousY) - int(currentY));

	if (xDifference == 0 && yDifference == 0)
	{
		return;
	}

	const Vector3 offset(xDifference * interactionSpeed, -yDifference * interactionSpeed, 0);

	HomogenousMatrix4 transformation(view.transformation());
	transformation.setTranslation(transformation.translation() + transformation.rotation() * offset);

	view.setTransformation(transformation);
}

Rendering::SceneRef RendererUtilities::createScene(Rendering::Engine& engine)
{
	Rendering::SceneRef renderingScene = engine.factory().createScene();
	Rendering::BoxRef box = engine.factory().createBox();
	Rendering::GeometryRef geometry = engine.factory().createGeometry();

	Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();
	attributeSet->addAttribute(engine.factory().createBlendAttribute());

	Rendering::MaterialRef material = engine.factory().createMaterial();

	material->setDiffuseColor(RGBAColor(1, 0, 0));
	material->setTransparency(0.5);

	attributeSet->addAttribute(material);

	geometry->addRenderable(box, attributeSet);

	Rendering::GroupRef root = engine.factory().createGroup();

	for (int x = -10; x <= 10; x += 5)
	{
		for (int y = -10; y <= 10; y += 5)
		{
			Rendering::TransformRef transform = engine.factory().createTransform();
			transform->setTransformation(HomogenousMatrix4(Vector3(Scalar(x), Scalar(y), 0), Random::quaternion(), Random::vector3(1, 5)));
			transform->addChild(geometry);

			root->addChild(transform);
		}
	}

	renderingScene->addChild(root);

	return renderingScene;
}
