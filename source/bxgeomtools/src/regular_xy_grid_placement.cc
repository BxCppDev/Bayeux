// -*- mode: c++; -*- 
/* regular_xy_grid_placement.cc 
 */

#include <geomtools/regular_xy_grid_placement.h>

namespace geomtools {

  using namespace std;
 
  bool regular_xy_grid_placement::is_valid () const
  {
    return (__number_of_columns > 0)
      && (__number_of_rows > 0) 
      && __basic_placement.is_valid ()
      && isnormal (__column_step)
      && isnormal (__row_step);
  }

  void regular_xy_grid_placement::invalidate ()
  {
    reset ();
  }

  bool regular_xy_grid_placement::is_centered () const
  {
    return __centered;
  }

  void regular_xy_grid_placement::set_centered (bool c_)
  {
    __centered = c_;
  }

  void regular_xy_grid_placement::set_column_step (double dx_)
  {
    __column_step = dx_;
  }

  void regular_xy_grid_placement::set_row_step (double dy_)
  {
    __row_step = dy_;
  }

  void set_steps (double dx_, double dy_);

  void regular_xy_grid_placement::set_steps (double dx_, double dy_)
  {
    set_column_step (dx_);
    set_row_step (dy_);
  }

  double regular_xy_grid_placement::get_column_step () const
  {
    return __column_step;
  }

  double regular_xy_grid_placement::get_x_step () const
  {
    return get_column_step();
  }

  double regular_xy_grid_placement::get_row_step () const
  {
    return __row_step;
  }

  double regular_xy_grid_placement::get_y_step () const
  {
    return get_row_step();
  }

  void regular_xy_grid_placement::set_basic_placement (const placement & bp_)
  {
    __basic_placement = bp_;
  }
  
  const placement & regular_xy_grid_placement::get_basic_placement () const
  {
    return __basic_placement;
  }
  
  placement & regular_xy_grid_placement::get_basic_placement ()
  {
    return __basic_placement;
  }

  void regular_xy_grid_placement::set_number_of_columns (size_t nc_)
  {
    __number_of_columns = nc_;
  }

  void regular_xy_grid_placement::set_number_of_rows (size_t nr_)
  {
    __number_of_rows = nr_;
  }
  
  size_t regular_xy_grid_placement::get_number_of_items () const
  {
    return __number_of_columns * __number_of_rows;
  }

  void regular_xy_grid_placement::get_placement (int col_, int row_, placement & p_) const
  {
    int item = col_ + row_ * __number_of_columns;
    get_placement (item, p_);
  }

  placement regular_xy_grid_placement::get_placement (int col_, int row_) const
  {
    placement p;
    get_placement (col_, row_, p);
    return p;
  }
  
  void regular_xy_grid_placement::get_placement (int item_, placement & p_) const
  {
    p_ = __basic_placement;
    vector_3d trans = p_.get_translation ();
    vector_3d step;
    double x = (item_ % __number_of_columns) * __column_step;
    double y = (item_ / __number_of_columns) * __row_step;
    if (__centered)
      {
	x -= 0.5 * (__number_of_columns - 1) * __column_step;
	y -= 0.5 * (__number_of_rows    - 1) * __row_step;
      }
    step.set (x, y, 0.0);
    trans += step;
    p_.set_translation (trans);
  }

  void regular_xy_grid_placement::set_replica (bool r_)
  {
    __replica = r_;
  }

  bool regular_xy_grid_placement::is_replica () const
  {
    return __replica;
  }
 
  // ctor:
  regular_xy_grid_placement::regular_xy_grid_placement () : i_placement ()
  {
    reset ();
  }
		
  // ctor:
  regular_xy_grid_placement::regular_xy_grid_placement (const placement & basic_placement_, 
					double column_step_,
					double row_step_,
					size_t number_of_columns_,
					size_t number_of_rows_,
					bool centered_,
					bool replica_) : i_placement ()
  {
    init (basic_placement_,
	  column_step_,
	  row_step_,
	  number_of_columns_,
	  number_of_rows_,
	  centered_,
	  replica_);
  }

  // dtor:
  regular_xy_grid_placement::~regular_xy_grid_placement ()
  {
  }
 
  void regular_xy_grid_placement::init (const placement & basic_placement_, 
					double column_step_,
					double row_step_,
					size_t number_of_columns_,
					size_t number_of_rows_,
					bool centered_,
					bool replica_)
  {
    set_basic_placement (basic_placement_);
    set_steps (column_step_, row_step_);
    set_number_of_columns (number_of_columns_);
    set_number_of_rows (number_of_rows_);
    set_centered (centered_);
    set_replica (replica_);
  }
  
  void regular_xy_grid_placement::reset ()
  {
    __basic_placement.invalidate ();
    __column_step = std::numeric_limits<double>::quiet_NaN();
    __row_step = std::numeric_limits<double>::quiet_NaN();
    __number_of_columns = 0;
    __number_of_rows = 0;
    __centered = true;
    __replica = false;
  }
    
  void regular_xy_grid_placement::tree_dump (ostream & out_, 
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

    out_ << indent << i_tree_dumpable::tag << "Column step : " 
	 << __column_step  << endl;

    out_ << indent << i_tree_dumpable::tag << "Row step : " 
	 << __row_step  << endl;

    out_ << indent << i_tree_dumpable::tag << "Number of columns : " 
	 << __number_of_columns  << endl;

    out_ << indent << i_tree_dumpable::tag << "Number of rows : " 
	 << __number_of_rows  << endl;

    out_ << indent << i_tree_dumpable::tag 
	 << "Centered :" << is_centered () << endl;

    out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
	 << "Replica :" << is_replica () << endl;

    return;
  }


} // end of namespace geomtools

// end of regular_xy_grid_placement.cc

