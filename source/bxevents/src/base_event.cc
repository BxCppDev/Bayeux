// base_event.cc

// Ourselves:
#include <events/base_event.h>

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <iomanip>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>

// This project:
// #include <events/utils.h>

namespace events {

  // Serial tag for datatools::serialization::i_serializable interface :
  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(base_event, "events::base_event")

 // static
  const int32_t base_event::ANY_CATEGORY;
  const int32_t base_event::INVALID_CATEGORY;
  const int32_t base_event::FIRST_CATEGORY;

  base_event::base_event() : geomtools::base_hit()
  {
    _category_ = INVALID_CATEGORY;
    geomtools::invalidate(_position_);
    datatools::invalidate(_time_);
    return;
  }

  base_event::~base_event()
  {
    base_event::reset();
    return;
  }

  bool base_event::has_category() const
  {
    return _category_ > INVALID_CATEGORY;
  }

  int32_t base_event::get_category() const
  {
    return _category_;
  }

  void base_event::set_category(const int32_t cat_)
  {
    _store |= STORE_CATEGORY;
    _category_ = cat_;
    return;
  }

  void base_event::invalidate_category()
  {
    _category_ = INVALID_CATEGORY;
    _store &= ~STORE_CATEGORY;
    return;
  }

  bool base_event::has_position() const
  {
    return geomtools::is_valid(_position_);
  }

  const geomtools::vector_3d &
  base_event::get_position() const
  {
    return _position_;
  }

  void
  base_event::set_position(const geomtools::vector_3d & pos_)
  {
    _store |= STORE_POSITION;
    _position_ = pos_;
    return;
  }

  void base_event::invalidate_position()
  {
    geomtools::invalidate(_position_);
    _store &= ~STORE_POSITION;
    return;
  }

  bool base_event::has_time() const
  {
    return datatools::is_valid(_time_);
  }

  double
  base_event::get_time() const
  {
    return _time_;
  }

  void
  base_event::set_time(const double t_)
  {
    _store |= STORE_TIME;
    _time_ = t_;
    return;
  }

  void base_event::invalidate_time()
  {
    datatools::invalidate(_time_);
    _store &= ~STORE_TIME;
    return;
  }

  void base_event::reset()
  {
    invalidate_category();
    invalidate_position();
    invalidate_time();
    geomtools::base_hit::reset();
    return;
  }

  void base_event::clear()
  {
    base_event::reset();
    return;
  }

  void base_event::invalidate()
  {
    base_event::reset();
    return;
  }

  bool base_event::is_valid () const
  {
    if (! geomtools::base_hit::is_valid()) return false;
    if (! has_category()) return false;
    if (! has_time()) return false;
    if (! has_position()) return false;
    return true;
  }

  void base_event::tree_dump(std::ostream & out_,
                             const std::string & title_,
                             const std::string & indent_,
                             bool inherit_) const
  {
    geomtools::base_hit::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Category : "
         << _category_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Position : "
         << std::setprecision(15) << _position_ / CLHEP::mm
         << " mm" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Time     : " << _time_ / CLHEP::ns
         << " ns" << std::endl;

    return;
  }

} // end of namespace events
