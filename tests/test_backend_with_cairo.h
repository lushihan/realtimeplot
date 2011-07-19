#include <cxxtest/TestSuite.h>

#include <boost/shared_ptr.hpp>

#include "realtimeplot/backend.h"
#include "realtimeplot/eventhandler.h"
#include "realtimeplot/events.h"

using namespace realtimeplot;
namespace realtimeplot {
	class TestBackendWithCairo : public CxxTest::TestSuite 
	{
		public:
			Display *dpy;
			Window win;
			Cairo::RefPtr<Cairo::XlibSurface> xSurface;
			Cairo::RefPtr<Cairo::Context> xContext;

			void testOpenAndClosePlot() {
				PlotConfig config = PlotConfig();
				boost::shared_ptr<EventHandler> pEventHandler( new EventHandler( config ) );
				pEventHandler->add_event( boost::shared_ptr<Event>( new OpenPlotEvent( config, 
								pEventHandler ) )  );
				pEventHandler->add_event( boost::shared_ptr<Event>( new FinalEvent(pEventHandler, false ) ) );
				pEventHandler->pEventProcessingThrd->join();
			}

			void testDrawPoint() {
				PlotConfig config = PlotConfig();
				boost::shared_ptr<EventHandler> pEventHandler( new EventHandler( config ) );
				pEventHandler->add_event( boost::shared_ptr<Event>( new OpenPlotEvent( config, 
								pEventHandler ) )  );
				pEventHandler->add_event( boost::shared_ptr<Event>( new PointEvent(5, 20) ) );
				pEventHandler->add_event( boost::shared_ptr<Event>( new FinalEvent(pEventHandler, false ) ) );
				pEventHandler->pEventProcessingThrd->join();
			}

			void testResetPlot() {
				PlotConfig config = PlotConfig();
				PlotConfig conf = PlotConfig();
				conf.xlabel = "second window";
				boost::shared_ptr<EventHandler> pEventHandler( new EventHandler( config ) );
				pEventHandler->add_event( boost::shared_ptr<Event>( new OpenPlotEvent( config, 
								pEventHandler ) )  );

				std::vector<boost::shared_ptr<Event> > events(2);
				events[0] = boost::shared_ptr<Event>( new CloseWindowEvent() );
				config = conf;
				events[1] = boost::shared_ptr<Event>( new OpenPlotEvent( config, 
							pEventHandler ) );
				pEventHandler->add_event( 
						boost::shared_ptr<Event>( new MultipleEvents( events ) ));

				pEventHandler->add_event( boost::shared_ptr<Event>( new FinalEvent(pEventHandler, false ) ) );
				pEventHandler->pEventProcessingThrd->join();
			}


	};
};
	
