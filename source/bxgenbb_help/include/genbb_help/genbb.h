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
 *   GENBB/Decay0 generator
 * 
 * History: 
 * 
 */

#ifndef __genbb_help__genbb_h
#define __genbb_help__genbb_h 1

#include <iostream>
#include <fstream>
#include <string>

#include <genbb_help/i_genbb.h>
#include <mygsl/rng.h>

#include <datatools/utils/properties.h>

namespace genbb {

  using namespace std;

  class genbb : public i_genbb
  {
  public:
    static const size_t MAX_BUFFER_SIZE     = 100000;
    static const size_t DEFAULT_BUFFER_SIZE = 10000;

    enum decay_type_t
    {
      DECAY_TYPE_DBD = 1,
      DECAY_TYPE_BACKGROUND = 2
    };

    static const size_t TMP_DIR_BUFSZ = 1024;

  public:

    bool is_debug () const;
 
    void set_debug (bool d_);

    bool is_initialized () const;

    void set_delete_conf_file (bool);

    void set_delete_log_files (bool);

    void set_delete_data_files (bool);

    void set_delete_tmp_dir (bool);

    void set_tmp_dir (const string &);

    void set_tmp_base_dir (const string &);

    const mygsl::rng & get_random () const;

    mygsl::rng & get_random ();

    const string & get_tmp_base_dir () const;

    string get_tmp_dir () const;

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

    void _init_ ();  //> Initialize material associated to a new buffer file

    void _clean_ (); //> Clean material associated to the current buffer file
 
  private:
    bool   _initialized_;
    bool   _debug_;
    size_t _buffer_size_;
    size_t _buffer_item_;
    size_t _event_count_;
    size_t _buffer_count_;
    bool   _delete_conf_file_;
    bool   _delete_log_files_;
    bool   _delete_data_files_;
    bool   _delete_tmp_dir_;

    int    _decay_type_;
    string _decay_isotope_;  
    int    _decay_dbd_level_;  
    int    _decay_dbd_mode_;  

    ifstream * _genbb_in_;
    bool       _test_;
    string     _tmp_base_dir_;
    string     _forced_tmp_dir_;
    char       _tmp_dir_[TMP_DIR_BUFSZ];
    string     _genbb_conf_;
    string     _genbb_data_;
    string     _genbb_log_;
    ofstream   _genbb_conf_file_;
    unsigned long _seed_;
    mygsl::rng    _random_;

  };

} // end of namespace genbb

#endif // __genbb_help__genbb_h

// end of genbb.h
