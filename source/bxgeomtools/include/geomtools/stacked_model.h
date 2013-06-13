// -*- mode: c++ ; -*-
/* stacked_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-02
 * Last modified: 2010-10-16
 *
 * License:
 *
 * Description:
 *
 *   Geometry model with multiple stacked boxes.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_STACKED_MODEL_H_
#define GEOMTOOLS_STACKED_MODEL_H_ 1

#include <string>
#include <iostream>
#include <map>

#include <geomtools/i_boxed_model.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/box.h>
//#include <geomtools/cylinder.h>
#include <geomtools/model_with_internal_items_tools.h>

namespace geomtools {

  // define a geometry model with some stacked boxed models along
  // a specific axis (X, Y or Z)
  GEOMTOOLS_BOXED_MODEL_CLASS_DECLARE(stacked_model)
  {
  public:

    static const std::string STACKED_PROPERTY_PREFIX;
    static const std::string STACKED_MODEL_PROPERTY_PREFIX;
    static const std::string STACKED_LABEL_PROPERTY_PREFIX;
    static const std::string DEFAULT_STACKED_LABEL_PREFIX;
    static const double DEFAULT_MECHANICS_PLAY;
    static const double DEFAULT_NUMERICS_PLAY;

    enum stacking_axis_t
      {
        STACKING_ALONG_INVALID = -1,
        STACKING_ALONG_X = AXIS_X,
        STACKING_ALONG_Y = AXIS_Y,
        STACKING_ALONG_Z = AXIS_Z
      };

    struct stacked_item
    {
      std::string      label;
      const i_model *  model;
      placement        placmt;
      physical_volume  phys;
      double           limit_min;
      double           limit_max;

    public:

      stacked_item ();

      bool has_limit_min () const;

      bool has_limit_max () const;

      double get_limit_min () const;

      double get_limit_max () const;

      const std::string & get_label () const;

      const i_model & get_model () const;

      const placement & get_placement () const;

      const physical_volume & get_physical_volume () const;
    };

    typedef std::map<int, stacked_item> stacked_dict_type;
    typedef std::map<std::string, int> labels_dict_type;

  public:

    bool is_box_solid() const;

    bool is_cylinder_solid() const;

    const std::string & get_material_name () const;

    void set_material_name (const std::string &);

    void set_stacking_axis (int a_);

    int get_stacking_axis () const;

    bool is_stacking_along_x () const;

    bool is_stacking_along_y () const;

    bool is_stacking_along_z () const;

    virtual const geomtools::box & get_box () const;

    const geomtools::box & get_solid () const;

    size_t get_number_of_stacked_models () const;

    void add_stacked_model (int i_, const i_model &, const std::string & label_ = "");

    bool has_stacked_model (const std::string & label_) const;

    const i_model & get_stacked_model (int i_) const;

    const stacked_item & get_stacked_item (int i_) const;

    const i_model & get_stacked_model (const std::string & label_) const;

    const stacked_item & get_stacked_item (const std::string & label_) const;

    const labels_dict_type & get_labels () const;

    const stacked_dict_type & get_models () const;

    virtual std::string get_model_id () const;

    stacked_model ();

    virtual ~stacked_model ();

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

    std::string       _material_name_;
    int               _stacking_axis_;
    stacked_dict_type _stacked_models_;
    labels_dict_type  _labels_;
    geomtools::box    _solid_;
    //geomtools::cylinder _cyl_solid_;
    double            _numerics_play_;
    double            _mechanics_play_;

    // 2011-12-05 FM : add support for additional internal objects :
    // internal items :
    MWIM              _internals_;

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(stacked_model);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_STACKED_MODEL_H_

// end of stacked_model.h
