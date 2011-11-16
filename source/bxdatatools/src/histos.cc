// -*- mode: c++; -*- 
/* histos.cc
 *  
 */

#include <datatools/utils/histos.h>

#include <cstdlib>
#include <cmath>
#include <string>
#include <exception>

namespace datatools {

  namespace utils {

    histos::histos()
    {
      _debug_ = false;
      return;
    }
    
    histos::histos ( bool debug_ )
    {
      set_debug ( debug_ );
      return;
    }

    histos::~histos()
    {
      return;
    }


    bool histos::get_debug()
    {
      return _debug_;
    }
  
    double histos::get_x_min()
    {
      return _x_min_;
    }
  
    double histos::get_x_max()
    {
      return _x_max_;
    }
  
    int histos::get_x_step()
    {
      return _x_step_;
    }
  
    double histos::get_y_min()
    {
      return _y_min_;
    }
  
    double histos::get_y_max()
    {
      return _y_max_;
    }
  
    int histos::get_y_step()
    {
      return _y_step_;
    }
  

    void histos::set_debug ( bool debug_ )
    {
      _debug_ = debug_;
      return;
    }
  
    void histos::set_x_min ( double x_min_ )
    {
      _x_min_ = x_min_;
			return;
		}
  
    void histos::set_x_max ( double x_max_ )
    {
      _x_max_ = x_max_;
      return;
    }
  
    void histos::set_x_step ( int x_step_ )
    {
      _x_step_ = x_step_;
      return;
    }
  
    void histos::set_y_min ( double y_min_ )
    {
      _y_min_ = y_min_;
      return;
    }
  
    void histos::set_y_max ( double y_max_ )
    {
      _y_max_ = y_max_;
      return;
    }
  
    void histos::set_y_step ( int y_step_ )
    {
      _y_step_ = y_step_;
      return;
    }
  
    double histos::get_z ( int i_, int j_ )
    {
      std::vector<double> z_values;
      get_z_values ( z_values );
      return z_values.at ( i_ * get_y_step() + j_ );
    }

    void histos::set_z ( int i_, int j_, double val_ )
    {
      _histo_3d_[i_][j_] = val_;
      return;
    }

    void histos::set_x_range ( double x_min_, double x_max_ )
    {
      set_x_min  ( x_min_ );
      set_x_max  ( x_max_ );
			return;
		}

    void histos::set_y_range ( double y_min_, double y_max_ )
    {
      set_y_min  ( y_min_ );
      set_y_max  ( y_max_ );
      return;
    }

    void histos::set_x_range ( double x_min_, double x_max_, int x_step_ )
    {
      set_x_min  ( x_min_ );
      set_x_max  ( x_max_ );
      set_x_step ( x_step_ );
      return;
    }

    void histos::set_y_range ( double y_min_, double y_max_, int y_step_ )
    {
      set_y_min  ( y_min_ );
      set_y_max  ( y_max_ );
      set_y_step ( y_step_ );
      return;
    }


    void histos::init()
    {
      _histo_3d_.resize ( _x_step_ );
    
      for ( size_t i = 0; i < _histo_3d_.size(); i++ )
				{
					_histo_3d_[i].resize ( _y_step_ );

					for ( size_t j = 0; j < _histo_3d_[i].size(); j++ )
						{
							_histo_3d_[i][j] = 0;
						}
				}
			return;
		}


    void histos::build ( double x_, double y_, double weight_ )
    {
      double x_step_size = ( _x_max_ - _x_min_ ) / (double)( _x_step_ );
      double y_step_size = ( _y_max_ - _y_min_ ) / (double)( _y_step_ );
    
      for ( int i = 0; i < _x_step_; i++ )
				{
					for ( int j = 0; j < _y_step_; j++ )
						{
							if ( x_    >  ( _x_min_ + (double)(i)   * x_step_size )
									 && x_ <= ( _x_min_ + (double)(i+1) * x_step_size )
									 && y_ >  ( _y_min_ + (double)(j)   * y_step_size )
									 && y_ <= ( _y_min_ + (double)(j+1) * y_step_size ) )
								{
									_histo_3d_[i][j] += weight_;
								}
						}
				}
			return;
		}

  
    void histos::do_ratio( histos & histo_3d_2_, double n_, double m_ )
    {
      if ( _x_min_  == histo_3d_2_.get_x_min()  &&
					 _x_max_  == histo_3d_2_.get_x_max()  &&
					 _x_step_ == histo_3d_2_.get_x_step() &&
					 _y_min_  == histo_3d_2_.get_y_min()  &&
					 _y_max_  == histo_3d_2_.get_y_max()  &&
					 _y_step_ == histo_3d_2_.get_y_step() )
				{
					for ( int i = 0; i < _x_step_; i++ )
						for ( int j = 0; j < _y_step_; j++ )
							if ( histo_3d_2_.get_z( i, j ) != 0. )
								_histo_3d_[i][j] /= histo_3d_2_.get_z( i, j ) * m_ / n_ ;
							else
								_histo_3d_[i][j] = -1.;
				}
			return;
		}
    

    void histos::x_normalize()
    {
      for ( int i = 0; i < _x_step_; i++ )
				{
					double i_sum = 0;
	
					for ( int j = 0; j < _y_step_; j++ )
						i_sum += _histo_3d_[i][j];
	
					for ( int j = 0; j < _y_step_; j++ )
						{
							if ( _histo_3d_[i][j] != 0 )
								_histo_3d_[i][j] /= i_sum;
						}
				}
			return;
		}
  
		void histos::do_ratio_sqrt ( histos & histo_3d_2_, double n_, double m_ )
		{
			if ( _x_min_  == histo_3d_2_.get_x_min()  &&
					 _x_max_  == histo_3d_2_.get_x_max()  &&
					 _x_step_ == histo_3d_2_.get_x_step() &&
					 _y_min_  == histo_3d_2_.get_y_min()  &&
					 _y_max_  == histo_3d_2_.get_y_max()  &&
					 _y_step_ == histo_3d_2_.get_y_step() )
				{
					for ( size_t i = 0; i < _x_step_; i++ )
						for ( size_t j = 0; j < _y_step_; j++ )
							if ( histo_3d_2_.get_z( i, j ) != 0. )
								_histo_3d_[i][j] /= ( sqrt ( histo_3d_2_.get_z( i, j ) * m_ ) / n_ );
							else
								_histo_3d_[i][j] = -1.;
				}
		}
 
		void histos::compute_significance ( histos & histo_3d_2_, double n_, double m_ )
		{
			if ( _x_min_  == histo_3d_2_.get_x_min()  &&
					 _x_max_  == histo_3d_2_.get_x_max()  &&
					 _x_step_ == histo_3d_2_.get_x_step() &&
					 _y_min_  == histo_3d_2_.get_y_min()  &&
					 _y_max_  == histo_3d_2_.get_y_max()  &&
					 _y_step_ == histo_3d_2_.get_y_step() )
				{
					for ( size_t i = 0; i < _x_step_; i++ )
						for ( size_t j = 0; j < _y_step_; j++ )
							if ( histo_3d_2_.get_z( i, j ) != 0. )
								{
									double current_val = _histo_3d_[i][j];
									_histo_3d_[i][j] /= ( sqrt ( histo_3d_2_.get_z( i, j ) * m_
																							 + current_val * n_ )
																				/ n_ );
								}
							else
								_histo_3d_[i][j] = -1.;
				}
		}
 

    void histos::y_normalize()
    {
      for ( int j = 0; j < _y_step_; j++ )
				{
					double j_sum = 0;
	
					for ( int i = 0; i < _x_step_; i++ )
						j_sum += _histo_3d_[i][j];
	
					for ( int i = 0; i < _x_step_; i++ )
						{
							if ( _histo_3d_[i][j] != 0 )
								_histo_3d_[i][j] /= j_sum;
						}
				}
			return;
		}
  

    void histos::get_x_values ( std::vector<double> & values_ )
    {
      std::vector<double> x_values;
      double x_step_size = ( _x_max_ - _x_min_ ) / (double)( _x_step_ );
      //double y_step_size = ( _y_max_ - _y_min_ ) / (double)( _y_step_ );
      for ( int i = 0; i < _x_step_; i++ )
        for ( int j = 0; j < _y_step_; j++ )
          x_values.push_back ( _x_min_ + (double)(i) * x_step_size );
      values_ = x_values;
      return;
    }
    
    void histos::get_y_values ( std::vector<double> & values_ )
    {
      std::vector<double> y_values;
      //double x_step_size = ( _x_max_ - _x_min_ ) / (double)( _x_step_ );
      double y_step_size = ( _y_max_ - _y_min_ ) / (double)( _y_step_ );
      for ( int i = 0; i < _x_step_; i++ )
        for ( int j = 0; j < _y_step_; j++ )
          y_values.push_back ( _y_min_ + (double)(j) * y_step_size );
      values_ = y_values;
			return;
		}  
    
    void histos::get_z_values ( std::vector<double> & values_ )
    {
      std::vector<double> z_values;
      for ( int i = 0; i < _x_step_; i++ )
        for ( int j = 0; j < _y_step_; j++ )
          z_values.push_back ( _histo_3d_[i][j] );
      values_ = z_values;
			return;
		}
    
    
    void histos::print ( std::ostream & out_ )
    {
      double x_step_size = ( _x_max_ - _x_min_ ) / (double)( _x_step_ );
      double y_step_size = ( _y_max_ - _y_min_ ) / (double)( _y_step_ );
    
      out_ << "# Result"
					 << std::endl;

      if ( _debug_ ) std::cerr << std::endl
															 << "histos::print: DEBUG: Result : "
															 << std::endl;
    
      for ( int i = 0; i < _x_step_; i++ )
				{
					for ( int j = 0; j < _y_step_; j++ )
						{
							out_ << ( _x_min_ + (double)(i) * x_step_size ) << " "
									 << ( _y_min_ + (double)(j) * y_step_size ) << " "
									 << _histo_3d_[i][j]
									 << std::endl;
	    
							if ( _debug_ ) std::cerr << _histo_3d_[i][j] << '\t';
						}
					if ( _debug_ ) std::cerr << std::endl;
				}
			return;
		}
    
    void histos::print2 ( std::ostream & out_ )
    {
      double x_step_size = ( _x_max_ - _x_min_ ) / (double)( _x_step_ );
      double y_step_size = ( _y_max_ - _y_min_ ) / (double)( _y_step_ );
    
      out_ << "# Result"
					 << std::endl;

      for ( int i = 0; i < _x_step_; i++ )
				{
					for ( int j = 0; j < _y_step_; j++ )
						{
							out_ << ( _x_min_ + (double)(i) * x_step_size ) << " "
									 << ( _y_min_ + (double)(j) * y_step_size ) << " "
									 << _histo_3d_[i][j]
									 << std::endl
									 << ( _x_min_ + (double)(i) * x_step_size ) << " "
									 << ( _y_min_ + ((double)(j)+1.) * y_step_size ) << " "
									 << _histo_3d_[i][j]
									 << std::endl;
						}
					out_ << std::endl;
					for ( int j = 0; j < _y_step_; j++ )
						{
							out_ << ( _x_min_ + ((double)(i)+1.) * x_step_size ) << " "
									 << ( _y_min_ + (double)(j) * y_step_size ) << " "
									 << _histo_3d_[i][j]
									 << std::endl
									 << ( _x_min_ + ((double)(i)+1.) * x_step_size ) << " "
									 << ( _y_min_ + ((double)(j)+1.) * y_step_size ) << " "
									 << _histo_3d_[i][j]
									 << std::endl;
						}
					out_ << std::endl;
				}
			return;
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
					 << "set dgrid3d " << _y_step_ << "," << _x_step_ << ",2" << std::endl
					 << "splot [" << _x_min_ << ":" << _x_max_ << "]"
					 << "[" << _y_min_ << ":" << _y_max_ << "][0:*] '" << out_txt
					 << "' u 1:2:3 with dots title 'histogram 3d'"
					 << std::endl << std::endl;
			return;
		}

    
    void histos::help2 ( std::ostream & out_, std::string file_ )
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
					 << "set hidden3d"       << std::endl
					 << "splot [" << _x_min_ << ":" << _x_max_ << "]"
					 << "[" << _y_min_ << ":" << _y_max_ << "][0:*] '" << out_txt
					 << "' u 1:2:3 with lines title 'histogram 3d'"
					 << std::endl << std::endl;
    }
  }
  
} // end of histos.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
