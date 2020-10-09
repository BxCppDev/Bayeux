/// \file mygsl/histogram_2d.h

#ifndef MYGSL_HISTOGRAM_2D_H
#define MYGSL_HISTOGRAM_2D_H 1

// Standard library:
#include <iostream>
#include <vector>

// Third party:
// - GSL:
#include <gsl/gsl_histogram2d.h>
#include <gsl/gsl_vector.h>
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <datatools/i_serializable.h>
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>

// This project:
#include <mygsl/histogram_utils.h>

namespace mygsl {

  class histogram;

  /// \brief Two dimensional histogram
  class histogram_2d : public datatools::i_serializable
  {
  public:

    friend class histogram;

    const datatools::properties & get_auxiliaries () const;

    datatools::properties & grab_auxiliaries ();

    double get_x_binning_info () const;

    double get_y_binning_info () const;

    bool is_uniform_x_binning () const;

    bool is_logarithmic_x_binning () const;

    bool is_any_x_binning () const;

    double get_uniform_x_binning () const;

    double get_logarithmic_x_binning () const;

    bool is_uniform_y_binning () const;

    bool is_logarithmic_y_binning () const;

    bool is_any_y_binning () const;

    double get_uniform_y_binning () const;

    double get_logarithmic_y_binning () const;

    bool is_initialized () const;

    void initialize (size_t nx_, double xmin_, double xmax_,
                     size_t ny_, double ymin_, double ymax_,
                     unsigned int xmode_ = BIN_MODE_LINEAR,
                     unsigned int ymode_ = BIN_MODE_LINEAR);

    void initialize (const std::vector<double> & xranges_,
                     const std::vector<double> & yranges_);

    void initialize (const histogram & hx_, const histogram & hy_,
                     const std::vector<std::string> & imported_aux_prefixes_);

    void initialize (const histogram_2d & h_,
                     const std::vector<std::string> & imported_aux_prefixes_);

    void init (size_t nx_, double xmin_, double xmax_,
               size_t ny_, double ymin_, double ymax_,
               unsigned int xmode_ = BIN_MODE_LINEAR,
               unsigned int ymode_ = BIN_MODE_LINEAR);

    void init (const std::vector<double> & xranges_,
               const std::vector<double> & yranges_);

    /// Destroy the histogram structure, making the histogram invalid
    void destroy ();

    bool are_underflow_overflow_available () const;

    void invalidate_underflow_overflow ();

    void reset_underflow_overflow ();

    void increment_x_underflow (double weight_);

    void increment_x_overflow (double weight_);

    void increment_y_underflow (double weight_);

    void increment_y_overflow (double weight_);

    void invalidate_counters ();

    void reset_counters ();

    bool is_counts_available () const;

    int32_t counts () const;

    void reset_counts ();

    void increment_counts ();

    void invalidate_counts ();

    histogram_2d ();

    histogram_2d (size_t nx_, double xmin_, double xmax_,
                  size_t ny_, double ymin_, double ymax_,
                  unsigned int xmode_ = BIN_MODE_LINEAR,
                  unsigned int ymode_ = BIN_MODE_LINEAR);

    histogram_2d (const std::vector<double> & xranges_,
                  const std::vector<double> & yranges_);

    ~histogram_2d () override;

    histogram_2d (const histogram_2d &);

    histogram_2d & operator= (const histogram_2d &);

    bool is_inside (double x_, double y_) const;

    void accumulate (double x_, double y_, double weight_ = 1.0);

    void fill (double x_, double y_, double weight_ = 1.0);

    void fill (int i_ , double y_, double safe_x_delta_ = 1e-7, double weight_ = 1.0);

    void fill (double x_, int j_, double safe_y_delta_ = 1e-7, double weight_ = 1.0);

    void fill (int i_, int j_,
               double safe_x_delta_ = 1e-7, double safe_y_delta_ = 1e-7,
               double weight_ = 1.0);

    double underflow_x () const;

    double overflow_x () const;

    double underflow_y () const;

    double overflow_y () const;

    double xmin () const;

    double xmax () const;

    double ymin () const;

    double ymax () const;

    size_t xbins () const;

    size_t ybins () const;

    void set (size_t ix_, size_t iy_, double value_);

    double get (size_t ix_, size_t iy_) const;

    double at (size_t ix_, size_t iy_) const;

    bool find (double x_, double y_, size_t & i_, size_t & j_) const;

    /// Reset the bin contents to zero
    void reset ();

    double min_val () const;

    double max_val () const;

    void min_bin (size_t & i_, size_t & j_) const;

    void max_bin (size_t & i_, size_t & j_) const;

    double xmean () const;

    double xsigma () const;

    double ymean () const;

    double ysigma () const;

    double sum () const;

    double cov () const;

    void to_stream (std::ostream &) const;

    void from_stream (std::istream &);

    void print (std::ostream & ,
                int precision_ = 9,
                bool x_skip_line_ = true) const;

    void dump (std::ostream & , int precision_ = 9) const;

    bool has_size (size_t xbins_, size_t ybins_) const;

    bool has_xsize (size_t xbins_) const;

    bool has_ysize (size_t ybins_) const;

    std::pair<double,double> get_xrange (size_t i_) const;

    std::pair<double,double> get_yrange (size_t j_) const;

    void shift (double s_);

    void scale (double s_);

    void negate ();

    void zero ();

    bool same (const histogram_2d &) const;

    void add (const histogram_2d &);

    void sub (const histogram_2d &);

    void mul (const histogram_2d &);

    void div (const histogram_2d &);

    histogram_2d & operator+= (const histogram_2d &);

    histogram_2d & operator-= (const histogram_2d &);

    histogram_2d & operator*= (const histogram_2d &);

    histogram_2d & operator/= (const histogram_2d &);

    histogram_2d & operator+= (double);

    histogram_2d & operator-= (double);

    histogram_2d & operator- ();

    histogram_2d & operator*= (double);

    histogram_2d & operator/= (double);

    friend histogram_2d operator+ (const histogram_2d & ,
                                const histogram_2d &);

    friend histogram_2d operator- (const histogram_2d & ,
                                const histogram_2d &);

    friend histogram_2d operator* (const histogram_2d & ,
                                const histogram_2d &);

    friend histogram_2d operator/ (const histogram_2d & ,
                                const histogram_2d &);

    friend histogram_2d operator* (double ,
                                const histogram_2d &);

    friend histogram_2d operator/ (double ,
                                const histogram_2d &);

    friend histogram_2d operator* (const histogram_2d & ,
                                double);

    friend histogram_2d operator/ (const histogram_2d & ,
                                double);

    /// Main interface method for smart dump
    virtual void tree_dump (std::ostream & out         = std::clog,
                            const std::string & title  = "",
                            const std::string & indent = "",
                            bool inherit               = false) const;

  private:

    double            _x_binning_info_;
    double            _y_binning_info_;
    int32_t           _counts_;
    double            _x_underflow_;
    double            _x_overflow_;
    double            _y_underflow_;
    double            _y_overflow_;
    gsl_histogram2d * _h_;  // bin(i,j) = bin[i * ny + j].
    datatools::properties _auxiliaries_;

    DATATOOLS_SERIALIZATION_DECLARATION()

  public:

    class pdf
    {

    public:

      pdf ();

      pdf (const histogram_2d & h_);

      virtual ~pdf ();

      bool is_initialized () const;

      void initialize (const histogram_2d & h_);

      void init (const histogram_2d & h_);

      void reset ();

      void sample (double r1_, double r2_, double & x_, double & y_);

      template<class _ran_functor_t>
      void sample (_ran_functor_t & ran_, double & x_, double & y_)
      {
        return gsl_histogram_pdf_sample (_pdf_, ran_(), ran_(), &x_, &y_);
      }

    private:

      gsl_histogram2d_pdf * _pdf_;

    };

  };

} // end of namespace mygsl

#endif // MYGSL_HISTOGRAM_2D_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
