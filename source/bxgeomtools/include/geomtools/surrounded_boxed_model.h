// -*- mode: c++ ; -*-
/* surrounded_boxed_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-02
 * Last modified: 2010-04-02
 *
 * License:
 *
 * Description:
 *
 *   Geometry model with multiple surrounded boxes.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_SURROUNDED_BOXED_MODEL_H_
#define GEOMTOOLS_SURROUNDED_BOXED_MODEL_H_ 1

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include <geomtools/i_boxed_model.h>
#include <geomtools/box.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/model_macros.h>
#include <geomtools/model_with_internal_items_tools.h>

namespace geomtools {

  // define a geometry model with some surrounded boxed models along
  // a specific axis (X, Y or Z)
  GEOMTOOLS_BOXED_MODEL_CLASS_DECLARE(surrounded_boxed_model)
  {
  public:

    static const std::string LABEL_PROPERTY_PREFIX;
    static const std::string MODEL_PROPERTY_PREFIX;
    static const std::string SURROUNDED_LABEL;
    static const std::string SURROUNDING_LABEL;

    struct surrounding_item
    {
      std::string      label;
      const i_model *  model;
      placement        placmt;
      physical_volume  phys;

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
    };

    typedef std::map<int, surrounding_item> surrounding_dict_type;
    typedef std::map<std::string, int> labels_dict_type;

    const std::string & get_material_name () const;

    void set_material_name (const std::string &);

    virtual const geomtools::box & get_box () const;

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

    surrounded_boxed_model ();

    virtual ~surrounded_boxed_model ();

    virtual std::string get_model_id () const;

  protected:

    virtual void _at_construct (const std::string & name_,
                                const datatools::properties & config_,
                                models_col_type * models_ = 0);

  public:

    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

  private:

    std::vector<std::string> _position_labels_;
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

    // 2011-12-05 FM : add support for additional internal objects :
    // internal items :
    MWIM                    _internals_;

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(surrounded_boxed_model);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SURROUNDED_BOXED_MODEL_H_

// end of surrounded_boxed_model.h
