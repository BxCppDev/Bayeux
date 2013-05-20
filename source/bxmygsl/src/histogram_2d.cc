// mygsl::histogram_2d.cc

#include <mygsl/histogram_2d.h>
#include <mygsl/histogram_1d.h>

#include <stdexcept>
#include <sstream>
#include <limits>
#include <cmath>

#include <gsl/gsl_math.h>
#include <boost/preprocessor/stringize.hpp>

namespace mygsl {

  using namespace std;

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(histogram_2d,"mygsl::histogram_2d")

  const datatools::properties & histogram_2d::get_auxiliaries () const
  {
    return _auxiliaries_;
  }

  datatools::properties & histogram_2d::grab_auxiliaries ()
  {
    return _auxiliaries_;
  }

  bool histogram_2d::is_counts_available () const
  {
    return _counts_ >= 0;
  }

  int32_t histogram_2d::counts () const
  {
    return _counts_;
  }

  double histogram_2d::get_x_binning_info () const
  {
    return _x_binning_info_;
  }

 double histogram_2d::get_y_binning_info () const
  {
    return _y_binning_info_;
  }

  bool histogram_2d::is_uniform_x_binning() const
  {
    return _x_binning_info_ > 0;
  }

  bool histogram_2d::is_logarithmic_x_binning() const
  {
    return _x_binning_info_ < 0;
  }

  bool histogram_2d::is_any_x_binning() const
  {
    return _x_binning_info_ == 0;
  }

  bool histogram_2d::is_uniform_y_binning() const
  {
    return _y_binning_info_ > 0;
  }

  bool histogram_2d::is_logarithmic_y_binning() const
  {
    return _y_binning_info_ < 0;
  }

  bool histogram_2d::is_any_y_binning() const
  {
    return _y_binning_info_ == 0;
  }

  double histogram_2d::get_uniform_x_binning () const
  {
    DT_THROW_IF (! is_uniform_x_binning (), std::logic_error, "No uniform X binning!");
    return _x_binning_info_;
  }

  double histogram_2d::get_logarithmic_x_binning () const
  {
    DT_THROW_IF (! is_logarithmic_x_binning (), std::logic_error, "No log X binning!");
    return -_x_binning_info_;
  }

  double histogram_2d::get_uniform_y_binning () const
  {
    DT_THROW_IF (! is_uniform_y_binning (), std::logic_error, "No uniform Y binning!");
    return _y_binning_info_;
  }

  double histogram_2d::get_logarithmic_y_binning () const
  {
    DT_THROW_IF (! is_logarithmic_y_binning (), std::logic_error, "No log Y binning!");
    return -_y_binning_info_;
  }

  bool histogram_2d::is_initialized () const
  {
    return _h_ != 0;
  }

  void histogram_2d::initialize (const histogram_2d & h_,
                                 const std::vector<std::string> & imported_aux_prefixes_)
  {
    DT_THROW_IF (&h_ == &(*this), std::logic_error, "Self-reference initialization is forbidden!");
    DT_THROW_IF (! h_.is_initialized (), std::logic_error, "Invalid 2D-histogram source !");
    // Reset internals :
    if (_h_ != 0) {
      gsl_histogram2d_free (_h_);
      _h_ = 0;
    }
    reset_counters ();
    // Steal the internal structure of the source histogram :
    _x_binning_info_ = h_._x_binning_info_;
    _y_binning_info_ = h_._y_binning_info_;
    datatools::properties aux;
    h_._auxiliaries_.export_starting_with(aux, "unit.");
    h_._auxiliaries_.export_starting_with(aux, "display.");
    aux.export_all (_auxiliaries_);
    for (int i = 0; i < imported_aux_prefixes_.size(); i++) {
      h_._auxiliaries_.export_starting_with(_auxiliaries_,
                                            imported_aux_prefixes_[i]);
    }
    if (h_._h_ != 0) {
      _h_ = gsl_histogram2d_clone (h_._h_);
    }
    return;
  }

  void histogram_2d::initialize (const histogram_1d & hx_, const histogram_1d & hy_,
                                 const std::vector<std::string> & imported_aux_prefixes_)
  {
    DT_THROW_IF (! hx_.is_initialized (), std::logic_error, "Invalid 1D-histogram source !");
    DT_THROW_IF (! hy_.is_initialized (), std::logic_error, "Invalid 1D-histogram source !");
    // Reset internals :
    if (_h_ != 0) {
      gsl_histogram2d_free (_h_);
      _h_ = 0;
    }
    reset_counters ();
    // Steal the internal structure of the source histogram :
    _x_binning_info_ = hx_.get_binning_info ();
    _y_binning_info_ = hy_.get_binning_info ();
    datatools::properties aux;
    hx_.get_auxiliaries ().export_and_rename_starting_with(aux, "unit.", "x.unit.");
    hx_.get_auxiliaries ().export_starting_with(aux, "display.xaxis.");
    hy_.get_auxiliaries ().export_and_rename_starting_with(aux, "unit.", "y.unit.");
    hy_.get_auxiliaries ().export_and_rename_starting_with(aux, "display.xaxis.", "display.yaxis.");
    aux.export_all (_auxiliaries_);
    for (int i = 0; i < imported_aux_prefixes_.size(); i++) {
      hx_.get_auxiliaries ().export_starting_with(_auxiliaries_,
                                                  imported_aux_prefixes_[i]);
      hy_.get_auxiliaries ().export_starting_with(_auxiliaries_,
                                                  imported_aux_prefixes_[i]);
    }
    initialize (hx_.bins (), hx_.min (), hx_.max (),
                hy_.bins (), hy_.min (), hy_.max (),
                hx_.is_logarithmic_binning () ? BIN_MODE_LOG : BIN_MODE_LINEAR,
                hy_.is_logarithmic_binning () ? BIN_MODE_LOG : BIN_MODE_LINEAR);
    return;
  }

  void histogram_2d::initialize (size_t nx_, double xmin_, double xmax_,
                                 size_t ny_, double ymin_, double ymax_,
                                 unsigned int xmode_,
                                 unsigned int ymode_)
  {
    init (nx_, xmin_, xmax_, ny_, ymin_, ymax_, xmode_, ymode_);
    return;
  }

  void histogram_2d::initialize (const std::vector<double> & xranges_,
                                 const std::vector<double> & yranges_)
  {
    init (xranges_, yranges_);
    return;
  }

  void histogram_2d::init (size_t nx_, double xmin_, double xmax_,
                           size_t ny_, double ymin_, double ymax_,
                           unsigned int xmode_,
                           unsigned int ymode_)
  {
    DT_THROW_IF (nx_ < 1, std::logic_error, "Invalid X size !");
    DT_THROW_IF (xmin_ >= xmax_, std::logic_error, "Invalid X range !");
    DT_THROW_IF (ny_ < 1, std::logic_error, "Invalid Y size !");
    DT_THROW_IF (ymin_ >= ymax_, std::logic_error, "Invalid Y range !");
    reset_counters ();
    if (_h_ != 0) {
      gsl_histogram2d_free (_h_);
      _h_ = 0;
    }
    if (xmode_ == BIN_MODE_LINEAR) {
      _x_binning_info_ = (xmax_ -  xmin_) / nx_;
    }
    if (ymode_ == BIN_MODE_LINEAR) {
      _y_binning_info_ = (ymax_ -  ymin_) / ny_;
    }
    if (xmode_ == BIN_MODE_LINEAR && ymode_ == BIN_MODE_LINEAR) {
      _h_ = gsl_histogram2d_alloc (nx_, ny_);
      gsl_histogram2d_set_ranges_uniform (_h_, xmin_, xmax_, ymin_, ymax_);
    } else {
      std::vector<double> xranges;
      xranges.reserve (nx_ + 1);
      if (xmode_ == BIN_MODE_LINEAR) {
        for (int i = 0; i < (nx_ + 1); i++)
          {
            xranges.push_back (xmin_ + i * _x_binning_info_);
          }
      }
      else
        {
          double xfactor = pow (xmax_/xmin_, 1./nx_);
          for (int i = 0; i < (nx_ + 1); i++)
            {
              double xval = xmin_ * gsl_pow_int (xfactor, i);
              xranges.push_back (xval);
            }
          _x_binning_info_ = -1.0 * xfactor;
        }

      std::vector<double> yranges;
      yranges.reserve (ny_ + 1);
      if (ymode_ == BIN_MODE_LINEAR)
        {
          for (int i = 0; i < (ny_ + 1); i++)
            {
              yranges.push_back (ymin_ + i * _y_binning_info_);
            }
        }
      else
        {
          double yfactor = pow (ymax_/ymin_, 1./ny_);
          for (int i = 0; i < (ny_ + 1); i++)
            {
              double yval = ymin_ * gsl_pow_int (yfactor, i);
              yranges.push_back (yval);
            }
          _y_binning_info_ = -1.0 * yfactor;
        }
      const double & ax = * (xranges.begin ());
      const double & ay = * (yranges.begin ());
      _h_ = gsl_histogram2d_alloc (nx_, ny_);
      gsl_histogram2d_set_ranges (_h_, &ax, xranges.size (), &ay, yranges.size ());
    }
    return;
  }

  void histogram_2d::init (const std::vector<double> & xranges_,
                           const std::vector<double> & yranges_)
  {
    size_t nx = xranges_.size () - 1;
    DT_THROW_IF (nx < 1, std::logic_error, "Invalid X size!");
    size_t ny = yranges_.size () - 1;
    DT_THROW_IF (ny < 1, std::logic_error, "Invalid Y size!");
    reset_counters ();
    if (_h_ != 0) {
      gsl_histogram2d_free (_h_);
      _h_ = 0;
    }
    _h_ = gsl_histogram2d_alloc (nx, ny);
    const double & ax = * (xranges_.begin ());
    const double & ay = * (yranges_.begin ());
    gsl_histogram2d_set_ranges (_h_, &ax, xranges_.size (), &ay, yranges_.size ());
    _x_binning_info_ = 0;
    _y_binning_info_ = 0;
    return;
  }

  histogram_2d::histogram_2d ()
  {
    _x_binning_info_ = std::numeric_limits<double>::quiet_NaN ();
    _y_binning_info_ = std::numeric_limits<double>::quiet_NaN ();
    invalidate_counters ();
    _h_ = 0;
    return;
  }

  histogram_2d::histogram_2d (size_t nx_, double xmin_, double xmax_,
                              size_t ny_, double ymin_, double ymax_,
                              unsigned int xmode_,
                              unsigned int ymode_)
  {
    _x_binning_info_ = std::numeric_limits<double>::quiet_NaN ();
    _y_binning_info_ = std::numeric_limits<double>::quiet_NaN ();
    invalidate_counters ();
    _h_ = 0;
    init (nx_, xmin_, xmax_, ny_, ymin_, ymax_);
    return;
  }

  histogram_2d::histogram_2d (const std::vector<double> & xranges_,
                              const std::vector<double> & yranges_)
  {
    _x_binning_info_ = std::numeric_limits<double>::quiet_NaN ();
    _y_binning_info_ = std::numeric_limits<double>::quiet_NaN ();
    invalidate_counters ();
    _h_ = 0;
    init (xranges_, yranges_);
    return;
  }

  histogram_2d::~histogram_2d ()
  {
    if (_h_ != 0) {
      destroy ();
    }
    return;
  }

  void histogram_2d::destroy ()
  {
    if (is_initialized ()) {
      invalidate_counters ();
      reset ();
      gsl_histogram2d_free (_h_);
      _h_ = 0;
      _auxiliaries_.clear ();
      _x_binning_info_ = std::numeric_limits<double>::quiet_NaN ();
      _y_binning_info_ = std::numeric_limits<double>::quiet_NaN ();
    }
    return;
  }

  histogram_2d::histogram_2d (const histogram_2d & h_)
  {
    _x_binning_info_ = h_._x_binning_info_;
    _y_binning_info_ = h_._y_binning_info_;
    _counts_         = h_._counts_;
    _auxiliaries_    = h_._auxiliaries_;
    _h_ = 0;
    if (h_._h_ != 0) {
      _h_ = gsl_histogram2d_clone (h_._h_);
    }
    return;
  }

  void histogram_2d::invalidate_counters ()
  {
    invalidate_counts ();
    invalidate_underflow_overflow ();
    return;
  }

  void histogram_2d::reset_counters ()
  {
    reset_counts ();
    reset_underflow_overflow ();
    return;
  }

  void histogram_2d::reset_counts ()
  {
    _counts_ = 0;
    return;
  }

  void histogram_2d::invalidate_counts ()
  {
    _counts_ = -1;
    return;
  }

  void histogram_2d::increment_counts ()
  {
    if (! is_counts_available ()) return;
    _counts_++;
    return;
  }

  void histogram_2d::invalidate_underflow_overflow ()
  {
    const double nan = std::numeric_limits<double>::quiet_NaN ();
    _x_underflow_ = nan;
    _x_overflow_  = nan;
    _y_underflow_ = nan;
    _y_overflow_  = nan;
    return;
  }

  double histogram_2d::underflow_x () const
  {
    return _x_underflow_;
  }

  double histogram_2d::overflow_x () const
  {
    return _x_overflow_;
  }

  double histogram_2d::underflow_y () const
  {
    return _y_underflow_;
  }

  double histogram_2d::overflow_y () const
  {
    return _y_overflow_;
  }

  void histogram_2d::increment_x_underflow (double weight_)
  {
    if (_x_underflow_ != _x_underflow_) return;
    _x_underflow_ += weight_;
    return;
  }

  void histogram_2d::increment_x_overflow (double weight_)
  {
    if (_x_overflow_ != _x_overflow_) return;
    _x_overflow_ += weight_;
    return;
  }

  void histogram_2d::increment_y_underflow (double weight_)
  {
    if (_y_underflow_ != _y_underflow_) return;
    _y_underflow_ += weight_;
    return;
  }

  void histogram_2d::increment_y_overflow (double weight_)
  {
    if (_y_overflow_ != _y_overflow_) return;
    _y_overflow_ += weight_;
    return;
  }

  void histogram_2d::reset_underflow_overflow ()
  {
    _x_underflow_ = 0.0;
    _x_overflow_  = 0.0;
    _y_underflow_ = 0.0;
    _y_overflow_  = 0.0;
    return;
  }


  bool histogram_2d::are_underflow_overflow_available () const
  {
    return
      (_x_overflow_  == _x_overflow_)  &&
      (_x_underflow_ == _x_underflow_) &&
      (_y_overflow_  == _y_overflow_)  &&
      (_y_underflow_ == _y_underflow_);
  }

  histogram_2d & histogram_2d::operator= (const histogram_2d & h_)
  {
    if (& (*this) == &h_) return *this;

    if  (_h_ != 0) {
      gsl_histogram2d_free (_h_);
      _h_=0;
    }
    _x_binning_info_ = h_._x_binning_info_;
    _y_binning_info_ = h_._y_binning_info_;
    _counts_         = h_._counts_;
    _auxiliaries_    = h_._auxiliaries_;
    _x_underflow_    = h_._x_underflow_;
    _x_overflow_     = h_._x_overflow_;
    _y_underflow_    = h_._y_underflow_;
    _y_overflow_     = h_._y_overflow_;
    if (h_._h_ !=0)
      {
        _h_ = gsl_histogram2d_clone (h_._h_);
      }
    return *this;
  }

  bool histogram_2d::is_inside (double x_, double y_) const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return (x_ >= _h_->xrange[0]) && (x_ < _h_->xrange[_h_->nx])
      && (y_ >= _h_->yrange[0]) && (y_ < _h_->yrange[_h_->ny]);
  }

  void histogram_2d::accumulate (double x_, double y_, double weight_)
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    if (! is_inside (x_, y_)) {
        return;
    }
    gsl_histogram2d_accumulate (_h_,x_,y_,weight_);
    increment_counts ();
    return;
  }

  void histogram_2d::fill (double x_ , double y_ , double weight_)
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    if (x_ < _h_->xrange[0]) {
      increment_x_underflow (weight_);
      return;
    }
    if (y_ < _h_->yrange[0]) {
      increment_y_underflow (weight_);
      return;
    }
    if (x_ >= _h_->xrange[_h_->nx]) {
      increment_x_overflow (weight_);
      return;
    }
    if (y_ >= _h_->yrange[_h_->ny]) {
      increment_y_overflow (weight_);
      return;
    }
    gsl_histogram2d_accumulate (_h_,x_,y_,weight_);
    increment_counts ();
    return;
  }

  double histogram_2d::at (size_t ix_, size_t iy_) const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    DT_THROW_IF (ix_ < 0 || ix_ >= _h_->nx, std::logic_error, "Invalid X range !");
    DT_THROW_IF (iy_ < 0 || iy_ >= _h_->ny, std::logic_error, "Invalid Y range !");
    return gsl_histogram2d_get (_h_,ix_,iy_);
  }

  void histogram_2d::set (size_t ix_, size_t iy_, double value_)
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    DT_THROW_IF (ix_ < 0 || ix_ >= _h_->nx, std::logic_error, "Invalid range for X index!");
    DT_THROW_IF (iy_ < 0 || iy_ >= _h_->ny, std::logic_error, "Invalid range for Y index!");
    invalidate_counters ();
    _h_->bin[ix_ * _h_->ny + iy_] = value_;
    return;
  }

  double histogram_2d::get (size_t ix_, size_t iy_) const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    if (ix_ < 0)        return _x_underflow_;
    if (ix_ >= _h_->nx) return _x_overflow_;
    if (iy_ < 0)        return _y_underflow_;
    if (iy_ >= _h_->ny) return _y_overflow_;
    return gsl_histogram2d_get (_h_, ix_, iy_);
  }

  bool histogram_2d::find (double x_ , double y_ , size_t & i_, size_t & j_) const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_find (_h_, x_, y_, &i_, &j_) == GSL_SUCCESS;
  }

  double histogram_2d::xmin () const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_xmin (_h_);
  }

  double histogram_2d::xmax () const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_xmax (_h_);
  }

  double histogram_2d::ymin () const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_ymin (_h_);
  }

  double histogram_2d::ymax () const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_ymax (_h_);
  }

  size_t histogram_2d::xbins () const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_nx (_h_);
  }

  size_t histogram_2d::ybins () const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_ny (_h_);
  }

  void histogram_2d::reset ()
  {
    reset_counters ();
    gsl_histogram2d_reset (_h_);
    return;
  }

  double histogram_2d::min_val () const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_min_val (_h_);
  }

  void histogram_2d::min_bin (size_t & i_, size_t & j_) const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_min_bin (_h_, &i_, &j_);
  }

  double histogram_2d::max_val () const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_max_val (_h_);
  }

  void histogram_2d::max_bin (size_t & i_, size_t & j_) const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_max_bin (_h_, &i_, &j_);
  }

  double histogram_2d::xmean () const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_xmean (_h_);
  }

  double histogram_2d::xsigma () const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_xsigma (_h_);
  }

  double histogram_2d::ymean () const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_ymean (_h_);
  }

  double histogram_2d::ysigma () const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_ysigma (_h_);
  }

  double histogram_2d::sum () const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_sum (_h_);
  }

  double histogram_2d::cov () const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_cov (_h_);
  }

  void histogram_2d::to_stream (std::ostream & out_) const
  {
    if (is_initialized())
      {
        out_ << '1' << ' '; // validity byte
        out_ << _h_->nx << ' ' << _h_->ny;
        for (size_t i = 0; i <= _h_->nx; i++) {
          out_  << ' ' << _h_->xrange[i];
        }
        for (size_t i = 0; i <= _h_->ny; i++) {
          out_  << ' ' << _h_->yrange[i];
        }
        for (size_t i = 0; i < (_h_->nx * _h_->ny); i++)
          {
            out_ << ' ' << _h_->bin[i];
          }
        out_ << ' ' << _x_binning_info_ << ' ';
        out_ << ' ' << _y_binning_info_ << ' ';
        out_ << _counts_ << ' ';
      }
    else
      {
        out_ << '0' << ' '; // invalidity byte
      }
    return;
  }

  void histogram_2d::from_stream (std::istream & in_)
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
    size_t nx;
    in_ >> nx;
    DT_THROW_IF (! in_, std::logic_error, "Cannot read histogram_2d X size from stream!");
    DT_THROW_IF (nx < 1, std::logic_error, "Invalid histogram_2d X size!");
    size_t ny;
    in_ >> ny;
    DT_THROW_IF (! in_, std::logic_error, "Cannot read histogram_2d Y size from stream!");
    DT_THROW_IF (ny < 1, std::logic_error, "Invalid histogram_2d Y size!");
    init (nx, 0.0, 1.0, ny, 0.0, 1.0);
    for (size_t i = 0; i <= nx; i++) {
      double xr;
      in_ >> xr;
      DT_THROW_IF (! in_, std::logic_error,"Cannot read histogram_2d X range contents from stream !");
      _h_->xrange[i]=xr;
    }
    for (size_t i = 0; i <= ny; i++) {
      double yr;
      in_ >> yr;
      DT_THROW_IF (! in_, std::logic_error, "Cannot read histogram_2d Y range contents from stream !");
      _h_->yrange[i]=yr;
    }
    for (size_t i = 0; i < nx*ny; i++) {
      double count;
      in_ >> count;
      DT_THROW_IF (! in_, std::logic_error, "Cannot read histogram_2d bin count contents from stream !");
      _h_->bin[i] = count;
    }
    int counts;
    double x_binning_info, y_binning_info;
    in_ >> x_binning_info >> y_binning_info >> counts;
    double x_underflow, x_overflow, y_underflow, y_overflow;
    std::string uo_token;
    in_ >> uo_token;
    if (uo_token == "1")
      {
        in_ >> x_underflow >> x_overflow >> y_underflow >> y_overflow;
      }
    else
      {
        invalidate_underflow_overflow ();
      }
    DT_THROW_IF (! in_, std::logic_error, "Cannot read X/Y binning info and counts from stream!");
    _x_binning_info_ = x_binning_info;
    _y_binning_info_ = y_binning_info;
    _x_underflow_    = x_underflow;
    _x_overflow_     = x_overflow;
    _y_underflow_    = y_underflow;
    _y_overflow_     = y_overflow;
    _counts_         = counts;
    return;
  }

  void histogram_2d::print (std::ostream & out_ , int precision_) const
  {
    out_.precision (precision_);
    out_ << "#@mygsl::histogram_2d::nxbins=" << _h_->nx << std::endl;
    out_ << "#@mygsl::histogram_2d::nybins=" << _h_->ny << std::endl;
    out_ << "#@mygsl::histogram_2d::x_binning_info=" << ' ' << _x_binning_info_ << std::endl;
    out_ << "#@mygsl::histogram_2d::y_binning_info=" << ' ' << _y_binning_info_ << std::endl;
    out_ << "#@mygsl::histogram_2d::counts=" << _counts_ << std::endl;
    out_ << "#@mygsl::histogram_2d::x_underflow=" << _x_underflow_ << std::endl;
    out_ << "#@mygsl::histogram_2d::x_overflow=" << _x_overflow_ << std::endl;
    out_ << "#@mygsl::histogram_2d::y_underflow=" << _y_underflow_ << std::endl;
    out_ << "#@mygsl::histogram_2d::y_overflow=" << _y_overflow_ << std::endl;
    for (size_t i=0; i<_h_->nx; i++) {
      for (size_t j=0; j<_h_->ny; j++) {
        out_  << _h_->xrange[i] << ' '
              << _h_->xrange[i+1] << ' '
              << _h_->yrange[j] << ' '
              << _h_->yrange[j+1] << ' '
              << _h_->bin[i * _h_->ny + j]
              << std::endl;
      }
    }
    return;
  }


  void histogram_2d::dump (std::ostream & out_ , int precision_) const
  {
    out_.precision (9);
    out_ << "Histogram 2D:" << std::endl;
    if (is_initialized ())
      {
        out_ << "  X binning   = " << _x_binning_info_ << std::endl;
        out_ << "  Y binning   = " << _y_binning_info_ << std::endl;
        out_ << "  Counts      = " << counts () << std::endl;
        out_ << "  X-underflow = " << underflow_x () << std::endl;
        out_ << "  X-overflow  = " << overflow_x () << std::endl;
        out_ << "  Y-underflow = " << underflow_y () << std::endl;
        out_ << "  Y-overflow  = " << overflow_y () << std::endl;
        out_ << "  X-min       = " << xmin () << std::endl;
        out_ << "  X-max       = " << xmax () << std::endl;
        out_ << "  Y-min       = " << ymin () << std::endl;
        out_ << "  Y-max       = " << ymax () << std::endl;
        out_ << "  #xbins      = " << xbins () << std::endl;
        out_ << "  #ybins      = " << ybins () << std::endl;
        out_ << "  X-mean      = " << xmean () << std::endl;
        out_ << "  X-sigma     = " << xsigma () << std::endl;
        out_ << "  Y-mean      = " << ymean () << std::endl;
        out_ << "  Y-sigma     = " << ysigma () << std::endl;
        out_ << "  Sum         = " << sum () << std::endl;
        out_ << "  Covariance  = " << cov () << std::endl;
      }
    else
      {
        out_ << "  <not initialized>" << std::endl;
      }
    return;
  }

  std::pair<double,double> histogram_2d::get_xrange (size_t i_) const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    std::pair<double,double> p;
    gsl_histogram2d_get_xrange (_h_,i_,&p.first,&p.second);
    return p;
  }

  std::pair<double,double> histogram_2d::get_yrange (size_t i_) const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    std::pair<double,double> p;
    gsl_histogram2d_get_yrange (_h_,i_,&p.first,&p.second);
    return p;
  }

  bool histogram_2d::has_xsize (size_t xbins_) const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return xbins_ == xbins ();
  }

  bool histogram_2d::has_ysize (size_t ybins_) const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return ybins_ == ybins ();
  }

  bool histogram_2d::has_size (size_t xbins_, size_t ybins_) const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return has_xsize (xbins_) && has_ysize (ybins_);
  }

  bool histogram_2d::same (const histogram_2d & h_) const
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    DT_THROW_IF (!h_.is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    return gsl_histogram2d_equal_bins_p (_h_, h_._h_) == 1;
  }

  void histogram_2d::shift (double s_)
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    gsl_histogram2d_shift (_h_,s_);
    invalidate_counters ();
    return;
  }

  void histogram_2d::scale (double s_)
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    gsl_histogram2d_scale (_h_,s_);
    invalidate_counters ();
    return;
  }

  void histogram_2d::negate ()
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    gsl_histogram2d_scale (_h_,-1.0);
    invalidate_counters ();
    return;
  }

  void histogram_2d::zero ()
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    reset ();
    return;
  }

  void histogram_2d::add (const histogram_2d & h_)
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    DT_THROW_IF (!h_.is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    gsl_histogram2d_add (_h_,h_._h_);
    invalidate_counters ();
    return;
  }

  void histogram_2d::sub (const histogram_2d & h_)
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    DT_THROW_IF (!h_.is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    gsl_histogram2d_sub (_h_,h_._h_);
    invalidate_counters ();
    return;
  }

  void histogram_2d::mul (const histogram_2d & h_)
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    DT_THROW_IF (!h_.is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    gsl_histogram2d_mul (_h_,h_._h_);
    invalidate_counters ();
    return;
  }

  void histogram_2d::div (const histogram_2d & h_)
  {
    DT_THROW_IF (!is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    DT_THROW_IF (!h_.is_initialized (), std::logic_error, "Histogram 2D is not initialized !");
    gsl_histogram2d_div (_h_,h_._h_);
    invalidate_counters ();
    return;
  }

  histogram_2d & histogram_2d::operator+= (const histogram_2d & h_)
  {
    this->add (h_);
    return *this;
  }

  histogram_2d & histogram_2d::operator-= (const histogram_2d & h_)
  {
    this->sub (h_);
    return *this;
  }

  histogram_2d & histogram_2d::operator*= (const histogram_2d & h_)
  {
    this->mul (h_);
    return *this;
  }

  histogram_2d & histogram_2d::operator/= (const histogram_2d & h_)
  {
    this->div (h_);
    return *this;
  }

  histogram_2d & histogram_2d::operator+= (double a_)
  {
    shift (a_);
    return *this;
  }

  histogram_2d & histogram_2d::operator-= (double a_)
  {
    shift (-a_);
    return *this;
  }

  histogram_2d & histogram_2d::operator- ()
  {
    negate ();
    return *this;
  }

  histogram_2d & histogram_2d::operator*= (double a_)
  {
    scale (a_);
    return *this;
  }

  histogram_2d & histogram_2d::operator/= (double a_)
  {
    scale (1./a_);
    return *this;
  }

  histogram_2d operator+ (const histogram_2d & h1_ ,
                          const histogram_2d & h2_)
  {
    histogram_2d h (h1_);
    h+=h2_;
    return h;
  }

  histogram_2d operator- (const histogram_2d & h1_ ,
                          const histogram_2d & h2_)
  {
    histogram_2d h (h1_);
    h-=h2_;
    return h;
  }

  histogram_2d operator* (const histogram_2d & h1_ ,
                          const histogram_2d & h2_)
  {
    histogram_2d h (h1_);
    h*=h2_;
    return h;
  }

  histogram_2d operator/ (const histogram_2d & h1_ ,
                          const histogram_2d & h2_)
  {
    histogram_2d h (h1_);
    h /= h2_;
    return h;
  }

  histogram_2d operator* (const histogram_2d & h1_ ,
                          double a_)
  {
    histogram_2d h (h1_);
    h.scale (a_);
    return h;
  }

  histogram_2d operator/ (const histogram_2d & h1_ ,
                          double a_)
  {
    histogram_2d h (h1_);
    h.scale (1. / a_);
    return h;
  }

  histogram_2d operator* (double a_ ,
                          const histogram_2d & h1_)
  {
    histogram_2d h (h1_);
    h.scale (a_);
    return h;
  }

  histogram_2d operator/ (double a_ ,
                          const histogram_2d & h1_)
  {
    histogram_2d h (h1_);
    h.scale (1. / a_);
    return h;
  }

  /*****************************************/

  histogram_2d::pdf::pdf ()
  {
    _pdf_ = 0;
    return;
  }

  histogram_2d::pdf::pdf (const histogram_2d & h_)
  {
    _pdf_ = 0;
    init (h_);
    return;
  }

  histogram_2d::pdf::~pdf ()
  {
    reset ();
    return;
  }

  bool histogram_2d::pdf::is_initialized () const
  {
    return _pdf_ != 0;
  }

  void histogram_2d::pdf::initialize (const histogram_2d & h_)
  {
    init (h_);
    return;
  }

  void histogram_2d::pdf::init (const histogram_2d & h_)
  {
    if  (_pdf_ != 0)
      {
        reset ();
      }
    if (h_._h_ != 0)
      {
        _pdf_ = gsl_histogram2d_pdf_alloc (h_.xbins (), h_.ybins ());
        DT_THROW_IF (_pdf_ == 0, std::logic_error, "Cannot allocate histogram_2d's PDF!");
        gsl_histogram2d_pdf_init (_pdf_, h_._h_);
      }
    return;
  }

  void histogram_2d::pdf::reset ()
  {
    gsl_histogram2d_pdf_free (_pdf_);
    _pdf_ = 0;
    return;
  }

  void histogram_2d::pdf::sample (double r1_, double r2_, double & x_, double & y_)
  {
    gsl_histogram2d_pdf_sample (_pdf_, r1_, r2_, &x_, &y_);
    return;
  }

  void histogram_2d::tree_dump(std::ostream& out_,
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
         << "X lower bound : " << xmin () << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "X upper bound : " << xmax () << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Number of X bins : " << xbins () << std::endl;

    if (is_uniform_x_binning ())
      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Uniform X binning with length : " << _x_binning_info_ << std::endl;
      }
    else if (is_logarithmic_x_binning ())
      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Logarithmic X binning with factor : " << -1.0 *_x_binning_info_ << std::endl;
      }
    else
      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Unresolved X binning information." << std::endl;
      }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Y lower bound : " << ymin () << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Y upper bound : " << ymax () << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Number of Y bins : " << ybins () << std::endl;

    if (is_uniform_y_binning ())
      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Uniform Y binning with length : " << _y_binning_info_ << std::endl;
      }
    else if (is_logarithmic_y_binning ())
      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Logarithmic Y binning with factor : " << -1.0 *_y_binning_info_ << std::endl;
      }
    else
      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Unresolved Y binning information." << std::endl;
      }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Counts      : " <<  _counts_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "X-Underflow : " <<  _x_underflow_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "X-Overflow  : " <<  _x_overflow_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Y-Underflow : " <<  _y_underflow_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Y-Overflow  : " <<  _y_overflow_ << std::endl;

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

// end of mygsl::histogram_2d.cc


