// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
