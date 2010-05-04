// -*- mode: c++ ; -*- 
/* multiple_items_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-20
 * Last modified: 2010-02-20
 * 
 * License: 
 * 
 * Description: 
 *   A model that hosts several objects (items) in a box mother shape
 * 
 * History: 
 * 
 */

#ifndef __geomtools__multiple_items_model_h
#define __geomtools__multiple_items_model_h 1

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <datatools/utils/units.h>

#include <geomtools/i_model.h>
#include <geomtools/box.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/utils.h>
#include <geomtools/visibility.h>
#include <geomtools/material.h>

namespace geomtools {
    
  using namespace std;  
 
  class multiple_items_model : public i_model
  {
  public: 

    struct item
    {
    public:
      string             __label;
      const i_model *    __model;
      placement          __placement;
      physical_volume    __phys;

    public:
      item ();
      void set_label (const string &);
      const string & get_label () const;
      void set_model (const i_model &);
      const i_model & get_model () const;
      void set_placement (const placement &);
      const placement & get_placement () const;
      const physical_volume & get_physical_volume () const;
      physical_volume & get_physical_volume ();
    };

    typedef map<string, item> item_dict_t;

  private:

    string           __material_name;
    item_dict_t      __items;
    box              __solid;

    const string & __get_material_name () const;

  public: 

    const box & get_solid () const;

    const string & get_material_name () const;
    void set_material_name (const string &);
    const item & get_item (const string &) const;
    item & get_item (const string &);
    void add_item (const string & label_, 
		   const i_model & model_,
		   const placement & placement_);
    bool has_item (const string &) const;
    size_t get_number_of_items () const;
    const item_dict_t & get_items () const;
    item_dict_t & get_items ();
     
    virtual string get_model_id () const;

  public: 
    // ctor:
    multiple_items_model ();

    // dtor:
    virtual ~multiple_items_model ();

  public: 
    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "", 
			    const string & indent_ = "", 
			    bool inherit_          = false) const;

  protected:

    virtual void _at_construct (const string & label_,
				const datatools::utils::properties & config_,
				models_col_t * models_ = 0);
  private:
    static i_model::creator_registration<multiple_items_model> __CR;

  };

} // end of namespace geomtools

#endif // __geomtools__multiple_items_model_h

// end of multiple_items_model.h
