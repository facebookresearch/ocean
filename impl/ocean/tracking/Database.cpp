/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/Database.h"

namespace Ocean
{

namespace Tracking
{

Database::PoseImagePointTopologyGroups Database::objectPointTopology(const TopologyTriples& topologyTriples, const Indices32* indices)
{
	ocean_assert(indices == nullptr || indices->size() <= topologyTriples.size());

	PoseImagePointTopologyGroups result;
	Index32To32Map objectPointId2Index;

	if (indices)
	{
		for (Indices32::const_iterator i = indices->begin(); i != indices->end(); ++i)
		{
			ocean_assert(*i < topologyTriples.size());
			const TopologyTriple& triple = topologyTriples[*i];

			Index32To32Map::const_iterator iI = objectPointId2Index.find(triple.objectPointId());
			if (iI == objectPointId2Index.end())
			{
				iI = objectPointId2Index.insert(std::make_pair(triple.objectPointId(), (unsigned int)result.size())).first;
				result.push_back(std::make_pair(triple.objectPointId(), PoseImagePointTopology()));
			}

			ocean_assert(iI->first == result[iI->second].first);
			result[iI->second].second.push_back(PoseImagePointPair(triple.poseId(), triple.imagePointId()));
		}
	}
	else
	{
		for (TopologyTriples::const_iterator i = topologyTriples.begin(); i != topologyTriples.end(); ++i)
		{
			const TopologyTriple& triple = *i;

			Index32To32Map::const_iterator iI = objectPointId2Index.find(triple.objectPointId());
			if (iI == objectPointId2Index.end())
			{
				iI = objectPointId2Index.insert(std::make_pair(triple.objectPointId(), (unsigned int)result.size())).first;
				result.push_back(std::make_pair(triple.objectPointId(), PoseImagePointTopology()));
			}

			ocean_assert(iI->first == result[iI->second].first);
			result[iI->second].second.push_back(PoseImagePointPair(triple.poseId(), triple.imagePointId()));
		}
	}

	return result;
}

}

}
