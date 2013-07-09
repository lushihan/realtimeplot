/*
	 -------------------------------------------------------------------

	 Copyright (C) 2010, 2011 Edwin van Leeuwen

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
#include <cxxtest/TestSuite.h>

#include "testhelpers.h"

#include "realtimeplot/actor.hh"
#include "realtimeplot/backend.h"

using namespace realtimeplot;
class TestActor : public CxxTest::TestSuite 
{
	public:
		void wait_for_exit( actor_ptr actor ) {
			actor << make_any_tuple( atom("close") );
			bool open = true;
			do_receive (
				on( atom("DONE") ) >> [&open]() { open = false; }
			).until( gref(open) == false );
		}

		void testClose() {
			actor_ptr actor = spawn<Actor>();
			wait_for_exit( actor );
			TS_ASSERT( true );
		}

		void testOpen() {
			actor_ptr actor = spawn<Actor>();
			actor << make_any_tuple( atom("open_test"), "plot" );
			actor << make_any_tuple( atom("save"), fn("empty_plot") );
			wait_for_exit( actor );
			TS_ASSERT( check_plot( "empty_plot" ) );
		}

		void testPoint() {
			actor_ptr actor = spawn<Actor>();
			actor << make_any_tuple( atom("open_test"), "plot" );
			actor << make_any_tuple( atom("point"), 1, 1 );
			actor << make_any_tuple( atom("save"), fn("point_plot") );
			wait_for_exit( actor );
			TS_ASSERT( check_plot( "point_plot" ) );
		}

		void testColor() {
			actor_ptr actor = spawn<Actor>();
			actor << make_any_tuple( atom("open_test"), "plot" );
			actor << make_any_tuple( atom("color"), Color::red() );
			actor << make_any_tuple( atom("point"), 1, 1 );
			actor << make_any_tuple( atom("save"), fn("red_point_plot") );
			wait_for_exit( actor );
			TS_ASSERT( check_plot( "red_point_plot" ) );
		}

		void testPlotLines() { // Need to change id and then add point
			actor_ptr actor = spawn<Actor>();
			actor << make_any_tuple( atom("open_test"), "plot" );
			actor << make_any_tuple( atom("color"), Color::green() );
			actor << make_any_tuple( atom("line_id"), 1 );
			actor << make_any_tuple( atom("line_add"), 1, 1 );
			actor << make_any_tuple( atom("color"), Color::red() );
			actor << make_any_tuple( atom("line_id"), 2 );
			actor << make_any_tuple( atom("line_add"), 1, 1 );
			actor << make_any_tuple( atom("line_id"), 1 );
			actor << make_any_tuple( atom("line_add"), 1, 2 );
			actor << make_any_tuple( atom("line_add"), 2, 2 );
			actor << make_any_tuple( atom("line_id"), 2 );
			actor << make_any_tuple( atom("line_add"), -1, 2 );
			actor << make_any_tuple( atom("line_add"), -2, 2 );
			actor << make_any_tuple( atom("save"), fn("line_plot1") );
			wait_for_exit( actor );
			TS_ASSERT( check_plot( "line_plot1" ) );
		}
};
