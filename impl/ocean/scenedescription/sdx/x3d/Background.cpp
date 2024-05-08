/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Background.h"
#include "ocean/scenedescription/sdx/x3d/X3DUrlObject.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/rendering/MediaTexture2D.h"
#include "ocean/rendering/SkyBackground.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Background::Background(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DBindableNode(environment),
	X3DBackgroundNode(environment),
	transparency_(0)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createSkyBackground();
}

Background::~Background()
{
	const Rendering::Engine::Framebuffers framebuffers = engine()->framebuffers();
	for (Rendering::Engine::Framebuffers::const_iterator i = framebuffers.begin(); i != framebuffers.end(); ++i)
	{
		Rendering::ViewRef view((*i)->view());

		if (view)
		{
			view->removeBackground(renderingObject_);
		}
	}
}

Background::NodeSpecification Background::specifyNode()
{
	NodeSpecification specification("Background");

	registerField(specification, "backUrl", backUrl_, ACCESS_GET_SET);
	registerField(specification, "bottomUrl", bottomUrl_, ACCESS_GET_SET);
	registerField(specification, "frontUrl", frontUrl_, ACCESS_GET_SET);
	registerField(specification, "leftUrl", leftUrl_, ACCESS_GET_SET);
	registerField(specification, "rightUrl", rightUrl_, ACCESS_GET_SET);
	registerField(specification, "topUrl", topUrl_, ACCESS_GET_SET);
	registerField(specification, "transparency", transparency_, ACCESS_GET_SET);

	X3DBackgroundNode::registerFields(specification);

	return specification;
}

void Background::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DBackgroundNode::onInitialize(scene, timestamp);

	try
	{
		Rendering::SkyBackgroundRef renderingSkyBackground(renderingObject_);

		if (renderingSkyBackground)
		{
			Rendering::TexturesRef frontTextures;

			if (!frontUrl_.values().empty())
			{
				frontTextures = createTextures(frontUrl_.values());
				renderingSkyBackground->setFrontTextures(frontTextures);
			}

			if (!backUrl_.values().empty())
			{
				if (backUrl_.values() == frontUrl_.values() && frontTextures)
				{
					renderingSkyBackground->setBackTextures(frontTextures);
				}
				else
				{
					renderingSkyBackground->setBackTextures(createTextures(backUrl_.values()));
				}
			}

			if (!leftUrl_.values().empty())
			{
				if (leftUrl_.values() == frontUrl_.values() && frontTextures)
				{
					renderingSkyBackground->setLeftTextures(frontTextures);
				}
				else
				{
					renderingSkyBackground->setLeftTextures(createTextures(leftUrl_.values()));
				}
			}

			if (!rightUrl_.values().empty())
			{
				if (rightUrl_.values() == frontUrl_.values() && frontTextures)
				{
					renderingSkyBackground->setRightTextures(frontTextures);
				}
				else
				{
					renderingSkyBackground->setRightTextures(createTextures(rightUrl_.values()));
				}
			}

			if (!topUrl_.values().empty())
			{
				if (topUrl_.values() == frontUrl_.values() && frontTextures)
				{
					renderingSkyBackground->setTopTextures(frontTextures);
				}
				else
				{
					renderingSkyBackground->setTopTextures(createTextures(topUrl_.values()));
				}
			}

			if (!bottomUrl_.values().empty())
			{
				if (bottomUrl_.values() == frontUrl_.values() && frontTextures)
				{
					renderingSkyBackground->setBottomTextures(frontTextures);
				}
				else
				{
					renderingSkyBackground->setBottomTextures(createTextures(bottomUrl_.values()));
				}
			}

			const Rendering::Engine::Framebuffers framebuffers = engine()->framebuffers();
			for (Rendering::Engine::Framebuffers::const_iterator i = framebuffers.begin(); i != framebuffers.end(); ++i)
			{
				Rendering::ViewRef view((*i)->view());

				if (view)
				{
					view->addBackground(renderingSkyBackground);
				}
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void Background::onFieldChanged(const std::string& fieldName)
{
	try
	{
		ocean_assert(false && "Missing implementation!");
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	X3DBackgroundNode::onFieldChanged(fieldName);
}

size_t Background::objectAddress() const
{
	return size_t(this);
}

Rendering::TexturesRef Background::createTextures(const MultiString::Values& urls) const
{
	const IO::Files resolvedFiles(X3DUrlObject::resolveUrls(filename(), urls));

	for (const IO::File& resolvedFile : resolvedFiles)
	{
		if (resolvedFile.exists())
		{
			const Media::FrameMediumRef frameMedium(Media::Manager::get().newMedium(resolvedFile(), Media::Medium::FRAME_MEDIUM, true));

			if (frameMedium)
			{
				const Rendering::TexturesRef renderingTextures(engine()->factory().createTextures());
				const Rendering::MediaTexture2DRef renderingTexture(engine()->factory().createMediaTexture2D());

				if (renderingTextures && renderingTexture)
				{
					renderingTexture->setMedium(frameMedium);
					renderingTexture->setEnvironmentMode(Rendering::Texture::MODE_REPLACE);

					frameMedium->start();

					renderingTextures->addTexture(renderingTexture);
					return renderingTextures;
				}
			}
		}
	}

	return Rendering::TexturesRef();
}

}

}

}

}
