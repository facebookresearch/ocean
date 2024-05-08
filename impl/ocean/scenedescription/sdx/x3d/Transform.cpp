/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Transform.h"

#include "ocean/rendering/Transform.h"

#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Transform::Transform(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DBoundedObject(environment),
	X3DGroupingNode(environment),
	center_(Vector3(0, 0, 0)),
	rotation_(Rotation(0, 0, 1, 0)),
	scale_(Vector3(1, 1, 1)),
	scaleOrientation_(Rotation(0, 0, 1, 0)),
	translation_(Vector3(0, 0, 0))
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createTransform();
}

Transform::NodeSpecification Transform::specifyNode()
{
	NodeSpecification specification("Transform");

	registerField(specification, "center", center_, ACCESS_GET_SET);
	registerField(specification, "rotation", rotation_, ACCESS_GET_SET);
	registerField(specification, "scale", scale_, ACCESS_GET_SET);
	registerField(specification, "scaleOrientation", scaleOrientation_, ACCESS_GET_SET);
	registerField(specification, "translation", translation_, ACCESS_GET_SET);

	X3DGroupingNode::registerFields(specification);

	return specification;
}

void Transform::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DGroupingNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::TransformRef renderingTransform(renderingObject_);
		if (renderingTransform)
		{
			if (center_.timestamp().isInvalid() && scaleOrientation_.timestamp().isInvalid())
			{
				renderingTransform->setTransformation(HomogenousMatrix4(translation_.value(), rotation_.value(), scale_.value()));
			}
			else
			{
				// transformation to apply: P' = (T * C * R * SR * S * -SR * -C) * P

				const HomogenousMatrix4 matrixT(translation_.value());
				const HomogenousMatrix4 matrixC(center_.value());
				const HomogenousMatrix4 matrixR(rotation_.value());
				const HomogenousMatrix4 matrixSR(scaleOrientation_.value());
				const HomogenousMatrix4 matrixS(Vector3(scale_.value().x(), 0, 0), Vector3(0, scale_.value().y(), 0), Vector3(0, 0, scale_.value().z()));

				const HomogenousMatrix4 transformation(matrixT * matrixC * matrixR * matrixSR * matrixS * HomogenousMatrix4(-scaleOrientation_.value()) * HomogenousMatrix4(-center_.value()));
				renderingTransform->setTransformation(transformation);
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void Transform::onFieldChanged(const std::string& fieldName)
{
	try
	{
		const Rendering::TransformRef renderingTransform(renderingObject_);
		if (renderingTransform)
		{
			if ((fieldName == "rotation" || fieldName == "scale" || fieldName == "translation") && center_.timestamp().isInvalid() && scaleOrientation_.timestamp().isInvalid())
			{
				renderingTransform->setTransformation(HomogenousMatrix4(translation_.value(), rotation_.value(), scale_.value()));
			}
			else if (fieldName == "center" || fieldName == "rotation" || fieldName == "scale" || fieldName == "scaleOrientation" || fieldName == "translation")
			{
				// transformation to apply: P' = (T * C * R * SR * S * -SR * -C) * P

				const HomogenousMatrix4 matrixT(translation_.value());
				const HomogenousMatrix4 matrixC(center_.value());
				const HomogenousMatrix4 matrixR(rotation_.value());
				const HomogenousMatrix4 matrixSR(scaleOrientation_.value());
				const HomogenousMatrix4 matrixS(Vector3(scale_.value().x(), 0, 0), Vector3(0, scale_.value().y(), 0), Vector3(0, 0, scale_.value().z()));

				const HomogenousMatrix4 transformation(matrixT * matrixC * matrixR * matrixSR * matrixS * HomogenousMatrix4(-scaleOrientation_.value()) * HomogenousMatrix4(-center_.value()));
				ocean_assert(transformation.isValid());
				renderingTransform->setTransformation(transformation);
			}
			else
			{
				ocean_assert(false);
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	X3DGroupingNode::onFieldChanged(fieldName);
}

size_t Transform::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
