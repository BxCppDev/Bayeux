// -*- mode: c++; -*- 
/* genbb_mgr.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-01-19
 * Last modified: 2010-04-11
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
 *   Manager for GENBB input data
 * 
 * History: 
 * 
 */

#ifndef __genbb_help__genbb_mgr_h
#define __genbb_help__genbb_mgr_h 1

#include <string>
#include <list>
#include <iostream>
#include <fstream>

#include <genbb_help/i_genbb.h>
#include <genbb_help/primary_event.h>
#include <datatools/serialization/io_factory.h>
#include <datatools/utils/properties.h>

// Implementation of serialization method for the 'primary_event' class, 
// implies also <genbb_help/primary_particle.ipp> :
#include <genbb_help/primary_event.ipp>

namespace genbb {

  class genbb_mgr : public i_genbb
  {
  public:

    enum format_type
      {
        FORMAT_GENBB = 0,
        FORMAT_BOOST = 1
      };

    static const std::string FORMAT_GENBB_LABEL;
    static const std::string FORMAT_BOOST_LABEL;

  public: 

    bool is_initialized () const;

    bool is_debug () const;

    void set_debug (bool d_);

    int get_format () const;

    void set_format (int format_ = FORMAT_GENBB);

    void set_format (const std::string & format_ = FORMAT_GENBB_LABEL);

    bool is_format_genbb () const;

    bool is_format_boost () const;

    // ctor:
    genbb_mgr (int format_ = FORMAT_GENBB);
    
    // dtor:
    virtual ~genbb_mgr ();
  
    void set (const std::string & filename_);

    void init ();

    void initialize ();

    void initialize (const datatools::utils::properties & config_);

    void reset ();

    virtual bool has_next ();

  protected:

    // from 'i_genbb' interface:
    virtual void _load_next (primary_event & event_, 
                             bool compute_classification_ = true);

  public:

    void dump (ostream & out_ = clog) const;

  private:

    void _at_init_ ();

    void _at_reset_ ();

    void _load_next_ ();

    void _load_next_genbb_ ();

    void _load_next_boost_ ();

  private:

    bool _debug_;
    bool _initialized_;
    std::list<std::string> _filenames_;
    std::string      _current_filename_;
    int         _format_;
    std::istream * _in_;
    std::ifstream  _fin_;
    datatools::serialization::data_reader _reader_;
    primary_event  _current_;

  };

} // end of namespace genbb

#endif // __genbb_help__genbb_mgr_h

// end of genbb_mgr.h
