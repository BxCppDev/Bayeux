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

  histogram::histogram( const histogram & h_ , int mode_ )
  {
    __h         = 0;
    __underflow = h_.__underflow;
    __overflow  = h_.__overflow;
    __h = gsl_histogram_clone(h_.__h);
    if ( mode_ == RESET ) {
      reset();
    }
    if ( mode_ > 0 ) {
      size_t new_bins = (size_t) mode_;
      rebin(new_bins);
    }
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

  void histogram::accumulate( double x_ , double weight_ )
  {
    if ( x_ < __h->range[0] ) {
      return;
    }
    if ( x_ >= __h->range[__h->n] ) {
      return;
    }
    gsl_histogram_accumulate(__h,x_,weight_);
  }
  
  void histogram::increment( double x_ )
  {
    if ( x_ < __h->range[0] ) {
      return;
    }
    if ( x_ >= __h->range[__h->n] ) {
      return;
    }
    gsl_histogram_increment(__h,x_);
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

  double histogram::at( size_t i_ ) const
  {
    if ( i_ < 0 ||  i_ >= __h->n) {
      throw std::runtime_error("histogram::at: Invalid range!");
    }
    return gsl_histogram_get(__h,i_);   
  }

  double histogram::get( size_t i_ ) const
  {
    if ( i_ < 0 ) return __underflow;
    if ( i_ >= __h->n ) return __overflow;
    return gsl_histogram_get(__h,i_);
    
  }

  bool histogram::find( double x_ , size_t & i_ ) const
  {
    return gsl_histogram_find(__h,x_,&i_) == GSL_SUCCESS;
    /*
    size_t s;
    int status=gsl_histogram_find(__h,x_,s);
    if ( status == 0 ) {
      i_ = s;
      return true;
    }
    else {
      i_ = bins();
      return (false);
    }
    */
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

  bool histogram::can_rebin( size_t new_bins_ ) const
  {
    size_t n=gsl_histogram_bins(__h);
    if ( n<new_bins_ ) {
      return false;
    }
    if ( (n%new_bins_) != 0 ) {
      return false;
    }
    return true;
  }

  void histogram::rebin( size_t new_bins_ )
  {
    if ( ! can_rebin( new_bins_ ) ) {
      throw std::runtime_error("histogram::rebin: New number of bins is invalid!");
    }
    size_t n=gsl_histogram_bins(__h);
    size_t f  = n/new_bins_;
    size_t n2 = new_bins_;
    gsl_histogram * h2 = gsl_histogram_alloc(n2);

    //std::cerr << "DEBUG: histogram::rebin: f  = " << f << std::endl;
    //std::cerr << "DEBUG: histogram::rebin: n2 = " << n2 << std::endl;

    size_t nrange2  = n2+1;
    double * range2 = new double[nrange2];

    //gsl_vector_const_view range2 = 
    //  gsl_vector_const_view_array_with_stride(
    //    __h->range,
    //	f,
    //	n2+1);

    for ( int i=0; i<nrange2; i++ ) {
      int i2=i*f;
      //std::cerr << "DEBUG: histogram::rebin: i2 = " << i2 << std::endl;
      range2[i]=__h->range[i2];
    }
    gsl_histogram_set_ranges(h2,range2,nrange2);
    for ( int i=0; i<n2; i++ ) {
      double w=0.0;
      for ( int j=0; j<f; j++ ) {
	w += __h->bin[f*i+j];
      }
      //std::cerr << "DEBUG: histogram::rebin: w = " << w << std::endl;
      gsl_histogram_accumulate(h2,0.5*(range2[i]+range2[i+1]),w);
    }
    delete[] range2;
    gsl_histogram_free(__h);
    __h=0;
    __h=h2;
  }

  double histogram::min_val() const
  {
    return gsl_histogram_min_val(__h);
  }

  size_t histogram::min_bin() const
  {
    return gsl_histogram_min_bin(__h);
  }

  double histogram::max_val() const
  {
    return gsl_histogram_max_val(__h);
  }

  size_t histogram::max_bin() const
  {
    return gsl_histogram_max_bin(__h);
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


  void histogram::dump( std::ostream & out_ , int precision_ ) const
  {
    out_.precision(9);
    out_ << "Histogram:" << std::endl;
    out_ << "  Min   = " << min() << std::endl;
    out_ << "  Max   = " << max() << std::endl;
    out_ << "  #bins = " << bins() << std::endl;
    out_ << "  Mean  = " << mean() << std::endl;
    out_ << "  Sigma = " << sigma() << std::endl;
    out_ << "  Sum   = " << sum() << std::endl;
  }
  
  double histogram::operator[]( size_t i_ ) const
  {
    return at(i_);
  }

  std::pair<double,double> histogram::get_range( size_t i_ ) const
  {
    std::pair<double,double> p;
    gsl_histogram_get_range(__h,i_,&p.first,&p.second);
    return p;
  }

  bool histogram::has_size( size_t bins_ ) const
  {
    return bins_==bins();
  }

  bool histogram::same( const histogram & h_ ) const
  {
    return gsl_histogram_equal_bins_p(__h,h_.__h);
  }

  void histogram::shift( double s_ )
  {
    gsl_histogram_shift(__h,s_);
  }

  void histogram::scale( double s_ )
  {
    gsl_histogram_scale(__h,s_);
  }

  void histogram::negate()
  {
    gsl_histogram_scale(__h,-1.0);
  }
 
  void histogram::zero()
  {
    reset();
  }
 
  void histogram::add( const histogram & h_ )
  {
    gsl_histogram_add(__h,h_.__h);
  }
  
  void histogram::sub( const histogram & h_ )
  {
    gsl_histogram_sub(__h,h_.__h);
  }
  
  void histogram::mul( const histogram & h_ )
  {
    gsl_histogram_mul(__h,h_.__h);
  }
  
  void histogram::div( const histogram & h_ )
  {
    gsl_histogram_div(__h,h_.__h);
  }
  
  histogram & histogram::operator+=( const histogram & h_ )
  {
    this->add(h_);
    return *this;
  }
  
  histogram & histogram::operator-=( const histogram & h_ )
  {
    this->sub(h_);
    return *this;
  }
  
  histogram & histogram::operator*=( const histogram & h_ )
  {
    this->mul(h_);
    return *this;
  }
  
  histogram & histogram::operator/=( const histogram & h_ )
  {
    this->div(h_);
    return *this;
  }
  
  histogram & histogram::operator+=( double a_ )
  {
    shift(a_);
    return *this;
  }
  
  histogram & histogram::operator-=( double a_ )
  {
    shift(-a_);
    return *this;
  }
  
  histogram & histogram::operator-()
  {
    negate();
    return *this;
  }
  
  histogram & histogram::operator*=( double a_ )
  {
    scale(a_);
    return *this;
  }
  
  histogram & histogram::operator/=( double a_ )
  {
    scale(1./a_);
    return *this;
  }

  histogram operator+( const histogram & h1_ , 
		       const histogram & h2_ )
  {
    histogram h(h1_);
    h+=h2_;
    return h;
  }

  histogram operator-( const histogram & h1_ , 
		       const histogram & h2_ )
  {
    histogram h(h1_);
    h-=h2_;
    return h;
  }
  
  histogram operator*( const histogram & h1_ , 
		       const histogram & h2_ )
  {
    histogram h(h1_);
    h*=h2_;
    return h;
  }

  histogram operator/( const histogram & h1_ , 
		       const histogram & h2_ )
  {
    histogram h(h1_);
    h/=h2_;
    return h;
  }

  histogram operator*( const histogram & h1_ , 
		       double a_ )
  {
    histogram h(h1_);
    h.scale(a_);
    return h;
  }

  histogram operator/( const histogram & h1_ , 
		       double a_ )
  {
    histogram h(h1_);
    h.scale(1./a_);
    return h;
  }

  histogram operator*( double a_ , 
		       const histogram & h1_ )
  {
    histogram h(h1_);
    h.scale(a_);
    return h;
  }

  histogram operator/( double a_ , 
		       const histogram & h1_ )
  {
    histogram h(h1_);
    h.scale(1./a_);
    return h;
  }

  histogram::pdf::pdf( size_t n_ )
  {
    __pdf=0;
  }
  
  histogram::pdf::pdf( const histogram & h_ )
  {
    __pdf=0;
    init(h_);
  }
  
  histogram::pdf::~pdf()
  {
    reset();
  }
  
  void histogram::pdf::init( const histogram & h_ )
  {
    if ( __pdf != 0 ) {
      reset(); 
    }
    __pdf = gsl_histogram_pdf_alloc(h_.bins());
    if ( __pdf == 0 ) {
      throw std::runtime_error(
        "histogram::pdf::init: Cannot allocate histogram's PDF!");
    }
    gsl_histogram_pdf_init(__pdf,h_.__h);
    /*
    if ( __pdf != 0 && __pdf.n != h_.bins) {
      reset(); 
      __pdf = gsl_histogram_pdf_alloc(h_.bins());
    }
    gsl_histogram_pdf_init(__pdf,h_.__h);
    */
  }
  
  void histogram::pdf::reset()
  {
    gsl_histogram_pdf_free(__pdf);
    __pdf=0;
  }
  
  double histogram::pdf::sample( double r_ )
  {
    return gsl_histogram_pdf_sample(__pdf,r_);
  }

}

// end of mygsl::histogram.cc


