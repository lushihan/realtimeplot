#include "cairo_plot/plot.h"

using namespace cairo_plot;

int main() {
	PlotConfig config = PlotConfig();
	config.min_x = -10;
	config.max_x = 90;
	config.min_y = -20;
	config.max_y = 30;
	config.aspect_ratio = 0.8;

	Plot pl = Plot( config );
	for (int i=0; i<10;++i) {
		pl.point( (i-1)*10, (i-4)*5 );
		sleep(1);
	}
	pl.point( 10, 10 );
	return 0;
}