// -*- mode: c++; -*- 
/* wgenbb.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-28
 * Last modified: 
 * 
 * License: 
 * Copyright 2007-2011 F. Mauger
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
 * Description: 
 *
 *   C++ wrapper GENBB/Decay0 generator
 * 
 * History: 
 * 
 */

#ifndef __genbb_help__wgenbb_h
#define __genbb_help__wgenbb_h 1

#include <iostream>

#include <genbb_help/i_genbb.h>
#include <genbb_help/genbb_commons.h>

#include <mygsl/rng.h>

#include <datatools/utils/properties.h>

#include <genbb_help/detail/__genbb_help_FC2.h>

extern "C" 
{ 
  void set_genbb_random_seed (int *); 

  void genbbparprint (void); 

  void genbbsub (int *, 
                 const char *,
                 int *,
                 int *,
                 int *,
                 int *); 

  void grndm (float *, int *); 

  void genbbinit (void); 

} 

typedef GENEVENT_t GENEVENT_DEF;
typedef ENRANGE_t ENRANGE_DEF;

namespace genbb {

  using namespace std;

  class wgenbb : public i_genbb
  {
  public:

    enum decay_type_t
    {
      DECAY_TYPE_UNDEFINED  = 0,
      DECAY_TYPE_DBD        = 1,
      DECAY_TYPE_BACKGROUND = 2
    };

  private:

    void _set_decay_isotope_ (const string & di_);

  public:

    bool is_debug () const;
 
    void set_debug (bool d_);

    bool use_local_prng () const;
 
    void set_use_local_prng (bool u_);

    const mygsl::rng & get_random () const;

    mygsl::rng & get_random ();

    size_t get_event_count () const;

    double get_to_all_events () const;

    wgenbb ();

    virtual ~wgenbb ();

    void initialize (const datatools::utils::properties & config_);

    void reset ();

    virtual bool has_next ();

  protected:

    virtual void _load_next (primary_event & event_, 
                             bool compute_classification_ = true);

  public:

    void dump (ostream & = clog) const;

  private:

    void _init_ ();

    void _clean_ ();

  private:

    static size_t _g_counter_;

  private:

    bool   _debug_;
    bool   _initialized_;

    int    _decay_type_;
    string _decay_isotope_;  
    char   _c_decay_isotope_[32];
    int    _decay_dbd_level_;  
    int    _decay_dbd_mode_;  
    size_t _event_count_;

    double _energy_min_; 
    double _energy_max_;

    bool          _use_local_prng_; //> Local PRNG flag
    unsigned long _seed_;           //> PRNG seed (local or global)
    mygsl::rng    _random_;         //> Local PRNG

  };

} // end of namespace genbb

#endif // __genbb_help__wgenbb_h

// end of wgenbb.h
