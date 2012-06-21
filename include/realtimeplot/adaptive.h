/*
	 -------------------------------------------------------------------

	 Copyright (C) 2010, Edwin van Leeuwen

	 This file is part of RealTimePlot.

	 RealTimePlot is free software; you can redistribute it and/or modify
	 it under the terms of the GNU General Public License as published by
	 the Free Software Foundation; either version 3 of the License, or
	 (at your option) any later version.

	 RealTimePlot is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	 GNU General Public License for more details.

	 You should have received a copy of the GNU General Public License
	 along with RealTimePlot. If not, see <http://www.gnu.org/licenses/>.

	 -------------------------------------------------------------------
	 */

#ifndef REALTIMEPLOT_ADAPTIVE_H
#define REALTIMEPLOT_ADAPTIVE_H
#include "realtimeplot/backend.h"
class TestAdaptive;

namespace realtimeplot {

	/**
	 * General ideas about implementation:
	 *
	 * Eventhandler instead of deleting events will keep them around
	 * for some time. Will also have a function reapply, which will basically
	 * reapply all the previous events again. Watch for first/openplot event!
	 *
	 * BackendAdaptivePlot overrides all functions that take a x/y value. If the 
	 * x/y value false outside of the plot (maybe just overrid within_bounds function
	 * or something) we redo our config and then ask the EventHandler to reaplly
	 * all previous events. Note that during that reapplication we won't need to check
	 * ranges (since everything should be in range) and also we should set it to 
	 * unmovalbe etc, so that moving events won't screw it up (pause??).
	 */


	/**
	 * \brief Adaptive plot. Will automatically choose reasonable x/y range
	 *
	 * Works by keeping last x (default 100) events and whenever a new event
	 * falls outside the current region -> Redraw the whole plot
	 */
	class BackendAdaptivePlot : public BackendPlot {
		public:
			size_t no_events;
			/**
			 * \brief Creates an Adaptive Plot 
			 *
			 * One can specify the default number of events to "remember". After more
			 * events than that we won't adapt any more.
			 */
			BackendAdaptivePlot( PlotConfig config, 
				boost::shared_ptr<EventHandler> pEventHandler, size_t no_events = 100 );
		private:
	};

	class AdaptiveEventHandler : public EventHandler {
		public:
			AdaptiveEventHandler() : EventHandler() {}

			friend class TestAdaptive;
		private:
			std::list<boost::shared_ptr<Event> > processed_events;

			void process_events();
	};
};
#endif
