// gsl::rng.h
/*
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef __gsl__rng_h
#define __gsl__rng_h 1

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <gsl/gsl_rng.h>

namespace mygsl {

  //using   namespace std;

  class rng
  {
  public:
      
    static bool g_debug;

    static const std::string DEFAULT_RNG_TYPE;

    struct initializer
    {
      std::map<std::string, const gsl_rng_type *> dict;
      initializer ();
      ~initializer ();
    };
      
    typedef std::vector<unsigned char> state_buffer_type;

  private:

    static initializer g_initializer_;

    gsl_rng * _r_;

  public:

    static void default_setup ();

    static void print_dict (std::ostream &);

  public:

    bool is_initialized () const;

    void initialize (const std::string & id_ = "mt19937", 
                     unsigned long int seed_ = 0);

    void init (const std::string & id_ = "mt19937", 
               unsigned long int seed_ = 0);

    void set_seed (unsigned long int seed_ = 0);

    rng ();
            
    rng (const std::string & id_, unsigned long int seed_);

    void reset ();
      
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

    // specific distributions:
 
    double flat (double a_, double b_) const;
      
    double gaussian (double sigma_ = 1.0) const;
      
    double gaussian (double mu_, double sigma_) const;

    double gaussian_tail (double min_, double sigma_ = 1.0) const;

    double exponential (double sigma_ = 1.0) const;

    double chisquare (double nu_ = 1.0) const;

    unsigned long int poisson (double mu_) const;

    unsigned long int bernoulli (double p_ = 0.5) const;

    unsigned long int binomial (double p_, 
                                unsigned long int n_) const;

    // 2009-11-08 FM: to be used as a functor:
    double operator () (void);

  };
  
}

#endif // __gsl__rng_h

// end of gsl::rng.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
