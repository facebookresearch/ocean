/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAG_H
#define META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAG_H

#include "ocean/tracking/oculustags/OculusTags.h"

namespace Ocean
{

namespace Tracking
{

namespace OculusTags
{

/**
 * This class implements a Oculus tag.
 * @ingroup trackingoculustags
 */
class OCEAN_TRACKING_OCULUSTAGS_EXPORT OculusTag
{
	public:

		/**
		* Reflectance types that the tag can have
		*/
		enum ReflectanceType : uint8_t
		{
			/// Normal reflectance: '1' - dark intensity, '0' - light intensity
			RT_REFLECTANCE_NORMAL = 0u,

			/// Inverted reflectance: '1' - light intensity, '0' - dark intensity
			RT_REFLECTANCE_INVERTED,

			/// Used if the reflectance type is unknown or invalid
			RT_REFLECTANCE_UNDEFINED
		};

		/// Data structure for the payload of the code: 4 x 4 = 16 bits
		typedef uint16_t DataMatrix;

	public:

		/**
		* Creates an empty Oculus Tag instance with normal reflectance
		*/
		OculusTag();

		/**
		* Creates an Oculus Tag instance
		* @param tagID The ID of the tag, range: [0, 1024)
		* @param reflectanceType The reflectance type of the tag; can be normal or inverted (but not undefined)
		* @param intensityThreshold The intensity value that was used to threshold foreground and background pixel values, range: [0, 256)
		* @param world_T_tag The 6DOF pose of the tag in the world such that `worldCoord = world_T_tag * tagCoord`, must be valid
		* @param tagSize The edge length of the tag in 3D object space, range: (0, infinity)
		*/
		OculusTag(const uint32_t& tagID, const ReflectanceType& reflectanceType, const uint8_t& intensityThreshold, const HomogenousMatrix4& world_T_tag, const Scalar tagSize);

		/**
		* Returns true if the tag is valid
		* @return True, if the tag is valid, otherwise false
		*/
		bool isValid() const;

		/**
		* Return the ID of this tag
		* @return The ID of this tag, range: [0, 1024)
		*/
		inline uint32_t tagID() const;

		/**
		* Returns the reflectance type of this tag
		* @return The reflectance type
		*/
		inline ReflectanceType reflectanceType() const;

		/**
		* Returns the grayscale intensity that was used for binary separation of foreground and background
		* @return The value of the threshold
		*/
		inline uint8_t intensityThreshold() const;

		/**
		* Returns the transformation to map tag coordinates into world coordinates
		* @return The pose such that `deviceCoord = world_T_device() * tagCoord`
		* @tparam tOriginInCenter If true, the center of the tag is defined as its origin, otherwise its top-left corner is its origin
		*/
		template <bool tOriginInCenter = false>
		inline const HomogenousMatrix4 world_T_tag() const;

		/**
		* Updates the 6DOF pose of the tag
		* @param world_T_tag The 6DOF pose of the tag in the world such that `worldCoord = world_T_tag * tagCoord`, must be valid
		*/
		inline void setWorld_T_tag(const HomogenousMatrix4& world_T_tag);

		/**
		* Return the size of the tag
		* @return The size of the tag
		*/
		inline Scalar tagSize() const;

	public:

		/// Number of modules along the vertical and horizontal directions
		static constexpr uint32_t numberOfModules = 8u;

	protected:

		/// The ID of this tag
		uint32_t tagID_;

		/// The reflectance type of this tag instance
		ReflectanceType reflectanceType_;

		/// The grayscale intensity that was used binary separation of foreground and background
		uint8_t intensityThreshold_;

		/// The 6DOF pose of the tag relative to the world
		HomogenousMatrix4 world_T_tag_;

		/// The edge length of the tag in 3D object space
		Scalar tagSize_;
};

/**
 * A vector of Oculus tags.
 * @ingroup trackingoculustags
 */
typedef std::vector<OculusTag> OculusTags;

/**
 * A data structure to map tag IDs to tag sizes.
 * @ingroup trackingoculustags
 */
typedef std::unordered_map<uint32_t, Scalar> TagSizeMap;

inline uint32_t OculusTag::tagID() const
{
	return tagID_;
}

inline OculusTag::ReflectanceType OculusTag::reflectanceType() const
{
	return reflectanceType_;
}

inline uint8_t OculusTag::intensityThreshold() const
{
	return intensityThreshold_;
}

template <bool tOriginInCenter>
inline const HomogenousMatrix4 OculusTag::world_T_tag() const
{
	if constexpr (tOriginInCenter)
	{
		return world_T_tag_ * HomogenousMatrix4(Vector3(Scalar(0.5) * tagSize(), Scalar(-0.5) * tagSize(), 0));
	}

	return world_T_tag_; // Returned this as `const HomogenousMatrix4&` again once possible
}

inline void OculusTag::setWorld_T_tag(const HomogenousMatrix4& world_T_tag)
{
	ocean_assert(world_T_tag.isValid());
	world_T_tag_ = world_T_tag;
}

inline Scalar OculusTag::tagSize() const
{
	return tagSize_;
}

} // namespace OculusTags

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAG_H
