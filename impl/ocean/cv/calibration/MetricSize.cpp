/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/calibration/MetricSize.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

MetricSize::MetricSize(const double value, const UnitType unitType) :
	valueMillimeter_(convert(unitType, value, UT_MILLIMETER))
{
	// nothing to do here
}

double MetricSize::value(const UnitType unitType) const
{
	return convert<UT_MILLIMETER>(valueMillimeter_, unitType);
}

double MetricSize::convert(const UnitType sourceUnit, const double sourceValue, const UnitType targetUnit)
{
	ocean_assert(sourceUnit != UT_INVALID && targetUnit != UT_INVALID);

	if (sourceUnit == targetUnit)
	{
		return sourceValue;
	}

	switch (sourceUnit)
	{
		case UT_INVALID:
			break;

		case UT_MILLIMETER:
			return convert<UT_MILLIMETER>(sourceValue, targetUnit);

		case UT_INCH:
			return convert<UT_INCH>(sourceValue, targetUnit);
	};

	ocean_assert(false && "Invalid unit type!");
	return sourceValue;
}

bool MetricSize::determinePaperSize(const MetricSize::PaperType paperType, MetricSize& width, MetricSize& height)
{
	switch (paperType)
	{
		case PT_DIN_A3:
		{
			width = MetricSize(297.0, CV::Calibration::MetricSize::UT_MILLIMETER);
			height = MetricSize(420.0, CV::Calibration::MetricSize::UT_MILLIMETER);
			return true;
		}

		case PT_DIN_A4:
		{
			width = MetricSize(210.0, CV::Calibration::MetricSize::UT_MILLIMETER);
			height = MetricSize(297.0, CV::Calibration::MetricSize::UT_MILLIMETER);
			return true;
		}

		case PT_LETTER:
		{
			width = MetricSize(8.5, CV::Calibration::MetricSize::UT_INCH);
			height = MetricSize(11.0, CV::Calibration::MetricSize::UT_INCH);
			return true;
		}

		case PT_TABLOID:
		{
			width = MetricSize(11.0, CV::Calibration::MetricSize::UT_INCH);
			height = MetricSize(17.0, CV::Calibration::MetricSize::UT_INCH);
			return true;
		}

		case PT_INVALID:
			break;
	}

	ocean_assert(false && "Invalid paper type!");
	return false;
}

std::string MetricSize::translateUnitType(const UnitType unitType)
{
	switch (unitType)
	{
		case UT_INVALID:
			break;

		case UT_MILLIMETER:
			return "mm";

		case UT_INCH:
			return "in";
	}

	ocean_assert(false && "Invalid unit type!");
	return "Invalid";
}

MetricSize::PaperType MetricSize::translatePaperType(const std::string& paperType)
{
	ocean_assert(!paperType.empty());

	const std::string lowerPaperType(String::toLower(paperType));

	if (lowerPaperType == "dina3" || lowerPaperType == "a3")
	{
		return PT_DIN_A3;
	}

	if (lowerPaperType == "dina4" || lowerPaperType == "a4")
	{
		return PT_DIN_A4;
	}

	if (lowerPaperType == "letter")
	{
		return PT_LETTER;
	}

	if (lowerPaperType == "tabloid" || lowerPaperType == "ledger")
	{
		return PT_TABLOID;
	}

	ocean_assert(false && "Unknown paper type!");
	return PT_INVALID;
}

std::string MetricSize::translatePaperType(const PaperType paperType)
{
	switch (paperType)
	{
		case PT_INVALID:
			break;

		case PT_DIN_A3:
			return std::string("a3");

		case PT_DIN_A4:
			return std::string("a4");

		case PT_LETTER:
			return std::string("letter");

		case PT_TABLOID:
			return std::string("tabloid");
	}

	ocean_assert(false && "Invalid paper type!");
	return std::string("invalid");
}

}

}

}
