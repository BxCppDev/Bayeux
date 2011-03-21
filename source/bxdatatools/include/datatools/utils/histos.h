// -*- mode: c++; -*- 
/* histos.cc
 * Author(s):     
 *   Arnaud Chapon <chapon@lpccaen.in2p3.fr>
 *                
 * Creation date: 2009-04-29
 * Last modified: 2010-12-02
 * 
 * License: 
 * 
 * Description: 
 * 
 * Construct 3d histograms.
 *
 *
 * History: 
 * 
 */


#ifndef __datatools__utils__histos_h
#define __datatools__utils__histos_h 1

#include <cstdlib>
#include <math.h>
#include <iostream>
#include <string>
#include <exception>
#include <vector>


namespace datatools {
  namespace utils {

    class histos
    {

    public:
      
      // ctor
      histos();
      histos ( bool );

      // dtor
      virtual ~histos();

      // get
      bool   get_debug ();
      double get_x_min ();
      double get_x_max ();
      int    get_x_step();
      double get_y_min ();
      double get_y_max ();
      int    get_y_step();

      double get_z ( int i_, int j_ );
	    
      // set
      void set_debug  ( bool );
      void set_x_min  ( double );
      void set_x_max  ( double );
      void set_x_step ( int );
      void set_y_min  ( double );
      void set_y_max  ( double );
      void set_y_step ( int );

      void set_x_range ( double, double );
      void set_y_range ( double, double );
      void set_x_range ( double, double, int );
      void set_y_range ( double, double, int );

      void init();
      void build ( double, double );

      void x_normalize();
      void y_normalize();
      void do_ratio( histos & histo_3d_2_ );
      
      void print ( std::ostream & );
      void help  ( std::ostream &, std::string file_ = "" );

      void get_x_values ( std::vector<double> & );
      void get_y_values ( std::vector<double> & );
      void get_z_values ( std::vector<double> & );


    private:

      bool                               __debug;
      double                             __x_min, __x_max;
      int                                __x_step;
      double                             __y_min, __y_max;
      int                                __y_step;
      std::vector< std::vector<double> > __histo_3d;

    };

  } // end of namespace utils
} // end of namespace datatools

#endif // __datatools__utils__histos_h

// end of histos.h
