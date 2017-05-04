// Example for C++ Interface to Gnuplot ( http://code.google.com/p/gnuplot-cpp )

// requirements:
// * gnuplot has to be installed (http://www.gnuplot.info/download.html)

// Standard library:
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_i.h>

#define SLEEP_LGTH 2  // sleep time in seconds
#define NPOINTS    50 // length of array

void wait_for_key();  // Program halts until keypress

int main(/*int argc, char* argv[]*/)
{

  std::clog << "*** example of gnuplot control through C++ ***" << std::endl << std::endl;
    bool interactive = false;

    // datatools::temp_file tmp_file;
    // tmp_file.set_remove_at_destroy(true);
    // tmp_file.create("/tmp", "test_gnuplot_i");

    //
    // Using the GnuplotException class
    //
    try {
        Gnuplot g1("lines");
        //if (! interactive) g1.cmd("set terminal postscript; set output '" + tmp_file.get_filename() + "';");
        //
        // Slopes
        //
        std::clog << "*** plotting slopes" << std::endl;
        g1.set_title("Slopes\\nNew Line");

        std::clog << "y = x" << std::endl;
        g1.plot_slope(1.0,0.0,"y=x");

        std::clog << "y = 2*x" << std::endl;
        g1.plot_slope(2.0,0.0,"y=2x");

        std::clog << "y = -x" << std::endl;
        g1.plot_slope(-1.0,0.0,"y=-x");
        g1.unset_title();

        //
        // Equations
        //
        g1.reset_plot();
        std::clog << std::endl << std::endl << "*** various equations" << std::endl;

        std::clog << "y = sin(x)" << std::endl;
        g1.plot_equation("sin(x)","sine");

        std::clog << "y = log(x)" << std::endl;
        g1.plot_equation("log(x)","logarithm");

        std::clog << "y = sin(x) * cos(2*x)" << std::endl;
        g1.plot_equation("sin(x)*cos(2*x)","sine product");

        //
        // Styles
        //
        g1.reset_plot();
        std::clog << std::endl << std::endl << "*** showing styles" << std::endl;

        std::clog << "sine in points" << std::endl;
        g1.set_pointsize(0.8).set_style("points");
        g1.plot_equation("sin(x)","points");

        std::clog << "sine in impulses" << std::endl;
        g1.set_style("impulses");
        g1.plot_equation("sin(x)","impulses");

        std::clog << "sine in steps" << std::endl;
        g1.set_style("steps");
        g1.plot_equation("sin(x)","steps");

        //
        // Save to ps
        //
        g1.reset_all();
        std::clog << std::endl << std::endl << "*** save to ps " << std::endl;

        std::clog << "y = sin(x) saved to test_output.ps in working directory" << std::endl;
        g1.savetops("test_output");
        g1.set_style("lines").set_samples(300).set_xrange(0,5);
        g1.plot_equation("sin(12*x)*exp(-x)").plot_equation("exp(-x)");

        g1.showonscreen(); // window output


        //
        // User defined 1d, 2d and 3d point sets
        //
        std::vector<double> x, y, y2, dy, z;

        for (int i = 0; i < NPOINTS; i++) { // fill double arrays x, y, z
            x.push_back((double)i);             // x[i] = i
            y.push_back((double)i * (double)i); // y[i] = i^2
            z.push_back( x[i]*y[i] );           // z[i] = x[i]*y[i] = i^3
            dy.push_back((double)i * (double)i / (double) 10); // dy[i] = i^2 / 10
        }
        y2.push_back(0.00); y2.push_back(0.78); y2.push_back(0.97); y2.push_back(0.43);
        y2.push_back(-0.44); y2.push_back(-0.98); y2.push_back(-0.77); y2.push_back(0.02);


        g1.reset_all();
        std::clog << std::endl << std::endl << "*** user-defined lists of doubles" << std::endl;
        g1.set_style("impulses").plot_x(y,"user-defined doubles");
        usleep(200);

        g1.reset_plot();
        std::clog << std::endl << std::endl << "*** user-defined lists of points (x,y)" << std::endl;
        g1.set_grid();
        g1.set_style("points").plot_xy(x,y,"user-defined points 2d");
        usleep(200);

        g1.reset_plot();
        std::clog << std::endl << std::endl << "*** user-defined lists of points (x,y,z)" << std::endl;
        g1.unset_grid();
        g1.plot_xyz(x,y,z,"user-defined points 3d");
        usleep(200);

        g1.reset_plot();
        std::clog << std::endl << std::endl << "*** user-defined lists of points (x,y,dy)" << std::endl;
        g1.plot_xy_err(x,y,dy,"user-defined points 2d with errorbars");
        usleep(200);


        //
        // Multiple output screens
        //
        std::clog << std::endl << std::endl;
        std::clog << "*** multiple output windows" << std::endl;

        g1.reset_plot();
        g1.set_style("lines");
        std::clog << "window 1: sin(x)" << std::endl;
        g1.set_grid().set_samples(600).set_xrange(0,300);
        g1.plot_equation("sin(x)+sin(x*1.1)");
        usleep(200);
        g1.set_xautoscale().replot();
        usleep(200);

        {
          Gnuplot g2;
          // if (! interactive) g2.cmd("set terminal postscript; set output '" + tmp_file.get_filename() + "';");
          std::clog << "window 2: user defined points" << std::endl;
          g2.plot_x(y2,"points");
          usleep(200);
          g2.set_smooth().plot_x(y2,"cspline");
          usleep(200);
          g2.set_smooth("bezier").plot_x(y2,"bezier");
          usleep(200);
          g2.unset_smooth();
          usleep(200);
        }

        {
          Gnuplot g3("lines");
          // if (! interactive) g3.cmd("set terminal postscript; set output '" + tmp_file.get_filename() + "';");
          std::clog << "window 3: log(x)/x" << std::endl;
          g3.set_grid();
          g3.plot_equation("log(x)/x","log(x)/x");
          usleep(200);
        }

        {
          Gnuplot g4("lines");
          // if (! interactive) g4.cmd("set terminal postscript; set output '" + tmp_file.get_filename() + "';");
          std::clog << "window 4: splot x*x+y*y" << std::endl;
          g4.set_zrange(0,100);
          g4.set_xlabel("x-axis").set_ylabel("y-axis").set_zlabel("z-axis");
          g4.plot_equation3d("x*x+y*y");
          usleep(200);
        }

        {
          Gnuplot g5("lines");
          // if (! interactive) g5.cmd("set terminal postscript; set output '" + tmp_file.get_filename() + "';");
          std::clog << "window 5: splot with hidden3d" << std::endl;
          g5.set_isosamples(25).set_hidden3d();
          g5.plot_equation3d("x*y*y");
          usleep(200);
        }

        {
          Gnuplot g6("lines");
          // if (! interactive) g6.cmd("set terminal postscript; set output '" + tmp_file.get_filename() + "';");
          std::clog << "window 6: splot with contour" << std::endl;
          g6.set_isosamples(60).set_contour();
          g6.unset_surface().plot_equation3d("sin(x)*sin(y)+4");
          g6.set_surface().replot();
          usleep(200);
        }

        {
          Gnuplot g7("lines");
          // if (! interactive) g7.cmd("set terminal postscript; set output '" + tmp_file.get_filename() + "';");
          std::clog << "window 7: set_samples" << std::endl;
          g7.set_xrange(-30,20).set_samples(40);
          g7.plot_equation("besj0(x)*0.12e1").plot_equation("(x**besj0(x))-2.5");
          g7.set_samples(400).replot();
          usleep(200);
        }

        {
          Gnuplot g8("filledcurves");
          // if (! interactive) g8.cmd("set terminal postscript; set output '" + tmp_file.get_filename() + "';");
          std::clog << "window 8: filledcurves" << std::endl;
          g8.set_legend("outside right top").set_xrange(-5,5);
          g8.plot_equation("x*x").plot_equation("-x*x+4");
          usleep(200);
        }

        {
          //
          // Plot an image
          //
          Gnuplot g9;
          // if (! interactive) g9.cmd("set terminal postscript; set output '" + tmp_file.get_filename() + "';");
          std::clog << "window 9: plot_image" << std::endl;
          const int iWidth  = 255;
          const int iHeight = 255;
          g9.set_xrange(0,iWidth).set_yrange(0,iHeight).set_cbrange(0,255);
          g9.cmd("set palette gray");
          unsigned char ucPicBuf[iWidth*iHeight];
          // generate a greyscale image
          for(int iIndex = 0; iIndex < iHeight*iWidth; iIndex++)
            {
              ucPicBuf[iIndex] = iIndex%255;
            }
          g9.plot_image(ucPicBuf,iWidth,iHeight,"greyscale");
          usleep(200);

          //g9.set_pointsize(0.6).unset_legend().plot_slope(0.8,20);
        }

        {
          //
          // manual control
          //
          Gnuplot g10;
          // if (! interactive) g10.cmd("set terminal postscript; set output '" + tmp_file.get_filename() + "';");
          std::clog << "window 10: manual control" << std::endl;
          g10.cmd("set samples 400").cmd("plot abs(x)/2"); // either with cmd()
          g10 << "replot sqrt(x)" << "replot sqrt(-x)";    // or with <<
        }
        if (interactive) wait_for_key();
        usleep(200);
    }
    catch (GnuplotException ge)
    {
        std::clog << ge.what() << std::endl;
    }

    std::clog << std::endl << "*** end of gnuplot example" << std::endl;

    return 0;
}

void wait_for_key ()
{
#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    std::clog << std::endl << "Press ENTER to continue..." << std::endl;

    std::cin.clear();
    std::cin.ignore(std::cin.rdbuf()->in_avail());
    std::cin.get();
#endif
    return;
}
