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
  EMFIELD_REGISTRATION_IMPLEMENT(polynomial_magnetic_field, "emfield::polynomial_magnetic_field")

  polynomial_magnetic_field::magnetic_field_coordinate::magnetic_field_coordinate()
  {
    coordinate_limits_type limits
      = std::make_pair(-std::numeric_limits<double>::infinity(),
                       +std::numeric_limits<double>::infinity());
    xlimits = limits;
    ylimits = limits;
    zlimits = limits;
    return;
  }

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
    // DT_LOG_DEBUG(get_logging_priority(), "*** COMPUTE POLYNOMIAL MAGNETIC FIELD ***");
    geomtools::invalidate(magnetic_field_);

    const double x = position_.x();
    const double y = position_.y();
    const double z = position_.z();
    double bx = 0.0;
    {
      for (size_t i = 0; i < _bx_.px.size(); ++i) {
        if (x < _bx_.xlimits.first || x > _bx_.xlimits.second) continue;
        bx += _bx_.px.at(i) * std::pow(x, i);
      }
      for (size_t i = 0; i < _by_.py.size(); ++i) {
        if (y < _bx_.ylimits.first || y > _bx_.ylimits.second) continue;
        bx += _bx_.py.at(i) * std::pow(y, i);
      }
      for (size_t i = 0; i < _bx_.pz.size(); ++i) {
        if (z < _bx_.zlimits.first || z > _bx_.zlimits.second) continue;
        bx += _bx_.pz.at(i) * std::pow(z, i);
      }
    }
    double by = 0.0;
    {
      for (size_t i = 0; i < _by_.px.size(); ++i) {
        if (x < _by_.xlimits.first || x > _by_.xlimits.second) continue;
        by += _by_.px.at(i) * std::pow(position_.x(), i);
      }
      for (size_t i = 0; i < _by_.py.size(); ++i) {
        if (y < _by_.ylimits.first || y > _by_.ylimits.second) continue;
        by += _by_.py.at(i) * std::pow(position_.y(), i);
      }
      for (size_t i = 0; i < _by_.pz.size(); ++i) {
        if (z < _by_.zlimits.first || z > _by_.zlimits.second) continue;
        by += _by_.pz.at(i) * std::pow(position_.z(), i);
      }
    }
    double bz = 0.0;
    {
      for (size_t i = 0; i < _bz_.px.size(); ++i) {
        if (x < _bz_.xlimits.first || x > _bz_.xlimits.second) continue;
        bz += _bz_.px.at(i) * std::pow(position_.x(), i);
      }
      for (size_t i = 0; i < _bz_.py.size(); ++i) {
        if (y < _bz_.ylimits.first || y > _bz_.ylimits.second) continue;
        bz += _bz_.py.at(i) * std::pow(position_.y(), i);
      }
      for (size_t i = 0; i < _bz_.pz.size(); ++i) {
        if (z < _bz_.zlimits.first || z > _bz_.zlimits.second) continue;
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
          const size_t degree = setup_.fetch_integer(key_base + ".polynomial_degree") + 1;
          DT_THROW_IF(! setup_.has_key(key_base + ".polynomial_parameters"), std::logic_error,
                      "Missing polynomial parameters for " << a_field << " field "
                      << "and " << a_coord << " coordinate !");
          polynomial_parameters_type * params = 0;
          coordinate_limits_type * limits = 0;
          if (a_coord == "x") {
            params = &magfield->px;
            limits = &magfield->xlimits;
          } else if (a_coord == "y") {
            params = &magfield->py;
            limits = &magfield->ylimits;
          } else if (a_coord == "z") {
            params = &magfield->pz;
            limits = &magfield->zlimits;
          }
          setup_.fetch(key_base + ".polynomial_parameters", *params);
          DT_THROW_IF(params->size() != degree, std::logic_error,
                      "Missing parameters (" << degree << " required and only "
                      << params->size() << " set)");

          if (setup_.has_key(key_base + ".limits")) {
            std::vector<double> the_limits;
            setup_.fetch(key_base + ".limits", the_limits);
            DT_LOG_WARNING(get_logging_priority(), "Limits " << a_coord << " = (" << the_limits[0]/CLHEP::mm
                           << "," << the_limits[1]/CLHEP::mm << ") mm");
            limits->first = the_limits[0];
            limits->second = the_limits[1];
            if (!setup_.has_explicit_unit(key_base + ".limits")) {
              limits->first *= CLHEP::mm;
              limits->second *= CLHEP::mm;
            }
          }
        }
      }
    }

    _set_initialized(true);
    return;
  }

  void polynomial_magnetic_field::tree_dump(std::ostream & out_,
                                         const std::string & title_,
                                         const std::string & indent_,
                                         bool inherit_) const
  {
    this->base_electromagnetic_field::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Magnetic field unit : " << _magnetic_field_unit_ / (CLHEP::gauss) << " G" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Magnetic field X coordinates: "
         << "<not displayed>"
         << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Magnetic field Y coordinates: "
         << "<not displayed>"
         << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Magnetic field Z coordinates: "
         << "<not displayed>"
         << std::endl;

    return;
  }

} // end of namespace emfield
