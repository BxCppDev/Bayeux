// -*- mode: c++ ; -*- 
/* plate_with_hole_model.h
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

#ifndef __geomtools__plate_with_hole_model_h
#define __geomtools__plate_with_hole_model_h 1

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <geomtools/i_model.h>
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/subtraction_3d.h>

#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/logical_volume.h>
#include <geomtools/gnuplot_draw.h>

namespace geomtools {
 
  GEOMTOOLS_MODEL_CLASS_DECLARE(plate_with_hole_model)
  {
  private: 

    std::string               _material_;
    geomtools::box            _mother_;
    geomtools::box            _box_hole_;
    geomtools::cylinder       _cyl_hole_;
    geomtools::subtraction_3d _solid_;
    double                    _x_; /// X box
    double                    _y_; /// Y box
    double                    _z_; /// Z box
    
    double                    _r_hole_;
    double                    _x_hole_;
    double                    _y_hole_;
    double                    _z_hole_;
    double                    _x_pos_hole_;
    double                    _y_pos_hole_;

  public: 

    const std::string & get_material () const;

    const geomtools::subtraction_3d & get_solid () const;

    virtual std::string get_model_id () const;

  public: 
    // ctor:
    plate_with_hole_model ();

    // dtor:
    virtual ~plate_with_hole_model ();
      
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

    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE (plate_with_hole_model);
     
  public: 

    static void gnuplot_draw_user_function (std::ostream &, 
                                            const geomtools::vector_3d &, 
                                            const geomtools::rotation_3d &,
                                            const geomtools::i_object_3d &, 
                                            void * = 0);

  };

} // end of namespace geomtools

#endif // __geomtools__plate_with_hole_model_h

// end of plate_with_hole_model.h
