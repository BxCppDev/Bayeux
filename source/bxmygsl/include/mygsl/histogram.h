/// \file mygsl/histogram.h

#ifndef MYGSL_HISTOGRAM_H
#define MYGSL_HISTOGRAM_H 1

// Standard library:
#include <iostream>
#include <vector>

// Third party:
// - GSL:
#include <gsl/gsl_histogram.h>
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

  // Forward declaration:
  class histogram_2d;

  /// \brief One dimensional histogram
  class histogram :
    public datatools::i_serializable,
    public datatools::i_tree_dumpable
  {
  public:

    const datatools::properties & get_auxiliaries () const;

    datatools::properties & grab_auxiliaries ();

    double get_binning_info () const;

    bool is_uniform_binning () const;

    bool is_logarithmic_binning () const;

    bool is_any_binning () const;

    double get_uniform_binning () const;

    double get_logarithmic_binning () const;

    bool is_initialized () const;

    void initialize (size_t n_, double min_, double max_,
                     unsigned int mode_ = BIN_MODE_LINEAR);

    void init (const std::vector<double> & ranges_);

    void initialize (const std::vector<double> & ranges_);

    void initialize (const histogram & h_,
                     const std::vector<std::string> & imported_aux_prefixes_);

    void initialize (const histogram_2d & h_, int bin_axis_,
                     const std::vector<std::string> & imported_aux_prefixes_);

    void init (size_t n_, double min_, double max_,
               unsigned int mode_ = BIN_MODE_LINEAR);

    /// Destroy the histogram structure, making the histogram invalid
    void destroy ();

    histogram ();

    histogram (size_t n_, double min_, double max_,
               unsigned int mode_ = BIN_MODE_LINEAR);

    histogram (const std::vector<double> & ranges_);

    ~histogram () override;

    histogram (const histogram &);

    histogram & operator= (const histogram &);

    bool is_inside (double x_) const;

    void accumulate (double x_ , double weight_ = 1.0);

    void fill (double x_ , double weight_ = 1.0);

    void fill (int i_ , double safe_delta_ = 1e-7, double weight_ = 1.0);

    void set (size_t i_, double value_);

    double underflow () const;

    double overflow () const;

    double min () const;

    double max () const;

    size_t bins () const;

    double get (size_t i_) const;

    double at (size_t i_) const;

    double operator[] (size_t i_) const;

    bool find (double x_ , size_t & i_) const;

    /// Reset the bin contents to zero
    void reset ();

    bool can_rebin (size_t new_bins_) const;

    void rebin (size_t new_bins_);

    bool are_underflow_overflow_available () const;

    void invalidate_underflow_overflow ();

    void reset_underflow_overflow ();

    void increment_underflow (double weight_);

    void increment_overflow (double weight_);

    bool is_counts_available () const;

    int32_t counts () const;

    void invalidate_counters ();

    void reset_counters ();

    void reset_counts ();

    void increment_counts ();

    void invalidate_counts ();

    double min_val () const;

    double max_val () const;

    size_t min_bin () const;

    size_t max_bin () const;

    double mean () const;

    double sigma () const;

    double sum () const;

    double sum (size_t begin_, size_t end_) const;

    void to_stream (std::ostream &) const;

    void from_stream (std::istream &);

    void print (std::ostream & , int precision_ = 9) const;

    void print_ascii (std::ostream & , int mode_ = 0) const;

    void dump (std::ostream & , int precision_ = 9) const;

    bool has_size (size_t bins_) const;

    std::pair<double,double> get_range (size_t i_) const;

    void shift (double s_);

    void scale (double s_);

    void negate ();

    void zero ();

    bool same (const histogram &) const;

    void add (const histogram &);

    void sub (const histogram &);

    void mul (const histogram &);

    void div (const histogram &);

    histogram & operator+= (const histogram &);

    histogram & operator-= (const histogram &);

    histogram & operator*= (const histogram &);

    histogram & operator/= (const histogram &);

    histogram & operator+= (double);

    histogram & operator-= (double);

    histogram & operator- ();

    histogram & operator*= (double);

    histogram & operator/= (double);

    friend histogram operator+ (const histogram & ,
                                const histogram &);

    friend histogram operator- (const histogram & ,
                                const histogram &);

    friend histogram operator* (const histogram & ,
                                const histogram &);

    friend histogram operator/ (const histogram & ,
                                const histogram &);

    friend histogram operator* (double ,
                                const histogram &);

    friend histogram operator* (const histogram & ,
                                double);

    friend histogram operator/ (const histogram & ,
                                double);

    /// Main interface method for smart dump
    void tree_dump (std::ostream& out = std::clog,
                            const std::string& title  = "",
                            const std::string& indent = "",
                            bool inherit = false) const override;

  private:

    double          _binning_info_;
    int32_t         _counts_;
    double          _underflow_;
    double          _overflow_;
    gsl_histogram * _h_;
    datatools::properties _auxiliaries_;

    DATATOOLS_SERIALIZATION_DECLARATION()

  public:

    /// \brief PDF associated to a one dimensional histogram
    class pdf
    {

    public:

      pdf ();

      pdf (const histogram & h_);

      virtual ~pdf ();

      bool is_initialized () const;

      void initialize (const histogram & h_);

      void init (const histogram & h_);

      void reset ();

      double sample (double r_);

      void sample (double r_, double & x_);

      template<class _ran_functor_type>
      double sample (_ran_functor_type & ran_)
      {
        return  gsl_histogram_pdf_sample (_pdf_, ran_());
      }

    private:

      gsl_histogram_pdf * _pdf_;

    };

  };

  typedef histogram histogram_1d;

} // end of namespace mygsl

#endif // MYGSL_HISTOGRAM_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
