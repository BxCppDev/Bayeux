// -*- mode: c++; -*- 
/* regular_grid_placement.cc 
 */

#include <geomtools/regular_grid_placement.h>

namespace geomtools {

  using namespace std;
  
  bool regular_grid_placement::has_only_one_rotation () const
  {
    return true;
  }

  bool regular_grid_placement::is_replica () const
  {
    return false;
  }
 
  bool regular_grid_placement::is_valid () const
  {
    return (__number_of_columns > 0)
      && (__number_of_rows > 0) 
      && __basic_placement.is_valid ()
      && isnormal (__column_step)
      && isnormal (__row_step);
  }

  void regular_grid_placement::invalidate ()
  {
    reset ();
  }

  bool regular_grid_placement::is_centered () const
  {
    return __centered;
  }

  void regular_grid_placement::set_centered (bool c_)
  {
    __centered = c_;
  }

  void regular_grid_placement::set_column_step (double dx_)
  {
    __column_step = dx_;
  }

  void regular_grid_placement::set_row_step (double dy_)
  {
    __row_step = dy_;
  }

  void set_steps (double dx_, double dy_);

  void regular_grid_placement::set_steps (double dx_, double dy_)
  {
    set_column_step (dx_);
    set_row_step (dy_);
  }

  double regular_grid_placement::get_column_step () const
  {
    return __column_step;
  }

  double regular_grid_placement::get_row_step () const
  {
    return __row_step;
  }

  double regular_grid_placement::get_x_step () const
  {
    if (is_mode_xy ()) return get_column_step ();
    if (is_mode_xz ()) return get_column_step ();
    if (is_mode_yz ()) return 0.0;
    return 0.0;
  }

  double regular_grid_placement::get_y_step () const
  {
    if (is_mode_xy ()) return get_row_step ();
    if (is_mode_xz ()) return 0.0;
    if (is_mode_yz ()) return get_column_step ();
    return 0.0;
  }

  double regular_grid_placement::get_z_step () const
  {
    if (is_mode_xy ()) return 0.0;
    if (is_mode_xz ()) return get_row_step ();
    if (is_mode_yz ()) return get_row_step ();
    return 0.0;
  }

  void regular_grid_placement::set_basic_placement (const placement & bp_)
  {
    __basic_placement = bp_;
  }
  
  const placement & regular_grid_placement::get_basic_placement () const
  {
    return __basic_placement;
  }
  
  placement & regular_grid_placement::get_basic_placement ()
  {
    return __basic_placement;
  }

  void regular_grid_placement::set_number_of_columns (size_t nc_)
  {
    __number_of_columns = nc_;
  }

  void regular_grid_placement::set_number_of_rows (size_t nr_)
  {
    __number_of_rows = nr_;
  }
  
  size_t regular_grid_placement::get_number_of_rows () const
  {
    return __number_of_rows;
  }
  
  size_t regular_grid_placement::get_number_of_columns () const
  {
    return __number_of_columns;
  }
  
  size_t regular_grid_placement::get_number_of_items () const
  {
    return __number_of_columns * __number_of_rows;
  }

  size_t regular_grid_placement::compute_index_map (vector<uint32_t> & map_, 
						    int item_) const
  {
    if ((item_ < 0) || (item_ >= get_number_of_items ()))
      {
 	ostringstream message;
	message << "regular_grid_placement::compute_index_map: " 
		<< "Invalid item index '" << item_ << "' !" << endl;
	throw runtime_error (message.str ());
      }
    uint32_t nitem = (uint32_t) item_;
    uint32_t icol = nitem % __number_of_columns;
    uint32_t irow = nitem / __number_of_columns;
    map_.clear ();
    map_.push_back (icol);
    map_.push_back (irow);
    return map_.size ();
  }

  void regular_grid_placement::get_placement (int col_, int row_, placement & p_) const
  {
    int item = col_ + row_ * __number_of_columns;
    get_placement (item, p_);
  }

  placement regular_grid_placement::get_placement (int col_, int row_) const
  {
    placement p;
    get_placement (col_, row_, p);
    return p;
  }
  
  void regular_grid_placement::get_placement (int item_, placement & p_) const
  {
    p_ = __basic_placement;
    vector_3d trans = p_.get_translation ();
    vector_3d step;
    double x, y, z;
    if (is_mode_xy ())
      {
	x = (item_ % __number_of_columns) * __column_step;
	y = (item_ / __number_of_columns) * __row_step;
	z = 0.0;
	if (__centered)
	  {
	    x -= 0.5 * (__number_of_columns - 1) * __column_step;
	    y -= 0.5 * (__number_of_rows    - 1) * __row_step;
	  }
      }
    if (is_mode_xz ())
      {
	x = (item_ % __number_of_columns) * __column_step;
	z = (item_ / __number_of_columns) * __row_step;
	y = 0.0;
	if (__centered)
	  {
	    x -= 0.5 * (__number_of_columns - 1) * __column_step;
	    z -= 0.5 * (__number_of_rows    - 1) * __row_step;
	  }
      }
    if (is_mode_yz ())
      {
	y = (item_ % __number_of_columns) * __column_step;
	z = (item_ / __number_of_columns) * __row_step;
	x = 0.0;
	if (__centered)
	  {
	    y -= 0.5 * (__number_of_columns - 1) * __column_step;
	    z -= 0.5 * (__number_of_rows    - 1) * __row_step;
	  }
      }
    step.set (x, y, z);
    trans += step;
    p_.set_translation (trans);
  }

  bool regular_grid_placement::is_mode_xy () const
  {
    return __mode == MODE_XY;
  }

  bool regular_grid_placement::is_mode_yz () const
  {
    return __mode == MODE_YZ;
  }
  
  bool regular_grid_placement::is_mode_xz () const
  {
    return __mode == MODE_XZ;
  }

  int regular_grid_placement::get_mode () const
  {
    return __mode;
  }

  void regular_grid_placement::set_mode (int mode_)
  {
    if ((mode_ < MODE_XY) || (mode_ > MODE_XZ))
      {
	throw runtime_error ("regular_grid_placement: Invalid mode !");
      }
    __mode = mode_;
  }
 
  // ctor:
  regular_grid_placement::regular_grid_placement () : i_placement ()
  {
    reset ();
  }
		
  // ctor:
  regular_grid_placement::regular_grid_placement (const placement & basic_placement_, 
					double column_step_,
					double row_step_,
					size_t number_of_columns_,
					size_t number_of_rows_,
					int mode_,
					bool centered_) : i_placement ()
  {
    init (basic_placement_,
	  column_step_,
	  row_step_,
	  number_of_columns_,
	  number_of_rows_,
	  mode_,
	  centered_);
  }

  // dtor:
  regular_grid_placement::~regular_grid_placement ()
  {
  }
 
  void regular_grid_placement::init (const placement & basic_placement_, 
					double column_step_,
					double row_step_,
					size_t number_of_columns_,
					size_t number_of_rows_,
					int mode_,
					bool centered_)
  {
    set_basic_placement (basic_placement_);
    set_steps (column_step_, row_step_);
    set_number_of_columns (number_of_columns_);
    set_number_of_rows (number_of_rows_);
    set_mode (mode_);
    set_centered (centered_);
  }
  
  void regular_grid_placement::reset ()
  {
    __basic_placement.invalidate ();
    __column_step = std::numeric_limits<double>::quiet_NaN();
    __row_step = std::numeric_limits<double>::quiet_NaN();
    __number_of_columns = 0;
    __number_of_rows = 0;
    __mode = MODE_XY;
    __centered = true;
  }
    
  void regular_grid_placement::tree_dump (ostream & out_, 
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

    out_ << indent << i_tree_dumpable::tag << "Mode : " 
	 << __mode  << endl;

    out_ << indent << i_tree_dumpable::tag << "Column step : " 
	 << __column_step  << endl;

    out_ << indent << i_tree_dumpable::tag << "Row step : " 
	 << __row_step  << endl;

    out_ << indent << i_tree_dumpable::tag << "Number of columns : " 
	 << __number_of_columns  << endl;

    out_ << indent << i_tree_dumpable::tag << "Number of rows : " 
	 << __number_of_rows  << endl;

    out_ << indent << i_tree_dumpable::inherit_tag (inherit_)  
	 << "Centered :" << is_centered () << endl;

    return;
  }


} // end of namespace geomtools

// end of regular_grid_placement.cc

