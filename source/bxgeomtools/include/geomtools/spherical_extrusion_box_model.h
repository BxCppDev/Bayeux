// -*- mode: c++ ; -*- 
/* spherical_extrusion_box_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-04
 * Last modified: 2012-04-04
 * 
 * License: 
 * 
 * Description: 
 *
 *   A box extruded by a spherical volumes on top or bottom face.
 * 
 * History: 
 * 
 */

#ifndef __geomtools__spherical_extrusion_box_model_h
#define __geomtools__spherical_extrusion_box_model_h 1

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <geomtools/i_model.h>
#include <geomtools/box.h>
#include <geomtools/sphere.h>
#include <geomtools/subtraction_3d.h>

#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/logical_volume.h>
#include <geomtools/gnuplot_draw.h>

namespace geomtools {
 
  GEOMTOOLS_MODEL_CLASS_DECLARE(spherical_extrusion_box_model) 
  {
  private: 

    std::string               _material_;
    geomtools::sphere         _extrusion_;
    geomtools::box            _mother_;
    geomtools::subtraction_3d _solid_;
    double                    _x_;
    double                    _y_;
    double                    _z_;
    double                    _r_extrusion_;
    double                    _r_sphere_;
    bool                      _bottom_;

  public: 

    const std::string & get_material () const;

    const geomtools::subtraction_3d & get_solid () const;

    virtual std::string get_model_id () const;

  public: 
    // ctor:
    spherical_extrusion_box_model ();

    // dtor:
    virtual ~spherical_extrusion_box_model ();
      
  public: 
    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "", 
                            const std::string & indent_ = "", 
                            bool inherit_          = false) const;
  protected:

    virtual void _at_construct (const std::string & name_,
                                const datatools::utils::properties & setup_,
                                geomtools::models_col_t * models_ = 0);
  private:

    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE (spherical_extrusion_box_model);
     
  public: 

    static void gnuplot_draw_user_function (std::ostream &, 
                                            const geomtools::vector_3d &, 
                                            const geomtools::rotation_3d &,
                                            const geomtools::i_object_3d &, 
                                            void * = 0);

  };

} // end of namespace geomtools

#endif // __geomtools__spherical_extrusion_box_model_h

// end of spherical_extrusion_box_model.h
