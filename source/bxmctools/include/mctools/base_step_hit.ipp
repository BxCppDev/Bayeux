/// \file mctools/base_step_hit.ipp

#ifndef MCTOOLS_BASE_STEP_HIT_IPP
#define MCTOOLS_BASE_STEP_HIT_IPP 1

// Ourselves:
#include <mctools/base_step_hit.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

// - Bayeux/geomtools
#include <geomtools/base_hit.ipp>
#include <geomtools/utils.ipp>
#include <geomtools/material.h>
#include <geomtools/sensitive.h>

namespace mctools {

  template<class Archive>
  void base_step_hit::serialize(Archive & ar_,
                                const unsigned int version_)
  {
    // Inherit from the 'base_hit' mother class:
    ar_ & boost::serialization::make_nvp("geomtools__base_hit",
                                         boost::serialization::base_object<geomtools::base_hit>(*this));

    // The '_store' field member from the 'base_hit' mother class
    // knows about the field to be stored/loaded from the archive.
    if (_store & STORE_POSITION_START) {
      ar_ & boost::serialization::make_nvp ("position_start", _position_start_);
    }
    if (_store & STORE_POSITION_STOP) {
      ar_ & boost::serialization::make_nvp ("position_stop",  _position_stop_);
    }
    if (_store & STORE_TIME_START) {
      ar_ & boost::serialization::make_nvp ("time_start",     _time_start_);
    }
    if (_store & STORE_TIME_STOP) {
      ar_ & boost::serialization::make_nvp ("time_stop",      _time_stop_);
    }
    if (_store & STORE_MOMENTUM_START) {
      ar_ & boost::serialization::make_nvp ("momentum_start", _momentum_start_);
    }
    if (_store & STORE_MOMENTUM_STOP) {
      ar_ & boost::serialization::make_nvp ("momentum_stop",  _momentum_stop_);
    }
    if (_store & STORE_ENERGY_DEPOSIT) {
      ar_ & boost::serialization::make_nvp ("energy_deposit", _energy_deposit_);
    }
    if (_store & STORE_PARTICLE_NAME) {
      ar_ & boost::serialization::make_nvp ("particle_name",  _particle_name_);
    }
    if (version_ < 1) {
      // Biasing weight is not available before version 1:
      if (Archive::is_loading::value)  {
        invalidate_biasing_weight();
      }
    } else {
      // Support biasing weight from version 1:
      if (_store & STORE_BIASING_WEIGHT) {
        ar_ & boost::serialization::make_nvp ("biasing_weight",  _biasing_weight_);
      }
      
      if (version_ < 2) {
        if (Archive::is_loading::value)  {
          // Convert auxiliaries properties into attributes:
          
          if (get_auxiliaries().has_key(mctools::track_utils::TRACK_ID_KEY)) {
            const int track_id = this->get_auxiliaries().fetch_integer(mctools::track_utils::TRACK_ID_KEY);
            this->set_track_id(track_id);
            this->grab_auxiliaries().erase(mctools::track_utils::TRACK_ID_KEY);
          }
          
          if (get_auxiliaries().has_key(mctools::track_utils::PARENT_TRACK_ID_KEY)) {
            const int parent_track_id = this->get_auxiliaries().fetch_integer(mctools::track_utils::PARENT_TRACK_ID_KEY);
            this->set_parent_track_id(parent_track_id);
            this->grab_auxiliaries().erase(mctools::track_utils::PARENT_TRACK_ID_KEY);
          }
         
          if (get_auxiliaries().has_key(mctools::track_utils::PRIMARY_PARTICLE_FLAG)) {
            const bool flag = this->get_auxiliaries().fetch_boolean(mctools::track_utils::PRIMARY_PARTICLE_FLAG);
            this->set_primary_particle(flag);
            this->grab_auxiliaries().erase(mctools::track_utils::PRIMARY_PARTICLE_FLAG);
          }
        
          if (get_auxiliaries().has_key(mctools::track_utils::MAJOR_TRACK_FLAG)) {
            const bool flag = this->get_auxiliaries().fetch_boolean(mctools::track_utils::MAJOR_TRACK_FLAG);
            this->set_major_track(flag);
            this->grab_auxiliaries().erase(mctools::track_utils::MAJOR_TRACK_FLAG);
          }
        
          if (get_auxiliaries().has_key(mctools::track_utils::DELTA_RAY_FROM_ALPHA_FLAG)) {
            const bool flag = this->get_auxiliaries().fetch_boolean(mctools::track_utils::DELTA_RAY_FROM_ALPHA_FLAG);
            this->set_delta_ray_from_alpha(flag);
            this->grab_auxiliaries().erase(mctools::track_utils::DELTA_RAY_FROM_ALPHA_FLAG);
          }
       
          if (get_auxiliaries().has_key(mctools::track_utils::ENTERING_VOLUME_FLAG)) {
            const bool flag = this->get_auxiliaries().fetch_boolean(mctools::track_utils::ENTERING_VOLUME_FLAG);
            this->set_entering_volume(flag);
            this->grab_auxiliaries().erase(mctools::track_utils::ENTERING_VOLUME_FLAG);
          }
       
          if (get_auxiliaries().has_key(mctools::track_utils::LEAVING_VOLUME_FLAG)) {
            const bool flag = this->get_auxiliaries().fetch_boolean(mctools::track_utils::LEAVING_VOLUME_FLAG);
            this->set_leaving_volume(flag);
            this->grab_auxiliaries().erase(mctools::track_utils::LEAVING_VOLUME_FLAG);
          }
       
          if (get_auxiliaries().has_key(mctools::track_utils::START_KINETIC_ENERGY_KEY)) {
            const double ke = this->get_auxiliaries().fetch_real(mctools::track_utils::START_KINETIC_ENERGY_KEY);
            this->set_kinetic_energy_start(ke);
            this->grab_auxiliaries().erase(mctools::track_utils::START_KINETIC_ENERGY_KEY);
          }
       
          if (get_auxiliaries().has_key(mctools::track_utils::STOP_KINETIC_ENERGY_KEY)) {
            const double ke = this->get_auxiliaries().fetch_real(mctools::track_utils::STOP_KINETIC_ENERGY_KEY);
            this->set_kinetic_energy_stop(ke);
            this->grab_auxiliaries().erase(mctools::track_utils::STOP_KINETIC_ENERGY_KEY);
          }
      
          if (get_auxiliaries().has_key(mctools::track_utils::STEP_LENGTH)) {
            const double sl = this->get_auxiliaries().fetch_real(mctools::track_utils::STEP_LENGTH);
            this->set_step_length(sl);
            this->grab_auxiliaries().erase(mctools::track_utils::STEP_LENGTH);
          }
         
          if (get_auxiliaries().has_key(mctools::track_utils::CREATOR_PROCESS_KEY)) {
            const std::string & cp = this->get_auxiliaries().fetch_string(mctools::track_utils::CREATOR_PROCESS_KEY);
            this->set_creator_process_name(cp);
            this->grab_auxiliaries().erase(mctools::track_utils::CREATOR_PROCESS_KEY);
          }

          {
            static const std::string material_ref_key =
              geomtools::material::make_key(geomtools::material::material_ref_property());       
            if (get_auxiliaries().has_key(material_ref_key)) {
              const std::string & mr = this->get_auxiliaries().fetch_string(material_ref_key);
              this->set_material_name(mr);
              this->grab_auxiliaries().erase(material_ref_key);
            }
          }
  
          {
            static const std::string sensitive_category_key =
              geomtools::sensitive::make_key(geomtools::sensitive::constants::instance().SENSITIVE_CATEGORY_PROPERTY);       
            if (get_auxiliaries().has_key(sensitive_category_key)) {
              const std::string & sc = this->get_auxiliaries().fetch_string(sensitive_category_key);
              this->set_sensitive_category(sc);
              this->grab_auxiliaries().erase(sensitive_category_key);
            }
          }
  
          {
            if (get_auxiliaries().has_key(sensitive_utils::SENSITIVE_G4_VOLUME_NAME_KEY)) {;
              const std::string & vn = this->get_auxiliaries().fetch_string(sensitive_utils::SENSITIVE_G4_VOLUME_NAME_KEY);
              this->set_g4_volume_name(vn);
              this->grab_auxiliaries().erase(sensitive_utils::SENSITIVE_G4_VOLUME_NAME_KEY);
            }
          }
          
          {
            if (get_auxiliaries().has_key(sensitive_utils::SENSITIVE_G4_VOLUME_COPY_NUMBER_KEY)) {
              const int vcn = this->get_auxiliaries().fetch_positive_integer(sensitive_utils::SENSITIVE_G4_VOLUME_COPY_NUMBER_KEY);
              this->set_g4_volume_copy_number(vcn);
              this->grab_auxiliaries().erase(sensitive_utils::SENSITIVE_G4_VOLUME_COPY_NUMBER_KEY);
            }
          }
          
          {
            static const std::string & hit_processor_key = mctools::hit_utils::HIT_MC_STEP_PROCESSOR_KEY;      
            if (get_auxiliaries().has_key(hit_processor_key)) {
              const std::string & hp = this->get_auxiliaries().fetch_string(hit_processor_key);
              this->set_hit_processor(hp);
              this->grab_auxiliaries().erase(hit_processor_key);
            }
          }
          
          {
            static const std::string & visu_highlight_key = mctools::hit_utils::HIT_VISU_HIGHLIGHTED_KEY;
            if (get_auxiliaries().has_key(visu_highlight_key)) {
              const bool vh = this->get_auxiliaries().fetch_boolean(visu_highlight_key);
              this->set_visu_highlight(vh);
              this->grab_auxiliaries().erase(visu_highlight_key);
            }
          }

        } // process loaded properties

      } else {
        // Version >= 2:
        // Support new attributes:
        if (_store & STORE_KINETIC_ENERGY_START) {
          ar_ & boost::serialization::make_nvp ("kinetic_energy_start", _kinetic_energy_start_);
        }
        if (_store & STORE_KINETIC_ENERGY_STOP) {
          ar_ & boost::serialization::make_nvp ("kinetic_energy_stop", _kinetic_energy_stop_);
        }
        if (_store & STORE_STEP_LENGTH) {
          ar_ & boost::serialization::make_nvp ("step_length", _step_length_);
        }
        if (_store & STORE_ENTERING_VOLUME_FLAG) {
          ar_ & boost::serialization::make_nvp ("entering_volume_flag", _entering_volume_flag_);
        }
        if (_store & STORE_LEAVING_VOLUME_FLAG) {
          ar_ & boost::serialization::make_nvp ("leaving_volume_flag", _leaving_volume_flag_);
        }
        if (_store & STORE_CREATOR_PROCESS_NAME) {
          ar_ & boost::serialization::make_nvp ("creator_process_name", _creator_process_name_);
        }
        if (_store & STORE_PRIMARY_PARTICLE_FLAG) {
          ar_ & boost::serialization::make_nvp ("primary_particle_flag", _primary_particle_flag_);
        }
        if (_store & STORE_MAJOR_TRACK_FLAG) {
          ar_ & boost::serialization::make_nvp ("major_track_flag", _major_track_flag_);
        }
        if (_store & STORE_DELTA_RAY_FROM_ALPHA_FLAG) {
          ar_ & boost::serialization::make_nvp ("delta_ray_from_alpha_flag", _delta_ray_from_alpha_flag_);
        }
        if (_store & STORE_TRACK_ID) {
          ar_ & boost::serialization::make_nvp ("track_id", _track_id_);
        }
        if (_store & STORE_PARENT_TRACK_ID) {
          ar_ & boost::serialization::make_nvp ("parent_track_id", _parent_track_id_);
        }
        if (_store & STORE_MATERIAL_NAME) {
          ar_ & boost::serialization::make_nvp ("material_name", _material_name_);
        }
        if (_store & STORE_SENSITIVE_CATEGORY) {
          ar_ & boost::serialization::make_nvp ("sensitive_category", _sensitive_category_);
        }
        if (_store & STORE_G4_VOLUME_NAME) {
          ar_ & boost::serialization::make_nvp ("g4_volume_name", _g4_volume_name_);
        }
        if (_store & STORE_G4_VOLUME_COPY_NUMBER) {
          ar_ & boost::serialization::make_nvp ("g4_volume_copy_number", _g4_volume_copy_number_);
        }
        if (_store & STORE_HIT_PROCESSOR) {
          ar_ & boost::serialization::make_nvp ("hit_processor", _hit_processor_);
        }
        if (_store & STORE_VISU_HIGHLIGHT_FLAG) {
          ar_ & boost::serialization::make_nvp ("visu_highlight", _visu_highlight_);
        }
      }
      
    }
    return;
  }

} // end of namespace mctools

#endif // MCTOOLS_BASE_STEP_HIT_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
