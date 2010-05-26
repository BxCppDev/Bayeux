// -*- mode: c++ ; -*- 
/* rotated_boxed_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-24
 * Last modified: 2010-02-24
 * 
 * License: 
 * 
 * Description: 
 *   Factory for geometry models
 * 
 * History: 
 * 
 */

#ifndef __geomtools__rotated_boxed_model_h
#define __geomtools__rotated_boxed_model_h 1

#include <iostream>
#include <string> 
#include <exception>
#include <limits> 

#include <geomtools/i_boxed_model.h>
#include <geomtools/box.h>
#include <geomtools/physical_volume.h>
#include <geomtools/regular_linear_placement.h>
#include <geomtools/visibility.h>
#include <geomtools/placement.h>

namespace geomtools {

  using namespace std;

  // define a geometry model with a boxed model rotated 
  // by some simple rotation:
  class rotated_boxed_model : public geomtools::i_boxed_model
  {
  private:
    
      const i_model *  __boxed_model;
      placement        __boxed_placement;
      physical_volume  __boxed_phys;
      geomtools::box   __solid;

    public: 
      void set_boxed_model (const i_model &);
      const i_model & get_boxed_model () const;
      virtual const geomtools::box & get_box () const;
      const geomtools::box & get_solid () const;

    public:
  
      rotated_boxed_model ();
  
      virtual ~rotated_boxed_model ();

    public:

      virtual string get_model_id () const;

    protected:
  
      virtual void _at_construct (const string & name_,
				  const datatools::utils::properties & config_,
				  models_col_t * models_ = 0);

    private:

      static creator_registration<rotated_boxed_model> __CR;
      
    public: 
      virtual void tree_dump (ostream & out_         = clog, 
                              const string & title_  = "", 
                              const string & indent_ = "", 
                              bool inherit_          = false) const;
  
    };

} // end of namespace geomtools

#endif // __geomtools__rotated_boxed_model_h

// end of rotated_boxed_model.h
