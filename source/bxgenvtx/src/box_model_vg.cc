// box_model_vg.cc

// Ourselves:
#include <genvtx/box_model_vg.h>

// Standard library:
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <limits>

// Third party:
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/units.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/materials:
#include <materials/manager.h>
// - Bayeux/geomtools:
#include <geomtools/geom_info.h>
#include <geomtools/logical_volume.h>
#include <geomtools/i_shape_3d.h>
#include <geomtools/box.h>
#include <geomtools/manager.h>
#include <geomtools/mapping.h>
#include <geomtools/mapping_plugin.h>
#include <geomtools/materials_plugin.h>
#include <geomtools/id_selector.h>

// This project:
#include <genvtx/utils.h>
#include <genvtx/detail/geom_manager_utils.h>
#include <genvtx/vertex_validation.h>
#include <genvtx/box_vg.h>

namespace genvtx {

  GENVTX_VG_REGISTRATION_IMPLEMENT(box_model_vg, "genvtx::box_model_vg")

  // Pimpl-ized working data
  struct box_model_vg::_work_type {
    genvtx::box_vg         vg;           //!< Embedded vertex generator from a box
    geomtools::id_selector src_selector; //!< A selector of GIDs
    geomtools::box         bb; //!< Bounding box
    geomtools::placement   bb_placement; //!< Bounding box placement
    std::vector<weight_entry_type> entries; //!< Information about the weights
    _work_type();
    ~_work_type();
    void reset();
  };

  box_model_vg::_work_type::_work_type()
  {
    bb_placement.invalidate();
    return;
  }

  box_model_vg::_work_type::~_work_type()
  {
    reset();
    return;
  }

  void box_model_vg::_work_type::reset()
  {
    entries.clear();
    if (bb_placement.is_valid()) bb_placement.invalidate();
    if (bb.is_valid()) bb.reset();
    if (vg.is_initialized()) vg.reset();
    if (src_selector.is_initialized()) src_selector.reset();
    return;
  }

  bool box_model_vg::is_mode_valid () const
  {
    return is_mode_bulk () || is_mode_surface ();
  }

  bool box_model_vg::is_mode_bulk () const
  {
    return get_mode () == utils::MODE_BULK;
  }

  bool box_model_vg::is_mode_surface () const
  {
    return get_mode () == utils::MODE_SURFACE;
  }

  bool box_model_vg::is_surface_back () const
  {
    return _surface_back_;
  }

  bool box_model_vg::is_surface_front () const
  {
    return _surface_front_;
  }

  bool box_model_vg::is_surface_bottom () const
  {
    return _surface_bottom_;
  }

  bool box_model_vg::is_surface_top () const
  {
    return _surface_top_;
  }

  bool box_model_vg::is_surface_left () const
  {
    return _surface_left_;
  }

  bool box_model_vg::is_surface_right () const
  {
    return _surface_right_;
  }

  void box_model_vg::set_surface_back (bool s_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _surface_back_ = s_;
    return;
  }

  void box_model_vg::set_surface_front (bool s_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _surface_front_ = s_;
    return;
  }

  void box_model_vg::set_surface_bottom (bool s_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _surface_bottom_ = s_;
    return;
  }

  void box_model_vg::set_surface_top (bool s_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _surface_top_ = s_;
    return;
  }

  void box_model_vg::set_surface_left (bool s_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _surface_left_ = s_;
    return;
  }

  void box_model_vg::set_surface_right (bool s_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _surface_right_ = s_;
    return;
  }

  void box_model_vg::set_skin_skip (double skin_skip_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _skin_skip_ = skin_skip_;
    return;
  }

  void box_model_vg::set_skin_thickness (double skin_thickness_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _skin_thickness_ = skin_thickness_;
    return;
  }

  const std::string & box_model_vg::get_origin_rules () const
  {
    return _origin_rules_;
  }

  void box_model_vg::set_origin_rules (const std::string & origin_rules_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _origin_rules_ = origin_rules_;
    return;
  }

  bool box_model_vg::has_mapping_plugin_name() const
  {
    return ! _mapping_plugin_name_.empty();
  }

  const std::string & box_model_vg::get_mapping_plugin_name() const
  {
    return _mapping_plugin_name_;
  }

  void box_model_vg::set_mapping_plugin_name(const std::string & mpn_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _mapping_plugin_name_ = mpn_;
    return;
  }

  bool box_model_vg::has_materials_plugin_name() const
  {
    return ! _materials_plugin_name_.empty();
  }

  const std::string & box_model_vg::get_materials_plugin_name() const
  {
    return _materials_plugin_name_;
  }

  void box_model_vg::set_materials_plugin_name(const std::string & mpn_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _materials_plugin_name_ = mpn_;
    return;
  }

  int box_model_vg::get_mode () const
  {
    return _mode_;
  }

  void box_model_vg::set_mode (int mode_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    if ((mode_ == utils::MODE_BULK) || (mode_ == utils::MODE_SURFACE)) {
      _mode_ = mode_;
    } else {
      _mode_ = utils::MODE_INVALID;
    }
    return;
  }

  void box_model_vg::set_use_bounding_box(bool ubb_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _use_bb_ = ubb_;
    return;
  }

  bool box_model_vg::is_using_bounding_box()
  {
    return _use_bb_;
  }

  box_model_vg::box_model_vg() : genvtx::i_vertex_generator()
  {
    _initialized_ = false;
    _set_defaults_();
    return;
  }

  bool box_model_vg::is_initialized () const
  {
    return _initialized_;
  }

  box_model_vg::~box_model_vg()
  {
    if (is_initialized()) reset ();
    return;
  }

  void box_model_vg::_set_defaults_ ()
  {
    // Internal reset:
    utils::origin_invalidate (_origin_rules_);
    _mode_           = utils::MODE_INVALID;
    _use_bb_         = false;
    _surface_back_   = false;
    _surface_front_  = false;
    _surface_bottom_ = false;
    _surface_top_    = false;
    _surface_left_   = false;
    _surface_right_  = false;
    _skin_skip_ = 0.0;
    _skin_thickness_ = 0.0;
    _origin_rules_.clear();
    _mapping_plugin_name_.clear();
    _materials_plugin_name_.clear();
    this->i_vertex_generator::_reset ();
    return;
  }

  void box_model_vg::_reset_ ()
  {
    if (_work_.get() != 0) {
      _work_->reset();
      _work_.reset();
    }
    _set_defaults_ ();
    return;
  }

  void box_model_vg::reset ()
  {
    DT_THROW_IF (! is_initialized (), std::logic_error, "Vertex generator '" << get_name() << "' is not initialized !");
    _initialized_ = false;
    _reset_();
    // WAS: _initialized_ = false;
    return;
  }

  void box_model_vg::_shoot_vertex(::mygsl::rng & random_,
                                   ::geomtools::vector_3d & vertex_)
  {
    DT_THROW_IF (! is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is not initialized !");
    geomtools::invalidate(vertex_);
    this->_shoot_vertex_boxes(random_, vertex_);
    return;
  }

  void box_model_vg::_shoot_vertex_boxes (mygsl::rng & random_,
                                          geomtools::vector_3d & vertex_)
  {
    double ran_w = random_.uniform();
    int index = -1;
    for (size_t i = 0; i < _work_->entries.size (); i++) {
      if (ran_w <= _work_->entries[i].cumulated_weight) {
        index = i;
        break;
      }
    }
    DT_THROW_IF (index < 0, std::logic_error,
                 "Cannot determine the vertex location index in vertex generator '" << get_name() << "' !");
    geomtools::vector_3d src_vtx;
    if (_use_bb_) {
      geomtools::vector_3d bb_vtx;
      _work_->vg.shoot_vertex(random_, bb_vtx);
      _work_->bb_placement.child_to_mother(bb_vtx, src_vtx);
    } else {
      _work_->vg.shoot_vertex(random_, src_vtx);
    }
    const geomtools::placement & world_plct
      = _work_->entries[index].ginfo->get_world_placement ();
    // Special treatment for geomtools::rotated_boxed_model :
    if (_work_->entries[index].ginfo->get_logical().has_effective_relative_placement()) {
      const geomtools::placement & eff_ref_placement = _work_->entries[index].ginfo->get_logical().get_effective_relative_placement();
      geomtools::vector_3d rel_vtx;
      eff_ref_placement.child_to_mother(src_vtx, rel_vtx);
      src_vtx = rel_vtx;
    }
    world_plct.child_to_mother (src_vtx, vertex_);

    if (has_vertex_validation()) {
      // Setup the geometry context for the vertex validation system:
      _grab_vertex_validation().grab_geometry_context().set_local_candidate_vertex(src_vtx);
      _grab_vertex_validation().grab_geometry_context().set_global_candidate_vertex(vertex_);
      _grab_vertex_validation().grab_geometry_context().set_ginfo(_work_->entries[index].get_ginfo());
    }
    return;
  }

  void box_model_vg::_init_ ()
  {
    DT_THROW_IF (! is_mode_valid (), std::logic_error, "Invalid mode in vertex generator '" << get_name() << "' !");
    DT_THROW_IF (! has_geom_manager (), std::logic_error, "Missing geometry manager in vertex generator '" << get_name() << "' !");

    const geomtools::mapping * mapping_ptr
      = detail::access_geometry_mapping(get_geom_manager (), _mapping_plugin_name_);
    DT_THROW_IF (mapping_ptr == 0, std::logic_error,
                 "No available geometry mapping was found in vertex generator '" << get_name() << "' !");

    _work_.reset(new _work_type);

    //DT_LOG_FATAL(get_logging_priority(), "*** Origin rules : " << _origin_rules_);
    _work_->src_selector.set_id_mgr(get_geom_manager ().get_id_mgr ());
    _work_->src_selector.initialize(_origin_rules_);
    // _work_->src_selector.dump(std::cerr, "genvtx::box_model_vg::initialize: ID selector:");

    const geomtools::mapping & the_mapping = *mapping_ptr;
    const geomtools::geom_info_dict_type & geom_infos
      = the_mapping.get_geom_infos ();
    std::list<weight_entry_type> entries;
    for (geomtools::geom_info_dict_type::const_iterator i
           = geom_infos.begin ();
         i != geom_infos.end ();
         i++) {
      const geomtools::geom_id & gid = i->first;
      if (_work_->src_selector.match(gid)) {
        const geomtools::geom_info * ginfo = &(i->second);
        weight_entry_type e;
        e.weight = 0.0;
        e.cumulated_weight = 0.0;
        e.ginfo = ginfo;
        entries.push_back (e);
      }
    }
    DT_THROW_IF (entries.size () == 0, std::logic_error,
                 "Cannot compute any source of vertex in vertex generator '" << get_name() << "' !");

    weight_entry_type dummy;
    _work_->entries.assign(entries.size(), dummy);
    std::copy(entries.begin(), entries.end(), _work_->entries.begin());

    const geomtools::logical_volume * src_log = 0;
    for (size_t i = 0; i < _work_->entries.size (); i++) {
      DT_LOG_DEBUG(get_logging_priority(),
                   "ID #" << i << " = " << _work_->entries[i].ginfo->get_id ());
      if (src_log == 0) {
        src_log = &_work_->entries[i].ginfo->get_logical ();
      } else {
        //DT_THROW_IF (src_log != &_work_->entries[i].ginfo->get_logical (),
        DT_THROW_IF (! geomtools::logical_volume::same(*src_log, _work_->entries[i].ginfo->get_logical()),
                     std::logic_error,
                     "Vertex location with different logical geometry volumes ('" << src_log->get_name()
                     << "' vs. '" << _work_->entries[i].ginfo->get_logical ().get_name()
                     << "') are not supported  (different shapes or materials) in vertex generator '"
                     << get_name() << "' !");
      }
    }

    // Attempt to extract material info :
    double density = -1.0;
    if (is_mode_bulk ()) {
      std::string material_name;
      if (src_log->has_material_ref ()) {
        material_name = src_log->get_material_ref ();
      }
      if (src_log->has_effective_material_ref ()) {
        material_name = src_log->get_effective_material_ref ();
      }
      if (! material_name.empty()) {
        const materials::manager * mat_mgr_ptr
          = detail::access_materials_manager(get_geom_manager (),
                                             _materials_plugin_name_);
        DT_THROW_IF (mat_mgr_ptr == 0, std::logic_error,
                     "No geometry materials plugin named '" << _materials_plugin_name_
                     << "' available from the geometry manager in vertex generator '" << get_name() << "' !");
        const materials::manager & mat_mgr = *mat_mgr_ptr;
        materials::material_dict_type::const_iterator found =
          mat_mgr.get_materials ().find (material_name);
        if (found != mat_mgr.get_materials ().end ()) {
          if (found->second.has_ref ()) {
            const materials::material & the_material = found->second.get_ref ();
            density = the_material.get_density ();
            DT_LOG_TRACE(get_logging_priority(),
                         "Density = " << density / (CLHEP::g/CLHEP::cm3) << " g/cm3");
          }
        }
      }
    }
    int surface_mask = 0;
    if (is_mode_surface ()) {
      _work_->vg.set_mode (utils::MODE_SURFACE);
      if (_surface_back_)   surface_mask |= geomtools::box::FACE_BACK;
      if (_surface_front_)  surface_mask |= geomtools::box::FACE_FRONT;
      if (_surface_bottom_) surface_mask |= geomtools::box::FACE_BOTTOM;
      if (_surface_top_)    surface_mask |= geomtools::box::FACE_TOP;
      if (_surface_left_)   surface_mask |= geomtools::box::FACE_LEFT;
      if (_surface_right_)  surface_mask |= geomtools::box::FACE_RIGHT;
      _work_->vg.set_surface_mask (surface_mask);
    } else {
      _work_->vg.set_mode (utils::MODE_BULK);
    }
    const geomtools::i_shape_3d * src_shape_ptr = 0;
    {
      DT_THROW_IF (! src_log->has_shape (), std::logic_error,
                   "Logical '" << src_log->get_name () << "' has " << "no shape !");
      src_shape_ptr = &src_log->get_shape ();
      if (src_log->has_effective_shape ()) {
        src_shape_ptr = &src_log->get_effective_shape ();
      }
    }

    const geomtools::i_shape_3d & src_shape = *src_shape_ptr;
    if (!_use_bb_) {
      // Default:
      DT_THROW_IF (src_shape.get_shape_name () != "box", std::logic_error,
                   "Shape is '" << src_shape.get_shape_name () << "' but "
                   << "only box shape is supported in vertex generator '" << get_name() << "' !");
      const geomtools::box & box_shape
        = dynamic_cast<const geomtools::box &> (src_shape);
      _work_->vg.set_box_ref(box_shape);
    } else {
      // Extract BB from the "src_shape"...
      DT_THROW_IF (!src_shape.has_bounding_data(), std::logic_error,
                   "Shape '" << src_shape.get_shape_name () << "' has no "
                   << "bounding data for vertex generator '" << get_name() << "' !");
      const geomtools::bounding_data & bbdata = src_shape.get_bounding_data();
      bbdata.compute_bounding_box(_work_->bb, _work_->bb_placement);
      // _work_->bb.tree_dump(std::cerr, "BB = ", "DEVEL: ");
      _work_->vg.set_box_ref(_work_->bb);
    }

    _work_->vg.set_skin_skip(_skin_skip_);
    _work_->vg.set_skin_thickness(_skin_thickness_);
    _work_->vg.initialize_simple ();
    // _work_->vg.tree_dump(std::cerr, "Box VG: ", "***** DEVEL ***** ");
    double weight = 0.0;
    if (is_mode_surface()) {
      weight = _work_->vg.get_box_safe().get_surface(surface_mask);
    } else {
      weight = _work_->vg.get_box_safe().get_volume();
    }

    // Compute weight:
    _work_->entries[0].cumulated_weight = _work_->entries[0].weight;
    for (size_t i = 0; i < _work_->entries.size(); i++) {
      _work_->entries[i].weight = weight;
      double cumul = 0.0;
      if (i > 0) cumul = _work_->entries[i - 1].cumulated_weight;
      _work_->entries[i].cumulated_weight = cumul + _work_->entries[i].weight;
    }

    // Store the total weight before normalization for alternative use :
    double the_weight_value = _work_->entries.back ().cumulated_weight;
    int the_weight_type = weight_info::WEIGHTING_VOLUME;
    if (is_mode_surface ())  {
      the_weight_type = weight_info::WEIGHTING_SURFACE;
    }
    weight_info the_weight_info;
    the_weight_info.set_type(the_weight_type);
    the_weight_info.set_value(the_weight_value);
    if (the_weight_type == weight_info::WEIGHTING_VOLUME && density > 0) {
      // Total mass computed for homogeneous density:
      the_weight_info.set_mass(the_weight_value * density);
    }
    _set_total_weight (the_weight_info);

    // Normalize weight:
    for (size_t i = 0; i < _work_->entries.size (); i++) {
      _work_->entries[i].cumulated_weight /= _work_->entries.back ().cumulated_weight;
    }
    return;
  }

  void box_model_vg::initialize (const ::datatools::properties & setup_,
                                 ::datatools::service_manager & service_manager_,
                                 ::genvtx::vg_dict_type & /*vgens_*/)
  {
    DT_THROW_IF (is_initialized (), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");

    this->::genvtx::i_vertex_generator::_initialize(setup_, service_manager_);

    int mode = utils::MODE_INVALID;
    std::string origin_rules;
    utils::origin_invalidate (origin_rules);
    std::string mode_str;
    bool use_bb = false;
    bool surface_back   = false;
    bool surface_front  = false;
    bool surface_bottom = false;
    bool surface_top    = false;
    bool surface_left   = false;
    bool surface_right  = false;
    double lunit = CLHEP::mm;
    double skin_skip = 0.0 * CLHEP::mm;
    double skin_thickness = 0.0 * CLHEP::mm;

    if (setup_.has_key ("length_unit")) {
      std::string lunit_str = setup_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    DT_THROW_IF (!setup_.has_key ("origin"),
                 std::logic_error,
                 "Missing 'origin_rules' property in vertex generator '" << get_name() << "' !");
    origin_rules = setup_.fetch_string ("origin");

    if (setup_.has_key ("materials.plugin_name")) {
      std::string mpn;
      mpn = setup_.fetch_string ("materials.plugin_name");
      set_materials_plugin_name(mpn);
    }

    DT_THROW_IF (! setup_.has_key ("mode"),
                 std::logic_error,
                 "Missing 'mode' property in vertex generator '" << get_name() << "' !");
    mode_str = setup_.fetch_string ("mode");

    if (setup_.has_key("use_bounding_box")) {
      use_bb = setup_.fetch_boolean("use_bounding_box");
    }
    if (mode_str == "bulk") mode = utils::MODE_BULK;
    if (mode_str == "surface") mode = utils::MODE_SURFACE;

    if (mode == utils::MODE_SURFACE) {
      if (setup_.has_key ("mode.surface.back")) {
        surface_back = setup_.fetch_boolean ("mode.surface.back");
      }
      if (setup_.has_key ("mode.surface.front")) {
        surface_front = setup_.fetch_boolean ("mode.surface.front");
      }
      if (setup_.has_key ("mode.surface.bottom")) {
          surface_bottom = setup_.fetch_boolean ("mode.surface.bottom");
      }
      if (setup_.has_key ("mode.surface.top")) {
        surface_top = setup_.fetch_boolean ("mode.surface.top");
      }
      if (setup_.has_key ("mode.surface.left")) {
        surface_left = setup_.fetch_boolean ("mode.surface.left");
      }
      if (setup_.has_key ("mode.surface.right")) {
        surface_right = setup_.fetch_boolean ("mode.surface.right");
      }
      bool surface_all =
        surface_back || surface_front ||
        surface_bottom || surface_top ||
        surface_left || surface_right;
      DT_THROW_IF (! surface_all,std::logic_error,
                   "Missing some activated surface(s) property in vertex generator '" << get_name() << "' !");
    }

    if (setup_.has_key ("skin_skip")) {
      skin_skip = setup_.fetch_real ("skin_skip");
      if (! setup_.has_explicit_unit ("skin_skip")) skin_skip *= lunit;
    }

    if (setup_.has_key ("skin_thickness")) {
      skin_thickness = setup_.fetch_real ("skin_thickness");
      if (! setup_.has_explicit_unit ("skin_thickness")) skin_thickness *= lunit;
    }

    set_use_bounding_box(use_bb);
    set_skin_skip(skin_skip);
    set_skin_thickness(skin_thickness);
    set_origin_rules (origin_rules);
    set_mode (mode);
    if (is_mode_surface ()) {
      set_surface_back   (surface_back);
      set_surface_front  (surface_front);
      set_surface_bottom (surface_bottom);
      set_surface_top    (surface_top);
      set_surface_left   (surface_left);
      set_surface_right  (surface_right);
    }

    _init_ ();
    _initialized_ = true;
    return;
  }

  void box_model_vg::tree_dump (std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const
  {
    namespace du = datatools;
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    this->i_vertex_generator::tree_dump (out_, title_, indent_, true);
    out_ << indent << du::i_tree_dumpable::tag
         << "Mode           : '" << _mode_ << "'" << std::endl;
    if (is_mode_surface ())  {
      out_ << indent << du::i_tree_dumpable::tag
           << "Surface back   : " << _surface_back_ << std::endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Surface front  : " << _surface_front_ << std::endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Surface bottom : " << _surface_bottom_ << std::endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Surface top    : " << _surface_top_ << std::endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Surface left   : " << _surface_left_ << std::endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Surface right  : " << _surface_right_ << std::endl;
    }
    out_ << indent << du::i_tree_dumpable::tag
         << "Skin skip      : " << _skin_skip_ / CLHEP::mm << std::endl;
    out_ << indent << du::i_tree_dumpable::tag
         << "Skin thickness : " << _skin_thickness_ / CLHEP::mm << std::endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
         << "Origin rules   : '" << _origin_rules_ << "'" << std::endl;
    if (is_initialized() && _work_.get() != 0) {
      // out_ << indent << du::i_tree_dumpable::tag
      //      << "ID selector " << std::endl;
      out_ << indent << du::i_tree_dumpable::inherit_skip_tag (inherit_)
           << du::i_tree_dumpable::last_tag << "Entries : " << _work_->entries.size () << std::endl;
    }
    return;
  }

} // end of namespace genvtx

/***************
 * OCD support *
 ***************/

// OCD support' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::genvtx::box_model_vg,ocd_)
{
  ocd_.set_class_name("genvtx::box_model_vg");
  ocd_.set_class_description("A vertex generator from a box-shaped geometry model");
  ocd_.set_class_library("genvtx");
  // ocd_.set_class_documentation("");

  ::genvtx::i_vertex_generator::ocd_support(ocd_);

  //ocd_.set_configuration_hints("Nothing special.");
  ocd_.add_example("From the bulk volume of a collection of box volumes::          \n"
                   "                                                               \n"
                   "  length_unit : string = \"mm\"                                \n"
                   "  origin : string = \" category='source_pad' sector=0 pad=* \" \n"
                   "  mode   : string = \"bulk\"                                   \n"
                   "  skin_skip      : real = 0 mm                                 \n"
                   "  skin_thickness : real = 0 mm                                 \n"
                   "                                                               \n"
                   );
  ocd_.add_example("From some surfaces of a collection of box volumes::            \n"
                   "                                                               \n"
                   "  length_unit : string = \"mm\"                                \n"
                   "  origin : string = \" category='source_pad' sector=0 pad=* \" \n"
                   "  mode : string = \"surface\"                                  \n"
                   "  mode.surface.back   : boolean = 1                            \n"
                   "  mode.surface.front  : boolean = 1                            \n"
                   "  mode.surface.bottom : boolean = 0                            \n"
                   "  mode.surface.top    : boolean = 1                            \n"
                   "  mode.surface.left   : boolean = 0                            \n"
                   "  mode.surface.right  : boolean = 1                            \n"
                   "  skin_skip           : real = 0 mm                            \n"
                   "  skin_thickness      : real = 0 mm                            \n"
                   "                                                               \n"
                   );
  ocd_.add_example("From the bulk volume of the bounding boxes of a collection of spheres:: \n"
                   "                                                               \n"
                   "  origin : string = \" category='ball' item=* \"               \n"
                   "  mode   : string = \"bulk\"                                   \n"
                   "  use_bounding_box : boolean = true                            \n"
                   "                                                               \n"
                   );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(genvtx::box_model_vg,"genvtx::box_model_vg")
