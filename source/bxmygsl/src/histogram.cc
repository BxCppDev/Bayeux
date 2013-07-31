// mygsl::histogram.cc

#include <mygsl/histogram.h>

#include <stdexcept>
#include <sstream>
#include <limits>
#include <cmath>

#include <gsl/gsl_math.h>
#include <boost/preprocessor/stringize.hpp>

#include <datatools/exception.h>

#include <mygsl/histogram_2d.h>

namespace mygsl {

  using namespace std;

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(histogram,"mygsl::histogram")

  const datatools::properties & histogram::get_auxiliaries () const
  {
    return _auxiliaries_;
  }

  datatools::properties & histogram::grab_auxiliaries ()
  {
    return _auxiliaries_;
  }

  double histogram::get_binning_info () const
  {
    return _binning_info_;
  }

  bool histogram::is_uniform_binning () const
  {
    return _binning_info_ > 0;
  }

  bool histogram::is_logarithmic_binning () const
  {
    return _binning_info_ < 0;
  }

  bool histogram::is_any_binning () const
  {
    return _binning_info_ == 0;
  }

  double histogram::get_uniform_binning () const
  {
    DT_THROW_IF (! is_uniform_binning (), std::logic_error, "No uniform binning!");
    return _binning_info_;
  }

  double histogram::get_logarithmic_binning () const
  {
    DT_THROW_IF (! is_logarithmic_binning (), std::logic_error, "No logarithmic binning!");
    return -_binning_info_;
  }


  bool histogram::is_initialized () const
  {
    return _h_ != 0;
  }

  void histogram::initialize (size_t n_, double min_, double max_, unsigned int mode_)
  {
    init (n_, min_, max_, mode_);
    return;
  }

  void histogram::initialize (const histogram_2d & h_,
                              int bin_axis_,
                              const std::vector<std::string> & imported_aux_prefixes_)
  {
    DT_THROW_IF (! h_.is_initialized (),std::logic_error,"Invalid 2D-histogram source !");
    // Reset internals :
    if (_h_ != 0) {
      gsl_histogram_free (_h_);
      _h_ = 0;
    }
    reset_counters ();
    // Steal the internal structure of the source histogram :
    datatools::properties aux;

    if (bin_axis_ == BIN_AXIS_X)
      {
        _binning_info_ = h_.get_x_binning_info ();
        _h_ = gsl_histogram_alloc (h_._h_->nx);
        gsl_histogram_set_ranges (_h_, h_._h_->xrange, h_._h_->nx + 1);
        h_._auxiliaries_.export_and_rename_starting_with(aux, "x.unit.", "unit.");
        h_._auxiliaries_.export_starting_with(aux, "display.xaxis.");
        h_._auxiliaries_.export_and_rename_starting_with(aux, "display.zaxis.", "display.yaxis.");
       }
    else if (bin_axis_ == BIN_AXIS_Y)
      {
        _binning_info_ = h_.get_y_binning_info ();
        _h_ = gsl_histogram_alloc (h_._h_->ny);
        gsl_histogram_set_ranges (_h_, h_._h_->yrange, h_._h_->ny + 1);
        h_._auxiliaries_.export_and_rename_starting_with(aux, "y.unit.", "unit.");
        h_._auxiliaries_.export_and_rename_starting_with(aux, "display.yaxis.", "display.xaxis.");
        h_._auxiliaries_.export_and_rename_starting_with(aux, "display.zaxis.", "display.yaxis.");
     }
    else
      {
        DT_THROW_IF(true, std::logic_error, "Invalid bin axis value !");
      }
    aux.export_all (_auxiliaries_);
    for (int i = 0; i < imported_aux_prefixes_.size(); i++)
      {
        h_._auxiliaries_.export_starting_with(_auxiliaries_,
                                              imported_aux_prefixes_[i]);
      }
    return;
  }

  void histogram::initialize (const histogram & h_,
                              const std::vector<std::string> & imported_aux_prefixes_)
  {
    DT_THROW_IF (&h_ == &(*this), std::logic_error, "Self-reference initialization is forbidden!");
    DT_THROW_IF (! h_.is_initialized (), std::logic_error, "Invalid 1D-histogram source !");
    // Reset internals :
    if (_h_ != 0)
      {
        gsl_histogram_free (_h_);
        _h_ = 0;
      }
    reset_counters ();
    // Steal the internal structure of the source histogram :
    _binning_info_ = h_._binning_info_;
    datatools::properties aux;
    h_._auxiliaries_.export_starting_with(aux, "unit.");
    h_._auxiliaries_.export_starting_with(aux, "display.");
    aux.export_all (_auxiliaries_);
    for (int i = 0; i < imported_aux_prefixes_.size(); i++)
      {
        h_._auxiliaries_.export_starting_with(_auxiliaries_,
                                              imported_aux_prefixes_[i]);
      }
    if (h_._h_ != 0)
      {
        _h_ = gsl_histogram_clone (h_._h_);
      }
    return;
  }

  void histogram::init (size_t n_, double min_, double max_, unsigned int mode_)
  {
    DT_THROW_IF (n_ < 1, std::logic_error, "Invalid size!");
    DT_THROW_IF (min_ >= max_, std::logic_error, "Invalid range !");
    reset_counters ();
    if (_h_ != 0) {
      gsl_histogram_free (_h_);
      _h_ = 0;
    }
    if (mode_ == BIN_MODE_LINEAR)
      {
        _binning_info_ = (max_ -  min_) / n_;
        _h_ = gsl_histogram_alloc (n_);
        gsl_histogram_set_ranges_uniform (_h_, min_, max_);
      }
    else
      {
        DT_THROW_IF (min_ <= 0.0, std::logic_error, "Invalid logarithmic range !");
        _h_ = gsl_histogram_alloc (n_);
        double log_factor = pow (max_/min_, 1./n_);
        std::vector<double> log_ranges;
        log_ranges.reserve (n_ + 1);
        for (int i = 0; i < (n_ + 1); i++)
          {
            double val = min_ * gsl_pow_int (log_factor, i);
            log_ranges.push_back (val);
          }
        const double & a = * (log_ranges.begin ());
        gsl_histogram_set_ranges (_h_, &a, log_ranges.size ());
        _binning_info_ = -1.0 * log_factor;
      }
    return;
  }

  void histogram::initialize (const std::vector<double> & ranges_)
  {
    init (ranges_);
    return;
  }


  void histogram::init (const std::vector<double> & ranges_)
  {
    size_t n = ranges_.size () - 1;
    DT_THROW_IF (n < 1, std::logic_error, "Invalid size!");
    reset_counters ();
    if (_h_ != 0)
      {
        gsl_histogram_free (_h_);
        _h_ = 0;
      }
    _h_ = gsl_histogram_alloc (n);
    const double & a = * (ranges_.begin ());
    gsl_histogram_set_ranges (_h_, &a, ranges_.size ());
    _binning_info_ = 0.0;
    return;
  }

  histogram::histogram ()
  {
    invalidate_counters ();
    _binning_info_ = std::numeric_limits<double>::quiet_NaN ();
    _h_ = 0;
    return;
  }

  histogram::histogram (size_t n_, double min_, double max_,
                        unsigned int mode_)
  {
    invalidate_counters ();
    _binning_info_ = std::numeric_limits<double>::quiet_NaN ();
    _h_ = 0;
    init (n_, min_, max_, mode_);
    return;
  }

  histogram::histogram (const std::vector<double> & ranges_)
  {
    invalidate_counters ();
    _binning_info_ = std::numeric_limits<double>::quiet_NaN ();
    _h_ = 0;
    init (ranges_);
    return;
  }

  histogram::~histogram ()
  {
    if (_h_ != 0)
      {
        destroy ();
      }
    return;
  }

  histogram::histogram (const histogram & h_)
  {
    _counts_      = h_._counts_;
    _underflow_   = h_._underflow_;
    _overflow_    = h_._overflow_;
    _binning_info_ = h_._binning_info_;
    _auxiliaries_ = h_._auxiliaries_;
    _h_           = 0;
    if (h_._h_ != 0)
      {
        _h_ = gsl_histogram_clone (h_._h_);
      }
    //std::cerr << "DEVEL: histogram::ctor/copy...\n";
    //dump(std::cerr);
    return;
  }

  void histogram::invalidate_counters ()
  {
    invalidate_counts ();
    invalidate_underflow_overflow ();
    return;
  }

  void histogram::reset_counters ()
  {
    reset_counts ();
    reset_underflow_overflow ();
    return;
  }

  void histogram::reset_counts ()
  {
    _counts_ = 0;
    return;
  }

  void histogram::invalidate_counts ()
  {
    _counts_ = -1;
    return;
  }

  void histogram::increment_counts ()
  {
    if (! is_counts_available ()) return;
    _counts_++;
    return;
  }

  void histogram::invalidate_underflow_overflow ()
  {
    _underflow_ = std::numeric_limits<double>::quiet_NaN ();
    _overflow_  = std::numeric_limits<double>::quiet_NaN ();
    return;
  }

  void histogram::increment_underflow (double weight_)
  {
    if (_underflow_ != _underflow_) return;
    _underflow_ += weight_;
    return;
  }

  void histogram::increment_overflow (double weight_)
  {
    if (_overflow_ != _overflow_) return;
    _overflow_ += weight_;
    return;
  }

  void histogram::reset_underflow_overflow ()
  {
    _underflow_ = 0.0;
    _overflow_  = 0.0;
    return;
  }


  bool histogram::are_underflow_overflow_available () const
  {
    return _overflow_ == _overflow_ && _underflow_ == _underflow_;
  }

  bool histogram::is_counts_available () const
  {
    return _counts_ >= 0;
  }

  int32_t histogram::counts () const
  {
    return _counts_;
  }

  histogram & histogram::operator= (const histogram & h_)
  {
    if (& (*this) == &h_) return *this;

    if  (_h_ != 0) {
      gsl_histogram_free (_h_);
      _h_=0;
    }
    _auxiliaries_  = h_._auxiliaries_;
    _binning_info_ = h_._binning_info_;
    _counts_       = h_._counts_;
    _underflow_    = h_._underflow_;
    _overflow_     = h_._overflow_;
    if (h_._h_ != 0)
      {
        _h_ = gsl_histogram_clone (h_._h_);
      }
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

  bool histogram::is_inside (double x_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    return (x_ >= _h_->range[0]) && (x_ < _h_->range[_h_->n]);
  }

  void histogram::accumulate (double x_ , double weight_)
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    if (! is_inside (x_)) {
      return;
    }
    gsl_histogram_accumulate (_h_, x_, weight_);
    increment_counts ();
    return;
  }

  void histogram::fill (double x_ , double weight_)
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    if (x_ < _h_->range[0]) {
      increment_underflow (weight_);
      return;
    }
    if (x_ >= _h_->range[_h_->n]) {
      increment_overflow (weight_);
      return;
    }
    gsl_histogram_accumulate (_h_, x_, weight_);
    increment_counts ();
    return;
  }

  void histogram::set (size_t i_, double value_)
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    DT_THROW_IF (i_ < 0 ||  i_ >= _h_->n,
                 std::logic_error, "Invalid range !");
    invalidate_counters ();
    _h_->bin[i_] = value_;
    return;
  }

  double histogram::at (size_t i_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    DT_THROW_IF (i_ < 0 ||  i_ >= _h_->n, std::logic_error, "Invalid range !");
    return gsl_histogram_get (_h_, i_);
  }

  double histogram::get (size_t i_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    return gsl_histogram_get (_h_,i_);
  }

  bool histogram::find (double x_ , size_t & i_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    return gsl_histogram_find (_h_, x_, &i_) == GSL_SUCCESS;
  }

  double histogram::min () const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    return gsl_histogram_min (_h_);
  }

  double histogram::max () const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    return gsl_histogram_max (_h_);
  }

  size_t histogram::bins () const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    return gsl_histogram_bins (_h_);
  }

  void histogram::destroy ()
  {
    if (is_initialized ()) {
      invalidate_counters ();
      reset ();
      gsl_histogram_free (_h_);
      _h_ = 0;
      _auxiliaries_.clear ();
      _binning_info_ = std::numeric_limits<double>::quiet_NaN ();
    }
    return;
  }

  void histogram::reset ()
  {
    reset_counters ();
    gsl_histogram_reset (_h_);
    return;
  }

  bool histogram::can_rebin (size_t new_bins_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    if (! is_uniform_binning ()) {
      return false;
    }
    size_t n = gsl_histogram_bins (_h_);
    if (n < new_bins_) {
      return false;
    }
    if ((n % new_bins_) != 0) {
      return false;
    }
    return true;
  }

  void histogram::rebin (size_t new_bins_)
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    DT_THROW_IF (! can_rebin (new_bins_), std::logic_error, "New number of bins is invalid!");
    _binning_info_ = std::numeric_limits<double>::quiet_NaN ();
    size_t n  = gsl_histogram_bins (_h_);
    size_t f  = n / new_bins_;
    size_t n2 = new_bins_;
    gsl_histogram * h2 = gsl_histogram_alloc (n2);

    size_t nrange2  = n2 + 1;
    double * range2 = new double[nrange2];

    for (int i = 0; i < nrange2; i++)
      {
        int i2 = i * f;
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
        gsl_histogram_accumulate (h2, 0.5 * (range2[i] + range2[i+1]), w);
      }
    delete[] range2;
    gsl_histogram_free (_h_);
    _h_ = 0;
    _h_ = h2;
    invalidate_counters ();
    return;
  }

  double histogram::min_val () const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    return gsl_histogram_min_val (_h_);
  }

  size_t histogram::min_bin () const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    return gsl_histogram_min_bin (_h_);
  }

  double histogram::max_val () const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    return gsl_histogram_max_val (_h_);
  }

  size_t histogram::max_bin () const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    return gsl_histogram_max_bin (_h_);
  }

  double histogram::mean () const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    return gsl_histogram_mean (_h_);
  }

  double histogram::sigma () const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    return gsl_histogram_sigma (_h_);
  }

  double histogram::sum () const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    return gsl_histogram_sum (_h_);
  }

  double histogram::sum (size_t begin_, size_t end_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
    double s = 0.0;
    for (size_t i = begin_; i < end_; i++) {
      s += gsl_histogram_get (_h_, i);
    }
    return s;
  }

  void histogram::to_stream (std::ostream & out_) const
  {
    if (is_initialized())
      {
        out_ << '1' << ' '; // validity byte
        out_ << _h_->n;
        for (size_t i = 0; i < _h_->n; i++) {
          out_  << ' ' << _h_->range[i]
                << ' ' << _h_->range[i+1]
                << ' ' << _h_->bin[i];
        }
        out_ << ' ' << _binning_info_ << ' ';
        out_ << _counts_ << ' ';
        if (are_underflow_overflow_available ())
          {
            out_ << '1' << ' ' << _underflow_ << ' ' << _overflow_ << ' ';
          }
        else
          {
            out_ << '0' << ' ';
          }
      }
    else
      {
        out_ << '0' << ' '; // invalidity byte
      }
    return;
  }

  void histogram::from_stream (std::istream & in_)
  {
    if (is_initialized ())
      {
        destroy ();
      }
    std::string token;
    in_ >> token;
    if (token == "0")
      {
        // checked invalidity byte
        return;
      }
    else if (token != "1")
      {
        DT_THROW_IF(true, std::logic_error, "Unrecognized histogram 1D validation token !");
      }
    unsigned int n;
    in_ >> n;
    DT_THROW_IF (! in_, std::logic_error, "Cannot read histogram size from stream!");
    DT_THROW_IF (n < 1, std::logic_error, "Invalid histogram size!");
    initialize (n, 0.0, 1.0, BIN_MODE_LINEAR);
    for (size_t i = 0; i < n; i++) {
      double min, max, bin_counts;
      in_ >> min >> max >> bin_counts;
      DT_THROW_IF (! in_, std::logic_error, "Cannot read histogram bin contents from stream!");
      if (i==0) _h_->range[i]=min;
      _h_->range[i+1]=max;
      _h_->bin[i]=bin_counts;
    }
    int counts;
    double binning_info, underflow, overflow;
    in_ >> binning_info >> counts;
    std::string uo_token;
    in_ >> uo_token;
    if (uo_token == "1")
      {
        in_ >> underflow >> overflow;
      }
    else
      {
        invalidate_underflow_overflow ();
      }
    DT_THROW_IF (! in_, std::logic_error, "Cannot read histogram underflow/overflow from stream!");
    _binning_info_ = binning_info;
    _counts_       = counts;
    _underflow_    = underflow;
    _overflow_     = overflow;
    return;
  }

  void histogram::print (std::ostream & out_ , int precision_) const
  {
    out_.precision (precision_);
    out_ << "#@mygsl::histogram::nbins=" << ' ' << _h_->n << std::endl;
    out_ << "#@mygsl::histogram::binning_info=" << ' ' << _binning_info_ << std::endl;
    out_ << "#@mygsl::histogram::counts=" << _counts_ << std::endl;
    out_ << "#@mygsl::histogram::underflow=" << _underflow_ << std::endl;
    out_ << "#@mygsl::histogram::overflow=" << _overflow_ << std::endl;
    for (size_t i = 0; i < _h_->n; i++) {
      out_  << _h_->range[i]
            << ' ' << _h_->range[i+1]
            << ' ' << _h_->bin[i]
            << std::endl;
    }
    return;
  }


  void histogram::print_ascii (std::ostream & out_, int mode_) const
  {
    size_t screen_width = 55;
    double ymin = min_val ();
    double ymax = max_val ();
    if (ymin == 0 & ymax == 0)
      {
        ymin = 0;
        ymax = +1;
      }
    else if (ymin > 0)
      {
        ymin = 0;
      }
    else if (ymin < 0 && ymax < 0)
      {
        ymax = 0;
      }
    double dy = 1.0;
    if (ymax > ymin)
      {
        dy = (ymax - ymin) / screen_width;
      }
    int j0 = 0;
    if (ymin < 0)
      {
        j0 = (int) (-ymin / dy);
      }
    int prec = 9;
    int jskip = prec + 4;

    // std::cout << "DEVEL: j0   = " << j0 << std::endl;
    // std::cout << "DEVEL: ymin = " << ymin << std::endl;
    // std::cout << "DEVEL: ymax = " << ymax << std::endl;
    // std::cout << "DEVEL: dy   = " << dy << std::endl;

    // rule:
    for (size_t i = 0; i < screen_width + jskip + prec; i++) {
      out_ << '-';
    }
    out_ << std::endl;
    // print lower bound :
    if (ymin < -2*dy)
      {
        out_.precision (prec);
        out_.width (jskip);
        out_ << std::right << ymin;
      }
    else
      {
        for (size_t i = 0; i < jskip ; i++) {
          out_ << ' ';
        }
      }
    // print 0 :
    for (size_t i = 0; i < screen_width; i++) {
      if (i == j0) out_ << '0';
      else out_ << ' ';
    }
    // print upper bound :
    if (ymax > 2*dy || ymax == 0)
      {
        out_.precision (prec);
        //out_.width ();
        out_ << std::left << ymax;
      }
      else
        {
          // for (size_t i = 0; i < jskip ; i++) {
          //   out_ << ' ';
          // }
        }
  out_ << std::endl;
  // rule:
  for (size_t i = 0; i < screen_width + jskip + prec; i++) {
    out_ << '-';
  }
  out_ << std::endl;
  for (size_t i = 0; i < _h_->n; i++) {
    // bin position :
    out_.precision (prec);
    out_.width (prec + 1);
    // bin value :
    out_ << std::right << _h_->range[i];
    out_ << " : ";
    double y = _h_->bin[i];
    int jmin = j0;
    int jmax = (int) ((y - ymin) / dy);
    if (y < 0.0)
      {
        jmin = j0 - (int) ((std::abs(y)) / dy);
        jmax = j0;
      }
    //std::cerr << "DEVEL: jmin=" << jmin << std::endl;
    //std::cerr << "DEVEL: jmax=" << jmax << std::endl;
    for (int j = 0; j < screen_width; j++)
      {
        if (j == j0)
          {
            out_ << '|';
          }
        else if (j < jmin)
          {
            out_ << ' ';
          }
        else if (j < jmax)
          {
            out_ << '*';
          }
        else
          {
            out_ << ' ';
          }
      }
    if (ymax == 0) out_ << '|';
    out_ << std::endl;
  }
  // rule:
  for (size_t i = 0; i < screen_width + jskip + prec; i++) {
    out_ << '-';
  }
  out_ << std::endl;
  // statistics:
  out_ << "Counts = " << _counts_
       << "    Underflow=" << _underflow_
       << "    Overflow=" << _overflow_ << std::endl;
  // rule:
  for (size_t i = 0; i < screen_width + jskip + prec; i++) {
    out_ << '-';
  }
  out_ << std::endl;

  return;
}

void histogram::dump (std::ostream & out_ , int precision_) const
{
  out_.precision (9);
  out_ << "Histogram 1D:" << std::endl;
  if (is_initialized ())
    {
      out_ << "  Binning   = " << _binning_info_ << std::endl;
      out_ << "  Counts    = " << counts () << std::endl;
      out_ << "  Underflow = " << underflow () << std::endl;
      out_ << "  Overflow  = " << overflow () << std::endl;
      out_ << "  Min       = " << min () << std::endl;
      out_ << "  Max       = " << max () << std::endl;
      out_ << "  Min val   = " << min_val () << std::endl;
      out_ << "  Max val   = " << max_val () << std::endl;
      out_ << "  #bins     = " << bins () << std::endl;
      out_ << "  Mean      = " << mean () << std::endl;
      out_ << "  Sigma     = " << sigma () << std::endl;
      out_ << "  Sum       = " << sum () << std::endl;
    }
  else
    {
      out_ << "  <not initialized>" << std::endl;
    }
  return;
}

double histogram::operator[] (size_t i_) const
{
  return at (i_);
}

std::pair<double,double> histogram::get_range (size_t i_) const
{
  DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  std::pair<double,double> p;
  gsl_histogram_get_range (_h_,i_,&p.first,&p.second);
  return p;
}

bool histogram::has_size (size_t bins_) const
{
  DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  return bins_ == bins ();
}

bool histogram::same (const histogram & h_) const
{
  DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  DT_THROW_IF(!h_.is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  return gsl_histogram_equal_bins_p (_h_,h_._h_);
}

void histogram::shift (double s_)
{
  DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  gsl_histogram_shift (_h_,s_);
  invalidate_counters ();
  return;
}

void histogram::scale (double s_)
{
  DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  gsl_histogram_scale (_h_,s_);
  invalidate_counters ();
  return;
}

void histogram::negate ()
{
  DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  gsl_histogram_scale (_h_,-1.0);
  invalidate_counters ();
  return;
}

void histogram::zero ()
{
  DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  reset ();
  return;
}

void histogram::add (const histogram & h_)
{
  DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  DT_THROW_IF(!h_.is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  gsl_histogram_add (_h_,h_._h_);
  invalidate_counters ();
  return;
}

void histogram::sub (const histogram & h_)
{
  DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  DT_THROW_IF(!h_.is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  gsl_histogram_sub (_h_,h_._h_);
  invalidate_counters ();
  return;
}

void histogram::mul (const histogram & h_)
{
  DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  DT_THROW_IF(!h_.is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  gsl_histogram_mul (_h_,h_._h_);
  invalidate_counters ();
  return;
}

void histogram::div (const histogram & h_)
{
  DT_THROW_IF(!is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  DT_THROW_IF(!h_.is_initialized(), std::logic_error, " Histogram 1D is not initialized !");
  gsl_histogram_div (_h_,h_._h_);
  invalidate_counters ();
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

histogram::pdf::pdf ()
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

bool histogram::pdf::is_initialized () const
{
  return _pdf_ != 0;
}

void histogram::pdf::initialize (const histogram & h_)
{
  init (h_);
  return;
}

void histogram::pdf::init (const histogram & h_)
{
  if  (_pdf_ != 0)
    {
      reset ();
    }
  _pdf_ = gsl_histogram_pdf_alloc (h_.bins ());
  DT_THROW_IF (_pdf_ == 0, std::logic_error, "Cannot allocate histogram's PDF !");
  gsl_histogram_pdf_init (_pdf_, h_._h_);
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

void histogram::pdf::sample (double r_, double & x_)
{
  x_ = gsl_histogram_pdf_sample (_pdf_, r_);
  return;
}

void histogram::tree_dump(std::ostream& out_,
                          const std::string& title_,
                          const std::string& indent_,
                          bool inherit_) const
{
  std::string indent;
  if (!indent_.empty()) indent = indent_;
  if (!title_.empty()) out_ << indent << title_ << std::endl;

  if (!is_initialized ())
    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "<not initialized>" << std::endl;
      return;
    }

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Lower bound : " << min () << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Upper bound : " << max () << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Number of bins : " << bins () << std::endl;

  if (is_uniform_binning ())
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Uniform binning with length : " << _binning_info_ << std::endl;
    }
  else if (is_logarithmic_binning ())
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Logarithmic binning with factor : " << -1.0 * _binning_info_ << std::endl;
    }
  else
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Unresolved binning information." << std::endl;
    }

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Counts      : " <<  _counts_ << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Sum         : " <<  sum () << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Underflow   : " <<  _underflow_ << std::endl;

  out_ << indent << datatools::i_tree_dumpable::tag
       << "Overflow    : " <<  _overflow_ << std::endl;

  out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
       << "Auxiliaries : ";
  if (_auxiliaries_.empty ())
    {
      out_ << "<empty>";
    }
  out_ << endl;
  {
    ostringstream indent_oss;
    indent_oss << indent;
    indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_) ;
    _auxiliaries_.tree_dump (out_, "", indent_oss.str ());
  }

  return;
}

}

// end of mygsl::histogram.cc
