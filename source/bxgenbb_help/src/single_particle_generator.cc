// -*- mode: c++ ; -*-
// single_particle_generator.cc
/*
 * Copyright 2007-2011 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Publi * License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Publi * License for more details.
 *
 * You should have received a copy of the GNU General Publi * License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <genbb_help/single_particle_generator.h>

namespace genbb {

  using namespace std;

  void single_particle_generator::_check_locked (const string & where_) const
  {
    if (__initialized)
      {
	ostringstream message;
	if (! where_.empty ())
	  {
	    message << where_ << ": ";
	  }
	message << "Operation prohibited ! Object is locked/initialized !";
	throw runtime_error (message.str ());
      }
    return;
  }

  bool single_particle_generator::is_debug () const
  {
    return __debug;
  }

  void single_particle_generator::set_debug (bool new_value_)
  {
    __debug = new_value_;
    return;
  }

  void single_particle_generator::set_randomized_direction (bool rd_)
  {
    _check_locked ("single_particle_generator::set_randomized_direction");
    __randomized_direction = rd_;
    return;
  }

  bool single_particle_generator::is_randomized_direction () const
  {
    return __randomized_direction;
  }

  bool single_particle_generator::is_initialized () const
  {
    return __initialized;
  }

  const string & single_particle_generator::get_particle_name () const
  {
    return __particle_name;
  }

  void single_particle_generator::set_particle_name (const string & new_value_)
  {
    _check_locked ("single_particle_generator::set_particle_name");
    __particle_name = new_value_;
    return;
  }

  double single_particle_generator::get_particle_mass () const
  {
    return __particle_mass;
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
    datatools::utils::invalidate (mass);

    if (particle_name_ == "electron" || particle_name_ == "positron"
	|| particle_name_ == "e+" || particle_name_ == "e-")
      {
	mass = CLHEP::electron_mass_c2;
      }

    if (particle_name_ == "gamma")
      {
	mass = 0.0 * CLHEP::eV;
      }

    if (particle_name_ == "alpha")
      {
	mass = 3.727417 * CLHEP::GeV;
      }

    if (particle_name_ == "neutron")
      {
	mass = 939.565560 * CLHEP::MeV;
      }

    if (particle_name_ == "proton")
      {
	mass = 938.272013 * CLHEP::MeV;
      }

    if (particle_name_ == "mu-" || particle_name_ == "mu+")
      {
	mass = 105.658369 * CLHEP::MeV;
      }

    return mass;
  }

  int single_particle_generator::get_mode () const
  {
    return __mode;
  }

  void single_particle_generator::set_mode (int new_value_)
  {
    _check_locked ("single_particle_generator::set_mode");
    __mode = new_value_;
    return;
  }

  double single_particle_generator::get_mean_energy () const
  {
    return __mean_energy;
  }

  void single_particle_generator::set_mean_energy (double mean_, double sigma_)
  {
    _check_locked ("single_particle_generator::set_mean_energy");
    if (mean_ <= 0.0)
      {
	throw runtime_error ("single_particle_generator::set_mean_energy: Invalid mean energy value !");
      }
    __mean_energy = mean_;
    if (sigma_ < 0.0)
      {
	throw runtime_error ("single_particle_generator::set_mean_energy: Invalid sigma energy value !");
      }
    __sigma_energy = sigma_;
    return;
  }

  double single_particle_generator::get_sigma_energy () const
  {
    return __sigma_energy;
  }

  double
  single_particle_generator::get_min_energy () const
  {
    return __min_energy;
  }

  void single_particle_generator::set_energy_range (double min_, double max_)
  {
    _check_locked ("single_particle_generator::set_min_energy");
    if (min_ < 0.0)
      {
	throw runtime_error ("single_particle_generator::set_energy_range: Invalid min energy value (0 <= min energy) !");
      }
    if (max_ < min_)
      {
	throw runtime_error ("single_particle_generator::set_energy_range: Invalid max energy value (0 <= min energy < max energy) !");
      }
    __min_energy = min_;
    __max_energy = max_;
    return;
  }

  double single_particle_generator::get_max_energy () const
  {
    return __max_energy;
  }

  void single_particle_generator::set_energy_spectrum_filename (const string & filename_)
  {
    _check_locked ("single_particle_generator::set_energy_spectrum_filename");
    __energy_spectrum_filename = filename_;
    return;
  }

  // ctor:
  single_particle_generator::single_particle_generator () : i_genbb ()
  {
    __debug = false;
    __initialized = false;

    __particle_name = "";
    __particle_type = primary_particle::PARTICLE_UNDEFINED;
    datatools::utils::invalidate (__particle_mass);

    __mode = MODE_INVALID;
    __mean_energy = 0.0;
    datatools::utils::invalidate (__mean_energy);
    __sigma_energy = 0.0;
    __min_energy = 0.0;
    datatools::utils::invalidate (__min_energy);
    __max_energy = 0.0;
    datatools::utils::invalidate (__max_energy);

    // GSL (mygsl::tabulated_function class)
    __spectrum_interpolation_name = mygsl::tabulated_function::LINEAR_INTERP_NAME;
    __randomized_direction = true;

    __seed = 0;
    return;
  }

  // dtor:
  single_particle_generator::~single_particle_generator ()
  {
    if (__initialized)
      {
	reset ();
      }
    return;
  }

  const mygsl::rng & single_particle_generator::get_random () const
  {
    return __random;
  }

  mygsl::rng & single_particle_generator::get_random ()
  {
    return __random;
  }

  void single_particle_generator::__at_reset ()
  {
    __mode = MODE_INVALID;

    __particle_name = "";
    __particle_type = primary_particle::PARTICLE_UNDEFINED;
    datatools::utils::invalidate (__particle_mass);

    __mean_energy = 0.0;
    datatools::utils::invalidate (__mean_energy);
    __sigma_energy = 0.0;
    datatools::utils::invalidate (__min_energy);
    datatools::utils::invalidate (__max_energy);

    __VNM.reset ();
    __energy_spectrum.reset ();
    __energy_spectrum_filename = "";
    __spectrum_interpolation_name = mygsl::tabulated_function::LINEAR_INTERP_NAME;

    __randomized_direction = true;

    __random.reset ();
    __seed = 0;

    return;
  }

  void single_particle_generator::reset ()
  {
    if (! __initialized) return;
    __at_reset ();
    __initialized = false;
    return;
  }

  void single_particle_generator::initialize (const datatools::utils::properties & config_)
  {

    if (config_.has_flag ("debug"))
      {
	set_debug (true);
      }

    if (config_.has_key ("seed"))
      {
	long seed = config_.fetch_integer ("seed");
	if (seed < 0)
	  {
	    throw runtime_error ("single_particle_generator::initialize: Invalid seed value (>=0) !");
	  }
	__seed = seed;
      }
    else
      {
	throw runtime_error ("single_particle_generator::initialize: Missing 'seed' property !");
      }

    if (config_.has_key ("particle_name"))
      {
	string particle_name = config_.fetch_string ("particle_name");
	if (! single_particle_generator::particle_name_is_valid (particle_name))
	  {
	    ostringstream message;
	    message << "single_particle_generator::initialize: "
		    << "Invalid particle name '"
		    << particle_name << "' !";
	    throw runtime_error (message.str ());
	  }
	  set_particle_name (particle_name);
      }
    else
      {
	throw runtime_error ("single_particle_generator::initialize: Missing 'seed' property !");
      }

    if (config_.has_key ("randomized_direction"))
      {
	bool rd = config_.fetch_boolean ("randomized_direction");
	set_randomized_direction (rd);
      }

    if (config_.has_key ("mode"))
      {
	string mode_str = config_.fetch_string ("mode");
	if (mode_str == "gaussian_energy")
	  {
	    set_mode (MODE_GAUSSIAN_ENERGY);
	  }
	else if (mode_str == "energy_range")
	  {
	    set_mode (MODE_ENERGY_RANGE);
	  }
	else if (mode_str == "monokinetic")
	  {
	    set_mode (MODE_MONOKINETIC);
	  }
	else if (mode_str == "spectrum")
	  {
	    set_mode (MODE_SPECTRUM);
	    __spectrum_mode = SPECTRUM_MODE_TABFUNC;
	    if (config_.has_key ("spectrum.mode"))
	      {
		string spectrum_mode_str = config_.fetch_string ("spectrum.mode");
		if (spectrum_mode_str == "tabulated_function")
		  {
		    __spectrum_mode = SPECTRUM_MODE_TABFUNC;
		  }
		else if (spectrum_mode_str == "histogram_pdf")
		  {
		    __spectrum_mode = SPECTRUM_MODE_HISTPDF;
		  }
	      }
	  }
	else
	  {
	    ostringstream message;
	    message << "single_particle_generator::initialize: "
		    << "Invalid mode '" << mode_str << "' !";
	    throw runtime_error (message.str ());
	  }
      }
    else
      {
	throw runtime_error ("single_particle_generator::initialize: Missing 'mode' property !");
      }

    double energy_unit = CLHEP::keV;

    if (config_.has_key ("energy_unit"))
      {
	string unit_str = config_.fetch_string ("energy_unit");
	energy_unit = datatools::utils::units::get_energy_unit_from (unit_str);
      }

    if (__mode == MODE_MONOKINETIC)
      {
	double energy = 0.0;
	if (! config_.has_key ("energy"))
	  {
	    throw runtime_error ("single_particle_generator::initialize: Missing 'energy' property !");
	  }
	energy = config_.fetch_real ("energy");
	energy *= energy_unit;
	set_mean_energy (energy, 0.0);
      }

    if (__mode == MODE_GAUSSIAN_ENERGY)
      {
	double mean_energy = 0.0;
	double sigma_energy = 0.0;
	if (! config_.has_key ("mean_energy"))
	  {
	    throw runtime_error ("single_particle_generator::initialize: Missing 'mean_energy' property !");
	  }
	mean_energy = config_.fetch_real ("mean_energy");
	mean_energy *= energy_unit;

	if (config_.has_key ("sigma_energy"))
	  {
	    sigma_energy = config_.fetch_real ("sigma_energy");
	    sigma_energy *= energy_unit;
	  }

	set_mean_energy (mean_energy, sigma_energy);
      }

    if (__mode == MODE_ENERGY_RANGE)
      {
	double min_energy = 0.0;
	double max_energy = 0.0;
	if (! config_.has_key ("max_energy"))
	  {
	    throw runtime_error ("single_particle_generator::initialize: Missing 'max_energy' property !");
	  }
	max_energy = config_.fetch_real ("max_energy");
	max_energy *= energy_unit;

	if (! config_.has_key ("min_energy"))
	  {
	    throw runtime_error ("single_particle_generator::initialize: Missing 'min_energy' property !");
	  }
	min_energy = config_.fetch_real ("min_energy");
	min_energy *= energy_unit;

	set_energy_range (min_energy, max_energy);
      }

    if (__mode == MODE_SPECTRUM)
      {
	if (config_.has_key ("spectrum.data_file"))
	  {
	    string spectrum_filename = config_.fetch_string ("spectrum.data_file");
	    set_energy_spectrum_filename (spectrum_filename);
	  }
        double min_energy = __min_energy;
        double max_energy = __max_energy;

        if (config_.has_key ("spectrum.min_energy"))
          {
            min_energy = config_.fetch_real ("spectrum.min_energy");
          }
        min_energy *= energy_unit;

        if (config_.has_key ("spectrum.min_energy"))
          {
            max_energy = config_.fetch_real ("spectrum.max_energy");
          }
        max_energy *= energy_unit;

        set_energy_range (min_energy, max_energy);
      }

    __at_init ();

    __initialized  = true;
    return;
  }

  bool single_particle_generator::has_next ()
  {
    return true;
  }

  void single_particle_generator::_load_next (primary_event & event_,
					      bool compute_classification_)
  {
    if (__debug)
      {
	clog << "debug: " << "single_particle_generator::_load_next: "
	     << "Entering..."
	     << endl;
      }
    if (! __initialized)
      {
	throw runtime_error ("single_particle_generator::_load_next: Generator is notlocked/initialized !");
      }
    event_.reset ();

    double kinetic_energy = -1.0;
    double mass = __particle_mass;

    if (__mode == MODE_MONOKINETIC)
      {
	kinetic_energy = __mean_energy;
      }

    if (__mode == MODE_GAUSSIAN_ENERGY)
      {
	while (kinetic_energy <= 0.0)
	  {
	    kinetic_energy = __random.gaussian (__mean_energy, __sigma_energy);
	  }
      }

    if (__mode == MODE_ENERGY_RANGE)
      {
	kinetic_energy = __random.flat (__min_energy, __max_energy);
      }

    if (__mode == MODE_SPECTRUM)
      {
	if (__spectrum_mode == SPECTRUM_MODE_TABFUNC)
	  {
	    kinetic_energy = __VNM.shoot (__random);
	  }
	else if (__spectrum_mode == SPECTRUM_MODE_HISTPDF)
	  {
	    kinetic_energy = __energy_histo_pdf.sample (__random);
	  }
      }

    double momentum = sqrt (kinetic_energy * (kinetic_energy + 2 * mass));
    double px, py, pz;
    px = 0.0;
    py = 0.0;
    pz = momentum;

    event_.time = 0.0 * CLHEP::second;;
    primary_particle pp;
    pp.set_type (__particle_type);
    pp.set_time (0.0 * CLHEP::second);
    geomtools::vector_3d p (px, py, pz);
    pp.set_momentum (p);
    event_.add_particle (pp);

    if (__randomized_direction)
      {
	double phi = __random.flat (0.0, 360.0 * CLHEP::degree);
	double cos_theta = __random.flat (-1.0, +1.0);
	double theta = acos (cos_theta);
	double psi = __random.flat (0.0, 360.0 * CLHEP::degree);
	event_.rotate (phi, theta, psi);
      }

    if (compute_classification_)
      {
	event_.compute_classification ();
      }

    if (__debug)
      {
	clog << "debug: " << "single_particle_generator::_load_next: "
	     << "Exiting."
	     << endl;
      }
    return;
  }

  void single_particle_generator::_init_energy_histo_pdf ()
  {
    using namespace std;
    string filename = __energy_spectrum_filename;
    datatools::utils::fetch_path_with_env (filename);
    ifstream ifile (filename.c_str());
    if (! ifile)
      {
	ostringstream message;
	message << "single_particle_generator::_init_energy_histo_pdf: "
		<< "Cannot open data file '"
		<< __energy_spectrum_filename << "' !";
	throw logic_error (message.str ());
      }

    double energy_unit = CLHEP::MeV;
    size_t nbins = 0;
    double min = 0.0, max = 0.0;
    // load histo
    while (! ifile.eof ())
      {
	string line;
	getline (ifile, line);
	if (line.empty ())
	  {
	    continue;
	  }
        {
          istringstream lineiss (line);
          string word;
          lineiss >> word;
          if ((word.length () > 0) && (word[0] == '#'))
            {
              if (word == "#@limits")
                {
                  lineiss >> nbins >> min >> max;
                  // initalize histo
                  __energy_histo.init (nbins, min*energy_unit, max*energy_unit);
              }
              else if (word == "#@energy_unit")
                {
                  string energy_unit_str;
                  lineiss >> energy_unit_str;
                  energy_unit = datatools::utils::units::get_energy_unit_from (energy_unit_str);
                }
              continue;
            }
        }

        if (!nbins)
          {
            ostringstream message;
            message << "single_particle_generator::_init_energy_histo_pdf: "
                    << "Limits to histogram are not given in data file '"
                    << __energy_spectrum_filename << "' !";
            throw logic_error (message.str ());
          }

        istringstream lineiss (line);
        double xmin, xmax;
        double weight;
        lineiss >> xmin >> xmax >> weight;
        if (! lineiss)
          {
            ostringstream message;
            message << "single_particle_generator::_init_energy_histo_pdf: "
                    << "Format error in data file '"
                    << __energy_spectrum_filename << "' !";
            throw logic_error (message.str ());
          }

        xmin *= energy_unit;
        xmax *= energy_unit;

        if (xmin < 0.0 || xmax < 0.0)
          {
            ostringstream message;
            message << "single_particle_generator::_init_energy_histo_pdf: "
                    << "Invalid energy value < 0)!";
            throw out_of_range (message.str ());
          }
        if (weight < 0.0)
          {
            ostringstream message;
            message << "single_particle_generator::_init_energy_histo_pdf: "
                    << "Invalid spectrum value (" << weight << " < 0)!";
            throw out_of_range (message.str ());
          }

        // filling histogram
        const double mean_energy = (xmin + xmax)/2.0;
        if (mean_energy < __min_energy || mean_energy > __max_energy)
          continue;

        __energy_histo.fill (mean_energy, weight);
      }

    // Calculate histogram PDF
    __energy_histo_pdf.init (__energy_histo);
    return;
  }

  void single_particle_generator::_init_energy_spectrum ()
  {
    if (__energy_spectrum.is_table_locked ())
      {
	ostringstream message;
	message << "single_particle_generator::_init_energy_spectrum: "
		<< "Tabulated energy spectrum is already locked !";
	throw logic_error (message.str ());
      }

    using namespace std;
    string filename = __energy_spectrum_filename;
    datatools::utils::fetch_path_with_env (filename);
    ifstream ifile;
    ifile.open (filename.c_str ());
    if (! ifile)
      {
	ostringstream message;
	message << "single_particle_generator::_init_energy_spectrum: "
		<< "Cannot open interpolation data file '"
		<< __energy_spectrum_filename << "' !";
	throw logic_error (message.str ());
      }
    string interpolator_name;
    double energy_unit = CLHEP::MeV;
    double ymax = -1.0;
    while (! ifile.eof ())
      {
	string line;
	getline (ifile, line);
	if (line.empty ())
	  {
	    continue;
	  }
	{
	  istringstream lineiss (line);
	  string word;
	  lineiss >> word;
	  if ((word.length () > 0) && (word[0] == '#'))
	    {
	      if (word == "#@interpolation_name")
		{
		  lineiss >> interpolator_name;
		}
	      else if (word == "#@energy_unit")
		{
		  string energy_unit_str;
		  lineiss >> energy_unit_str;
		  energy_unit = datatools::utils::units::get_energy_unit_from (energy_unit_str);
		}
              continue;
	    }
	}
	istringstream lineiss (line);
	double x, y;
	lineiss >> x >> y;
	if (! lineiss)
	  {
	    ostringstream message;
	    message << "single_particle_generator::_init_energy_spectrum: "
		    << "Format error in interpolation data file '"
		    << __energy_spectrum_filename << "' !";
	    throw logic_error (message.str ());
	  }
	x *= energy_unit;
	if (x < 0.0)
	  {
	    ostringstream message;
	    message << "single_particle_generator::_init_energy_spectrum: "
		    << "Invalid energy value (" << x << " < 0)!";
	    throw out_of_range (message.str ());
	  }
	if (y < 0.0)
	  {
	    ostringstream message;
	    message << "single_particle_generator::_init_energy_spectrum: "
		    << "Invalid spectrum value (" << y << " < 0)!";
	    throw out_of_range (message.str ());
	  }

        if (x < __min_energy || x > __max_energy)
          continue;

	__energy_spectrum.add_point (x, y, false);
	if (y > ymax) ymax = y;
      }
    if (interpolator_name.empty ())
      {
	interpolator_name = __spectrum_interpolation_name;
      }
    __energy_spectrum.lock_table (interpolator_name);
    __VNM.init (__energy_spectrum.x_min (),
		__energy_spectrum.x_max (),
		__energy_spectrum,
		ymax * 1.01);
    return;
  }

  void single_particle_generator::__at_init ()
  {
    __particle_type = primary_particle::get_particle_type_from_label (__particle_name);
    __particle_mass = get_particle_mass_from_label (__particle_name);
    if (! datatools::utils::is_valid (__particle_mass))
      {
	throw runtime_error ("single_particle_generator::__at_init: Particle mass is not defined !");
      }

    if (__mode == MODE_SPECTRUM)
      {
	if (__spectrum_mode == SPECTRUM_MODE_TABFUNC)
	  {
	    _init_energy_spectrum ();
	  }
	else if (__spectrum_mode == SPECTRUM_MODE_HISTPDF)
	  {
	    _init_energy_histo_pdf ();
	  }

       }

    __random.init ("taus2", __seed);
    return;
  }

} // end of namespace genbb

// end of single_particle_generator.cc
