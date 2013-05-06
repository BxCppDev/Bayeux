// -*- mode: c++; -*-
// wgenbb.cc
/*
 * Copyright 2007-2012 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public 3.0 License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public 3.0 License for more details.
 *
 * You should have received a copy of the GNU General Publi * License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <genbb_help/wgenbb.h>

#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

//#include <boost/filesystem.hpp>

#include <datatools/utils.h>
#include <datatools/units.h>

#include <genbb_help/genbb_utils.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/decay0_fortran/rng.h>

extern GENEVENT_DEF genevent;
extern ENRANGE_DEF  enrange;
extern GENBBPAR_DEF genbbpar;

namespace genbb {

  using namespace std;

  GENBB_PG_REGISTRATION_IMPLEMENT(wgenbb,"genbb::wgenbb");

  size_t wgenbb::_g_counter_ = 0;

  bool wgenbb::is_initialized () const
  {
    return _initialized_;
  }

  bool wgenbb::is_debug () const
  {
    return _debug_;
  }

  void wgenbb::set_debug (bool d_)
  {
    _debug_ = d_;
    return;
  }

  // ctor:
  wgenbb::wgenbb () : i_genbb ()
  {
    if (wgenbb::_g_counter_ > 0)
      {
        throw logic_error ("genbb::wgenbb::wgenbb: Only one instance of wgenbb is allowed !");
      }
    wgenbb::_g_counter_++;
    _debug_ = false;
    _initialized_ = false;
    _event_count_ = 0;
    _decay_type_ = DECAY_TYPE_UNDEFINED;
    _decay_isotope_ = "";
    for (int i = 0; i < ISOTOPE_NAME_MAXSIZE; i++)
      {
        _c_decay_isotope_[i] = 0;
      }
    _decay_dbd_level_ = 0;
    _decay_dbd_mode_ = DBD_MODE_INVALID;
    _use_local_prng_ = false;
    _seed_ = 0;
    datatools::invalidate (_energy_min_);
    datatools::invalidate (_energy_max_);
    return;
  }

  // dtor:
  wgenbb::~wgenbb ()
  {
    if (_initialized_)
      {
        reset ();
      }
    return;
  }

  bool wgenbb::can_external_random () const
  {
    return true;
  }

  const mygsl::rng & wgenbb::get_random () const
  {
    if (has_external_random ())
      {
        return get_external_random ();
      }
    if (! _use_local_prng_)
      {
        return genbb::rng::get_genbb_external_prng ();
      }
    return _random_;
  }

  mygsl::rng & wgenbb::grab_random ()
  {
    if (has_external_random ())
      {
        return grab_external_random ();
      }
    if (! _use_local_prng_)
      {
        return genbb::rng::grab_genbb_external_prng ();
      }
    return _random_;
  }

  void wgenbb::_clean_ ()
  {
    return;
  }

  void wgenbb::reset ()
  {
    if (! _initialized_)
      {
        return;
      }
    _initialized_ = false;
    _clean_ ();
    _event_count_ = 0;

    _decay_type_ = DECAY_TYPE_UNDEFINED;
    for (int i = 0; i < ISOTOPE_NAME_MAXSIZE; i++)
      {
        _c_decay_isotope_[i] = 0;
      }
    _decay_isotope_ = "";
    _decay_dbd_level_ = 0;
    _decay_dbd_mode_ = DBD_MODE_INVALID;

    datatools::invalidate (_energy_min_);
    datatools::invalidate (_energy_max_);

    _seed_ = 0;
    if (! has_external_random ())
      {
        if (use_local_prng ())
          {
            clog << "NOTICE: " << "genbb::wgenbb::reset: "
                 << "Reseting the local PRNG..." << endl;
            if (_random_.is_initialized ())
              {
                _random_.reset ();
              }
          }
      }
    _use_local_prng_ = false;

    return;
  }

  bool wgenbb::use_local_prng () const
  {
    return _use_local_prng_;
  }

  void wgenbb::set_use_local_prng (bool u_)
  {
    if (_initialized_)
      {
        throw logic_error ("genbb::wgenbb::set_use_local_prng: Already initialized !");
      }
    _use_local_prng_ = u_;
    return;
  }

  void wgenbb::initialize (const datatools::properties & config_,
                           datatools::service_manager & service_manager_,
                           detail::pg_dict_type & dictionary_)
  {
    if (_initialized_)
      {
        throw logic_error ("genbb::wgenbb::initialize: Already initialized !");
      }

    string decay_isotope;
    double energy_unit = CLHEP::MeV;
    double emin = 0.0;
    double emax = -1.0;
    datatools::invalidate (emin);
    datatools::invalidate (emax);

    if (config_.has_flag ("debug"))
      {
        _debug_ = true;
      }

    if (! has_external_random ())
      {
        if (config_.has_key ("local_prng"))
          {
            set_use_local_prng (true);
          }

        if (config_.has_key ("seed"))
          {
            long seed = config_.fetch_integer ("seed");
            if (seed < 0)
              {
                throw logic_error ("genbb::wgenbb::initialize: Invalid seed value (>=0) !");
              }
            _seed_ = seed;
          }
      }

    if (! config_.has_key ("decay_type"))
      {
        ostringstream message;
        message << "genbb::wgenbb::initialize: Missing 'decay_type' property !";
        throw logic_error (message.str());
      }
    else
      {
        string tmp = config_.fetch_string ("decay_type");
        if ((tmp != "DBD") && (tmp != "background"))
          {
            ostringstream message;
            message << "genbb::wgenbb::initialize: Invalid decay type '"
                    << tmp << "' !";
            throw logic_error (message.str());
          }
        if (tmp == "background")
          {
            _decay_type_ = DECAY_TYPE_BACKGROUND;
          }

        if (tmp == "DBD")
          {
            _decay_type_ = DECAY_TYPE_DBD;

            if (! config_.has_key ("decay_dbd_level"))
              {
                ostringstream message;
                message << "genbb::wgenbb::initialize: Missing DBD decay level !";
                throw logic_error (message.str());
              }
            _decay_dbd_level_ = config_.fetch_integer ("decay_dbd_level");

            if (! config_.has_key ("decay_dbd_mode"))
              {
                ostringstream message;
                message << "genbb::wgenbb::initialize: Missing DBD decay mode !";
                throw logic_error (message.str());
              }
            _decay_dbd_mode_ = config_.fetch_integer ("decay_dbd_mode");
          }

      }

    if (! config_.has_key ("decay_isotope"))
      {
        ostringstream message;
        message << "genbb::wgenbb::initialize: Missing 'decay_isotope' property !";
        throw logic_error (message.str());
      }
    else
      {
        decay_isotope = config_.fetch_string ("decay_isotope");
      }

    _set_decay_isotope_ (decay_isotope);

    if (_decay_type_ == DECAY_TYPE_DBD)
      {
        const std::vector<int> & dbdmwer
          = utils::get_dbd_modes_with_energy_range ();
        if (std::find (dbdmwer.begin (), dbdmwer.end (),_decay_dbd_mode_) != dbdmwer.end ())
          {
            if (config_.has_key ("energy_unit"))
              {
                string unit_str = config_.fetch_string ("energy_unit");
                energy_unit = datatools::units::get_energy_unit_from (unit_str);
              }

            if (config_.has_key ("energy_max"))
              {
                emax = config_.fetch_real ("energy_max");
                if (emax < 0.0)
                  {
                    throw logic_error ("genbb::wgenbb::initialize: Invalid maximum value !");
                  }
                emax *= energy_unit;
                _energy_max_ = emax;
              }

            if (config_.has_key ("energy_min"))
              {
                emin = config_.fetch_real ("energy_min");
                if (emin < 0.0)
                  {
                    throw logic_error ("genbb::wgenbb::initialize: Invalid minimum value !");
                  }
                emin *= energy_unit;
                _energy_min_ = emin;
              }

          }
      }

    if (datatools::is_valid (_energy_min_))
      {
        if (! datatools::is_valid (_energy_max_))
          {
            _energy_max_ = utils::DEFAULT_ENERGY_RANGE_MAX;
          }
      }
    if (datatools::is_valid (_energy_max_))
      {
        if (! datatools::is_valid (_energy_min_))
          {
            _energy_min_ = utils::DEFAULT_ENERGY_RANGE_MIN;
          }
      }
    if (_energy_min_ >= _energy_max_)
      {
        ostringstream message;
        message << "genbb::wgenbb::initialize: Invalid energy range !";
        throw logic_error (message.str());
      }

    if (has_external_random ())
      {
        clog << "NOTICE: " << "genbb::wgenbb::initialize: "
             << "Installing the external PRNG as the global PRNG for the fortran interface..." << endl;
        genbb::rng::set_genbb_external_prng (grab_external_random ());
      }
    else if (use_local_prng ())
      {
        clog << "NOTICE: " << "genbb::wgenbb::initialize: "
             << "Initializing the local PRNG..." << endl;
        _random_.init ("taus2", _seed_);
        clog << "NOTICE: " << "genbb::wgenbb::initialize: "
             << "Installing the local PRNG as the global PRNG for the fortran interface..." << endl;
        genbb::rng::set_genbb_external_prng (_random_);
      }

    _init_ ();

    _initialized_ = true;
    return;
  }

  bool wgenbb::has_next ()
  {
    return _initialized_;
  }

  void wgenbb::_set_decay_isotope_ (const string & di_)
  {
    _decay_isotope_ = di_;
    size_t sz = di_.length ();
    if (sz >= 15)
      {
        throw logic_error ("genbb::wgenbb::_set_decay_isotope_: Decay isotope string too long !");
      }
    for (int i = 0; i < sz; i++)
      {
        _c_decay_isotope_[i] = di_[i];
      }
    return;
  }

  void wgenbb::_load_next (primary_event & event_,
                           bool compute_classification_)
  {
    if (_debug_)
      {
        clog << "debug: " << "genbb::wgenbb::_load_next: "
             << "Entering..."
             << endl;
      }
    if (! _initialized_)
      {
        throw logic_error ("genbb::wgenbb::_load_next: Not initialized !");
      }
    event_.reset ();

    int error;
    int start = 1;

    // reset Fortran common block:
    genevent.reset ();

    genbbsub (&_decay_type_,
              _c_decay_isotope_,
              &_decay_dbd_level_,
              &_decay_dbd_mode_,
              &start,
              &error);
    if (error != 0)
      {
        throw logic_error ("genbb::wgenbb::_load_next: genbbsub failed !");
      }

    int    evnum;
    double time;
    int    npart;

    evnum = 0;
    event_.set_time (genevent.tevst * CLHEP::second);
    double part_time = 0.0;
    npart = genevent.npfull;
    /*
      clog << "DEVEL: " << "genbb::wgenbb::_load_next: "
      << "npart=" << npart << endl;
    */
    for (int i = 0; i < npart; i++)
      {
        primary_particle pp;
        int part_type;
        double x, y ,z, time_shift;
        part_type = genevent.npgeant[i];
        /*
          clog << "DEVEL: " << "genbb::wgenbb::_load_next: "
          << "part_type=" << part_type << endl;
        */
        x = genevent.pmoment[i][0];
        y = genevent.pmoment[i][1];
        z = genevent.pmoment[i][2];
        /*
          clog << "DEVEL: " << "genbb::wgenbb::_load_next: "
          << " px=" << x
          << " py=" << y
          << " pz=" << z
          << endl;
        */
        time_shift = genevent.ptime[i];
        /*
          clog << "DEVEL: " << "genbb::wgenbb::_load_next: "
          << "time_shift=" << time_shift << endl;
        */
        part_time += time_shift;
        pp.set_type (part_type);
        pp.set_time (part_time * CLHEP::second); // GENBB unit is s
        geomtools::vector_3d p (x, y, z);
        p *= CLHEP::MeV; // GENBB unit is MeV/c
        pp.set_momentum (p);
        event_.add_particle (pp);
      }

    if (compute_classification_)
      {
        event_.compute_classification ();
      }

    event_.set_genbb_weight (1.0);
    if (_debug_)
      {
        clog << "debug: " << "genbb::wgenbb::_load_next: "
             << "GENBB weight = " << get_to_all_events ()
             << endl;
      }
    if (get_to_all_events () > 1.0)
      {
        event_.set_genbb_weight (1.0 / get_to_all_events ());
      }

    //event_.dump (clog , "GENBB event: ", "DEVEL: ");
    _event_count_++;
    if (_debug_)
      {
        clog << "debug: " << "genbb::wgenbb::_load_next: "
             << "Exiting."
             << endl;
      }
    return;
  }

  size_t wgenbb::get_event_count () const
  {
    return _event_count_;
  }

  double wgenbb::get_to_all_events () const
  {
    return 1.0 * enrange.toallevents;
  }

  void wgenbb::_init_ ()
  {
    if (_debug_)
      {
        clog << "DEBUG: " << "genbb::wgenbb::_init_: "
             << "Entering..."
             << endl;
      }

    if (_decay_type_ == DECAY_TYPE_UNDEFINED)
      {
        throw logic_error ("genbb::wgenbb::_init_: Decay type is not defined !");
      }
    if (_decay_type_ == DECAY_TYPE_DBD && _decay_dbd_mode_ == DBD_MODE_INVALID)
      {
        throw logic_error ("genbb::wgenbb::_init_: Invalid DBD mode !");
      }

    enrange.reset ();
    const std::vector<int> & dbdmwer
      = utils::get_dbd_modes_with_energy_range ();
    if (_debug_) clog << "DEBUG: genbb::wgenbb::_init_: Decay DBD mode : "
                      << _decay_dbd_mode_ << endl;
    if (std::find (dbdmwer.begin (), dbdmwer.end (),_decay_dbd_mode_) != dbdmwer.end ())
      {
        if (datatools::is_valid (_energy_min_))
          {
            clog << "NOTICE: genbb::wgenbb::_init_: Setting energy min to "
                 << _energy_min_ / CLHEP::MeV << " MeV" << endl;
            enrange.ebb1 = (float) (_energy_min_ / CLHEP::MeV);
          }
        if (datatools::is_valid (_energy_max_))
          {
            clog << "NOTICE: genbb::wgenbb::_init_: Setting energy max to "
                 << _energy_max_ / CLHEP::MeV << " MeV" << endl;
            enrange.ebb2 = (float) (_energy_max_ / CLHEP::MeV);
          }
        if (enrange.ebb1 >= enrange.ebb2)
          {
            ostringstream message;
            message << "genbb::wgenbb::_init_: "
                    << "Invalid energy range !";
            throw logic_error (message.str ());
          }
      }
    else
      {
        if (_debug_)  clog << "DEBUG: genbb::wgenbb::_init_: Not a DBD energy range mode."
                           << endl;
      }

    if (! _use_local_prng_)
      {
        int genbb_seed = _seed_;
        set_genbb_random_seed (&genbb_seed);
      }
    int error = 0;
    int start = -1; // special initialization value without event generation
    genbbsub (&_decay_type_,
              _c_decay_isotope_,
              &_decay_dbd_level_,
              &_decay_dbd_mode_,
              &start,
              &error);
    if (error != 0)
      {
        throw logic_error ("genbb::wgenbb::_init_: genbbsub failed !");
      }

    if (_debug_)
      {
        clog << "DEBUG: " << "genbb::wgenbb::_init_: "
             << "Exiting."
             << endl;
      }
    return;
  }

  void wgenbb::dump (ostream & out_) const
  {
    const string tag = "|-- ";
    const string last_tag = "`-- ";
    out_ << "C++ GENBB wrapper: " << endl;
    out_ << tag << "debug            : " << _debug_ << endl;
    out_ << tag << "decay_type       : " << _decay_type_ << endl;
    out_ << tag << "decay_isotope    : '" << _decay_isotope_ << "'" << endl;
    out_ << tag << "decay_dbd_level  : " << _decay_dbd_level_ << endl;
    out_ << tag << "decay_dbd_mode   : " << _decay_dbd_mode_ << endl;
    out_ << tag << "event_count      : " << _event_count_ << endl;
    if (datatools::is_valid (_energy_min_))
      {
        out_ << tag << "energy_min       : " << _energy_min_ / CLHEP::keV << " keV" << endl;
      }
    if (datatools::is_valid (_energy_max_))
      {
        out_ << tag << "energy_max       : " << _energy_max_ / CLHEP::keV << " keV" << endl;
      }
    out_ << tag << "use local PRNG   : " << _use_local_prng_ << endl;
    out_ << last_tag << "seed             : " << _seed_ << endl;
    return;
  }

} // end of namespace genbb

// end of wgenbb.cc
