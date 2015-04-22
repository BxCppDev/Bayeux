// uniform_electric_field.cc

// Ourselves:
#include <emfield/uniform_electric_field.h>

// Third party:
// - Bayeux:
#include <datatools/properties.h>
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/service_manager.h>

namespace emfield {

  // Registration instantiation macro :
  EMFIELD_REGISTRATION_IMPLEMENT(uniform_electric_field, "emfield::uniform_electric_field");

  void uniform_electric_field::_set_defaults()
  {
    _set_electric_field(true);
    _set_electric_field_can_be_combined(true);
    _set_electric_field_is_time_dependent(false);
    _set_magnetic_field(false);
    _set_magnetic_field_can_be_combined(false);
    _set_magnetic_field_is_time_dependent(false);
     geomtools::invalidate(_uniform_electric_field_);
    return;
  }

  // Constructor :
  uniform_electric_field::uniform_electric_field(uint32_t flags_)
    : ::emfield::base_electromagnetic_field(flags_)
  {
    _set_defaults();
    return;
  }

  // Destructor :
  uniform_electric_field::~uniform_electric_field()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  // Getter :
  const geomtools::vector_3d & uniform_electric_field::get_uniform_electric_field() const
  {
    return _uniform_electric_field_;
  }

  // Setter :
  void uniform_electric_field::set_uniform_electric_field(const geomtools::vector_3d & e_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the magnetic field value !");
    _uniform_electric_field_ = e_;
    return;
  }

  int uniform_electric_field::compute_electric_field(const ::geomtools::vector_3d & /* position_ */,
                                                     double  /* time_ */,
                                                     ::geomtools::vector_3d & electric_field_) const
  {
    DT_LOG_DEBUG(get_logging_priority(), "*** COMPUTE UNIFORM ELECTRIC FIELD ***");
    electric_field_ = _uniform_electric_field_;
    DT_LOG_DEBUG(get_logging_priority(),
                 "Electric field value = "
                 << electric_field_ / (CLHEP::volt/CLHEP::m) << " V/m");
    return STATUS_SUCCESS;
  }

  int uniform_electric_field::compute_magnetic_field(const ::geomtools::vector_3d & /* position_ */,
                                                     double /* time_ */,
                                                     ::geomtools::vector_3d & magnetic_field_) const
  {
    geomtools::invalidate(magnetic_field_);
    return STATUS_ERROR;
  }

  void uniform_electric_field::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Cannot reset the uniform electric field !");
    _set_initialized(false);
    _set_defaults();
    this->base_electromagnetic_field::_set_defaults();
    return;
  }


  void uniform_electric_field::initialize(const ::datatools::properties & setup_,
                                          ::datatools::service_manager & service_manager_,
                                          base_electromagnetic_field::field_dict_type & fields_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Field is already initialized !");

    base_electromagnetic_field::_parse_basic_parameters(setup_, service_manager_, fields_);
    _set_electric_field(true);
    _set_electric_field_is_time_dependent(false);
    _set_magnetic_field(false);

    double e_unit = CLHEP::volt / CLHEP::meter;
    if (setup_.has_key("electric_field.unit")) {
      const std::string e_unit_str = setup_.fetch_string("electric_field.unit");
      e_unit = datatools::units::get_electric_field_unit_from(e_unit_str);
    }

    if (! geomtools::is_valid(_uniform_electric_field_)) {
      if (setup_.has_key("electric_field.coordinates")) {
        std::vector<double> elecfield_coord;
        setup_.fetch("electric_field.coordinates", elecfield_coord);
        DT_THROW_IF (elecfield_coord.size() != 3, std::logic_error,
                     "Invalid electric field vector 'electric_field' !");
        _uniform_electric_field_.set(elecfield_coord[0],
                                      elecfield_coord[1],
                                      elecfield_coord[2]);
        if (! setup_.has_explicit_unit("electric_field.coordinates")) {
          _uniform_electric_field_ *= e_unit;
        }
      }
    }

    if (! geomtools::is_valid(_uniform_electric_field_)) {
      double elecfield_amp;
      datatools::invalidate(elecfield_amp);
      if (setup_.has_key("electric_field.magnitude")) {
        elecfield_amp = setup_.fetch_real("electric_field.magnitude");
        if (! setup_.has_explicit_unit("electric_field.magnitude")) {
          elecfield_amp *= e_unit;
        }
      }

      int elecfield_axis = geomtools::ROTATION_AXIS_INVALID;
      if (setup_.has_key("electric_field.axis")) {
        const std::string axis_str = setup_.fetch_string("electric_field.axis");
        elecfield_axis = geomtools::get_rotation_axis_from_label(axis_str);
        DT_THROW_IF(elecfield_axis == geomtools::ROTATION_AXIS_INVALID, std::logic_error, "Invalid 'electric_field.axis' !");
      }

      geomtools::vector_3d elecfield_direction(0., 0., 0.);
      if (elecfield_axis == geomtools::ROTATION_AXIS_X) {
        elecfield_direction.setX(1.0);
      } else if (elecfield_axis == geomtools::ROTATION_AXIS_Y) {
        elecfield_direction.setY(1.0);
      } else if (elecfield_axis == geomtools::ROTATION_AXIS_Z) {
        elecfield_direction.setZ(1.0);
      }

      _uniform_electric_field_ = elecfield_amp * elecfield_direction;
    }

    _set_initialized(true);
    return;
  }

  void uniform_electric_field::tree_dump(std::ostream & out_,
                                         const std::string & title_,
                                         const std::string & indent_,
                                         bool inherit_) const
  {
    this->base_electromagnetic_field::tree_dump(out_, title_, indent_, true);


    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Uniform electric field  : " << _uniform_electric_field_ / (CLHEP::volt/CLHEP::cm) << " V/cm" << std::endl;

    return;
  }

} // end of namespace emfield
