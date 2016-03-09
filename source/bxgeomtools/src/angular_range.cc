// angular_range.cc

// Ourselves:
#include <geomtools/angular_range.h>

// Standard library:
#include <cmath>
#include <limits>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/exception.h>
#include <datatools/clhep_units.h>
#include <datatools/compiler_macros.h>

// This project:
#include <geomtools/utils.h>

namespace geomtools {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(angular_range, "geomtools::angular_range")

  // static
  std::string angular_range::type_to_label(angular_range::range_type rt_)
  {
    if (rt_ == RANGE_TYPE_PI) return std::string("pi");
    if (rt_ == RANGE_TYPE_TWOPI) return std::string("twopi");
    return std::string("");
  }

  // static
  angular_range::range_type angular_range::label_to_type(const std::string & token_)
  {
    if (token_ == "pi") return RANGE_TYPE_PI;
    if (token_ == "twopi") return RANGE_TYPE_TWOPI;
    return RANGE_TYPE_INVALID;
  }

  // static
  double angular_range::min_angle(range_type rt_)
  {
    double ma = 0.0;
    switch(rt_) {
    case RANGE_TYPE_INVALID :
      datatools::invalidate(ma);
      break;
    default:
      ;
    }
    return ma;
  }

  // static
  double angular_range::max_angle(range_type rt_)
  {
    double ma = 0.0;
    switch(rt_) {
    case RANGE_TYPE_INVALID :
      datatools::invalidate(ma);
      break;
    case RANGE_TYPE_PI :
      ma  = M_PI;
      break;
    case RANGE_TYPE_TWOPI :
      ma  = 2 * M_PI;
      break;
    }
    return ma;
  }

  void angular_range::_set_defaults()
  {
    datatools::invalidate(_start_angle_);
    datatools::invalidate(_delta_angle_);
    _type_ = RANGE_TYPE_INVALID;
    return;
  }

  angular_range::angular_range(const std::string & type_label_)
  {
    // std::cerr << "DEVEL: angular_range::angular_range::CTOR: type_label_ = '" << type_label_ << "'\n";
    _set_defaults();
    if (! type_label_.empty()) {
      range_type rt = label_to_type(type_label_);
      DT_THROW_IF(rt == RANGE_TYPE_INVALID, std::logic_error,
                  "Invalid type label '" << type_label_ << "'!");
      set_type(rt);
    }
    // std::cerr << "DEVEL: type = '" << get_type() << "'\n";
    return;
  }

  angular_range::angular_range(range_type rt_)
  {
    // std::cerr << "DEVEL: angular_range::angular_range::CTOR: rt_ = '" << rt_ << "'\n";
    _set_defaults();
    set_type(rt_);
    return;
  }

  angular_range::angular_range(range_type rt_, double start_angle_, double delta_angle_)
  {
    _set_defaults();
    set_type(rt_);
    set_start_angle(start_angle_);
    set_delta_angle(delta_angle_);
    return;
  }

  bool angular_range::has_type() const
  {
    return _type_ != RANGE_TYPE_INVALID;
  }

  void angular_range::set_type(range_type rt_)
  {
    _set_defaults();
    _type_ = rt_;
    return;
  }

  angular_range::range_type angular_range::get_type() const
  {
    return _type_;
  }

  bool angular_range::is_pi() const
  {
    return _type_ == RANGE_TYPE_PI;
  }

  bool angular_range::is_two_pi() const
  {
    return _type_ == RANGE_TYPE_TWOPI;
  }

  double angular_range::get_min_angle() const
  {
    return min_angle(_type_);
  }

  double angular_range::get_max_angle() const
  {
    return max_angle(_type_);
  }

  bool angular_range::is_partial() const
  {
    return _type_ != RANGE_TYPE_INVALID && has_start_angle() && has_delta_angle();
  }

  bool angular_range::has_start_angle() const
  {
    return datatools::is_valid(_start_angle_);
  }

  void angular_range::set_start_angle(double new_value_)
  {
    DT_THROW_IF (new_value_ < get_min_angle() || new_value_ >= get_max_angle(),
                 std::domain_error,
                 "Invalid [" << new_value_ / CLHEP::degree << " degree] start angle value !");
    if (!has_delta_angle()) {
      _delta_angle_ = get_max_angle() - _start_angle_;
    }
    DT_THROW_IF (_start_angle_ + _delta_angle_ > get_max_angle(),
                 std::domain_error,
                 "Angle overflow with [start=" <<  new_value_ / CLHEP::degree << " + delta="
                 << _delta_angle_  / CLHEP::degree << " > max=" << get_max_angle() / CLHEP::degree << " degree] !");
    _start_angle_ = new_value_;
    return;
  }

  double angular_range::get_start_angle() const
  {
    return _start_angle_;
  }

  bool angular_range::has_delta_angle() const
  {
    return datatools::is_valid(_delta_angle_);
  }

  void angular_range::set_delta_angle(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0 || new_value_ > get_max_angle(),
                 std::domain_error,
                 "Invalid [" << new_value_ / CLHEP::degree << " degree] delta angle value !");
    if (!has_start_angle()) {
      _start_angle_ = get_min_angle();
    }
    DT_THROW_IF (_start_angle_ + new_value_ > get_max_angle(),
                 std::domain_error,
                 "Angle overflow with [start=" << _start_angle_  / CLHEP::degree << " + delta="
                 <<  new_value_ / CLHEP::degree << " > max=" << get_max_angle() / CLHEP::degree << " degree] !");
    _delta_angle_ = new_value_;
    return;
  }

  double angular_range::get_delta_angle() const
  {
    return _delta_angle_;
  }

  void angular_range::set_partial_angles(double start_angle_, double delta_angle_)
  {
    reset_partial_angles();
    set_start_angle(start_angle_);
    set_delta_angle(delta_angle_);
    return;
  }

  bool angular_range::is_valid() const
  {
    if (!has_type()) {
      return false;
    }
    return true;
  }

  void angular_range::initialize(const datatools::properties & config_)
  {
    reset_partial_angles();

    double aunit = CLHEP::degree;
    if (config_.has_key ("angle_unit")) {
      const std::string aunit_str = config_.fetch_string ("angle_unit");
      aunit = datatools::units::get_angle_unit_from (aunit_str);
    }

    if (!has_type()) {
      // Fetch the range type if missing:
      range_type rt = RANGE_TYPE_INVALID;
      if (config_.has_key("type")) {
        const std::string & rt_str = config_.fetch_string("type");
        if (rt_str == type_to_label(RANGE_TYPE_PI)) {
          rt = RANGE_TYPE_PI;
        } else if (rt_str == type_to_label(RANGE_TYPE_TWOPI)) {
          rt = RANGE_TYPE_TWOPI;
        } else {
          DT_THROW(std::logic_error, "Invalid angular range type '" << rt_str << "'!");
        }
      }
      if (rt == RANGE_TYPE_INVALID) {
        // Default range type:
        rt = RANGE_TYPE_TWOPI;
      }
      set_type(rt);
    }

    // Fetch angle information:
    double start_angle = get_min_angle();
    double delta_angle = get_max_angle();
    bool not_full_angle = false;
    if (config_.has_key("start")) {
      start_angle = config_.fetch_real("start");
      if (! config_.has_explicit_unit("start")) {
        start_angle *= aunit;
      }
      not_full_angle = true;
    }
    if (config_.has_key("delta")) {
      delta_angle = config_.fetch_real("delta");
      if (! config_.has_explicit_unit("delta")) {
        delta_angle *= aunit;
      }
      not_full_angle = true;
    }
    if (not_full_angle) {
      set_start_angle(start_angle);
      set_delta_angle(delta_angle);
    }

    return;
  }

  void angular_range::reset()
  {
    _set_defaults();
    return;
  }

  void angular_range::reset_partial_angles()
  {
    datatools::invalidate(_start_angle_);
    datatools::invalidate(_delta_angle_);
    return;
  }

  void angular_range::invalidate()
  {
    _set_defaults();
    return;
  }

  double angular_range::get_first_angle() const
  {
    if (is_partial()) return _start_angle_;
    else return get_min_angle();
  }

  double angular_range::get_last_angle() const
  {
    if (is_partial()) return _start_angle_ + _delta_angle_;
    else return get_max_angle();
  }

  bool angular_range::contains(double angle_, bool DT_UNUSED(modulo_)) const
  {
    DT_THROW_IF(! is_valid(), std::range_error, "Angular range is not valid!");
    DT_THROW_IF(angle_ < get_min_angle(), std::range_error, "Angle [" << angle_ / CLHEP::degree << "] out of range validity!");
    DT_THROW_IF(angle_ > get_max_angle(), std::range_error, "Angle [" << angle_ / CLHEP::degree << "] out of range validity!");
    return ::geomtools::angle_is_in(angle_, get_first_angle(), get_last_angle(), 0.0, false);
  }

  void angular_range::tree_dump(std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Type : ";
    if (has_type()) {
      out_ << "'" << type_to_label(_type_) << "'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;
    if (has_type()) {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Min allowed angle : " << get_min_angle() / CLHEP::degree << " degree" << std::endl;
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Max allowed angle : " << get_max_angle() / CLHEP::degree << " degree" << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Partial : ";
    if (is_partial()) {
      out_ << "<yes>";
    } else {
      out_ << "<no>";
    }
    out_ << std::endl;

    if (is_partial()) {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Start angle : ";
      out_ << _start_angle_ / CLHEP::degree << " degree";
      out_ << std::endl;
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Delta angle : ";
      out_ << _delta_angle_ / CLHEP::degree << " degree";
      out_ << std::endl;
    }

    if (is_valid()) {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "First angle : " << get_first_angle() / CLHEP::degree << " degree" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Last angle : " << get_last_angle() / CLHEP::degree << " degree" << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Valid : " << (is_valid() ? "<yes>": "<no>") << std::endl;

    return;
  }

  angular_range::iterator::iterator(const angular_range & ar_, unsigned int nsteps_)
    : _ref_(ar_)
  {
    DT_THROW_IF(!ar_.is_valid(), std::logic_error, "Cannot construct an iterator from an invalid angular range!");
    DT_THROW_IF(nsteps_ < 1, std::logic_error, "Cannot construct an iterator from an invalid number of samples!");
    _nsamples_ = nsteps_ + 1,
      _step_angle_ = (ar_.get_last_angle() - ar_.get_first_angle()) / nsteps_;
    _current_angle_ = ar_.get_first_angle();
    _current_step_ = 0;
    return;
  }

  void angular_range::iterator::set_first()
  {
    this->_current_step_ = 0;
    this->_current_angle_ = _ref_.get_first_angle();
    return;
  }

  void angular_range::iterator::set_last()
  {
    this->_current_step_ = this->_nsamples_ - 1;
    this->_current_angle_ = _ref_.get_last_angle();
    return;
  }

  void angular_range::iterator::set_finished()
  {
    this->_current_step_ = this->_nsamples_;
    datatools::invalidate(this->_current_angle_);
    return;
  }

  bool angular_range::iterator::is_at_first() const
  {
    return this->_current_step_ == 0;
  }

  bool angular_range::iterator::is_at_last() const
  {
    return this->_current_step_ == (_nsamples_ - 1);
  }

  void angular_range::iterator::step()
  {
    if (! is_finished()) {
      this->_current_angle_ += this->_step_angle_;
      if (this->_current_angle_ > this->_ref_.get_last_angle()) this->_current_angle_ = this->_ref_.get_last_angle();
      this->_current_step_++;
      if (this->_current_step_ == _nsamples_) {
        datatools::invalidate(_current_angle_);
      }
    }
    return;
  }


  // bool angular_range::iterator::operator!=(const angular_range::iterator & rv_) const
  // {
  //   if (&this->_ref_ != &rv_._ref_) return false;
  //   if (this->_current_step_ != rv_._current_step_) return false;
  //   return true;
  // }

  // angular_range::iterator angular_range::iterator::operator--()
  // {
  //   if (_current_step_ > 0) {
  //     this->_current_angle_ -= this->_step_angle_;
  //     if (this->_current_angle_ < this->_ref_.get_first_angle()) this->_current_angle_ = this->_ref_.get_first_angle();
  //     this->_current_step_--;
  //   }
  //   return *this;
  // }

  void angular_range::iterator::tree_dump(std::ostream & out_,
                                          const std::string & title_,
                                          const std::string & indent_,
                                          bool inherit_) const
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Angular range : [@" << &_ref_ << "]\n";

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Nsamples      : [" << _nsamples_ << "]\n";

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Step angle    : [" << _step_angle_ / CLHEP::degree << " degree]\n";

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Current step  : [" << _current_step_ << "]\n";

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Current angle : [" << _current_angle_ / CLHEP::degree << " degree]\n";

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "At first      : " << (is_at_first() ? "<yes>" : "<no>") << "\n";

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "At last       : " << (is_at_last()  ? "<yes>" : "<no>") << "\n";

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Finished      : " << (is_finished() ? "<yes>" : "<no>")
         << std::endl;

    return;
  }

  // angular_range::iterator angular_range::iterator::operator++()
  // {
  //   _current_angle_ += _step_angle_;
  //   if (_current_angle_ > _ref_.get_last_angle()) _current_angle_ = _ref_.get_last_angle();
  //   _current_step_++;
  // }

  // angular_range::iterator angular_range::iterator::operator--()
  // {
  //   _current_angle_ -= _step_angle_;
  //   if (_current_angle_ < _ref_.get_first_angle()) _current_angle_ = _ref_.get_first_angle();
  // }

  unsigned int angular_range::iterator::get_nsamples() const
  {
    return _nsamples_;
  }

  int angular_range::iterator::get_current_step() const
  {
    return _current_step_;
  }

  double angular_range::iterator::get_current_angle() const
  {
    return _current_angle_;
  }

  bool angular_range::iterator::is_finished() const
  {
    return _current_step_ >= _nsamples_;
  }

  angular_range::iterator angular_range::iterator::operator++()
  {
    step();
    return *this;
  }

  double angular_range::iterator::operator*() const
  {
    return get_current_angle();
  }

  bool angular_range::iterator::operator!() const
  {
    return !is_finished();
  }

  // static
  void angular_range::init_ocd(datatools::object_configuration_description & ocd_, const std::string & prefix_ = "")
  {

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern(prefix_+"angle_unit")
        .set_from("geomtools::angular_range")
        .set_terse_description("The unit symbol used for angle unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("degree")
        .set_long_description("A character string that represents the default \n"
                              "angle unit for length parameters of the box.  \n"
                              )
        .add_example("Set the default angle unit::        \n"
                     "                                    \n"
                     "  angle_unit : string = \"degree\"  \n"
                     "                                    \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern(prefix_+"type")
        .set_from("geomtools::angular_range")
        .set_terse_description("The type of angular range")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(true)
        .set_default_value_string("twopi")
        .set_long_description("A character string that represents the type of     \n"
                              "angular range. Range in [0;pi] is labelled 'pi'.  \n"
                              " [0;2*pi] is labelled 'twopi'.  \n"
                              )
        .add_example("Set the type of the angular range:: \n"
                     "                                    \n"
                     "  type : string = \"pi\"            \n"
                     "                                    \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern(prefix_+"start")
        .set_from("geomtools::angle_unit")
        .set_terse_description("The value of the starting angle")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the starting angle::               \n"
                     "                                       \n"
                     "  start : real as angle = 10.0 degree  \n"
                     "                                       \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern(prefix_+"delta")
        .set_from("geomtools::angle_unit")
        .set_terse_description("The value of the angle spread")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the starting angle::               \n"
                     "                                       \n"
                     "  delta : real as angle = 50.0 degree  \n"
                     "                                       \n"
                     )
        ;
    }

    return;
  }

} // end of namespace geomtools

/** Opening macro for implementation
 *  @arg geomtools::angular_range the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized(passed by mutable reference).
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(geomtools::angular_range, ocd_)
{
  // The class name :
  ocd_.set_class_name("geomtools::angular_range");

  // The class terse description :
  ocd_.set_class_description("A class representing an angular range");

  // The library the class belongs to :
  ocd_.set_class_library("geomtools");

  // The class detailed documentation :
  ocd_.set_class_documentation("The angular range is defined by its type and angular spread\n"
                               );

  // Populate the specific OCD :
  geomtools::angular_range::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the      \n"
                               "``datatools::properties`` ASCII format::         \n"
                               "                                                 \n"
                               "  angle_unit : string = \"degree\"               \n"
                               "  type  : string = \"pi\"                        \n"
                               "  start : real as angle = 12.0 degree            \n"
                               "  delta : real as angle = 56.0 degree            \n"
                               "                                                 \n"
                               );

  /* Set the validation support flag :
   *  we activate this if the description of all configuration
   *  properties has been provides(see above). This enables the
   *  OCD tools to check the syntax and validaty of a given
   *  configuration file.
   */
  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'geomtools::angular_range' :
DOCD_CLASS_SYSTEM_REGISTRATION(geomtools::angular_range, "geomtools::angular_range")
