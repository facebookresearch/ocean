/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/obj/Material.h"

#include "ocean/scenedescription/SDLScene.h"

#include "ocean/rendering/Material.h"
#include "ocean/rendering/MediaTexture2D.h"
#include "ocean/rendering/Textures.h"

#include "ocean/media/Manager.h"

#include "ocean/io/FileResolver.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

Rendering::AttributeSetRef Material::attributeSet(const Rendering::EngineRef& engine, const SDLScene& scene)
{
	if (attributeSet_.isNull())
	{
		attributeSet_ = engine->factory().createAttributeSet();

		if (illuminationModel() != IlluminationModel(0u))
		{
			Rendering::MaterialRef material(engine->factory().createMaterial());

			material->setAmbientColor(ambientColor_);
			material->setDiffuseColor(diffuseColor_);
			material->setEmissiveColor(emissiveColor_);
			material->setSpecularColor(specularColor_);
			material->setSpecularExponent(float(specularExponent_));

			attributeSet_->addAttribute(material);

			if (transparency_ > 0)
			{
				material->setTransparency(transparency_);
				attributeSet_->addAttribute(engine->factory().createBlendAttribute());
			}
		}

		if (!textureName_.empty())
		{
			Rendering::TexturesRef textures(engine->factory().createTextures());
			Rendering::MediaTexture2DRef texture2D(engine->factory().createMediaTexture2D());
			texture2D->setEnvironmentMode(Rendering::Texture::MODE_MODULATE);

			// we use highest filter modes as the material does not allow to specify the intended filter mode

			texture2D->setMagnificationFilterMode(Rendering::Texture::MAG_MODE_LINEAR);
			texture2D->setMinificationFilterMode(Rendering::Texture::MIN_MODE_LINEAR_MIPMAP_LINEAR);
			texture2D->setUseMipmaps(true);

			texture2D->setWrapTypeS(Rendering::Texture::WRAP_REPEAT);
			texture2D->setWrapTypeT(Rendering::Texture::WRAP_REPEAT);

			const IO::Files resolvedFiles(IO::FileResolver::get().resolve(IO::File(textureName_), IO::File(scene.filename()), true));
			bool textureCreated = false;

			for (const IO::File& resolvedFile : resolvedFiles)
			{
				ocean_assert(resolvedFile.exists());

				Media::FrameMediumRef frameMedium(Media::Manager::get().newMedium(resolvedFile(), Media::Medium::IMAGE, true));

				if (frameMedium.isNull())
				{
					frameMedium = Media::Manager::get().newMedium(resolvedFile(), Media::Medium::FRAME_MEDIUM, true);
				}

				if (frameMedium)
				{
					frameMedium->start();
					texture2D->setMedium(frameMedium);
					textureCreated = true;

					break;
				}
				else
				{
					Log::error() << "Could not create a valid texture for '" << resolvedFile() << "'.";
				}
			}

			if (!textureCreated)
			{
				Log::warning() << "Could not resolve the defined texture file: '" << textureName_ << "'.";
				Log::warning() << "Possible path would have been:";

				for (const IO::File& resolvedFile : resolvedFiles)
				{
					Log::warning() << resolvedFile();
				}
			}

			textures->setTexture(texture2D, 0);
			attributeSet_->addAttribute(textures);
		}
	}

	return attributeSet_;
}

}

}

}

}
