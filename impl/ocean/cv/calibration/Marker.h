/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_MARKER_H
#define META_OCEAN_CV_CALIBRATION_MARKER_H

#include "ocean/cv/calibration/Calibration.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/**
 * This class implements the base class for a marker in a calibration board.
 * @ingroup cvcalibration
 */
class OCEAN_CV_CALIBRATION_EXPORT Marker
{
	public:

		/**
		 * This class manages all possible layouts of markers.
		 */
		class OCEAN_CV_CALIBRATION_EXPORT LayoutManager
		{
			friend class Marker;

			protected:

				/**
				 * Definition of an invalid marker id.
				 */
				static constexpr size_t invalidMarkerId_ = size_t(-1);

				/// The number of points in each row and column of the marker.
				static constexpr size_t numberRowsColumns_ = 5;

				/// The number of points in the marker.
				static constexpr size_t numberPoints_ = numberRowsColumns_ * numberRowsColumns_;

			public:

				/**
				 * Definition of a marker layout storing one bit for each marker point.
				 * The marker layout is defined for a marker with positive sign (mainly black dot's on white background).<br>
				 * The layout of a negative marker can be derived by inverting the layout of the positive marker.
				 */
				using Layout = std::array<uint8_t, numberPoints_>;

				/**
				 * Definition of a vector holding marker layouts.
				 */
				using Layouts = std::vector<Layout>;

			public:

				/**
				 * Returns the sign of point in a marker layout.
				 * @param markerId The id of the marker, with range [0, layouts().size() - 1]
				 * @param markerSign The sign of the marker; True, if the marker is a positive marker (mainly black dot's on white background); False, if the marker is a negative marker (mainly white dot's on black background)
				 * @param orientation The orientation of the marker, must be valid
				 * @param xUnoriented The horizontal position of the point in the marker, will be rotated due to 'orientation', with range [0, numberRowsColumns_ - 1]
				 * @param yUnoriented The vertical position of the point in the marker, will be rotated due to 'orientation', with range [0, numberRowsColumns_ - 1]
				 * @return True, if the point has a positive sign (black dot on white background); False, if the point has a negative sign (white dot on black background)
				 */
				static bool layoutPointSign(const size_t markerId, const bool markerSign, const CV::PixelDirection& orientation, const size_t xUnoriented, const size_t yUnoriented);

				/**
				 * Returns the sign of point in a marker layout.
				 * @param markerId The id of the marker, with range [0, layouts().size() - 1]
				 * @param markerSign The sign of the marker; True, if the marker is a positive marker (mainly black dot's on white background); False, if the marker is a negative marker (mainly white dot's on black background)
				 * @param orientation The orientation of the marker, must be valid
				 * @param indexInMarkerUnoriented The index of the point in the marker, will be rotated due to 'orientation', with range [0, numberPoints_ - 1]
				 * @return True, if the point has a positive sign (black dot on white background); False, if the point has a negative sign (white dot on black background)
				 */
				static inline bool layoutPointSign(const size_t markerId, const bool markerSign, const CV::PixelDirection& orientation, const size_t indexInMarkerUnoriented);

				/**
				 * Returns all possible marker layouts.
				 * Each marker layout is unique under rotation.
				 * @return The marker layouts
				 */
				static const Layouts& layouts();

			public:

				/**
				 * Determines all unique marker layouts so that no layout can be rotated to another layout.
				 * @return The unique marker layouts
				 */
				static Layouts determineUniqueLayouts();

				/**
				 * Returns whether a layout is similar to several other existing layouts.
				 * @param layouts The existing layouts to compare with, at least one
				 * @param layout The layout to check
				 * @param checkSelfSimilarity True, to check whether the layout is similar to itself; False, to skip the self similarity check
				 * @return True, if so
				 */
				static bool isSimilar(const Layouts& layouts, const Layout& layout, const bool checkSelfSimilarity = true);

				/**
				 * Returns whether two markers can be rotated in a way so that they are identical.
				 * @param layoutA The first layout to check
				 * @param layoutB The second layout to check
				 * @param checkIdentity True, to check whether both layouts are identical without rotation; False, to check whether the layout is rotated by 90, 180, or 270 degree
				 * @return True, if so
				 */
				static inline bool isSimilar(const Layout& layoutA, const Layout& layoutB, const bool checkIdentity = true);

				/**
				 * Returns whether a second layout is a rotated version of a first layout.
				 * @param layout The first layout
				 * @param rotatedLayout The second layout which can be rotated by a multiple of 90 degree
				 * @param checkIdentity True, to check whether both layouts are identical (no rotation); False, to check whether the layout is rotated by 90, 180, or 270 degree
				 * @return The direction in which the second layout is rotated compared to the first layout, PD_INVALID if the second layout is not a rotated version of the first layout
				 */
				static CV::PixelDirection isRotated(const Layout& layout, const Layout& rotatedLayout, const bool checkIdentity = true);
		};

		/**
		 * Definition of a marker type.
		 * A marker type combines the id of a marker with the sign of a marker.
		 */
		using MarkerType = uint32_t;

	public:

		/**
		 * Default constructor to creator a marker with invalid id and unknown sign.
		 */
		Marker() = default;

		/**
		 * Creates a new marker with valid marker id but unknown sign.
		 * @param markerId The id of the marker, must be valid
		 */
		explicit inline Marker(const size_t markerId);

		/**
		 * Creates a new marker with valid marker id and known sign.
		 * @param markerId The id of the marker, must be valid
		 * @param The sign of the marker; True, if the marker is a positive marker (mainly black dot's on white background); False, if the marker is a negative marker (mainly white dot's on black background)
		 */
		inline Marker(const size_t markerId, const bool sign);

		/**
		 * Returns the id of this marker.
		 * @return The marker's id
		 */
		inline size_t markerId() const;

		/**
		 * Returns the sign of this marker.
		 * @return The marker's sign; True, if the marker is a positive marker (mainly black dot's on white background); False, if the marker is a negative marker (mainly white dot's on black background)
		 */
		inline bool sign() const;

		/**
		 * Returns the type of this marker.
		 * @return The marker's type which is a combination of the marker id and the marker's sign
		 */
		inline MarkerType markerType() const;

		/**
		 * Sets the id of the marker.
		 * The id of the marker describes the marker layout but not the position of the marker within a calibration board.
		 * @param markerId The id of the marker to set
		 */
		inline void setMarkerId(const size_t markerId);

		/**
		 * Sets the sign of the marker.
		 * @param sign The sign of the marker to set; True, if the marker is a positive marker (mainly black dot's on white background); False, if the marker is a negative marker (mainly white dot's on black background)
		 */
		inline void setSign(const bool sign);

		/**
		 * Returns whether this marker has a known marker id.
		 * Having a valid marker id means that this marker has been associated with a unique marker layout.
		 * However, as every calibration board may be composed of markers with the same id, the marker id does not yet mean that this marker is associated with a unique position within a calibration board.
		 * @return True, if so
		 * @see hasMarkerCoordinate().
		 */
		inline bool hasMarkerId() const;

		/**
		 * Returns whether this marker has a known sign.
		 * @return True, if so
		 */
		inline bool hasSign() const;

		/**
		 * Returns whether this marker is associated with a valid id and a valid sign.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns the number of rows and columns of a marker.
		 * @return The number of rows and columns
		 */
		static constexpr size_t numberRowsColumns();

		/**
		 * Returns the number of points each marker has.
		 * @return The marker's number of points
		 */
		static constexpr size_t numberPoints();

		/**
		 * Returns the number of unique marker ids.
		 * @return The number of unique markers, with range [1, infinity)
		 */
		static inline size_t numberMarkerIds();

		/**
		 * Returns an invalid marker id.
		 * @return Invalid marker id
		 */
		static constexpr size_t invalidMarkerId();

	private:

		/// The id of the maker, invalid if unknown.
		size_t markerId_ = invalidMarkerId();

		/// The sign of the marker, 1 for markers with positive sign (mainly black dot's on white background); -1 for markers with negative sign (mainly white dot's on black background), 0 for invalid markers
		int32_t sign_ = 0;
};

inline bool Marker::LayoutManager::layoutPointSign(const size_t markerId, const bool markerSign, const CV::PixelDirection& orientation, const size_t indexInMarkerUnoriented)
{
	ocean_assert(indexInMarkerUnoriented < numberPoints_);

	const size_t xUnoriented = indexInMarkerUnoriented % numberRowsColumns_;
	const size_t yUnoriented = indexInMarkerUnoriented / numberRowsColumns_;

	return layoutPointSign(markerId, markerSign, orientation, xUnoriented, yUnoriented);
}

inline bool Marker::LayoutManager::isSimilar(const Layout& layoutA, const Layout& layoutB, const bool checkIdentity)
{
	const CV::PixelDirection rotation = isRotated(layoutA, layoutB, checkIdentity);

	return rotation != CV::PD_INVALID;
}

inline Marker::Marker(const size_t markerId) :
	markerId_(markerId)
{
	ocean_assert(markerId_ != invalidMarkerId());
	ocean_assert(markerId_ < numberMarkerIds());
}

inline Marker::Marker(const size_t markerId, const bool sign) :
	markerId_(markerId),
	sign_(sign ? 1 : -1)
{
	ocean_assert(markerId_ != invalidMarkerId());
	ocean_assert(markerId_ < numberMarkerIds());
}

size_t Marker::markerId() const
{
	return markerId_;
}

inline bool Marker::sign() const
{
	ocean_assert(sign_ != 0);

	return sign_ > 0;
}

inline Marker::MarkerType Marker::markerType() const
{
	ocean_assert(isValid());

	ocean_assert(markerId_ < size_t(1u << 31u));

	const MarkerType uniqueMarkerType = MarkerType(uint32_t(markerId_) << 1u | (sign() ? 1u : 0u));

	ocean_assert((uniqueMarkerType & 1u) == (sign() ? 1u : 0u));
	ocean_assert(size_t(uniqueMarkerType >> 1u) == markerId_);

	return uniqueMarkerType;
}

inline void Marker::setMarkerId(const size_t markerId)
{
	ocean_assert(markerId_ == invalidMarkerId());

	markerId_ = markerId;
}

inline void Marker::setSign(const bool sign)
{
	ocean_assert(sign_ == 0);

	sign_ = sign ? 1 : -1;
}

inline bool Marker::hasMarkerId() const
{
	return markerId_ != invalidMarkerId();
}

inline bool Marker::hasSign() const
{
	return sign_ != 0;
}

inline bool Marker::isValid() const
{
	return markerId_ != invalidMarkerId() && sign_ != 0;
}

constexpr size_t Marker::numberRowsColumns()
{
	return LayoutManager::numberRowsColumns_;
}

constexpr size_t Marker::numberPoints()
{
	return LayoutManager::numberPoints_;
}

size_t Marker::numberMarkerIds()
{
	return LayoutManager::layouts().size();
}

constexpr size_t Marker::invalidMarkerId()
{
	return LayoutManager::invalidMarkerId_;
}

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_MARKER_H
