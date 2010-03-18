// -*- mode: c++ ; -*- 
/* from_file_vg.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-13
 * Last modified: 2010-02-13
 * 
 * License: 
 * 
 * Description: 
 *   Generate vertex from a file
 * 
 * History: 
 * 
 */

#ifndef __genvtx__from_file_vg_h
#define __genvtx__from_file_vg_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <list>

#include <boost/filesystem.hpp>

#include <datatools/utils/utils.h>
#include <geomtools/units.h>

#include <genvtx/i_vertex_generator.h>

namespace genvtx {

  using namespace std;

  class from_file_vg : public i_vertex_generator
  {
  public:
    static const double LENGTH_UNIT;

  private: 
    string   __filename;
    bool     __open;
    ifstream __source;
    geomtools::vector_3d __next;
    double   __length_unit; // length unit (default == 1); 

  protected:

    void _open_source ();

    void _close_source ();

    void _read_next ();

    bool _has_next ();

  public: 

    bool is_open () const;

    const string & get_filename () const;

    void set_filename (const string &);

    void set_length_unit (double lu_);

    double get_length_unit () const;
  
  // ctor/dtor:
  public: 

    from_file_vg ();

    from_file_vg (const string & filename_);

    virtual ~from_file_vg ();
 
    // pure virtual methods for vertex generator factory stuff:
    virtual string vg_id () const;

    virtual vg_creator_t vg_creator () const;

    static i_vertex_generator * create (const properties & configuration_, void * user_ = 0);
 
  protected:
  
    virtual void _shoot_vertex (mygsl::rng & random_, 
				geomtools::vector_3d & vertex_);
 
  private:

    static creator_registration<from_file_vg> __CR;

  };

  typedef from_file_vg from_file_vertex_generator;

} // end of namespace genvtx

#endif // __genvtx__from_file_vg_h

// end of from_file_vg.h
