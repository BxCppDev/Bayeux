// -*- mode: c++; -*-
/* regular_linear_placement.cc
 */

#include <geomtools/regular_linear_placement.h>

#include <stdexcept>
#include <sstream>

namespace geomtools {

  using namespace std;

  size_t regular_linear_placement::get_dimension () const
  {
    return 1;
  }

  bool regular_linear_placement::has_only_one_rotation () const
  {
    return true;
  }

  bool regular_linear_placement::is_valid () const
  {
    return _number_of_items_ > 0
      && geomtools::is_valid (_step_)
      && _basic_placement_.is_valid ();
  }

  void regular_linear_placement::invalidate ()
  {
    geomtools::invalidate (_step_);
    _basic_placement_.invalidate ();
    _number_of_items_ = 0;
    _replicant_axis_ = REPLICANT_AXIS_NONE;
    return;
  }

  void regular_linear_placement::set_replicant_axis (int axis_)
  {
    if ((axis_ < REPLICANT_AXIS_X) || (axis_ > REPLICANT_AXIS_Z))
      {
        _replicant_axis_ = REPLICANT_AXIS_NONE;
        return;
      }
    _replicant_axis_ = axis_;
    return;
  }

  void regular_linear_placement::set_step (double x_, double y_, double z_)
  {
    _step_.set (x_, y_, z_);
    set_replicant_axis (REPLICANT_AXIS_NONE);
    return;
  }

  void regular_linear_placement::set_step (const vector_3d & step_)
  {
    _step_ = step_;
    set_replicant_axis (REPLICANT_AXIS_NONE);
    return;
  }

  int regular_linear_placement::get_replicant_axis () const
  {
    return _replicant_axis_;
  }

  bool regular_linear_placement::is_replicant_x_axis () const
  {
    return _replicant_axis_ == REPLICANT_AXIS_X;
  }

  bool regular_linear_placement::is_replicant_y_axis () const
  {
    return _replicant_axis_ == REPLICANT_AXIS_Y;
  }

  bool regular_linear_placement::is_replicant_z_axis () const
  {
    return _replicant_axis_ == REPLICANT_AXIS_Z;
  }

  void regular_linear_placement::set_replicant_step_x (double x_)
  {
    _step_.set (x_, 0., 0.);
    set_replicant_axis (REPLICANT_AXIS_X);
  }

  void regular_linear_placement::set_replicant_step_y (double y_)
  {
    _step_.set (0., y_, 0.);
    set_replicant_axis (REPLICANT_AXIS_Y);
    return;
  }

  void regular_linear_placement::set_replicant_step_z (double z_)
  {
    _step_.set (0., 0., z_);
    set_replicant_axis (REPLICANT_AXIS_Z);
    return;
  }

  const vector_3d & regular_linear_placement::get_step () const
  {
    return _step_;
  }

  void regular_linear_placement::set_basic_placement (const placement & bp_)
  {
    _basic_placement_ = bp_;
    return;
  }

  const placement & regular_linear_placement::get_basic_placement () const
  {
    return _basic_placement_;
  }

  placement & regular_linear_placement::get_basic_placement ()
  {
    return _basic_placement_;
  }

  void regular_linear_placement::set_number_of_items (size_t n_)
  {
    _number_of_items_ = n_;
  }

  size_t regular_linear_placement::get_number_of_items () const
  {
    return _number_of_items_;
  }

  size_t regular_linear_placement::compute_index_map (vector<uint32_t> & map_,
                                                      int item_) const
  {
    DT_THROW_IF ((item_ < 0) || (item_ >= (int)get_number_of_items ()),
                 std::domain_error,
                 "Invalid item index '" << item_ << "' !");
    map_.clear ();
    map_.push_back (item_);
    return map_.size ();
  }

  void regular_linear_placement::get_placement (int item_, placement & p_) const
  {
    p_ = _basic_placement_;
    vector_3d trans = p_.get_translation ();
    trans += item_ * _step_;
    p_.set_translation (trans);
    return;
  }

  bool regular_linear_placement::is_replica () const
  {
    return _replicant_axis_ != REPLICANT_AXIS_NONE
      && (!geomtools::is_valid(_step_) || _step_.mag() == 0.0);
  }

  // ctor:
  regular_linear_placement::regular_linear_placement () : i_placement ()
  {
    _basic_placement_.invalidate ();
    geomtools::invalidate_vector_3d (_step_);
    _number_of_items_ = 0;
    _replicant_axis_ = REPLICANT_AXIS_NONE;
    return;
  }

  // ctor:
  regular_linear_placement::regular_linear_placement (const placement & basic_placement_,
                                                      const vector_3d & step_,
                                                      size_t number_of_items_) : i_placement ()
  {
    init (basic_placement_,
          step_,
          number_of_items_);
    return;
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
    return;
  }

  // dtor:
  regular_linear_placement::~regular_linear_placement ()
  {
    return;
  }

  void regular_linear_placement::init (const placement & basic_placement_,
                                       const vector_3d & step_,
                                       size_t number_of_items_)
  {
    set_basic_placement (basic_placement_);
    set_number_of_items (number_of_items_);
    set_step (step_);
    return;
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
    else DT_THROW_IF (true, std::logic_error, "Invalid replicant axis !");
    return;
  }

  void regular_linear_placement::reset ()
  {
    _basic_placement_.invalidate ();
    geomtools::invalidate_vector_3d (_step_);
    _number_of_items_ = 0;
    _replicant_axis_ = REPLICANT_AXIS_NONE;
    return;
  }

  void regular_linear_placement::tree_dump (ostream & out_,
                                            const string & title_,
                                            const string & indent_,
                                            bool inherit_) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    this->i_placement::tree_dump (out_, title_, indent, true);

    {
      out_ << indent << datatools::i_tree_dumpable::tag << "Basic placement : "
           << endl;
      // ostringstream oss_title;
      // oss_title << indent << datatools::i_tree_dumpable::tag << "Basic placement :";
      ostringstream oss_indent;
      oss_indent << indent << datatools::i_tree_dumpable::skip_tag;
      _basic_placement_.tree_dump (out_,
                                   "",
                                   oss_indent.str ());
    }

    out_ << indent << datatools::i_tree_dumpable::tag << "Step : "
         << _step_  << endl;

    out_ << indent << datatools::i_tree_dumpable::tag << "Number of items : "
         << _number_of_items_  << endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Replica : " << is_replica ();
    if (is_replicant_x_axis ()) {
        out_ << " [X]";
      }
    if (is_replicant_y_axis ()) {
        out_ << " [Y]";
      }
    if (is_replicant_z_axis ()) {
        out_ << " [Z]";
      }
    out_ << endl;

    return;
  }


} // end of namespace geomtools

// end of regular_linear_placement.cc
