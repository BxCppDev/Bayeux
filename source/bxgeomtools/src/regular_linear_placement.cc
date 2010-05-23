// -*- mode: c++; -*- 
/* regular_linear_placement.cc 
 */

#include <geomtools/regular_linear_placement.h>

namespace geomtools {

  using namespace std;
  
  bool regular_linear_placement::has_only_one_rotation () const
  {
    return true;
  }

  bool regular_linear_placement::is_valid () const
  {
    return __number_of_items > 0 
      && geomtools::is_valid (__step)
      && __basic_placement.is_valid ();
  }

  void regular_linear_placement::invalidate ()
  {
    geomtools::invalidate (__step);
    __basic_placement.invalidate ();
    __number_of_items = 0;
    __replicant_axis = REPLICANT_AXIS_NONE;
  }
     
  void regular_linear_placement::set_replicant_axis (int axis_)
  {
    if ((axis_ < REPLICANT_AXIS_X) || (axis_ > REPLICANT_AXIS_Z))
      {
	__replicant_axis = REPLICANT_AXIS_NONE;
	return;
      }
    __replicant_axis = axis_;
  }
 
  void regular_linear_placement::set_step (double x_, double y_, double z_)
  {
    __step.set (x_, y_, z_);
    set_replicant_axis (REPLICANT_AXIS_NONE);
  }

  void regular_linear_placement::set_step (const vector_3d & step_)
  {
    __step = step_;
    set_replicant_axis (REPLICANT_AXIS_NONE);
  }
 
  int regular_linear_placement::get_replicant_axis () const
  {
    return __replicant_axis;
  }

  bool regular_linear_placement::is_replicant_x_axis () const
  {
    return __replicant_axis == REPLICANT_AXIS_X;
  }

  bool regular_linear_placement::is_replicant_y_axis () const
  {
    return __replicant_axis == REPLICANT_AXIS_Y;
  }

  bool regular_linear_placement::is_replicant_z_axis () const
  {
    return __replicant_axis == REPLICANT_AXIS_Z;
  }

  void regular_linear_placement::set_replicant_step_x (double x_)
  {
    __step.set (x_, 0., 0.);
    set_replicant_axis (REPLICANT_AXIS_X);
  }

  void regular_linear_placement::set_replicant_step_y (double y_)
  {
    __step.set (0., y_, 0.);
    set_replicant_axis (REPLICANT_AXIS_Y);
  }

  void regular_linear_placement::set_replicant_step_z (double z_)
  {
    __step.set (0., 0., z_);
    set_replicant_axis (REPLICANT_AXIS_Z);
  }

  const vector_3d & regular_linear_placement::get_step () const
  {
    return __step;
  }

  void regular_linear_placement::set_basic_placement (const placement & bp_)
  {
    __basic_placement = bp_;
  }
  
  const placement & regular_linear_placement::get_basic_placement () const
  {
    return __basic_placement;
  }
  
  placement & regular_linear_placement::get_basic_placement ()
  {
    return __basic_placement;
  }

  void regular_linear_placement::set_number_of_items (size_t n_)
  {
    __number_of_items = n_;
  }
  
  size_t regular_linear_placement::get_number_of_items () const
  {
    return __number_of_items;
  }

  size_t regular_linear_placement::compute_index_map (vector<uint32_t> & map_, 
						      int item_) const
  {
    if ((item_ < 0) || (item_ >= get_number_of_items ()))
      {
 	ostringstream message;
	message << "regular_linear_placement::compute_index_map: " 
		<< "Invalid item index '" << item_ << "' !" << endl;
	throw runtime_error (message.str ());
      }
    map_.clear ();
    map_.push_back (item_);
    return map_.size ();
  }
  
  void regular_linear_placement::get_placement (int item_, placement & p_) const
  {
    p_ = __basic_placement;
    vector_3d trans = p_.get_translation ();
    trans += item_ * __step;
    p_.set_translation (trans);
  }

  bool regular_linear_placement::is_replica () const
  {
    return __replicant_axis != REPLICANT_AXIS_NONE;
  }
 
  // ctor:
  regular_linear_placement::regular_linear_placement () : i_placement ()
  {
    __basic_placement.invalidate ();
    geomtools::invalidate_vector_3d (__step);
    __number_of_items = 0;
    __replicant_axis = REPLICANT_AXIS_NONE;
  }
		
  // ctor:
  regular_linear_placement::regular_linear_placement (const placement & basic_placement_, 
						      const vector_3d & step_,
						      size_t number_of_items_) : i_placement ()
  {
    init (basic_placement_,
	  step_,
	  number_of_items_);
  }

  // ctor:
  regular_linear_placement::regular_linear_placement (const placement & basic_placement_, 
						      double step_,
						      size_t number_of_items_,
						      int replicant_axis_)
  {
    init (basic_placement_,
	  step_,
	  number_of_items_,
	  replicant_axis_);
  }

  // dtor:
  regular_linear_placement::~regular_linear_placement ()
  {
  }
 
  void regular_linear_placement::init (const placement & basic_placement_, 
				       const vector_3d & step_,
				       size_t number_of_items_)
  {
    set_basic_placement (basic_placement_);
    set_number_of_items (number_of_items_);
    set_step (step_);
  }

  void regular_linear_placement::init (const placement & basic_placement_, 
				       double step_,
				       size_t number_of_items_,
				       int replicant_axis_)
  {
    set_basic_placement (basic_placement_);
    set_number_of_items (number_of_items_);
    if (replicant_axis_ == REPLICANT_AXIS_X) set_replicant_step_x (step_);
    else if (replicant_axis_ == REPLICANT_AXIS_Y) set_replicant_step_y (step_);
    else if (replicant_axis_ == REPLICANT_AXIS_Z) set_replicant_step_z (step_);
    else throw runtime_error ("regular_linear_placement::init: Invalid replicant axis !");
  }
   
  void regular_linear_placement::reset ()
  {
    __basic_placement.invalidate ();
    geomtools::invalidate_vector_3d (__step);
    __number_of_items = 0;
    __replicant_axis = REPLICANT_AXIS_NONE;
  }
    
  void regular_linear_placement::tree_dump (ostream & out_, 
					    const string & title_, 
					    const string & indent_, 
					    bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    this->i_placement::tree_dump (out_, title_, indent, true);

    {
      ostringstream oss_title;
      oss_title << indent << i_tree_dumpable::tag << "Basic placement :";
      ostringstream oss_indent;
      oss_indent << indent << i_tree_dumpable::skip_tag;
      __basic_placement.tree_dump (out_, 
				   oss_title.str (), 
				   oss_indent.str ());
    }

    out_ << indent << i_tree_dumpable::tag << "Step : " 
	 << __step  << endl;

    out_ << indent << i_tree_dumpable::tag << "Number of items : " 
	 << __number_of_items  << endl;

    out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
	 << "Replica :" << is_replica ();
    if (is_replicant_x_axis ())
      {
	out_ << " [X]";
      }
    if (is_replicant_y_axis ())
      {
	out_ << " [Y]";
      }
    if (is_replicant_z_axis ())
      {
	out_ << " [Z]";
      }
    out_ << endl;

    return;
  }


} // end of namespace geomtools

// end of regular_linear_placement.cc

