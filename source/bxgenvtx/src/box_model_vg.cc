// -*- mode: c++ ; -*-
/* box_model_vg.cc
 */

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <limits>

#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <mygsl/rng.h>

#include <materials/manager.h>

#include <geomtools/geom_info.h>
#include <geomtools/logical_volume.h>
#include <geomtools/i_shape_3d.h>
#include <geomtools/box.h>
#include <geomtools/manager.h>
#include <geomtools/mapping.h>
#include <geomtools/mapping_plugin.h>
#include <geomtools/materials_plugin.h>

#include <genvtx/utils.h>
#include <genvtx/box_model_vg.h>
#include <genvtx/detail/geom_manager_utils.h>

namespace genvtx {

  GENVTX_VG_REGISTRATION_IMPLEMENT(box_model_vg,"genvtx::box_model_vg");

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
    _assert_lock ("genvtx::box_model_vg::set_surface_back");
    _surface_back_ = s_;
    return;
  }

  void box_model_vg::set_surface_front (bool s_)
  {
    _assert_lock ("genvtx::box_model_vg::set_surface_front");
    _surface_front_ = s_;
    return;
  }

  void box_model_vg::set_surface_bottom (bool s_)
  {
    _assert_lock ("genvtx::box_model_vg::set_surface_bottom");
    _surface_bottom_ = s_;
    return;
  }

  void box_model_vg::set_surface_top (bool s_)
  {
    _assert_lock ("genvtx::box_model_vg::set_surface_top");
    _surface_top_ = s_;
    return;
  }

  void box_model_vg::set_surface_left (bool s_)
  {
    _assert_lock ("genvtx::box_model_vg::set_surface_left");
    _surface_left_ = s_;
    return;
  }

  void box_model_vg::set_surface_right (bool s_)
  {
    _assert_lock ("genvtx::box_model_vg::set_surface_right");
    _surface_right_ = s_;
    return;
  }

  const std::string & box_model_vg::get_origin_rules () const
  {
    return _origin_rules_;
  }

  void box_model_vg::set_origin_rules (const std::string & origin_rules_)
  {
    _assert_lock ("genvtx::box_model_vg::set_origin_rules");
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
    if (is_initialized())
      {
        throw std::logic_error ("genvtx::box_model_vg::set_mapping_plugin_name: Already initialized !");
      }
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
    if (is_initialized())
      {
        throw std::logic_error ("genvtx::box_model_vg::set_materials_plugin_name: Already initialized !");
      }
    _materials_plugin_name_ = mpn_;
    return;
  }

  int box_model_vg::get_mode () const
  {
    return _mode_;
  }

  void box_model_vg::set_mode (int mode_)
  {
    _assert_lock ("genvtx::box_model_vg::set_mode");
    if ((mode_ == utils::MODE_BULK)
        ||
        (mode_ == utils::MODE_SURFACE))
      {
        _mode_ = mode_;
      }
    else
      {
        _mode_ = utils::MODE_INVALID;
      }
    return;
  }

  // ctor:
  GENVTX_VG_CONSTRUCTOR_IMPLEMENT_HEAD(box_model_vg)
  {
    _initialized_ = false;
    _set_defaults_ ();
    return;
  }

  GENVTX_VG_IS_INITIALIZED_IMPLEMENT_HEAD(box_model_vg)
  {
    return _initialized_;
  }

  // Destructor :
  GENVTX_VG_DEFAULT_DESTRUCTOR_IMPLEMENT(box_model_vg)

  void box_model_vg::_set_defaults_ ()
  {
    // internal reset:
    utils::origin_invalidate (_origin_rules_);
    _mode_           = utils::MODE_INVALID;
    _surface_back_   = false;
    _surface_front_  = false;
    _surface_bottom_ = false;
    _surface_top_    = false;
    _surface_left_   = false;
    _surface_right_  = false;
    if (_box_vg_.is_initialized()) _box_vg_.reset ();
    _origin_rules_.clear();
    _mapping_plugin_name_.clear();
    _materials_plugin_name_.clear();
    _entries_.clear ();
    _src_selector_.reset ();
    this->i_vertex_generator::_reset ();
    return;
  }

  void box_model_vg::_reset_ ()
  {
    _set_defaults_ ();
    return;
  }

  GENVTX_VG_RESET_IMPLEMENT_HEAD(box_model_vg)
  {
    if (! is_initialized ())
      {
        throw std::logic_error ("genvtx::box_model_vg: Not initialized !");
      }
    _reset_ ();
    _initialized_ = false;
    return;
  }

  GENVTX_VG_SHOOT_VERTEX_IMPLEMENT_HEAD(box_model_vg,random_,vertex_)
  {
    if (! _initialized_)
      {
        std::ostringstream message;
        message << "genvtx::box_model_vg::_shoot_vertex: "
                << "Generator is not initialized !";
        throw std::logic_error (message.str ());
      }
    geomtools::invalidate (vertex_);
    this->_shoot_vertex_boxes (random_, vertex_);
    return;
  }

  void box_model_vg::_shoot_vertex_boxes (mygsl::rng & random_,
                                            geomtools::vector_3d & vertex_)
  {
    double ran_w = random_.uniform ();
    int index = -1;
    for (int i = 0; i < _entries_.size (); i++)
      {
        if (ran_w <= _entries_[i].cumulated_weight)
          {
            index = i;
            break;
          }
      }
    if (index < 0)
      {
        std::ostringstream message;
        message << "genvtx::box_model_vg::_shoot_vertex_boxes: "
                << "Cannot determine the volume location index !";
        throw std::logic_error (message.str ());
      }
    geomtools::vector_3d src_vtx;
    _box_vg_.shoot_vertex (random_, src_vtx);

    const geomtools::placement & world_plct
      = _entries_[index].ginfo->get_world_placement ();
    world_plct.child_to_mother (src_vtx, vertex_);
    return;
  }

  void box_model_vg::_init_ ()
  {
    if (! is_mode_valid ())
      {
        std::ostringstream message;
        message << "genvtx::box_model_vg::initialize: "
                << "Invalid mode !";
        throw std::logic_error (message.str ());
      }

    if (! has_geom_manager ())
      {
        std::ostringstream message;
        message << "genvtx::box_model_vg::initialize: "
                << "Missing geometry manager !";
        throw std::logic_error (message.str ());
      }

    const geomtools::mapping * mapping_ptr 
      = detail::access_geometry_mapping(get_geom_manager (), _mapping_plugin_name_);
    if (mapping_ptr == 0)
      {
        std::ostringstream message;
        message << "genvtx::box_model_vg::initialize: "
                << "No available geometry mapping was found !";
        throw std::logic_error (message.str ());                        
      }

    _src_selector_.set_id_mgr (get_geom_manager ().get_id_mgr ());
    _src_selector_.initialize (_origin_rules_);
    //_src_selector_.dump (clog, "genvtx::box_model_vg::initialize: ID selector:");

    const geomtools::mapping & the_mapping = *mapping_ptr;
    const geomtools::geom_info_dict_type & geom_infos = the_mapping.get_geom_infos ();
    std::list<weight_entry_type> entries;
    for (geomtools::geom_info_dict_type::const_iterator i
           = geom_infos.begin ();
         i != geom_infos.end ();
         i++)
      {
        const geomtools::geom_id & gid = i->first;
        if (_src_selector_.match (gid))
          {
            const geomtools::geom_info * ginfo = &(i->second);
            weight_entry_type e;
            e.weight = 0.0;
            e.cumulated_weight = 0.0;
            e.ginfo = ginfo;
            entries.push_back (e);
          }
      }
    if (entries.size () == 0)
      {
        std::ostringstream message;
        message << "genvtx::box_model_vg::initialize: "
                << "Cannot compute any source of vertex !";
        throw std::logic_error (message.str ());
      }

    weight_entry_type dummy;
    _entries_.assign (entries.size (), dummy);
    std::copy (entries.begin (), entries.end (), _entries_.begin ());

    const geomtools::logical_volume * src_log = 0;
    for (int i = 0; i < _entries_.size (); i++)
      {
        if (is_debug ())
          {
            std::clog << datatools::io::debug
                      << "genvtx::box_model_vg::initialize: "
                      << "ID #" << i << " = " << _entries_[i].ginfo->get_id ()
                      << std::endl;
          }
        if (src_log == 0)
          {
            src_log = &_entries_[i].ginfo->get_logical ();
          }
        else
          {
            if (src_log != &_entries_[i].ginfo->get_logical ())
              {
                std::ostringstream message;
                message << "genvtx::box_model_vg::initialize: "
                        << "Vertex location with different logical geometry volume are not supported  (different shapes or materials) !";
                throw std::logic_error (message.str ());
              }
          }
      }

    // Attempt to extract material info :
    double density = -1.0;
    if (is_mode_bulk () 
        && src_log->has_material_ref () 
        && has_materials_plugin_name())
      {
        std::string material_name = src_log->get_material_ref ();
        const materials::manager * mat_mgr_ptr 
          = detail::access_materials_manager(get_geom_manager (),
                                             _materials_plugin_name_);
        if (mat_mgr_ptr == 0) {
          std::ostringstream message;
          message << "genvtx::box_model_vg::initialize: "
                  << "No geometry materials plugin named '" << _materials_plugin_name_  
                  << "' available from the geometry manager !";
          throw std::logic_error (message.str ());
        }
        const materials::manager & mat_mgr = *mat_mgr_ptr;
        materials::material_dict_type::const_iterator found =
          mat_mgr.get_materials ().find (material_name);
        if (found != mat_mgr.get_materials ().end ())
          {
            if (found->second.has_ref ())
              {
                const materials::material & the_material = found->second.get_ref ();
                if (is_debug ())
                  {
                    the_material.tree_dump (std::clog, "genvtx::box_model_vg::initialize: Material : ", "DEBUG: ");
                  }
                density = the_material.get_density ();
              }
          }
      }

    int surface_mask = 0;
    if (is_mode_surface ())
      {
        _box_vg_.set_mode (utils::MODE_SURFACE);
        if (_surface_back_)   surface_mask |= geomtools::box::FACE_BACK;
        if (_surface_front_)  surface_mask |= geomtools::box::FACE_FRONT;
        if (_surface_bottom_) surface_mask |= geomtools::box::FACE_BOTTOM;
        if (_surface_top_)    surface_mask |= geomtools::box::FACE_TOP;
        if (_surface_left_)   surface_mask |= geomtools::box::FACE_LEFT;
        if (_surface_right_)  surface_mask |= geomtools::box::FACE_RIGHT;
        _box_vg_.set_surface_mask (surface_mask);
      }
    else
      {
        _box_vg_.set_mode (utils::MODE_BULK);
      }
    if (! src_log->has_shape ())
      {
        std::ostringstream message;
        message << "genvtx::box_model_vg::initialize: "
                << "Logical '" << src_log->get_name () << "' has "
                << "no shape !";
        throw std::logic_error (message.str ());
      }
    const geomtools::i_shape_3d & src_shape = src_log->get_shape ();
    if (src_shape.get_shape_name () != "box")
      {
        std::ostringstream message;
        message << "genvtx::box_model_vg::initialize: "
                << "Shape is '" << src_shape.get_shape_name () << "' but "
                << "only box shape is supported !";
        throw std::logic_error (message.str ());
      }
    const geomtools::box * box_shape
      = dynamic_cast<const geomtools::box *> (&src_shape);
    if (is_debug ())
      {
        box_shape->tree_dump (std::clog, "Box model VG: box_shape == ", "DEBUG: ");
      }
    _box_vg_.set_box_ref (*box_shape);
    _box_vg_.initialize_simple ();
    if (is_debug ())
      {
        _box_vg_.get_box_safe ().tree_dump (std::clog, "Box model VG box: ", "DEBUG: ");
      }
    double weight = 0.0;
    if (is_mode_surface ())
      {
        weight = box_shape->get_surface (surface_mask);
      }
    else
      {
        weight = box_shape->get_volume ();
      }

    if (is_debug ())
      {
        std::clog << datatools::io::debug
                  << "genvtx::box_model_vg::initialize: "
                  << "Weight = " << weight << std::endl;
      }
    // compute weight:
    _entries_[0].cumulated_weight = _entries_[0].weight;
    for (int i = 0; i < _entries_.size (); i++)
      {
        _entries_[i].weight = weight;
        double cumul = 0.0;
        if (i > 0) cumul = _entries_[i - 1].cumulated_weight;
        _entries_[i].cumulated_weight = cumul + _entries_[i].weight;
      }

    // store the total weight before normalization for alternative use :
    double the_weight_value = _entries_.back ().cumulated_weight;
    int the_weight_type = weight_info::WEIGHTING_VOLUME;
    if (is_mode_surface ())
      {
        the_weight_type = weight_info::WEIGHTING_SURFACE;
      }
    weight_info the_weight_info;
    the_weight_info.type = the_weight_type;
    the_weight_info.value = the_weight_value;
    if (the_weight_type == weight_info::WEIGHTING_VOLUME && density > 0)
      {
        // total mass computed for homogeneous density and
        the_weight_info.mass = the_weight_value * density;
      }
    _set_total_weight (the_weight_info);

    // normalize weight:
    for (int i = 0; i < _entries_.size (); i++)
      {
        _entries_[i].cumulated_weight /= _entries_.back ().cumulated_weight;
        if (is_debug ())
          {
            std::clog << datatools::io::debug
                      << "genvtx::box_model_vg::initialize: "
                      << "Cumulated weight for ID " << _entries_[i].ginfo->get_id ()
                      << " = " << _entries_[i].cumulated_weight << std::endl;
          }
      }
    return;
  }

  GENVTX_VG_INITIALIZE_IMPLEMENT_HEAD(box_model_vg,setup_,service_manager_,vgens_)
  {
    if (is_debug ()) std::cerr << "DEBUG: genvtx::box_model_vg::initialize: Entering..." << std::endl;
    using namespace std;
    bool devel = false;
    if (is_initialized ())
      {
        throw std::logic_error ("genvtx::box_model_vg::initialize: Already initialized !");
      }

    GENVTX_VG_INITIALIZE_BASICS_INVOKE(setup_,service_manager_);
    GENVTX_VG_INITIALIZE_GEO_MANAGER_INVOKE(setup_,service_manager_);

    int mode = utils::MODE_INVALID;
    std::string origin_rules;
    utils::origin_invalidate (origin_rules);
    std::string mode_str;
    bool surface_back   = false;
    bool surface_front  = false;
    bool surface_bottom = false;
    bool surface_top    = false;
    bool surface_left   = false;
    bool surface_right  = false;

    if (setup_.has_key ("origin"))
      {
        origin_rules = setup_.fetch_string ("origin");
      }
    else
      {
        std::ostringstream message;
        message << "genvtx::box_model_vg::create: "
                << "Missing 'origin_rules' property !";
        throw std::logic_error (message.str ());
      }

    if (setup_.has_key ("materials.plugin_name"))
      {
        std::string mpn;
        mpn = setup_.fetch_string ("materials.plugin_name");
        set_materials_plugin_name(mpn);
      }

    if (setup_.has_key ("mode"))
      {
        mode_str = setup_.fetch_string ("mode");
      }
    else
      {
        std::ostringstream message;
        message << "genvtx::box_model_vg::create: "
                << "Missing 'mode' property !";
        throw std::logic_error (message.str ());
      }
    if (mode_str == "bulk") mode = utils::MODE_BULK;
    if (mode_str == "surface") mode = utils::MODE_SURFACE;

    if (mode == utils::MODE_SURFACE)
      {
        if (setup_.has_key ("mode.surface.back"))
          {
            surface_back = setup_.fetch_boolean ("mode.surface.back");
          }
        if (setup_.has_key ("mode.surface.front"))
          {
            surface_front = setup_.fetch_boolean ("mode.surface.front");
          }
        if (setup_.has_key ("mode.surface.bottom"))
          {
            surface_bottom = setup_.fetch_boolean ("mode.surface.bottom");
          }
        if (setup_.has_key ("mode.surface.top"))
          {
            surface_top = setup_.fetch_boolean ("mode.surface.top");
          }
        if (setup_.has_key ("mode.surface.left"))
          {
            surface_left = setup_.fetch_boolean ("mode.surface.left");
          }
        if (setup_.has_key ("mode.surface.right"))
          {
            surface_right = setup_.fetch_boolean ("mode.surface.right");
          }
        bool surface_all =
          surface_back || surface_front ||
          surface_bottom || surface_top ||
          surface_left || surface_right;
        if (! surface_all )
          {
            std::ostringstream message;
            message << "genvtx::box_model_vg::create: "
                    << "Missing some activated surface(s) property !";
            throw std::logic_error (message.str ());
          }
      }

    set_origin_rules (origin_rules);
    set_mode (mode);
    if (is_mode_surface ())
      {
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
    if (is_mode_surface ())
      {
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
         << "Origin rules   : '" << _origin_rules_ << "'" << std::endl;
    out_ << indent << du::i_tree_dumpable::tag
         << "ID selector " << std::endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
         << "Entries        : " << _entries_.size () << std::endl;
    return;
  }

} // end of namespace genvtx

// end of box_model_vg.cc
