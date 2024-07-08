/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_JS_EXTERNAL_H
#define META_OCEAN_INTERACTION_JS_EXTERNAL_H

#include "ocean/interaction/javascript/JavaScript.h"
#include "ocean/interaction/javascript/JSDevice.h"

#include "ocean/devices/Measurement.h"

#include "ocean/math/Euler.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Line3.h"
#include "ocean/math/Plane3.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/RGBAColor.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/SquareMatrix4.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"
#include "ocean/math/Vector4.h"

#include "ocean/media/MediumRef.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/ObjectRef.h"

#include "ocean/scenedescription/Node.h"

namespace Ocean
{

namespace Interaction
{

namespace JavaScript
{

/**
 * This class implements a type-safe object wrapper for non-JavaScript objects.
 * @ingroup interactionjs
 */
class OCEAN_INTERACTION_JS_EXPORT JSExternal
{
	public:

		/**
		 * Definition of different external object types.
		 */
		enum Type
		{
			/// Undefined object.
			TYPE_UNDEFINED,
			/// Color object.
			TYPE_COLOR,
			/// Device object object.
			TYPE_DEVICE_OBJECT,
			/// Device sample object.
			TYPE_DEVICE_SAMPLE,
			/// Euler object.
			TYPE_EULER,
			/// HomogenousMatrix4 object.
			TYPE_HOMOGENOUS_MATRIX_4,
			/// Line3 object.
			TYPE_LINE_3,
			/// Media object object.
			TYPE_MEDIA_OBJECT,
			/// Plane3 object.
			TYPE_PLANE_3,
			/// Quaternion object.
			TYPE_QUATERNION,
			/// Rendering engine object.
			TYPE_RENDERING_ENGINE,
			/// Rendering object object.
			TYPE_RENDERING_OBJECT,
			/// Rotation object.
			TYPE_ROTATION,
			/// Scene description node.
			TYPE_SCENE_DESCRIPTION_NODE,
			/// SquareMatrix3 object.
			TYPE_SQUARE_MATRIX_3,
			/// SquareMatrix4 object.
			TYPE_SQUARE_MATRIX_4,
			/// Vector2 object.
			TYPE_VECTOR_2,
			/// Vector3 object.
			TYPE_VECTOR_3,
			/// Vector4 object.
			TYPE_VECTOR_4
		};

	public:

		/**
		 * Disabled copy constructor.
		 */
		JSExternal(const JSExternal&) = delete;

		/**
		 * Returns the type of the wrapped C++ object.
		 * @return The wrapped type
		 */
		inline Type type() const;

		/**
		 * Returns the wrapped C++ object.
		 * @return The reference to the C++ object which can be used to change the value of the object
		 * @tparam TNative The data type of the wrapped native C++ object
		 */
		template <typename TNative>
		inline TNative& value();

		/**
		 * Returns the type of a C++ object.
		 * @return The type associated with a native C++ object
		 * @tparam TNative The data type of the wrapped native C++ object
		 */
		template <typename TNative>
		static inline Type type();

		/**
		 * Returns the external object from a given JavaScript object.
		 * @param value The JavaScript value for which the external object will be returned
		 * @return The resulting external object, otherwise nullptr
		 */
		static JSExternal* external(const v8::Local<v8::Value>& value);

		/**
		 * Returns the external object from a given JavaScript object.
		 * @param object The JavaScript object for which the external object will be returned
		 * @return The resulting external object, otherwise nullptr
		 */
		static JSExternal* external(const v8::Local<v8::Object>& object);

		/**
		 * Sets or changes the native C++ object of an external JavaScript object.
		 * @param object The external JavaScript object for which the native vlaue will be set or changed, must be valid
		 * @param value The native value to be set, will be moved
		 * @tparam TNative The data type of the native C++ object
		 */
		template <typename TNative>
		static inline void setValue(v8::Local<v8::Object>& object, TNative&& value);

		/**
		 * Sets or changes the native C++ object of an external JavaScript object.
		 * @param object The external JavaScript object for which the native vlaue will be set or changed, must be valid
		 * @param value The native value to be set
		 * @tparam TNative The data type of the native C++ object
		 */
		template <typename TNative>
		static inline void setValue(v8::Local<v8::Object>& object, const TNative& value);

		/**
		 * Creates a new type-safe wrapper object for a specific native C++ object.
		 * @param data The pointer to the C++ object to be wrapped, must be valid
		 * @param owner The owner of this new JSExternal object
		 * @param isolate The current isolate
		 * @tparam TNative The data type of the native C++ object to be wrapped
		 */
		template <typename TNative>
		static JSExternal* create(TNative* data, v8::Local<v8::Object>& owner, v8::Isolate* isolate);

	protected:

		/**
		 * Creates a new type-safe wrapper object for a specific native C++ object.
		 * @param data The pointer to the C++ object to be wrapped, must be valid
		 * @param type The type of the C++ object, must be valid
		 * @param owner The owner of this new JSExternal object
		 * @param isolate The current isolate
		 */
		JSExternal(void* data, const Type type, v8::Local<v8::Object>& owner, v8::Isolate* isolate);

		/**
		 * Destructs this object.
		 */
		~JSExternal();

		/**
		 * The callback function which will be called from v8 once the wrapped object is not used anymore.
		 * @param info The function callback info object as provided by v8
		 */
		static void destructorCallback(const v8::WeakCallbackInfo<JSExternal>& info);

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		JSExternal& operator=(JSExternal&) = delete;

	private:

		/// The pointer to the actual native object.
		void* data_;

		/// The type of the native object.
		Type type_;

		/// The v8 Persistent object ensuring that we get informed when the JS object is not needed anymore.
		v8::Persistent<v8::Object> persistent_;
};

template <typename TNative>
JSExternal* JSExternal::create(TNative* data, v8::Local<v8::Object>& owner, v8::Isolate* isolate)
{
	return new JSExternal((void*)(data), type<TNative>(), owner, isolate);
}

inline JSExternal::Type JSExternal::type() const
{
	return type_;
}

template <>
inline JSDevice& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_DEVICE_OBJECT);

	return *static_cast<JSDevice*>(data_);
}

template <>
inline Devices::Measurement::SampleRef& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_DEVICE_SAMPLE);

	return *static_cast<Devices::Measurement::SampleRef*>(data_);
}

template <>
inline Euler& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_EULER);

	return *static_cast<Euler*>(data_);
}

template <>
inline HomogenousMatrix4& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_HOMOGENOUS_MATRIX_4);

	return *static_cast<HomogenousMatrix4*>(data_);
}

template <>
inline Line3& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_LINE_3);

	return *static_cast<Line3*>(data_);
}

template <>
inline Plane3& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_PLANE_3);

	return *static_cast<Plane3*>(data_);
}

template <>
inline Rendering::EngineRef& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_RENDERING_ENGINE);

	return *static_cast<Rendering::EngineRef*>(data_);
}

template <>
inline Rendering::ObjectRef& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_RENDERING_OBJECT);

	return *static_cast<Rendering::ObjectRef*>(data_);
}

template <>
inline Media::MediumRef& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_MEDIA_OBJECT);

	return *static_cast<Media::MediumRef*>(data_);
}

template <>
inline SceneDescription::NodeRef& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_SCENE_DESCRIPTION_NODE);

	return *static_cast<SceneDescription::NodeRef*>(data_);
}

template <>
inline Quaternion& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_QUATERNION);

	return *static_cast<Quaternion*>(data_);
}

template <>
inline RGBAColor& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_COLOR);

	return *static_cast<RGBAColor*>(data_);
}

template <>
inline Rotation& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_ROTATION);

	return *static_cast<Rotation*>(data_);
}

template <>
inline Vector2& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_VECTOR_2);

	return *static_cast<Vector2*>(data_);
}

template <>
inline Vector3& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_VECTOR_3);

	return *static_cast<Vector3*>(data_);
}

template <>
inline Vector4& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_VECTOR_4);

	return *static_cast<Vector4*>(data_);
}

template <>
inline SquareMatrix3& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_SQUARE_MATRIX_3);

	return *static_cast<SquareMatrix3*>(data_);
}

template <>
inline SquareMatrix4& JSExternal::value()
{
	ocean_assert(data_ != nullptr);
	ocean_assert(type_ == TYPE_SQUARE_MATRIX_4);

	return *static_cast<SquareMatrix4*>(data_);
}

template <typename TNative>
inline TNative& JSExternal::value()
{
	static_assert(oceanFalse<TNative>(), "This function is not defined for the data type");
}

template <>
inline JSExternal::Type JSExternal::type<Devices::Measurement::SampleRef>()
{
	return TYPE_DEVICE_SAMPLE;
}

template <>
inline JSExternal::Type JSExternal::type<JSDevice>()
{
	return TYPE_DEVICE_OBJECT;
}

template <>
inline JSExternal::Type JSExternal::type<Euler>()
{
	return TYPE_EULER;
}

template <>
inline JSExternal::Type JSExternal::type<HomogenousMatrix4>()
{
	return TYPE_HOMOGENOUS_MATRIX_4;
}

template <>
inline JSExternal::Type JSExternal::type<Line3>()
{
	return TYPE_LINE_3;
}

template <>
inline JSExternal::Type JSExternal::type<Plane3>()
{
	return TYPE_PLANE_3;
}

template <>
inline JSExternal::Type JSExternal::type<Rendering::EngineRef>()
{
	return TYPE_RENDERING_ENGINE;
}

template <>
inline JSExternal::Type JSExternal::type<Rendering::ObjectRef>()
{
	return TYPE_RENDERING_OBJECT;
}


template <>
inline JSExternal::Type JSExternal::type<Media::MediumRef>()
{
	return TYPE_MEDIA_OBJECT;
}

template <>
inline JSExternal::Type JSExternal::type<SceneDescription::NodeRef>()
{
	return TYPE_SCENE_DESCRIPTION_NODE;
}

template <>
inline JSExternal::Type JSExternal::type<Quaternion>()
{
	return TYPE_QUATERNION;
}

template <>
inline JSExternal::Type JSExternal::type<RGBAColor>()
{
	return TYPE_COLOR;
}

template <>
inline JSExternal::Type JSExternal::type<Rotation>()
{
	return TYPE_ROTATION;
}

template <>
inline JSExternal::Type JSExternal::type<SquareMatrix3>()
{
	return TYPE_SQUARE_MATRIX_3;
}

template <>
inline JSExternal::Type JSExternal::type<SquareMatrix4>()
{
	return TYPE_SQUARE_MATRIX_4;
}

template <>
inline JSExternal::Type JSExternal::type<Vector2>()
{
	return TYPE_VECTOR_2;
}

template <>
inline JSExternal::Type JSExternal::type<Vector3>()
{
	return TYPE_VECTOR_3;
}

template <>
inline JSExternal::Type JSExternal::type<Vector4>()
{
	return TYPE_VECTOR_4;
}

template <typename TNative>
inline JSExternal::Type JSExternal::type()
{
	static_assert(oceanFalse<TNative>(), "This function is not defined for the data type");
}


template <typename TNative>
inline void JSExternal::setValue(v8::Local<v8::Object>& object, TNative&& value)
{
	JSExternal* externalObject = external(object);
	ocean_assert(externalObject != nullptr);

	externalObject->value<TNative>() = std::move(value);
}

template <typename TNative>
inline void JSExternal::setValue(v8::Local<v8::Object>& object, const TNative& value)
{
	JSExternal* externalObject = external(object);
	ocean_assert(externalObject != nullptr);

	externalObject->value<TNative>() = value;
}

}

}

}

#endif // META_OCEAN_INTERACTION_JS_EXTERNAL_H
