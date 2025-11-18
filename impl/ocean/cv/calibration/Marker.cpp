/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/calibration/Marker.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

bool Marker::LayoutManager::layoutPointSign(const size_t markerId, const bool markerSign, const CV::PixelDirection& orientation, const size_t xUnoriented, const size_t yUnoriented)
{
	assert(xUnoriented < 5 && yUnoriented < 5);

	size_t rotatedX = xUnoriented;
	size_t rotatedY = yUnoriented;

	switch (orientation)
	{
		case CV::PD_NORTH:
			break;

		case CV::PD_WEST:
			rotatedX = 4 - yUnoriented;
			rotatedY = xUnoriented;
			break;

		case CV::PD_SOUTH:
			rotatedX = 4 - xUnoriented;
			rotatedY = 4 - yUnoriented;
			break;

		case CV::PD_EAST:
			rotatedX = yUnoriented;
			rotatedY = 4 - xUnoriented;
			break;

		default:
			ocean_assert(false && "This should never happen!");
			break;
	}

	ocean_assert(rotatedX < 5 && rotatedY < 5);

	ocean_assert(markerId < layouts().size());

	const Layout& layout = layouts()[markerId];

	const uint8_t value = layout[rotatedY * 5 + rotatedX];
	ocean_assert(value == 0u || value == 1u);

	bool sign = value != 0u;

	if (!markerSign)
	{
		sign = !sign;
	}

	return sign;
}

const Marker::LayoutManager::Layouts& Marker::LayoutManager::layouts()
{
	static const Layouts layouts = determineUniqueLayouts();

	return layouts;
}

Marker::LayoutManager::Layouts Marker::LayoutManager::determineUniqueLayouts()
{
	//**TODO** this function can be optimized

	Layouts layouts;
	layouts.reserve(96);

	Layout candidateLayout;

	// let's test all candidates with one bit change

	for (size_t nOuter = 0; nOuter < 25; ++nOuter)
	{
		const size_t yOuter = nOuter / 5;
		const size_t xOuter = nOuter % 5;

		if (yOuter == 0 || xOuter == 0 || yOuter == 4 || xOuter == 4)
		{
			continue;
		}

		memset(candidateLayout.data(), 1, sizeof(Layout));

		candidateLayout[nOuter] = 0u;

		if (!isSimilar(layouts, candidateLayout))
		{
			layouts.push_back(candidateLayout);
		}
	}

	// let's test all candidates with two bit changes

	for (size_t nOuter = 0; nOuter < 24; ++nOuter)
	{
		const size_t yOuter = nOuter / 5;
		const size_t xOuter = nOuter % 5;

		if (yOuter == 0 || xOuter == 0 || yOuter == 4 || xOuter == 4)
		{
			continue;
		}

		for (size_t nInner = nOuter + 1; nInner < 25; ++nInner)
		{
			const size_t yInner = nInner / 5;
			const size_t xInner = nInner % 5;

			if (yInner == 0 || xInner == 0 || yInner == 4 || xInner == 4)
			{
				continue;
			}

			if (std::abs(int(yOuter) - int(yInner)) <= 1 && std::abs(int(xOuter) - int(xInner)) <= 1)
			{
				// let's aways that two bits are neighbors
				continue;
			}

			memset(candidateLayout.data(), 1, sizeof(Layout));

			candidateLayout[nOuter] = 0u;
			candidateLayout[nInner] = 0u;

			if (!isSimilar(layouts, candidateLayout))
			{
				layouts.push_back(candidateLayout);
			}
		}
	}

	// let's test all candidates with three bit changes

	for (size_t nOuter = 0; nOuter < 24; ++nOuter)
	{
		const size_t yOuter = nOuter / 5;
		const size_t xOuter = nOuter % 5;

		if (yOuter == 0 || xOuter == 0 || yOuter == 4 || xOuter == 4)
		{
			continue;
		}

		for (size_t nInner = nOuter + 1; nInner < 25; ++nInner)
		{
			const size_t yInner = nInner / 5;
			const size_t xInner = nInner % 5;

			if (yInner == 0 || xInner == 0 || yInner == 4 || xInner == 4)
			{
				continue;
			}

			for (size_t nCore = nInner + 1; nCore < 25; ++nCore)
			{
				const size_t yCore = nCore / 5;
				const size_t xCore = nCore % 5;

				if (yCore == 0 || xCore == 0 || yCore == 4 || xCore == 4)
				{
					continue;
				}

				memset(candidateLayout.data(), 1, sizeof(Layout));

				candidateLayout[nOuter] = 0u;
				candidateLayout[nInner] = 0u;
				candidateLayout[nCore] = 0u;

				if (!isSimilar(layouts, candidateLayout))
				{
					layouts.push_back(candidateLayout);
				}
			}
		}
	}

	// let's test all candidates with four bit changes

	for (size_t nOuter = 0; nOuter < 24; ++nOuter)
	{
		const size_t yOuter = nOuter / 5;
		const size_t xOuter = nOuter % 5;

		if (yOuter == 0 || xOuter == 0 || yOuter == 4 || xOuter == 4)
		{
			continue;
		}

		for (size_t nInner = nOuter + 1; nInner < 25; ++nInner)
		{
			const size_t yInner = nInner / 5;
			const size_t xInner = nInner % 5;

			if (yInner == 0 || xInner == 0 || yInner == 4 || xInner == 4)
			{
				continue;
			}

			for (size_t nCore = nInner + 1; nCore < 25; ++nCore)
			{
				const size_t yCore = nCore / 5;
				const size_t xCore = nCore % 5;

				if (yCore == 0 || xCore == 0 || yCore == 4 || xCore == 4)
				{
					continue;
				}

				for (size_t nCore2 = nCore + 1; nCore2 < 25; ++nCore2)
				{
					const size_t yCore2 = nCore2 / 5;
					const size_t xCore2 = nCore2 % 5;

					if (yCore2 == 0 || xCore2 == 0 || yCore2 == 4 || xCore2 == 4)
					{
						continue;
					}

					memset(candidateLayout.data(), 1, sizeof(Layout));

					candidateLayout[nOuter] = 0u;
					candidateLayout[nInner] = 0u;
					candidateLayout[nCore] = 0u;
					candidateLayout[nCore2] = 0u;

					if (!isSimilar(layouts, candidateLayout))
					{
						layouts.push_back(candidateLayout);
					}
				}
			}
		}
	}

	return layouts;
}

bool Marker::LayoutManager::isSimilar(const Layouts& layouts, const Layout& layout, const bool checkSelfSimilarity)
{
	if (checkSelfSimilarity)
	{
		if (isSimilar(layout, layout, false /*checkIdentity*/))
		{
			return true;
		}
	}

	for (const Layout& layoutA : layouts)
	{
		ocean_assert(isSimilar(layoutA, layout) == isSimilar(layout, layoutA));

		if (isSimilar(layoutA, layout, true /*checkIdentity*/))
		{
			return true;
		}
	}

	return false;
}

CV::PixelDirection Marker::LayoutManager::isRotated(const Layout& layout, const Layout& rotatedLayout, const bool checkIdentity)
{
	/**
	 * Indices of the 25 elements on one layout
	 *  0  1  2  3  4
	 *  5  6  7  8  9
	 * 10 11 12 13 14
	 * 15 16 17 18 19
	 * 20 21 22 23 24
	 */

	if (checkIdentity)
	{
		if (memcmp(layout.data(), rotatedLayout.data(), sizeof(Layout)) == 0)
		{
			return CV::PD_NORTH;
		}
	}

	// in case checkIdentity == true, the layout can still be identical and rotated (e.g., a symmetric layout)

	/**
	 * Rotation by 90 degree to the right
	 * 20 15 10 5 0
	 * 21 16 11 6 1
	 * 22 17 12 7 2
	 * 23 18 13 8 3
	 * 24 19 14 9 4
	 */
	static constexpr std::array<uint8_t, 25> mapping90 =
	{
		20, 15, 10, 5, 0,
		21, 16, 11, 6, 1,
		22, 17, 12, 7, 2,
		23, 18, 13, 8, 3,
		24, 19, 14, 9, 4
	};

	bool identical = true;
	for (size_t n = 0; n < 25; ++n)
	{
		if (layout[n] != rotatedLayout[mapping90[n]])
		{
			identical = false;
			break;
		}
	}

	if (identical)
	{
		return CV::PD_EAST;
	}

	/**
	 * Rotation by 180 degree, simply indices with reverse order
	 * 24 23 22 21 20
	 * 19 18 17 16 15
	 * 14 13 12 11 10
	 *  9  8  7  6  5
	 *  4  3  2  1  0
	 */

	identical = true;
	for (size_t n = 0; n < 25; ++n)
	{
		if (layout[n] != rotatedLayout[24 - n])
		{
			identical = false;
			break;
		}
	}

	if (identical)
	{
		return CV::PD_SOUTH;
	}

	/**
	 * Rotation by 90 degree to the left, reverse indices of 90 degree to the right
	 * 4 9 14 19 24
	 * 3 8 13 18 23
	 * 2 7 12 17 22
	 * 1 6 11 16 21
	 * 0 5 10 15 20
	 */

	identical = true;
	for (size_t n = 0; n < 25; ++n)
	{
		if (layout[n] != rotatedLayout[mapping90[24 - n]])
		{
			identical = false;
			break;
		}
	}

	if (identical)
	{
		return CV::PD_WEST;
	}

	return CV::PD_INVALID;
}

}

}

}
