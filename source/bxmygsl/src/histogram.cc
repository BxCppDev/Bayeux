// mygsl::histogram.cc

#include <mygsl/histogram.h>

#include <stdexcept>

namespace mygsl {

  bool histogram::g_debug = false;

  void histogram::init( size_t n_ , double min_  , double max_ )
  {
    if ( n_ < 1 ) {
      throw std::runtime_error("histogram::init: Invalid size!");
    }
    if ( min_ >= max_ ) {
      throw std::runtime_error("histogram::init: Invalid range!");
    }
    __underflow=0.0;
    __overflow=0.0;
    if ( __h != 0 ) {
      gsl_histogram_free(__h);
      __h=0;
    }
    __h = gsl_histogram_alloc(n_);
    gsl_histogram_set_ranges_uniform(__h,min_,max_);
  }

  void histogram::init( const std::vector<double> & ranges_ )
  {
    size_t n = ranges_.size()-1;
    if ( n < 1 ) {
      throw std::runtime_error("histogram::init: Invalid size!");
    }
    __underflow=0.0;
    __overflow=0.0;
    if ( __h != 0 ) {
      gsl_histogram_free(__h);
      __h=0;
    }
    __h = gsl_histogram_alloc(n);
    const double & a = *(ranges_.begin());
    gsl_histogram_set_ranges(__h,&a,ranges_.size());
  }

  histogram::histogram( size_t n_ , 
			double min_ , double max_ )
  {
    __underflow=0.0;
    __overflow=0.0;
    __h=0;
    init(n_,min_,max_);
  }

  histogram::histogram( const std::vector<double> & ranges_ )
  {
    __underflow=0.0;
    __overflow=0.0;
    __h=0;
    init(ranges_);
  }

  histogram::~histogram()
  {
    if ( __h != 0 ) {
      gsl_histogram_free(__h);
    }
  }

  histogram::histogram( const histogram & h_ )
  {
    __h         = 0;
    __underflow = h_.__underflow;
    __overflow  = h_.__overflow;
    __h = gsl_histogram_clone(h_.__h);
  }

  histogram & histogram::operator=( const histogram & h_ ) 
  {
    if ( &(*this) == &h_ ) return *this;

    if ( __h != 0 ) {
      gsl_histogram_free(__h);
      __h=0;
    }
    __underflow = h_.__underflow;
    __overflow  = h_.__overflow;
    __h = gsl_histogram_clone(h_.__h);
    return *this;
  }

  double histogram::underflow() const
  {
    return __underflow;
  }
  
  double histogram::overflow() const
  {
    return __overflow;
  }

  void histogram::fill( double x_ , double weight_ )
  {
    if ( x_ < __h->range[0] ) {
      __underflow+=weight_;
      return;
    }
    if ( x_ >= __h->range[__h->n] ) {
      __overflow+=weight_;
      return;
    }
    gsl_histogram_accumulate(__h,x_,weight_);
  }

  double histogram::get( size_t i_ ) const
  {
    if ( i_ < 0 ) return __underflow;
    if ( i_ >= __h->n ) return __overflow;
    return gsl_histogram_get(__h,i_);
    
  }

  double histogram::min() const
  {
    return gsl_histogram_min(__h);
  }
  
  double histogram::max() const
  {
    return gsl_histogram_max(__h);
  }
  
  size_t histogram::bins() const
  {
    return gsl_histogram_bins(__h);
  }

  void histogram::reset()
  {
    __underflow=0.0;
    __overflow=0.0;
    return gsl_histogram_reset(__h);
  }

  /*
  void histogram::rebin( size_t new_bins_ )
  {
    size_t n=gsl_histogram_bins(__h);
    if ( n<new_bins_ ) {
      throw std::runtime_error("histogram::rebin: New number of bins is too large!");
    }
    if ( (n%new_bins_) != 0 ) {
      throw std::runtime_error("histogram::rebin: Invalid new number of bins!");
    }
    size_t f  = n/new_bins_;
    size_t n2 = new_bins_;
    gsl_histogram * h2 = gsl_histogram_alloc(n2);

    double * range2=new double[n2+1];

    //gsl_vector_const_view range2 = 
    //  gsl_vector_const_view_array_with_stride(
    //    __h->range,
    //	f,
    //	n2+1);

    for ( int i=0; i< n2+1; i++ ) {
      range2[i]=__h->range[i*f];
    }
    gsl_histogram_set_ranges(h2,range2,n2+1);
    delete[] range2;
    for ( int i=0; i<n2; i++ ) {
      double w=0.0;
      for ( int j=0; j<f; j++ ) {
	w += __h->bin[f*i+j];
      }
      gsl_histogram_accumulate(h2,__h->bin[i],w);
    }
    gsl_histogram_free(__h);
    __h=0;
    __h=h2;
  }
  */

  double histogram::min_value() const
  {
    return gsl_histogram_min_val(__h);
  }

  double histogram::max_value() const
  {
    return gsl_histogram_max_val(__h);
  }

  double histogram::mean() const
  {
    return gsl_histogram_mean(__h);
  }

  double histogram::sigma() const
  {
    return gsl_histogram_sigma(__h);
  }

  double histogram::sum() const
  {
    return gsl_histogram_sum(__h);
  }

  double histogram::shift( double s_ )
  {
    gsl_histogram_shift(__h,s_);
  }

  double histogram::scale( double s_ )
  {
    gsl_histogram_scale(__h,s_);
  }

  void histogram::to_stream( std::ostream & out_ ) const
  {
    //out_ << "gsl::histogram" << ' ' 
    out_ << __h->n;
    for ( size_t i=0; i<__h->n; i++ ) {
      out_  << ' ' << __h->range[i] 
	    << ' ' << __h->range[i+1] 
	    << ' ' << __h->bin[i]; 
    }
    out_ << ' ' << __underflow << ' ' << __overflow;
  }

  void histogram::from_stream( std::istream & in_ )
  {
    /*
    std::string token;
    in_ >> token;
    if ( ! in_ ) {
      throw std::runtime_error("gsl::histogram::from_stream: Cannot read generator name from stream!");      
    }
    if ( token != "gsl::histogram" ) {
      throw std::runtime_error("gsl::histogram::from_stream: Not a gsl::histogram!");      
    }
    */
    size_t n;
    in_ >> n;
    if ( ! in_ ) {
      throw std::runtime_error("gsl::histogram::from_stream: Cannot read histogram size from stream!");      
    }
    if ( n < 1 ) {
      throw std::runtime_error("gsl::histogram::from_stream: Invalid histogram size!");      
    }
    init(n);
    for ( size_t i=0; i<n; i++ ) {
      double min,max,counts;
      in_ >> min >> max >> counts;
      if ( ! in_ ) {
	throw std::runtime_error("gsl::histogram::from_stream: Cannot read histogram bin contents from stream!");      
      }
      if ( i==0 ) __h->range[i]=min;
      __h->range[i+1]=max;
      __h->bin[i]=counts;
    } 
    double underflow, overflow;
    in_ >> underflow >> overflow;
    if ( ! in_ ) {
      throw std::runtime_error("gsl::histogram::from_stream: Cannot read histogram underflow/overflow from stream!");      
    }
    __underflow=underflow;
    __overflow=overflow;
  }

  void histogram::print( std::ostream & out_ , int precision_ ) const
  {
    out_.precision(precision_);
    out_ << "# gsl::histogram" << ' ' << __h->n << std::endl;
    for ( size_t i=0; i<__h->n; i++ ) {
      out_  << __h->range[i] 
	    << ' ' << __h->range[i+1] 
	    << ' ' << __h->bin[i]
	    << std::endl; 
    }
    out_ << "# " << __underflow << ' ' << __overflow << std::endl;
  }

}

// end of mygsl::histogram.cc


