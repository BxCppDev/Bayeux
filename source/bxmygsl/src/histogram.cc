// mygsl::histogram.cc
 
#include <mygsl/histogram.h>

#include <stdexcept>
//#include <gsl/gsl_vector.h>

namespace mygsl {
  
  using namespace std;

  bool histogram::g_debug = false;

  bool histogram::is_uniform_bin_spacing() const
  {
    return _bin_spacing_ > 0;
  }

  double histogram::get_uniform_bin_spacing () const
  {
    if (! is_uniform_bin_spacing ())
      {
        throw std::runtime_error ("histogram::get_uniform_bin_spacing: No uniform bin spacing!");
      }
    return  _bin_spacing_;
  }

  void histogram::init (size_t n_ , double min_  , double max_)
  {
    if (n_ < 1) {
      throw std::runtime_error ("histogram::init: Invalid size!");
    }
    if (min_ >= max_) {
      throw std::runtime_error ("histogram::init: Invalid range!");
    }
    _underflow_ = 0.0;
    _overflow_  = 0.0;
    if (_h_ != 0) {
      gsl_histogram_free (_h_);
      _h_ = 0;
    }
    _bin_spacing_ = (max_ -  min_) / n_;
    _h_ = gsl_histogram_alloc (n_);
    gsl_histogram_set_ranges_uniform (_h_, min_, max_);
    return;
  }

  void histogram::init (const std::vector<double> & ranges_)
  {
    _bin_spacing_ = -1;
    size_t n = ranges_.size () - 1;
    if (n < 1)
      {
        throw std::runtime_error ("histogram::init: Invalid size!");
      }
    _underflow_ = 0.0;
    _overflow_  = 0.0;
    if (_h_ != 0)
      {
        gsl_histogram_free (_h_);
        _h_ = 0;
      }
    _h_ = gsl_histogram_alloc (n);
    const double & a = * (ranges_.begin ());
    gsl_histogram_set_ranges (_h_, &a, ranges_.size ());
    return;
  }

  histogram::histogram (size_t n_ ,
                        double min_ , double max_)
  {
    _underflow_ = 0.0;
    _overflow_  = 0.0;
    _bin_spacing_ = -1;
    _h_ = 0;
    init (n_, min_, max_);
    return;
  }

  histogram::histogram (const std::vector<double> & ranges_)
  {
    _underflow_ = 0.0;
    _overflow_  = 0.0;
    _bin_spacing_ = -1;
    _h_ = 0;
    init (ranges_);
    return;
  }

  histogram::~histogram ()
  {
    if (_h_ != 0)
      {
        gsl_histogram_free (_h_);
        _h_ = 0;
      }
    return;
  }

  histogram::histogram (const histogram & h_ , int mode_)
  {
    _h_           = 0;
    _underflow_   = h_._underflow_;
    _overflow_    = h_._overflow_;
    _bin_spacing_ = h_._bin_spacing_;
    _h_ = gsl_histogram_clone (h_._h_);
    if (mode_ == RESET) {
      reset ();
    }
    if  (mode_ > 0) {
      size_t new_bins =  (size_t) mode_;
      rebin (new_bins);
    }
    return;
  }

  histogram & histogram::operator= (const histogram & h_)
  {
    if (& (*this) == &h_) return *this;

    if  (_h_ != 0) {
      gsl_histogram_free (_h_);
      _h_=0;
    }
    _underflow_ = h_._underflow_;
    _overflow_  = h_._overflow_;
    _h_ = gsl_histogram_clone (h_._h_);
    return *this;
  }

  double histogram::underflow () const
  {
    return _underflow_;
  }

  double histogram::overflow () const
  {
    return _overflow_;
  }

  void histogram::accumulate (double x_ , double weight_)
  {
    if (x_ < _h_->range[0]) {
      return;
    }
    if (x_ >= _h_->range[_h_->n]) {
      return;
    }
    gsl_histogram_accumulate (_h_,x_,weight_);
    return;
  }

  void histogram::increment (double x_)
  {
    if (x_ < _h_->range[0]) {
      return;
    }
    if (x_ >= _h_->range[_h_->n]) {
      return;
    }
    gsl_histogram_increment (_h_,x_);
    return;
  }

  void histogram::fill (double x_ , double weight_)
  {
    if (x_ < _h_->range[0]) {
      _underflow_+=weight_;
      return;
    }
    if (x_ >= _h_->range[_h_->n]) {
      _overflow_+=weight_;
      return;
    }
    gsl_histogram_accumulate (_h_,x_,weight_);
    return;
  }

  double histogram::at (size_t i_) const
  {
    if (i_ < 0 ||  i_ >= _h_->n) {
      throw std::runtime_error("histogram::at: Invalid range!");
    }
    return gsl_histogram_get (_h_,i_);
  }

  double histogram::get (size_t i_) const
  {
    if (i_ < 0) return _underflow_;
    if (i_ >= _h_->n) return _overflow_;
    return gsl_histogram_get (_h_,i_);   
  }

  bool histogram::find (double x_ , size_t & i_) const
  {
    return gsl_histogram_find (_h_,x_,&i_) == GSL_SUCCESS;
    /*
      size_t s;
      int status=gsl_histogram_find (_h_,x_,s);
      if  (status == 0) {
      i_ = s;
      return true;
      }
      else {
      i_ = bins ();
      return (false);
      }
    */
  }

  double histogram::min () const
  {
    return gsl_histogram_min (_h_);
  }

  double histogram::max () const
  {
    return gsl_histogram_max (_h_);
  }

  size_t histogram::bins () const
  {
    return gsl_histogram_bins (_h_);
  }

  void histogram::reset ()
  {
    _bin_spacing_ = -1.0;
    _underflow_   = 0.0;
    _overflow_    = 0.0;
    gsl_histogram_reset (_h_);
    return;
  }

  bool histogram::can_rebin (size_t new_bins_) const
  {
    size_t n = gsl_histogram_bins (_h_);
    if (n < new_bins_)
      {
        return false;
      }
    if ((n % new_bins_) != 0)
      {
        return false;
      }
    return true;
  }

  void histogram::rebin (size_t new_bins_)
  {
    _bin_spacing_ = -1;
    if (! can_rebin (new_bins_))
      {
        throw std::runtime_error ("histogram::rebin: New number of bins is invalid!");
      }
    size_t n  = gsl_histogram_bins (_h_);
    size_t f  = n / new_bins_;
    size_t n2 = new_bins_;
    gsl_histogram * h2 = gsl_histogram_alloc (n2);

    //std::cerr << "DEBUG: histogram::rebin: f  = " << f << std::endl;
    //std::cerr << "DEBUG: histogram::rebin: n2 = " << n2 << std::endl;

    size_t nrange2  = n2 + 1;
    double * range2 = new double[nrange2];

    //gsl_vector_const_view range2 =
    //  gsl_vector_const_view_array_with_stride(
    //    _h_->range,
    //  f,
    //  n2+1);

    for (int i = 0; i < nrange2; i++)
      {
        int i2 = i * f;
        //std::cerr << "DEBUG: histogram::rebin: i2 = " << i2 << std::endl;
        range2[i] = _h_->range[i2];
      }
    gsl_histogram_set_ranges (h2, range2, nrange2);
    for (int i = 0; i < n2; i++)
      {
        double w = 0.0;
        for (int j = 0; j < f; j++)
          {
            w += _h_->bin[f * i + j];
          }
        //std::cerr << "DEBUG: histogram::rebin: w = " << w << std::endl;
        gsl_histogram_accumulate (h2, 0.5 * (range2[i] + range2[i+1]), w);
      }
    delete[] range2;
    gsl_histogram_free (_h_);
    _h_ = 0;
    _h_ = h2;
    return;
  }

  double histogram::min_val () const
  {
    return gsl_histogram_min_val (_h_);
  }

  size_t histogram::min_bin () const
  {
    return gsl_histogram_min_bin (_h_);
  }

  double histogram::max_val () const
  {
    return gsl_histogram_max_val (_h_);
  }

  size_t histogram::max_bin () const
  {
    return gsl_histogram_max_bin (_h_);
  }

  double histogram::mean () const
  {
    return gsl_histogram_mean (_h_);
  }

  double histogram::sigma () const
  {
    return gsl_histogram_sigma (_h_);
  }

  double histogram::sum () const
  {
    return gsl_histogram_sum (_h_);
  }

  double histogram::sum (size_t begin_, size_t end_) const
  {
    double s = 0.0;
    for (size_t i = begin_; i < end_; i++)
      {
        s += gsl_histogram_get (_h_, i);
      }
    return s;
  }

  void histogram::to_stream (std::ostream & out_) const
  {
    //out_ << "gsl::histogram" << ' '
    out_ << _h_->n;
    for (size_t i=0; i<_h_->n; i++) {
      out_  << ' ' << _h_->range[i]
            << ' ' << _h_->range[i+1]
            << ' ' << _h_->bin[i];
    }
    out_ << ' ' << _underflow_ << ' ' << _overflow_;
    return;
  }

  void histogram::from_stream (std::istream & in_)
  {
    /*
      std::string token;
      in_ >> token;
      if (! in_) {
      throw std::runtime_error("gsl::histogram::from_stream: Cannot read generator name from stream!");
      }
      if (token != "gsl::histogram") {
      throw std::runtime_error("gsl::histogram::from_stream: Not a gsl::histogram!");
      }
    */
    size_t n;
    in_ >> n;
    if (! in_) {
      throw std::runtime_error ("gsl::histogram::from_stream: Cannot read histogram size from stream!");
    }
    if (n < 1) {
      throw std::runtime_error ("gsl::histogram::from_stream: Invalid histogram size!");
    }
    init (n);
    for (size_t i=0; i<n; i++) {
      double min,max,counts;
      in_ >> min >> max >> counts;
      if (! in_) {
        throw std::runtime_error ("gsl::histogram::from_stream: Cannot read histogram bin contents from stream!");
      }
      if (i==0) _h_->range[i]=min;
      _h_->range[i+1]=max;
      _h_->bin[i]=counts;
    }
    double underflow, overflow;
    in_ >> underflow >> overflow;
    if (! in_) {
      throw std::runtime_error ("gsl::histogram::from_stream: Cannot read histogram underflow/overflow from stream!");
    }
    _underflow_=underflow;
    _overflow_=overflow;
    return;
  }

  void histogram::print (std::ostream & out_ , int precision_) const
  {
    out_.precision (precision_);
    out_ << "# gsl::histogram" << ' ' << _h_->n << std::endl;
    for (size_t i=0; i<_h_->n; i++) {
      out_  << _h_->range[i]
            << ' ' << _h_->range[i+1]
            << ' ' << _h_->bin[i]
            << std::endl;
    }
    out_ << "# " << _underflow_ << ' ' << _overflow_ << std::endl;
    return;
  }


  void histogram::dump (std::ostream & out_ , int precision_) const
  {
    out_.precision (9);
    out_ << "Histogram:" << std::endl;
    out_ << "  Min   = " << min () << std::endl;
    out_ << "  Max   = " << max () << std::endl;
    out_ << "  #bins = " << bins () << std::endl;
    out_ << "  Mean  = " << mean () << std::endl;
    out_ << "  Sigma = " << sigma () << std::endl;
    out_ << "  Sum   = " << sum () << std::endl;
    return;
  }

  double histogram::operator[] (size_t i_) const
  {
    return at (i_);
  }

  std::pair<double,double> histogram::get_range (size_t i_) const
  {
    std::pair<double,double> p;
    gsl_histogram_get_range (_h_,i_,&p.first,&p.second);
    return p;
  }

  bool histogram::has_size (size_t bins_) const
  {
    return bins_==bins ();
  }

  bool histogram::same (const histogram & h_) const
  {
    return gsl_histogram_equal_bins_p (_h_,h_._h_);
  }

  void histogram::shift (double s_)
  {
    gsl_histogram_shift (_h_,s_);
    return;
  }

  void histogram::scale (double s_)
  {
    gsl_histogram_scale (_h_,s_);
    return;
  }

  void histogram::negate ()
  {
    gsl_histogram_scale (_h_,-1.0);
    return;
  }

  void histogram::zero ()
  {
    reset ();
    return;
  }

  void histogram::add (const histogram & h_)
  {
    gsl_histogram_add (_h_,h_._h_);
    return;
  }

  void histogram::sub (const histogram & h_)
  {
    gsl_histogram_sub (_h_,h_._h_);
    return;
  }

  void histogram::mul (const histogram & h_)
  {
    gsl_histogram_mul (_h_,h_._h_);
    return;
  }

  void histogram::div (const histogram & h_)
  {
    gsl_histogram_div (_h_,h_._h_);
    return;
  }

  histogram & histogram::operator+= (const histogram & h_)
  {
    this->add (h_);
    return *this;
  }

  histogram & histogram::operator-= (const histogram & h_)
  {
    this->sub (h_);
    return *this;
  }

  histogram & histogram::operator*= (const histogram & h_)
  {
    this->mul (h_);
    return *this;
  }

  histogram & histogram::operator/= (const histogram & h_)
  {
    this->div (h_);
    return *this;
  }

  histogram & histogram::operator+= (double a_)
  {
    shift (a_);
    return *this;
  }

  histogram & histogram::operator-= (double a_)
  {
    shift (-a_);
    return *this;
  }

  histogram & histogram::operator- ()
  {
    negate ();
    return *this;
  }

  histogram & histogram::operator*= (double a_)
  {
    scale (a_);
    return *this;
  }

  histogram & histogram::operator/= (double a_)
  {
    scale (1./a_);
    return *this;
  }

  histogram operator+ (const histogram & h1_ ,
                       const histogram & h2_)
  {
    histogram h (h1_);
    h+=h2_;
    return h;
  }

  histogram operator- (const histogram & h1_ ,
                       const histogram & h2_)
  {
    histogram h (h1_);
    h-=h2_;
    return h;
  }

  histogram operator* (const histogram & h1_ ,
                       const histogram & h2_)
  {
    histogram h (h1_);
    h*=h2_;
    return h;
  }

  histogram operator/ (const histogram & h1_ ,
                       const histogram & h2_)
  {
    histogram h (h1_);
    h /= h2_;
    return h;
  }

  histogram operator* (const histogram & h1_ ,
                       double a_)
  {
    histogram h (h1_);
    h.scale (a_);
    return h;
  }

  histogram operator/ (const histogram & h1_ ,
                       double a_)
  {
    histogram h (h1_);
    h.scale (1. / a_);
    return h;
  }

  histogram operator* (double a_ ,
                       const histogram & h1_)
  {
    histogram h (h1_);
    h.scale (a_);
    return h;
  }

  histogram operator/ (double a_ ,
                       const histogram & h1_)
  {
    histogram h (h1_);
    h.scale (1. / a_);
    return h;
  }

  histogram::pdf::pdf (size_t n_)
  {
    _pdf_  =0;
    return;
  }

  histogram::pdf::pdf (const histogram & h_)
  {
    _pdf_ = 0;
    init (h_);
    return;
  }

  histogram::pdf::~pdf ()
  {
    reset ();
    return;
  }

  void histogram::pdf::init (const histogram & h_)
  {
    if  (_pdf_ != 0)
      {
        reset ();
      }
    _pdf_ = gsl_histogram_pdf_alloc (h_.bins ());
    if  (_pdf_ == 0)
      {
        throw std::runtime_error (
                                  "histogram::pdf::init: Cannot allocate histogram's PDF!");
      }
    gsl_histogram_pdf_init (_pdf_, h_._h_);
    /*
      if (_pdf_ != 0 && _pdf_.n != h_.bins) {
      reset();
      _pdf_ = gsl_histogram_pdf_alloc(h_.bins());
      }
      gsl_histogram_pdf_init(_pdf_,h_._h_);
    */
    return;
  }

  void histogram::pdf::reset ()
  {
    gsl_histogram_pdf_free (_pdf_);
    _pdf_ = 0;
    return;
  }

  double histogram::pdf::sample (double r_)
  {
    return gsl_histogram_pdf_sample (_pdf_, r_);
  }

}

// end of mygsl::histogram.cc


