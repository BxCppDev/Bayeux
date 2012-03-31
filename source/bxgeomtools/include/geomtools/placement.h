// -*- mode: c++; -*- 
/* placement.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 * 
 * License: 
 * 
 * Description: 
 *  Utilities.
 *
 * History: 
 * 
 */

#ifndef __geomtools__placement_h
#define __geomtools__placement_h 1

#include <iostream>
#include <string>
#include <vector>

#include <geomtools/i_placement.h>

namespace geomtools {

  using namespace std;

  class placement : public i_placement
  {

  public:
 
    bool is_valid () const;

    void invalidate ();

    const vector_3d & get_translation () const;

    void set_translation (double, double, double);

    void set_translation (const vector_3d &);

    bool is_simple_rotation () const; 

    bool is_rotation_x () const; 

    bool is_rotation_y () const; 

    bool is_rotation_z () const; 

    int get_rotation_axis () const;

    double get_rotation_angle () const;

    bool is_zyz_rotation () const; 

    double get_phi () const;

    double get_theta () const;

    double get_delta () const;

    bool has_angles () const;

    const rotation_3d & get_rotation () const;

    const rotation_3d & get_inverse_rotation () const;

  public: 

    // i_placement interface:
    virtual size_t get_number_of_items () const;
   
    virtual void get_placement (int item_, placement & p_) const;

    virtual bool is_replica () const;
 
    virtual bool has_only_one_rotation () const;

    virtual size_t compute_index_map (vector<uint32_t> & map_, 
                                      int item_) const;
 
  private:

    void _compute_orientation_ ();

    void _compute_orientation_xyz_ (); // just for test

  public:

    void set_orientation (int axis_, double angle_);

    void set_orientation (double phi_, double theta_, double delta_);

    void set (double x_, double y_, double z_, 
              double phi_, double theta_, double delta_);

    void set (double x_, double y_, double z_, 
              int axis_, double angle_);

    void set (const vector_3d & t_, 
              double phi_, double theta_, double delta_);

    void set (const vector_3d & t_, 
              int axis_, double angle_);

    // Not recommended at all:
    void set_orientation (const rotation_3d &);

    // just for test:
    void set_orientation_xyz (double phi_, double theta_, double delta_);

  public: 
    // ctor:
    placement ();

    // ctor:
    placement (const vector_3d & translation_, 
               double phi_,
               double theta_,
               double delta_);

    placement (const vector_3d & translation_, 
               int axis_, 
               double angle_);

    // ctor:
    placement (double x_,
               double y_,
               double z_, 
               double phi_,
               double theta_,
               double delta_);

    placement (double x_,
               double y_,
               double z_, 
               int    axis_, 
               double angle_);

    // dtor:
    virtual ~placement ();

    virtual void reset ();
  
    //  transformation methods:
    void mother_to_child (const vector_3d &, vector_3d &) const;

    vector_3d mother_to_child (const vector_3d &) const;

    void child_to_mother (const vector_3d &, vector_3d &) const;

    vector_3d child_to_mother (const vector_3d &) const;

    void mother_to_child_direction (const vector_3d &, vector_3d &) const;

    vector_3d mother_to_child_direction (const vector_3d &) const;

    void child_to_mother_direction (const vector_3d &, vector_3d &) const;

    vector_3d child_to_mother_direction (const vector_3d &) const;

    //void mother_to_child (const placement &, placement &) const;

    void child_to_mother (const placement &, placement &) const;


    /** Given two placements P1 and  P2 expressed in  some mother
     * coordinate system,  recompute the relative placement P2 in  
     * the coordinate system of placement P1 and store the result 
     * in P2_1.
     *
     * Usage:
     *
     *   P1.relocate (P2, P2_1);
     *
     */
    void relocate (const placement &, placement &) const;

    // i_tree_dump interface:
    virtual void tree_dump (ostream & out_  = clog, 
                            const string & title_ = "geomutils::placement", 
                            const string & indent_ = "", 
                            bool inherit_ = false) const;

    void dump (ostream      & out_    = clog, 
               const string & title_  = "geomutils::placement", 
               const string & indent_ = "") const;
    
    static bool from_string (const string &, placement &);

    static void to_string (string &, const placement &);
 
    void test () const;

  private: 

    vector_3d    _translation_;      // absolute position in mother frame
    int          _rotation_axis_;    // see utils.h: ROTATION_AXIS_X/Y/Z
    double       _rotation_angle_;
    double       _phi_, _theta_, _delta_; // ZYZ Euler angles
    rotation_3d  _rotation_;         // mother->child frame coord. transformation
    rotation_3d  _inverse_rotation_; // child->mother frame coord. transformation
    
    DATATOOLS_SERIALIZATION_DECLARATION();

  };

  ostream & operator<< (ostream & out_, const placement &);

} // end of namespace geomtools

#endif // __geomtools__placement_h

// end of placement.h
