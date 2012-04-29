// -*- mode: c++ ; -*- 
/* from_file_vg.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-13
 * Last modified: 2012-04-27
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

#include <fstream>
#include <string>

#include <genvtx/i_vertex_generator.h>

namespace genvtx {

  GENVTX_VG_CLASS_DECLARE(from_file_vg)
  {
  public:

    static const double LENGTH_UNIT;

  protected:

    void _open_source ();

    void _close_source ();

    void _read_next ();

    bool _has_next ();

  public: 

    bool is_open () const;

    const std::string & get_filename () const;

    void set_filename (const std::string &);

    void set_length_unit (double lu_);

    double get_length_unit () const;
  
  public: 

    GENVTX_VG_INTERFACE_CTOR_DTOR (from_file_vg)

    GENVTX_VG_HAS_NEXT_VERTEX_DECLARE();

    // Constructor :
    //from_file_vg (const std::string & filename_);

  private: 

    std::string          _filename_;
    bool                 _open_;
    std::ifstream        _source_;
    geomtools::vector_3d _next_;
    double               _length_unit_; // length unit (default == 1); 
 
  private:

    GENVTX_VG_REGISTRATION_INTERFACE(from_file_vg);

  };

  typedef from_file_vg from_file_vertex_generator;

} // end of namespace genvtx

#endif // __genvtx__from_file_vg_h

// end of from_file_vg.h
