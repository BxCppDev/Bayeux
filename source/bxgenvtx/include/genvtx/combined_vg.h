// -*- mode: c++; -*-
/* combined_vg.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-04-22
 * Last modified: 2011-04-22
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

#ifndef GENVTX_COMBINED_VG_H_
#define GENVTX_COMBINED_VG_H_ 1

#include <vector>

#include <genvtx/vg_macros.h>
#include <genvtx/utils.h>

namespace genvtx {

  GENVTX_VG_CLASS_DECLARE(combined_vg)
  {

  public:

    struct entry_type
    {
      std::string name;
      double weight;
      double cumulated_weight;
      genvtx::vg_handle_type vg_handle;
      entry_type ();
    };

    void add_generator (genvtx::vg_handle_type & a_vg,
                        const std::string & a_name,
                        double a_weight = 1.0);

  public:

    virtual void tree_dump (std::ostream & out_ = std::clog, 
                            const std::string & title_ = "", 
                            const std::string & indent_ = "", 
                            bool inherit_ = false) const;
 
    GENVTX_VG_INTERFACE_CTOR_DTOR (combined_vg);

 
  private:

    void _init_ ();

    void _reset_ ();

    void _set_defaults_ ();

  protected:

    void _shoot_vertex_combined (mygsl::rng & random_,
                                 geomtools::vector_3d & vertex_);

  private:

    bool _initialized_;
    std::vector<entry_type> _entries_;

    GENVTX_VG_REGISTRATION_INTERFACE(combined_vg);

  };

} // end of namespace genvtx

#endif // GENVTX_COMBINED_VG_H_

// end of combined_vg.h
