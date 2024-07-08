/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_VERTEX_SET_H
#define META_OCEAN_RENDERING_GLES_VERTEX_SET_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESObject.h"
#include "ocean/rendering/glescenegraph/GLESShaderProgram.h"

#include "ocean/math/BoundingBox.h"

#include "ocean/rendering/TriangleFace.h"
#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph vertex set object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESVertexSet :
	virtual public GLESObject,
	virtual public VertexSet
{
	public:

		/**
		 * This class is the base class for all VertexBufferObjects
		 */
		class VertexBufferObject
		{
			public:

				/**
 				 * Destructs a vertex buffer object and releases all resources.
 				 */
				virtual ~VertexBufferObject() = default;

				/**
				 * Binds a vertex buffer object to a given program.
				 * @param programId The id of the program to which this object will be bound.
				 */
				virtual void bind(const GLuint programId) const = 0;
		};

		/**
		 * This class implements the wrapper around a vertex buffer object.
		 * @tparam T The data type of the elements the vertex buffer object is holding
		 */
		template <typename T>
		class VertexBufferObjectT : public VertexBufferObject
		{
			public:

				/**
				 * Creates a new vertex buffer object.
				 * @param attributeName The attribute name to which this vertex buffer will be bound
				 */
				explicit VertexBufferObjectT(std::string attributeName);

				/**
				 * Move constructor.
				 * @param vertexBufferObject Vertex buffer object to be moved
				 */
				VertexBufferObjectT(VertexBufferObjectT<T>&& vertexBufferObject);

				/**
				 * Destructs the object and releases all resources.
				 */
				~VertexBufferObjectT() override // need to be defined here in the class as Clang will complain otherwise
				{
					release();
				}

				/**
				 * Sets the data of this buffer.
				 * @param elements The elements to be set, must be valid
				 * @param numberElements The number of elements to set, with range [1, infinity)
				 * @param usage Specifies the expected usage pattern of the data store
				 */
				void setData(const T* elements, const size_t numberElements, const GLenum usage = GL_STATIC_DRAW);

				/**
				 * Binds this vertex buffer to a specified program.
				 * @param programId The id of the program to which the buffer will be bound
				 * @param attributeName Optional explicit attribute name to be used instead of the object's attribute name; nullptr to use the object's attribute name
				 */
				void bind(const GLuint programId, const char* attributeName) const;

				/**
				 * Binds a vertex buffer object to a given program.
				 * @see VertexBufferObject::bind().
				 */
				void bind(const GLuint programId) const override;

				/**
				 * Returns the number of elements the object holds.
				 * @return The object's elements
				 */
				inline unsigned int numberElements() const;

				/**
				 * Explicitly releases the vertex buffer object and all resources.
				 */
				void release();

			protected:

				/**
				 * Disabled copy constructor.
				 * @param vertexBufferObject The vertex buffer object which would be copied
				 */
				VertexBufferObjectT(const VertexBufferObjectT& vertexBufferObject) = delete;

			protected:

				/// The attribute name associated with this buffer object.
				std::string attributeName_;

				/// The buffer object.
				GLuint buffer_ = 0u;

				/// The number of elements stored in the buffer object.
				unsigned int numberElements_ = 0u;
		};

	protected:

		typedef std::unordered_map<std::string, std::shared_ptr<VertexBufferObject>> VertexBufferObjectMap;

		/**
		 * Returns the number of components a data type has.
		 * @return The data type's number of components
		 * @tparam T The data type
		 */
		template <typename T>
		static constexpr size_t numberComponents();

		/**
		 * Returns the type of the components of a data type.
		 * @return The data type's component type
		 * @tparam T The data type
		 */
		template <typename T>
		static constexpr GLenum componentType();

		/**
		 * Returns whether the component is float component.
		 * @return True, if so; False, if the component is an integer component
		 * @tparam T The data type
		 */
		template <typename T>
		static constexpr bool isFloatComponent();

	public:

		/**
		 * Creates a new GLESceneGraph vertex set object.
		 */
		GLESVertexSet();

		/**
		 * Destructs a GLESceneGraph vertex set object.
		 */
		~GLESVertexSet() override;

		/**
		 * Returns the normals of this set.
		 * @see VertexSet::normals().
		 */
		Normals normals() const override;

		/**
		 * Returns the texture coordinates of this set.
		 * @see VertexSet::textureCoordinates().
		 */
		TextureCoordinates textureCoordinates(const unsigned int layerIndex) const override;

		/**
		 * Returns the explicit reference coordinate system of the phantom object, if any.
		 * @return The name of the reference coordinate system
		 */
		std::string phantomTextureCoordinateSystem() const override;

		/**
		 * Returns the vertices of this set.
		 * @see VertexSet::vertices().
		 */
		Vertices vertices() const override;

		/**
		 * Returns the colors of this set.
		 * @see VertexSet::colors();
		 */
		RGBAColors colors() const override;

		/**
		 * Returns the number of normals of this set.
		 * @see VertexSet::numberNormals().
		 */
		unsigned int numberNormals() const override;

		/**
		 * Returns the number of texture coordinates of this set.
		 * @see VertexSet::numberTextureCoordinates().
		 */
		unsigned int numberTextureCoordinates(const unsigned int layerIndex) const override;

		/**
		 * Returns the number of vertices of this set.
		 * @see VertexSet::numberVertices().
		 */
		unsigned int numberVertices() const override;

		/**
		 * Returns the number of colors of this set.
		 * @see VertexSet::numberColors().
		 */
		unsigned int numberColors() const override;

		/**
		 * Sets the normals for this set.
		 * @see VertexSet::setNormals().
		 */
		void setNormals(const Normals& normals) override;

		/**
		 * Sets the normals for this set.
		 * @see VertexSet::setNormals().
		 */
		void setNormals(const Vector3* normals, const size_t size) override;

		/**
		 * Sets the texels for this set.
		 * @see VertexSet::setTextureCoordinates().
		 */
		void setTextureCoordinates(const TextureCoordinates& textureCoordinates, const unsigned int layerIndex) override;

		/**
		 * Sets 3D texels for this set used for phantom objects.
		 * @see VertexSet::setPhantomTextureCoordinates().
		 */
		void setPhantomTextureCoordinates(const Vertices& textureCoordinates, const unsigned int layerIndex) override;

		/**
		 * Sets the explicit reference coordinate system of the phantom objects.
		 * @see VertexSet::setPhantomTextureCoordinateSystem().
		 */
		void setPhantomTextureCoordinateSystem(const std::string& reference) override;

		/**
		 * Sets the vertices for this set.
		 * @see VertexSet::setVertices().
		 */
		void setVertices(const Vertices& vertices) override;

		/**
		 * Sets the vertices for this set.
		 * @see VertexSet::setVertices().
		 */
		void setVertices(const Vector3* vertices, const size_t size) override;

		/**
		 * Sets the colors for this set.
		 * @see VertexSet::setColors().
		 */
		void setColors(const RGBAColors& colors) override;

		/**
		 * Sets several attributes concurrently.
		 * @see VertexSet::set().
		 */
		void set(const Vertices& vertices, const Normals& normals, const TextureCoordinates& textureCoordinates, const RGBAColors& colors) override;

		/**
		 * Sets a custom VertexSet attribute in addition to the standard attributes like e.g., vertices, normals, textureCoordinates etc.
		 * @param attributeName The name of the attribute, must be valid
		 * @param elements The elements of the attribute to set, must be valid
		 * @param numberElements The number of elements to set, with range [1, infinity)
		 * @tparam T The data type of the elements of the attribute
		 */
		template <typename T>
		void setAttribute(const std::string& attributeName, const T* elements, const size_t numberElements);

		/**
		 * Binds the vertex set to a program.
		 * @param programId The id of the program to which the buffer will be bound
		 */
		void bindVertexSet(const GLuint programId);

		/**
		 * Determines the bounding box of this vertex set.
		 * @param vertexIndices The indices of the vertices for which the bounding box will be determined, can be empty
		 * @return The resulting bounding box
		 */
		BoundingBox boundingBox(const VertexIndices& vertexIndices) const;

		/**
		 * Determines the bounding box of this vertex set.
		 * @param triangleFaces The triangle faces of the vertices for which the bounding box will be determined, can be empty
		 * @return The resulting bounding box
		 */
		BoundingBox boundingBox(const TriangleFaces& triangleFaces) const;

		/**
		 * Determines the bounding box of this vertex set.
		 * @param strips The triangle strips of the vertices for which the bounding box will be determined, can be empty
		 * @return The resulting bounding box
		 */
		BoundingBox boundingBox(const VertexIndexGroups& strips) const;

		/**
		 * Determines the bounding box of this vertex set.
		 * @param numberVertices The number of vertices for which the bounding box will be determined, with range [0, infinity)
		 * @return The resulting bounding box
		 */
		BoundingBox boundingBox(const unsigned int numberVertices) const;

		/**
		 * Creates and initializes a buffer object's data store.
		 * @param target Specifies the target to which the buffer object is bound
		 * @param values The values to be set, must be valid
		 * @param size The number of values to be set, with range [1, infinity)
		 * @param usage Specifies the expected usage pattern of the data store
		 */
		template <typename T>
		static void setBufferData(const GLenum target, const T* values, const size_t size, const GLenum usage);

	protected:

		/**
		 * Releases all internal OpenGL ES vertex buffer objects.
		 */
		void release();

	protected:

		/// The vertex array for this VertexSet.
		GLuint vertexArray_ = 0u;

		/// The buffer object for the vertices.
		VertexBufferObjectT<Vector3> bufferVertices_ = VertexBufferObjectT<Vector3>("aVertex");

		/// The buffer object for the normals.
		VertexBufferObjectT<Vector3> bufferNormals_ = VertexBufferObjectT<Vector3>("aNormal");

		/// The buffer object for the 2D texture coordinates.
		VertexBufferObjectT<Vector2> bufferTextureCoordinates2D_ = VertexBufferObjectT<Vector2>("aTextureCoordinate");

		/// The buffer object for the 3D textures coordinates (for phantom objects).
		VertexBufferObjectT<Vector3> bufferTextureCoordinates3D_ = VertexBufferObjectT<Vector3>("aTextureCoordinate");

		/// The buffer object for the colors.
		VertexBufferObjectT<RGBAColor> bufferColors_ = VertexBufferObjectT<RGBAColor>("aColor");

		/// The custom buffer objects.
		VertexBufferObjectMap customVertexBufferObjectMap_;

		/// Explicit reference coordinate system for phantom texture coordinates.
		std::string phantomTextureReferenceCoordinateSystem_;

		/// The vertices stored in this vertex set.
		Vectors3 vertices_;
};

template <typename T>
GLESVertexSet::VertexBufferObjectT<T>::VertexBufferObjectT(std::string attributeName) :
	attributeName_(std::move(attributeName))
{
	// nothing to do here
}

template <typename T>
GLESVertexSet::VertexBufferObjectT<T>::VertexBufferObjectT(VertexBufferObjectT<T>&& vertexBufferObject) :
	attributeName_(std::move(vertexBufferObject.attributeName_)),
	buffer_(vertexBufferObject.buffer_),
	numberElements_(vertexBufferObject.numberElements_)
{
	vertexBufferObject.buffer_ = 0u;
	vertexBufferObject.numberElements_ = 0u;
}

template <typename T>
void GLESVertexSet::VertexBufferObjectT<T>::setData(const T* elements, const size_t numberElements, const GLenum usage)
{
	ocean_assert(elements != nullptr);
	ocean_assert(numberElements != 0);

	if (buffer_ == 0u)
	{
		glGenBuffers(1, &buffer_);
		ocean_assert(GL_NO_ERROR == glGetError());
		ocean_assert(buffer_ != 0u);
	}

	glBindBuffer(GL_ARRAY_BUFFER, buffer_);
	ocean_assert(GL_NO_ERROR == glGetError());

	setBufferData(GL_ARRAY_BUFFER, elements, numberElements, usage);
	numberElements_ = (unsigned int)(numberElements);

	if (glGetError() == GL_OUT_OF_MEMORY)
	{
		release();

		ocean_assert(false && "Out of memory!");
	}
}

template <typename T>
void GLESVertexSet::VertexBufferObjectT<T>::bind(const GLuint programId, const char* attributeName) const
{
	if (buffer_ != 0u)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer_);
		ocean_assert(GL_NO_ERROR == glGetError());

		ocean_assert(glIsProgram(programId));
		const GLint location = glGetAttribLocation(programId, attributeName != nullptr ? attributeName : attributeName_.c_str());
		ocean_assert(GL_NO_ERROR == glGetError());

		if (location != -1)
		{
			glEnableVertexAttribArray(location);
			ocean_assert(GL_NO_ERROR == glGetError());

			constexpr GLint size = GLint(numberComponents<T>());
			constexpr GLenum type = componentType<T>();

			if (isFloatComponent<T>())
			{
				glVertexAttribPointer(location, size, type, GL_FALSE, 0, nullptr);
			}
			else
			{
				glVertexAttribIPointer(location, size, type, 0, nullptr);
			}

			ocean_assert(GL_NO_ERROR == glGetError());
		}
	}
}

template <typename T>
void GLESVertexSet::VertexBufferObjectT<T>::bind(const GLuint programId) const
{
	bind(programId, attributeName_.c_str());
}

template <typename T>
inline unsigned int GLESVertexSet::VertexBufferObjectT<T>::numberElements() const
{
	return numberElements_;
}

template <typename T>
void GLESVertexSet::VertexBufferObjectT<T>::release()
{
	if (buffer_ != 0u)
	{
		glDeleteBuffers(1, &buffer_);
		ocean_assert(GL_NO_ERROR == glGetError());

		buffer_ = 0u;
		numberElements_ = 0u;
	}
}

template <>
constexpr size_t GLESVertexSet::numberComponents<uint8_t>()
{
	return 1;
}

template <>
constexpr size_t GLESVertexSet::numberComponents<uint16_t>()
{
	return 1;
}

template <>
constexpr size_t GLESVertexSet::numberComponents<uint32_t>()
{
	return 1;
}

template <>
constexpr size_t GLESVertexSet::numberComponents<float>()
{
	return 1;
}

template <>
constexpr size_t GLESVertexSet::numberComponents<double>()
{
	return 1;
}

template <>
constexpr size_t GLESVertexSet::numberComponents<RGBAColor>()
{
	return 4;
}

template <>
constexpr size_t GLESVertexSet::numberComponents<VectorF2>()
{
	return 2;
}

template <>
constexpr size_t GLESVertexSet::numberComponents<VectorD2>()
{
	return 2;
}

template <>
constexpr size_t GLESVertexSet::numberComponents<VectorF3>()
{
	return 3;
}

template <>
constexpr size_t GLESVertexSet::numberComponents<VectorD3>()
{
	return 3;
}

template <>
constexpr size_t GLESVertexSet::numberComponents<VectorF4>()
{
	return 4;
}

template <>
constexpr size_t GLESVertexSet::numberComponents<VectorD4>()
{
	return 4;
}

template <>
constexpr size_t GLESVertexSet::numberComponents<VectorT4<uint16_t>>()
{
	return 4;
}

template <>
constexpr size_t GLESVertexSet::numberComponents<VectorT4<uint32_t>>()
{
	return 4;
}

template <typename T>
constexpr size_t GLESVertexSet::numberComponents()
{
	ocean_assert(false && "Missing implementation!");
	return 0;
}

template <>
constexpr GLenum GLESVertexSet::componentType<uint8_t>()
{
	return  GL_UNSIGNED_BYTE;
}

template <>
constexpr GLenum GLESVertexSet::componentType<uint16_t>()
{
	return  GL_UNSIGNED_SHORT;
}

template <>
constexpr GLenum GLESVertexSet::componentType<uint32_t>()
{
	return  GL_UNSIGNED_INT;
}

template <>
constexpr GLenum GLESVertexSet::componentType<float>()
{
	return GL_FLOAT;
}

template <>
constexpr GLenum GLESVertexSet::componentType<double>()
{
	return GL_FLOAT; // float as we use float32 only
}

template <>
constexpr GLenum GLESVertexSet::componentType<RGBAColor>()
{
	return GL_FLOAT;
}

template <>
constexpr GLenum GLESVertexSet::componentType<VectorF2>()
{
	return GL_FLOAT;
}

template <>
constexpr GLenum GLESVertexSet::componentType<VectorD2>()
{
	return GL_FLOAT;
}

template <>
constexpr GLenum GLESVertexSet::componentType<VectorF3>()
{
	return GL_FLOAT;
}

template <>
constexpr GLenum GLESVertexSet::componentType<VectorD3>()
{
	return GL_FLOAT;
}

template <>
constexpr GLenum GLESVertexSet::componentType<VectorF4>()
{
	return GL_FLOAT;
}

template <>
constexpr GLenum GLESVertexSet::componentType<VectorD4>()
{
	return GL_FLOAT;
}

template <>
constexpr GLenum GLESVertexSet::componentType<VectorT4<uint16_t>>()
{
	return GL_UNSIGNED_SHORT;
}

template <>
constexpr GLenum GLESVertexSet::componentType<VectorT4<uint32_t>>()
{
	return GL_UNSIGNED_INT;
}

template <typename T>
constexpr GLenum GLESVertexSet::componentType()
{
	ocean_assert(false && "Missing implementation!");
	return 0;
}

template <>
constexpr bool GLESVertexSet::isFloatComponent<uint8_t>()
{
	return false;
}

template <>
constexpr bool GLESVertexSet::isFloatComponent<uint16_t>()
{
	return false;
}

template <>
constexpr bool GLESVertexSet::isFloatComponent<uint32_t>()
{
	return false;
}

template <>
constexpr bool GLESVertexSet::isFloatComponent<VectorT4<uint16_t>>()
{
	return false;
}

template <>
constexpr bool GLESVertexSet::isFloatComponent<VectorT4<uint32_t>>()
{
	return false;
}

template <typename T>
constexpr bool GLESVertexSet::isFloatComponent()
{
	return true;
}

template <typename T>
void GLESVertexSet::setAttribute(const std::string& attibuteName, const T* elements, const size_t numberElements)
{
	VertexBufferObjectMap::iterator iObject = customVertexBufferObjectMap_.find(attibuteName);

	if (iObject == customVertexBufferObjectMap_.cend())
	{
		iObject = customVertexBufferObjectMap_.emplace(attibuteName, std::make_shared<VertexBufferObjectT<T>>(attibuteName)).first;
	}

	dynamic_cast<VertexBufferObjectT<T>*>(iObject->second.get())->setData(elements, numberElements);
}

template <>
inline void GLESVertexSet::setBufferData(const GLenum target, const uint8_t* values, const size_t size, const GLenum usage)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	static_assert(sizeof(GLbyte) == sizeof(uint8_t), "Invalid data type!");

	glBufferData(target, GLsizeiptr(size * sizeof(GLbyte)), values, usage);
	ocean_assert(GL_NO_ERROR == glGetError());
}

template <>
inline void GLESVertexSet::setBufferData(const GLenum target, const uint16_t* values, const size_t size, const GLenum usage)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	static_assert(sizeof(GLshort) == sizeof(uint16_t), "Invalid data type!");

	glBufferData(target, GLsizeiptr(size * sizeof(GLshort)), values, usage);
	ocean_assert(GL_NO_ERROR == glGetError());
}

template <>
inline void GLESVertexSet::setBufferData(const GLenum target, const uint32_t* values, const size_t size, const GLenum usage)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	static_assert(sizeof(GLint) == sizeof(uint32_t), "Invalid data type!");

	glBufferData(target, GLsizeiptr(size * sizeof(GLint)), values, usage);
	ocean_assert(GL_NO_ERROR == glGetError());
}

template <>
inline void GLESVertexSet::setBufferData(const GLenum target, const float* values, const size_t size, const GLenum usage)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	static_assert(sizeof(GLfloat) == sizeof(float), "Invalid data type!");

	glBufferData(target, GLsizeiptr(size * sizeof(GLfloat)), values, usage);
	ocean_assert(GL_NO_ERROR == glGetError());
}

template <>
inline void GLESVertexSet::setBufferData(const GLenum target, const double* values, const size_t size, const GLenum usage)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	std::vector<float> floatValues(size);

	for (size_t n = 0; n < size; n++)
	{
		floatValues[n] = float(values[n]);
	}

	static_assert(sizeof(GLfloat) == sizeof(float), "Invalid data type!");

	glBufferData(target, GLsizeiptr(size * sizeof(GLfloat)), floatValues.data(), usage);
	ocean_assert(GL_NO_ERROR == glGetError());
}

template <>
inline void GLESVertexSet::setBufferData(const GLenum target, const RGBAColor* values, const size_t size, const GLenum usage)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	static_assert(sizeof(GLfloat) * 4 == sizeof(RGBAColor), "Invalid data type!");

	glBufferData(target, GLsizeiptr(size * sizeof(GLfloat) * 4), values, usage);
	ocean_assert(GL_NO_ERROR == glGetError());
}

template <>
inline void GLESVertexSet::setBufferData(const GLenum target, const VectorF2* values, const size_t size, const GLenum usage)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	static_assert(sizeof(GLfloat) * 2 == sizeof(VectorF2), "Invalid data type!");

	glBufferData(target, GLsizeiptr(size * sizeof(GLfloat) * 2), values, usage);
	ocean_assert(GL_NO_ERROR == glGetError());
}

template <>
inline void GLESVertexSet::setBufferData(const GLenum target, const VectorD2* values, const size_t size, const GLenum usage)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	std::vector<float> floatValues(size * 2);

	for (size_t n = 0; n < size; n++)
	{
		floatValues[2 * n + 0] = float(values[n][0]);
		floatValues[2 * n + 1] = float(values[n][1]);
	}

	static_assert(sizeof(GLfloat) * 2 == sizeof(VectorF2), "Invalid data type!");

	glBufferData(target, GLsizeiptr(size * sizeof(GLfloat) * 2), floatValues.data(), usage);
	ocean_assert(GL_NO_ERROR == glGetError());
}

template <>
inline void GLESVertexSet::setBufferData(const GLenum target, const VectorF3* values, const size_t size, const GLenum usage)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	static_assert(sizeof(GLfloat) * 3 == sizeof(VectorF3), "Invalid data type!");

	glBufferData(target, GLsizeiptr(size * sizeof(GLfloat) * 3), values, usage);
	ocean_assert(GL_NO_ERROR == glGetError());
}

template <>
inline void GLESVertexSet::setBufferData(const GLenum target, const VectorD3* values, const size_t size, const GLenum usage)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	std::vector<float> floatValues(size * 3);

	for (size_t n = 0; n < size; n++)
	{
		floatValues[3 * n + 0] = float(values[n][0]);
		floatValues[3 * n + 1] = float(values[n][1]);
		floatValues[3 * n + 2] = float(values[n][2]);
	}

	static_assert(sizeof(GLfloat) * 3 == sizeof(VectorF3), "Invalid data type!");

	glBufferData(target, GLsizeiptr(size * sizeof(GLfloat) * 3), floatValues.data(), usage);
	ocean_assert(GL_NO_ERROR == glGetError());
}

template <>
inline void GLESVertexSet::setBufferData(const GLenum target, const VectorF4* values, const size_t size, const GLenum usage)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	static_assert(sizeof(GLfloat) * 4 == sizeof(VectorF4), "Invalid data type!");

	glBufferData(target, GLsizeiptr(size * sizeof(GLfloat) * 4), values, usage);
	ocean_assert(GL_NO_ERROR == glGetError());
}

template <>
inline void GLESVertexSet::setBufferData(const GLenum target, const VectorD4* values, const size_t size, const GLenum usage)
{
	ocean_assert(GL_NO_ERROR == glGetError());

	std::vector<float> floatValues(size * 4);

	for (size_t n = 0; n < size; n++)
	{
		floatValues[4 * n + 0] = float(values[n][0]);
		floatValues[4 * n + 1] = float(values[n][1]);
		floatValues[4 * n + 2] = float(values[n][2]);
		floatValues[4 * n + 3] = float(values[n][3]);
	}

	static_assert(sizeof(GLfloat) * 4 == sizeof(VectorF4), "Invalid data type!");

	glBufferData(target, GLsizeiptr(size * sizeof(GLfloat) * 4), floatValues.data(), usage);
	ocean_assert(GL_NO_ERROR == glGetError());
}

template <>
inline void GLESVertexSet::setBufferData(const GLenum target, const VectorT4<uint16_t>* values, const size_t size, const GLenum usage) // **TODO** use template parameters to remove functions above
{
	ocean_assert(GL_NO_ERROR == glGetError());

	static_assert(sizeof(GLushort) * 4 == sizeof(VectorT4<uint16_t>), "Invalid data type!");

	glBufferData(target, GLsizeiptr(size * sizeof(GLushort) * 4), values, usage);
	ocean_assert(GL_NO_ERROR == glGetError());
}

template <>
inline void GLESVertexSet::setBufferData(const GLenum target, const VectorT4<uint32_t>* values, const size_t size, const GLenum usage) // **TODO** use template parameters to remove functions above
{
	ocean_assert(GL_NO_ERROR == glGetError());

	static_assert(sizeof(GLuint) * 4 == sizeof(VectorT4<uint32_t>), "Invalid data type!");

	glBufferData(target, GLsizeiptr(size * sizeof(GLuint) * 4), values, usage);
	ocean_assert(GL_NO_ERROR == glGetError());
}

template <typename T>
void GLESVertexSet::setBufferData(const GLenum /*target*/, const T* /*values*/, const size_t /*size*/, const GLenum /*usage*/)
{
	ocean_assert(false && "Missing implementation!");
}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_VERTEX_SET_H
