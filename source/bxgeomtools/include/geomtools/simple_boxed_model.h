// -*- mode: c++ ; -*- 
/* simple_boxed_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-24
 * Last modified: 2010-02-24
 * 
 * License: 
 * 
 * Description: 
 *   Simple boxed models
 * 
 * History: 
 * 
 */

#ifndef __geomtools__simple_boxed_model_h
#define __geomtools__simple_boxed_model_h 1

#include <iostream>
#include <string> 
#include <exception>
#include <limits> 

#include <datatools/utils/units.h>

#include <geomtools/i_boxed_model.h>
#include <geomtools/box.h>
#include <geomtools/visibility.h>
#include <geomtools/material.h>
#include <geomtools/sensitive.h>

namespace geomtools {

  using namespace std;

  // define a geometry model with a single box: 
  class simple_boxed_model : public i_boxed_model
    {
    private:

      string         __material_name;
      double         __x;
      double         __y;
      double         __z;
      geomtools::box __solid;

    public: 

      void set_x (double x_);
      void set_y (double y_);
      void set_z (double z_);
      double get_x () const;
      double get_y () const;
      double get_z () const;
      void set_material_name (const string &);
      const string & get_material_name () const;
      virtual const geomtools::box & get_box () const;
      const geomtools::box & get_solid () const;

    public:
  
      simple_boxed_model ();
  
      virtual ~simple_boxed_model ();

    public:

      virtual string get_model_id () const;

    protected:
  
      virtual void _at_construct (const string & name_,
				  const datatools::utils::properties & config_,
				  models_col_t * models_ = 0);

    private:

      static creator_registration<simple_boxed_model> __CR;
      
    public: 
      virtual void tree_dump (ostream & out_         = clog, 
                              const string & title_  = "", 
                              const string & indent_ = "", 
                              bool inherit_          = false) const;
  
    };

} // end of namespace geomtools

#endif // __geomtools__simple_boxed_model_h

// end of simple_boxed_model.h
