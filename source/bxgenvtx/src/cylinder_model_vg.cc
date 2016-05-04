/* cylinder_model_vg.cc
 */

// Ourselves:
#include <genvtx/cylinder_model_vg.h>

// Standard library:
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <limits>

// Third party:
// - Bayeux/datatools
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/exception.h>
// - Bayeux/mygsl
#include <mygsl/rng.h>
// - Bayeux/materials
#include <materials/manager.h>
// - Bayeux/geomtools
#include <geomtools/geom_info.h>
#include <geomtools/logical_volume.h>
#include <geomtools/i_shape_3d.h>
#include <geomtools/cylinder.h>
#include <geomtools/manager.h>
#include <geomtools/mapping.h>
#include <geomtools/mapping_plugin.h>
#include <geomtools/materials_plugin.h>
#include <geomtools/id_selector.h>

// This project:
#include <genvtx/utils.h>
#include <genvtx/detail/geom_manager_utils.h>
#include <genvtx/vertex_validation.h>

namespace genvtx {

  GENVTX_VG_REGISTRATION_IMPLEMENT(cylinder_model_vg,"genvtx::cylinder_model_vg")

  bool cylinder_model_vg::is_mode_valid() const
  {
    return is_mode_bulk() || is_mode_surface();
  }

  bool cylinder_model_vg::is_mode_bulk() const
  {
    return get_mode() == utils::MODE_BULK;
  }

  bool cylinder_model_vg::is_mode_surface() const
  {
    return get_mode() == utils::MODE_SURFACE;
  }

  bool cylinder_model_vg::is_surface_side() const
  {
    return _surface_side_;
  }

  bool cylinder_model_vg::is_surface_bottom() const
  {
    return _surface_bottom_;
  }

  bool cylinder_model_vg::is_surface_top() const
  {
    return _surface_top_;
  }

  void cylinder_model_vg::set_surface_side(bool s_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _surface_side_ = s_;
    return;
  }

  void cylinder_model_vg::set_surface_top(bool s_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _surface_top_ = s_;
    return;
  }

  void cylinder_model_vg::set_surface_bottom(bool s_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _surface_bottom_ = s_;
    return;
  }

  void cylinder_model_vg::set_skin_skip(double skin_skip_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
    _skin_skip_ = skin_skip_;
    return;
  }

  void cylinder_model_vg::set_skin_thickness(double skin_thickness_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
    _skin_thickness_ = skin_thickness_;
    return;
  }

  int cylinder_model_vg::get_mode() const
  {
    return _mode_;
  }

  void cylinder_model_vg::set_mode(int mode_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    if ((mode_ == utils::MODE_BULK) || (mode_ == utils::MODE_SURFACE)) {
      _mode_ = mode_;
    } else {
      _mode_ = utils::MODE_INVALID;
    }
    return;
  }

  cylinder_model_vg::cylinder_model_vg() : genvtx::i_from_model_vg()
  {
    _initialized_ = false;
    _set_defaults_();
    return;
  }

  bool cylinder_model_vg::is_initialized() const
  {
    return _initialized_;
  }

  cylinder_model_vg::~cylinder_model_vg()
  {
    if (is_initialized()) reset();
    return;
  }

  void cylinder_model_vg::_set_defaults_()
  {
    // Internal reset:
    _mode_ = utils::MODE_INVALID;
    _surface_side_ = false;
    _surface_bottom_ = false;
    _surface_top_ = false;
    _skin_skip_ = 0.0;
    _skin_thickness_ = 0.0;
    this->i_from_model_vg::_set_defaults();
    return;
  }

  void cylinder_model_vg::_reset_()
  {
    _entries_.clear();
    if (_cylinder_vg_.is_initialized()) _cylinder_vg_.reset();
    this->i_from_model_vg::_reset();
    _set_defaults_();
    return;
  }

  void cylinder_model_vg::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is not initialized !");
    _initialized_ = false;
    _reset_();
    return;
  }

  void cylinder_model_vg::_shoot_vertex(::mygsl::rng & random_,
                                        ::geomtools::vector_3d & vertex_)
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is not initialized !");
    geomtools::invalidate(vertex_);
    this->_shoot_vertex_cylinders(random_, vertex_);
    return;
  }

  void cylinder_model_vg::_shoot_vertex_cylinders(mygsl::rng & random_,
                                                  geomtools::vector_3d & vertex_)
  {
    double ran_w = random_.uniform();
    int index = -1;
    for (size_t i = 0; i < _entries_.size(); i++) {
      if (ran_w <= _entries_[i].cumulated_weight) {
        index = i;
        break;
      }
    }
    DT_THROW_IF(index < 0, std::logic_error,
                "Cannot determine vertex location index for vertex generator '" << get_name() << "' !");
    geomtools::vector_3d src_vtx;
    _cylinder_vg_.shoot_vertex(random_, src_vtx);

    const geomtools::placement & world_plct
      = _entries_[index].ginfo->get_world_placement();
    // Special treatment for geomtools::rotated_boxed_model :
    if (_entries_[index].ginfo->get_logical().has_effective_relative_placement()) {
      const geomtools::placement & eff_ref_placement = _entries_[index].ginfo->get_logical().get_effective_relative_placement();
      geomtools::vector_3d rel_vtx;
      eff_ref_placement.child_to_mother(src_vtx, rel_vtx);
      src_vtx = rel_vtx;
    }
    world_plct.child_to_mother(src_vtx, vertex_);

    if (has_vertex_validation()) {
      // Setup the geometry context for the vertex validation system:
      _grab_vertex_validation().grab_geometry_context().set_local_candidate_vertex(src_vtx);
      _grab_vertex_validation().grab_geometry_context().set_global_candidate_vertex(vertex_);
      _grab_vertex_validation().grab_geometry_context().set_ginfo(_entries_[index].get_ginfo());
    }
    return;
  }


  void cylinder_model_vg::_init_()
  {
    DT_THROW_IF(! is_mode_valid(), std::logic_error, "Invalid mode for vertex generator '" << get_name() << "' !");
    DT_THROW_IF(! has_geom_manager(),  std::logic_error,"Missing geometry manager for vertex generator '" << get_name() << "' !");

    const geomtools::mapping * mapping_ptr
      = detail::access_geometry_mapping(get_geom_manager(), get_mapping_plugin_name());
    DT_THROW_IF(mapping_ptr == 0, std::logic_error,
                "No available geometry mapping was found for vertex generator '" << get_name() << "' !");

    const geomtools::mapping & the_mapping = *mapping_ptr;
    const geomtools::geom_info_dict_type & geom_infos
      = the_mapping.get_geom_infos();
    std::list<weight_entry_type> entries;
    for (geomtools::geom_info_dict_type::const_iterator i
           = geom_infos.begin();
         i != geom_infos.end();
         i++) {
      const geomtools::geom_id & gid = i->first;
      if (_get_source_selector().match(gid)) {
        const geomtools::geom_info * ginfo = &(i->second);
        weight_entry_type e;
        e.weight = 0.0;
        e.cumulated_weight = 0.0;
        e.ginfo = ginfo;
        entries.push_back(e);
      }
    }
    DT_THROW_IF(entries.size() == 0, std::logic_error,
                "Cannot compute any source of vertex in vertex generator '" << get_name() << "' !");
    weight_entry_type dummy;
    _entries_.assign(entries.size(), dummy);
    std::copy(entries.begin(), entries.end(), _entries_.begin());

    const geomtools::logical_volume * src_log = 0;
    for (size_t i = 0; i < _entries_.size(); i++) {
      if (src_log == 0) {
        src_log = &_entries_[i].ginfo->get_logical();
      } else {
        DT_THROW_IF(! geomtools::logical_volume::same(*src_log, _entries_[i].ginfo->get_logical()),
                    std::logic_error,
                    "Vertex location with different logical geometry volumes ('" << src_log->get_name()
                    << "' vs. '" << _entries_[i].ginfo->get_logical().get_name()
                    << "') are not supported  (different shapes or materials) in vertex generator '"
                    << get_name() << "' !");
      }
    }

    // Attempt to extract material info :
    double density = -1.0;
    if (is_mode_bulk()) {
      std::string material_name;
      if (src_log->has_material_ref()) {
        material_name = src_log->get_material_ref();
      }
      if (src_log->has_effective_material_ref()) {
        material_name = src_log->get_effective_material_ref();
      }
      if (! material_name.empty()) {
        const materials::manager * mat_mgr_ptr
          = detail::access_materials_manager(get_geom_manager(),
                                             get_materials_plugin_name());
        DT_THROW_IF(mat_mgr_ptr == 0, std::logic_error,
                    "No geometry materials plugin named '" << get_materials_plugin_name()
                    << "' available from the geometry manager for vertex generator '" << get_name() << "' !");
        const materials::manager & mat_mgr = *mat_mgr_ptr;
        materials::material_dict_type::const_iterator found =
          mat_mgr.get_materials().find(material_name);
        if (found != mat_mgr.get_materials().end()) {
          if (found->second.has_ref()) {
            const materials::material & the_material = found->second.get_ref();
            density = the_material.get_density();
            DT_LOG_TRACE(get_logging_priority(),
                         "Density = " << density / (CLHEP::g/CLHEP::cm3) << " g/cm3");
          }
        }
      }
    }

    int surface_mask = 0;
    if (is_mode_surface()) {
      _cylinder_vg_.set_mode(utils::MODE_SURFACE);
      if (_surface_side_)   surface_mask |= geomtools::cylinder::FACE_SIDE;
      if (_surface_bottom_) surface_mask |= geomtools::cylinder::FACE_BOTTOM;
      if (_surface_top_)    surface_mask |= geomtools::cylinder::FACE_TOP;
      _cylinder_vg_.set_surface_mask(surface_mask);
    } else {
      _cylinder_vg_.set_mode(utils::MODE_BULK);
    }
    const geomtools::i_shape_3d * src_shape_ptr = 0;
    {
      DT_THROW_IF(! src_log->has_shape(), std::logic_error,
                  "Logical '" << src_log->get_name() << "' has " << "no shape !");
      src_shape_ptr = &src_log->get_shape();
      if (src_log->has_effective_shape()) {
        src_shape_ptr = &src_log->get_effective_shape();
      }
    }
    const geomtools::i_shape_3d & src_shape = *src_shape_ptr;
    DT_THROW_IF(src_shape.get_shape_name() != "cylinder",
                std::logic_error,
                "Shape is '" << src_shape.get_shape_name() << "' but "
                << "only cylinder shape is supported in vertex generator '" << get_name() << "' !");
    const geomtools::cylinder & cylinder_shape
      = dynamic_cast<const geomtools::cylinder &>(src_shape);
    _cylinder_vg_.set_cylinder(cylinder_shape);
    _cylinder_vg_.set_skin_skip(_skin_skip_);
    _cylinder_vg_.set_skin_thickness(_skin_thickness_);
    _cylinder_vg_.initialize_simple();
    double weight = 0.0;
    if (is_mode_surface()) {
      weight = cylinder_shape.get_surface(surface_mask);
    } else {
      weight = cylinder_shape.get_volume();
    }
    // Compute weight:
    _entries_[0].cumulated_weight = _entries_[0].weight;
    for (size_t i = 0; i < _entries_.size(); i++) {
      _entries_[i].weight = weight;
      double cumul = 0.0;
      if (i > 0) cumul = _entries_[i - 1].cumulated_weight;
      _entries_[i].cumulated_weight = cumul + _entries_[i].weight;
    }

    // Store the total weight before normalization for alternative use :
    double the_weight_value = _entries_.back().cumulated_weight;
    int the_weight_type = weight_info::WEIGHTING_VOLUME;
    if (is_mode_surface()) {
      the_weight_type = weight_info::WEIGHTING_SURFACE;
    }
    weight_info the_weight_info;
    the_weight_info.set_type(the_weight_type);
    the_weight_info.set_value(the_weight_value);
    if (the_weight_type == weight_info::WEIGHTING_VOLUME && density > 0.0) {
      // Total mass computed for homogeneous density and
      the_weight_info.set_mass(the_weight_value * density);
    }
    _set_total_weight(the_weight_info);

    // Normalize weight:
    for (size_t i = 0; i < _entries_.size(); i++) {
      _entries_[i].cumulated_weight /= _entries_.back().cumulated_weight;
    }
    return;
  }

  void cylinder_model_vg::initialize(const ::datatools::properties & setup_,
                                     ::datatools::service_manager & service_manager_,
                                     ::genvtx::vg_dict_type & generators_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");

    this->::genvtx::i_from_model_vg::_initialize(setup_, service_manager_, generators_);

    int mode = utils::MODE_INVALID;
    std::string mode_str;
    bool surface_side   = false;
    bool surface_bottom = false;
    bool surface_top    = false;
    double lunit = CLHEP::mm;
    double skin_skip = 0.0 * CLHEP::mm;
    double skin_thickness = 0.0 * CLHEP::mm ;

    if (setup_.has_key("length_unit")) {
      std::string lunit_str = setup_.fetch_string("length_unit");
      lunit = datatools::units::get_length_unit_from(lunit_str);
    }

    DT_THROW_IF(! setup_.has_key("mode"), std::logic_error,
                "Missing 'mode' property !");
    mode_str = setup_.fetch_string("mode");
    if (mode_str == "bulk") mode = utils::MODE_BULK;
    if (mode_str == "surface") mode = utils::MODE_SURFACE;

    if (mode == utils::MODE_SURFACE) {
      if (setup_.has_key("mode.surface.side")) {
        surface_side = setup_.fetch_boolean("mode.surface.side");
      }
      if (setup_.has_key("mode.surface.bottom")) {
        surface_bottom = setup_.fetch_boolean("mode.surface.bottom");
      }
      if (setup_.has_key("mode.surface.top")) {
        surface_top = setup_.fetch_boolean("mode.surface.top");
      }
      bool surface_all =
        surface_side ||
        surface_bottom || surface_top;
      DT_THROW_IF(! surface_all, std::logic_error,
                  "Missing some activated surface(s) property in vertex generator '" << get_name() << "' !");
    }

    if (setup_.has_key("skin_skip")) {
      skin_skip = setup_.fetch_real("skin_skip");
      if (! setup_.has_explicit_unit("skin_skip")) skin_skip *= lunit;
    }

    if (setup_.has_key("skin_thickness")) {
      skin_thickness = setup_.fetch_real("skin_thickness");
      if (! setup_.has_explicit_unit("skin_thickness")) skin_thickness *= lunit;
    }

    set_skin_skip(skin_skip);
    set_skin_thickness(skin_thickness);
    set_mode(mode);
    if (is_mode_surface()) {
      set_surface_side(surface_side);
      set_surface_bottom(surface_bottom);
      set_surface_top(surface_top);
    }

    _init_();
    _initialized_ = true;
    return;
  }

  void cylinder_model_vg::tree_dump(std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
  {
    namespace du = datatools;
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    this->i_from_model_vg::tree_dump(out_, title_, indent_, true);
    out_ << indent << du::i_tree_dumpable::tag
         << "Mode           : '" << _mode_ << "'" << std::endl;
    if (is_mode_surface()) {
      out_ << indent << du::i_tree_dumpable::tag
           << "Surface side   : " << _surface_side_ << std::endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Surface bottom : " << _surface_bottom_ << std::endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Surface top    : " << _surface_top_ << std::endl;
    }
    out_ << indent << du::i_tree_dumpable::tag
         << "Skin skip      : " << _skin_skip_ / CLHEP::mm << std::endl;
    out_ << indent << du::i_tree_dumpable::tag
         << "Skin thickness : " << _skin_thickness_ / CLHEP::mm << std::endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag(inherit_)
         << "Entries        : " << _entries_.size() << std::endl;
    return;
  }

} // end of namespace genvtx

/***************
 * OCD support *
 ***************/

// OCD support' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::genvtx::cylinder_model_vg,ocd_)
{
  ocd_.set_class_name("genvtx::cylinder_model_vg");
  ocd_.set_class_description("A vertex generator from a cylinder-shaped geometry model");
  ocd_.set_class_library("genvtx");
  // ocd_.set_class_documentation("");

  ::genvtx::i_from_model_vg::ocd_support(ocd_);


  //ocd_.set_configuration_hints("Nothing special.");
  ocd_.add_example("From the bulk volume of a collection of cylinder volumes::           \n"
                   "                                                                     \n"
                   "  length_unit : string = \"mm\"                                      \n"
                   "  origin : string = \" category='field_wire' tracker={0,1} wire=* \" \n"
                   "  mode   : string = \"bulk\"                                         \n"
                   "  skin_skip      : real = 0 mm                                       \n"
                   "  skin_thickness : real = 0 mm                                       \n"
                   "                                                                     \n"
                   );
  ocd_.add_example("From some surfaces of a collection of cylinder volumes::             \n"
                   "                                                                     \n"
                   "  length_unit : string = \"mm\"                                      \n"
                   "  origin : string = \" category='field_wire' tracker={0,1} wire=* \" \n"
                   "  mode : string = \"surface\"                                        \n"
                   "  mode.surface.side   : boolean = 1                                  \n"
                   "  mode.surface.bottom : boolean = 0                                  \n"
                   "  mode.surface.top    : boolean = 0                                  \n"
                   "  skin_skip           : real = 0 mm                                  \n"
                   "  skin_thickness      : real = 0 mm                                  \n"
                   "                                                                     \n"
                   );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(genvtx::cylinder_model_vg,"genvtx::cylinder_model_vg")
