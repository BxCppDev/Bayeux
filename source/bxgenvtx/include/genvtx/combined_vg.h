/// \file genvtx/combined_vg.h
/* Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-04-22
 * Last modified: 2014-04-09
 *
 * License:
 *
 * Description:
 *
 *   Vertex generator that combines several weighted vertex generators
 *
 * History:
 *
 */

#ifndef GENVTX_COMBINED_VG_H
#define GENVTX_COMBINED_VG_H 1

// Standard library:
#include <vector>

// Third party:
// - Bayeux/datatools
#include <datatools/logger.h>

// This project:
#include <genvtx/vg_macros.h>
#include <genvtx/utils.h>

namespace genvtx {

  /// \brief A vertex generator that combined several vertex generators wuth specific weighting rules
  class combined_vg : public i_vertex_generator
  {

  public:

    /// \brief Entry for a vertex generator to be combined
    struct entry_type
    {
      std::string name; //!< Name of the generator
      double weight; //!< Weight of the generator (branching ratio)
      double cumulated_weight; //!< Cumulated weight of the generator
      genvtx::vg_handle_type vg_handle; //!< Handle to the generator
      /// Default constructor
      entry_type ();
    };

    /// Add a generator to be combined
    void add_generator (genvtx::vg_handle_type & a_vg,
                        const std::string & a_name,
                        double a_weight = 1.0);

    /// Smart print
    void tree_dump (std::ostream & out_ = std::clog,
                            const std::string & title_ = "",
                            const std::string & indent_ = "",
                            bool inherit_ = false) const override;

    /// Constructor
    combined_vg();

    /// Destructor
    ~combined_vg() override;

    /// Initialization
    void initialize(const ::datatools::properties &,
                             ::datatools::service_manager &,
                             ::genvtx::vg_dict_type &) override;

    /// Reset
    void reset() override;

    /// Check initialization status
    bool is_initialized() const override;

  protected:

    /// Randomize vertex
    void _shoot_vertex(::mygsl::rng & random_,
                               ::geomtools::vector_3d & vertex_) override;

    /// Main algorithm
    void _shoot_vertex_combined (mygsl::rng & random_,
                                 geomtools::vector_3d & vertex_);

  private:

    /// Private initialization
    void _init_ ();

    /// Private reset
    void _reset_ ();

    /// Set default attributes' values
    void _set_defaults_ ();

  private:

    bool _initialized_; //!< Initialization flag
    std::vector<entry_type> _entries_; //!< Array of combined vertex generators entries

    /// Registration macro
    /// @arg combined_vg the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(combined_vg)

  };

} // end of namespace genvtx

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genvtx::combined_vg)

#endif // GENVTX_COMBINED_VG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
