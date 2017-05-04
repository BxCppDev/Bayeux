// crazy_gun.cc

// Ourselves:
#include <crazy_gun.hh>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/units.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/genbb_help:
#include <genbb_help/primary_event.h>

namespace genbb {

  namespace ex02 {

    // Class registration macro:
    GENBB_PG_REGISTRATION_IMPLEMENT(crazy_gun,"genbb::ex02::crazy_gun");

    void crazy_gun::_set_defaults()
    {
      // Force invalid values:
      _min_number_of_particles_ = 0;
      _max_number_of_particles_ = 0;
      return;
    }

    bool crazy_gun::can_external_random () const
    {
      return true;
    }

    void crazy_gun::set_min_number_of_particles(unsigned int n_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Generator is already initialized!");
      DT_THROW_IF(n_ < MIN_NUMBER_OF_PARTICLES, std::range_error,
                  "Invalid minimum number of particles!");
      _min_number_of_particles_ = n_;
      return;
    }

    unsigned int crazy_gun::get_min_number_of_particles() const
    {
      return _min_number_of_particles_;
    }

    void crazy_gun::set_max_number_of_particles(unsigned int n_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Generator is already initialized!");
      DT_THROW_IF(n_ > MAX_NUMBER_OF_PARTICLES, std::range_error,
                  "Invalid maximum number of particles!");
      _max_number_of_particles_ = n_;
      return;
    }

    unsigned int crazy_gun::get_max_number_of_particles() const
    {
      return _max_number_of_particles_;
    }

    crazy_gun::crazy_gun()
    {
      _initialized_ = false;
      _set_defaults();
      return;
    }

    crazy_gun::~crazy_gun()
    {
      if (_initialized_) {
        reset ();
      }
      return;
    }

    void crazy_gun::initialize(const datatools::properties & setup_,
                               datatools::service_manager & service_manager_,
                               detail::pg_dict_type & dictionary_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Generator is already initialized!");

      // Invoke initialization actions from the mother class:
      _initialize_base(setup_);

      DT_THROW_IF(! has_external_random (), std::logic_error,
                  "This generator needs an external PRNG!");

      if (_min_number_of_particles_ == 0) {
        if (setup_.has_key("min_number_of_particles")) {
          int dummy = setup_.fetch_integer("min_number_of_particles");
          DT_THROW_IF (dummy < MIN_NUMBER_OF_PARTICLES || dummy > MAX_NUMBER_OF_PARTICLES,
                       std::range_error,
                       "Invalid minimum number of particles!");
          set_min_number_of_particles((unsigned int) dummy);
        }
      }

      if (_max_number_of_particles_ == 0) {
        if (setup_.has_key("max_number_of_particles")) {
          int dummy = setup_.fetch_integer("max_number_of_particles");
          DT_THROW_IF (dummy < MIN_NUMBER_OF_PARTICLES || dummy > MAX_NUMBER_OF_PARTICLES,
                       std::range_error,
                       "Invalid maximum number of particles!");
          set_max_number_of_particles((unsigned int) dummy);
        }
      }

      // Default behaviour if not attributes are set (0):
      if (_min_number_of_particles_ == 0) {
        set_min_number_of_particles(MIN_NUMBER_OF_PARTICLES);
      }
      if (_max_number_of_particles_ == 0) {
        set_max_number_of_particles(_min_number_of_particles_);
      }

      // Check:
      DT_THROW_IF(_min_number_of_particles_ > _max_number_of_particles_,
                  std::range_error,
                  "Invalid range for the number of particles!");

      /* Possible other parsing and internal resources initialization... */

      // Initialization is terminated.
      _initialized_ = true;
      return;
    }

    void crazy_gun::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Generator is not initialized!");
      _initialized_ = false;
      _set_defaults(); // Revert to default values
      return;
    }

    bool crazy_gun::has_next()
    {
      // Always possible to request a new generated event:
      return true;
    }

    bool crazy_gun::is_initialized() const
    {
      return _initialized_;
    }

    void crazy_gun::_load_next(genbb::primary_event & event_,
                               bool compute_classification_)
    {
      DT_THROW_IF (! _initialized_, std::logic_error, "Generator is not initialized !");
      event_.reset ();
      mygsl::rng & prng = grab_external_random();

      event_.set_time (0.0 * CLHEP::second);
      unsigned int npart = _min_number_of_particles_
        + prng.uniform_int(_max_number_of_particles_ - _min_number_of_particles_);
      double time = 0.0;
      for (unsigned int ipart = 0; ipart < npart; ipart++) {
        genbb::primary_particle pp;
        double prob_part = prng.uniform();
        genbb::primary_particle::particle_type ptype = genbb::primary_particle::PARTICLE_UNDEFINED;
        if (prob_part < 0.35) {
          ptype = genbb::primary_particle::GAMMA;
        } else if (prob_part < 0.60) {
          ptype = genbb::primary_particle::ELECTRON;
        } else if (prob_part < 0.75) {
          ptype = genbb::primary_particle::ALPHA;
        } else if (prob_part < 0.85) {
          ptype = genbb::primary_particle::POSITRON;
        } else  if (prob_part < 0.95) {
          ptype = genbb::primary_particle::MUON_PLUS;
        } else {
          ptype = genbb::primary_particle::MUON_MINUS;
        }
        pp.set_type(ptype);

        double tau = 1.0 * CLHEP::ns;
        time += prng.exponential(1.0 / tau);
        pp.set_time(time);

        double kinetic_energy = prng.flat(0.5 * CLHEP::MeV, 10.0 * CLHEP::MeV);
        double mass = pp.get_mass();
        double momentum = std::sqrt(kinetic_energy * (kinetic_energy + 2 * mass));
        double px, py, pz;
        px = 0.0;
        py = 0.0;
        pz = momentum;
        geomtools::vector_3d p(px, py, pz);
        pp.set_momentum(p);
        event_.add_particle(pp);
      }

      // Randomize the direction of the event:
      double phi = prng.flat(0.0, 360.0 * CLHEP::degree);
      double cos_theta = prng.flat(-1.0, +1.0);
      double theta = std::acos(cos_theta);
      double psi = prng.flat(0.0, 360.0 * CLHEP::degree);
      event_.rotate(phi, theta, psi);

      // Compute classification string:
      if (compute_classification_) {
        event_.compute_classification();
      }

      return;
    }

  } // end of namespace ex02

} // end of namespace genbb
