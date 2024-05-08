/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/maps/Utilities.h"

#include "ocean/cv/Canvas.h"

namespace Ocean
{

namespace IO
{

namespace Maps
{

Frame Utilities::drawToImage(const Basemap::Tile& tile, const unsigned int imageExtent)
{
	ocean_assert(tile.isValid());
	ocean_assert(imageExtent >= 1u);

	const Basemap::SharedObjects& sharedObjects = tile.objects();

	Frame frame(FrameType(imageExtent, imageExtent, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
	frame.setValue(0xFF);

	const uint8_t* const colorSidewalk = CV::Canvas::blue(frame.pixelFormat());
	const uint8_t* const colorRoad = CV::Canvas::gray(frame.pixelFormat());
	const uint8_t* const colorTransit = CV::Canvas::gray(frame.pixelFormat());
	const uint8_t* const colorBuilding = CV::Canvas::black(frame.pixelFormat());
	const uint8_t* const colorWater = CV::Canvas::blue(frame.pixelFormat());
	const uint8_t* const colorLandUse = CV::Canvas::green(frame.pixelFormat());
	const uint8_t colorLandCover[3] = {0xB0, 0xB0, 0xB0};

	for (const Basemap::SharedObject& sharedObject : sharedObjects)
	{
		ocean_assert(sharedObject);

		switch (sharedObject->objectType())
		{
			case Basemap::Object::OT_ROAD:
			{
				const Basemap::Road* road = reinterpret_cast<const Basemap::Road*>(sharedObject.get());
				ocean_assert(road != nullptr);

				const uint8_t* color = colorRoad;

				if (road->roadType() == Basemap::Road::RT_SIDEWALK || road->roadType() == Basemap::Road::RT_CROSSWALK || road->roadType() == Basemap::Road::RT_STEPS)
				{
					color = colorSidewalk;
				}

				const Basemap::PixelPositionGroupsI& lineStrings = road->lineStrings();

				for (const Basemap::PixelPositionsI& lineString : lineStrings)
				{
					ocean_assert(!lineString.empty());

					for (size_t n = 1; n < lineString.size(); ++n)
					{
						const Vector2 point0 = road->vectorFromCoordinate(lineString[n - 1], Scalar(imageExtent));
						const Vector2 point1 = road->vectorFromCoordinate(lineString[n], Scalar(imageExtent));

						CV::Canvas::line<1u>(frame, point0, point1, color);
					}
				}

				continue;
			}

			case Basemap::Object::OT_TRANSIT:
			{
				const Basemap::Transit* transit = reinterpret_cast<const Basemap::Transit*>(sharedObject.get());
				ocean_assert(transit != nullptr);

				const uint8_t* color = colorTransit;

				const Basemap::PixelPositionGroupsI& lineStrings = transit->lineStrings();

				for (const Basemap::PixelPositionsI& lineString : lineStrings)
				{
					ocean_assert(!lineString.empty());

					for (size_t n = 1; n < lineString.size(); ++n)
					{
						const Vector2 point0 = transit->vectorFromCoordinate(lineString[n - 1], Scalar(imageExtent));
						const Vector2 point1 = transit->vectorFromCoordinate(lineString[n], Scalar(imageExtent));

						CV::Canvas::line<1u>(frame, point0, point1, color);
					}
				}

				continue;
			}

			case Basemap::Object::OT_BUILDING:
			{
				const Basemap::Building* building = reinterpret_cast<const Basemap::Building*>(sharedObject.get());
				ocean_assert(building != nullptr);

				const Basemap::PixelPositionGroupsI& outerPolygons = building->outerPolygons();

				for (const Basemap::PixelPositionsI& outerPolygon : outerPolygons)
				{
					ocean_assert(outerPolygon.size() >= 2);

					CV::Canvas::line<1u>(frame, building->vectorFromCoordinate(outerPolygon.front(), Scalar(imageExtent)), building->vectorFromCoordinate(outerPolygon.back(), Scalar(imageExtent)), colorBuilding);

					for (size_t n = 1; n < outerPolygon.size(); ++n)
					{
						const Vector2 point0 = building->vectorFromCoordinate(outerPolygon[n - 1], Scalar(imageExtent));
						const Vector2 point1 = building->vectorFromCoordinate(outerPolygon[n], Scalar(imageExtent));

						CV::Canvas::line<1u>(frame, point0, point1, colorBuilding);
					}
				}

				const Basemap::PixelPositionGroupsI& innerPolygons = building->innerPolygons();

				for (const Basemap::PixelPositionsI& innerPolygon : innerPolygons)
				{
					ocean_assert(innerPolygon.size() >= 2);

					CV::Canvas::line<1u>(frame, building->vectorFromCoordinate(innerPolygon.front(), Scalar(imageExtent)), building->vectorFromCoordinate(innerPolygon.back(), Scalar(imageExtent)), colorBuilding);

					for (size_t n = 1; n < innerPolygon.size(); ++n)
					{
						const Vector2 point0 = building->vectorFromCoordinate(innerPolygon[n - 1], Scalar(imageExtent));
						const Vector2 point1 = building->vectorFromCoordinate(innerPolygon[n], Scalar(imageExtent));

						CV::Canvas::line<1u>(frame, point0, point1, colorBuilding);
					}
				}

				const Basemap::PixelPositionGroupsI& lineStrings = building->lineStrings();

				for (const Basemap::PixelPositionsI& lineString : lineStrings)
				{
					ocean_assert(!lineString.empty());

					for (size_t n = 1; n < lineString.size(); ++n)
					{
						const Vector2 point0 = building->vectorFromCoordinate(lineString[n - 1], Scalar(imageExtent));
						const Vector2 point1 = building->vectorFromCoordinate(lineString[n], Scalar(imageExtent));

						CV::Canvas::line<1u>(frame, point0, point1, colorBuilding);
					}
				}

				continue;
			}

			case Basemap::Object::OT_WATER:
			{
				const Basemap::Water* water = reinterpret_cast<const Basemap::Water*>(sharedObject.get());
				ocean_assert(water != nullptr);

				const Basemap::PixelPositionGroupsI& outerPolygons = water->outerPolygons();

				for (const Basemap::PixelPositionsI& outerPolygon : outerPolygons)
				{
					ocean_assert(outerPolygon.size() >= 2);

					CV::Canvas::line<1u>(frame, water->vectorFromCoordinate(outerPolygon.front(), Scalar(imageExtent)), water->vectorFromCoordinate(outerPolygon.back(), Scalar(imageExtent)), colorWater);

					for (size_t n = 1; n < outerPolygon.size(); ++n)
					{
						const Vector2 point0 = water->vectorFromCoordinate(outerPolygon[n - 1], Scalar(imageExtent));
						const Vector2 point1 = water->vectorFromCoordinate(outerPolygon[n], Scalar(imageExtent));

						CV::Canvas::line<1u>(frame, point0, point1, colorWater);
					}
				}

				const Basemap::PixelPositionGroupsI& innerPolygons = water->innerPolygons();

				for (const Basemap::PixelPositionsI& innerPolygon : innerPolygons)
				{
					ocean_assert(innerPolygon.size() >= 2);

					CV::Canvas::line<1u>(frame, water->vectorFromCoordinate(innerPolygon.front(), Scalar(imageExtent)), water->vectorFromCoordinate(innerPolygon.back(), Scalar(imageExtent)), colorWater);

					for (size_t n = 1; n < innerPolygon.size(); ++n)
					{
						const Vector2 point0 = water->vectorFromCoordinate(innerPolygon[n - 1], Scalar(imageExtent));
						const Vector2 point1 = water->vectorFromCoordinate(innerPolygon[n], Scalar(imageExtent));

						CV::Canvas::line<1u>(frame, point0, point1, colorWater);
					}
				}

				continue;
			}

			case Basemap::Object::OT_LAND_USE:
			{
				const Basemap::LandUse* landUse = reinterpret_cast<const Basemap::LandUse*>(sharedObject.get());
				ocean_assert(landUse != nullptr);

				const Basemap::PixelPositionGroupsI& outerPolygons = landUse->outerPolygons();

				for (const Basemap::PixelPositionsI& outerPolygon : outerPolygons)
				{
					ocean_assert(outerPolygon.size() >= 2);

					CV::Canvas::line<1u>(frame, landUse->vectorFromCoordinate(outerPolygon.front(), Scalar(imageExtent)), landUse->vectorFromCoordinate(outerPolygon.back(), Scalar(imageExtent)), colorLandUse);

					for (size_t n = 1; n < outerPolygon.size(); ++n)
					{
						const Vector2 point0 = landUse->vectorFromCoordinate(outerPolygon[n - 1], Scalar(imageExtent));
						const Vector2 point1 = landUse->vectorFromCoordinate(outerPolygon[n], Scalar(imageExtent));

						CV::Canvas::line<1u>(frame, point0, point1, colorLandUse);
					}
				}

				const Basemap::PixelPositionGroupsI& innerPolygons = landUse->innerPolygons();

				for (const Basemap::PixelPositionsI& innerPolygon : innerPolygons)
				{
					ocean_assert(innerPolygon.size() >= 2);

					CV::Canvas::line<1u>(frame, landUse->vectorFromCoordinate(innerPolygon.front(), Scalar(imageExtent)), landUse->vectorFromCoordinate(innerPolygon.back(), Scalar(imageExtent)), colorLandUse);

					for (size_t n = 1; n < innerPolygon.size(); ++n)
					{
						const Vector2 point0 = landUse->vectorFromCoordinate(innerPolygon[n - 1], Scalar(imageExtent));
						const Vector2 point1 = landUse->vectorFromCoordinate(innerPolygon[n], Scalar(imageExtent));

						CV::Canvas::line<1u>(frame, point0, point1, colorLandUse);
					}
				}

				continue;
			}

			case Basemap::Object::OT_LAND_COVER:
			{
				const Basemap::LandCover* landCover = reinterpret_cast<const Basemap::LandCover*>(sharedObject.get());
				ocean_assert(landCover != nullptr);

				const Basemap::PixelPositionGroupsI& outerPolygons = landCover->outerPolygons();

				for (const Basemap::PixelPositionsI& outerPolygon : outerPolygons)
				{
					ocean_assert(outerPolygon.size() >= 2);

					CV::Canvas::line<1u>(frame, landCover->vectorFromCoordinate(outerPolygon.front(), Scalar(imageExtent)), landCover->vectorFromCoordinate(outerPolygon.back(), Scalar(imageExtent)), colorLandCover);

					for (size_t n = 1; n < outerPolygon.size(); ++n)
					{
						const Vector2 point0 = landCover->vectorFromCoordinate(outerPolygon[n - 1], Scalar(imageExtent));
						const Vector2 point1 = landCover->vectorFromCoordinate(outerPolygon[n], Scalar(imageExtent));

						CV::Canvas::line<1u>(frame, point0, point1, colorLandCover);
					}
				}

				const Basemap::PixelPositionGroupsI& innerPolygons = landCover->innerPolygons();

				for (const Basemap::PixelPositionsI& innerPolygon : innerPolygons)
				{
					ocean_assert(innerPolygon.size() >= 2);

					CV::Canvas::line<1u>(frame, landCover->vectorFromCoordinate(innerPolygon.front(), Scalar(imageExtent)), landCover->vectorFromCoordinate(innerPolygon.back(), Scalar(imageExtent)), colorLandCover);

					for (size_t n = 1; n < innerPolygon.size(); ++n)
					{
						const Vector2 point0 = landCover->vectorFromCoordinate(innerPolygon[n - 1], Scalar(imageExtent));
						const Vector2 point1 = landCover->vectorFromCoordinate(innerPolygon[n], Scalar(imageExtent));

						CV::Canvas::line<1u>(frame, point0, point1, colorLandCover);
					}
				}

				continue;
			}

			case Basemap::Object::OT_UNKNOWN:
				break;
		}

		ocean_assert(false && "Invalid object type!");
	}

	return frame;
}

}

}

}
