/// \file genvtx/reusing_vg.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2023-05-29
 * Last modified: 2023-05-29
 *
 * Description:
 *
 *   Vertex generator with reused vertex.
 *
 */

#ifndef GENVTX_REUSING_VG_H
#define GENVTX_REUSING_VG_H 1

// This project:
#include <genvtx/i_vertex_generator.h>

namespace genvtx {

  /// \brief A vertex generator that reuses N times a vertex previously shot by another vertex generator
  class reusing_vg
    : public i_vertex_generator
  {
  public:

    std::size_t get_reusing_count() const;

    void set_reusing_count(const std::size_t);

    bool has_vg() const;

    bool is_owned() const;

    const i_vertex_generator & get_vg() const;

    i_vertex_generator & grab_vg();

    void set_vg(i_vertex_generator &);

    void set_vg(i_vertex_generator *);

    void set_vg(vg_handle_type &);

    /// Constructor
    reusing_vg();

    /// Constructor
    reusing_vg(i_vertex_generator & vg_, const std::size_t reusing_count_);

    /// Destructor
    ~reusing_vg() override;

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

    /// Set the remaining counter to zero
    void reset_remaining_counter();

  protected :

    /// Randomize vertex
    void _shoot_vertex(::mygsl::rng & random_, ::geomtools::vector_3d & vertex_) override;

    void _clear_vg_();

  private:

    std::size_t          _reusing_count_ = 0;
    bool                 _owned_ = false;
    i_vertex_generator * _vg_ = nullptr;
    vg_handle_type       _hvg_;

    // Internal:
    ::geomtools::vector_3d _cached_; ///< Cached vertex
    std::size_t _remaining_counter_ = 0; ///< Countdown for the cached vertex
   

  private:

    /// Registration macro
    /// @arg reusing_vg the class to be registered
    GENVTX_VG_REGISTRATION_INTERFACE(reusing_vg)
    
  };

} // end of namespace genvtx

#endif //GENVTX_REUSING_VG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
