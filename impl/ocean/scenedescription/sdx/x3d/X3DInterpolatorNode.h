/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_INTERPOLATOR_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_INTERPOLATOR_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DChildNode.h"

#include <map>

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d interpolator node object.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DInterpolatorNode : virtual public X3DChildNode
{
	protected:

		/**
		 * Definition of a pair holding two key value indices.
		 */
		typedef std::pair<size_t, size_t> IndexPair32;

		/**
		 * Definition of a map mapping keys to key value index pairs.
		 */
		typedef std::map<Scalar, IndexPair32> KeyMap;

	protected:

		/**
		 * Creates a new abstract x3d interpolator node.
		 * @param environment Node environment
		 */
		explicit X3DInterpolatorNode(const SDXEnvironment* environment);

	protected:

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);

		/**
		 * Event function to inform the node that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * @see SDXNode::onInitialize().
		 */
		void onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp) override;

		/**
		 * Explicit changing event function for node fields.
		 * @see X3DNode::onFieldChanging().
		 */
		bool onFieldChanging(const std::string& fieldName, const Field& field) override;

		/**
		 * Event function for single value 'interpolates'.
		 * @param index Index of the element to 'interpolate'
		 * @param eventTimestamp Timestamp of the faction event
		 */
		virtual void onSingleValue(const size_t index, const Timestamp eventTimestamp) = 0;

		/**
		 * Event function to interpolate between two values.
		 * @param leftIndex Index of the left (first) interpolation value
		 * @param rightIndex Index of the right (second) interpolation value
		 * @param interpolationFactor Interpolation factor between the left and the right value with range [0, 1]
		 * @param eventTimestamp Timestamp of the faction event
		 */
		virtual void onInterpolate(const size_t leftIndex, const size_t rightIndex, const Scalar interpolationFactor, const Timestamp eventTimestamp) = 0;

	protected:

		/// Set_fraction field.
		SingleFloat setFraction_;

		/// Key field.
		MultiFloat key_;

		/// Map mapping interpolation keys to interpolation value indices.
		KeyMap keyMap_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_INTERPOLATOR_NODE_H
