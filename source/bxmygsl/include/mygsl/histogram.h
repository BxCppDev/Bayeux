// gsl::histogram.h

#ifndef __gsl__histogram_h
#define __gsl__histogram_h 1

#include <gsl/gsl_histogram.h>
#include <gsl/gsl_vector.h>

#include <iostream>
#include <vector>

namespace mygsl {

  class histogram
    {
    public:
      
      static bool g_debug;

    private:
      double          __underflow;
      double          __overflow;
      gsl_histogram * __h;

    public:

      void init( size_t n_ = 10 , double min_ = 0.0 , double max_ = 1.0 );

      void init( const std::vector<double> & ranges_ );

      histogram( size_t n_ = 10 , 
		 double min_ = 0.0 , double max_ = 1.0 );
      
      histogram( const std::vector<double> & ranges_ );
      
      virtual ~histogram();

      histogram( const histogram & ); // not implemented

      histogram & operator=( const histogram & ); // not implemented

      void fill( double x_ , double weight_ = 1.0 );

      double underflow() const;

      double overflow() const;

      double min() const;

      double max() const;

      size_t bins() const;

      double get( size_t i_ ) const;

      void reset();

      //void rebin( size_t new_bins_ );

      double min_value() const;

      double max_value() const;

      double mean() const;

      double sigma() const;

      double sum() const;

      double shift( double s_ );

      double scale( double s_ );

      void to_stream( std::ostream & ) const;

      void from_stream( std::istream & );

      void print( std::ostream & , int precision_ = 9 ) const;

    };
  
}

#endif // __gsl__histogram_h

// end of gsl::histogram.h
