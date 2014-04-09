/// \file genvtx/from_file_vg.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-13
 * Last modified: 2014-04-09
 *
 * License:
 *
 * Description:
 *   Generate vertex from a file
 *
 * History:
 *
 */

#ifndef GENVTX_FROM_FILE_VG_H
#define GENVTX_FROM_FILE_VG_H 1

// Standard library:
#include <fstream>
#include <string>

// This project:
#include <genvtx/i_vertex_generator.h>

namespace genvtx {

  /// \brief Generate vertex from a file storing precomputed vertexes
  class from_file_vg : public i_vertex_generator
  {
  public:

    static double default_length_unit();

    bool is_open () const;

    const std::string & get_filename () const;

    void set_filename (const std::string &);

    void set_length_unit (double lu_);

    double get_length_unit () const;

    /// Constructor
    from_file_vg();

    /// Destructor
    virtual ~from_file_vg();

    /// Initialization
    virtual void initialize(const ::datatools::properties &,
                             ::datatools::service_manager &,
                             ::genvtx::vg_dict_type &);

    /// Reset
    virtual void reset();

    /// Check initialization status
    virtual bool is_initialized() const;

    /// Check if at least one more vertex is available
    virtual bool has_next_vertex() const;

  protected :

    /// Randomize vertex
    virtual void _shoot_vertex(::mygsl::rng & random_, ::geomtools::vector_3d & vertex_);

    void _open_source ();

    void _close_source ();

    void _read_next ();

    bool _has_next ();

  private:

    std::string          _filename_;
    bool                 _open_;
    std::ifstream        _source_;
    geomtools::vector_3d _next_;
    double               _length_unit_; // default length unit (default == 1);

    /// Registration macro
    /// @arg from_file_vg the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(from_file_vg);

  };

  typedef from_file_vg from_file_vertex_generator;

} // end of namespace genvtx

#endif // GENVTX_FROM_FILE_VG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
