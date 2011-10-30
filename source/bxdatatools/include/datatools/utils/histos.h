// -*- mode: c++; -*- 
/* histos.cc
 * Author(s):     
 *   Arnaud Chapon <chapon@lpccaen.in2p3.fr>
 *                
 * Creation date: 2009-04-29
 * Last modified: 2011-10-29
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

#include <iostream>
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
      void print2 ( std::ostream & );
      void help  ( std::ostream &, std::string file_ = "" );

      void get_x_values ( std::vector<double> & );
      void get_y_values ( std::vector<double> & );
      void get_z_values ( std::vector<double> & );

      double get_z ( int i_, int j_ );
      void   set_z ( int i_, int j_, double val_ );


    private:

      bool                               _debug_;
      double                             _x_min_, _x_max_;
      int                                _x_step_;
      double                             _y_min_, _y_max_;
      int                                _y_step_;
      std::vector< std::vector<double> > _histo_3d_;

    };

  } // end of namespace utils
} // end of namespace datatools

#endif // __datatools__utils__histos_h

// end of histos.h
