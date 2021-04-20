/// \file geomtools/model_with_internal_items_tools.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-16
 * Last modified: 2014-05-17
 *
 * License:
 *
 * Description:
 *
 *   Some tools to make a model owning internal items
 *   within its mother shape
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_MODEL_WITH_INTERNAL_ITEMS_TOOLS_H
#define GEOMTOOLS_MODEL_WITH_INTERNAL_ITEMS_TOOLS_H 1

// Standard library:
#include <string>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/object_configuration_description.h>

// This project:
#include <geomtools/i_model.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <datatools/properties.h>

namespace geomtools {

  /// \brief Handler for internal items
  class model_with_internal_items_tools
  {
  public:

    static const std::string INTERNAL_ITEM_PREFIX;           ///< value = "internal_item."
    static const std::string INTERNAL_ITEM_LABELS_KEY;       ///< value = "internal_item.labels"
    static const std::string INTERNAL_ITEM_MODEL_PREFIX;     ///< value = "internal_item.model."
    static const std::string INTERNAL_ITEM_PLACEMENT_PREFIX; ///< value = "internal_item.placement."

    /// \brief Internal item record
    struct item_type
    {

    public:
      item_type ();
      void set_label (const std::string &);
      const std::string & get_label () const;
      void set_model (const i_model &);
      const i_model & get_model () const;
      void set_placement (const placement &);
      const placement & get_placement () const;
      const physical_volume & get_physical_volume () const;
      physical_volume & grab_physical_volume ();

    public:
      std::string      _label_;     ///< Daughter label
      const i_model *  _model_;     ///< Model handler
      placement        _placement_; ///< Daughter placement
      physical_volume  _phys_;      ///< Physical volume handler

    };

    typedef std::map<std::string, item_type> item_dict_type;

  public:

    model_with_internal_items_tools ();

    ~model_with_internal_items_tools ();

    const item_type & get_item (const std::string &) const;

    item_type & grab_item (const std::string &);

    void add_item (const std::string & label_,
                   const i_model & model_,
                   const placement & placement_);

    bool has_item (const std::string &) const;

    size_t get_number_of_items () const;

    const item_dict_type & get_items () const;

    item_dict_type & grab_items ();

    void plug_internal_models (const datatools::properties & setup_,
                               logical_volume & log_,
                               models_col_type * models_);

    void init_internal_items (logical_volume & log_,
                              models_col_type * models_);


    static void init_ocd(datatools::object_configuration_description & ocd_,
                         const std::string & prefix_ = "");

  private:

    item_dict_type _items_; //!< Collection of internal items

  };

  // Type alias:
  typedef model_with_internal_items_tools MWIM;

} // end of namespace geomtools

#endif // GEOMTOOLS_MODEL_WITH_INTERNAL_ITEMS_TOOLS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
