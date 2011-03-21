// -*- mode: c++; -*- 
/* histos.cc
 * 
 */

#include <datatools/utils/histos.h>

namespace datatools {

  namespace utils {

    histos::histos()
    {
      __debug = false;
    }
    
    histos::histos ( bool debug_ )
    {
      set_debug ( debug_ );
    }

    histos::~histos()
    {
    }


    bool histos::get_debug()
    {
      return __debug;
    }
  
    double histos::get_x_min()
    {
      return __x_min;
    }
  
    double histos::get_x_max()
    {
      return __x_max;
    }
  
    int histos::get_x_step()
    {
      return __x_step;
    }
  
    double histos::get_y_min()
    {
      return __y_min;
    }
  
    double histos::get_y_max()
    {
      return __y_max;
    }
  
    int histos::get_y_step()
    {
      return __y_step;
    }
  

    void histos::set_debug ( bool debug_ )
    {
      __debug = debug_;
    }
  
    void histos::set_x_min ( double x_min_ )
    {
      __x_min = x_min_;
    }
  
    void histos::set_x_max ( double x_max_ )
    {
      __x_max = x_max_;
    }
  
    void histos::set_x_step ( int x_step_ )
    {
      __x_step = x_step_;
    }
  
    void histos::set_y_min ( double y_min_ )
    {
      __y_min = y_min_;
    }
  
    void histos::set_y_max ( double y_max_ )
    {
      __y_max = y_max_;
    }
  
    void histos::set_y_step ( int y_step_ )
    {
      __y_step = y_step_;
    }
  
    double histos::get_z ( int i_, int j_ )
    {
      std::vector<double> z_values;
      get_z_values ( z_values );
      return z_values.at ( i_ * get_y_step() + j_ );
    }

    void histos::set_x_range ( double x_min_, double x_max_ )
    {
      set_x_min  ( x_min_ );
      set_x_max  ( x_max_ );
    }

    void histos::set_y_range ( double y_min_, double y_max_ )
    {
      set_y_min  ( y_min_ );
      set_y_max  ( y_max_ );
    }

    void histos::set_x_range ( double x_min_, double x_max_, int x_step_ )
    {
      set_x_min  ( x_min_ );
      set_x_max  ( x_max_ );
      set_x_step ( x_step_ );
    }

    void histos::set_y_range ( double y_min_, double y_max_, int y_step_ )
    {
      set_y_min  ( y_min_ );
      set_y_max  ( y_max_ );
      set_y_step ( y_step_ );
    }


    void histos::init()
    {
      __histo_3d.resize ( __x_step );
    
      for ( size_t i = 0; i < __histo_3d.size(); i++ )
	{
	  __histo_3d[i].resize ( __y_step );

	  for ( size_t j = 0; j < __histo_3d[i].size(); j++ )
	    {
	      __histo_3d[i][j] = 0;
	    }
	}
    }


    void histos::build ( double x_, double y_ )
    {
      double x_step_size = ( __x_max - __x_min ) / (double)( __x_step );
      double y_step_size = ( __y_max - __y_min ) / (double)( __y_step );
    
      for ( size_t i = 0; i < __x_step; i++ )
	{
	  for ( size_t j = 0; j < __y_step; j++ )
	    {
	      if ( x_    >  ( __x_min + (double)(i)   * x_step_size )
		   && x_ <= ( __x_min + (double)(i+1) * x_step_size )
		   && y_ >  ( __y_min + (double)(j)   * y_step_size )
		   && y_ <= ( __y_min + (double)(j+1) * y_step_size ) )
		{
		  __histo_3d[i][j]++;
		}
	    }
	}
    }

  
    void histos::do_ratio( histos & histo_3d_2_ )
    {
      if ( __x_min  == histo_3d_2_.get_x_min()  &&
	   __x_max  == histo_3d_2_.get_x_max()  &&
	   __x_step == histo_3d_2_.get_x_step() &&
	   __y_min  == histo_3d_2_.get_y_min()  &&
	   __y_max  == histo_3d_2_.get_y_max()  &&
	   __y_step == histo_3d_2_.get_y_step() )
	{
	  for ( size_t i = 0; i < __x_step; i++ )
	    for ( size_t j = 0; j < __y_step; j++ )
	      if ( histo_3d_2_.get_z( i, j ) != 0. )
		__histo_3d[i][j] /= histo_3d_2_.get_z( i, j );
	      else
		__histo_3d[i][j] = -1.;
	}
    }
    

    void histos::x_normalize()
    {
      for ( size_t i = 0; i < __x_step; i++ )
	{
	  double i_sum = 0;
	
	  for ( size_t j = 0; j < __y_step; j++ )
	    i_sum += __histo_3d[i][j];
	
	  for ( size_t j = 0; j < __y_step; j++ )
	    {
	      if ( __histo_3d[i][j] != 0 )
		__histo_3d[i][j] /= i_sum;
	    }
	}
    }
  
    void histos::y_normalize()
    {
      for ( size_t j = 0; j < __y_step; j++ )
	{
	  double j_sum = 0;
	
	  for ( size_t i = 0; i < __x_step; i++ )
	    j_sum += __histo_3d[i][j];
	
	  for ( size_t i = 0; i < __x_step; i++ )
	    {
	      if ( __histo_3d[i][j] != 0 )
		__histo_3d[i][j] /= j_sum;
	    }
	}
    }
  

    void histos::get_x_values ( std::vector<double> & values_ )
    {
      std::vector<double> x_values;
      double x_step_size = ( __x_max - __x_min ) / (double)( __x_step );
      double y_step_size = ( __y_max - __y_min ) / (double)( __y_step );
      for ( int i = 0; i < __x_step; i++ )
        for ( int j = 0; j < __y_step; j++ )
          x_values.push_back ( __x_min + (double)(i) * x_step_size );
      values_ = x_values;
    }
    
    void histos::get_y_values ( std::vector<double> & values_ )
    {
      std::vector<double> y_values;
      double x_step_size = ( __x_max - __x_min ) / (double)( __x_step );
      double y_step_size = ( __y_max - __y_min ) / (double)( __y_step );
      for ( int i = 0; i < __x_step; i++ )
        for ( int j = 0; j < __y_step; j++ )
          y_values.push_back ( __y_min + (double)(j) * y_step_size );
      values_ = y_values;
    }  
    
    void histos::get_z_values ( std::vector<double> & values_ )
    {
      std::vector<double> z_values;
      for ( int i = 0; i < __x_step; i++ )
        for ( int j = 0; j < __y_step; j++ )
          z_values.push_back ( __histo_3d[i][j] );
      values_ = z_values;
    }
    
    
    void histos::print ( std::ostream & out_ )
    {
      double x_step_size = ( __x_max - __x_min ) / (double)( __x_step );
      double y_step_size = ( __y_max - __y_min ) / (double)( __y_step );
    
      out_ << "# Result"
	   << std::endl;

      if ( __debug ) std::cerr << std::endl
			       << "histos::print: DEBUG: Result : "
			       << std::endl;
    
      for ( int i = 0; i < __x_step; i++ )
	{
	  for ( int j = 0; j < __y_step; j++ )
	    {
	      out_ << ( __x_min + (double)(i) * x_step_size ) << " "
		   << ( __y_min + (double)(j) * y_step_size ) << " "
		   << __histo_3d[i][j]
		   << std::endl;
	    
	      if ( __debug ) std::cerr << __histo_3d[i][j] << '\t';
	    }
	  if ( __debug ) std::cerr << std::endl;
	}
    }


    void histos::help ( std::ostream & out_, std::string file_ )
    {
      std::string out_txt = "histo_3d.his";
      if ( file_ != "" )
	out_txt = file_;
      
      out_ << std::endl
	   << "** histos: gnuplot example: **" << std::endl
	   << "------------------------------" << std::endl;
    
      out_ << "set autoscale"  << std::endl
	   << "set xlabel 'x'" << std::endl
	   << "set ylabel 'y'" << std::endl
	   << "set zlabel 'N'" << std::endl
	   << "set pm3d"       << std::endl
	   << "set palette rgbformulae -31, -36, -32" << std::endl
	   << "set view 0, 0"  << std::endl
	   << "unset surface"  << std::endl
	   << "set dgrid3d " << __y_step << "," << __x_step << ",2" << std::endl
	   << "splot [" << __x_min << ":" << __x_max << "]"
	   << "[" << __y_min << ":" << __y_max << "][0:*] '" << out_txt
	   << "' u 1:2:3 with dots title 'histogram_3d'"
	   << std::endl << std::endl;
    }
  }
  
} // end of histos.cc
