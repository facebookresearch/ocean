/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_OBJECT_H
#define META_OCEAN_RENDERING_GLES_OBJECT_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESDynamicLibrary.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/RGBAColor.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/SquareMatrix4.h"
#include "ocean/math/Vector3.h"
#include "ocean/math/Vector4.h"

#include "ocean/rendering/Object.h"

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
	#include <OpenGLES/ES3/gl.h>
#elif defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS)
	#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl3.h>
#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
	#include <GLES3/gl32.h>
#else
	#define GL_GLEXT_PROTOTYPES

	#if defined(OCEAN_PLATFORM_BUILD_LINUX)
		#include <GL/gl.h>
		#include <GL/glext.h>
	#else
		#include <gl/GL.h>
		#include <gl/glext.h>
	#endif
#endif

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class is the base class for all GLESceneGraph objects.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESObject : virtual public Object
{
	public:

		/**
		 * Returns the name of the owner engine.
		 * @see Object::engineId().
		 */
		const std::string& engineName() const override;

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const int32_t value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const uint32_t value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const float value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const double value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const RGBAColor& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const SquareMatrixF3& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const SquareMatrixD3& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const SquareMatrixF4& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const SquareMatrixD4& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const HomogenousMatrixF4& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const HomogenousMatrixD4& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const VectorF2& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const VectorD2& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const VectorF3& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const VectorD3& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const VectorF4& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const VectorD4& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const SquareMatricesT3<float>& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const SquareMatricesT3<double>& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const SquareMatricesT4<float>& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const  SquareMatricesT4<double>& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const HomogenousMatricesF4& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const HomogenousMatricesD4& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const VectorsF2& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const VectorsD2& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const VectorsF3& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const VectorsD3& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const VectorsF4& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const VectorsD4& value);

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param values The values to be set, must be valid
		 * @param size The number of elements, with range [1, infinity)
		 */
		static void setUniform(const GLint location, const float* values, const size_t size);

	protected:

		/**
		 * Creates a new GLESceneGraph object.
		 */
		GLESObject();

		/**
		 * Destructs a GLESceneGraph object.
		 */
		~GLESObject() override;

	private:

		/**
		 * Sets the value of a uniform variable for the current (currently bound) program object.
		 * @param location The location of the uniform variable to be modified, must be valid
		 * @param value The value to be set
		 */
		static void setUniform(const GLint location, const bool value) = delete;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_OBJECT_H
