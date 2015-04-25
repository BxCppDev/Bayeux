// regular_grid_placement.cc

// Ourselves:
#include <geomtools/regular_grid_placement.h>

// Standard library:
#include <stdexcept>
#include <sstream>
#include <limits>

namespace geomtools {

  // static
  const bool regular_grid_placement::CENTERED;
  const bool regular_grid_placement::UNCENTERED;

  size_t regular_grid_placement::get_dimension () const
  {
    return 2;
  }

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
    return (_number_of_columns_ > 0)
      && (_number_of_rows_ > 0)
      && _basic_placement_.is_valid ()
      && std::isnormal (_column_step_)
      && std::isnormal (_row_step_);
  }

  void regular_grid_placement::invalidate ()
  {
    reset ();
    return;
  }

  bool regular_grid_placement::is_centered () const
  {
    return _centered_;
  }

  void regular_grid_placement::set_centered (bool c_)
  {
    _centered_ = c_;
    return;
  }

  void regular_grid_placement::set_column_step (double dx_)
  {
    _column_step_ = dx_;
    return;
  }

  void regular_grid_placement::set_row_step (double dy_)
  {
    _row_step_ = dy_;
    return;
  }

  void set_steps (double dx_, double dy_);

  void regular_grid_placement::set_steps (double dx_, double dy_)
  {
    set_column_step (dx_);
    set_row_step (dy_);
    return;
  }

  double regular_grid_placement::get_column_step () const
  {
    return _column_step_;
  }

  double regular_grid_placement::get_row_step () const
  {
    return _row_step_;
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
    _basic_placement_ = bp_;
  }

  const placement & regular_grid_placement::get_basic_placement () const
  {
    return _basic_placement_;
  }

  placement & regular_grid_placement::get_basic_placement ()
  {
    return _basic_placement_;
  }

  void regular_grid_placement::set_number_of_columns (size_t nc_)
  {
    _number_of_columns_ = nc_;
    return;
  }

  void regular_grid_placement::set_number_of_rows (size_t nr_)
  {
    _number_of_rows_ = nr_;
    return;
  }

  size_t regular_grid_placement::get_number_of_rows () const
  {
    return _number_of_rows_;
  }

  size_t regular_grid_placement::get_number_of_columns () const
  {
    return _number_of_columns_;
  }

  size_t regular_grid_placement::get_number_of_items () const
  {
    return _number_of_columns_ * _number_of_rows_;
  }

  size_t regular_grid_placement::compute_index_map (std::vector<uint32_t> & map_,
                                                    int item_) const
  {
    DT_THROW_IF ((item_ < 0) || (item_ >= (int)get_number_of_items ()), std::domain_error,
                 "Invalid item index '" << item_ << "' !");
    const uint32_t nitem = (uint32_t) item_;
    const uint32_t icol = nitem % _number_of_columns_;
    const uint32_t irow = nitem / _number_of_columns_;
    map_.clear ();
    map_.push_back (icol);
    map_.push_back (irow);
    return map_.size ();
  }

  void regular_grid_placement::get_placement (int col_, int row_, placement & p_) const
  {
    int item = col_ + row_ * _number_of_columns_;
    get_placement (item, p_);
    return;
  }

  placement regular_grid_placement::get_placement (int col_, int row_) const
  {
    placement p;
    get_placement (col_, row_, p);
    return p;
  }

  void regular_grid_placement::get_placement (int item_, placement & p_) const
  {
    p_ = _basic_placement_;
    double x = 0.0, y = 0.0, z = 0.0;
    const bool centered = _centered_;
    if (is_mode_xy ())
      {
        x = (item_ % _number_of_columns_) * _column_step_;
        y = (item_ / _number_of_columns_) * _row_step_;
        z = 0.0;
        if (centered)
          {
            x -= 0.5 * (_number_of_columns_ - 1) * _column_step_;
            y -= 0.5 * (_number_of_rows_    - 1) * _row_step_;
          }
      }
    if (is_mode_xz ())
      {
        x = (item_ % _number_of_columns_) * _column_step_;
        z = (item_ / _number_of_columns_) * _row_step_;
        y = 0.0;
        if (centered)
          {
            x -= 0.5 * (_number_of_columns_ - 1) * _column_step_;
            z -= 0.5 * (_number_of_rows_    - 1) * _row_step_;
          }
      }
    if (is_mode_yz ())
      {
        y = (item_ % _number_of_columns_) * _column_step_;
        z = (item_ / _number_of_columns_) * _row_step_;
        x = 0.0;
        if (centered)
          {
            y -= 0.5 * (_number_of_columns_ - 1) * _column_step_;
            z -= 0.5 * (_number_of_rows_    - 1) * _row_step_;
          }
      }
    const vector_3d step (x, y, z);
    const vector_3d trans = p_.get_translation () + step;
    p_.set_translation (trans);
    return;
  }

  bool regular_grid_placement::is_mode_xy () const
  {
    return _mode_ == MODE_XY;
  }

  bool regular_grid_placement::is_mode_yz () const
  {
    return _mode_ == MODE_YZ;
  }

  bool regular_grid_placement::is_mode_xz () const
  {
    return _mode_ == MODE_XZ;
  }

  int regular_grid_placement::get_mode () const
  {
    return _mode_;
  }

  void regular_grid_placement::set_mode (int mode_)
  {
    DT_THROW_IF ((mode_ < MODE_XY) || (mode_ > MODE_XZ), std::logic_error, "Invalid mode !");
    _mode_ = mode_;
    return;
  }

  // ctor:
  regular_grid_placement::regular_grid_placement () : i_placement ()
  {
    reset ();
    return;
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
    return;
  }

  // dtor:
  regular_grid_placement::~regular_grid_placement ()
  {
    return;
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
    return;
  }

  void regular_grid_placement::reset ()
  {
    _basic_placement_.invalidate ();
    _column_step_ = std::numeric_limits<double>::quiet_NaN();
    _row_step_ = std::numeric_limits<double>::quiet_NaN();
    _number_of_columns_ = 0;
    _number_of_rows_ = 0;
    _mode_ = MODE_XY;
    _centered_ = true;
    return;
  }

  void regular_grid_placement::tree_dump (std::ostream & out_,
                                          const std::string & title_,
                                          const std::string & indent_,
                                          bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    this->i_placement::tree_dump (out_, title_, indent, true);

    {
      std::ostringstream oss_title;
      oss_title << indent << datatools::i_tree_dumpable::tag << "Basic placement :";
      std::ostringstream oss_indent;
      oss_indent << indent << datatools::i_tree_dumpable::skip_tag;
      _basic_placement_.tree_dump (out_,
                                   oss_title.str (),
                                   oss_indent.str ());
    }

    out_ << indent << datatools::i_tree_dumpable::tag << "Mode : "
         << _mode_  << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag << "Column step : "
         << _column_step_  << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag << "Row step : "
         << _row_step_  << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag << "Number of columns : "
         << _number_of_columns_  << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag << "Number of rows : "
         << _number_of_rows_  << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Centered :" << is_centered () << std::endl;

    return;
  }


} // end of namespace geomtools

// end of regular_grid_placement.cc
