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

#include "realtimeplot/backend.h"

using namespace realtimeplot;

class TestBackend : public CxxTest::TestSuite 
{
	public:
		PlotConfig conf;

		void setUp() {
			conf = PlotConfig();
			conf.area = 50*50;
			conf.min_x = -5;
			conf.max_x = 5;
			conf.min_y = -5;
			conf.max_y = 5;
			conf.margin_x = 20;
			conf.margin_y = 20;
			conf.display = false;
		}

		void testPlotConfig() {
			PlotConfig conf = PlotConfig();
			TS_ASSERT_EQUALS( conf.max_x, 1 );
			TS_ASSERT_EQUALS( conf.min_x, 0 );
			TS_ASSERT_EQUALS( conf.max_y, 1 );
			TS_ASSERT_EQUALS( conf.min_y, 0 );
		}

		void testEmptyPlot() {
			BackendPlot bpl = BackendPlot( conf, boost::shared_ptr<EventHandler>()  );
			bpl.save( fn( "empty_plot" ) );
			TS_ASSERT( check_plot( "empty_plot" ) );
		}

		void testPointPlot() {
			BackendPlot bpl = BackendPlot( conf, boost::shared_ptr<EventHandler>()  );
			bpl.point( 1, 1 );
			bpl.save( fn( "point_plot" ) );
			TS_ASSERT( check_plot( "point_plot" ) );
		}

		void testScaling() {
			conf.scaling = true;
			BackendPlot bpl = BackendPlot( conf, boost::shared_ptr<EventHandler>()  );
			bpl.point( 1, 1 );
			bpl.save( fn( "point_plot" ) );
			TS_ASSERT( check_plot( "point_plot" ) );
			bpl.scale_xsurface( 120, 90 );
			bpl.save( fn( "scaled_plot" ) );
			TS_ASSERT( check_plot( "scaled_plot" ) );
		}

		void testLinePlot() {
			//conf.area = 500*500;
			BackendPlot bpl = BackendPlot( conf, boost::shared_ptr<EventHandler>()  );
			bpl.line_add( 1, 1, 1, Color::green() );
			bpl.line_add( 1, 1, 2, Color::red() );
			bpl.line_add( 1, 2, 1, Color::green() );
			bpl.line_add( -1, 2, 2, Color::red() );
			bpl.line_add( 2, 2, 1, Color::green() );
			bpl.line_add( -2, 2, 2, Color::red() );
			bpl.save( fn( "line_plot1" ) );
			TS_ASSERT( check_plot( "line_plot1" ) );
			// Make sure lines get cleared, i.e. line 1 is not at 2,2 anymore
			bpl.clear();
			bpl.line_add( 1, 1, 1, Color::green() );
			bpl.line_add( -1, 2, 1, Color::green() );
			bpl.save( fn( "line_plot2" ) );
			TS_ASSERT( check_plot( "line_plot2" ) );
		}

		void testReset() {
			//conf.area = 500*500;
			BackendPlot bpl = BackendPlot( conf, boost::shared_ptr<EventHandler>()  );
			bpl.point( 1, 1 );
			bpl.save( fn( "point_plot" ) );
			TS_ASSERT( check_plot( "point_plot" ) );
			PlotConfig new_conf = PlotConfig();
			new_conf.area = conf.area;
			new_conf.max_x = 2;
			new_conf.max_y = 2;
			bpl.reset( new_conf );
			TS_ASSERT_EQUALS( bpl.config.max_x, 2 );
			TS_ASSERT_EQUALS( bpl.config.max_y, 2 );
			TS_ASSERT_EQUALS( bpl.pPlotArea->max_x, 6 );
			TS_ASSERT_EQUALS( bpl.pPlotArea->max_y, 6 );
			TS_ASSERT_EQUALS( bpl.pPlotArea->min_x, -4 );
			TS_ASSERT_EQUALS( bpl.pPlotArea->min_y, -4 );
			bpl.save( fn( "bpl_reset" ) );
			TS_ASSERT( check_plot( "bpl_reset" ) );
		}

		void testRollingUpdate() {
			//conf.area = 500*500;
			conf.overlap = 0.6;
			BackendPlot bpl = BackendPlot( conf, boost::shared_ptr<EventHandler>()  );
			bpl.point( 0, 0 );
			bpl.point( -7, -7 );
			bpl.save( fn( "bpl_rolling1" ) );
			TS_ASSERT( check_plot( "bpl_rolling1" ) );
			// Rolling beyond PlotArea.surface
			bpl.point( 52, -52 );
			bpl.point( 56, -56 );
			bpl.point( 57, -57 );
			bpl.save( fn( "bpl_rolling2" ) );
			TS_ASSERT( check_plot( "bpl_rolling2" ) );
		}

		/*
		 * Histogram
		 */
		void testHistogramFixed() {
			conf.area = 500*500;
			conf.fixed_plot_area = true;
			BackendHistogram bh = BackendHistogram( conf, true, 20,
					boost::shared_ptr<EventHandler>() );
			TS_ASSERT_EQUALS( bh.frequency, true );
			TS_ASSERT_EQUALS( bh.no_bins, 20 );
			TS_ASSERT_EQUALS( bh.rebin, false );
			TS_ASSERT_EQUALS( bh.config.min_y, 0 );
			TS_ASSERT_DELTA( bh.config.max_y, 1.2, 1e-5 );
			TS_ASSERT_EQUALS( bh.bin_width, 0.5 );
			TS_ASSERT_EQUALS( bh.bins_y.size(), 20 );
		}

		void testHistogramFixedAddData() {
			conf.area = 500*500;
			conf.fixed_plot_area = true;
			BackendHistogram bh = BackendHistogram( conf, true, 20,
					boost::shared_ptr<EventHandler>() );

			bh.add_data( 0.1 );
			TS_ASSERT_EQUALS( bh.data.size(), 1 );
			TS_ASSERT_EQUALS( bh.data[0], 0.1 );
			TS_ASSERT_EQUALS( bh.rebin, false );
			bh.add_data( 6 );
			TS_ASSERT_EQUALS( bh.data.size(), 2 );
			TS_ASSERT_EQUALS( bh.data[1], 6 );
			TS_ASSERT_EQUALS( bh.rebin, false );
		}
	
		void testHistogramAdjust() {
			conf.area = 500*500;
			conf.fixed_plot_area = false;
			BackendHistogram bh = BackendHistogram( conf, true, 20,
					boost::shared_ptr<EventHandler>() );
			TS_ASSERT_EQUALS( bh.frequency, true );
			TS_ASSERT_EQUALS( bh.no_bins, 20 );
			TS_ASSERT_EQUALS( bh.rebin, true );
			TS_ASSERT_EQUALS( bh.config.min_y, 0 );
			TS_ASSERT_DELTA( bh.config.max_y, 1.2, 1e-5 );
			TS_ASSERT_EQUALS( bh.config.min_x, 0 );
			TS_ASSERT_EQUALS( bh.config.max_x, 1 );
			TS_ASSERT_DELTA( bh.bin_width, 0.05, 1e-4 );
			TS_ASSERT_EQUALS( bh.bins_y.size(), 20 );
		}

		void testHistogramSimple() {
			//conf.area = 500*500;
			conf.min_y = 0;
			conf.min_x = 0;
			conf.max_x = 5;

			BackendHistogram bhm = BackendHistogram( conf, 
					boost::shared_ptr<EventHandler>(), 0, 5, 20 );
			bhm.plot();
			bhm.save( fn( "bhm_empty" ) );
			TS_ASSERT( check_plot( "bhm_empty" ) );
			bhm.add_data( 1.1, true, true, 20, true );
			bhm.plot();
			bhm.save( fn( "bhm_data1" ) );
			TS_ASSERT( check_plot( "bhm_data1" ) );
			bhm.add_data( 3.1, true, true, 20, true );
			bhm.add_data( 3.1, true, true, 20, true );
			bhm.save( fn( "bhm_data2" ) );
			TS_ASSERT( check_plot( "bhm_data2" ) );
			bhm.add_data( 3.1, true, false, 20, true );
			bhm.save( fn( "bhm_data3" ) );
			TS_ASSERT( check_plot( "bhm_data3" ) );
			bhm.add_data( 6.1, true, false, 20, true );
			// Nothing should have changed
			bhm.save( fn( "bhm_data3" ) );
			TS_ASSERT( check_plot( "bhm_data3" ) );
			bhm.add_data( 6.1, true, false, 20, false );
			bhm.save( fn( "bhm_data4" ) );
			TS_ASSERT( check_plot( "bhm_data4" ) );
		}

		/*
		 * Histogram3d
		 */

		/*
		 * HeightMap
		 */
		void testHeightMapSimple() {
			conf.area = 60*60;
			BackendHeightMap bhm = BackendHeightMap( conf, 
					boost::shared_ptr<EventHandler>() );
			bhm.add_data( 0,0,1, true );
			bhm.add_data( 0.1,1,0.2, true );
			bhm.add_data( -1,1.1,0.2, true );
			bhm.save( fn( "bhm_3points" ) );
			TS_ASSERT( check_plot( "bhm_3points" ) );
			bhm.calculate_height_scaling();
			bhm.plot();
			bhm.save( fn( "bhm_3points_rescale" ) );
			TS_ASSERT( check_plot( "bhm_3points_rescale" ) );
		}


		void testVertex3DCrossProduct() {
			boost::shared_ptr<Vertex3D> pV1( new Vertex3D( 1,0,0 ) );
			boost::shared_ptr<Vertex3D> pV2( new Vertex3D( 0,1,0 ) );
			boost::shared_ptr<Vertex3D> pV3 = pV1->crossProduct( pV2 );
			TS_ASSERT_EQUALS( pV3->x, 0 );
			TS_ASSERT_EQUALS( pV3->y, 0 );
			TS_ASSERT_EQUALS( pV3->z, 1 );
		}

		void testTriangle3DgradientVector() {
			boost::shared_ptr<Vertex3D> pV1( new Vertex3D( 1,0,0 ) );
			boost::shared_ptr<Vertex3D> pV2( new Vertex3D( 0,1,0 ) );
			boost::shared_ptr<Vertex3D> pV3( new Vertex3D( 1,1,1 ) );

			Triangle3D tr = Triangle3D();
			tr.vertices.push_back( pV1 );
			tr.vertices.push_back( pV2 );
			tr.vertices.push_back( pV3 );
			std::vector<boost::shared_ptr<Vertex3D> > v;
			v = tr.gradientVector();
			TS_ASSERT_EQUALS( v[0]->x, 1 );
			TS_ASSERT_EQUALS( v[0]->y, 0 );
			TS_ASSERT_EQUALS( v[0]->z, 0 );
			TS_ASSERT_EQUALS( v[1]->x, 1.5 );
			TS_ASSERT_EQUALS( v[1]->y, 0.5 );
			TS_ASSERT_EQUALS( v[1]->z, 1 );

			TS_ASSERT_EQUALS( pV3->x, 1 );
			TS_ASSERT_EQUALS( pV3->y, 1 );
			TS_ASSERT_EQUALS( pV3->z, 1 );

			Triangle3D tr2 = Triangle3D();
			tr2.vertices.push_back( pV3 );
			tr2.vertices.push_back( pV1 );
			tr2.vertices.push_back( pV2 );
			v = tr2.gradientVector();
			TS_ASSERT_EQUALS( v[0]->x, 1 );
			TS_ASSERT_EQUALS( v[0]->y, 0 );
			TS_ASSERT_EQUALS( v[0]->z, 0 );
			TS_ASSERT_EQUALS( v[1]->x, 1.5 );
			TS_ASSERT_EQUALS( v[1]->y, 0.5 );
			TS_ASSERT_EQUALS( v[1]->z, 1 );
		}
};
	
