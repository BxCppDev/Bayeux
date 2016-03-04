// placement_field.cc

// Ourselves:
#include <emfield/placement_field.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/service_manager.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

namespace emfield {

  // Registration instantiation macro :
  EMFIELD_REGISTRATION_IMPLEMENT(placement_field, "emfield::placement_field")

  void placement_field::_set_defaults()
  {
    _set_electric_field(true);
    _set_electric_field_can_be_combined(false);
    _set_magnetic_field(false);
    _set_magnetic_field_can_be_combined(false);
    return;
  }

  // Constructor :
  placement_field::placement_field(uint32_t flags_)
    : base_electromagnetic_field(flags_)
  {
    _set_defaults();
    return;
  }

  // Destructor :
  placement_field::~placement_field()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  const geomtools::placement & placement_field::get_placement () const
  {
    return _placement_;
  }

  void placement_field::set_placement (const geomtools::placement & p_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the placement value !");
    _placement_ = p_;
    return;
  }

  void placement_field::set_field (base_electromagnetic_field::handle_type & f_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the EM field !");
    _field_ = f_;
    return;
  }

  int placement_field::compute_electric_field(const ::geomtools::vector_3d & position_,
                                              double time_,
                                              ::geomtools::vector_3d & electric_field_) const
  {
    geomtools::invalidate(electric_field_);
    if (! _field_.get().is_electric_field()) {
      return STATUS_ERROR;
    }
    geomtools::vector_3d local_pos;
    _placement_.mother_to_child(position_, local_pos);
    geomtools::vector_3d local_electric_field;
    int status = _field_.get().compute_electric_field(local_pos,
                                                      time_,
                                                      local_electric_field);
    if (status != STATUS_SUCCESS) {
      return status;
    }
    _placement_.child_to_mother(local_electric_field, electric_field_);
    return STATUS_SUCCESS;
  }

  int placement_field::compute_magnetic_field(const ::geomtools::vector_3d & position_,
                                              double time_,
                                              ::geomtools::vector_3d & magnetic_field_) const
  {
    geomtools::invalidate(magnetic_field_);
    if (! _field_.get().is_magnetic_field()) {
      return STATUS_ERROR;
    }
    geomtools::vector_3d local_pos;
    _placement_.mother_to_child(position_, local_pos);
    geomtools::vector_3d local_magnetic_field;
    int status = _field_.get().compute_magnetic_field(local_pos,
                                                      time_,
                                                      local_magnetic_field);
    if (status != STATUS_SUCCESS) {
      return status;
    }
    _placement_.child_to_mother(local_magnetic_field, magnetic_field_);
    return STATUS_SUCCESS;
  }

  void placement_field::reset()
  {
    DT_THROW_IF (! is_initialized (), std::logic_error, "Cannot reset the magnetic field !");
    _set_initialized (false);

    _placement_.reset ();
    _field_.reset ();
    _set_defaults();

    return;
  }

  void placement_field::initialize(const ::datatools::properties & setup_,
                                   ::datatools::service_manager & service_manager_,
                                   ::emfield::base_electromagnetic_field::field_dict_type & fields_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error, "Field is already initialized !");

    base_electromagnetic_field::_parse_basic_parameters (setup_, service_manager_, fields_);

    double length_unit = CLHEP::millimeter;
    double angle_unit  = CLHEP::degree;

    if (setup_.has_key ("length.unit")) {
      const std::string length_unit_str = setup_.fetch_string ("length.unit");
      length_unit = datatools::units::get_length_unit_from (length_unit_str);
    }

    if (setup_.has_key ("angle.unit")) {
      const std::string angle_unit_str = setup_.fetch_string ("angle.unit");
      angle_unit = datatools::units::get_angle_unit_from (angle_unit_str);
    }

    // Parameters of the placement field:

    // Placement:
    if (! _placement_.is_valid()) {
      geomtools::placement pl;
      if (setup_.has_key ("placement")) {
        std::string placement_rule = setup_.fetch_string("placement");
        DT_THROW_IF(! geomtools::placement::from_string(placement_rule, pl), std::logic_error,
                    "Invalid syntax for placement '" << placement_rule << "'!");
        set_placement(pl);
      }
    }

    if (! _placement_.is_valid()) {

      double x = 0.0;
      if (setup_.has_key ("placement.x")) {
        x = setup_.fetch_real ("placement.x");
        if (!setup_.has_explicit_unit("placement.x")) {
          x *= length_unit;
        }
      }

      double y = 0.0;
      if (setup_.has_key ("placement.y")) {
        y = setup_.fetch_real ("placement.y");
        if (!setup_.has_explicit_unit("placement.y")) {
          y *= length_unit;
        }
      }

      double z = 0.0;
      if (setup_.has_key ("placement.z")) {
        z = setup_.fetch_real ("placement.z");
        if (!setup_.has_explicit_unit("placement.z")) {
          z *= length_unit;
        }
      }

      double phi = 0.0;
      if (setup_.has_key ("placement.phi")) {
        phi = setup_.fetch_real ("placement.phi");
        if (!setup_.has_explicit_unit("placement.phi")) {
          phi *= angle_unit;
        }
      }

      double theta = 0.0;
      if (setup_.has_key ("placement.theta")) {
        theta = setup_.fetch_real ("placement.theta");
        if (!setup_.has_explicit_unit("placement.theta")) {
          theta *= angle_unit;
        }
      }

      double delta = 0.0;
      if (setup_.has_key ("placement.delta")) {
        delta = setup_.fetch_real ("placement.delta");
        if (!setup_.has_explicit_unit("placement.delta")) {
          delta *= angle_unit;
        }
      }

      geomtools::placement pl;
      pl.set(x, y, z, phi, theta, delta);
      set_placement(pl);
    }

    DT_THROW_IF(! _placement_.is_valid(), std::logic_error, "Invalid placement!");

    // Field:
    if (! _field_.has_data()) {
      std::string field_name;
      if (setup_.has_key ("field.name")) {
        field_name = setup_.fetch_string ("field.name");
      }
      DT_THROW_IF(field_name.empty(), std::logic_error, "Missing field name!");
      base_electromagnetic_field::field_dict_type::iterator field_found
        = fields_.find (field_name);
      DT_THROW_IF (field_found == fields_.end (), std::logic_error, "Cannot find a EM field with name '" << field_name << "' !");
      set_field (field_found->second);
    }
    _set_electric_field (_field_.get().is_electric_field ());
    _set_magnetic_field (_field_.get().is_magnetic_field ());

    _set_initialized (true);
    return;
  }

  // virtual
  void placement_field::tree_dump(std::ostream & out_,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
  {
    this->base_electromagnetic_field::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Field : " << "'" << _field_.get().get_name() << "' "
         << "(class ID='" <<  _field_.get().get_class_id() << "' "
         << "[@" << &_field_.get() << "])" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Placement " << _placement_ << std::endl;

    return;
  }

} // end of namespace emfield
