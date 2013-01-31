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

#ifndef GEOMTOOLS_MODEL_WITH_INTERNAL_ITEMS_TOOLS_H_
#define GEOMTOOLS_MODEL_WITH_INTERNAL_ITEMS_TOOLS_H_ 1

#include <string>
#include <map>

#include <geomtools/i_model.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <datatools/properties.h>

namespace geomtools {

  class model_with_internal_items_tools
  {
  public:

    struct item_type
    {
    public:
      std::string        _label_;
      const i_model *    _model_;
      placement          _placement_;
      physical_volume    _phys_;

    public:
      item_type ();
      void set_label (const std::string &);
      const std::string & get_label () const;
      void set_model (const i_model &);
      const i_model & get_model () const;
      void set_placement (const placement &);
      const placement & get_placement () const;
      const physical_volume & get_physical_volume () const;
      physical_volume & get_physical_volume ();
    };

    typedef std::map<std::string, item_type> item_dict_type;

  public:

    model_with_internal_items_tools ();

    ~model_with_internal_items_tools ();

    const item_type & get_item (const std::string &) const;
    item_type & get_item (const std::string &);
    void add_item (const std::string & label_, 
                   const i_model & model_,
                   const placement & placement_);
    bool has_item (const std::string &) const;
    size_t get_number_of_items () const;
    const item_dict_type & get_items () const;
    item_dict_type & get_items ();

    void plug_internal_models (const datatools::properties & setup_,
                               logical_volume & log_,
                               models_col_type * models_);

  private:

    item_dict_type      _items_;

  };

  typedef model_with_internal_items_tools MWIM;

} // end of namespace geomtools

#endif // GEOMTOOLS_MODEL_WITH_INTERNAL_ITEMS_TOOLS_H_

// end of model_with_internal_items_tools.h
