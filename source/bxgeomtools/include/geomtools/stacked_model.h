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

#ifndef __geomtools__stacked_model_h
#define __geomtools__stacked_model_h 1

#include <string> 
#include <iostream>
#include <map>

#include <geomtools/i_boxed_model.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/box.h>
#include <geomtools/model_with_internal_items_tools.h>

namespace geomtools {

  using namespace std;

  // define a geometry model with some stacked boxed models along
  // a specific axis (X, Y or Z)
  GEOMTOOLS_BOXED_MODEL_CLASS_DECLARE(stacked_model) 
  {
  public:

    static const string STACKED_PROPERTY_PREFIX;
    static const string STACKED_MODEL_PROPERTY_PREFIX;
    static const string STACKED_LABEL_PROPERTY_PREFIX;
    static const string DEFAULT_STACKED_LABEL_PREFIX;
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
      string           label;
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

      const string & get_label () const
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

    typedef map<int, stacked_item> stacked_dict_t;
    typedef map<string, int> labels_dict_t;

  private:

    int _get_stacking_axis_ () const;
    const string & _get_material_name_ () const;

  public: 
    const string & get_material_name () const;
    void set_material_name (const string &);
    void set_stacking_axis (int a_);
    int get_stacking_axis () const;
    bool is_stacking_along_x () const;
    bool is_stacking_along_y () const;
    bool is_stacking_along_z () const;

    virtual const geomtools::box & get_box () const;
    const geomtools::box & get_solid () const;

    size_t get_number_of_stacked_models () const;
    void add_stacked_model (int i_, const i_model &, const string & label_ = "");
    bool has_stacked_model (const string & label_) const;
    const i_model & get_stacked_model (int i_) const;
    const stacked_item & get_stacked_item (int i_) const;
    const i_model & get_stacked_model (const string & label_) const;
    const stacked_item & get_stacked_item (const string & label_) const;
    const labels_dict_t & get_labels () const;
    const stacked_dict_t & get_models () const;

  public:
  
    stacked_model ();
  
    virtual ~stacked_model ();

  public:

    virtual string get_model_id () const;

  protected:
  
    virtual void _at_construct (const string & name_,
                                const datatools::utils::properties & config_,
                                models_col_t * models_ = 0);
      
  public: 

    virtual void tree_dump (ostream & out_         = clog, 
                            const string & title_  = "", 
                            const string & indent_ = "", 
                            bool inherit_          = false) const;

  private:

    string         _material_name_;
    int            _stacking_axis_;
    stacked_dict_t _stacked_models_;
    labels_dict_t  _labels_;
    geomtools::box _solid_;
    double         _numerics_play_;
    double         _mechanics_play_;

    // 2011-12-05 FM : add support for additional internal objects :
    // internal items :
    MWIM                    _internals_;

    // registration interface :
    GEOMTOOLS_MODEL_REGISTRATION_INTERFACE(stacked_model);
  
  };

} // end of namespace geomtools

#endif // __geomtools__stacked_model_h

// end of stacked_model.h
