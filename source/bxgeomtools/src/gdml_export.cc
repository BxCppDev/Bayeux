// gdml_export.cc

// Ourselves:
#include <geomtools/gdml_export.h>

// Standard library:
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <algorithm>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>
// - Bayeux/materials:
#include <materials/material.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/detail/model_tools.h>
#include <geomtools/model_factory.h>
#include <geomtools/gdml_writer.h>
#include <geomtools/units.h>
#include <geomtools/utils.h>
#include <geomtools/material.h>
#include <geomtools/physical_volume.h>
#include <geomtools/placement.h>
#include <geomtools/regular_linear_placement.h>

#include <geomtools/box.h>
#include <geomtools/right_circular_conical_frustrum.h>
#include <geomtools/ellipsoid.h>
#include <geomtools/elliptical_cylinder.h>
#include <geomtools/sphere.h>
#include <geomtools/polycone.h>
#include <geomtools/polyhedra.h>
#include <geomtools/right_polygonal_frustrum.h>
#include <geomtools/cylinder.h>
#include <geomtools/tube.h>
#include <geomtools/torus.h>
#include <geomtools/tessellation.h>
#include <geomtools/wall_solid.h>
#include <geomtools/union_3d.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/intersection_3d.h>

namespace geomtools {

  const std::string & gdml_export::default_length_unit()
  {
    static std::string _lunit;
    if (_lunit.empty()) {
      _lunit = "mm";
    }
    return _lunit;
  }

  const std::string & gdml_export::default_angle_unit()
  {
    static std::string _aunit;
    if (_aunit.empty()) {
      _aunit = "deg";
    }
    return _aunit;
  }

  const std::string & gdml_export::default_density_unit()
  {
    static std::string _dunit;
    if (_dunit.empty()) {
      _dunit = "g/cm3";
    }
    return _dunit;
  }

  datatools::logger::priority gdml_export::get_logging_priority () const
  {
    return _logging_priority_;
  }

  void gdml_export::set_logging_priority (datatools::logger::priority p)
  {
    _logging_priority_ = p;
    return;
  }

  bool gdml_export::is_auxiliary_supported () const
  {
    return _support_auxiliary_;
  }

  void gdml_export::add_auxiliary_support (bool s_)
  {
    _support_auxiliary_ = s_;
    return;
  }

  bool gdml_export::is_replica_supported () const
  {
    return _support_replica_;
  }

  void gdml_export::add_replica_support (bool s_)
  {
    _support_replica_ = s_;
    return;
  }

  const datatools::properties & gdml_export::parameters () const
  {
    return _parameters_;
  }

  datatools::properties & gdml_export::parameters ()
  {
    return _parameters_;
  }

  bool gdml_export::has_fake_materials () const
  {
    return _fake_materials_;
  }

  void gdml_export::add_fake_materials(bool fm_)
  {
    _fake_materials_ = fm_;
    return;
  }

  gdml_export::gdml_export ()
  {
    _logging_priority_          = datatools::logger::PRIO_WARNING;
    _factory_                   = 0;
    _length_unit_               = default_length_unit();
    _angle_unit_                = default_angle_unit();
    _density_unit_              = default_density_unit();
    _external_materials_stream_ = 0;
    _support_auxiliary_         = true;
    _support_replica_           = false;
    _fake_materials_            = false;
    return;
  }

  gdml_export::~gdml_export ()
  {
    return;
  }

  void gdml_export::attach_external_materials (const std::ostringstream & oss_)
  {
    _external_materials_stream_ = &oss_;
    return;
  }

  void gdml_export::export_gdml (const std::string & filename_,
                                 const model_factory & factory_,
                                 const std::string & model_name_)
  {
    DT_THROW_IF (! factory_.is_locked (), std::logic_error, "Factory is not locked !");
    std::string model_name = model_name_;
    if (model_name.empty() || model_name == "<default>") {
      model_name = model_factory::default_world_label();
    }
    std::ofstream fout;
    std::string gdml_filename = filename_;
    datatools::fetch_path_with_env(gdml_filename);
    fout.open(gdml_filename.c_str());
    DT_THROW_IF (! fout,std::runtime_error, "Cannot open GDML file '"
                 << filename_ << "' (as '" << gdml_filename
                 << "') !");
    _export_gdml(fout, factory_, model_name);
    return;
  }

  void gdml_export::_export_gdml (std::ostream & out_,
                                  const model_factory & factory_,
                                  const std::string & model_name_)
  {
    _factory_ = &factory_;
    const models_col_type & models = _factory_->get_models ();
    models_col_type::const_iterator found = models.find (model_name_);
    DT_THROW_IF (found == models.end (), std::logic_error,
                 "Cannot find model '" << model_name_ << "' !");
    const i_model & top_model = *(found->second);
    if (_external_materials_stream_ != 0) {
      DT_LOG_NOTICE(_logging_priority_,
                    "Attach a stream with materials external definitions.");
      _writer_.attach_external_materials (*_external_materials_stream_);
    }
    _writer_.init ();
    _export_gdml_model (top_model);

    std::string xml_version  = gdml_writer::default_xml_version();
    std::string xml_encoding = gdml_writer::default_xml_encoding();
    std::string xsi          = gdml_writer::default_xsi();
    std::string gdml_schema  = gdml_writer::default_remote_gdml_schema();

    if (_parameters_.has_key ("xml_version")) {
      xml_version = _parameters_.fetch_string ("xml_version");
    }

    if (_parameters_.has_key ("xml_encoding")) {
      xml_encoding = _parameters_.fetch_string ("xml_encoding");
    }

    if (_parameters_.has_flag ("local_xsi")) {
      std::ostringstream out;
      out << "file://"
          << GEOMTOOLS_GDML_SCHEMA_LOCAL_PATH
          << '/' << "XMLSchema-instance";
      xsi = out.str ();
    } else {
      if (_parameters_.has_key ("xsi")) {
        xsi = _parameters_.fetch_string ("xsi");
      }
    }

    if (_parameters_.has_flag ("local_gdml_schema")) {
      std::ostringstream out;
      out << "file://"
          << GEOMTOOLS_GDML_SCHEMA_LOCAL_PATH
          << '/' << gdml_writer::default_gdml_schema();
      gdml_schema = out.str ();
    } else {
      if (_parameters_.has_key ("gdml_schema")) {
        gdml_schema = _parameters_.fetch_string ("gdml_schema");
      }
    }

    if (_parameters_.has_key ("length_unit")) {
      _length_unit_ = _parameters_.fetch_string ("length_unit");
    }

    if (_parameters_.has_key ("angle_unit")) {
      _angle_unit_ = _parameters_.fetch_string ("angle_unit");
    }

    if (_parameters_.has_key ("density_unit")) {
      _density_unit_ = _parameters_.fetch_string ("density_unit");
    }

    // add a fake material:
    if (has_fake_materials()) {
      _writer_.add_material (material::material_ref_default(),
                             1.0,
                             1. * CLHEP::g / CLHEP::cm3,
                             1.00,
                             gdml_writer::state_convert(::materials::material::STATE_SOLID));
      _writer_.add_material (material::material_ref_unknown(),
                             1.0,
                             1. * CLHEP::g / CLHEP::cm3,
                             1.00,
                             gdml_writer::state_convert(::materials::material::STATE_SOLID));
      _writer_.add_material (material::material_ref_vacuum(),
                             1.0,
                             1.e-15 * CLHEP::g / CLHEP::cm3,
                             1.00,
                             gdml_writer::state_convert(::materials::material::STATE_GAS));
    }

    _writer_.add_setup("Setup", top_model.get_logical().get_name());

    _writer_.full_write(out_, xml_version, xml_encoding, gdml_schema, xsi);
    _writer_.reset();

    return;
  }

  void gdml_export::_export_gdml_solid (const i_shape_3d & shape_,
                                        const std::string & solid_name_)
  {
    if (_solid_refs_.count(solid_name_) == 1) {
      return;
    }
    std::string shape_name = shape_.get_shape_name();

    // DT_THROW_IF (! gdml_writer::solid_type_is_valid (shape_name),
    //              std::logic_error,
    //              "Solid type '" << shape_name << "' is not valid !");
    DT_THROW_IF (! gdml_writer::solid_type_is_supported (shape_name),
                 std::logic_error,
                 "Solid type '" << shape_name << "' is not supported !");

    if (shape_.is_composite ()) {
      /* GDML constraints:
       * One should check if placement of shape 1 in any composite
       * solid is NULL (translation & rotation).
       */
      if (shape_name == "union_3d") {
        const union_3d & u = static_cast<const union_3d &>(shape_);
        std::string shape_label_1 = u.get_shape1().get_shape_label();
        if (shape_label_1.empty()) {
          shape_label_1 = solid_name_ + ".union.first_ref" + i_model::solid_suffix();
        }
        std::string shape_label_2 = u.get_shape2().get_shape_label();
        if (shape_label_2.empty()) {
          shape_label_2 = solid_name_ + ".union.second_ref" + i_model::solid_suffix();
        }
        std::string pos_ref = solid_name_ + ".union.pos_ref";
        std::string rot_ref = solid_name_ + ".union.rot_ref";
        // only stores the solid #2 placement:
        _writer_.add_position (pos_ref,
                               u.get_shape2 ().get_placement ().get_translation (),
                               _length_unit_);
        _writer_.add_rotation (rot_ref,
                               u.get_shape2 ().get_placement ().get_rotation (),
                               _angle_unit_);
        this->_export_gdml_solid (u.get_shape1 ().get_shape (), shape_label_1);
        this->_export_gdml_solid (u.get_shape2 ().get_shape (), shape_label_2);

        _writer_.add_gdml_union (solid_name_,
                                 shape_label_1,
                                 shape_label_2,
                                 pos_ref,
                                 rot_ref);
      } else if (shape_name == "subtraction_3d") {
        const subtraction_3d & s = static_cast<const subtraction_3d &>(shape_);
        std::string shape_label_1 = s.get_shape1().get_shape_label();
        if (shape_label_1.empty()) {
          shape_label_1 = solid_name_ + ".subtraction.first_ref" + i_model::solid_suffix();
        }
        std::string shape_label_2 = s.get_shape2().get_shape_label();
        if (shape_label_2.empty()) {
          shape_label_2 = solid_name_ + ".subtraction.second_ref" + i_model::solid_suffix();
        }
        std::string pos_ref = solid_name_ + ".subtraction.pos_ref";
        std::string rot_ref = solid_name_ + ".subtraction.rot_ref";

        // only stores the solid #2 placement:
        _writer_.add_position (pos_ref,
                               s.get_shape2 ().get_placement ().get_translation (),
                               _length_unit_);
        _writer_.add_rotation (rot_ref,
                               s.get_shape2 ().get_placement ().get_rotation (),
                               _angle_unit_);
        this->_export_gdml_solid (s.get_shape1 ().get_shape (), shape_label_1);
        this->_export_gdml_solid (s.get_shape2 ().get_shape (), shape_label_2);

        _writer_.add_gdml_subtraction (solid_name_,
                                       shape_label_1,
                                       shape_label_2,
                                       pos_ref,
                                       rot_ref);
      } else if (shape_name == "intersection_3d") {
        const intersection_3d & i = static_cast<const intersection_3d &> (shape_);
        std::string shape_label_1 = i.get_shape1().get_shape_label();
        if (shape_label_1.empty()) {
          shape_label_1 = solid_name_ + ".intersection.first_ref" + i_model::solid_suffix();
        }
        std::string shape_label_2 = i.get_shape2().get_shape_label();
        if (shape_label_2.empty()) {
          shape_label_2 = solid_name_ + ".intersection.second_ref" + i_model::solid_suffix();
        }
        std::string pos_ref = solid_name_ + ".intersection.pos_ref";
        std::string rot_ref = solid_name_ + ".intersection.rot_ref";

        // Only stores the solid #2 placement:
        _writer_.add_position (pos_ref,
                               i.get_shape2 ().get_placement ().get_translation (),
                               _length_unit_);
        _writer_.add_rotation (rot_ref,
                               i.get_shape2 ().get_placement ().get_rotation (),
                               _angle_unit_);
        this->_export_gdml_solid (i.get_shape1 ().get_shape (), shape_label_1);
        this->_export_gdml_solid (i.get_shape2 ().get_shape (), shape_label_2);

        _writer_.add_gdml_intersection (solid_name_,
                                        shape_label_1,
                                        shape_label_2,
                                        pos_ref,
                                        rot_ref);
      } else {
        DT_THROW_IF(true, std::logic_error, "Boolean solid type '" << shape_name << "' is not supported yet !");
      }
    } else {
      if (shape_name == "box") {
        const box & b = dynamic_cast<const box &> (shape_);
        _writer_.add_box(solid_name_, b, _length_unit_);
      } else if (shape_name == "cylinder") {
        const cylinder & c = dynamic_cast<const cylinder &> (shape_);
        _writer_.add_cylinder(solid_name_, c, _length_unit_, _angle_unit_);
      } else if (shape_name == "tube") {
        const tube & t = dynamic_cast<const tube &> (shape_);
        _writer_.add_tube(solid_name_, t, _length_unit_, _angle_unit_);
      } else if (shape_name == "torus") {
        const torus & t = dynamic_cast<const torus &> (shape_);
        _writer_.add_torus(solid_name_, t, _length_unit_, _angle_unit_);
      } else if (shape_name == "sphere") {
        const sphere & s = dynamic_cast<const sphere &> (shape_);
        _writer_.add_sphere(solid_name_, s, _length_unit_, _angle_unit_);
      } else if (shape_name == "right_circular_conical_frustrum") {
        const right_circular_conical_frustrum & cf= dynamic_cast<const right_circular_conical_frustrum &> (shape_);
        _writer_.add_cone_segment(solid_name_, cf, _length_unit_, _angle_unit_);
      } else if (shape_name == "polycone") {
        const polycone & pc = dynamic_cast<const polycone &> (shape_);
        _writer_.add_polycone(solid_name_, pc, _length_unit_, _angle_unit_);
      } else if (shape_name == "polyhedra") {
        const polyhedra & ph = dynamic_cast<const polyhedra &> (shape_);
        _writer_.add_polyhedra(solid_name_, ph, _length_unit_, _angle_unit_);
      } else if (shape_name == "ellipsoid") {
        const ellipsoid & e = dynamic_cast<const ellipsoid &> (shape_);
        _writer_.add_ellipsoid(solid_name_, e, _length_unit_, _angle_unit_);
      } else if (shape_name == "elliptical_cylinder") {
        const elliptical_cylinder & et = dynamic_cast<const elliptical_cylinder &> (shape_);
        _writer_.add_elliptical_tube(solid_name_, et, _length_unit_, _angle_unit_);
      } else if (shape_name == "tessellated") {
        const tessellated_solid & ts = dynamic_cast<const tessellated_solid &> (shape_);
        _writer_.add_tessellated(solid_name_, ts, _length_unit_);
      } else if (shape_name == "wall_solid") {
        const wall_solid & ws = dynamic_cast<const wall_solid &> (shape_);
        _writer_.add_wall(solid_name_, ws, _length_unit_);
      } else {
        DT_THROW(std::logic_error, "Simple solid type '" << shape_name << "' is not supported !");
      }
    }
    _solid_refs_.insert(solid_name_);

    return;
  }

  void gdml_export::_export_gdml_logical (const logical_volume & lv_)
  {
    const geomtools::logical_volume & logical = lv_;
    const std::string & log_name = logical.get_name ();

    if (_volume_refs_.count(log_name) == 1) {
      DT_LOG_TRACE(get_logging_priority (), "Logical '" << log_name << "' is already exported !");
      return;
    }

    // export solid shape:
    const i_shape_3d & log_solid = logical.get_shape();
    std::ostringstream solid_name_oss;
    solid_name_oss << log_name << i_model::solid_suffix();
    std::string solid_name = solid_name_oss.str();
    _export_gdml_solid (log_solid, solid_name);

    // prepare volume export
    std::string material_ref = material::material_ref_unknown();
    std::string solid_ref = solid_name;
    DT_LOG_TRACE (get_logging_priority (), "Logical:");
    if (get_logging_priority () >= datatools::logger::PRIO_TRACE) logical.tree_dump (std::cerr);
    if (logical.has_material_ref ()) {
      material_ref = logical.get_material_ref ();
    } else {
      DT_THROW_IF (! logical.is_abstract (), std::logic_error,
                   "Logical volume '" << log_name << "' has no material !");
    }

    bool skip = false;

    // export a dictionary of auxiliary properties:
    std::map<std::string, std::string> auxprops;
    if (is_auxiliary_supported ()) {
      logical.get_parameters ().export_to_string_based_dictionary (auxprops, false);
    }

    if (! skip && logical.get_physicals ().size () == 0) {
      _writer_.add_volume (log_name,
                           material_ref,
                           solid_ref,
                           auxprops);
      skip = true;
    }

    // there is a replica children:
    if (! skip && (_support_replica_ && logical.is_replica ())) {
      DT_LOG_TRACE (get_logging_priority (), "************** REPLICA **************");
      const physical_volume & phys = *(logical.get_physicals ().begin ()->second);
      DT_LOG_TRACE (get_logging_priority (), "replica phys=" << phys.get_name ());
      const logical_volume & log_child = phys.get_logical ();
      DT_LOG_TRACE (get_logging_priority (), "replica log_child=" << log_child.get_name ());

      _export_gdml_logical (log_child);
      const i_placement * pp = &(phys.get_placement ());

      gdml_writer::replicavol a_replicavol;
      // only support for 'regular_linear_placement':
      const regular_linear_placement * RLP = 0;
      RLP = dynamic_cast<const regular_linear_placement *> (pp);
      DT_THROW_IF (RLP == 0, std::logic_error, "GDML replica support is for 'regular_linear_placement' only !");
      a_replicavol.volumeref = log_child.get_name ();
      a_replicavol.number = pp->get_number_of_items ();
      a_replicavol.mode = "replicate_along_axis";
      if (RLP->is_replicant_x_axis ()) {
        a_replicavol.direction = "x";
        a_replicavol.width = RLP->get_step ().x ();
      } else if (RLP->is_replicant_y_axis ()) {
        a_replicavol.direction = "y";
        a_replicavol.width = RLP->get_step ().y ();
      } else if (RLP->is_replicant_z_axis ()) {
        a_replicavol.direction = "z";
        a_replicavol.width = RLP->get_step ().z ();
      }
      a_replicavol.offset = 0.0;
      DT_LOG_TRACE (get_logging_priority (), "Add volume '" << log_name << "' (replica)...");

      _writer_.add_replica_volume (log_name,
                                   material_ref,
                                   solid_ref,
                                   a_replicavol,
                                   _length_unit_,
                                   _angle_unit_,
                                   auxprops);
      skip = true;
    }

    // there are children:
    if (! skip && (logical.get_physicals ().size () > 0)) {
      DT_LOG_TRACE (get_logging_priority (), "Here we should export the daughter physicals...");
      DT_LOG_TRACE (get_logging_priority (), "List of daughter physicals:");
      if (get_logging_priority () >= datatools::logger::PRIO_TRACE) {
        for (logical_volume::physicals_col_type::const_iterator i
               = logical.get_physicals ().begin ();
             i != logical.get_physicals ().end ();
             i++) {
          const std::string & name = (i->first);
          const physical_volume & phys = *(i->second);
          DT_LOG_TRACE (get_logging_priority (), "    name=" << name << "     " << "phys->name=" << phys.get_name ());
        }
      }

      std::list<gdml_writer::physvol> physvols;
      for (logical_volume::physicals_col_type::const_iterator iter
             = logical.get_physicals ().begin ();
           iter != logical.get_physicals ().end ();
           iter++) {
        const physical_volume & phys = *(iter->second);
        DT_LOG_TRACE (get_logging_priority (), "phys=" << phys.get_name ());
        const logical_volume & log_child = phys.get_logical ();
        DT_LOG_TRACE (get_logging_priority (), "log_child=" << log_child.get_name ());

        _export_gdml_logical (log_child);

        const i_placement * pp = &(phys.get_placement ());
        bool multiple = false;
        size_t nitems = pp->get_number_of_items ();
        bool only_one_rotation = pp->has_only_one_rotation ();
        multiple = (nitems > 1);

        DT_LOG_TRACE (get_logging_priority (), "No replica... for '" << log_name << "'...");
        // XXX UUU
        rotation_3d ref_rot;
        invalidate_rotation_3d (ref_rot);
        std::ostringstream ref_rot_name_oss;
        if (only_one_rotation) {
          ref_rot_name_oss << log_name << '.' << phys.get_name ();
          if (multiple) ref_rot_name_oss << "__" << '0' << ".." << (nitems - 1) << "__";
          ref_rot_name_oss << ".rot";
        }
        DT_LOG_TRACE (get_logging_priority (), "Add placements for " << nitems << " items...");
        for (size_t i = 0; i < nitems; i++) {
          // extract placement for item 'i':
          placement p;
          pp->get_placement (i, p);

          // register the position of item 'i':
          std::ostringstream pos_name_oss;
          pos_name_oss << log_name << '.' << phys.get_name ();
          if (multiple) pos_name_oss << "__" << i << "__";
          pos_name_oss << io::position_suffix();
          _writer_.add_position (pos_name_oss.str (),
                                 p.get_translation (),
                                 _length_unit_);

          DT_LOG_TRACE (get_logging_priority (), "Add rotation...");
          // register the rotation of item 'i':
          //   default rotation name:
          std::ostringstream rot_name_oss;
          rot_name_oss << log_name << '.' << phys.get_name ();
          if (multiple) rot_name_oss << "__" << i << "__";
          rot_name_oss << io::rotation_suffix();
          std::string rot_name = rot_name_oss.str ();
          bool add_rot = false;
          // XXX YYY ZZZ
          if (only_one_rotation) {
            rot_name = ref_rot_name_oss.str ();
            if (! is_valid_rotation_3d (ref_rot)) {
              ref_rot = p.get_rotation ();
              add_rot= true;
            }
          } else {
            // Force add_rot:
            add_rot = true;
          }
          if (add_rot) {
            if (! is_identity (p.get_rotation ())) {
              _writer_.add_rotation (rot_name,
                                     p.get_rotation (),
                                     _angle_unit_);
            }
          }
          if (is_identity (p.get_rotation ())) {
            rot_name = "";
          }
          physvols.push_back (gdml_writer::physvol (log_child.get_name (),
                                                    pos_name_oss.str (),
                                                    rot_name));
        } // for ... items
        DT_LOG_TRACE (get_logging_priority (), "Add volume '" << log_name << "' with physvols=");
        if (get_logging_priority () >= datatools::logger::PRIO_TRACE) {
          std::ostringstream message;
          for (std::list<gdml_writer::physvol>::const_iterator jj = physvols.begin ();
               jj != physvols.end ();
               jj++) {
            message << '"' << jj->volumeref << '"' << ' ';
          }
          DT_LOG_TRACE (get_logging_priority (), message.str ());
        }
      }
      _writer_.add_volume (log_name,
                           material_ref,
                           solid_ref,
                           physvols,
                           auxprops);
      skip = true;
    } // there are children:

    _volume_refs_.insert(log_name);
    return;
  }

  void gdml_export::_export_gdml_model (const i_model & model_)
  {
    const i_model & model = model_;
    DT_THROW_IF (! model.is_constructed (), std::logic_error,
                 "Model '" << model.get_name () << "' is not constructed !");
    _export_gdml_logical (model.get_logical ());
    return;
  }

} // end of namespace geomtools
