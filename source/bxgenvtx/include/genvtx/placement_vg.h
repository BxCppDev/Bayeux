/// \file genvtx/placement_vg.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-13
 * Last modified: 2023-05-29
 *
 * Description:
 *
 *   Vertex generator with placement.
 *
 */

#ifndef GENVTX_PLACEMENT_VG_H
#define GENVTX_PLACEMENT_VG_H 1

// This project:
#include <geomtools/placement.h>
#include <genvtx/i_vertex_generator.h>

namespace genvtx {

  /// \brief A vertex generator that changes the placement (translation/rotation) of another vertex generator
  class placement_vg : public i_vertex_generator
  {
  public:

    const geomtools::placement & get_placement () const;

    geomtools::placement & get_placement ();

    void set_placement (const geomtools::placement &);

    bool has_vg () const;

    bool is_owned () const;

    const i_vertex_generator & get_vg () const;

    i_vertex_generator & grab_vg ();

    void set_vg (i_vertex_generator &);

    void set_vg (i_vertex_generator *);

    void set_vg (vg_handle_type &);

    /// Constructor
    placement_vg();

    /// Constructor
    placement_vg(i_vertex_generator &,
                 const geomtools::placement &);

    /// Destructor
    ~placement_vg() override;

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

    /// Randomize vertex
    void _shoot_vertex(::mygsl::rng & random_, ::geomtools::vector_3d & vertex_) override;

    void _clear_vg_ ();

  private:

    geomtools::placement _placement_;
    bool                 _owned_ = false;
    i_vertex_generator * _vg_ = nullptr;
    vg_handle_type       _hvg_;

  private:

    /// Registration macro
    /// @arg placement_vg the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(placement_vg)

  };

} // end of namespace genvtx

#endif //GENVTX_PLACEMENT_VG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
