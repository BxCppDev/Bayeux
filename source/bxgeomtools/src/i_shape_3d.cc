/// \file geomtools/i_shape_3d.cc

// Ourselves:
#include <geomtools/i_shape_3d.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/units.h>

// This project:
#include <geomtools/utils.h>
#include <geomtools/i_shape_2d.h>
#include <geomtools/box.h>

namespace geomtools {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(i_shape_3d, "geomtools::i_shape_3d")

  using namespace std;

  // static
  const double i_shape_3d::DEFAULT_SKIN      = GEOMTOOLS_DEFAULT_TOLERANCE;
  const double i_shape_3d::ZERO_SKIN         = GEOMTOOLS_ZERO_TOLERANCE;
  const double i_shape_3d::USING_PROPER_SKIN = GEOMTOOLS_PROPER_TOLERANCE;

  // static
  const std::string & i_shape_3d::volume_key()
  {
    static const std::string _key("shape_3d.volume");
    return _key;
  }

  // static
  const std::string & i_shape_3d::surface_key()
  {
    static const std::string _key("shape_3d.surface");
    return _key;
  }

  // static
  double i_shape_3d::get_default_skin()
  {
    return GEOMTOOLS_DEFAULT_TOLERANCE;
  }

  // static
  double i_shape_3d::get_zero_skin()
  {
    return GEOMTOOLS_ZERO_TOLERANCE;
  }

  // static
  double i_shape_3d::get_proper_skin()
  {
    return GEOMTOOLS_PROPER_TOLERANCE;
  }

  bool i_shape_3d::has_number_of_faces() const
  {
    return get_number_of_faces() > 0;
  }

  unsigned int i_shape_3d::get_number_of_faces() const
  {
    return 0;
  }

  bool i_shape_3d::volume_can_be_forced() const
  {
    return false;
  }

  bool i_shape_3d::has_forced_volume() const
  {
    return _forced_volume_ != boost::none;
  }

  void i_shape_3d::set_forced_volume(double forced_volume_)
  {
    DT_THROW_IF(!volume_can_be_forced(),
                std::logic_error,
                "This shape does not accept forced volume!");
    _forced_volume_ = forced_volume_;
    return;
  }

  double i_shape_3d::get_forced_volume() const
  {
    DT_THROW_IF(!_forced_volume_, std::logic_error, "Shape has no forced volume!");
    return _forced_volume_.get();
  }

  bool i_shape_3d::has_volume(uint32_t flags_) const
  {
    /*
    if (get_auxiliaries().has_key(volume_key())) {
      double v = get_auxiliaries().fetch_real(volume_key());
      if (! get_auxiliaries().has_explicit_unit(volume_key())) {
        // v *= 1.0; // No default volume unit;
      }
      return datatools::is_valid(v);
    }
    */
    return datatools::is_valid(get_volume(flags_));
  }

  double i_shape_3d::get_volume(uint32_t /*flags_*/) const
  {
    /*
    if (get_auxiliaries().has_key(volume_key())) {
      double v = get_auxiliaries().fetch_real(volume_key());
      if (! get_auxiliaries().has_explicit_unit(volume_key())) {
        // v *= 1.0; // No default volume unit;
      }
      return v;
    }
    */
    return std::numeric_limits<double>::quiet_NaN();
  }

  double i_shape_3d::get_effective_volume() const
  {
    if (has_forced_volume()) {
      return get_forced_volume();
    }
    return get_volume();
  }

  bool i_shape_3d::has_surface(uint32_t flags_) const
  {
    /*
    if (get_auxiliaries().has_key(surface_key())) {
      double v = get_auxiliaries().fetch_real(surface_key());
      if (! get_auxiliaries().has_explicit_unit(surface_key())) {
        //v *= 1.0; // No default surface unit;
      }
      return datatools::is_valid(v);
    }
    */
    return datatools::is_valid(get_surface(flags_));
  }

  double i_shape_3d::get_surface(uint32_t /*flags_*/) const
  {
    /*
    if (get_auxiliaries().has_key(surface_key())) {
      double v = get_auxiliaries().fetch_real(surface_key());
      if (! get_auxiliaries().has_explicit_unit(surface_key())) {
        // v *= 1.0; // No default surface unit;
      }
      return v;
    }
    */
    return std::numeric_limits<double>::quiet_NaN();
  }

  bool i_shape_3d::is_composite() const
  {
    return false;
  }

  bool i_shape_3d::using_face_id_bits() const
  {
    return true;
  }

  bool i_shape_3d::using_face_id_index() const
  {
    return false;
  }

  bool i_shape_3d::using_face_id_part_index() const
  {
    return false;
  }

  bool i_shape_3d::is_native_stackable() const
  {
    const i_stackable * the_stackable
      = dynamic_cast<const i_stackable *>(this);
    return the_stackable != nullptr;
  }

  // // static
  // bool i_shape_3d::is_stackable(const i_shape_3d & a_shape)
  // {
  //   if (a_shape.has_stackable_data()) {
  //     if (a_shape.get_stackable_data().is_valid()) {
  //       return true;
  //     } else {
  //       DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
  //                      "Stackable data is invalid for shape '"
  //                      << a_shape.get_shape_name() << "'!");
  //     }
  //   }
  //   return a_shape.is_native_stackable();
  // }

  // static
  bool i_shape_3d::pickup_stackable(const i_shape_3d & a_shape,
                                    stackable_data & a_stackable_data)
  {
    stackable_data sd;
    const i_stackable * the_stackable
      = dynamic_cast<const i_stackable *> (&a_shape);
    if (the_stackable != nullptr) {
      // First initialize the SD with native stackability informations from the stackable shape itself:
      sd.xmin = the_stackable->get_xmin();
      sd.xmax = the_stackable->get_xmax();
      sd.ymin = the_stackable->get_ymin();
      sd.ymax = the_stackable->get_ymax();
      sd.zmin = the_stackable->get_zmin();
      sd.zmax = the_stackable->get_zmax();
    }
    if (a_shape.has_stackable_data()) {
      // Then overwrite stacking informations with enforced plugged stackable data, if any:
      if (a_shape._stackable_data_->has_xmin()) {
        sd.xmin = a_shape._stackable_data_->get_xmin();
      }
      if (a_shape._stackable_data_->has_xmax()) {
        sd.xmax = a_shape._stackable_data_->get_xmax();
      }
      if (a_shape._stackable_data_->has_ymin()) {
        sd.ymin = a_shape._stackable_data_->get_ymin();
      }
      if (a_shape._stackable_data_->has_ymax()) {
        sd.ymax = a_shape._stackable_data_->get_ymax();
      }
      if (a_shape._stackable_data_->has_zmin()) {
        sd.zmin = a_shape._stackable_data_->get_zmin();
      }
      if (a_shape._stackable_data_->has_zmax()) {
        sd.zmax = a_shape._stackable_data_->get_zmax();
      }
    }
    a_stackable_data = sd;
    return a_stackable_data.is_valid_very_weak();
  }

  // static
  bool i_shape_3d::check_stackability(const i_shape_3d & shape_, const stackable::stackability_mode sm_)
  {
    stackable_data sd;
    pickup_stackable(shape_, sd);
    return sd.check(sm_);
  }

  double i_shape_3d::get_skin(double a_skin) const
  {
    return compute_tolerance(a_skin);
  }

  double i_shape_3d::get_skin() const
  {
    return get_tolerance();
  }

  void i_shape_3d::set_skin(double a_skin)
  {
    set_tolerance(a_skin);
  }

  int i_shape_3d::get_dimensional() const
  {
    return DIMENSIONAL_3;
  }

  bool i_shape_3d::has_stackable_data() const
  {
    return _stackable_data_ != nullptr;
  }

  bool i_shape_3d::owns_stackable_data () const
  {
    return has_stackable_data() && _owns_stackable_data_;
  }

  const stackable_data & i_shape_3d::get_stackable_data() const
  {
    DT_THROW_IF(!has_stackable_data(), std::logic_error, "No plugged stackable data!");
    return *_stackable_data_;
  }

  void i_shape_3d::reset_stackable_data()
  {
    if (_stackable_data_ != nullptr) {
      if (_owns_stackable_data_) {
        delete _stackable_data_;
      }
      _stackable_data_ = nullptr;
    }
    _owns_stackable_data_ = false;
    return;
  }

  void i_shape_3d::set_stackable_data(const stackable_data & a_stackable_data)
  {
    if (&a_stackable_data == _stackable_data_) return;
    reset_stackable_data();
    _owns_stackable_data_ = false;
    _stackable_data_ = &a_stackable_data;
    return;
  }

  void i_shape_3d::set_stackable_data(const stackable_data * a_stackable_data)
  {
    if (a_stackable_data == _stackable_data_) return;
    reset_stackable_data();
    _owns_stackable_data_ = true;
    _stackable_data_ = a_stackable_data;
    return;
  }

  void i_shape_3d::_set_defaults()
  {
    _locked_ = false;
    _owns_stackable_data_ = false;
    _stackable_data_ = nullptr;
    return;
  }

  i_shape_3d::i_shape_3d() : i_object_3d()
  {
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "CTOR=" << this);
    _set_defaults();
    return;
  }

  i_shape_3d::i_shape_3d(double a_skin) : i_object_3d(a_skin)
  {
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "CTOR=" << this);
    _set_defaults();
    return;
  }

  i_shape_3d::i_shape_3d(double a_skin,
                         double angular_tolerance_)
    : i_object_3d(a_skin, angular_tolerance_)
  {
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "CTOR=" << this);
    _set_defaults();
    return;
  }

  i_shape_3d::i_shape_3d(const i_shape_3d & src_) : i_object_3d(src_)
  {
    _set_defaults();
    return;
  }

  i_shape_3d & i_shape_3d::operator=(const i_shape_3d & src_)
  {
    if (this == &src_) {
      return *this;
    }
    reset();
    this->i_object_3d::operator=(src_);
    return *this;
  }

  i_shape_3d::~i_shape_3d ()
  {
    // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "DTOR=" << this);
    reset_stackable_data();
    return;
  }

  void i_shape_3d::initialize(const datatools::properties & config_,
                               const handle_dict_type * objects_)
  {
    this->i_shape_3d::_initialize(config_, objects_);
    return;
  }

  void i_shape_3d::reset()
  {
    this->i_shape_3d::_reset();
    return;
  }

  void i_shape_3d::_initialize(const datatools::properties & config_,
                               const handle_dict_type * objects_)
  {
    this->i_object_3d::_initialize(config_, objects_);
    _initialize_bounding_data(config_);
    _initialize_stackable_data(config_);

    double default_volume_unit = CLHEP::millimeter3;

    if (config_.has_key("volume_unit")) {
      const std::string ustr = config_.fetch_string("volume_unit");
      default_volume_unit = datatools::units::get_volume_unit_from(ustr);
    }

    if (config_.has_key("forced_volume")) {
      double v = config_.fetch_real("forced_volume");
      if (!config_.has_explicit_unit("forced_volume")) {
        v *= default_volume_unit;
      }
      set_forced_volume(v);
    }
    return;
  }

  void i_shape_3d::_initialize_stackable_data(const datatools::properties & config_)
  {
    if (config_.has_flag("enforce_stackable_data")) {
      stackable_data * ptr_sd = new stackable_data;
      try {
        ptr_sd->initialize(config_);
        set_stackable_data(ptr_sd);
      } catch (std::exception & error) {
        delete ptr_sd;
        DT_THROW(std::logic_error,
                 "Invalid stackable data for shape of type '" << get_shape_name() << "': "
                 << error.what());
      }
    }
    return;
  }

  void i_shape_3d::_initialize_bounding_data(const datatools::properties & config_)
  {
    if (config_.has_flag("enforce_bounding_data")) {
      datatools::properties bounding_data_config;
      config_.export_and_rename_starting_with(bounding_data_config,
                                              "bounding_data.",
                                              "");
      _bounding_data_.parse_bounding_data(bounding_data_config);
      DT_THROW_IF(!_bounding_data_.is_valid(), std::logic_error,
                  "Invalid bounding data for shape of type '" << get_shape_name() << "'!");
    }
    return;
  }

  void i_shape_3d::_reset_bounding_data()
  {
    _bounding_data_.reset();
    return;
  }

  void i_shape_3d::_reset()
  {
    if (is_locked()) {
      unlock();
    }
    reset_stackable_data();
    if (_forced_volume_) {
      _forced_volume_ = boost::none;
    }
    reset_computed_faces();
    _set_defaults();
    this->i_object_3d::_reset();
    return;
  }

  bool i_shape_3d::has_bounding_data() const
  {
    return _bounding_data_.is_valid();
  }

  void i_shape_3d::reset_bounding_data()
  {
    _bounding_data_.reset();
    return;
  }

  const bounding_data & i_shape_3d::get_bounding_data() const
  {
    if (!has_bounding_data()) {
      i_shape_3d * mutable_this = const_cast<i_shape_3d *>(this);
      mutable_this->_build_bounding_data();
    }
    return _bounding_data_;
  }

  void i_shape_3d::set_bounding_data(const bounding_data & bd_)
  {
    DT_THROW_IF(is_locked(), std::logic_error,
                "Shape '" << get_shape_name() << "' is locked!");
    _bounding_data_ = bd_;
    return;
  }

  bounding_data & i_shape_3d::_grab_bounding_data()
  {
    return _bounding_data_;
  }

  // virtual
  void i_shape_3d::make_any_face(face_identifier & fid_) const
  {
    fid_.invalidate();
    if (using_face_id_bits()) {
      fid_.set_face_bits_any();
    }
    if (using_face_id_index()) {
      fid_.set_face_index_any();
    }
    if (is_composite()) {
      // Let the FID invalid
    }
    return;
  }

  bool i_shape_3d::check_inside(const vector_3d & position_, double skin_) const
  {
    double skin = get_skin(skin_);

    if (has_bounding_data()) {
      double double_skin = 2 * skin;
      const bounding_data & bd = get_bounding_data();
      box bb;
      placement bb_placement;
      bd.compute_bounding_box(bb, bb_placement);
      vector_3d position_in_bb;
      bb_placement.mother_to_child(position_, position_in_bb);
      if (bb.is_outside(position_in_bb, double_skin)) {
        // If the point is outside the BB using a safety skin margin,
        // then it cannot be inside the solid:
        return false;
      }
    }

    if (is_inside(position_, skin)) {
      // If the point is detected to be inside the solid through the primitive
      // is_inside method, then it is accepted:
      // std::cerr << "DEVEL: i_shape_3d::check_inside: " << "is_inside" << std::endl;
      return true;
    }

    // Questionable...
    if (!is_outside(position_, 0.0)) {
      face_identifier any_face_id;
      make_any_face(any_face_id);
      if (!is_on_surface(position_, any_face_id, skin)) {
        // std::cerr << "DEVEL: i_shape_3d::check_inside: " << "!is_outside && !is_on_surface" << std::endl;
        return true;
      }
    }

    return false;
  }


  bool i_shape_3d::check_outside(const vector_3d & position_, double skin_) const
  {
    double skin = get_skin(skin_);
    if (has_bounding_data()) {
      double double_skin = 2 * skin;
      const bounding_data & bd = get_bounding_data();
      box bb;
      placement bb_placement;
      bd.compute_bounding_box(bb, bb_placement);
      vector_3d position_in_bb;
      bb_placement.mother_to_child(position_, position_in_bb);
      if (bb.is_outside(position_in_bb, double_skin)) {
        // If the point is outside the BB using a safety skin margin,
        // then it is automatically outside the solid:
        return true;
      }
    }

    // Then invoke the primitive "is_outside" method
    if (is_outside(position_, skin_)) {
      return true;
    }

    // Questionable...
    if (!is_inside(position_, 0.0)) {
      face_identifier any_face_id;
      make_any_face(any_face_id);
      if (!is_on_surface(position_, any_face_id, skin)) {
        return true;
      }
    }

    return false;
  }

  face_identifier i_shape_3d::check_surface(const vector_3d & position_,
                                            const face_identifier & surface_mask_,
                                            double skin_) const
  {
    double skin = get_skin(skin_);
    if (has_bounding_data()) {
      double double_skin = 2 * skin;
      const bounding_data & bd = get_bounding_data();
      box bb;
      placement bb_placement;
      bd.compute_bounding_box(bb, bb_placement);
      vector_3d position_in_bb;
      bb_placement.mother_to_child(position_, position_in_bb);
      if (bb.is_outside(position_in_bb, double_skin)) {
        // If the point is outside the BB using a safety skin margin,
        // then it cannot be on the surface:
        return face_identifier::face_invalid();
      }
    }

    return on_surface(position_, surface_mask_, skin);
  }

  bool i_shape_3d::check_surface(const vector_3d & position_, double skin_) const
  {
    double skin = get_skin(skin_);
    if (has_bounding_data()) {
      double double_skin = 2 * skin;
      const bounding_data & bd = get_bounding_data();
      box bb;
      placement bb_placement;
      bd.compute_bounding_box(bb, bb_placement);
      vector_3d position_in_bb;
      bb_placement.mother_to_child(position_, position_in_bb);
      if (bb.is_outside(position_in_bb, double_skin)) {
        // If the point is outside the BB using a safety skin margin,
        // then it cannot be on the surface:
        return false;
      }
    }

    {
      face_identifier any_face_id;
      make_any_face(any_face_id);
      if (is_on_surface(position_, any_face_id, skin)) {
        return true;
      }
    }

    return false;
  }

  shape_domain_flags_type i_shape_3d::where_is(const vector_3d & position_, double skin_) const
  {
    double skin = get_skin(skin_);
    try {
      if (check_outside(position_, skin)) {
        return SHAPE_DOMAIN_OUTSIDE;
      } else if (check_surface(position_, skin)) {
        return SHAPE_DOMAIN_ON_SURFACE;
      } else if (check_inside(position_, skin)) {
        return SHAPE_DOMAIN_INSIDE;
      } else {
        DT_THROW(std::logic_error, "Cannot classify position wrt the shape!");
      }
    } catch (std::exception & error) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   "Cannot determine the domain for position " << position_ << " w/r to shape '"
                   << this->get_shape_name() << "' : "
                   << error.what());
    } catch (...) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   "Cannot determine the domain for position " << position_ << " w/r to shape '"
                   << this->get_shape_name() << "' !");
    }
    return SHAPE_DOMAIN_NONE;
  }

  // virtual
  bool i_shape_3d::is_on_surface(const vector_3d & a_point,
                                 const face_identifier & a_surface_mask,
                                 double a_skin) const
  {
    return on_surface(a_point, a_surface_mask, a_skin).is_valid();
  }

  void i_shape_3d::tree_dump(ostream & a_out,
                             const string & a_title,
                             const string & a_indent,
                             bool a_inherit) const
  {
    i_object_3d::tree_dump(a_out, a_title, a_indent, true);

    {
      a_out << a_indent << datatools::i_tree_dumpable::tag
            << "Bounding data: "
            << (has_bounding_data() ? "<yes>" : "<none>")
            << std::endl;
      std::ostringstream indent2_oss;
      indent2_oss << a_indent << datatools::i_tree_dumpable::skip_tag;
      _bounding_data_.tree_dump(a_out, "", indent2_oss.str());
    }

    if (i_shape_3d::check_stackability(*this, stackable::STACKABILITY_VERY_WEAK)) {
      stackable_data SD;
      i_shape_3d::pickup_stackable(*this, SD);
      a_out << a_indent << datatools::i_tree_dumpable::tag
            << "Stackable data : ";
      if (has_stackable_data()) {
        a_out << "[plugged]";
      } else {
        a_out << "[native]";
      }
      a_out << endl;
      ostringstream indent_oss;
      indent_oss << a_indent;
      indent_oss << datatools::i_tree_dumpable::skip_tag;
      SD.tree_dump(a_out, "", indent_oss.str());
    }

    {
      a_out << a_indent << datatools::i_tree_dumpable::tag
            << "Computed faces: ";
      if (!has_computed_faces()) {
        a_out << "<none>";
      } else {
        a_out << _computed_faces_.get().size();
      }
      a_out << std::endl;
      // if (has_computed_faces()) {
      // }
    }

    a_out << a_indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
      << "Locked : " << (_locked_? "<yes>": "<none>") << endl;

    return;
  }

  bool i_shape_3d::is_locked() const
  {
    return _locked_;
  }

  void i_shape_3d::lock()
  {
    DT_LOG_DEBUG(get_logging_priority(), "Locking shape type '" << get_shape_name() << "'");
    if (is_locked()) return;
    _at_lock();
    _locked_ = true;
    return;
  }

  void i_shape_3d::unlock()
  {
   DT_LOG_DEBUG(get_logging_priority(), "Unlocking shape type '" << get_shape_name() << "'");
   if (!is_locked()) return;
    _locked_ = false;
    _at_unlock();
    return;
  }

  void i_shape_3d::_at_lock()
  {
    DT_LOG_DEBUG(get_logging_priority(), "Building bounding data for shape type '" << get_shape_name() << "'");
    _build_bounding_data();
    return;
  }

  void i_shape_3d::_at_unlock()
  {
    if (has_computed_faces()) {
      reset_computed_faces();
    }
    _reset_bounding_data();
    return;
  }

  void i_shape_3d::build_default_bounding_data()
  {
    
    _build_bounding_data();
    return;
  }

  void i_shape_3d::_build_bounding_data()
  {
    DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "!!!DEFAULT EMPTY METHOD!!!");
    return;
  }

  // virtual
  unsigned int i_shape_3d::compute_faces(face_info_collection_type & /* faces_ */) const
  {
    // Default implementation: do nothing
    return 0;
  }

  bool i_shape_3d::has_computed_faces() const
  {
    return _computed_faces_.has_data() && _computed_faces_.get().size() > 0;
  }

  const face_info_collection_type & i_shape_3d::get_computed_faces() const
  {
    if (! _computed_faces_.has_data()) {
      face_info_collection_type * faces = new face_info_collection_type;
      compute_faces(*faces);
      i_shape_3d * mutable_this = const_cast<i_shape_3d *>(this);
      mutable_this->_computed_faces_.reset(faces);
    }
    return _computed_faces_.get();
  }

  void i_shape_3d::reset_computed_faces()
  {
    _computed_faces_.reset();
    return;
  }

  // static
  void i_shape_3d::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_object_3d::init_ocd(ocd_);
    return;
  }

} // end of namespace geomtools
