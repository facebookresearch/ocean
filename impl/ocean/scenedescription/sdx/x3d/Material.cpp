/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Material.h"
#include "ocean/scenedescription/sdx/x3d/Appearance.h"

#include "ocean/rendering/Material.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Material::Material(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DAppearanceChildNode(environment),
	X3DMaterialNode(environment),
	ambientIntensity_(Scalar(0.2)),
	diffuseColor_(RGBAColor(0.8f, 0.8f, 0.8f)),
	emissiveColor_(RGBAColor(0, 0, 0)),
	shininess_(Scalar(0.2)),
	specularColor_(RGBAColor(0, 0, 0)),
	transparency_(0),
	reflectivity_(0),
	refractionIndex_(1)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createMaterial();
}

Material::NodeSpecification Material::specifyNode()
{
	NodeSpecification specification("Material");

	registerField(specification, "ambientIntensity", ambientIntensity_, ACCESS_GET_SET);
	registerField(specification, "diffuseColor", diffuseColor_, ACCESS_GET_SET);
	registerField(specification, "emissiveColor", emissiveColor_, ACCESS_GET_SET);
	registerField(specification, "shininess", shininess_, ACCESS_GET_SET);
	registerField(specification, "specularColor", specularColor_, ACCESS_GET_SET);
	registerField(specification, "transparency", transparency_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));

	// not part of the standard X3D specification
	registerField(specification, "reflectivity", reflectivity_, ACCESS_GET_SET);
	registerField(specification, "refractionIndex", refractionIndex_, ACCESS_GET_SET);

	X3DMaterialNode::registerFields(specification);

	return specification;
}

void Material::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DMaterialNode::onInitialize(scene, timestamp);

	try
	{
		const Rendering::MaterialRef renderingMaterial(renderingObject_);

		if (renderingMaterial)
		{
			renderingMaterial->set(diffuseColor_.value().damped(float(ambientIntensity_.value())), diffuseColor_.value(), emissiveColor_.value(), specularColor_.value(), float(shininess_.value()) * 128.0f, float(transparency_.value()));

			// not all rendering engines support reflectivity

			try
			{
				renderingMaterial->setReflectivity(float(reflectivity_.value()));
			}
			catch(...)
			{
				// nothing to do here
			}

			try
			{
				renderingMaterial->setRefractionIndex(float(refractionIndex_.value()));
			}
			catch(...)
			{
				// nothing to do here
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void Material::onFieldChanged(const std::string& fieldName)
{
	try
	{
		const Rendering::MaterialRef renderingMaterial(renderingObject_);
		if (renderingMaterial)
		{
			if (fieldName == "ambientIntensity")
			{
				renderingMaterial->setAmbientColor(diffuseColor_.value().damped(float(ambientIntensity_.value())));
				return;
			}

			if (fieldName == "diffuseColor")
			{
				renderingMaterial->setDiffuseColor(diffuseColor_.value());
				renderingMaterial->setAmbientColor(diffuseColor_.value().damped(float(ambientIntensity_.value())));

				return;
			}

			if (fieldName == "emissiveColor")
			{
				renderingMaterial->setEmissiveColor(emissiveColor_.value());
				return;
			}

			if (fieldName == "shininess")
			{
				renderingMaterial->setSpecularExponent(float(shininess_.value()) * 128.0f);
				return;
			}

			if (fieldName == "specularColor")
			{
				renderingMaterial->setSpecularColor(specularColor_.value());
				return;
			}

			if (fieldName == "reflectivity")
			{
				renderingMaterial->setReflectivity(float(reflectivity_.value()));
				return;
			}

			if (fieldName == "refractionIndex")
			{
				renderingMaterial->setRefractionIndex(float(refractionIndex_.value()));
				return;
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	X3DMaterialNode::onFieldChanged(fieldName);
}

bool Material::onFieldChanging(const std::string& fieldName, const Field& field)
{
	if (fieldName == "transparency")
	{
		if (field.isType(Field::TYPE_FLOAT, 0))
		{
			const SingleFloat& newField(field.cast<SingleFloat>(field));

			const bool checkTrasparencyState = (transparency_.value() == 0 && newField.value() != 0)
													|| (transparency_.value() != 0 && newField.value() == 0);

			if (transparency_.assign(newField))
			{
				const Rendering::MaterialRef renderingMaterial(renderingObject_);
				renderingMaterial->setTransparency(float(transparency_.value()));

				if (checkTrasparencyState)
				{
					const NodeRefs parents(parentNodes());

					for (NodeRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
					{
						SmartObjectRef<Appearance, Node> appearance(*i);
						ocean_assert(appearance);

						appearance->checkTransparencyState();
					}
				}
			}

			return true;
		}

		return false;
	}

	return X3DMaterialNode::onFieldChanging(fieldName, field);
}

size_t Material::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
