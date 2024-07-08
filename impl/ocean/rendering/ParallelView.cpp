/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/ParallelView.h"

namespace Ocean
{

namespace Rendering
{

ParallelView::ParallelView() :
	View()
{
	// nothing to do here
}

ParallelView::~ParallelView()
{
	// nothing to do here
}

ParallelView::ObjectType ParallelView::type() const
{
	return TYPE_PARALLEL_VIEW;
}

}

}
