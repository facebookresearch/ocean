/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESProgramManager.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

#ifdef OCEAN_RENDERING_GLES_USE_ES
const char* GLESProgramManager::partPlatform_ =
	// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
	R"SHADER(#version 300 es

			#define OCEAN_LOWP lowp

			#define OCEAN_TEXTURE_TWO_CHANNELS_FIRST r // the first channel in a 2-channel texture
			#define OCEAN_TEXTURE_TWO_CHANNELS_SECOND a // the second channel in a 2-channel texture
		)SHADER";
#else
const char* GLESProgramManager::partPlatform_ =
	// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
	R"SHADER(#version 330

			#define OCEAN_LOWP // does not exist

			#define OCEAN_TEXTURE_TWO_CHANNELS_FIRST r // the first channel in a 2-channel texture
			#define OCEAN_TEXTURE_TWO_CHANNELS_SECOND g // the second channel in a 2-channel texture
		)SHADER";
#endif

const char* GLESProgramManager::partOneTextureLookupY8ToRGBA32_ =
	R"SHADER(
		#define OCEAN_ONE_TEXTURE_LOOKUP_TO_RGBA(TEXTURE, TEXTURE_COORDINATE) vec4(texture(TEXTURE, TEXTURE_COORDINATE).OCEAN_TEXTURE_TWO_CHANNELS_FIRST, 1.0, 1.0, 1.0).rrra;
	)SHADER";

const char* GLESProgramManager::partOneTextureLookupRGBA32ToRGBA32_ =
	R"SHADER(
		#define OCEAN_ONE_TEXTURE_LOOKUP_TO_RGBA(TEXTURE, TEXTURE_COORDINATE) texture(TEXTURE, TEXTURE_COORDINATE).rgba
	)SHADER";

const char* GLESProgramManager::partOneTextureLookupBGRA32ToRGBA32_ =
	R"SHADER(
		#define OCEAN_ONE_TEXTURE_LOOKUP_TO_RGBA(TEXTURE, TEXTURE_COORDINATE) texture(TEXTURE, TEXTURE_COORDINATE).bgra
	)SHADER";

const char* GLESProgramManager::partOneTextureLookupYUV24ToRGBA32_ =
	R"SHADER(
		const OCEAN_LOWP mat3 colorTransform = mat3(1, 1, 1, 0, -0.39465, 2.03211, 1.13983, -0.58060, 0);
		const OCEAN_LOWP vec3 colorCorrection = vec3(0.0625, 0.5, 0.5);

		#define OCEAN_ONE_TEXTURE_LOOKUP_TO_RGBA(TEXTURE, TEXTURE_COORDINATE) vec4(colorTransform * (texture(TEXTURE, TEXTURE_COORDINATE).rgb - colorCorrection), 1.0)
	)SHADER";

const char* GLESProgramManager::partOneTextureLookupYVU24ToRGBA32_ =
	R"SHADER(
		const OCEAN_LOWP mat3 colorTransform = mat3(1, 1, 1, 0, -0.39465, 2.03211, 1.13983, -0.58060, 0);
		const OCEAN_LOWP vec3 colorCorrection = vec3(0.0625, 0.5, 0.5);

		#define OCEAN_ONE_TEXTURE_LOOKUP_TO_RGBA(TEXTURE, TEXTURE_COORDINATE) vec4(colorTransform * (texture(TEXTURE, TEXTURE_COORDINATE).rbg - colorCorrection), 1.0)
	)SHADER";

const char* GLESProgramManager::partTwoTexturesLookupY_UV12ToRGBA32_ =
	R"SHADER(
		const OCEAN_LOWP mat3 colorTransform = mat3(1, 1, 1, 0, -0.39465, 2.03211, 1.13983, -0.58060, 0);
		const OCEAN_LOWP vec3 colorCorrection = vec3(0.0625, 0.5, 0.5);

		#define OCEAN_TWO_TEXTURES_LOOKUP_TO_RGBA(TEXTURE_PRIMARY, TEXTURE_SECONDARY, TEXTURE_COORDINATE) vec4(colorTransform * (vec3(texture(TEXTURE_PRIMARY, TEXTURE_COORDINATE).r, texture(TEXTURE_SECONDARY, TEXTURE_COORDINATE).OCEAN_TEXTURE_TWO_CHANNELS_FIRST, texture(TEXTURE_SECONDARY, TEXTURE_COORDINATE).OCEAN_TEXTURE_TWO_CHANNELS_SECOND) - colorCorrection), 1.0)
	)SHADER";

const char* GLESProgramManager::partTwoTexturesLookupY_VU12ToRGBA32_ =
	R"SHADER(
		const OCEAN_LOWP mat3 colorTransform = mat3(1, 1, 1, 0, -0.39465, 2.03211, 1.13983, -0.58060, 0);
		const OCEAN_LOWP vec3 colorCorrection = vec3(0.0625, 0.5, 0.5);

		#define OCEAN_TWO_TEXTURES_LOOKUP_TO_RGBA(TEXTURE_PRIMARY, TEXTURE_SECONDARY, TEXTURE_COORDINATE) vec4(colorTransform * (vec3(texture(TEXTURE_PRIMARY, TEXTURE_COORDINATE).r, texture(TEXTURE_SECONDARY, TEXTURE_COORDINATE).OCEAN_TEXTURE_TWO_CHANNELS_SECOND, texture(TEXTURE_SECONDARY, TEXTURE_COORDINATE).OCEAN_TEXTURE_TWO_CHANNELS_FIRST) - colorCorrection), 1.0)
	)SHADER";

const char* GLESProgramManager::partTwoTexturesLookupY_U_V12ToRGBA32_ =
	R"SHADER(
		const OCEAN_LOWP mat3 colorTransform = mat3(1, 1, 1, 0, -0.39465, 2.03211, 1.13983, -0.58060, 0);
		const OCEAN_LOWP vec3 colorCorrection = vec3(0.0625, 0.5, 0.5);

		#define OCEAN_TWO_TEXTURES_LOOKUP_TO_RGBA(TEXTURE_PRIMARY, TEXTURE_SECONDARY, TEXTURE_COORDINATE) vec4(colorTransform * (vec3(texture(TEXTURE_PRIMARY, TEXTURE_COORDINATE).r, texture(TEXTURE_SECONDARY, vec2(TEXTURE_COORDINATE.x, TEXTURE_COORDINATE.y * 0.5)).OCEAN_TEXTURE_TWO_CHANNELS_FIRST, texture(TEXTURE_SECONDARY, vec2(TEXTURE_COORDINATE.x, TEXTURE_COORDINATE.y * 0.5 + 0.5)).OCEAN_TEXTURE_TWO_CHANNELS_FIRST) - colorCorrection), 1.0)
	)SHADER";


const char* GLESProgramManager::partDefinitionMaterial_ =
	R"SHADER(
		// Material structure
		struct Material
		{
			// Ambient color value
			OCEAN_LOWP vec4 ambientColor;

			// Diffuse color value
			OCEAN_LOWP vec4 diffuseColor;

			// Specular color value
			OCEAN_LOWP vec4 specularColor;

			// Emissive color value
			OCEAN_LOWP vec4 emissiveColor;

			// Specular exponent
			OCEAN_LOWP float specularExponent;
		};
	)SHADER";

const char* GLESProgramManager::partDefinitionLight_ =
	R"SHADER(
		// Light structure used for directional, point and spot light
		struct Light
		{
			// Position in camera coordinate system for point and spot light
			// or normalized light direction for directional light with w == 0
			OCEAN_LOWP vec4 positionOrDirection;

			// Ambient color value
			OCEAN_LOWP vec4 ambientColor;

			// Diffuse color value
			OCEAN_LOWP vec4 diffuseColor;

			// Specular color value
			OCEAN_LOWP vec4 specularColor;

			// Direction of spot light
			OCEAN_LOWP vec3 spotDirection;

			// Cut off angle for spot light
			OCEAN_LOWP float spotCutOffAngle;

			// Spot exponent for spot light
			OCEAN_LOWP float spotExponent;

			// Flag determining whether point or spot light use attenuation
			int attenuationIsUsed;

			// Attenuation factors for point and spot light
			// Order is constant, linear, quadratic
			OCEAN_LOWP vec3 attenuationFactors;
		};
	)SHADER";

const char* GLESProgramManager::partFunctionLighting_ =
	R"SHADER(
		// Lights used for lighting
		uniform Light lights[8];

		// The number of defined lights, with range [0, 8]
		uniform int numberLights;

		void lighting(in vec4 vertexInCamera, in vec3 normal, in vec4 materialAmbientColor, in vec4 materialEmissiveColor, in vec4 materialDiffuseColor, in vec4 materialSpecularColor, in float materialSpecularExponent, out vec4 resultingColor, out vec4 resultingColorSpecular)
		{
			// when using textures, specular color must be handled separately
			resultingColorSpecular = vec4(0.0, 0.0, 0.0, 0.0);

			resultingColor = materialEmissiveColor;

			for (int lightIndex = 0; lightIndex < 8; ++lightIndex)
			{
				if (lightIndex >= numberLights)
				{
					break;
				}

				Light light = lights[lightIndex];

				vec4 localResultingColor = vec4(0, 0, 0, 0);

				vec3 lightVector;
				float attenuationFactor = 1.0;

				// if this light is a point or spot light)
				if (light.positionOrDirection.w != 0.0)
				{
					// vector from vertex to light
					lightVector = light.positionOrDirection.xyz - vertexInCamera.xyz;

					// check if the attenuation factor has to be calculated
					if (light.attenuationIsUsed == 1)
					{
						// distance vector (1, d, d^2)
						vec3 distances;
						distances.x = 1.0;
						distances.z = dot(lightVector, lightVector);
						distances.y = sqrt(distances.z);

						attenuationFactor = 1.0 / dot(distances, light.attenuationFactors); // == 1 / (constant + linear * d + quadratic * d^2)
					}

					// from this position the light vector is normalized
					lightVector = normalize(lightVector);

					// if this light is a spot light
					if (light.spotCutOffAngle > 0.0)
					{
						float spotFactor = dot(-lightVector, light.spotDirection);

						if (spotFactor >= cos(light.spotCutOffAngle))
						{
							spotFactor = pow(spotFactor, light.spotExponent);
						}
						else
						{
							spotFactor = 0.0;
						}

						attenuationFactor *= spotFactor;
					}
				}
				// if this light is a directional light
				else
				{
					// for a directional light the position value holds the light direction
					lightVector	= -light.positionOrDirection.xyz;
				}

				if (attenuationFactor > 0.0)
				{
					// The ambient color is independent from any light or viewing direction
					localResultingColor += light.ambientColor * materialAmbientColor;

					// The diffuse color depends on the normal and light direction
					localResultingColor += light.diffuseColor * materialDiffuseColor * max(0.0, dot(normal, lightVector));

					vec3 viewDirection = -vertexInCamera.xyz; // (0, 0, 0) - vertexInCamera.xyz

					// Compute half vector
					vec3 hVector = normalize(lightVector + viewDirection);

					float normalDotHVector = dot(normal, hVector);

					if (normalDotHVector > 0.0)
					{
						resultingColorSpecular += pow(normalDotHVector, materialSpecularExponent) * materialSpecularColor * light.specularColor * attenuationFactor;
					}

					localResultingColor *= attenuationFactor;
				}

				resultingColor += localResultingColor;
			}

			resultingColor.a = materialDiffuseColor.a;
			resultingColorSpecular.a = 0.0;
		}
	)SHADER";


const char* GLESProgramManager::programVertexShaderStaticColor_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Vertex attribute
		in vec4 aVertex;

		// 1, to use per-vertex colors; 0, to use the material's colors
		uniform int usePerVertexColors;

		// The static color, in case 'usePerVertexColors == 0'
		uniform OCEAN_LOWP vec4 color;

		// Per-vertex color attribute, in case 'usePerVertexColors == 1'
		in vec4 aColor;

		// Resulting front face color
		out OCEAN_LOWP vec4 vFrontColor;

		// The size of the points, in pixel, with range [1, infinity)
		uniform float pointSize;

		void main(void)
		{
			gl_Position = projectionMatrix * modelViewMatrix * aVertex;
			gl_PointSize = pointSize;

			vFrontColor = usePerVertexColors == 0 ? color : aColor;
		}
	)SHADER";

const char* GLESProgramManager::programVertexShaderColorId_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Vertex attribute
		in vec4 aVertex;

		// The color id.
		uniform uint colorId;

		// The resulting color id.
		flat out uint vColorId;

		void main(void)
		{
			gl_Position = projectionMatrix * modelViewMatrix * aVertex;

			vColorId = uint(colorId);
		}
	)SHADER";

const char* GLESProgramManager::programVertexShaderPoints_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Vertex attribute
		in vec4 aVertex;

		// 1, to use per-vertex colors; 0, to use the material's colors
		uniform int usePerVertexColors;

		// The static color, in case 'usePerVertexColors == 0'
		uniform OCEAN_LOWP vec4 color;

		// Per-vertex color attribute, in case 'usePerVertexColors == 1'
		in vec4 aColor;

		// Resulting front face color
		out OCEAN_LOWP vec4 vFrontColor;

		// The size of the points, in pixel, with range [1, infinity)
		uniform float pointSize;

		void main(void)
		{
			gl_Position = projectionMatrix * modelViewMatrix * aVertex;
			gl_PointSize = pointSize;

			vFrontColor = usePerVertexColors == 0 ? color : aColor;
		}
	)SHADER";

const char* GLESProgramManager::programVertexShaderPointsMaterial_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Normal matrix which is the inversed and transposed of the upper left 3x3 model view matrix
		uniform mat3 normalMatrix;

		// Global material for all vertices
		uniform Material material;

		// 1, to use per-vertex colors; 0, to use the material's colors
		uniform int usePerVertexColors;

		// Resulting front face color
		out OCEAN_LOWP vec4 vFrontColor;

		// Resulting back face color
		out OCEAN_LOWP vec4 vBackColor;

		// Vertex attribute
		in vec4 aVertex;

		// Normal attribute
		in vec3 aNormal;

		// Color attribute
		in vec4 aColor;

		// The size of the points, in pixel, with range [1, infinity)
		uniform float pointSize;

		// vertex shader main function
		void main()
		{
			// compute vertex position for camera coordinate system
			vec4 vertexInCamera = modelViewMatrix * aVertex;

			// compute vertex position in normalized screen coordinates
			gl_Position = projectionMatrix * vertexInCamera;

			gl_PointSize = pointSize;

			if (usePerVertexColors == 0)
			{
				vFrontColor = material.ambientColor + material.emissiveColor;
				vBackColor = material.ambientColor + material.emissiveColor;
			}
			else
			{
				vFrontColor = vec4(aColor.xyz, material.diffuseColor.a);
				vBackColor = vec4(aColor.xyz, material.diffuseColor.a);
			}
		}
	)SHADER";

const char* GLESProgramManager::programVertexShaderPointsMaterialLight_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Normal matrix which is the inversed and transposed of the upper left 3x3 model view matrix
		uniform mat3 normalMatrix;

		// Flag determining whether to use two sided lighting
		uniform int lightingTwoSided;

		// Global material for all vertices
		uniform Material material;

		// 1, to use per-vertex colors; 0, to use the material's colors
		uniform int usePerVertexColors;

		// Resulting front face color
		out OCEAN_LOWP vec4 vFrontColor;

		// Resulting back face color
		out OCEAN_LOWP vec4 vBackColor;

		// Vertex attribute
		in vec4 aVertex;

		// Normal attribute
		in vec3 aNormal;

		// Color attribute
		in vec4 aColor;

		// The size of the points, in pixel, with range [1, infinity)
		uniform float pointSize;

		// vertex shader main function
		void main()
		{
			// compute vertex position for camera coordinate system
			vec4 vertexInCamera = modelViewMatrix * aVertex;

			// compute vertex position in normalized screen coordinates
			gl_Position = projectionMatrix * vertexInCamera;

			gl_PointSize = pointSize;

			if (numberLights > 0)
			{
				vec4 materialAmbientColor = usePerVertexColors == 0 ? material.ambientColor : aColor;
				vec4 materialDiffuseColor = usePerVertexColors == 0 ? material.diffuseColor : vec4(aColor.xyz, material.diffuseColor.a);

				vec3 normal = normalize(normalMatrix * aNormal);

				vec4 resultingColor;
				vec4 resultingColorSpecular;

				lighting(vertexInCamera, normal, materialAmbientColor, material.emissiveColor, materialDiffuseColor, material.specularColor, material.specularExponent, resultingColor, resultingColorSpecular);

				vFrontColor = resultingColor + resultingColorSpecular;

				if (lightingTwoSided == 1)
				{
					lighting(vertexInCamera, -normal, materialAmbientColor, material.emissiveColor, materialDiffuseColor, material.specularColor, material.specularExponent, resultingColor, resultingColorSpecular);

					vBackColor = resultingColor + resultingColorSpecular;
				}
				else
				{
					vBackColor = vFrontColor;
				}
			}
			else
			{
				vFrontColor = usePerVertexColors == 0 ? material.diffuseColor : vec4(aColor.xyz, material.diffuseColor.a);
				vBackColor = usePerVertexColors == 0 ? material.diffuseColor : vec4(aColor.xyz, material.diffuseColor.a);
			}
		}
	)SHADER";

const char* GLESProgramManager::programVertexShaderDebugGray_ =
	R"SHADER(
		Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Normal matrix which is the inversed and transposed of the upper left 3x3 model view matrix
		uniform mat3 normalMatrix;

		// Vertex attribute
		in vec4 aVertex;

		// Normal attribute
		in vec3 aNormal;

		// Resulting front face color
		out OCEAN_LOWP vec4 vFrontColor;

		void main(void)
		{
			gl_Position = projectionMatrix * modelViewMatrix * aVertex;
			vFrontColor = vec4(0.6, 0.6, 0.6, 1) * dot(vec3(0, 0, 1), normalize(normalMatrix * aNormal));
		}
	)SHADER";

const char* GLESProgramManager::programVertexShaderMaterial_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Normal matrix which is the inversed and transposed of the upper left 3x3 model view matrix
		uniform mat3 normalMatrix;

		// Global material for all vertices
		uniform Material material;

		// 1, to use per-vertex colors; 0, to use the material's colors
		uniform int usePerVertexColors;

		// Resulting front face color
		out OCEAN_LOWP vec4 vFrontColor;

		// Resulting back face color
		out OCEAN_LOWP vec4 vBackColor;

		// Vertex attribute
		in vec4 aVertex;

		// Normal attribute
		in vec3 aNormal;

		// Color attribute
		in vec4 aColor;

		// vertex shader main function
		void main()
		{
			// compute vertex position for camera coordinate system
			vec4 vertexInCamera = modelViewMatrix * aVertex;

			// compute vertex position in normalized screen coordinates
			gl_Position = projectionMatrix * vertexInCamera;

			if (usePerVertexColors == 0)
			{
				vFrontColor = material.ambientColor + material.emissiveColor;
				vBackColor = material.ambientColor + material.emissiveColor;
			}
			else
			{
				vFrontColor = vec4(aColor.xyz, material.diffuseColor.a);
				vBackColor = vec4(aColor.xyz, material.diffuseColor.a);
			}
		}
	)SHADER";

const char* GLESProgramManager::programVertexShaderMaterialLight_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Normal matrix which is the inversed and transposed of the upper left 3x3 model view matrix
		uniform mat3 normalMatrix;

		// Global material for all vertices
		uniform Material material;

		// 1, to use per-vertex colors; 0, to use the material's colors
		uniform int usePerVertexColors;

		// Flag determining whether to use two sided lighting
		uniform int lightingTwoSided;

		// Resulting front face color
		out OCEAN_LOWP vec4 vFrontColor;

		// Resulting back face color
		out OCEAN_LOWP vec4 vBackColor;

		// Vertex attribute
		in vec4 aVertex;

		// Normal attribute
		in vec3 aNormal;

		// Color attribute
		in vec4 aColor;

		// vertex shader main function
		void main()
		{
			// compute vertex position for camera coordinate system
			vec4 vertexInCamera = modelViewMatrix * aVertex;

			// compute vertex position in normalized screen coordinates
			gl_Position = projectionMatrix * vertexInCamera;

			if (numberLights > 0)
			{
				vec4 materialAmbientColor = usePerVertexColors == 0 ? material.ambientColor : aColor;
				vec4 materialDiffuseColor = usePerVertexColors == 0 ? material.diffuseColor : vec4(aColor.xyz, material.diffuseColor.a);

				vec3 normal = normalize(normalMatrix * aNormal);

				vec4 resultingColor;
				vec4 resultingColorSpecular;

				lighting(vertexInCamera, normal, materialAmbientColor, material.emissiveColor, materialDiffuseColor, material.specularColor, material.specularExponent, resultingColor, resultingColorSpecular);

				vFrontColor = resultingColor + resultingColorSpecular;

				if (lightingTwoSided == 1)
				{
					lighting(vertexInCamera, -normal, materialAmbientColor, material.emissiveColor, materialDiffuseColor, material.specularColor, material.specularExponent, resultingColor, resultingColorSpecular);

					vBackColor = resultingColor + resultingColorSpecular;
				}
				else
				{
					vBackColor = vFrontColor;
				}
			}
			else
			{
				vFrontColor = usePerVertexColors == 0 ? material.diffuseColor : vec4(aColor.xyz, material.diffuseColor.a);
				vBackColor = usePerVertexColors == 0 ? material.diffuseColor : vec4(aColor.xyz, material.diffuseColor.a);
			}
		}
	)SHADER";

const char* GLESProgramManager::programVertexShaderTexture_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Texture transform matrix;
		uniform mat4 textureTransformationMatrix;

		// Texture origin in the lower left corner
		uniform int textureOriginLowerLeft;

		// Vertex attribute
		in vec4 aVertex;

		// Texture coordinate attribute
		in vec4 aTextureCoordinate;

		// Resulting texture coordinate
		out OCEAN_LOWP vec2 vTextureCoordinate;

		void main(void)
		{
			gl_Position = projectionMatrix * modelViewMatrix * aVertex;
			vec4 coordinate = textureTransformationMatrix * aTextureCoordinate;

			vTextureCoordinate = (textureOriginLowerLeft == 1) ? coordinate.xy : vec2(coordinate.x, 1.0 - coordinate.y);
		}
	)SHADER";

const char* GLESProgramManager::programVertexShaderMaterialLightTexture_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Normal matrix which is the inversed and transposed of the upper left 3x3 model view matrix
		uniform mat3 normalMatrix;

		// Texture transform matrix;
		uniform mat4 textureTransformationMatrix;

		// Texture origin in the lower left corner
		uniform int textureOriginLowerLeft;

		// Global material for all vertices
		uniform Material material;

		// 1, to use per-vertex colors; 0, to use the material's colors
		uniform int usePerVertexColors;

		// Flag determining whether to use two sided lighting
		uniform int lightingTwoSided;

		// Resulting front face color
		out OCEAN_LOWP vec4 vFrontColor;

		// Resulting front face specular color
		out OCEAN_LOWP vec4 vFrontSpecularColor;

		// Resulting front face color
		out OCEAN_LOWP vec4 vBackColor;

		// Resulting front face specular color
		out OCEAN_LOWP vec4 vBackSpecularColor;

		// Vertex attribute
		in vec4 aVertex;

		// Resulting texture coordinate
		out OCEAN_LOWP vec2 vTextureCoordinate;

		// Normal attribute
		in vec3 aNormal;

		// Color attribute
		in vec4 aColor;

		// Texture coordinate attribute
		in vec4 aTextureCoordinate;

		// vertex shader main function
		void main()
		{
			// compute vertex position for camera coordinate system
			vec4 vertexInCamera = modelViewMatrix * aVertex;

			// compute vertex position in normalized screen coordinates
			gl_Position = projectionMatrix * vertexInCamera;

			if (numberLights > 0)
			{
				vec4 materialAmbientColor = usePerVertexColors == 0 ? material.ambientColor : aColor;
				vec4 materialDiffuseColor = usePerVertexColors == 0 ? material.diffuseColor : vec4(aColor.xyz, material.diffuseColor.a);

				vec3 normal = normalize(normalMatrix * aNormal);

				vec4 resultingColor;
				vec4 resultingColorSpecular;

				lighting(vertexInCamera, normal, materialAmbientColor, material.emissiveColor, materialDiffuseColor, material.specularColor, material.specularExponent, resultingColor, resultingColorSpecular);

				vFrontColor = resultingColor;
				vFrontSpecularColor = resultingColorSpecular;

				if (lightingTwoSided == 1)
				{
					lighting(vertexInCamera, -normal, materialAmbientColor, material.emissiveColor, materialDiffuseColor, material.specularColor, material.specularExponent, resultingColor, resultingColorSpecular);

					vBackColor = resultingColor;
					vBackSpecularColor = resultingColorSpecular;
				}
				else
				{
					vBackColor = vFrontColor;
					vBackSpecularColor = vFrontSpecularColor;
				}
			}
			else
			{
				vFrontColor = usePerVertexColors == 0 ? material.diffuseColor : vec4(aColor.xyz, material.diffuseColor.a);
				vBackColor = usePerVertexColors == 0 ? material.diffuseColor : vec4(aColor.xyz, material.diffuseColor.a);
			}

			vec4 textureCoordinate = textureTransformationMatrix * aTextureCoordinate;
			vTextureCoordinate = textureOriginLowerLeft == 1 ? textureCoordinate.xy : vec2(textureCoordinate.x, 1.0 - textureCoordinate.y);
		}

	)SHADER";

const char* GLESProgramManager::programVertexShaderPhantomVideoTextureCoordinatesFast_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Normalized frustum matrix;
		uniform mat4 textureFrustumMatrix;

		// Texture origin in the lower left corner
		uniform int textureOriginLowerLeft;

		// Vertex attribute
		in vec4 aVertex;
		// Texture coordinate attribute
		in vec4 aTextureCoordinate;

		out OCEAN_LOWP vec4 vTextureCoordinate;

		void main(void)
		{
			gl_Position = projectionMatrix * modelViewMatrix * aVertex;

			vec4 textureCoordinate = textureFrustumMatrix * aTextureCoordinate;
			vTextureCoordinate = (textureOriginLowerLeft == 1) ? textureCoordinate : vec4(textureCoordinate.x, textureCoordinate.a - textureCoordinate.y, textureCoordinate.z, textureCoordinate.a);
		}
	)SHADER";

const char* GLESProgramManager::programVertexShaderOpaqueTextMaterialLight_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Normal matrix which is the inversed and transposed of the upper left 3x3 model view matrix
		uniform mat3 normalMatrix;

		// Global material for the text
		uniform Material material;

		// Global material for the background
		uniform Material backgroundMaterial;

		// Flag determining whether to use two sided lighting
		uniform int lightingTwoSided;

		// Resulting front face color
		out lowp vec4 vTextFrontColor;

		// Resulting back face color
		out lowp vec4 vTextBackColor;

		// Resulting front background color
		out lowp vec4 vBackgroundFrontColor;

		// Resulting back background color
		out lowp vec4 vBackgroundBackColor;

		// Vertex attribute
		in vec4 aVertex;

		// Resulting texture coordinate
		out lowp vec2 vTextureCoordinate;

		// Normal attribute
		in vec3 aNormal;

		// Texture coordinate attribute
		in vec4 aTextureCoordinate;

		// vertex shader main function
		void main()
		{
			// compute vertex position for camera coordinate system
			vec4 vertexInCamera = modelViewMatrix * aVertex;

			// compute vertex position in normalized screen coordinates
			gl_Position = projectionMatrix * vertexInCamera;

			if (numberLights > 0)
			{
				vec3 normal = normalize(normalMatrix * aNormal);

				vec4 resultingColor;
				vec4 resultingColorSpecular;

				lighting(vertexInCamera, normal, material.ambientColor, material.emissiveColor, material.diffuseColor, material.specularColor, material.specularExponent, resultingColor, resultingColorSpecular);
				vTextFrontColor = resultingColor + resultingColorSpecular;

				lighting(vertexInCamera, normal, backgroundMaterial.ambientColor, backgroundMaterial.emissiveColor, backgroundMaterial.diffuseColor, backgroundMaterial.specularColor, backgroundMaterial.specularExponent, resultingColor, resultingColorSpecular);
				vBackgroundFrontColor = resultingColor + resultingColorSpecular;

				if (lightingTwoSided == 1)
				{
					lighting(vertexInCamera, -normal, material.ambientColor, material.emissiveColor, material.diffuseColor, material.specularColor, material.specularExponent, resultingColor, resultingColorSpecular);
					vTextBackColor = resultingColor + resultingColorSpecular;

					lighting(vertexInCamera, -normal, backgroundMaterial.ambientColor, backgroundMaterial.emissiveColor, backgroundMaterial.diffuseColor, backgroundMaterial.specularColor, backgroundMaterial.specularExponent, resultingColor, resultingColorSpecular);
					vBackgroundBackColor = resultingColor + resultingColorSpecular;
				}
				else
				{
					vTextBackColor = vTextFrontColor;
					vBackgroundBackColor = vBackgroundFrontColor;
				}
			}
			else
			{
				vTextFrontColor = material.diffuseColor;
				vTextBackColor = material.diffuseColor;

				vBackgroundFrontColor = backgroundMaterial.diffuseColor;
				vBackgroundBackColor = backgroundMaterial.diffuseColor;
			}

			vTextureCoordinate = vec2(aTextureCoordinate.x, 1.0 - aTextureCoordinate.y);
		}
	)SHADER";


const char* GLESProgramManager::programVertexShaderTransparentTextMaterialLight_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Normal matrix which is the inversed and transposed of the upper left 3x3 model view matrix
		uniform mat3 normalMatrix;

		// Global material for the text
		uniform Material material;

		// Flag determining whether to use two sided lighting
		uniform int lightingTwoSided;

		// Resulting front face color
		out lowp vec4 vTextFrontColor;

		// Resulting back face color
		out lowp vec4 vTextBackColor;

		// Vertex attribute
		in vec4 aVertex;

		// Resulting texture coordinate
		out OCEAN_LOWP vec2 vTextureCoordinate;

		// Normal attribute
		in vec3 aNormal;

		// Texture coordinate attribute
		in vec4 aTextureCoordinate;

		// vertex shader main function
		void main()
		{
			// compute vertex position for camera coordinate system
			vec4 vertexInCamera = modelViewMatrix * aVertex;

			// compute vertex position in normalized screen coordinates
			gl_Position = projectionMatrix * vertexInCamera;

			if (numberLights > 0)
			{
				vec3 normal = normalize(normalMatrix * aNormal);

				vec4 resultingColor;
				vec4 resultingColorSpecular;

				lighting(vertexInCamera, normal, material.ambientColor, material.emissiveColor, material.diffuseColor, material.specularColor, material.specularExponent, resultingColor, resultingColorSpecular);
				vTextFrontColor = resultingColor + resultingColorSpecular;

				if (lightingTwoSided == 1)
				{
					lighting(vertexInCamera, -normal, material.ambientColor, material.emissiveColor, material.diffuseColor, material.specularColor, material.specularExponent, resultingColor, resultingColorSpecular);
					vTextBackColor = resultingColor + resultingColorSpecular;
				}
				else
				{
					vTextBackColor = vTextFrontColor;
				}
			}
			else
			{
				vTextFrontColor = material.diffuseColor;
				vTextBackColor = material.diffuseColor;
			}

			vTextureCoordinate = vec2(aTextureCoordinate.x, 1.0 - aTextureCoordinate.y);
		}
	)SHADER";

const char* GLESProgramManager::programFragmentShaderStaticColor_ =
	R"SHADER(
		// The front face color
		in OCEAN_LOWP vec4 vFrontColor;

		// The out fragment color
		out OCEAN_LOWP vec4 fragColor;

		void main()
		{
			if (gl_FrontFacing)
			{
				fragColor = vFrontColor;
			}
			else
			{
				discard;
			}
		}
	)SHADER";

const char* GLESProgramManager::programFragmentShaderOneSidedColor_ =
	R"SHADER(
		// The front face color
		in OCEAN_LOWP vec4 vFrontColor;

		// The out fragment color
		out OCEAN_LOWP vec4 fragColor;

		void main()
		{
			if (gl_FrontFacing)
			{
				fragColor = vFrontColor;
			}
			else
			{
				discard;
			}
		}
	)SHADER";

const char* GLESProgramManager::programFragmentShaderTwoSidedColor_ =
	R"SHADER(
		// The front face color
		in OCEAN_LOWP vec4 vFrontColor;

		// The back face color
		in OCEAN_LOWP vec4 vBackColor;

		// The out fragment color
		out OCEAN_LOWP vec4 fragColor;

		void main()
		{
			if (gl_FrontFacing)
			{
				fragColor = vFrontColor;
			}
			else
			{
				fragColor = vBackColor;
			}
		}
	)SHADER";

const char* GLESProgramManager::programFragmentShaderColorId_ =
	R"SHADER(
		// The color id for the fragment
		flat in uint vColorId;

		// The out fragment color
		out uint fragColor;

		void main()
		{
			if (gl_FrontFacing)
			{
				fragColor = vColorId;
			}
			else
			{
				discard;
			}
		}
	)SHADER";

const char* GLESProgramManager::programFragmentShaderOneTexture_ =
	R"SHADER(
		// Input texture coordinate
		in OCEAN_LOWP vec2 vTextureCoordinate;

		// Texture sampler object
		uniform sampler2D primaryTexture;

		// The out fragment color
		out OCEAN_LOWP vec4 fragColor;

		void main()
		{
			fragColor = OCEAN_ONE_TEXTURE_LOOKUP_TO_RGBA(primaryTexture, vTextureCoordinate);
		}
	)SHADER";

const char* GLESProgramManager::programFragmentShaderTwoTextures_ =
	R"SHADER(
		// Input texture coordinate
		in OCEAN_LOWP vec2 vTextureCoordinate;

		// Texture sampler object
		uniform sampler2D primaryTexture;
		uniform sampler2D secondaryTexture;

		// The out fragment color
		out OCEAN_LOWP vec4 fragColor;

		void main()
		{
			fragColor = OCEAN_TWO_TEXTURES_LOOKUP_TO_RGBA(primaryTexture, secondaryTexture, vTextureCoordinate);
		}
	)SHADER";

const char* GLESProgramManager::programFragmentShaderMaterialTexture_ =
	R"SHADER(
		// Resulting front face color
		in OCEAN_LOWP vec4 vFrontColor;

		// Resulting back face color
		in OCEAN_LOWP vec4 vBackColor;

		// Resulting front face specular color
		in OCEAN_LOWP vec4 vFrontSpecularColor;

		// Resulting back face specular color
		in OCEAN_LOWP vec4 vBackSpecularColor;

		// Resulting texture coordinate
		in OCEAN_LOWP vec2 vTextureCoordinate;

		// 2D texture sample
		uniform sampler2D primaryTexture;

		// The out fragment color
		out OCEAN_LOWP vec4 fragColor;

		void main()
		{
			if (gl_FrontFacing)
			{
				fragColor = vFrontSpecularColor + vFrontColor * OCEAN_ONE_TEXTURE_LOOKUP_TO_RGBA(primaryTexture, vTextureCoordinate.xy);
			}
			else
			{
				fragColor = vBackSpecularColor + vBackColor * OCEAN_ONE_TEXTURE_LOOKUP_TO_RGBA(primaryTexture, vTextureCoordinate.xy);
			}
		}
	)SHADER";

const char* GLESProgramManager::programFragmentShaderPhantomVideoFastOneTexture_ =
	R"SHADER(
		// Input texture coordinate
		in OCEAN_LOWP vec4 vTextureCoordinate;

		// Texture sampler object
		uniform sampler2D primaryTexture;

		// The out fragment color
		out OCEAN_LOWP vec4 fragColor;

		void main()
		{
			OCEAN_LOWP float invA = 1.0 / vTextureCoordinate.a;
			OCEAN_LOWP vec2 textureCoordinate = vec2(vTextureCoordinate.x, vTextureCoordinate.y) * invA;

			fragColor = OCEAN_ONE_TEXTURE_LOOKUP_TO_RGBA(primaryTexture, textureCoordinate);
		}
	)SHADER";

const char* GLESProgramManager::programFragmentShaderPhantomVideoFastTwoTextures_ =
	R"SHADER(
		// Input texture coordinate
		in OCEAN_LOWP vec4 vTextureCoordinate;

		// Texture sampler object
		uniform sampler2D primaryTexture;
		uniform sampler2D secondaryTexture;

		// The out fragment color
		out OCEAN_LOWP vec4 fragColor;

		void main()
		{
			OCEAN_LOWP float invA = 1.0 / vTextureCoordinate.a;
			OCEAN_LOWP vec2 textureCoordinate = vec2(vTextureCoordinate.x, vTextureCoordinate.y) * invA;

			fragColor = OCEAN_TWO_TEXTURES_LOOKUP_TO_RGBA(primaryTexture, secondaryTexture, textureCoordinate);
		}
	)SHADER";

const char* GLESProgramManager::programFragmentShaderOpaqueTextY_ =
	R"SHADER(
		// Resulting front face color of the actual text
		in OCEAN_LOWP vec4 vTextFrontColor;

		// Resulting back face color of the actual text
		in OCEAN_LOWP vec4 vTextBackColor;

		// Resulting front face color of the background
		in OCEAN_LOWP vec4 vBackgroundFrontColor;

		// Resulting back face color of the background
		in OCEAN_LOWP vec4 vBackgroundBackColor;

		// Resulting texture coordinate
		in OCEAN_LOWP vec2 vTextureCoordinate;

		// 2D texture sample
		uniform sampler2D primaryTexture;

		// The out fragment color
		out lowp vec4 fragColor;

		void main()
		{
			OCEAN_LOWP float textFactor = texture(primaryTexture, vTextureCoordinate.xy).r;
			OCEAN_LOWP float backgroundFactor = 1.0 - textFactor;

			if (gl_FrontFacing)
			{
				fragColor = vTextFrontColor * textFactor + vBackgroundFrontColor * backgroundFactor;
			}
			else
			{
				fragColor = vTextBackColor * textFactor + vBackgroundBackColor * backgroundFactor;
			}

			fragColor.a = vTextFrontColor.a * textFactor + vBackgroundBackColor.a * backgroundFactor;
		}
	)SHADER";

const char* GLESProgramManager::programFragmentShaderTransparentTextY_ =
	R"SHADER(
		// Resulting front face color of the actual text
		in lowp vec4 vTextFrontColor;

		// Resulting back face color of the actual text
		in lowp vec4 vTextBackColor;

		// Resulting texture coordinate
		in lowp vec2 vTextureCoordinate;

		// 2D texture sample
		uniform sampler2D primaryTexture;

		// The out fragment color
		out lowp vec4 fragColor;

		void main()
		{
			OCEAN_LOWP float textFactor = max(0.0, texture(primaryTexture, vTextureCoordinate.xy).r - (1.0 - vTextFrontColor.a));

			if (gl_FrontFacing)
			{
				fragColor = vTextFrontColor * textFactor; // creating a pre-multiplied alpha
			}
			else
			{
				fragColor = vTextBackColor * textFactor;

			}

			fragColor.a = textFactor;
		}
	)SHADER";

GLESProgramManager::GLESProgramManager()
{
#ifdef OCEAN_DEBUG
	debugReleased_ = false;
#endif // OCEAN_DEBUG
}

GLESProgramManager::~GLESProgramManager()
{
	ocean_assert(debugReleased_);
}

GLESShaderProgramRef GLESProgramManager::program(const Engine& engine, const GLESAttribute::ProgramType programType)
{
	ocean_assert(programType != GLESAttribute::PT_UNKNOWN);

	const ScopedLock scopedLock(lock_);

#ifdef OCEAN_DEBUG
	if (debugReleased_)
	{
		Log::warning() << "GLESProgramManager has been released already, framebuffers need to be separated from GL contexts";
		debugReleased_ = false;
	}
#endif

	const ProgramMap::const_iterator i = programMap_.find(programType);
	if (i != programMap_.cend())
	{
		return i->second;
	}

	const ShaderCodes vertexCodes = vertexShaderCodes(programType);
	const ShaderCodes fragmentCodes = fragmentShaderCodes(programType);

	if (vertexCodes.empty() || fragmentCodes.empty())
	{
		return GLESShaderProgramRef();
	}

	GLESShaderRef vertexShader;

	const ShaderMap::const_iterator iV = vertexShaders_.find(vertexCodes);
	if (iV == vertexShaders_.end())
	{
		vertexShader = GLESShaderRef(new GLESShader());

		std::vector<int> lengths;
		lengths.reserve(vertexCodes.size());

		for (const char* vertexCode : vertexCodes)
		{
			lengths.emplace_back(int(strlen(vertexCode)));
		}

		std::string message;
		if (vertexShader->compile(GL_VERTEX_SHADER, vertexCodes, lengths, message) == false)
		{
			Log::error() << "Failed to compile a vertex shader: " << message;
			return GLESShaderProgramRef();
		}

		vertexShaders_[{vertexCodes}] = vertexShader;
	}
	else
	{
		vertexShader = iV->second;
	}

	GLESShaderRef fragmentShader;

	const ShaderMap::const_iterator iF = fragmentShaders_.find({fragmentCodes});
	if (iF == fragmentShaders_.end())
	{
		fragmentShader = GLESShaderRef(new GLESShader());

		std::vector<int> lengths;
		lengths.reserve(fragmentCodes.size());

		for (const char* fragmentCode : fragmentCodes)
		{
			lengths.emplace_back(int(strlen(fragmentCode)));
		}

		std::string message;
		if (fragmentShader->compile(GL_FRAGMENT_SHADER, fragmentCodes, lengths, message) == false)
		{
			Log::error() << "Failed to compile a fragment shader: " << message;
			return GLESShaderProgramRef();
		}

		fragmentShaders_[{fragmentCodes}] = fragmentShader;
	}
	else
	{
		fragmentShader = iF->second;
	}

	GLESShaderProgramRef newProgram = engine.factory().createShaderProgram();
	ocean_assert(newProgram);

	std::string message;
	if (newProgram->link(programType, vertexShader, fragmentShader, message) == false)
	{
		Log::error() << "Failed to link a shader program: " << message;
		return GLESShaderProgramRef();
	}

	ocean_assert(programMap_.find(programType) == programMap_.cend());
	programMap_.emplace(programType, newProgram);

	Log::debug() << "Created shader program: " << GLESAttribute::translateProgramType(programType);

	return newProgram;
}

void GLESProgramManager::release()
{
	const ScopedLock scopedLock(lock_);

	programMap_.clear();
	vertexShaders_.clear();
	fragmentShaders_.clear();

#ifdef OCEAN_DEBUG
	debugReleased_ = true;
#endif
}

GLESProgramManager::ShaderCodes GLESProgramManager::vertexShaderCodes(const GLESAttribute::ProgramType programType) const
{
	switch (uint32_t(programType))
	{
		case GLESAttribute::PT_STATIC_COLOR:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_Y:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_BGRA:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_RGBA:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_YUV24:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_YVU24:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_Y_UV12:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_Y_VU12:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_Y_U_V12:
			return {partPlatform_, programVertexShaderStaticColor_};

		case GLESAttribute::PT_POINTS:
		case GLESAttribute::PT_POINTS | GLESAttribute::PT_STATIC_COLOR:
			return {partPlatform_, programVertexShaderPoints_};

		case GLESAttribute::PT_POINTS | GLESAttribute::PT_MATERIAL:
			return {partPlatform_, partDefinitionMaterial_, programVertexShaderPointsMaterial_};

		case GLESAttribute::PT_POINTS | GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT:
			return {partPlatform_, partDefinitionMaterial_, partDefinitionLight_, partFunctionLighting_, programVertexShaderPointsMaterialLight_};

		case GLESAttribute::PI_DEBUG_GRAY:
			return {partPlatform_, programVertexShaderDebugGray_};

		case GLESAttribute::PT_MATERIAL:
			return {partPlatform_, partDefinitionMaterial_, programVertexShaderMaterial_};

		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT:
			return {partPlatform_, partDefinitionMaterial_, partDefinitionLight_, partFunctionLighting_, programVertexShaderMaterialLight_};

		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_Y:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_RGBA:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_BGRA:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_YUV24:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_YVU24:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_Y_UV12:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_Y_VU12:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_Y_U_V12:
		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y:
		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_RGBA:
		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_BGRA:
		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_YUV24:
		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_YVU24:
		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y_UV12:
		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y_VU12:
		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y_U_V12:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_Y:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_RGBA:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_BGRA:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_YUV24:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_YVU24:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_Y_UV12:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_Y_VU12:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_Y_U_V12:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_RGBA:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_BGRA:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_YUV24:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_YVU24:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y_UV12:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y_VU12:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y_U_V12:
			return {partPlatform_, programVertexShaderTexture_};

		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT | GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT | GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_RGBA:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT | GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_BGRA:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT | GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_RGBA:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT | GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_BGRA:
			return {partPlatform_, partDefinitionMaterial_, partDefinitionLight_, partFunctionLighting_, programVertexShaderMaterialLightTexture_};

		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_Y:
		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_BGRA:
		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_RGBA:
		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_YVU24:
		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_YUV24:
		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_Y_UV12:
		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_Y_VU12:
		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_Y_U_V12:
			return {partPlatform_, programVertexShaderPhantomVideoTextureCoordinatesFast_};

		default:
			break;
	}

	if (programType & GLESAttribute::PT_TEXT_OPAQUE_Y)
	{
		return {partPlatform_, partDefinitionMaterial_, partDefinitionLight_, partFunctionLighting_, programVertexShaderOpaqueTextMaterialLight_};
	}

	if (programType & GLESAttribute::PT_TEXT_TRANSPARENT_Y)
	{
		return {partPlatform_, partDefinitionMaterial_, partDefinitionLight_, partFunctionLighting_, programVertexShaderTransparentTextMaterialLight_};
	}

	if (programType & GLESAttribute::PT_COLOR_ID)
	{
		return {partPlatform_, programVertexShaderColorId_};
	}

	Log::debug() << "Invalid vertex shader: " << GLESAttribute::translateProgramType(programType);
	ocean_assert(false && "Undefined shader.");

	return ShaderCodes();
}

GLESProgramManager::ShaderCodes GLESProgramManager::fragmentShaderCodes(const GLESAttribute::ProgramType programType) const
{
	switch (uint32_t(programType))
	{
		case GLESAttribute::PT_STATIC_COLOR:
		case GLESAttribute::PT_POINTS:
		case GLESAttribute::PT_STATIC_COLOR | GLESAttribute::PT_POINTS:
			return {partPlatform_, programFragmentShaderStaticColor_};

		case GLESAttribute::PI_DEBUG_GRAY:
			return {partPlatform_, programFragmentShaderOneSidedColor_};

		case GLESAttribute::PT_MATERIAL:
		case GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT:
		case GLESAttribute::PT_POINTS | GLESAttribute::PT_MATERIAL:
		case GLESAttribute::PT_POINTS | GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT:
			return {partPlatform_, programFragmentShaderTwoSidedColor_};

		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_Y:
			return {partPlatform_, partOneTextureLookupY8ToRGBA32_, programFragmentShaderOneTexture_};

		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_BGRA:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_BGRA:
			return {partPlatform_, partOneTextureLookupBGRA32ToRGBA32_, programFragmentShaderOneTexture_};

		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_RGBA:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_RGBA:
			return {partPlatform_, partOneTextureLookupRGBA32ToRGBA32_, programFragmentShaderOneTexture_};

		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_YUV24:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_YUV24:
			return {partPlatform_, partOneTextureLookupYUV24ToRGBA32_, programFragmentShaderOneTexture_};

		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_YVU24:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_YVU24:
			return {partPlatform_, partOneTextureLookupYVU24ToRGBA32_, programFragmentShaderOneTexture_};

		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y_VU12:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_Y_VU12:
			return {partPlatform_, partTwoTexturesLookupY_VU12ToRGBA32_, programFragmentShaderTwoTextures_};

		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y_U_V12:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_Y_U_V12:
			return {partPlatform_, partTwoTexturesLookupY_U_V12ToRGBA32_, programFragmentShaderTwoTextures_};

		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y_UV12:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_Y_UV12:
			return {partPlatform_, partTwoTexturesLookupY_UV12ToRGBA32_, programFragmentShaderTwoTextures_};

		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_Y | GLESAttribute::PT_MATERIAL:
		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y | GLESAttribute::PT_MATERIAL:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_Y | GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT:
		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_Y | GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT:
			return {partPlatform_, partOneTextureLookupY8ToRGBA32_, programFragmentShaderMaterialTexture_};

		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_BGRA | GLESAttribute::PT_MATERIAL:
		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_BGRA | GLESAttribute::PT_MATERIAL:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_BGRA | GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT:
		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_BGRA | GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT:
			return {partPlatform_, partOneTextureLookupBGRA32ToRGBA32_, programFragmentShaderMaterialTexture_};

		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_RGBA | GLESAttribute::PT_MATERIAL:
		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_RGBA | GLESAttribute::PT_MATERIAL:
		case GLESAttribute::PT_TEXTURE_LOWER_LEFT | GLESAttribute::PT_TEXTURE_RGBA | GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT:
		case GLESAttribute::PT_TEXTURE_UPPER_LEFT | GLESAttribute::PT_TEXTURE_RGBA | GLESAttribute::PT_MATERIAL | GLESAttribute::PT_LIGHT:
			return {partPlatform_, partOneTextureLookupRGBA32ToRGBA32_, programFragmentShaderMaterialTexture_};

		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_Y:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_BGRA:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_RGBA:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_YUV24:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_YVU24:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_Y_UV12:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_Y_VU12:
		case GLESAttribute::PT_PHANTOM_VIDEO_FAST | GLESAttribute::PT_TEXTURE_Y_U_V12:
			return {partPlatform_, programFragmentShaderStaticColor_};

		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_Y:
			return {partPlatform_, partOneTextureLookupY8ToRGBA32_, programFragmentShaderPhantomVideoFastOneTexture_};

		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_BGRA:
			return {partPlatform_, partOneTextureLookupBGRA32ToRGBA32_, programFragmentShaderPhantomVideoFastOneTexture_};

		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_RGBA:
			return {partPlatform_, partOneTextureLookupRGBA32ToRGBA32_, programFragmentShaderPhantomVideoFastOneTexture_};

		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_Y_UV12:
			return {partPlatform_, partTwoTexturesLookupY_UV12ToRGBA32_, programFragmentShaderPhantomVideoFastTwoTextures_};

		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_Y_VU12:
			return {partPlatform_, partTwoTexturesLookupY_VU12ToRGBA32_, programFragmentShaderPhantomVideoFastTwoTextures_};

		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_YUV24:
			return {partPlatform_, partOneTextureLookupYUV24ToRGBA32_, programFragmentShaderPhantomVideoFastOneTexture_};

		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_YVU24:
			return {partPlatform_, partOneTextureLookupYVU24ToRGBA32_, programFragmentShaderPhantomVideoFastOneTexture_};

		case GLESAttribute::PT_PHANTOM_VIDEO_TEXTURE_COORDINATES_FAST | GLESAttribute::PT_TEXTURE_Y_U_V12:
			return {partPlatform_, partTwoTexturesLookupY_U_V12ToRGBA32_, programFragmentShaderPhantomVideoFastTwoTextures_};

		default:
			break;
	}

	if (programType & GLESAttribute::PT_TEXT_OPAQUE_Y)
	{
		return {partPlatform_, programFragmentShaderOpaqueTextY_};
	}

	if (programType & GLESAttribute::PT_TEXT_TRANSPARENT_Y)
	{
		return {partPlatform_, programFragmentShaderTransparentTextY_};
	}

	if (programType & GLESAttribute::PT_COLOR_ID)
	{
		return {partPlatform_, programFragmentShaderColorId_};
	}

	Log::debug() << "Invalid fragment shader: " << GLESAttribute::translateProgramType(programType);
	ocean_assert(false && "Undefined shader.");

	return ShaderCodes();
}

} // namespace GLESceneGraph

} // namespace Rendering

} // namespace Ocean
