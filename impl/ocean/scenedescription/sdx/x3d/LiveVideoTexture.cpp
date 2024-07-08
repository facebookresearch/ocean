/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/LiveVideoTexture.h"

#include "ocean/rendering/Texture2D.h"

#include "ocean/media/LiveVideo.h"
#include "ocean/media/Manager.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

LiveVideoTexture::LiveVideoTexture(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DAppearanceChildNode(environment),
	X3DTextureNode(environment),
	X3DTexture2DNode(environment),
	X3DUrlObject(environment),
	dimension_(Vector2(640, 480)),
	frequency_(30)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

LiveVideoTexture::NodeSpecification LiveVideoTexture::specifyNode()
{
	NodeSpecification specification("LiveVideoTexture");

	registerField(specification, "dimension", dimension_);
	registerField(specification, "frequency", frequency_);

	X3DTexture2DNode::registerFields(specification);
	X3DUrlObject::registerFields(specification);

	return specification;
}

void LiveVideoTexture::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DTexture2DNode::onInitialize(scene, timestamp);
	X3DUrlObject::onInitialize(scene, timestamp);

	try
	{
		applyUrl(url_.values(), Media::Medium::LIVE_VIDEO);
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void LiveVideoTexture::onFieldChanged(const std::string& fieldName)
{
	if (fieldName == "url" || fieldName == "dimension" || fieldName == "frequency")
	{
		applyUrl(url_.values(), Media::Medium::LIVE_VIDEO);
		return;
	}

	X3DTexture2DNode::onFieldChanged(fieldName);
	X3DUrlObject::onFieldChanged(fieldName);
}

void LiveVideoTexture::onMediumChanged(const Media::MediumRef& medium)
{
	ocean_assert(medium);
	Media::LiveVideoRef liveVideo(medium);
	ocean_assert(liveVideo);

	const Vector2 dim(dimension_.value());
	if (dim[0] > 0 && dim[1] > 0)
	{
		liveVideo->setPreferredFrameDimension((unsigned int)(dim[0]), (unsigned int)(dim[1]));
	}

	if (frequency_.value() > 0)
	{
		liveVideo->setPreferredFrameFrequency(Media::FrameMedium::FrameFrequency(frequency_.value()));
	}
}

size_t LiveVideoTexture::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
