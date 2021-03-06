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

#ifndef REALTIMEPLOT_XCBHANDLER_H
#define REALTIMEPLOT_XCBHANDLER_H
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#ifndef NO_X
#include <xcb/xcb.h>
#endif

#include <cairomm/surface.h>

#include "realtimeplot/eventhandler.h"


namespace realtimeplot {
	/**
	 * \brief Base class that handles display connections
	 *
	 * Plotting backends (i.e. xcb, xlib, gtk) should be derived from this base
	 * class.
	 */
	class DisplayHandler {
		public:
			//static DisplayHandler* Instance();
			boost::mutex map_mutex; 
			/**
			 * \brief Opens a window and returns an id
			 */
			virtual size_t open_window( size_t width, size_t height,
					boost::shared_ptr<EventHandler> pEventHandler = 
					boost::shared_ptr<EventHandler>() ) = 0;

			/**
			 * \brief Return a cairo surface that draws onto a window
			 *
			 * Used by BackendPlot to get a surface to draw to.
			 */
			virtual Cairo::RefPtr<Cairo::Surface> get_cairo_surface( 
					size_t window_id, size_t width, size_t height ) = 0;

			virtual void set_title( size_t window_id, std::string title ) =0;
			virtual void close_window( size_t window_id ) =0;
		protected:
			DisplayHandler() {};
			virtual ~DisplayHandler() {};
			static DisplayHandler *pInstance;
			static boost::mutex i_mutex; 

			//std::map<size_t, boost::shared_ptr<EventHandler> > mapWindow;

			//virtual void send_event( size_t window_id, boost::shared_ptr<Event> pEvent );
	};


	/**
	 *	\brief Singleton class that maintains an x_connection and handles xevents
	 *
	 * This class is used in the following way. Each plot will request a window from
	 * this class. With that request it will also give provide a ptr to itself.
	 *
	 * The handler starts a thread that handles all x events, translate them into plot
	 * events and then send them to the eventhandler that is associated with that plot.
	 * It uses a std::map to map x windows to eventhandlers.
	 *
	 */
#ifndef NO_X
	class XcbHandler : public DisplayHandler {
		public:
			xcb_connection_t *connection;

			// Should probably be enough to give access to root visual type
			// see get_root_visual_type(pXcbHandler->screen) in backend.cc
			xcb_visualtype_t *visual_type;
			static DisplayHandler* Instance();

			size_t open_window(size_t width, size_t height,
					boost::shared_ptr<EventHandler> pEventHandler = 
					boost::shared_ptr<EventHandler>() );
			Cairo::RefPtr<Cairo::Surface> get_cairo_surface( size_t window_id, size_t width, size_t height );

			void set_title( size_t window_id, std::string );
			void close_window( size_t window_id );

			/**
			 * \brief Check if we can connect to x
			 */
			static bool checkXRunning();
		protected:
			boost::shared_ptr<boost::thread> pXEventProcessingThrd;
			int mask;
			uint32_t values[2];
			xcb_screen_t *screen;
			xcb_intern_atom_reply_t* reply;
			xcb_intern_atom_reply_t* reply2;

			XcbHandler();
			~XcbHandler() { if (pXEventProcessingThrd->joinable())
					pXEventProcessingThrd->join(); }
			//static DisplayHandler *pInstance;

			void process_xevents();

			xcb_visualtype_t *get_root_visual_type(xcb_screen_t *s);
			void send_event( xcb_drawable_t window, 
					boost::shared_ptr<Event> pEvent );

			std::map<xcb_drawable_t, boost::shared_ptr<EventHandler> > mapWindow;
			std::map<size_t, xcb_drawable_t > mapWindowId;
	};
#endif

	/**
	 * \brief DisplayClass that gives out surfaces, but nothing else
	 * 
	 * Usefull if you don't want windows to open. You'll need to save plot to
	 * disk to get any result
	 */
	class DummyHandler : public DisplayHandler {
		public:
			static DisplayHandler* Instance();

			// Returns an id
			size_t open_window( size_t width, size_t height,
					boost::shared_ptr<EventHandler> pEventHandler = 
					boost::shared_ptr<EventHandler>() );

			/**
			 * \brief Return a cairo surface that draws onto a window
			 *
			 * Used by BackendPlot to get a surface to draw to.
			 */
			Cairo::RefPtr<Cairo::Surface> get_cairo_surface( size_t window_id, size_t width, size_t height );

			void set_title( size_t window_id, std::string title );
			void close_window( size_t window_id );
		protected:
			size_t latest_id;
			DummyHandler();
			~DummyHandler() {};
	};

}
#endif
