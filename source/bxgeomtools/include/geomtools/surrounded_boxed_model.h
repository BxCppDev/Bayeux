/// \file geomtools/surrounded_boxed_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-02
 * Last modified: 2021-04-23
 *
 * Description:
 *
 *   Geometry model with multiple surrounded boxes.
 *
 */

#ifndef GEOMTOOLS_SURROUNDED_BOXED_MODEL_H
#define GEOMTOOLS_SURROUNDED_BOXED_MODEL_H 1

// Standard library:
#include <string>
#include <iostream>
#include <vector>
#include <map>

// This project:
#include <geomtools/i_boxed_model.h>
#include <geomtools/box.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/model_macros.h>
#include <geomtools/model_with_internal_items_tools.h>

namespace geomtools {

  /// \brief A geometry model with a boxed model surrounded by up to 6 other models
  class surrounded_boxed_model : public i_boxed_model
  {
  public:

    static const std::string LABEL_PROPERTY_PREFIX;
    static const std::string MODEL_PROPERTY_PREFIX;
    static const std::string SURROUNDED_LABEL;
    static const std::string SURROUNDING_LABEL;

    static const std::vector<std::string> & position_labels();

    struct surrounding_item {
    public:
      const std::string & get_label () const
      {
        return label;
      }
      const i_model & get_model () const
      {
        return *model;
      }
      const placement & get_placement () const
      {
        return placmt;
      }
      const physical_volume & get_physical_volume () const
      {
        return phys;
      }
    public:
      std::string      label;
      const i_model *  model;
      placement        placmt;
      physical_volume  phys;
    };

    typedef std::map<int, surrounding_item> surrounding_dict_type;
    typedef std::map<std::string, int> labels_dict_type;

    const std::string & get_material_name () const;

    void set_material_name (const std::string &);

    const geomtools::box & get_box () const override;

    const geomtools::box & get_solid () const;

    void set_surrounded_label (const std::string & label_);

    const std::string & get_surrounded_label () const;

    void set_surrounded_model (const i_model & model_);

    const i_model & get_surrounded_model () const;

    size_t get_number_of_surrounding_items () const;

    bool has_surrounding_model (const std::string & label_) const;

    void add_surrounding_model (int pos_, const i_model &, const std::string & label_ = "");

    const i_model & get_surrounding_model (int i_) const;

    const surrounding_item & get_surrounding_item (int i_) const;

    const i_model & get_surrounding_model (const std::string & label_) const;

    const surrounding_item & get_surrounding_item (const std::string & label_) const;

    const labels_dict_type & get_surrounding_labels () const;

    const surrounding_dict_type & get_surrounding_items () const;

    /// Default constructor
    surrounded_boxed_model ();

    /// Destructor
    ~surrounded_boxed_model () override;

    std::string get_model_id () const override;

    /// Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                    const std::string & title_  = "",
                    const std::string & indent_ = "",
                    bool inherit_          = false) const override;

  protected:

    /// Construction
    void _at_construct (const datatools::properties & config_,
                        models_col_type * models_) override;

  private:

    std::string              _material_name_;

    const i_model *     _surrounded_model_;
    std::string         _surrounded_label_;
    placement           _surrounded_placmt_;
    physical_volume     _surrounded_phys_;

    surrounding_dict_type  _surrounding_items_;
    labels_dict_type       _surrounding_labels_;
    bool                _centered_x_;
    bool                _centered_y_;
    bool                _centered_z_;
    geomtools::box      _solid_;
    MWIM                _internals_; /// Internal/daughter volumes

    // Registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(surrounded_boxed_model)

  };

} // end of namespace geomtools

#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(geomtools::surrounded_boxed_model)

#endif // GEOMTOOLS_SURROUNDED_BOXED_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
