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

    static double default_time_unit();

    bool is_open() const;

    const std::string & get_filename() const;

    void set_filename(const std::string &);

    void set_length_unit(double lu_);

    double get_length_unit() const;

    void set_time_unit(double tu_);

    double get_time_unit() const;

    /// Constructor
    from_file_vg();

    /// Destructor
    ~from_file_vg() override;

    /// Initialization
    void initialize(const ::datatools::properties &,
                             ::datatools::service_manager &,
                             ::genvtx::vg_dict_type &) override;

    /// Reset
    void reset() override;

    /// Check initialization status
    bool is_initialized() const override;

    /// Check if at least one more vertex is available
    bool has_next_vertex() const override;

  protected :

    /// Load vertex from the source
    void _shoot_vertex(::mygsl::rng & random_, ::geomtools::vector_3d & vertex_) override;

    /// Load vertex/time from the source
    void _shoot_vertex_and_time(::mygsl::rng & random_, ::geomtools::vector_3d & vertex_, double & time_) override;

    /// Open the source file
    void _open_source();

    /// Close the source file
    void _close_source();

    /// Read the next vertex[/time] entry
    void _read_next();

    /// Check for a next vertex[/time] entry
    bool _has_next();

  private:

    // Configuration:
    std::string          _filename_;    //!< Filename of the input source file
    double               _length_unit_; // default length unit (default == 1);
    double               _time_unit_;   // default time unit (default == 1);

    // Working:
    bool                 _open_;      //!< Open source flag
    std::ifstream        _source_;    //!< Input source file stream
    geomtools::vector_3d _next_;      //!< Next vertex to be delivered
    double               _next_time_; //!< Next decay time to be delivered

    /// Registration macro
    /// @arg from_file_vg the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(from_file_vg)

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
