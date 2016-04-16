// lahague/dosimetry/fluence_to_h10.cc
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//                       and Arnaud Chapon <achapon@cerap.fr>
//
// This file is part of LaHague.
//
// LaHague is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaHague is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LaHague. If not, see <http://www.gnu.org/licenses/>.
//

// Ourselves:
#include <lahague/dosimetry/fluence_to_h10.h>

// Third Party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/logger.h>
// - Bayeux/mygsl:
#include <mygsl/tabulated_function.h>
// - Bayeux/mctools:
#include <mctools/utils.h>

namespace lahague {

  namespace dosimetry {

    double gamma_fluence_to_h10_conversion(double energy_, uint32_t /* flags_ */)
    {
      static const double picosievert = datatools::units::get_unit("picosievert");
      const double pSv_cm2 =  picosievert * CLHEP::cm2;
      static boost::scoped_ptr<mygsl::tabulated_function> _gamma_tf;
      if (_gamma_tf.get() == 0) {
        DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS,
                      "Create the gamma fluence to h10 conversion table.");
        _gamma_tf.reset(new mygsl::tabulated_function);
        std::string table = "@lahague:data/dosimetry/gamma_fluence_to_h10.table";
        datatools::fetch_path_with_env(table);
        _gamma_tf->load_from_file(table);
        _gamma_tf->lock_table();
      }
      DT_THROW_IF(! _gamma_tf->is_valid(energy_),
                  std::range_error,
                  "Invalid gamma energy for fluence to H*(10) conversion!");
      return _gamma_tf->eval(energy_) * pSv_cm2;
    }

    double neutron_fluence_to_h10_conversion(double energy_, uint32_t /* flags_ */)
    {
      static const double picosievert = datatools::units::get_unit("picosievert");
      const double pSv_cm2 =  picosievert * CLHEP::cm2;
      static boost::scoped_ptr<mygsl::tabulated_function> _neutron_tf;
      if (_neutron_tf.get() == 0) {
        DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS,
                      "Create the neutron fluence to h10 conversion table.");
        _neutron_tf.reset(new mygsl::tabulated_function);
        std::string table = "@lahague:data/dosimetry/neutron_fluence_to_h10.table";
        datatools::fetch_path_with_env(table);
        _neutron_tf->load_from_file(table);
        _neutron_tf->lock_table();
      }
      DT_THROW_IF(! _neutron_tf->is_valid(energy_),
                  std::range_error,
                  "Invalid neutron energy for fluence to H*(10) conversion!");
      return _neutron_tf->eval(energy_) * pSv_cm2;
    }

    mc_fluence_to_h10_processor::mc_fluence_to_h10_processor()
    {
      _geometry_manager_ = 0;
      _throw_on_hit_misconfiguration_ = true;
      return;
    }

    mc_fluence_to_h10_processor::~mc_fluence_to_h10_processor()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    bool mc_fluence_to_h10_processor::has_geometry_manager() const
    {
      return _geometry_manager_ != 0;
    }

    const geomtools::manager & mc_fluence_to_h10_processor::get_geometry_manager() const
    {
      DT_THROW_IF(!has_geometry_manager(), std::logic_error, "Processor has no geometry manager!");
      return *_geometry_manager_;
    }

    void mc_fluence_to_h10_processor::set_geometry_manager(const geomtools::manager & geo_mgr_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Processor is already initialized!");
      _geometry_manager_ = &geo_mgr_;
      return;
    }

    bool mc_fluence_to_h10_processor::is_throw_on_hit_misconfiguration() const
    {
      return _throw_on_hit_misconfiguration_;
    }

    void mc_fluence_to_h10_processor::set_throw_on_hit_misconfiguration(bool t_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Processor is already initialized!");
      _throw_on_hit_misconfiguration_ = t_;
      return;
    }

    bool mc_fluence_to_h10_processor::is_initialized() const
    {
      return _initialized_;
    }

    void mc_fluence_to_h10_processor::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void mc_fluence_to_h10_processor::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Processor is already initialized!");

      if (_dose_configs_.empty()) {
        std::vector<std::string> labels;
        /*
         * compute_dose.labels : string[2] = "monitor" "dosimetre"
         */
        if (config_.has_key("compute_dose.labels")) {
          config_.fetch("compute_dose.labels", labels);
        }


        for (int i = 0; i < (int) labels.size(); i++) {
          /*
           *
           * compute_dose.monitor.hit_category        : string = "monitor_hit"
           * compute_dose.dosimetre.hit_category      : string = "dosimetre_hit"
           * compute_dose.monitor.geometry_category   : string = "monitor.gc"
           * compute_dose.dosimetre.geometry_category : string = "dosimetre.gc"
           * compute_dose.monitor.particle_name       : string = "neutron"
           * compute_dose.dosimetre.particle_name     : string = "gamma"
           *
           *
           */
          compute_dose_config cdconf;
          const std::string & label = labels[i];
          {
            std::ostringstream hit_category_oss;
            hit_category_oss << "compute_dose." << label << ".hit_category";
            if (config_.has_key(hit_category_oss.str())) {
              cdconf.hit_category = config_.fetch_string(hit_category_oss.str());
            }
          }
          {
            std::ostringstream geometry_category_oss;
            geometry_category_oss << "compute_dose." << label << ".geometry_category";
            if (config_.has_key(geometry_category_oss.str())) {
              cdconf.geometry_category = config_.fetch_string(geometry_category_oss.str());
            }
          }
          {
            std::ostringstream particle_name_oss;
            particle_name_oss << "compute_dose." << label << ".particle_name";
            if (config_.has_key(particle_name_oss.str())) {
              cdconf.particle_name = config_.fetch_string(particle_name_oss.str());
            }
          }
          add_compute_dose_config(label, cdconf);
        }

      }

      _initialized_ = true;
      return;
    }

    void mc_fluence_to_h10_processor::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Processor is not initialized!");
      _initialized_ = false;

      _dose_configs_.clear();
      _geometry_manager_ = 0;
      return;
    }

    mc_fluence_to_h10_processor::compute_dose_config::compute_dose_config()
    {
      hit_category = "";
      geometry_category = "";
      particle_name = "";
      geometry_type = geomtools::geom_id::INVALID_TYPE;
      return;
    }

    void mc_fluence_to_h10_processor::add_compute_dose_config(const std::string & label_,
                                                              const compute_dose_config & cdconf_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Processor is already initialized!");
      DT_THROW_IF(_dose_configs_.find(label_) != _dose_configs_.end(),
                  std::logic_error,
                  "Processor already has a dose config with label '" << label_ << "'!");
      compute_dose_config cdc = cdconf_;
      if (cdc.hit_category.empty()) {
        DT_THROW(std::logic_error, "Missing hit category for dose processor '" << label_ << "'!");
      }

      if (has_geometry_manager()) {
        if (!cdc.geometry_category.empty()) {
          const geomtools::id_mgr & id_mgr = _geometry_manager_->get_id_mgr();
          DT_THROW_IF(!id_mgr.has_category_info(cdc.geometry_category),
                      std::logic_error,
                      "Geometry category '" << cdc.geometry_category << "' does not exists!");
          const geomtools::id_mgr::category_info & catinfo = id_mgr.get_category_info(cdc.geometry_category);
          cdc.geometry_type = catinfo.get_type();
        }
      }
      DT_THROW_IF(cdconf_.particle_name != "gamma" && cdc.particle_name != "neutron",
                  std::logic_error,
                  "Particle '" << cdc.particle_name << "' is not supported in dose processor '"
                  << label_ << "'!");
      _dose_configs_[label_] = cdc;
      return;
    }

    void mc_fluence_to_h10_processor::process(const mctools::simulated_data & sd_,
                                              std::vector<fluence_hit> & flhits_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Processor is not initialized!");
      flhits_.clear();

      for (dose_config_dict_type::const_iterator i = _dose_configs_.begin();
           i != _dose_configs_.end();
           i++) {
        // const std::string & cdc_label = i->first;
        const compute_dose_config & cdc = i->second;
        if (sd_.has_step_hits(cdc.hit_category)) {
          unsigned int nsteps = sd_.get_number_of_step_hits(cdc.hit_category);
          for (int istep = 0; istep < (int) nsteps; istep++) {
            const mctools::base_step_hit & truth_hit = sd_.get_step_hit(cdc.hit_category, istep);
            bool new_flhit = true;
            if (new_flhit && !truth_hit.get_auxiliaries().has_flag(mctools::track_utils::ENTERING_VOLUME_FLAG)) {
              new_flhit = false;
            }
            if (new_flhit && !cdc.particle_name.empty() && truth_hit.get_particle_name() != cdc.particle_name) {
              new_flhit = false;
            }
            if (new_flhit && !truth_hit.get_auxiliaries().has_key(mctools::track_utils::ENTERING_KINETIC_ENERGY_KEY)) {
              DT_THROW_IF(is_throw_on_hit_misconfiguration(),
                          std::runtime_error,
                          "We found a source truth hit in the hit category '" << cdc.hit_category << "' "
                          << " that does not have the '" << mctools::track_utils::ENTERING_KINETIC_ENERGY_KEY << "' property set!"
                          << " This is probably due to some misconfiguration in the Monte-Carlo setup!");
              new_flhit = false;
            }
            if (cdc.geometry_type != geomtools::geom_id::INVALID_TYPE) {
              // Additional required match:
              if (truth_hit.get_geom_id().get_type() != cdc.geometry_type) {
                new_flhit = false;
              }
            }

            if (new_flhit) {
              {
                fluence_hit dummy;
                flhits_.push_back(dummy);
              }
              fluence_hit & flhit = flhits_.back();
              flhit.set_hit_id(flhits_.size() - 1);
              flhit.set_geom_id(truth_hit.get_geom_id());
              flhit.set_particle_name(truth_hit.get_particle_name());
              flhit.set_entrance_energy(truth_hit.get_auxiliaries().fetch_real(mctools::track_utils::ENTERING_KINETIC_ENERGY_KEY));
              flhit.set_h10_star(0.0);
              if (flhit.get_particle_name() == "neutron") {
                flhit.set_h10_star(neutron_fluence_to_h10_conversion(flhit.get_entrance_energy()));
              } else if (flhit.get_particle_name() == "gamma") {
                flhit.set_h10_star(gamma_fluence_to_h10_conversion(flhit.get_entrance_energy()));
              }
            } // new_flhit
          } // for
        } // if
      } // for

      return;
    }

  } // namespace dosimetry

} // namespace lahague
