/// \file genvtx/spot_vertex_generator.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-12
 * Last modified: 2014-04-09
 *
 * License:
 *
 * Description:
 *   Spot vertex generator
 *
 * History:
 *
 */

#ifndef GENVTX_SPOT_VERTEX_GENERATOR_H
#define GENVTX_SPOT_VERTEX_GENERATOR_H 1

// This project:
#include <genvtx/i_vertex_generator.h>

namespace datatools {
  class properties;
}

namespace genvtx {

  /// \brief Vertex generator from a single point
  class spot_vertex_generator : public i_vertex_generator
  {

  public:

    const geomtools::vector_3d & get_spot () const;

    void set_spot (double x_, double y_, double z_);

    void set_spot (const geomtools::vector_3d &);

    /// Constructor
    spot_vertex_generator();

    /// Constructor
    spot_vertex_generator (double x_, double y_, double z_);

    /// Constructor
    spot_vertex_generator (const geomtools::vector_3d & spot_);

    /// Destructor
    ~spot_vertex_generator() override;

    /// Initialization
    void initialize(const ::datatools::properties &,
                             ::datatools::service_manager &,
                             ::genvtx::vg_dict_type &) override;

    /// Reset
    void reset() override;

    /// Check initialization status
    bool is_initialized() const override;

  protected :

    /// Randomize vertex
    void _shoot_vertex(::mygsl::rng & random_, ::geomtools::vector_3d & vertex_) override;

  private:

    geomtools::vector_3d _spot_; /// The position of the vertexes

    /// Registration macro
    /// @arg spot_vertex_generator the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(spot_vertex_generator)

  };

} // end of namespace genvtx

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genvtx::spot_vertex_generator)

#endif // GENVTX_SPOT_VERTEX_GENERATOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
