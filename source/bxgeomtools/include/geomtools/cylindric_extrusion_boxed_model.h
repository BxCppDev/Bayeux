// -*- mode: c++ ; -*- 
/* cylindric_extrusion_boxed_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-02
 * Last modified: 2010-04-02
 * 
 * License: 
 * 
 * Description: 
 *
 *   Geometry model for a box with a central cylindric extrusion.
 * 
 * History: 
 * 
 */

#ifndef __geomtools__cylindric_extrusion_boxed_model_h
#define __geomtools__cylindric_extrusion_boxed_model_h 1

#include <iostream>
#include <string> 
#include <exception>
#include <limits> 

#include <mygsl/min_max.h>

#include <geomtools/i_boxed_model.h>
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/visibility.h>

namespace geomtools {

  using namespace std;

  // define a geometry model:
  class cylindric_extrusion_boxed_model : public geomtools::i_boxed_model
  {
  public:

    static const string LABEL_PROPERTY_PREFIX;
    static const string EXTRUSION_LABEL;

  private:

    string           __material_name;
    string           __mother_material_name;
    double           __mother_x, __mother_y, __mother_z;
    double           __extrusion_radius;

    geomtools::box            __mother_solid;
    geomtools::cylinder       __extrusion_solid;
    geomtools::subtraction_3d __extruded_solid;

    logical_volume            __extruded_log;
    placement                 __extruded_placement;
    physical_volume           __extruded_phys;

    geomtools::box            __solid;

    void set_material_name (const string &);

    void set_mother_material_name (const string &);

    const string & get_material_name () const;

    const string & get_mother_material_name () const;
  
    void set_mother_x (double);

    void set_mother_y (double);

    void set_mother_z (double);

    void set_extrusion_radius (double);

    double get_mother_x () const;
  
    double get_mother_y () const;
  
    double get_mother_z () const;

    double get_extrusion_radius () const;

    /*
    const physical_volume & get_extrusion_physical_volume () const
    {
      return __extrusion_phys;
    }
    */

    virtual const geomtools::box & get_box () const;

    const geomtools::box & get_solid () const;


  public:
  
    cylindric_extrusion_boxed_model ();
  
    virtual ~cylindric_extrusion_boxed_model ();

  public:

    virtual string get_model_id () const;

  protected:
  
    virtual void _at_construct (const string & name_,
				const datatools::utils::properties & config_,
				models_col_t * models_ = 0);

  private:

    static creator_registration<cylindric_extrusion_boxed_model> __CR;
      
  public: 
    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "", 
			    const string & indent_ = "", 
			    bool inherit_          = false) const;
  
  };

} // end of namespace geomtools

#endif // __geomtools__cylindric_extrusion_boxed_model_h

// end of cylindric_extrusion_boxed_model.h
