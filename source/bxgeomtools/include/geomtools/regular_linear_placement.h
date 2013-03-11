// -*- mode: c++; -*- 
/* regular_linear_placement.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-26
 * Last modified: 2010-03-31
 * 
 * License: 
 * 
 * Description: 
 *  Utilities.
 *
 * History: 
 * 
 */

#ifndef GEOMTOOLS_REGULAR_LINEAR_PLACEMENT_H_
#define GEOMTOOLS_REGULAR_LINEAR_PLACEMENT_H_ 1

#include <iostream>
#include <iomanip>
#include <string>

#include <geomtools/i_placement.h>
#include <geomtools/placement.h>

namespace geomtools {
 
  class regular_linear_placement
    : public i_placement 
  {
  public:

    enum axis_t
      {
        REPLICANT_AXIS_NONE = -1,
        REPLICANT_AXIS_X = 0,
        REPLICANT_AXIS_Y = 1,
        REPLICANT_AXIS_Z = 2
      };
 
  public:

    virtual size_t get_dimension () const;
 
    virtual bool is_replica () const;
 
    virtual bool has_only_one_rotation () const;
 
    void set_replicant_axis (int);
    int  get_replicant_axis () const;
    bool is_replicant_x_axis () const;
    bool is_replicant_y_axis () const;
    bool is_replicant_z_axis () const;

    bool is_valid () const;

    void invalidate ();

    void set_replicant_step_x (double x_);

    void set_replicant_step_y (double y_);

    void set_replicant_step_z (double z_);

    void set_step (double x_, double y_, double z_);

    void set_step (const vector_3d & step_);

    const vector_3d & get_step () const;

    void set_basic_placement (const placement & bp_);

    const placement & get_basic_placement () const;

    placement & get_basic_placement ();

    void set_number_of_items (size_t n_);

  public: 
    // i_placement interface:
    virtual size_t get_number_of_items () const;
   
    virtual void get_placement (int item_, placement & p_) const;

    virtual size_t compute_index_map (std::vector<uint32_t> & map_, 
                                      int item_) const;

  public: 
    // ctor:
    regular_linear_placement ();
                
    // ctor:
    regular_linear_placement (const placement & basic_placement_, 
                              const vector_3d & step_,
                              size_t number_of_items_);
                
    // ctor:
    regular_linear_placement (const placement & basic_placement_, 
                              double step_,
                              size_t number_of_items_,
                              int replicant_axis_ = REPLICANT_AXIS_NONE);
                
    // dtor:
    virtual ~regular_linear_placement ();

    void init (const placement & basic_placement_, 
               const vector_3d & step_,
               size_t number_of_items_);

    void init (const placement & basic_placement_, 
               double step_,
               size_t number_of_items_,
               int replicant_axis_ = REPLICANT_AXIS_X);
      
    virtual void reset ();

    // i_tree_dump interface:
    virtual void tree_dump (std::ostream & out_  = std::clog, 
                            const std::string & title_ = "geomutils::regular_linear_placement", 
                            const std::string & indent_ = "", 
                            bool inherit_ = false) const;

  private:

    placement _basic_placement_;
    vector_3d _step_;
    size_t    _number_of_items_;
    int       _replicant_axis_;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_REGULAR_LINEAR_PLACEMENT_H_

// end of regular_linear_placement.h
