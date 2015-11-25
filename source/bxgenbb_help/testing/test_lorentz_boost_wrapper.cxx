// test_lorentz_boost_wrapper.cxx
/*
 * Usage:
 *
 * shell$ .../test_lorentz_boost_wrapper \
 *   | cut -d' ' -f1 | gsl-histogram 0 2 100 > ke.his
 *
 * gnuplot> plot './ke.his' using 1:3 with histep
 *
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

// This project:
#include <genbb_help/single_particle_generator.h>
#include <genbb_help/lorentz_boost_wrapper.h>
#include <genbb_help/primary_event.h>

/// \brief A generator of Lorentz boost along a chosen arbitrary axis and fixed position
class lbs : public genbb::i_lorentz_boost_generator
{
public:

  virtual bool has_next()
  {
    return true;
  }

  virtual void add_metadata(genbb::primary_event & pe_)
  {
    pe_.grab_auxiliaries().store_flag("lbs.test");
    return;
  }

  virtual bool is_initialized() const
  {
    return geomtools::is_valid(_vbeta_);
  }

  virtual void initialize(const datatools::properties & config_)
  {
    if (! geomtools::is_valid(_vbeta_)) {
      if (config_.has_flag("xaxis")) {
        if (config_.has_key("beta")) {
          double beta = config_.fetch_real("beta");
          _vbeta_.set(beta, 0.0, 0.0);
        }
      } else if (config_.has_flag("yaxis")) {
        if (config_.has_key("beta")) {
          double beta = config_.fetch_real("beta");
          _vbeta_.set(0.0, beta, 0.0);
        }
      } else if (config_.has_flag("zaxis")) {
        if (config_.has_key("beta")) {
          double beta = config_.fetch_real("beta");
          _vbeta_.set(0.0, 0.0, beta);
        }
      }
    }
    return;
  }

  virtual void reset()
  {
    geomtools::invalidate(_vbeta_);
    return;
  }

  lbs()
  {
    geomtools::invalidate(_vbeta_);
    return;
  }

  lbs(double zbeta_)
  {
    _vbeta_.set(0.0, 0.0, zbeta_);
    return;
  }

  lbs(double xbeta_, double ybeta_, double zbeta_)
  {
    _vbeta_.set(xbeta_, ybeta_, zbeta_);
    return;
  }

protected:

  virtual void _generate(geomtools::vector_3d & speed_,
                         geomtools::vector_3d & vtx_,
                         double & time_)
  {
    speed_ = _vbeta_ * CLHEP::c_light;
    geomtools::invalidate(vtx_);
    vtx_.set(0.0, 0.0, 1.0 * CLHEP::mm);
    time_ = 1.0 * CLHEP::ns;
    return;
  }

private:

  geomtools::vector_3d _vbeta_;  //!< Beta vector

  GENBB_LBG_REGISTRATION_INTERFACE(lbs);
};

GENBB_LBG_REGISTRATION_IMPLEMENT(lbs, "lbs");

int main(int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'genbb::lorentz_boost_wrapper'!" << std::endl;

    bool debug = false;
    int  many = 1;
    bool randomized_direction = false;
    bool cone_direction = false;
    bool energy_range_mode = false;
    char boost_dir = 'x';
    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug"))  {
          debug = true;
        } else if ((option == "-m") || (option == "--many")) {
          many *= 10;
        } else if ((option == "-r") || (option == "--randomized-direction")) {
          randomized_direction = true;
        } else if ((option == "-c") || (option == "--cone-direction"))  {
          cone_direction = true;
        } else if ((option == "-e") || (option == "--energy-range"))  {
          energy_range_mode = true;
        } else if (option == "--xboost") {
          boost_dir = 'x';
        } else if (option == "--yboost") {
          boost_dir = 'y';
        } else if (option == "--zboost") {
          boost_dir = 'z';
        } else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    genbb::single_particle_generator SPG;

    // Build a configuration properties container:
    datatools::properties config;
    if (debug) config.store_flag("debug");
    config.store("assign_generation_ids", true);

    /* The seed of the random number generator */
    config.store("seed", 314159);

    /* Default momentum direction is along the Z axis */
    if (randomized_direction) {
      config.store_flag("randomized_direction");
    } else if (cone_direction) {
      config.store_flag("cone_direction");
      config.store("angle_unit", "degree");
      config.store("cone.min_angle", 25.0);
      config.store("cone.max_angle", 35.0);
      config.store("cone.axis", "z");
      config.change("cone.axis", "30 45 degree");
    } else {
      config.store_flag("z_axis_direction");
    }
    config.tree_dump(std::clog, "Event generator configuration : ");

    /* Possible values are:
     *   "electron" or "e-"
     *   "positron" or "e+"
     *   "gamma"
     *   "alpha"
     *   "neutron"
     *   "protron"
     *   a few others...
     */
    config.store("particle_name", "electron");

    /* Possible values are:
     *  - "gaussian_energy" and required properties are:
     *    - "mean_energy"
     *    - "sigma_energy"
     *  - "energy_range" and required properties are:
     *    - "min_energy"
     *    - "max_energy"
     *  - "monokinetic" and required propertie is:
     *    - "energy"
     */
    config.store("mode", "monokinetic");

    /* Possible values are: "keV", "MeV"... */
    config.store("energy_unit", "keV");
    config.store("energy", 1000.0);

    if (energy_range_mode) {
      config.change("mode", "energy_range");
      config.store("min_energy", 950.0);
      config.store("max_energy", 1050.0);
    }

    if (debug) config.tree_dump(clog, "Configuration: ", "debug: ");

    // Configure the event generator:
    SPG.initialize_standalone(config);

    // A generator of Lorentz Boost
    lbs zLBS(0.0, 0.0, 0.5);
    zLBS.initialize_simple();

    // Another generator of Lorentz Boost
    lbs xLBS(0.5, 0.0, 0.0);
    xLBS.initialize_simple();

    genbb::lorentz_boost_wrapper LBW;
    LBW.set_generator(SPG);
    // LBW.set_lorentz_boost_generator(xLBS);
    // LBW.initialize_simple();

    datatools::properties lbw_config;
    lbw_config.store("lbg.id", "lbs");
    if (boost_dir == 'x') {
      lbw_config.store_flag("lbg.config.xaxis");
    } else if (boost_dir == 'y') {
      lbw_config.store_flag("lbg.config.yaxis");
    } else if (boost_dir == 'z') {
      lbw_config.store_flag("lbg.config.zaxis");
    }
    lbw_config.store("lbg.config.beta", 0.5);
    LBW.initialize_standalone(lbw_config);

    // Shoot events:
    size_t max_count = 10;
    max_count *= many;
    genbb::primary_event pe; // working primary event
    for (int i = 0; i < (int) max_count; i++) {
      std::clog << "Count : " << i << std::endl;
      LBW.load_next(pe);
      if (debug) pe.dump();
      // Print the kinetic energy from the only particle in each event:
      std::cout << pe.get_particles().front().get_kinetic_energy()
           << ' ' << pe.get_particles().front().get_momentum().x()
           << ' ' << pe.get_particles().front().get_momentum().y()
           << ' ' << pe.get_particles().front().get_momentum().z()
           << std::endl;
    }

    if (debug) std::clog << "debug: " << "The end." << std::endl;

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
