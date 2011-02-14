// -*- mode: c++; -*- 
/* genbb.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-28
 * Last modified: 
 * 
 * License: 
 * Copyright 2007-2011 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Publi * License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Publi * License for more details.
 * 
 * You should have received a copy of the GNU General Publi * License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * 
 * Description: 
 *   GENBB generator
 * 
 * History: 
 * 
 */

#ifndef __genbb_help__genbb_h
#define __genbb_help__genbb_h 1

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>

#include <genbb_help/i_genbb.h>

#include <mygsl/rng.h>

#include <datatools/utils/properties.h>
#include <datatools/utils/utils.h>
#include <boost/filesystem.hpp>

namespace genbb {

  using namespace std;

  class genbb : public i_genbb
  {
  public:
    static const size_t MAX_BUFFER_SIZE = 100000;
    static const size_t DEFAULT_BUFFER_SIZE = 10000;

    enum decay_type_t
    {
      DECAY_TYPE_DBD = 1,
      DECAY_TYPE_BACKGROUND = 2
    };

  private:
    bool   __debug;
    size_t __buffer_size;
    size_t __buffer_item;
    size_t __event_count;
    string __fin;

    int    __decay_type;
    string __decay_isotope;  
    int    __decay_dbd_level;  
    int    __decay_dbd_mode;  

    ifstream * __genbb_in;
    bool       __test;
    string     __tmp_base_dir;
    char       __tmp_dir[256];
    string     __genbb_conf;
    string     __genbb_data;
    ofstream   __genbb_conf_file;
    unsigned long __seed;
    mygsl::rng    __random;

  public:

    bool is_debug () const;

    size_t get_buffer_size () const;

    size_t get_event_count () const;

    genbb ();

    virtual ~genbb ();

    void initialize (const datatools::utils::properties & config_);

    void reset ();

    virtual bool has_next ();

  protected:

    virtual void _load_next (primary_event & event_, 
			    bool compute_classification_ = true);

  private:

    void __init ();

    void __clean ();

  };

} // end of namespace genbb

#endif // __genbb_help__genbb_h

// end of genbb.h
