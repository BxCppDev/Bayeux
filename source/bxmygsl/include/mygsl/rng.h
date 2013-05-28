// rng.h
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

#ifndef MYGSL_RNG_H_
#define MYGSL_RNG_H_ 1

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/scoped_ptr.hpp>

#include <gsl/gsl_rng.h>
#include <mygsl/random_utils.h>

namespace datatools {
  class properties;
}

namespace mygsl {

  class rng
  {
  public:

    static const std::string DEFAULT_RNG_ID;   // "taus2"
    static const std::string DEFAULT_RNG_TYPE; // idem

    typedef std::vector<unsigned char> state_buffer_type;

    static void default_setup ();

    static void print_dict (std::ostream &);

    static bool is_id_valid(const std::string & id_);

    static bool is_seed_valid(int32_t seed_);

    bool is_initialized () const;

    void initialize ();

    void initialize (int32_t seed_);

    void initialize (const std::string & id_, int32_t seed_ = 0);

    void initialize (const datatools::properties & config_);

    /// Deprecated :
    void init (const std::string & id_, int32_t seed_ = 0);

    bool is_seed_invalid() const;

    bool is_seed_valid() const;

    bool is_seed_time() const;

    bool is_id_valid() const;

    int32_t get_seed() const;

    void set_seed (int32_t seed_ = 0);

    const std::string & get_id() const;

    void set_id (const std::string & id_ = "");

    void set_trunc(int);

    bool has_tracker() const;

    void set_tracker (const std::string & filename_);

    void reset_tracker ();

    template<class Type>
    void tracker_tag(const std::string & tag_) {
      *_tracker_.get() << '#' << ' ' << tag_ << std::endl;
    }

    template<class Type>
    void tracker_tag(const std::string & tag_, const Type & value_) {
      *_tracker_.get() << '#' << ' ' << tag_ << " = " << value_ << std::endl;
    }

    rng ();

    rng (int32_t seed_, bool init_now_ = true);

    rng (const std::string & id_, int32_t seed_, bool init_now_ = true);

    void reset ();

    void clear ();

    virtual ~rng ();

    rng (const rng &); // not implemented

    void dump (std::ostream & = std::clog) const;

    rng & operator= (const rng &); // not implemented

    unsigned long int get ();

    double uniform ();

    double uniform_pos ();

    unsigned long int uniform_int (unsigned long int n_);

    std::string name () const;

    unsigned long int min () const;

    unsigned long int max () const;

    void store (const std::string & filename_) const;

    void load (const std::string & filename_);

    void to_stream (std::ostream &) const;

    void from_stream (std::istream &);

    void to_buffer (state_buffer_type &) const;

    void from_buffer (const state_buffer_type &);

    size_t get_internal_state_size () const;

    // specific useful distributions:

    double flat (double a_, double b_);

    double gaussian (double sigma_ = 1.0);

    double gaussian (double mu_, double sigma_);

    double gaussian_tail (double min_, double sigma_ = 1.0);

    double exponential (double sigma_ = 1.0);

    double chisquare (double nu_ = 1.0);

    unsigned long int poisson (double mu_);

    unsigned long int bernoulli (double p_ = 0.5);

    unsigned long int binomial (double p_,
                                unsigned long int n_);

    // 2009-11-08 FM: to be used as a functor:
    double operator () (void);

  private:

    void _init_defaults_();

    void _initialize_();

    void _reset_();

  private:

    std::string  _id_;   /// The ID(type) of GSL random number generator algorithm
    int32_t      _seed_; /// The initial seed set before initialization
    gsl_rng *    _r_;    /// The internal GSL random number generator
    int          _trunc_;                       /// Precision truncation index (debug)
    unsigned long int _trunc_norm_;             /// Precision truncation denominator (debug)
    boost::scoped_ptr<std::ofstream> _tracker_; /// Embeded tracker (debug)
    int _tracker_counter_;                      /// Embeded counter (debug)

  };

}

#endif // MYGSL_RNG_H_

// end of rng.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
