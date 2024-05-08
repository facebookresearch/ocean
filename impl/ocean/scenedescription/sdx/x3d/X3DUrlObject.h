/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_URL_OBJECT_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_URL_OBJECT_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"

#include "ocean/scenedescription/SDXNode.h"
#include "ocean/scenedescription/Field1D.h"

#include "ocean/io/FileResolver.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d url object.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DUrlObject : virtual public SDXNode
{
	public:

		/**
		 * Resolves the given urls and returns a list of possible filenames.
		 * @param filename Name of a file helping to resolve the urls
		 * @param urls Urls to be resolved
		 * @return Resolved filenames
		 */
		static IO::Files resolveUrls(const std::string& filename, const MultiString::Values& urls);

	protected:

		/**
		 * Creates a new abstract x3d url object.
		 * @param environment Node environment
		 */
		explicit X3DUrlObject(const SDXEnvironment* environment);

	protected:

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);

		/**
		 * Resolves the urls and returns a list of possible filenames.
		 * @return Resolved filenames
		 */
		IO::Files resolveUrls() const;

	protected:

		/// Url field.
		MultiString url_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_URL_OBJECT_H
