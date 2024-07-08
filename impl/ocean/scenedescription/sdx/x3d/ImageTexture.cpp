/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/ImageTexture.h"
#include "ocean/scenedescription/sdx/x3d/TextureProperties.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

ImageTexture::ImageTexture(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DAppearanceChildNode(environment),
	X3DTextureNode(environment),
	X3DTexture2DNode(environment),
	X3DUrlObject(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

ImageTexture::NodeSpecification ImageTexture::specifyNode()
{
	NodeSpecification specification("ImageTexture");

	X3DTexture2DNode::registerFields(specification);
	X3DUrlObject::registerFields(specification);

	return specification;
}

void ImageTexture::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DTexture2DNode::onInitialize(scene, timestamp);
	X3DUrlObject::onInitialize(scene, timestamp);

	try
	{
		const IO::Files resolvedFiles(resolveUrls());

		StringVector urls;
		for (IO::Files::const_iterator i = resolvedFiles.begin(); i != resolvedFiles.end(); ++i)
		{
			if (i->exists())
			{
				urls.push_back((*i)());
			}
		}

		applyUrl(urls, Media::Medium::IMAGE);

		if (urls.empty() && !resolvedFiles.empty())
		{
			Log::error() << "Failed to load an image texture: \"" << url_.values().front() << "\".";
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void ImageTexture::onFieldChanged(const std::string& fieldName)
{
	if (fieldName == "url")
	{
		const IO::Files resolvedFiles(resolveUrls());

		StringVector urls;
		for (IO::Files::const_iterator i = resolvedFiles.begin(); i != resolvedFiles.end(); ++i)
		{
			if (i->exists())
			{
				urls.push_back((*i)());
			}
		}

		applyUrl(urls, Media::Medium::IMAGE);
		return;
	}

	X3DTexture2DNode::onFieldChanged(fieldName);
	X3DUrlObject::onFieldChanged(fieldName);
}

size_t ImageTexture::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
