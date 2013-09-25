// -*- mode: c++ ; -*-
// single_particle_generator.cc
/*
 * Copyright 2007-2013 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <genbb_help/single_particle_generator.h>

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>

#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/exception.h>

#include <geomtools/utils.h>
#include <genbb_help/primary_event.h>

namespace genbb {

  using namespace std;

  GENBB_PG_REGISTRATION_IMPLEMENT(single_particle_generator,"genbb::single_particle_generator");

  bool single_particle_generator::is_debug () const
  {
    return _debug_;
  }

  void single_particle_generator::set_debug (bool new_value_)
  {
    _debug_ = new_value_;
    return;
  }

  bool single_particle_generator::can_external_random () const
  {
    return true;
  }

  void single_particle_generator::set_direction_mode (int dm_)
  {
    _direction_mode_ = dm_;
    return;
  }

  void single_particle_generator::set_randomized_direction (bool rd_)
  {
    DT_THROW_IF(_initialized_,logic_error, "Operation prohibited ! Object is locked/initialized !");
    if (rd_) {
      _direction_mode_ = DIRECTION_RANDOMIZED;
    } else {
      _direction_mode_ = DIRECTION_DEFAULT;
    }
    return;
  }

  bool single_particle_generator::is_randomized_direction () const
  {
    return _direction_mode_ == DIRECTION_RANDOMIZED;
  }

  void single_particle_generator::set_z_direction (bool zd_)
  {
    DT_THROW_IF(_initialized_,logic_error, "Operation prohibited ! Object is locked/initialized !");
    if (zd_) {
      _direction_mode_ = DIRECTION_Z_AXIS;
    } else {
      _direction_mode_ = DIRECTION_DEFAULT;
    }
    return;
  }

  bool single_particle_generator::is_z_direction () const
  {
    return _direction_mode_ == DIRECTION_Z_AXIS;
  }

  void single_particle_generator::set_cone_direction (bool rd_)
  {
    DT_THROW_IF(_initialized_,logic_error, "Operation prohibited ! Object is locked/initialized !");
    if (rd_) {
      _direction_mode_ = DIRECTION_CONE;
    } else {
      _direction_mode_ = DIRECTION_DEFAULT;
    }
    return;
  }

  bool single_particle_generator::is_cone_direction () const
  {
    return _direction_mode_ == DIRECTION_CONE;
  }

  void single_particle_generator::set_cone_max_angle(double ca_)
  {
    _cone_max_angle_ = ca_;
    return;
  }

  double single_particle_generator::get_cone_max_angle() const
  {
    return _cone_max_angle_;
  }

  void single_particle_generator::set_cone_min_angle(double ca_)
  {
    _cone_min_angle_ = ca_;
    return;
  }

  double single_particle_generator::get_cone_min_angle() const
  {
    return _cone_min_angle_;
  }

  void single_particle_generator::set_cone_axis (const geomtools::vector_3d & axis_)
  {
    DT_THROW_IF (axis_.mag() == 0.0, std::logic_error, "Null cone axis !");
    DT_THROW_IF (! geomtools::is_valid(axis_), std::logic_error, "Invalid cone axis !");
    _cone_axis_ = axis_.unit();
    return;
  }

  const geomtools::vector_3d & single_particle_generator::get_cone_axis() const
  {
    return _cone_axis_;
  }

  bool single_particle_generator::is_initialized () const
  {
    return _initialized_;
  }

  const string & single_particle_generator::get_particle_name () const
  {
    return _particle_name_;
  }

  void single_particle_generator::set_particle_name (const string & new_value_)
  {
    DT_THROW_IF(_initialized_,logic_error, "Operation prohibited ! Object is locked/initialized !");
    _particle_name_ = new_value_;
    return;
  }

  double single_particle_generator::get_particle_mass () const
  {
    return _particle_mass_;
  }

  bool single_particle_generator::particle_name_is_valid (const string & particle_name_)
  {
    if (particle_name_ == "electron") return true;
    if (particle_name_ == "e-")       return true;
    if (particle_name_ == "positron") return true;
    if (particle_name_ == "e+")       return true;
    if (particle_name_ == "gamma")    return true;
    if (particle_name_ == "alpha")    return true;
    if (particle_name_ == "neutron")  return true;
    if (particle_name_ == "proton")   return true;
    if (particle_name_ == "mu-")      return true;
    if (particle_name_ == "mu+")      return true;
    return false;
  }

  double single_particle_generator::get_particle_mass_from_label (const string & particle_name_)
  {
    double mass;
    datatools::invalidate (mass);

    if (particle_name_ == "electron" ||
        particle_name_ == "positron" ||
        particle_name_ == "e+"       ||
        particle_name_ == "e-") {
      mass = CLHEP::electron_mass_c2;
    }

    if (particle_name_ == "gamma") {
      mass = 0.0 * CLHEP::eV;
    }

    if (particle_name_ == "alpha") {
      mass = 3.727417 * CLHEP::GeV;
    }

    if (particle_name_ == "neutron") {
      mass = 939.565560 * CLHEP::MeV;
    }

    if (particle_name_ == "proton") {
      mass = 938.272013 * CLHEP::MeV;
    }

    if (particle_name_ == "mu-" ||
        particle_name_ == "mu+") {
      mass = 105.658369 * CLHEP::MeV;
    }

    return mass;
  }

  int single_particle_generator::get_mode () const
  {
    return _mode_;
  }

  void single_particle_generator::set_mode (int new_value_)
  {
    DT_THROW_IF(_initialized_,logic_error, "Operation prohibited ! Object is locked/initialized !");
    _mode_ = new_value_;
    return;
  }

  double single_particle_generator::get_mean_energy () const
  {
    return _mean_energy_;
  }

  void single_particle_generator::set_mean_energy (double mean_, double sigma_)
  {
    DT_THROW_IF(_initialized_,logic_error, "Operation prohibited ! Object is locked/initialized !");
    DT_THROW_IF (mean_ <= 0.0, logic_error, "Invalid mean energy value !");
    _mean_energy_ = mean_;
    DT_THROW_IF (sigma_ < 0.0, logic_error, "Invalid sigma energy value !");
    _sigma_energy_ = sigma_;
    return;
  }

  double single_particle_generator::get_sigma_energy () const
  {
    return _sigma_energy_;
  }

  double
  single_particle_generator::get_min_energy () const
  {
    return _min_energy_;
  }

  void single_particle_generator::set_energy_range (double min_, double max_)
  {
    DT_THROW_IF(_initialized_,logic_error, "Operation prohibited ! Object is locked/initialized !");
    DT_THROW_IF (min_ < 0.0, logic_error, "Invalid min energy value (0 <= min energy) !");
    DT_THROW_IF (max_ < min_, logic_error, "Invalid max energy value (0 <= min energy < max energy) !");
    _min_energy_ = min_;
    _max_energy_ = max_;
    return;
  }

  double single_particle_generator::get_max_energy () const
  {
    return _max_energy_;
  }

  void single_particle_generator::set_energy_spectrum_filename (const string & filename_)
  {
    DT_THROW_IF(_initialized_,logic_error, "Operation prohibited ! Object is locked/initialized !");
    _energy_spectrum_filename_ = filename_;
    return;
  }

  // ctor:
  single_particle_generator::single_particle_generator () : i_genbb ()
  {
    _debug_ = false;
    _initialized_ = false;

    _particle_name_ = "";
    _particle_type_ = primary_particle::PARTICLE_UNDEFINED;
    datatools::invalidate (_particle_mass_);

    _mode_ = MODE_INVALID;
    _mean_energy_ = 0.0;
    datatools::invalidate (_mean_energy_);
    _sigma_energy_ = 0.0;
    _min_energy_ = 0.0;
    datatools::invalidate (_min_energy_);
    _max_energy_ = 0.0;
    datatools::invalidate (_max_energy_);

    // GSL (mygsl::tabulated_function class)
    _spectrum_interpolation_name_ = mygsl::tabulated_function::linear_interp_name ();

    _direction_mode_ = DIRECTION_DEFAULT;
    _cone_max_angle_ = 0.0 * CLHEP::degree;
    _cone_min_angle_ = 0.0 * CLHEP::degree;
    _cone_axis_.set(0.0, 0.0, 1.0);

    _seed_ = 0;
    return;
  }

  // dtor:
  single_particle_generator::~single_particle_generator ()
  {
    if (_initialized_) {
      reset ();
    }
    return;
  }

  const mygsl::rng & single_particle_generator::get_random () const
  {
    if (has_external_random ()) {
      return get_external_random ();
    }
    return _random_;
  }

  mygsl::rng & single_particle_generator::grab_random ()
  {
    if (has_external_random ()) {
      return grab_external_random ();
    }
    return _random_;
  }

  void single_particle_generator::_at_reset_ ()
  {
    _mode_ = MODE_INVALID;

    _particle_name_ = "";
    _particle_type_ = primary_particle::PARTICLE_UNDEFINED;
    datatools::invalidate (_particle_mass_);

    _mean_energy_ = 0.0;
    datatools::invalidate (_mean_energy_);
    _sigma_energy_ = 0.0;
    datatools::invalidate (_min_energy_);
    datatools::invalidate (_max_energy_);

    _vnm_.reset ();
    _energy_spectrum_.reset ();
    _energy_spectrum_filename_ = "";
    _spectrum_interpolation_name_ = mygsl::tabulated_function::linear_interp_name ();

    _direction_mode_ = DIRECTION_DEFAULT;
    _cone_max_angle_ = 0.0 * CLHEP::degree;
    _cone_min_angle_ = 0.0 * CLHEP::degree;
    _cone_axis_.set(0.0, 0.0, 1.0);

    if (_random_.is_initialized ()) {
      _random_.reset ();
    }
    _seed_ = 0;

    return;
  }

  void single_particle_generator::reset ()
  {
    if (! _initialized_) return;
    _at_reset_ ();
    _initialized_ = false;
    return;
  }

  void single_particle_generator::initialize (const datatools::properties & config_,
                                              datatools::service_manager & service_manager_,
                                              detail::pg_dict_type & dictionary_)
  {
    DT_THROW_IF(_initialized_,logic_error, "Operation prohibited ! Object is already initialized !");

    _initialize_base(config_);

    if (config_.has_flag ("debug")) {
      set_debug (true);
    }

    if (! has_external_random ()) {
      DT_THROW_IF (! config_.has_key ("seed"), logic_error,
                   "Missing 'seed' property for particle generator '" << get_name() << "' !");
      long seed = config_.fetch_integer ("seed");
      DT_THROW_IF (seed < 0, logic_error, "Invalid seed value (>=0) for particle generator '" << get_name() << "' !");
      _seed_ = seed;
    }

    DT_THROW_IF(!config_.has_key ("particle_name"), logic_error,
                "Missing 'particle_name' property for particle generator '" << get_name() << "' !");
    string particle_name = config_.fetch_string ("particle_name");
    DT_THROW_IF (! single_particle_generator::particle_name_is_valid (particle_name),
                 logic_error,
                 "Invalid particle name '" << particle_name << "' !");
    set_particle_name (particle_name);

    if (config_.has_key ("emission_direction")) {
      std::string emission_direction = config_.fetch_string("emission_direction");
      if (emission_direction == "z_axis") {
        set_z_direction(true);
      } else if (emission_direction == "randomized") {
        set_randomized_direction (true);
      } else if (emission_direction == "cone") {
        set_cone_direction (true);
      } else {
        DT_THROW_IF(true, std::logic_error,
                    "Invalid 'emission_direction' property for particle generator '" << get_name() << "' !");
      }
    } else if (config_.has_flag ("z_axis_direction")) {
      set_z_direction (false);
    } else if (config_.has_flag ("randomized_direction")) {
      set_randomized_direction (true);
    } else if (config_.has_flag ("cone_direction")) {
      set_cone_direction (true);
    }

    double angle_unit = CLHEP::degree;
    if (config_.has_key ("angle_unit")) {
      std::string angle_unit_str = config_.fetch_string ("angle_unit");
      angle_unit = datatools::units::get_angle_unit_from (angle_unit_str);
    }

    if (is_cone_direction()) {
      if (config_.has_key ("cone.max_angle")) {
        double cone_max_angle = config_.fetch_real ("cone.max_angle");
        if (! config_.has_explicit_unit("cone.max_angle")) cone_max_angle *= angle_unit;
        set_cone_max_angle(cone_max_angle);
        if (config_.has_key ("cone.min_angle")) {
          double cone_min_angle = config_.fetch_real ("cone.min_angle");
          if (! config_.has_explicit_unit("cone.min_angle")) cone_min_angle *= angle_unit;
          set_cone_min_angle(cone_min_angle);
        }
      }

      if (config_.has_key ("cone.axis")) {
        std::string cone_axis_str = config_.fetch_string ("cone.axis");
        geomtools::vector_3d cone_axis;
        if (cone_axis_str == "x" || cone_axis_str == "X" || cone_axis_str == "+x" || cone_axis_str == "+X") {
          cone_axis.set (1.0, 0.0, 0.0);
        } else if (cone_axis_str == "y" || cone_axis_str == "Y" || cone_axis_str == "+y" || cone_axis_str == "+Y") {
          cone_axis.set (0.0, 1.0, 0.0);
        } else if (cone_axis_str == "z" || cone_axis_str == "Z" || cone_axis_str == "+z" || cone_axis_str == "+Z") {
          cone_axis.set (0.0, 0.0, 1.0);
        } else if (cone_axis_str == "-x" || cone_axis_str == "-X") {
          cone_axis.set (-1.0, 0.0, 0.0);
        } else if (cone_axis_str == "-y" || cone_axis_str == "-Y") {
          cone_axis.set (0.0, -1.0, 0.0);
        } else if (cone_axis_str == "-z" || cone_axis_str == "-Z") {
          cone_axis.set (0.0, 0.0, -1.0);
        } else {
          double phi, theta;
          double angle_unit2 = angle_unit;
          std::string angle_unit2_str;
          std::istringstream iss(cone_axis_str);
          iss >> phi >> theta >> ws >> angle_unit2_str;
          if (angle_unit2_str.empty()) {
            angle_unit2 = datatools::units::get_angle_unit_from (angle_unit2_str);
          }
          DT_THROW_IF (!iss, std::logic_error,
                       "Invalid format for phi and/or theta angles !");
          phi *= angle_unit2;
          theta *= angle_unit2;
          cone_axis.set(0.0,0.0,1.0);
          cone_axis.setTheta(theta);
          cone_axis.setPhi(phi);
        }
        set_cone_axis(cone_axis);
      }
    }

    DT_THROW_IF (! config_.has_key ("mode"), logic_error, "Missing 'mode' property !");
    string mode_str = config_.fetch_string ("mode");
    if (mode_str == "gaussian_energy") {
      set_mode (MODE_GAUSSIAN_ENERGY);
    } else if (mode_str == "energy_range") {
      set_mode (MODE_ENERGY_RANGE);
    } else if (mode_str == "monokinetic") {
      set_mode (MODE_MONOKINETIC);
    } else if (mode_str == "spectrum") {
      set_mode (MODE_SPECTRUM);
      _spectrum_mode_ = SPECTRUM_MODE_TABFUNC;
      if (config_.has_key ("spectrum.mode")) {
        string spectrum_mode_str = config_.fetch_string ("spectrum.mode");
        if (spectrum_mode_str == "tabulated_function") {
          _spectrum_mode_ = SPECTRUM_MODE_TABFUNC;
        } else if (spectrum_mode_str == "histogram_pdf") {
          _spectrum_mode_ = SPECTRUM_MODE_HISTPDF;
        }
      }

    } else {
      DT_THROW_IF (true, logic_error, "Invalid mode '" << mode_str << "' !");
    }

    double energy_unit = CLHEP::keV;

    if (config_.has_key ("energy_unit")) {
      string unit_str = config_.fetch_string ("energy_unit");
      energy_unit = datatools::units::get_energy_unit_from (unit_str);
    }

    if (_mode_ == MODE_MONOKINETIC) {
      double energy = 0.0;
      DT_THROW_IF (! config_.has_key ("energy"), logic_error, "Missing 'energy' property !");
      energy = config_.fetch_real ("energy");
      if (! config_.has_explicit_unit ("energy")) energy *= energy_unit;
      set_mean_energy (energy, 0.0);
    }

    if (_mode_ == MODE_GAUSSIAN_ENERGY) {
      double mean_energy = 0.0;
      double sigma_energy = 0.0;
      DT_THROW_IF (! config_.has_key ("mean_energy"), logic_error, "Missing 'mean_energy' property !");
      mean_energy = config_.fetch_real ("mean_energy");
      if (! config_.has_explicit_unit ("mean_energy")) mean_energy *= energy_unit;

      if (config_.has_key ("sigma_energy")) {
        sigma_energy = config_.fetch_real ("sigma_energy");
        if (! config_.has_explicit_unit ("sigma_energy")) sigma_energy *= energy_unit;
      }

      set_mean_energy (mean_energy, sigma_energy);
    }

    if (_mode_ == MODE_ENERGY_RANGE) {
      double min_energy = 0.0;
      double max_energy = 0.0;
      DT_THROW_IF (! config_.has_key ("max_energy"), logic_error, "Missing 'max_energy' property !");
      max_energy = config_.fetch_real ("max_energy");
      if (! config_.has_explicit_unit ("max_energy")) max_energy *= energy_unit;
      DT_THROW_IF (! config_.has_key ("min_energy"), logic_error, "Missing 'min_energy' property !");
      min_energy = config_.fetch_real ("min_energy");
      if (! config_.has_explicit_unit ("min_energy")) min_energy *= energy_unit;
      set_energy_range (min_energy, max_energy);
    }

    if (_mode_ == MODE_SPECTRUM) {
      if (config_.has_key ("spectrum.data_file")) {
        string spectrum_filename = config_.fetch_string ("spectrum.data_file");
        set_energy_spectrum_filename (spectrum_filename);
      }
      double min_energy = _min_energy_;
      double max_energy = _max_energy_;

      if (config_.has_key ("spectrum.min_energy")) {
        min_energy = config_.fetch_real ("spectrum.min_energy");
        if (! config_.has_explicit_unit ("spectrum.min_energy")) min_energy *= energy_unit;
      }

      if (config_.has_key ("spectrum.min_energy")) {
        max_energy = config_.fetch_real ("spectrum.max_energy");
        if (! config_.has_explicit_unit ("spectrum.max_energy")) max_energy *= energy_unit;
      }

      set_energy_range (min_energy, max_energy);
    }

    _at_init_ ();

    _initialized_  = true;
    return;
  }

  bool single_particle_generator::has_next ()
  {
    return true;
  }

  void single_particle_generator::_load_next (primary_event & event_,
                                              bool compute_classification_)
  {
    if (_debug_) {
      clog << "debug: " << "genbb::single_particle_generator::_load_next: "
           << "Entering..."
           << endl;
    }
    DT_THROW_IF (! _initialized_, logic_error, "Generator is notlocked/initialized !");
    event_.reset ();

    double kinetic_energy = -1.0;
    double mass = _particle_mass_;

    if (_mode_ == MODE_MONOKINETIC) {
      kinetic_energy = _mean_energy_;
    }

    if (_mode_ == MODE_GAUSSIAN_ENERGY) {
      while (kinetic_energy <= 0.0) {
        kinetic_energy = grab_random ().gaussian (_mean_energy_, _sigma_energy_);
      }
    }

    if (_mode_ == MODE_ENERGY_RANGE) {
      kinetic_energy = grab_random ().flat (_min_energy_, _max_energy_);
    }

    if (_mode_ == MODE_SPECTRUM) {
      if (_spectrum_mode_ == SPECTRUM_MODE_TABFUNC) {
        kinetic_energy = _vnm_.shoot (grab_random ());
      } else if (_spectrum_mode_ == SPECTRUM_MODE_HISTPDF) {
        kinetic_energy = _energy_histo_pdf_.sample (grab_random ());
      }
    }

    double momentum = sqrt (kinetic_energy * (kinetic_energy + 2 * mass));
    double px, py, pz;
    px = 0.0;
    py = 0.0;
    pz = momentum;

    event_.set_time (0.0 * CLHEP::second);
    primary_particle pp;
    pp.set_type (_particle_type_);
    pp.set_time (0.0 * CLHEP::second);
    geomtools::vector_3d p (px, py, pz);
    pp.set_momentum (p);
    event_.add_particle (pp);

    if (is_randomized_direction()) {
      double phi = grab_random ().flat (0.0, 360.0 * CLHEP::degree);
      double cos_theta = grab_random ().flat (-1.0, +1.0);
      double theta = std::acos (cos_theta);
      double psi = grab_random ().flat (0.0, 360.0 * CLHEP::degree);
      event_.rotate (phi, theta, psi);
    } else if (is_cone_direction ()) {
      double phi = grab_random ().flat (0.0, 360.0 * CLHEP::degree);
      double cos_theta = grab_random ().flat (std::cos(_cone_max_angle_), std::cos(_cone_min_angle_));
      double theta = std::acos (cos_theta);
      double psi = grab_random ().flat (0.0, 360.0 * CLHEP::degree);
      event_.rotate (phi, theta, psi);
      double theta_dir = _cone_axis_.getTheta();
      double phi_dir = _cone_axis_.getPhi();
      event_.rotate (phi_dir, theta_dir, 0.0);
    }

    if (compute_classification_) {
      event_.compute_classification ();
    }

    if (_debug_) {
      clog << "debug: " << "genbb::single_particle_generator::_load_next: "
           << "Exiting."
           << endl;
    }
    return;
  }

  void single_particle_generator::_init_energy_histo_pdf ()
  {
    using namespace std;
    string filename = _energy_spectrum_filename_;
    datatools::fetch_path_with_env (filename);
    ifstream ifile (filename.c_str());
    DT_THROW_IF (! ifile, logic_error, "Cannot open data file '" << _energy_spectrum_filename_ << "' !");

    double energy_unit = CLHEP::MeV;
    size_t nbins = 0;
    double min = 0.0, max = 0.0;
    // load histo
    while (! ifile.eof ()) {
      string line;
      getline (ifile, line);
      if (line.empty ())  {
        continue;
      }
      {
        istringstream lineiss (line);
        string word;
        lineiss >> word;
        if ((word.length () > 0) && (word[0] == '#')) {
          if (word == "#@limits") {
            lineiss >> nbins >> min >> max;
            // initalize histo
            _energy_histo_.init (nbins, min*energy_unit, max*energy_unit);
          } else if (word == "#@energy_unit") {
            string energy_unit_str;
            lineiss >> energy_unit_str;
            energy_unit = datatools::units::get_energy_unit_from (energy_unit_str);
          }
          continue;
        }
      }

      DT_THROW_IF (!nbins, logic_error, "Limits to histogram are not given in data file '"
                   << _energy_spectrum_filename_ << "' !");

      istringstream lineiss (line);
      double xmin, xmax;
      double weight;
      lineiss >> xmin >> xmax >> weight;
      DT_THROW_IF (! lineiss, logic_error, "Format error in data file '"
                   << _energy_spectrum_filename_ << "' !");

      xmin *= energy_unit;
      xmax *= energy_unit;

      DT_THROW_IF (xmin < 0.0 || xmax < 0.0, out_of_range,
                   "Invalid energy value < 0) !");
      DT_THROW_IF (weight < 0.0, out_of_range, "Invalid spectrum value (" << weight << " < 0) !");

      // filling histogram
      const double mean_energy = (xmin + xmax)/2.0;
      if (mean_energy < _min_energy_ || mean_energy > _max_energy_)
        continue;

      _energy_histo_.fill (mean_energy, weight);
    }

    // Calculate histogram PDF
    _energy_histo_pdf_.init (_energy_histo_);
    return;
  }

  void single_particle_generator::_init_energy_spectrum ()
  {
    DT_THROW_IF (_energy_spectrum_.is_table_locked (), logic_error,
                 "Tabulated energy spectrum is already locked !");
    using namespace std;
    string filename = _energy_spectrum_filename_;
    datatools::fetch_path_with_env (filename);
    ifstream ifile;
    ifile.open (filename.c_str ());
    DT_THROW_IF (! ifile, logic_error, "Cannot open interpolation spectrum data file '"
                 << _energy_spectrum_filename_ << "' !");
    string interpolator_name;
    double energy_unit = CLHEP::MeV;
    double ymax = -1.0;
    while (! ifile.eof ()) {
      string line;
      getline (ifile, line);
      if (line.empty ()) {
        continue;
      }
      {
        istringstream lineiss (line);
        string word;
        lineiss >> word;
        if ((word.length () > 0) && (word[0] == '#')) {
          if (word == "#@interpolation_name") {
            lineiss >> interpolator_name;
          } else if (word == "#@energy_unit") {
            string energy_unit_str;
            lineiss >> energy_unit_str;
            energy_unit = datatools::units::get_energy_unit_from (energy_unit_str);
          }
          continue;
        }
      }
      istringstream lineiss (line);
      double x, y;
      lineiss >> x >> y;
      DT_THROW_IF (! lineiss, logic_error, "Format error in interpolation data file '"
                   << _energy_spectrum_filename_ << "' !");
      x *= energy_unit;
      DT_THROW_IF (x < 0.0, out_of_range, "Invalid energy value (" << x << " < 0) !");
      DT_THROW_IF (y < 0.0, out_of_range, "Invalid spectrum value (" << y << " < 0) !");

      if (x < _min_energy_ || x > _max_energy_)
        continue;

      _energy_spectrum_.add_point (x, y, false);
      if (y > ymax) ymax = y;
    }
    if (interpolator_name.empty ()) {
      interpolator_name = _spectrum_interpolation_name_;
    }
    _energy_spectrum_.lock_table (interpolator_name);
    _vnm_.init (_energy_spectrum_.x_min (),
                _energy_spectrum_.x_max (),
                _energy_spectrum_,
                ymax * 1.001);
    return;
  }

  void single_particle_generator::_at_init_ ()
  {
    _particle_type_ = primary_particle::get_particle_type_from_label (_particle_name_);
    _particle_mass_ = get_particle_mass_from_label (_particle_name_);
    DT_THROW_IF (! datatools::is_valid (_particle_mass_), logic_error,
                 "Particle mass is not defined !");
    DT_THROW_IF (_cone_min_angle_ > _cone_max_angle_, out_of_range,
                 "Invalid  cone angle range (" << _cone_min_angle_ << ">" << _cone_max_angle_ << ") !");

    if (_mode_ == MODE_SPECTRUM) {
      if (_spectrum_mode_ == SPECTRUM_MODE_TABFUNC) {
        _init_energy_spectrum ();
      } else if (_spectrum_mode_ == SPECTRUM_MODE_HISTPDF) {
        _init_energy_histo_pdf ();
      }
    }

    if (! has_external_random ()) {
      _random_.init ("taus2", _seed_);
    }
    return;
  }

} // end of namespace genbb

/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::genbb::single_particle_generator' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::genbb::single_particle_generator,ocd_)
{
  ocd_.set_class_name ("genbb::single_particle_generator");
  ocd_.set_class_description ("A single particle generator");
  ocd_.set_class_library("genbb_help");
  ocd_.set_class_documentation("The ``genbb::single_particle_generator`` enables to          \n"
                               "randomize the initial kinematics of an unique particle       \n"
                               "as the input of some Monte-Carlo process (example: Geant4).  \n"
                               "It can be typically initialized from an ASCII configuration  \n"
                               "file. The generator can be given an external PRNG provided   \n"
                               "by the user or use its own PRNG.                             \n"
                              );
  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("logging.priority")
      .set_terse_description("Set the logging priority threshold")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Allowed values are:                           \n"
                            "                                              \n"
                            " * ``\"trace\"``                              \n"
                            " * ``\"debug\"``                              \n"
                            " * ``\"information\"``                        \n"
                            " * ``\"notice\"``                             \n"
                            " * ``\"warning\"``                            \n"
                            " * ``\"error\"``                              \n"
                            " * ``\"critical\"``                           \n"
                            " * ``\"fatal\"``                              \n"
                            "                                              \n"
                            "Default value: ``\"warning\"``                \n"
                            "                                              \n"
                            "Example::                                     \n"
                            "                                              \n"
                            "  logging.priority: string = \"notice\"       \n"
                            "                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("seed")
      .set_terse_description("Embeded PRNG's seed")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_complex_triggering_conditions(true)
      .set_long_description("The seed of the embeded PRNG.                 \n"
                            "Not used if some external PRNG is used.       \n"
                            "Example::                                     \n"
                            "                                              \n"
                            "  seed: integer = 314159                      \n"
                            "                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("particle_name")
      .set_terse_description("The name of the generated particle")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the generated particle must be taken from the \n"
                            "following list:                               \n"
                            "                                              \n"
                            "  * ``\"electron\"`` or ``\"e-\"``            \n"
                            "  * ``\"positron\"`` or ``\"e+\"``            \n"
                            "  * ``\"gamma\"``                             \n"
                            "  * ``\"alpha\"``                             \n"
                            "  * ``\"neutron\"``                           \n"
                            "  * ``\"proton\"``                            \n"
                            "  * ``\"mu-\"``                               \n"
                            "  * ``\"mu+\" ``                              \n"
                            "                                              \n"
                            "                                              \n"
                            "Example::                                     \n"
                            "                                              \n"
                            "  particle_name: string = \"gamma\"           \n"
                            "                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("emission_direction")
      .set_terse_description("The label representing the emission direction mode")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The label representing the emission direction mode \n"
                            "must be taken from the following list:             \n"
                            "                                                   \n"
                            "  * \"z_axis\" : the particle is emitted along the positive Z axis.        \n"
                            "  * \"randomized\" : the particle is emitted in a random direction (4xPi). \n"
                            "  * \"cone\" : the particle is emitted within an arbitrary cone of         \n"
                            "    which the characteristics (angles and axis) must be provided.          \n"
                            "                                                   \n"
                            "Examples::                                         \n"
                            "                                                   \n"
                            "  emission_direction : string = \"randomized\"     \n"
                            "                                                   \n"
                            "or:                                                \n"
                            "                                                   \n"
                            "  emission_direction : string = \"cone\"           \n"
                            "  cone.min_angle : real as angle =  0 degree       \n"
                            "  cone.max_angle : real as angle = 25 degree       \n"
                            "  cone.axis : string = \"30 45 degree\"            \n"
                            "                                                   \n"
                            )
       ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode")
      .set_terse_description("The energy spectrum mode")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The label representing the energy dspectrum mode \n"
                            "must be taken from the following list:             \n"
                            "                                                   \n"
                            "  * \"monokinetic\" : the particle is emitted with a monokinetic energy        \n"
                            "  * \"gaussian_energy\" : the particle is emitted with a random kinetic energy \n"
                            "    of which the characteristics must be provided.                             \n"
                            "  * \"energy_range\" : the particle is emitted with a flat distributed         \n"
                            "    kinetic energy in a range of which the characteristics must be provided.   \n"
                            "  * \'spectrum\" : the particle is emitted with a kinetic energ following      \n"
                            "     a PDF read from a file that must be provided.                             \n"
                            "                                                   \n"
                            "Examples::                                         \n"
                            "                                                   \n"
                            "  mode : string = \"monokinetic\"                  \n"
                            "  energy_unit : string = \"keV\"                   \n"
                            "  energy      : real = 511.0                       \n"
                            "                                                   \n"
                            "or::                                               \n"
                            "                                                   \n"
                            "  mode : string = \"gaussian_energy\"              \n"
                            "  energy_unit : string = \"keV\"                   \n"
                            "  mean_energy  : real as energy  = 1.0 MeV         \n"
                            "  sigma_energy : real as energy = 100.0 keV        \n"
                            "  energy      : real = 511.0                       \n"
                            "                                                   \n"
                            "or::                                               \n"
                            "                                                   \n"
                            "  mode : string = \"energy_range\"                 \n"
                            "  energy_unit : string = \"keV\"                   \n"
                            "  min_energy : real = 50                           \n"
                            "  max_energy : real as energy = 3.0 MeV            \n"
                            "                                                   \n"
                            )
      ;

  }

  ocd_.set_configuration_hints ("Examples:                                                   \n"
                                "                                                            \n"
                                "Monokinetic gamma::                                         \n"
                                "                                                            \n"
                                "  logging.priority: string = \"notice\"                     \n"
                                "  seed: integer = 314159                                    \n"
                                "  particle_name: string = \"gamma\"                         \n"
                                "  emission_direction : string = \"randomized\"              \n"
                                "  mode : string = \"monokinetic\"                           \n"
                                "  energy_unit : string = \"keV\"                            \n"
                                "  energy      : real   = 511                                \n"
                                "                                                            \n"
                                "Gamma with gaussian energy::                                \n"
                                "                                                            \n"
                                "  logging.priority: string = \"notice\"                     \n"
                                "  seed: integer = 314159                                    \n"
                                "  particle_name: string = \"gamma\"                         \n"
                                "  emission_direction : string = \"randomized\"              \n"
                                "  mode : string = \"gaussian_energy\"                       \n"
                                "  energy_unit : string = \"keV\"                            \n"
                                "  mean_energy : real as energy = 1.0 MeV                    \n"
                                "  sigma_energy : real = 100.0                               \n"
                                "                                                            \n"
                                "Gamma with flat ranged energy::                             \n"
                                "                                                            \n"
                                "  logging.priority: string = \"notice\"                     \n"
                                "  seed: integer = 314159                                    \n"
                                "  particle_name: string = \"gamma\"                         \n"
                                "  emission_direction : string = \"randomized\"              \n"
                                "  mode : string = \"energy_range\"                          \n"
                                "  energy_unit : string = \"keV\"                            \n"
                                "  min_energy : real as energy = 50 keV                      \n"
                                "  max_energy : real as energy = 3.0 MeV                     \n"
                                "                                                            \n"
                                "Gamma with energy PDF from a spectrum::                     \n"
                                "                                                            \n"
                                "  logging.priority: string = \"notice\"                     \n"
                                "  seed: integer = 314159                                    \n"
                                "  particle_name: string = \"gamma\"                         \n"
                                "  emission_direction : string = \"cone\"                    \n"
                                "  angle_unit : string = \"degree\"                          \n"
                                "  cone.max_angle : real = 25 degree                         \n"
                                "  cone.min_angle : real = 0 degree                          \n"
                                "  cone.axis : string = \"20 45 degree\"                     \n"
                                "  mode : string = \"spectrum\"                              \n"
                                "  energy_unit : string = \"keV\"                            \n"
                                "  spectrum.data_file : string = \"energy_spectrum.data\"    \n"
                                "  spectrum.min_energy : real as energy = 300 keV            \n"
                                "  spectrum.max_energy : real as energy = 2.0 MeV            \n"
                                "                                                            \n"
                                );
  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(genbb::single_particle_generator,"genbb::single_particle_generator")

// end of single_particle_generator.cc
