// -*- mode: c++ ; -*- 
/* model_with_internal_items_tools.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-16
 * Last modified: 2010-10-16
 * 
 * License: 
 * 
 * Description: 
 *   Some tools to make a model owning internal items
 *   within its mother shape
 * 
 * History: 
 * 
 */

#ifndef __geomtools__model_with_internal_items_tools_h
#define __geomtools__model_with_internal_items_tools_h 1

#include <string>
#include <map>

#include <geomtools/i_model.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <datatools/utils/properties.h>

namespace geomtools {
    
  using namespace std;  

  class model_with_internal_items_tools
  {
  public:

    struct item
    {
    public:
      string             _label_;
      const i_model *    _model_;
      placement          _placement_;
      physical_volume    _phys_;

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

  public:

    model_with_internal_items_tools ();

    ~model_with_internal_items_tools ();

    const item & get_item (const string &) const;
    item & get_item (const string &);
    void add_item (const string & label_, 
                   const i_model & model_,
                   const placement & placement_);
    bool has_item (const string &) const;
    size_t get_number_of_items () const;
    const item_dict_t & get_items () const;
    item_dict_t & get_items ();

    void plug_internal_models (const datatools::utils::properties & setup_,
                               logical_volume & log_,
                               models_col_t * models_);

  private:

    item_dict_t      _items_;

  };

  typedef model_with_internal_items_tools MWIM;

} // end of namespace geomtools

#endif // __geomtools__model_with_internal_items_tools_h

// end of model_with_internal_items_tools.h
