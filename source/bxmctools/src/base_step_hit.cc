// base_step_hit.cc

// Ourselves:
#include <mctools/base_step_hit.h>

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <iomanip>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>

// This project:
#include <mctools/utils.h>

namespace mctools {

  // Serial tag for datatools::serialization::i_serializable interface :
  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(base_step_hit, "mctools::base_step_hit")

  /* Position start */

  bool base_step_hit::has_position_start() const
  {
    return geomtools::is_valid(_position_start_);
  }

  const geomtools::vector_3d &
  base_step_hit::get_position_start() const
  {
    return _position_start_;
  }

  void
  base_step_hit::set_position_start(const geomtools::vector_3d & new_value_)
  {
    if (geomtools::is_valid(new_value_)) {
      _store |= STORE_POSITION_START;
    } else {
      _store &= ~STORE_POSITION_START;
    }
    _position_start_ = new_value_;
    return;
  }

  void base_step_hit::invalidate_position_start()
  {
    set_position_start(geomtools::invalid_vector_3d());
    return;
  }

  /* Position stop */
  
  bool base_step_hit::has_position_stop() const
  {
    return geomtools::is_valid(_position_stop_);
  }

  const geomtools::vector_3d &
  base_step_hit::get_position_stop() const
  {
    return _position_stop_;
  }

  void
  base_step_hit::set_position_stop(const geomtools::vector_3d & new_value_)
  {
    if (geomtools::is_valid(new_value_)) {
      _store |= STORE_POSITION_STOP;
    } else {
      _store &= ~STORE_POSITION_STOP;
    }
    _position_stop_ = new_value_;
    return;
  }

  void base_step_hit::invalidate_position_stop()
  {
    set_position_stop(geomtools::invalid_vector_3d());
    return;
  }

  /* Time start */

  bool base_step_hit::has_time_start() const
  {
    return geomtools::is_valid(_time_start_);
  }

  double
  base_step_hit::get_time_start() const
  {
    return _time_start_;
  }

  void
  base_step_hit::set_time_start(const double new_value_)
  {
    if (datatools::is_valid(new_value_)) {
      _store |= STORE_TIME_START;
    } else {
      _store &= ~STORE_TIME_START;
    }
    _time_start_ = new_value_;
    return;
  }

  void base_step_hit::invalidate_time_start()
  {
    set_position_start(geomtools::invalid_vector_3d());
    return;
  }

  /* Time stop */

  bool base_step_hit::has_time_stop() const
  {
    return geomtools::is_valid(_time_stop_);
  }

  double
  base_step_hit::get_time_stop() const
  {
    return _time_stop_;
  }

  void
  base_step_hit::set_time_stop(const double new_value_)
  {
    if (datatools::is_valid(new_value_)) {
      _store |= STORE_TIME_STOP;
    } else {
      _store &= ~STORE_TIME_STOP;
    }
    _time_stop_ = new_value_;
    return;
  }

  void base_step_hit::invalidate_time_stop()
  {
    set_position_stop(geomtools::invalid_vector_3d());
    return;
  }

  /* Momentum start */
  
  bool base_step_hit::has_momentum_start() const
  {
    return geomtools::is_valid(_momentum_start_);
  }

  const geomtools::vector_3d &
  base_step_hit::get_momentum_start() const
  {
    return _momentum_start_;
  }

  void
  base_step_hit::set_momentum_start(const geomtools::vector_3d & new_value_)
  {
    if (geomtools::is_valid(new_value_)) {
      _store |= STORE_MOMENTUM_START;
    } else {
      _store &= ~STORE_MOMENTUM_START;
    }
    _momentum_start_ = new_value_;
    return;
  }

  void base_step_hit::invalidate_momentum_start()
  {
    set_momentum_start(geomtools::invalid_vector_3d());
    return;
  }

  /* Momentum stop */

  bool base_step_hit::has_momentum_stop() const
  {
    return geomtools::is_valid(_momentum_stop_);
  }

  const geomtools::vector_3d &
  base_step_hit::get_momentum_stop() const
  {
    return _momentum_stop_;
  }

  void
  base_step_hit::set_momentum_stop(const geomtools::vector_3d & new_value_)
  {
    if (geomtools::is_valid(new_value_)) {
      _store |= STORE_MOMENTUM_STOP;
    } else {
      _store &= ~STORE_MOMENTUM_STOP;
    }
    _momentum_stop_ = new_value_;
    return;
  }

  void base_step_hit::invalidate_momentum_stop()
  {
    set_momentum_stop(geomtools::invalid_vector_3d());
    return;
  }

  /* Energy deposit */
  
  bool base_step_hit::has_energy_deposit() const
  {
    return datatools::is_valid(_energy_deposit_);
  }

  double
  base_step_hit::get_energy_deposit() const
  {
    return _energy_deposit_;
  }

  void
  base_step_hit::set_energy_deposit(const double new_value_)
  {
    if (datatools::is_valid(new_value_)) {
      _store |= STORE_ENERGY_DEPOSIT;
    } else {
      _store &= ~STORE_ENERGY_DEPOSIT;
    }
    _energy_deposit_ = new_value_;
    return;
  }

  void base_step_hit::invalidate_energy_deposit()
  {
    set_energy_deposit(datatools::invalid_real());
    return;
  }

  /* Particle name */
  
  bool base_step_hit::has_particle_name() const
  {
    return !_particle_name_.empty();
  }
                
  const std::string & base_step_hit::get_particle_name() const
  {
    return _particle_name_;
  }

  void base_step_hit::set_particle_name(const std::string & name_)
  {
    if (!name_.empty()) {
      _store |= STORE_PARTICLE_NAME;
    } else {
      _store &= ~STORE_PARTICLE_NAME;
    }
    _particle_name_ = name_;
    return;
  }

  void base_step_hit::invalidate_particle_name()
  {
    set_particle_name("");
    return;
  }

  /* Biasing weight */
  
  bool base_step_hit::has_biasing_weight() const
  {
    return (_store & STORE_BIASING_WEIGHT) && datatools::is_valid(_biasing_weight_);
  }

  void base_step_hit::set_biasing_weight(double bw_)
  {
    DT_THROW_IF(bw_ < 0.0,
                std::domain_error,
                "Invalid biasing weight value ('" << bw_ << "')!");
    if (datatools::is_valid(bw_)) {
      _store |= STORE_BIASING_WEIGHT;
    } else {
      _store &= ~STORE_BIASING_WEIGHT;
    }
    _biasing_weight_ = bw_;
    return;
  }

  double base_step_hit::get_biasing_weight() const
  {
    return _biasing_weight_;
  }

  void base_step_hit::invalidate_biasing_weight()
  {
    set_biasing_weight(datatools::invalid_real());
    return;
  }

  /* Kinetic energy start */
  
  bool base_step_hit::has_kinetic_energy_start() const
  {
    return datatools::is_valid(_kinetic_energy_start_);
  }

  double
  base_step_hit::get_kinetic_energy_start() const
  {
    return _kinetic_energy_start_;
  }

  void
  base_step_hit::set_kinetic_energy_start(const double new_value_)
  {
    if (datatools::is_valid(new_value_)) {
      _store |= STORE_KINETIC_ENERGY_START;
    } else {
      _store &= ~STORE_KINETIC_ENERGY_START;
    }
    _kinetic_energy_start_ = new_value_;
    return;
  }

  void base_step_hit::invalidate_kinetic_energy_start()
  {
    set_kinetic_energy_start(datatools::invalid_real());
    return;
  }

  /* Kinetic energy stop */
  
  bool base_step_hit::has_kinetic_energy_stop() const
  {
    return datatools::is_valid(_kinetic_energy_stop_);
  }

  double
  base_step_hit::get_kinetic_energy_stop() const
  {
    return _kinetic_energy_stop_;
  }

  void
  base_step_hit::set_kinetic_energy_stop(const double new_value_)
  {
    if (datatools::is_valid(new_value_)) {
      _store |= STORE_KINETIC_ENERGY_STOP;
    } else {
      _store &= ~STORE_KINETIC_ENERGY_STOP;
    }
    _kinetic_energy_stop_ = new_value_;
    return;
  }

  void base_step_hit::invalidate_kinetic_energy_stop()
  {
    set_kinetic_energy_stop(datatools::invalid_real());
    return;
  }

  /* Step length */
  
  bool base_step_hit::has_step_length() const
  {
    return datatools::is_valid(_step_length_);
  }

  double
  base_step_hit::get_step_length() const
  {
    return _step_length_;
  }

  void
  base_step_hit::set_step_length(const double new_value_)
  {
    if (datatools::is_valid(new_value_)) {
      _store |= STORE_STEP_LENGTH;
    } else {
      _store &= ~STORE_STEP_LENGTH;
    }
    _step_length_ = new_value_;
    return;
  }

  void base_step_hit::invalidate_step_length()
  {
    set_step_length(datatools::invalid_real());
    return;
  }

  /* Entering volume */
  
  bool base_step_hit::has_entering_volume() const
  {
    return _store & STORE_ENTERING_VOLUME_FLAG;
  }

  bool base_step_hit::is_entering_volume() const
  {
    return _entering_volume_flag_;
  }

  void
  base_step_hit::set_entering_volume(const bool new_value_)
  {
    _store |= STORE_ENTERING_VOLUME_FLAG;
    _entering_volume_flag_ = new_value_;
    return;
  }

  void base_step_hit::invalidate_entering_volume()
  {
    _entering_volume_flag_ = false;
    _store &= ~STORE_ENTERING_VOLUME_FLAG;
    return;
  }

  /* Leaving volume */
  
  bool base_step_hit::has_leaving_volume() const
  {
    return _store & STORE_LEAVING_VOLUME_FLAG;
  }

  bool base_step_hit::is_leaving_volume() const
  {
    return _leaving_volume_flag_;
  }

  void
  base_step_hit::set_leaving_volume(const bool new_value_)
  {
    _store |= STORE_LEAVING_VOLUME_FLAG;
    _leaving_volume_flag_ = new_value_;
    return;
  }

  void base_step_hit::invalidate_leaving_volume()
  {
    _leaving_volume_flag_ = false;
    _store &= ~STORE_LEAVING_VOLUME_FLAG;
    return;
  }
 
  /* Creator process */
  
  bool base_step_hit::has_creator_process_name() const
  {
    return !_creator_process_name_.empty();
  }
                
  const std::string & base_step_hit::get_creator_process_name() const
  {
    return _creator_process_name_;
  }

  void base_step_hit::set_creator_process_name(const std::string & name_)
  {
    if (!name_.empty()) {
      _store |= STORE_CREATOR_PROCESS_NAME;
    } else {
      _store &= ~STORE_CREATOR_PROCESS_NAME;
    }
    _creator_process_name_ = name_;
    return;
  }

  void base_step_hit::invalidate_creator_process_name()
  {
    set_creator_process_name("");
    return;
  }
  
  /* Primary particle */

  bool base_step_hit::has_primary_particle() const
  {
    return _store & STORE_PRIMARY_PARTICLE_FLAG;
  }

  bool base_step_hit::is_primary_particle() const
  {
    return _primary_particle_flag_;
  }

  void base_step_hit::set_primary_particle(const bool flag_)
  {
    _store |= STORE_PRIMARY_PARTICLE_FLAG;
    _primary_particle_flag_ = flag_;
    return;
  }

  void base_step_hit::invalidate_primary_particle()
  {
    _primary_particle_flag_ = false;
    _store &= ~STORE_PRIMARY_PARTICLE_FLAG;
    return;
  }

  /* Major track */

  bool  base_step_hit::has_major_track() const
  {
    return _store & STORE_MAJOR_TRACK_FLAG;
  }

  bool base_step_hit::is_major_track() const
  {
    return _major_track_flag_;
  }

  void base_step_hit::set_major_track(const bool flag_)
  {
    _store |= STORE_MAJOR_TRACK_FLAG;
    _major_track_flag_ = flag_;
    return;
  }

  void base_step_hit::invalidate_major_track()
  {
    _major_track_flag_ = false;
    _store &= ~STORE_MAJOR_TRACK_FLAG;
    return;
  }
   
  /* Delta-ray from alpha particle */
  
  bool base_step_hit::has_delta_ray_from_alpha() const
  {
    return _store & STORE_DELTA_RAY_FROM_ALPHA_FLAG;
  }

  bool base_step_hit::is_delta_ray_from_alpha() const
  {
    return _delta_ray_from_alpha_flag_;
  }

  void
  base_step_hit::set_delta_ray_from_alpha(const bool new_value_)
  {
    _store |= STORE_DELTA_RAY_FROM_ALPHA_FLAG;
    _delta_ray_from_alpha_flag_ = new_value_;
    return;
  }

  void base_step_hit::invalidate_delta_ray_from_alpha()
  {
    _delta_ray_from_alpha_flag_ = false;
    _store &= ~STORE_DELTA_RAY_FROM_ALPHA_FLAG;
    return;
  }

  /* track ID */

  bool base_step_hit::has_track_id() const
  {
    return _track_id_ != INVALID_HIT_ID;
  }

  int base_step_hit::get_track_id() const
  {
    return _track_id_;
  }

  void base_step_hit::set_track_id(const int id_)
  {
    if (id_ != INVALID_HIT_ID) {
      _store |= STORE_TRACK_ID;
    } else {
      _store &= ~STORE_TRACK_ID;
    }
    _track_id_ = id_;
    return;
  }

  void base_step_hit::invalidate_track_id()
  {
    set_track_id(INVALID_HIT_ID);
    return;
  }
   /* parent track ID */

  bool base_step_hit::has_parent_track_id() const
  {
    return _parent_track_id_ != INVALID_HIT_ID;
  }

  int base_step_hit::get_parent_track_id() const
  {
     return _parent_track_id_;
  }

  void base_step_hit::set_parent_track_id(const int id_)
  {
    if (id_ != INVALID_HIT_ID) {
      _store |= STORE_PARENT_TRACK_ID;
    } else {
      _store &= ~STORE_PARENT_TRACK_ID;
    }
    _parent_track_id_ = id_;
    return;
  }

  void base_step_hit::invalidate_parent_track_id()
  {
    set_parent_track_id(INVALID_HIT_ID);
    return;
  }

  /* Material name */
  
  bool base_step_hit::has_material_name() const
  {
    return !_material_name_.empty();
  }
                
  const std::string & base_step_hit::get_material_name() const
  {
    return _material_name_;
  }

  void base_step_hit::set_material_name(const std::string & name_)
  {
    if (!name_.empty()) {
      _store |= STORE_MATERIAL_NAME;
    } else {
      _store &= ~STORE_MATERIAL_NAME;
    }
    _material_name_ = name_;
    return;
  }

  void base_step_hit::invalidate_material_name()
  {
    set_material_name("");
    return;
  }

  /* Sensitive category */
  
  bool base_step_hit::has_sensitive_category() const
  {
    return !_sensitive_category_.empty();
  }
                
  const std::string & base_step_hit::get_sensitive_category() const
  {
    return _sensitive_category_;
  }

  void base_step_hit::set_sensitive_category(const std::string & name_)
  {
    if (!name_.empty()) {
      _store |= STORE_SENSITIVE_CATEGORY;
    } else {
      _store &= ~STORE_SENSITIVE_CATEGORY;
    }
    _sensitive_category_ = name_;
    return;
  }

  void base_step_hit::invalidate_sensitive_category()
  {
    set_sensitive_category("");
    return;
  }

  /* Geant4 volume name */
  
  bool base_step_hit::has_g4_volume_name() const
  {
    return !_g4_volume_name_.empty();
  }
                
  const std::string & base_step_hit::get_g4_volume_name() const
  {
    return _g4_volume_name_;
  }

  void base_step_hit::set_g4_volume_name(const std::string & name_)
  {
    if (!name_.empty()) {
      _store |= STORE_G4_VOLUME_NAME;
    } else {
      _store &= ~STORE_G4_VOLUME_NAME;
    }
    _g4_volume_name_ = name_;
    return;
  }

  void base_step_hit::invalidate_g4_volume_name()
  {
    set_g4_volume_name("");
    return;
  }
 
  /* Geant4 volume copy number */

  bool base_step_hit::has_g4_volume_copy_number() const
  {
    return _g4_volume_copy_number_ != -1;
  }

  int base_step_hit::get_g4_volume_copy_number() const
  {
    return _g4_volume_copy_number_;
  }

  void base_step_hit::set_g4_volume_copy_number(const int number_)
  {
    if (number_ > -1) {
      _store |= STORE_G4_VOLUME_COPY_NUMBER;
      _g4_volume_copy_number_ = number_;
    } else {
      _store &= ~STORE_G4_VOLUME_COPY_NUMBER;
      _g4_volume_copy_number_ = -1;
    }
    return;
  }

  void base_step_hit::invalidate_g4_volume_copy_number()
  {
    set_g4_volume_copy_number(-1);
    return;
  }

  /* General */
  
  base_step_hit::base_step_hit()
    : geomtools::base_hit()
  {
    base_step_hit::reset();
    return;
  }

  base_step_hit::~base_step_hit()
  {
    base_step_hit::reset();
    return;
  }

  bool base_step_hit::is_valid() const
  {
    if (!geomtools::base_hit::is_valid()) return false;
    if (!has_time_start()) return false;
    if (!has_position_start()) return false;
    return true;
  }

  void base_step_hit::reset()
  {
    // Version 2:
    invalidate_kinetic_energy_start();
    invalidate_kinetic_energy_stop();
    invalidate_step_length();
    invalidate_entering_volume();
    invalidate_leaving_volume();
    invalidate_creator_process_name();
    invalidate_primary_particle();
    invalidate_major_track();
    invalidate_delta_ray_from_alpha();
    invalidate_material_name();
    invalidate_sensitive_category();
    invalidate_g4_volume_name();
    invalidate_g4_volume_copy_number();

    // Version 1:
    invalidate_biasing_weight();

    // Version 0:
    invalidate_position_start();
    invalidate_position_stop();
    invalidate_time_start();
    invalidate_time_stop();
    invalidate_momentum_start();
    invalidate_momentum_stop();
    invalidate_energy_deposit();
    invalidate_particle_name();

    // Base class:
    geomtools::base_hit::reset();
    return;
  }

  void base_step_hit::clear()
  {
    base_step_hit::reset();
    return;
  }

  void base_step_hit::invalidate()
  {
    base_step_hit::reset();
    return;
  }

  void base_step_hit::tree_dump(std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const
  {
    geomtools::base_hit::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << tag
         << "Position start : "
         << std::setprecision(15) << _position_start_ / CLHEP::mm  << " mm" << std::endl;
    
    out_ << indent_ << tag
         << "Position stop  : "
         << std::setprecision(15) << _position_stop_ / CLHEP::mm << " mm" << std::endl;
    
    out_ << indent_ << tag
         << "Time start     : " << _time_start_ / CLHEP::ns << " ns" << std::endl;
    
    out_ << indent_ << tag
         << "Time stop      : " << _time_stop_ / CLHEP::ns << " ns" << std::endl;
    
    out_ << indent_ << tag
         << "Momentum start : " << _momentum_start_ / CLHEP::keV << " keV" << std::endl;
    
    out_ << indent_ << tag
         << "Momentum stop  : " << _momentum_stop_ / CLHEP::keV << " keV" << std::endl;
    
    out_ << indent_ << tag
         << "Energy deposit : " << _energy_deposit_ / CLHEP::keV << " keV" << std::endl;
    
    out_ << indent_ << tag
         << "Particle name  : `" << _particle_name_ << "'" << std::endl;
    
    out_ << indent_ << tag
         << "Biasing weight  : " << _biasing_weight_ << std::endl;
   
    out_ << indent_ << tag
         << "Kinetic energy start : " << _kinetic_energy_start_ / CLHEP::keV << " keV" << std::endl;
  
    out_ << indent_ << tag
         << "Kinetic energy stop : " << _kinetic_energy_stop_ / CLHEP::keV << " keV" << std::endl;
  
    out_ << indent_ << tag
         << "Step length : " << _step_length_ / CLHEP::mm << " mm" << std::endl;

    out_ << indent_ << tag
         << "Entering volume : " << std::boolalpha << _entering_volume_flag_ << std::endl;

    out_ << indent_ << tag
         << "Leaving volume : " << std::boolalpha << _leaving_volume_flag_ << std::endl;
    
    out_ << indent_ << tag
         << "Creator process name  : `" << _creator_process_name_ << "'" << std::endl;

    out_ << indent_ << tag
         << "Primary particle : " << std::boolalpha << _primary_particle_flag_ << std::endl;

    out_ << indent_ << tag
         << "Major track : " << std::boolalpha << _major_track_flag_ << std::endl;

    out_ << indent_ << tag
         << "Delta-ray from alpha : " << std::boolalpha << _delta_ray_from_alpha_flag_ << std::endl;
 
    out_ << indent_ << tag
         << "Track ID : " << _track_id_ << std::endl;

    out_ << indent_ << tag
         << "Parent track ID : " << _parent_track_id_ << std::endl;
    
    out_ << indent_ << tag
         << "Sensitive category  : `" << _sensitive_category_ << "'" << std::endl;
    
    out_ << indent_ << tag
         << "Geant4 volume name  : `" << _g4_volume_name_ << "'" << std::endl;
    
    out_ << indent_ << tag
         << "Geant4 volume copy number : " << _g4_volume_copy_number_ << std::endl;
    
    out_ << indent_ << inherit_tag(inherit_)
         << "Validity : " << std::boolalpha << is_valid() << std::endl;

    return;
  }

  void base_step_hit::dump() const
  {
    tree_dump(std::clog);
    return;
  }

} // end of namespace mctools
