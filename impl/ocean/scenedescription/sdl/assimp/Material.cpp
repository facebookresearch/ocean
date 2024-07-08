/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/assimp/Material.h"

#include "ocean/io/File.h"
#include "ocean/io/FileResolver.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/rendering/FrameTexture2D.h"
#include "ocean/rendering/Material.h"
#include "ocean/rendering/MediaTexture2D.h"

#include <assimp/pbrmaterial.h>

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace Assimp
{

Rendering::AttributeSetRef Material::parseMaterial(const Rendering::Engine& engine, const std::string& sceneFilename, const aiScene& assimpScene, const aiMaterial& assimpMaterial)
{
	const Rendering::MaterialRef material = engine.factory().createMaterial();
	ocean_assert(material);

	aiString assimpString;
	if (assimpMaterial.Get(AI_MATKEY_NAME, assimpString) == aiReturn_SUCCESS && assimpString.length != 0u)
	{
		material->setName(assimpString.data);
	}

	ai_real assimpFloat;
	aiColor4D assimpColor;

	if (assimpMaterial.Get(AI_MATKEY_COLOR_DIFFUSE, assimpColor) == aiReturn_SUCCESS)
	{
		// Assimp's alpha uses 0 for fully transparent and 1 for fully opaque
		material->setDiffuseColor(RGBAColor(assimpColor.r, assimpColor.g, assimpColor.b, assimpColor.a));
	}

	if (assimpMaterial.Get(AI_MATKEY_COLOR_AMBIENT, assimpColor) == aiReturn_SUCCESS)
	{
		// Assimp's alpha uses 0 for fully transparent and 1 for fully opaque
		material->setAmbientColor(RGBAColor(assimpColor.r, assimpColor.g, assimpColor.b, assimpColor.a));
	}

	if (assimpMaterial.Get(AI_MATKEY_COLOR_SPECULAR, assimpColor) == aiReturn_SUCCESS)
	{
		assimpFloat = 1.0f;
		assimpMaterial.Get(AI_MATKEY_SHININESS_STRENGTH, assimpFloat);

		// Assimp's alpha uses 0 for fully transparent and 1 for fully opaque
		material->setSpecularColor(RGBAColor(assimpColor.r, assimpColor.g, assimpColor.b, assimpColor.a).damped(assimpFloat));
	}

	if (assimpMaterial.Get(AI_MATKEY_COLOR_EMISSIVE, assimpColor) == aiReturn_SUCCESS)
	{
		// Assimp's alpha uses 0 for fully transparent and 1 for fully opaque
		material->setEmissiveColor(RGBAColor(assimpColor.r, assimpColor.g, assimpColor.b, assimpColor.a));
	}

	if (assimpMaterial.Get(AI_MATKEY_OPACITY, assimpFloat) == aiReturn_SUCCESS)
	{
		ocean_assert(assimpFloat >= 0.0f && assimpFloat <= 1.0f);
		material->setTransparency(1.0f - assimpFloat);
	}

	if (assimpMaterial.Get(AI_MATKEY_SHININESS, assimpFloat) == aiReturn_SUCCESS)
	{
		ocean_assert(assimpFloat >= 0.0f);
		material->setSpecularExponent(assimpFloat);
	}

	if (assimpMaterial.Get(AI_MATKEY_REFRACTI, assimpFloat) == aiReturn_SUCCESS)
	{
		try
		{
			material->setRefractionIndex(assimpFloat);
		}
		catch (const std::exception& message)
		{
			Log::warning() << message.what();
		}
	}

	const Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();
	ocean_assert(attributeSet);

	bool useMaterial = true;

	int shadingModel;
	if (assimpMaterial.Get(AI_MATKEY_SHADING_MODEL, shadingModel) == aiReturn_SUCCESS)
	{
		if (aiShadingMode(shadingModel) == aiShadingMode_NoShading)
		{
			useMaterial = false;
		}
	}

	if (useMaterial)
	{
		attributeSet->addAttribute(material);
	}

	bool textureIsTransparent = false;
	const Rendering::TexturesRef textures = parseTextures(engine, sceneFilename, assimpScene, assimpMaterial, textureIsTransparent);

	if (textures)
	{
		attributeSet->addAttribute(textures);

		if (textureIsTransparent)
		{
			attributeSet->addAttribute(engine.factory().createBlendAttribute());
		}
	}

	return attributeSet;
}

std::vector<Rendering::AttributeSetRef> Material::parseMaterials(const Rendering::Engine& engine, const std::string& sceneFilename, const aiScene& assimpScene)
{
	std::vector<Rendering::AttributeSetRef> attributeSets;
	attributeSets.reserve(assimpScene.mNumMaterials);

	for (unsigned int n = 0u; n < assimpScene.mNumMaterials; ++n)
	{
		const aiMaterial* material = assimpScene.mMaterials[n];
		ocean_assert(material != nullptr);

		attributeSets.emplace_back(parseMaterial(engine, sceneFilename, assimpScene, *material));
	}

	return attributeSets;
}

Rendering::TexturesRef Material::parseTextures(const Rendering::Engine& engine, const std::string& sceneFilename, const aiScene& assimpScene, const aiMaterial& assimpMaterial, bool& textureIsTransparent)
{
	textureIsTransparent = false;

	const IO::File sceneFile(sceneFilename);

	constexpr aiTextureType diffuseTextureType = aiTextureType_DIFFUSE;

	const unsigned int diffuseTextures = assimpMaterial.GetTextureCount(diffuseTextureType);

	if (diffuseTextures == 0u)
	{
		return Rendering::TexturesRef();
	}

	const Rendering::TexturesRef textures = engine.factory().createTextures();

	for (unsigned int n = 0u; n < diffuseTextures; ++n)
	{
		aiString texturePath;
		aiTextureOp assimpTextureOperation = aiTextureOp_Multiply;
		aiTextureMapMode assimpTextureMapingMode[3] = {aiTextureMapMode_Wrap, aiTextureMapMode_Wrap, aiTextureMapMode_Wrap};

		if (assimpMaterial.GetTexture(diffuseTextureType, n, &texturePath, nullptr, nullptr, nullptr, &assimpTextureOperation, assimpTextureMapingMode) == aiReturn_SUCCESS && texturePath.length != 0u)
		{
			Rendering::Texture2DRef texture;

			const aiTexture* embeddedTexture = assimpScene.GetEmbeddedTexture(texturePath.data);

			if (embeddedTexture != nullptr)
			{
				Frame frame;

				if (embeddedTexture->mHeight == 0u)
				{
					// we have a compressed texture

					frame = Media::Utilities::loadImage(embeddedTexture->pcData, size_t(embeddedTexture->mWidth));
				}
				else
				{
					// we have a texture with RGBA8888 format

					if (memcmp(embeddedTexture->achFormatHint, "rgba8888", 8) == 0)
					{
						constexpr unsigned int paddingElements = 0u;
						frame = Frame(FrameType(embeddedTexture->mWidth, embeddedTexture->mHeight, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_LOWER_LEFT), embeddedTexture->pcData, Frame::CM_COPY_REMOVE_PADDING_LAYOUT, paddingElements);
					}
					else
					{
						Log::warning() << "Invalid built-in texture format";
					}
				}

				if (frame.isValid())
				{
					if (frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && frame.hasTransparentPixel<uint8_t>(0xFF))
					{
						textureIsTransparent = true;
					}

					const Rendering::FrameTexture2DRef frameTexture = engine.factory().createFrameTexture2D();
					ocean_assert(frameTexture);

					frameTexture->setTexture(std::move(frame));

					texture = frameTexture;
				}
				else
				{
					Log::error() << "Failed to load embedded texture '" << texturePath.data << "'";
				}
			}
			else
			{
				const IO::Files resolvedFiles = IO::FileResolver::get().resolve(IO::File(texturePath.data), sceneFile, true);

				if (!resolvedFiles.empty())
				{
					const Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium(resolvedFiles.front()(), Media::Medium::IMAGE);

					if (frameMedium)
					{
						frameMedium->start();

						FrameRef frameRef = frameMedium->frame();
						if (frameRef)
						{
							if (frameRef->dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && frameRef->hasTransparentPixel<uint8_t>(0xFF))
							{
								textureIsTransparent = true;
							}
						}

						const Rendering::MediaTexture2DRef mediaTexture = engine.factory().createMediaTexture2D();
						ocean_assert(mediaTexture);

						mediaTexture->setMedium(frameMedium);

						texture = mediaTexture;
					}
				}

				if (!texture)
				{
					Log::error() << "Failed to load texture '" << texturePath.data << "' from file";
				}
			}

			if (texture)
			{
				Rendering::Texture::WrapType wrapTypeS = Rendering::Texture::WRAP_CLAMP;
				Rendering::Texture::WrapType wrapTypeT = Rendering::Texture::WRAP_CLAMP;

				switch (assimpTextureMapingMode[0])
				{
					case aiTextureMapMode_Wrap:
						wrapTypeS = Rendering::Texture::WRAP_REPEAT;
						break;

					case aiTextureMapMode_Clamp:
						wrapTypeS = Rendering::Texture::WRAP_CLAMP;
						break;

					default:
						// not supported
						break;
				}

				switch (assimpTextureMapingMode[1])
				{
					case aiTextureMapMode_Wrap:
						wrapTypeT = Rendering::Texture::WRAP_REPEAT;
						break;

					case aiTextureMapMode_Clamp:
						wrapTypeT = Rendering::Texture::WRAP_CLAMP;
						break;

					default:
						// not supported
						break;
				}

				try
				{
					texture->setWrapTypeS(wrapTypeS);
					texture->setWrapTypeT(wrapTypeT);
				}
				catch (...)
				{
					// nothing to do here
				}

				Rendering::Texture::EnvironmentMode environmentMode = Rendering::Texture::MODE_INVALID;

				switch (assimpTextureOperation)
				{
					case aiTextureOp_Multiply:
						environmentMode = Rendering::Texture::MODE_MODULATE;
						break;

					case aiTextureOp_Add:
						environmentMode = Rendering::Texture::MODE_ADD;
						break;

					case aiTextureOp_Subtract:
						environmentMode = Rendering::Texture::MODE_SUBTRACT;
						break;

					case aiTextureOp_SignedAdd:
						environmentMode = Rendering::Texture::MODE_ADD_SIGNED;
						break;

					default:
						// not supported
						break;
				}

				try
				{
					texture->setEnvironmentMode(environmentMode);
				}
				catch (...)
				{
					// nothing to do here
				}

				Rendering::Texture::MagFilterMode magFilterMode = Rendering::Texture::MAG_MODE_LINEAR;
				Rendering::Texture::MinFilterMode minFilterMode = Rendering::Texture::MIN_MODE_LINEAR;
				bool useMipmap = false;

				const std::string sceneType = String::toLower(sceneFile.extension());

				if (sceneType == "obj")
				{
					// in case the source is a Wavefront OBJ file, we use highest filter modes as the material does not allow to specify the intended filter mode

					magFilterMode = Rendering::Texture::MAG_MODE_LINEAR;
					minFilterMode = Rendering::Texture::MIN_MODE_LINEAR_MIPMAP_LINEAR;
					useMipmap = true;
				}
				else if (sceneType == "gltf" || sceneType == "glb")
				{
					// in case the source is a GLTF file
					// https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#reference-sampler

					enum GLTFSamplerMagFilter : uint32_t
					{
						SamplerMagFilter_Unset = 0u,
						SamplerMagFilter_Nearest = 9728u,
						SamplerMagFilter_Linear = 9729u
					};

					enum GLTFSamplerMinFilter : uint32_t
					{
						SamplerMinFilter_Unset = 0u,
						SamplerMinFilter_Nearest = 9728u,
						SamplerMinFilter_Linear = 9729u,
						SamplerMinFilter_Nearest_Mipmap_Nearest = 9984u,
						SamplerMinFilter_Linear_Mipmap_Nearest = 9985u,
						SamplerMinFilter_Nearest_Mipmap_Linear = 9986u,
						SamplerMinFilter_Linear_Mipmap_Linear = 9987u
					};

					GLTFSamplerMagFilter gltFSamplerMagFilter;
					GLTFSamplerMinFilter gltfSamplerMinFilter;

					if (assimpMaterial.Get<GLTFSamplerMagFilter>(AI_MATKEY_GLTF_MAPPINGFILTER_MAG(diffuseTextureType, n), gltFSamplerMagFilter) == aiReturn_SUCCESS)
					{
						switch (gltFSamplerMagFilter)
						{
							case SamplerMagFilter_Linear:
								magFilterMode = Rendering::Texture::MAG_MODE_LINEAR;
								break;

							default:
								ocean_assert(gltFSamplerMagFilter == SamplerMagFilter_Nearest);
								magFilterMode = Rendering::Texture::MAG_MODE_NEAREST;
								break;
						}
					}

					if (assimpMaterial.Get<GLTFSamplerMinFilter>(AI_MATKEY_GLTF_MAPPINGFILTER_MIN(diffuseTextureType, n), gltfSamplerMinFilter) == aiReturn_SUCCESS)
					{
						switch (gltfSamplerMinFilter)
						{
							case SamplerMinFilter_Linear:
								minFilterMode = Rendering::Texture::MIN_MODE_LINEAR;
								break;

							case SamplerMinFilter_Nearest_Mipmap_Nearest:
								minFilterMode = Rendering::Texture::MIN_MODE_NEAREST_MIPMAP_NEAREST;
								useMipmap = true;
								break;

							case SamplerMinFilter_Linear_Mipmap_Nearest:
								minFilterMode = Rendering::Texture::MIN_MODE_LINEAR_MIPMAP_NEAREST;
								useMipmap = true;
								break;

							case SamplerMinFilter_Nearest_Mipmap_Linear:
								minFilterMode = Rendering::Texture::MIN_MODE_NEAREST_MIPMAP_LINEAR;
								useMipmap = true;
								break;

							case SamplerMinFilter_Linear_Mipmap_Linear:
								minFilterMode = Rendering::Texture::MIN_MODE_LINEAR_MIPMAP_LINEAR;
								useMipmap = true;
								break;

							default:
								ocean_assert(gltfSamplerMinFilter == SamplerMinFilter_Nearest);
								minFilterMode = Rendering::Texture::MIN_MODE_NEAREST;
								break;
						}
					}
				}

				texture->setMagnificationFilterMode(magFilterMode);
				texture->setMinificationFilterMode(minFilterMode);
				texture->setUseMipmaps(useMipmap);

				textures->addTexture(texture);

				// for now we do not support more than one diffuse texture
				break;
			}
		}
	}

	if (textures->numberTextures() == 0u)
	{
		return Rendering::TextureRef();
	}

	return textures;
}

}

}

}

}
