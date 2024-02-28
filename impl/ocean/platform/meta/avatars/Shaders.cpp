// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/avatars/Shaders.h"

#include "ocean/math/MathUtilities.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

const char* Shaders::partPlatform_ =
	// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
	R"SHADER(#version 300 es

		#define OCEAN_LOWP lowp
		#define OCEAN_HIGHP highp
	)SHADER";

const char* Shaders::partDefinitionLight_ =
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

const char* Shaders::partFunctionLighting_ =
	R"SHADER(
		// Lights used for lighting
		uniform Light lights[8];

		// The number of defined lights, with range [0, 8]
		uniform int numberLights;

		void lighting(in OCEAN_LOWP vec4 vertexInCamera, in OCEAN_LOWP vec3 normal, in OCEAN_LOWP vec4 materialAmbientColor, in OCEAN_LOWP vec4 materialEmissiveColor, in OCEAN_LOWP vec4 materialDiffuseColor, in OCEAN_LOWP vec4 materialSpecularColor, in OCEAN_LOWP float materialSpecularExponent, out OCEAN_LOWP vec4 resultingColor, out OCEAN_LOWP vec4 resultingColorSpecular)
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

				OCEAN_LOWP vec4 localResultingColor = vec4(0, 0, 0, 0);

				OCEAN_LOWP vec3 lightVector;
				OCEAN_LOWP float attenuationFactor = 1.0;

				// if this light is a point or spot light)
				if (light.positionOrDirection.w != 0.0)
				{
					// vector from vertex to light
					lightVector = light.positionOrDirection.xyz - vertexInCamera.xyz;

					// check if the attenuation factor has to be calculated
					if (light.attenuationIsUsed == 1)
					{
						// distance vector (1, d, d^2)
						OCEAN_LOWP vec3 distances;
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
						OCEAN_LOWP float spotFactor = dot(-lightVector, light.spotDirection);

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

					OCEAN_LOWP vec3 viewDirection = -vertexInCamera.xyz; // (0, 0, 0) - vertexInCamera.xyz

					// Compute half vector
					OCEAN_LOWP vec3 hVector = normalize(lightVector + viewDirection);

					OCEAN_LOWP float normalDotHVector = dot(normal, hVector);

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

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

const char* Shaders::partSkinMatrixLookup_ =
	R"SHADER(
		mat4 vertexMatrixFromTexture(sampler2D sampler, uint matrixIndex)
		{
			const OCEAN_HIGHP float minValue = -50.0;
			const OCEAN_HIGHP float maxValue = 50.0;
			const OCEAN_HIGHP float valueRange = maxValue - minValue;

			const OCEAN_HIGHP vec4 invNormalization = valueRange / vec4(1.0, 255.0, 65025.0, 16581375.0);

			OCEAN_HIGHP float y = (float(matrixIndex) + 0.5) / 256.0;

			OCEAN_HIGHP vec4 columns[4];

			for (int n = 0; n < 4; ++n)
			{
				OCEAN_HIGHP float x0 = (0.5 + float(n) * 4.0 + 0.0) / 32.0;
				OCEAN_HIGHP float x1 = (0.5 + float(n) * 4.0 + 1.0) / 32.0;
				OCEAN_HIGHP float x2 = (0.5 + float(n) * 4.0 + 2.0) / 32.0;
				OCEAN_HIGHP float x3 = (0.5 + float(n) * 4.0 + 3.0) / 32.0;

				OCEAN_HIGHP vec4 value0 = texture(sampler, vec2(x0, y));
				OCEAN_HIGHP vec4 value1 = texture(sampler, vec2(x1, y));
				OCEAN_HIGHP vec4 value2 = texture(sampler, vec2(x2, y));
				OCEAN_HIGHP vec4 value3 = texture(sampler, vec2(x3, y));

				columns[n] = vec4(minValue, minValue, minValue, minValue) + vec4(dot(value0, invNormalization), dot(value1, invNormalization), dot(value2, invNormalization), dot(value3, invNormalization));
			}

			return mat4(columns[0], columns[1], columns[2], columns[3]);
		}

		mat4 normalMatrixFromTexture(sampler2D sampler, uint matrixIndex)
		{
			const OCEAN_HIGHP float minValue = -50.0;
			const OCEAN_HIGHP float maxValue = 50.0;
			const OCEAN_HIGHP float valueRange = maxValue - minValue;

			const OCEAN_HIGHP vec4 invNormalization = valueRange / vec4(1.0, 255.0, 65025.0, 16581375.0);

			OCEAN_HIGHP float y = (float(matrixIndex) + 0.5) / 256.0;

			OCEAN_HIGHP vec4 columns[4];

			for (int n = 0; n < 4; ++n)
			{
				OCEAN_HIGHP float x0 = (16.5 + float(n) * 4.0 + 0.0) / 32.0;
				OCEAN_HIGHP float x1 = (16.5 + float(n) * 4.0 + 1.0) / 32.0;
				OCEAN_HIGHP float x2 = (16.5 + float(n) * 4.0 + 2.0) / 32.0;
				OCEAN_HIGHP float x3 = (16.5 + float(n) * 4.0 + 3.0) / 32.0;

				OCEAN_HIGHP vec4 value0 = texture(sampler, vec2(x0, y));
				OCEAN_HIGHP vec4 value1 = texture(sampler, vec2(x1, y));
				OCEAN_HIGHP vec4 value2 = texture(sampler, vec2(x2, y));
				OCEAN_HIGHP vec4 value3 = texture(sampler, vec2(x3, y));

				columns[n] = vec4(minValue, minValue, minValue, minValue) + vec4(dot(value0, invNormalization), dot(value1, invNormalization), dot(value2, invNormalization), dot(value3, invNormalization));
			}

			return mat4(columns[0], columns[1], columns[2], columns[3]);
		}
	)SHADER";

#else // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

const char* Shaders::partSkinMatrixLookup_ =
	R"SHADER(
		mat4 vertexMatrixFromTexture(sampler2D sampler, uint matrixIndex)
		{
			const OCEAN_LOWP float x0 = (0.5 + 0.0) / 8.0;
			const OCEAN_LOWP float x1 = (0.5 + 1.0) / 8.0;
			const OCEAN_LOWP float x2 = (0.5 + 2.0) / 8.0;
			const OCEAN_LOWP float x3 = (0.5 + 3.0) / 8.0;

			OCEAN_LOWP float y = (float(matrixIndex) + 0.5) / 256.0;

			OCEAN_LOWP vec4 column0 = texture(sampler, vec2(x0, y));
			OCEAN_LOWP vec4 column1 = texture(sampler, vec2(x1, y));
			OCEAN_LOWP vec4 column2 = texture(sampler, vec2(x2, y));
			OCEAN_LOWP vec4 column3 = texture(sampler, vec2(x3, y));

			return mat4(column0, column1, column2, column3);
		}

		mat4 normalMatrixFromTexture(sampler2D sampler, uint matrixIndex)
		{
			const OCEAN_LOWP float x0 = (0.5 + 4.0) / 8.0;
			const OCEAN_LOWP float x1 = (0.5 + 5.0) / 8.0;
			const OCEAN_LOWP float x2 = (0.5 + 6.0) / 8.0;
			const OCEAN_LOWP float x3 = (0.5 + 7.0) / 8.0;

			OCEAN_LOWP float y = (float(matrixIndex) + 0.5) / 256.0;

			OCEAN_LOWP vec4 column0 = texture(sampler, vec2(x0, y));
			OCEAN_LOWP vec4 column1 = texture(sampler, vec2(x1, y));
			OCEAN_LOWP vec4 column2 = texture(sampler, vec2(x2, y));
			OCEAN_LOWP vec4 column3 = texture(sampler, vec2(x3, y));

			return mat4(column0, column1, column2, column3);
		}
	)SHADER";

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

const char* Shaders::partVertexShaderTexture_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Normal matrix which is the inversed and transposed of the upper left 3x3 model view matrix
		uniform mat3 normalMatrix;

		/// The vertex and normal skin matrices
		uniform sampler2D skinMatricesTexture;

		// Vertex attribute
		in OCEAN_LOWP vec4 aVertex;

		// Normal attribute
		in OCEAN_LOWP vec3 aNormal;

		// Texture coordinate attribute
		in OCEAN_LOWP vec4 aTextureCoordinate;

		in uvec4 aJointIndices;
		in OCEAN_LOWP vec4 aJointWeights;

		// Resulting texture coordinate
		out OCEAN_LOWP vec2 vTextureCoordinate;

		out OCEAN_LOWP vec4 vVertexInCamera;

		out OCEAN_LOWP vec3 vNormal;

		void main(void)
		{
			mat4 vertexSkinMatrix = vertexMatrixFromTexture(skinMatricesTexture, aJointIndices.x) * aJointWeights.x
										+ vertexMatrixFromTexture(skinMatricesTexture, aJointIndices.y) * aJointWeights.y
										+ vertexMatrixFromTexture(skinMatricesTexture, aJointIndices.z) * aJointWeights.z
										+ vertexMatrixFromTexture(skinMatricesTexture, aJointIndices.w) * aJointWeights.w;

			mat4 normalSkinMatrix = normalMatrixFromTexture(skinMatricesTexture, aJointIndices.x) * aJointWeights.x
										+ normalMatrixFromTexture(skinMatricesTexture, aJointIndices.y) * aJointWeights.y
										+ normalMatrixFromTexture(skinMatricesTexture, aJointIndices.z) * aJointWeights.z
										+ normalMatrixFromTexture(skinMatricesTexture, aJointIndices.w) * aJointWeights.w;


			OCEAN_LOWP vec4 vertex = vertexSkinMatrix * aVertex;
			OCEAN_LOWP vec3 normal = mat3(normalSkinMatrix) * aNormal;

			gl_Position = projectionMatrix * modelViewMatrix * vertex;

			vVertexInCamera = modelViewMatrix * vertex;
			vNormal = normalize(normalMatrix * normal);

			OCEAN_LOWP vec4 coordinate = aTextureCoordinate;

			vTextureCoordinate = coordinate.xy;
		}
	)SHADER";

const char* Shaders::partFragmentShader_ =
	R"SHADER(
		// Input texture coordinate
		in OCEAN_LOWP vec2 vTextureCoordinate;

		in OCEAN_LOWP vec4 vVertexInCamera;

		in OCEAN_LOWP vec3 vNormal;

		// Texture sampler object
		uniform sampler2D baseTexture;
		uniform sampler2D metallicRougnessTexture;
		uniform sampler2D occlusionTexture;

		// The out fragment color
		out OCEAN_LOWP vec4 fragColor;

		const OCEAN_LOWP float gamma = 2.2;
		const OCEAN_LOWP float invGamma = 1.0 / gamma;
		OCEAN_LOWP vec4 srgbToLinear(OCEAN_LOWP vec4 srgbColor)
		{
			// alpha is always linear
			return vec4(pow(srgbColor.rgb, vec3(invGamma)), srgbColor.a);
		}

		void main()
		{
			OCEAN_LOWP vec4 baseColor = srgbToLinear(texture(baseTexture, vTextureCoordinate).rgba);
			OCEAN_LOWP vec4 metalicRougnessColor = texture(metallicRougnessTexture, vTextureCoordinate).rgba;

			OCEAN_LOWP float roughness = clamp(metalicRougnessColor.g, 0.0, 1.0);
			OCEAN_LOWP float metallic = metalicRougnessColor.b;

			OCEAN_LOWP vec3 f0 = vec3(0.04);
			OCEAN_LOWP vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0) * (1.0 - metallic);
			OCEAN_LOWP vec3 specularColor = mix(f0, baseColor.rgb, metallic);

			OCEAN_LOWP vec4 materialAmbientColor = vec4(0.0, 0.0, 0.0, 0.0);
			OCEAN_LOWP vec4 materialEmissiveColor = vec4(0.0, 0.0, 0.0, 0.0);
			OCEAN_LOWP vec4 materialDiffuseColor = vec4(diffuseColor, 1.0);
			OCEAN_LOWP vec4 materialSpecularColor = vec4(specularColor, 1.0);
			OCEAN_LOWP float materialSpecularExponent = 1.0;

			OCEAN_LOWP vec4 resultingColor;
			OCEAN_LOWP vec4 resultingColorSpecular;
			lighting(vVertexInCamera, vNormal, materialAmbientColor, materialEmissiveColor, materialDiffuseColor, materialSpecularColor, materialSpecularExponent, resultingColor, resultingColorSpecular);

			OCEAN_LOWP vec3 color = resultingColor.rgb + resultingColorSpecular.rgb;

			fragColor = vec4(color, 1.0);
		}
	)SHADER";

Rendering::ShaderProgramRef Shaders::shaderProgram(const Rendering::Engine& engine)
{
	if (shaderProgram_)
	{
		return shaderProgram_;
	}

	shaderProgram_ = engine.factory().createShaderProgram();
	if (shaderProgram_.isNull())
	{
		ocean_assert(false && "This should never happen!");
		return Rendering::ShaderProgramRef();
	}

	const std::vector<const char*> vertexShaderCode = {partPlatform_, partSkinMatrixLookup_, partVertexShaderTexture_};
	const std::vector<const char*> fragmentShaderCode = {partPlatform_, partDefinitionLight_, partFunctionLighting_, partFragmentShader_};

	std::string errorMessage;
	if (!shaderProgram_->setShader(Rendering::ShaderProgram::SL_GLSL, vertexShaderCode, fragmentShaderCode, errorMessage))
	{
		Log::error() << "Failed to create shader: " << errorMessage;

		shaderProgram_.release();
		return Rendering::ShaderProgramRef();
	}

	return shaderProgram_;
}

void Shaders::release()
{
	shaderProgram_.release();
}

bool Shaders::updateSkinMatricesTexture(Rendering::FrameTexture2D& frameTexture, const HomogenousMatricesF4& skinMatrices)
{
	if (skinMatrices.empty() || skinMatrices.size() % 2 != 0 || skinMatrices.size() > 256 * 2)
	{
		return false;
	}

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	// iOS does not support float textures, thus we need to compose floats as uint8

	Frame vertexSkinMatricesFrame(FrameType(8u * 4u, 256u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT));

	constexpr float rangeMin = -50.0f; // needs to match value in corresponding shader 'partSkinMatrixLookup_'
	constexpr float rangeMax = 50.0f;
	constexpr float invRange = 1.0f / (rangeMax - rangeMin);
	static_assert(invRange > 0.0f, "Invalid range");

	for (size_t nMatrix = 0; nMatrix < skinMatrices.size(); ++nMatrix)
	{
		const HomogenousMatrixF4& skinMatrix = skinMatrices[nMatrix];

		uint8_t* textureRow = vertexSkinMatricesFrame.row<uint8_t>(nMatrix / 2);

		if (nMatrix % 2 != 0)
		{
			textureRow += 4 * 16;
		}

		for (unsigned int n = 0u; n < 16u; ++n)
		{
			const float value = minmax<float>(0.0f, (skinMatrix.data()[n] - rangeMin) * invRange, 1.0f);

			MathUtilities::encodeFloatToUint8(value, textureRow);
			textureRow += 4;
		}
	}

#else

	Frame vertexSkinMatricesFrame(FrameType(8u, 256u, FrameType::genericPixelFormat<float, 4u>(), FrameType::ORIGIN_UPPER_LEFT));
	memcpy(vertexSkinMatricesFrame.data<void>(), skinMatrices.data(), skinMatrices.size() * sizeof(HomogenousMatrixF4));

#endif

	frameTexture.setTexture(std::move(vertexSkinMatricesFrame));

	return true;
}

}

}

}

}
