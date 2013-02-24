// -*- mode: c++ ; -*- 
/* spot_vertex_generator.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-12
 * Last modified: 2013-02-24
 * 
 * License: 
 * 
 * Description: 
 *   Spot vertex generator
 * 
 * History: 
 * 
 */

#ifndef GENVTX_SPOT_VERTEX_GENERATOR_H_
#define GENVTX_SPOT_VERTEX_GENERATOR_H_ 1

#include <genvtx/i_vertex_generator.h>

namespace datatools {
  class properties;
}

namespace genvtx {

  GENVTX_VG_CLASS_DECLARE(spot_vertex_generator)
  {

  public: 

    const geomtools::vector_3d & get_spot () const;

    void set_spot (double x_, double y_, double z_);

    void set_spot (const geomtools::vector_3d &);
  
  public: 

    GENVTX_VG_INTERFACE_CTOR_DTOR(spot_vertex_generator);
 
  public: 

    // ctor:
    spot_vertex_generator (double x_, double y_, double z_);

    // ctor:
    spot_vertex_generator (const geomtools::vector_3d & spot_);

  private: 

    geomtools::vector_3d _spot_;
 
  private:

    GENVTX_VG_REGISTRATION_INTERFACE(spot_vertex_generator);

  };

} // end of namespace genvtx

#endif // GENVTX_SPOT_VERTEX_GENERATOR_H_

// end of spot_vertex_generator.h
