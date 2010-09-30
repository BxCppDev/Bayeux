// -*- mode: c++; -*- 
/* genbb.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-28
 * Last modified: 
 * 
 * License: 
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

  class genbb
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

    virtual void load_next (primary_event & event_, 
			    bool compute_classification_ = true);

  private:

    void __init ();

    void __clean ();

  };

} // end of namespace genbb

#endif // __genbb_help__genbb_h

// end of genbb.h
