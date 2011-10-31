// -*- mode: c++; -*- 
/* genbb_writer.h
 * Author(s):     Arnaud Chapon <chapon@lpccaen.in2p3.fr>
 * Creation date: 2010-01-20
 * Last modified: 2010-01-20
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * 
 * Description: 
 *   Create GENBB input data
 * 
 * History: 
 * 
 */

#ifndef __genbb_help__genbb_writer_h
#define __genbb_help__genbb_writer_h 1

#include <string>
#include <iostream>

#include <geomtools/utils.h>

#include <genbb_help/genbb_mgr.h>

namespace genbb {

  class genbb_writer
  {

  private:

    bool        _debug_;
    std::string _fout_;

    int    _npart_, _part_per_evt_;
    int    _type_, _seed_;
    double _emin_, _emax_;
    double _phi_min_, _phi_max_;

  public:

    void run();

    void set_seed ( int seed_ )
    { _seed_ = seed_; }
    int get_seed() const { return _seed_; }

    void set_npart ( int npart_ )
    { _npart_ = npart_; }
    int get_npart() const { return _npart_; }

    void set_part_per_evt ( int part_per_evt_ )
    { _part_per_evt_ = part_per_evt_; }
    int get_part_per_evt() const { return _part_per_evt_; }

    void set_type ( int type_ )
    { _type_ = type_; }
    int get_type() const { return _type_; }

    void set_Emin ( double Emin_ )
    { _emin_ = Emin_; }
    double get_Emin() const { return _emin_; }

    void set_Emax ( double Emax_ )
    { _emax_ = Emax_; }
    double get_Emax() const { return _emax_; }

    void set_phi_min ( double phi_min_ )
    { _phi_min_ = phi_min_; }
    double get_phi_min() const { return _phi_min_; }

    void set_phi_max ( double phi_max_ )
    { _phi_max_ = phi_max_; }
    double get_phi_max() const { return _phi_max_; }

    void set_outfile ( std::string fout_ ) { _fout_ = fout_; }
    std::string get_outfile() const { return _fout_; }

    void set_debug ( bool d_ )
    {_debug_ = d_; }

    bool is_debug () const { return _debug_; }

    void dump ( std::ostream & out_ ) const;

    // ctor/dtor:
    genbb_writer ();
    
    virtual ~genbb_writer ();
    

  private:

    double myrandom ( double min_, double max_ );

    void init ();

  };

} // end of namespace genbb

#endif // __genbb_help__genbb_writer_h

// end of genbb_writer.h
