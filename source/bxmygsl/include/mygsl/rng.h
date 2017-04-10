// \file mygsl/rng.h
/*
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef MYGSL_RNG_H
#define MYGSL_RNG_H 1

// Standard library:
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <memory>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// #include <boost/scoped_ptr.hpp>
// - GSL:
#include <gsl/gsl_rng.h>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>

// This project:
#include <mygsl/random_utils.h>

namespace datatools {
  class properties;
}

namespace mygsl {

  /// \brief Pseudo random number generator
  class rng
  {
  public:

    static const std::string DEFAULT_RNG_ID;   // "taus2"
    static const std::string DEFAULT_RNG_TYPE; // idem

    typedef std::vector<unsigned char> state_buffer_type;

    static void default_setup();

    static void print_dict(std::ostream &);

    static bool is_id_valid(const std::string & id_);

    static bool is_seed_valid(int32_t seed_);

    /// Default constructor
    rng();

    /// Constructor from a seed
    rng(int32_t seed_, bool init_now_ = true);

    /// Constructor from a GSL PRNG id and a seed
    rng(const std::string & id_, int32_t seed_, bool init_now_ = true);

    rng(const rng &); // not implemented

    /// Destructor
    virtual ~rng();

    rng & operator=(const rng &); // not implemented

    bool is_initialized() const;

    void initialize();

    void initialize(int32_t seed_);

    void initialize(const std::string & id_, int32_t seed_ = 0);

    void initialize(const datatools::properties & config_);

    /// \deprecated
    void init(const std::string & id_, int32_t seed_ = 0);

    /// Reset
    void reset();

    void clear();

    bool is_seed_invalid() const;

    bool is_seed_valid() const;

    bool is_seed_time() const;

    bool is_id_valid() const;

    int32_t get_seed() const;

    void set_seed(int32_t seed_ = 0);

    const std::string & get_id() const;

    void set_id(const std::string & id_ = "");

    void set_trunc(int);

    bool has_tracker() const;

    void set_tracker(const std::string & filename_);

    void reset_tracker();

    template<class Type>
    void tracker_tag(const std::string & tag_) {
      DT_THROW_IF(_tracker_, std::logic_error, "No tracker is defined !");
      *_tracker_.get() << '#' << ' ' << tag_ << std::endl;
    }

    template<class Type>
    void tracker_tag(const std::string & tag_, const Type & value_) {
      DT_THROW_IF(_tracker_, std::logic_error, "No tracker is defined !");
      *_tracker_.get() << '#' << ' ' << tag_ << " = " << value_ << std::endl;
    }

    /// Raw print
    void dump(std::ostream & = std::clog) const;

    unsigned long int get();

    /// Shoot a real between 0 and 1
    double uniform();

    /// Shoot a real between 0 (excluded) and 1
    double uniform_pos();

    unsigned long int uniform_int(unsigned long int n_);

    /// Return the name/ID of the embedded GSL PRNG
    std::string name() const;

    unsigned long int min() const;

    unsigned long int max() const;

    void store(const std::string & filename_) const;

    void load(const std::string & filename_);

    void to_stream(std::ostream &) const;

    void from_stream(std::istream &);

    void to_buffer(state_buffer_type &) const;

    void from_buffer(const state_buffer_type &);

    size_t get_internal_state_size() const;

    // specific useful distributions:

    double flat(double a_, double b_);

    double gaussian(double sigma_ = 1.0);

    double gaussian(double mu_, double sigma_);

    double gaussian_tail(double min_, double sigma_ = 1.0);

    double exponential(double sigma_ = 1.0);

    double chisquare(double nu_ = 1.0);

    unsigned long int poisson(double mu_);

    unsigned long int bernoulli(double p_ = 0.5);

    unsigned long int binomial(double p_,
                               unsigned long int n_);

    // 2009-11-08 FM: to be used as a functor:
    double operator()(void);

    /// Return the logging priority
    datatools::logger::priority get_logging_priority() const;

    /// Set the logging priority
    void set_logging_priority(datatools::logger::priority);

  private:

    /// Set default values
    void _init_defaults_();

    /// Initialize
    void _initialize_();

    /// Reset
    void _reset_();

  private:

    datatools::logger::priority _logging_priority_; ///< Logging priority
    std::string  _id_;   ///< The ID(type) of GSL random number generator algorithm
    int32_t      _seed_; ///< The initial seed set before initialization
    gsl_rng *    _r_;    ///< The internal GSL random number generator
    int          _trunc_;                       ///< Precision truncation index (debug)
    unsigned long int _trunc_norm_;             ///< Precision truncation denominator (debug)
    std::unique_ptr<std::ofstream> _tracker_;   ///< Embedded tracker (debug)
    int _tracker_counter_;                      ///< Embedded counter (debug)

  };

}

#endif // MYGSL_RNG_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
