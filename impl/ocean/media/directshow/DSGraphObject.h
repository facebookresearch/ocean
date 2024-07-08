/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_GRAPH_OBJECT_H
#define META_OCEAN_MEDIA_DS_GRAPH_OBJECT_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSObject.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class is the base class for all object holding a DirectShow filter graph manager.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSGraphObject : virtual public DSObject
{
	protected:

		/**
		 * Definition of a vector holding DirectShow filters.
		 */
		typedef std::vector<ScopedIBaseFilter> Filters;

	protected:

		/**
		 * Creates a new DSGraphObject element.
		 */
		DSGraphObject() = default;

		/**
		 * Destructs a DSGraphObject.
		 */
		virtual ~DSGraphObject();

		/**
		 * Creates a new filter graph builder.
		 * @return True, if succeeded
		 */
		bool createGraphBuilder();

		/**
		 * Releases a filter graph builder.
		 */
		void releaseGraphBuilder();

		/**
		 * Builds the entire filter graph.
		 * @return True, if succeeded
		 */
		virtual bool buildGraph() = 0;

		/**
		 * Releases the entire filter graph.
		 */
		virtual void releaseGraph() = 0;

		/**
		 * Stars the filter graph.
		 * @return True, if succeeded
		 */
		virtual bool startGraph();

		/**
		 * Pauses the filter graph.
		 * @return True, if succeeded
		 */
		virtual bool pauseGraph();

		/**
		 * Stops the filter graph.
		 * @return True, if succeeded
		 */
		virtual bool stopGraph();

		/**
		 * Adds a new filter to the filter graph.
		 * The specified filter will be created and afterwards added to the filter graph.
		 * @param classIdentifier The filter's class identifier to add
		 * @param name The name of the resulting filter
		 * @return Pointer to the filter if succeeded
		 */
		ScopedIBaseFilter addFilter(const GUID& classIdentifier, const std::wstring& name);

		/**
		 * Connects two filters by using the first free pins.
		 * @param outputFilter Filter with a free output pin
		 * @param inputFilter Filter with a free input pin
		 * @param type Optional type of the filter connection
		 * @return True, if succeeded
		 */
		bool connectFilters(IBaseFilter* outputFilter, IBaseFilter* inputFilter, const AM_MEDIA_TYPE* type = nullptr);

		/**
		 * Connects a pin with the first free pin of a filter.
		 * @param outputPin Output pin of a filter
		 * @param inputFilter Filter with a free input pin
		 * @param type Optional type of the filter connection
		 * @return True, if succeeded
		 */
		bool connectFilter(IPin* outputPin, IBaseFilter* inputFilter, const AM_MEDIA_TYPE* type = nullptr);

		/**
		 * Connects a pin with the first free pin of a filter.
		 * @param outputFilter Filter with a free output pin
		 * @param inputPin Input pin of a filter
		 * @param type Optional type of the filter connection
		 * @return True, if succeeded
		 */
		bool connectFilter(IBaseFilter* outputFilter, IPin* inputPin, const AM_MEDIA_TYPE* type = nullptr);

		/**
		 * Returns all filters which are part of the current filter graph.
		 * The caller has to release all filters after usage.
		 * @return Enumerated filters
		 */
		Filters filters();

#ifdef OCEAN_DEBUG

		/**
		 * Adds the filter graph to the running object table.
		 */
		bool addFilterGraphToROT();

		/**
		 * Removes the filter graph from the running object table.
		 */
		bool removeFilterGraphFromROT();

#endif

	protected:

		/// DirectShow filter graph manager.
		ScopedIGraphBuilder filterGraph_;

		/// DirectShow media control interface.
		ScopedIMediaControl mediaControlInterface_;

		/// DirectShow media event interface.
		ScopedIMediaEvent mediaEventInterface_;

		/// Indicates whether the graph is running.
		bool graphIsRunning_ = false;

	private:

#ifdef OCEAN_DEBUG

		/// DirectShow ROT interface.
		DWORD rotId_ = DWORD(0);

#endif
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_GRAPH_OBJECT_H
