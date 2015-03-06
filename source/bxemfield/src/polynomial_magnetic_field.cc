// polynomial_magnetic_field.cc

// Ourselves:
#include <emfield/polynomial_magnetic_field.h>

// Third party:
#include <datatools/properties.h>
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/service_manager.h>

namespace emfield {

  // Registration instantiation macro :
  EMFIELD_REGISTRATION_IMPLEMENT(polynomial_magnetic_field, "emfield::polynomial_magnetic_field");

  // Constructor :
  polynomial_magnetic_field::polynomial_magnetic_field(uint32_t flags_)
    : emfield::base_electromagnetic_field(flags_)
  {
    _set_electric_field(false);
    _set_electric_field_can_be_combined(false);
    _set_magnetic_field(true);
    _set_magnetic_field_can_be_combined(false);
    _magnetic_field_unit_ = CLHEP::gauss;
    return;
  }

  // Destructor :
  polynomial_magnetic_field::~polynomial_magnetic_field()
  {
    if (is_initialized()) reset();
    return;
  }

  int polynomial_magnetic_field::compute_electric_field(const geomtools::vector_3d & /* position_ */,
                                                        double /* time_ */,
                                                        geomtools::vector_3d & electric_field_) const
  {
    geomtools::invalidate(electric_field_);
    return STATUS_ERROR;
  }

  int polynomial_magnetic_field::compute_magnetic_field(const geomtools::vector_3d & position_,
                                                        double /* time_ */,
                                                        geomtools::vector_3d & magnetic_field_) const
  {
    DT_LOG_DEBUG(get_logging_priority(), "*** COMPUTE POLYNOMIAL MAGNETIC FIELD ***");
    geomtools::invalidate(magnetic_field_);

    double bx = 0.0;
    {
      for (size_t i = 0; i < _bx_.px.size(); ++i) {
        bx += _bx_.px.at(i) * std::pow(position_.x(), i);
      }
      for (size_t i = 0; i < _by_.py.size(); ++i) {
        bx += _bx_.py.at(i) * std::pow(position_.y(), i);
      }
      for (size_t i = 0; i < _bx_.pz.size(); ++i) {
        bx += _bx_.pz.at(i) * std::pow(position_.z(), i);
      }
    }
    double by = 0.0;
    {
      for (size_t i = 0; i < _by_.px.size(); ++i) {
        by += _by_.px.at(i) * std::pow(position_.x(), i);
      }
      for (size_t i = 0; i < _by_.py.size(); ++i) {
        by += _by_.py.at(i) * std::pow(position_.y(), i);
      }
      for (size_t i = 0; i < _by_.pz.size(); ++i) {
        by += _by_.pz.at(i) * std::pow(position_.z(), i);
      }
    }
    double bz = 0.0;
    {
      for (size_t i = 0; i < _bz_.px.size(); ++i) {
        bz += _bz_.px.at(i) * std::pow(position_.x(), i);
      }
      for (size_t i = 0; i < _bz_.py.size(); ++i) {
        bz += _bz_.py.at(i) * std::pow(position_.y(), i);
      }
      for (size_t i = 0; i < _bz_.pz.size(); ++i) {
        bz += _bz_.pz.at(i) * std::pow(position_.z(), i);
      }
    }

    magnetic_field_.set(bx,by,bz);
    magnetic_field_ *= _magnetic_field_unit_;

    DT_LOG_DEBUG(get_logging_priority(),
                 "Magnetic field values @ " << position_/CLHEP::mm << " mm = "
                 << magnetic_field_/CLHEP::gauss << " G");
    return STATUS_SUCCESS;
  }

  void polynomial_magnetic_field::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Cannot reset the magnetic field !");
    _set_initialized(false);
    return;
  }

  void polynomial_magnetic_field::initialize(const datatools::properties & setup_,
                                             datatools::service_manager & service_manager_,
                                             emfield::base_electromagnetic_field::field_dict_type & fields_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Field is already initialized !");

    base_electromagnetic_field::_parse_basic_parameters(setup_, service_manager_, fields_);

    if (setup_.has_key("magnetic_field.unit")) {
      const std::string b_unit_str = setup_.fetch_string("magnetic_field.unit");
      _magnetic_field_unit_ = datatools::units::get_magnetic_field_unit_from(b_unit_str);
    }

    std::vector<std::string> coordinates;
    coordinates.push_back("x");
    coordinates.push_back("y");
    coordinates.push_back("z");

    for (size_t i = 0; i < coordinates.size(); ++i) {
      const std::string & a_field = "B" + coordinates.at(i);
      magnetic_field_coordinate * magfield = 0;
      if (a_field == "Bx") {
        magfield = &_bx_;
      } else if (a_field == "By") {
        magfield = &_by_;
      } else if (a_field == "Bz") {
        magfield = &_bz_;
      }
      for (size_t j = 0; j < coordinates.size(); ++j) {
        const std::string & a_coord = coordinates.at(j);
        const std::string key_base = "magnetic_field." + a_field + "." + a_coord;
        if (setup_.has_key(key_base + ".polynomial_degree")) {
          const size_t degree = setup_.fetch_integer(key_base + ".polynomial_degree");
          DT_THROW_IF(! setup_.has_key(key_base + ".polynomial_parameters"), std::logic_error,
                      "Missing polynomial parameters for " << a_field << " field "
                      << "and " << a_coord << " coordinate !");
          polynomial_parameters_type * params = 0;
          if (a_coord == "x") {
            params = &magfield->px;
          } else if (a_coord == "y") {
            params = &magfield->py;
          } else if (a_coord == "z") {
            params = &magfield->pz;
          }
          setup_.fetch(key_base + ".polynomial_parameters", *params);
          DT_THROW_IF(params->size() != degree, std::logic_error,
                      "Missing parameters (" << degree << " required and only "
                      << params->size() << " set)");
        }
      }
    }

    _set_initialized(true);
    return;
  }

} // end of namespace emfield
