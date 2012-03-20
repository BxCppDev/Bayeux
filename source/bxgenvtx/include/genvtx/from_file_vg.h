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

#include <fstream>
#include <string>

#include <genvtx/i_vertex_generator.h>

namespace genvtx {

  class from_file_vg : public i_vertex_generator
  {
  public:
    static const double LENGTH_UNIT;

  private: 
    std::string   _filename_;
    bool     _open_;
    std::ifstream _source_;
    geomtools::vector_3d _next_;
    double   _length_unit_; // length unit (default == 1); 

  protected:

    void _open_source ();

    void _close_source ();

    void _read_next ();

    bool _has_next ();

  public: 

    virtual bool has_next_vertex () const;

    bool is_open () const;

    const std::string & get_filename () const;

    void set_filename (const std::string &);

    void set_length_unit (double lu_);

    double get_length_unit () const;
  
  public: 

    // ctor:
    from_file_vg ();

    // ctor:
    from_file_vg (const std::string & filename_);

    // dtor:
    virtual ~from_file_vg ();
 
    // pure virtual methods for vertex generator factory stuff:
    virtual std::string vg_id () const;

    virtual vg_creator_type vg_creator () const;

    static i_vertex_generator * create (const datatools::utils::properties & configuration_, void * user_ = 0);
 
  protected:
  
    virtual void _shoot_vertex (mygsl::rng & random_, 
                                geomtools::vector_3d & vertex_);
 
  private:

    static creator_registration<from_file_vg> g_cr_;

  };

  typedef from_file_vg from_file_vertex_generator;

} // end of namespace genvtx

#endif // __genvtx__from_file_vg_h

// end of from_file_vg.h
