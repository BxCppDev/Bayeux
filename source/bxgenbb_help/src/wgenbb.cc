// -*- mode: c++; -*-
// wgenbb.cc
/*
 * Copyright 2007-2013 F. Mauger
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

#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/exception.h>

#include <genbb_help/genbb_utils.h>
#include <genbb_help/primary_event.h>

#include <genbb_help/decay0_fortran/rng.h>
#include <genbb_help/decay0_fortran/genbb_commons.h>

typedef GENEVENT_t GENEVENT_DEF;
typedef ENRANGE_t  ENRANGE_DEF;

// 2013-05-07 FM: does not work if included from the wgenbb.cc file !
// but is ok if included from the wgenbb.h. I don't understand !
//#include <genbb_help/decay0_fortran/__genbb_help_FC2.h>
// So I prefer to provide the following names with
// an explicit trailing underscore because the FC2 macros fail :
extern "C"
{
  void set_genbb_random_seed_ (int *);
  // was: void set_genbb_random_seed (int *);

  void genbbsub_ (int *,
                 const char *,
                 int *,
                 int *,
                 int *,
                 int *);
 // was:  void genbbsub (int *, ...)
}
extern GENEVENT_DEF genevent_;
extern ENRANGE_DEF  enrange_;
// was: extern GENEVENT_DEF genevent;
// was: extern ENRANGE_DEF  enrange;


namespace genbb {

  using namespace std;

  GENBB_PG_REGISTRATION_IMPLEMENT(wgenbb,"genbb::wgenbb");

  int wgenbb::_g_counter_ = 0;

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
    DT_THROW_IF (wgenbb::_g_counter_ > 0, logic_error, "Only one instance of wgenbb is allowed !");
    wgenbb::_g_counter_++;
    _debug_ = false;
    _initialized_ = false;
    _event_count_ = 0;
    _decay_type_ = DECAY_TYPE_UNDEFINED;
    _decay_isotope_ = "";
    for (int i = 0; i < ISOTOPE_NAME_MAXSIZE; i++) {
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
    if (_initialized_) {
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
    if (has_external_random ()) {
      return get_external_random ();
    }
    if (! _use_local_prng_) {
      return genbb::rng::get_genbb_external_prng ();
    }
    return _random_;
  }

  mygsl::rng & wgenbb::grab_random ()
  {
    if (has_external_random ()) {
      return grab_external_random ();
    }
    if (! _use_local_prng_) {
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
    if (! _initialized_) {
      return;
    }
    _initialized_ = false;
    _clean_ ();
    _event_count_ = 0;

    _decay_type_ = DECAY_TYPE_UNDEFINED;
    for (int i = 0; i < ISOTOPE_NAME_MAXSIZE; i++) {
      _c_decay_isotope_[i] = 0;
    }
    _decay_isotope_ = "";
    _decay_dbd_level_ = 0;
    _decay_dbd_mode_ = DBD_MODE_INVALID;

    datatools::invalidate (_energy_min_);
    datatools::invalidate (_energy_max_);

    _seed_ = 0;
    if (! has_external_random ()) {
      if (use_local_prng ()) {
        clog << "NOTICE: " << "genbb::wgenbb::reset: "
             << "Reseting the local PRNG..." << endl;
        if (_random_.is_initialized ()) {
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
    DT_THROW_IF (_initialized_, logic_error, "Already initialized !");
    _use_local_prng_ = u_;
    return;
  }

  void wgenbb::initialize (const datatools::properties & config_,
                           datatools::service_manager & service_manager_,
                           detail::pg_dict_type & dictionary_)
  {
    DT_THROW_IF (_initialized_, logic_error, "Already initialized !");
    _initialize_base(config_);

    string decay_isotope;
    double energy_unit = CLHEP::MeV;
    double emin = 0.0;
    double emax = -1.0;
    datatools::invalidate (emin);
    datatools::invalidate (emax);

    if (config_.has_flag ("debug")) {
      _debug_ = true;
    }

    if (! has_external_random ()) {
      if (config_.has_key ("local_prng")) {
        set_use_local_prng (true);
      }

      if (config_.has_key ("seed")) {
        long seed = config_.fetch_integer ("seed");
        DT_THROW_IF (seed < 0, logic_error, "Invalid seed value (>=0) !");
        _seed_ = seed;
      }
    }

    DT_THROW_IF (! config_.has_key ("decay_type"), logic_error,
                 "Missing 'decay_type' property !");
    string tmp = config_.fetch_string ("decay_type");
    if ((tmp != "DBD") && (tmp != "background")) {
      ostringstream message;
      message << "genbb::wgenbb::initialize: Invalid decay type '"
              << tmp << "' !";
      throw logic_error (message.str());
    }
    if (tmp == "background") {
      _decay_type_ = DECAY_TYPE_BACKGROUND;
    }

    if (tmp == "DBD") {
      _decay_type_ = DECAY_TYPE_DBD;

      if (! config_.has_key ("decay_dbd_level")) {
        ostringstream message;
        message << "genbb::wgenbb::initialize: Missing DBD decay level !";
        throw logic_error (message.str());
      }
      _decay_dbd_level_ = config_.fetch_integer ("decay_dbd_level");

      if (! config_.has_key ("decay_dbd_mode")) {
        ostringstream message;
        message << "genbb::wgenbb::initialize: Missing DBD decay mode !";
        throw logic_error (message.str());
      }
      _decay_dbd_mode_ = config_.fetch_integer ("decay_dbd_mode");
    }

    DT_THROW_IF (! config_.has_key ("decay_isotope"),logic_error,
                 "Missing 'decay_isotope' property !");
    decay_isotope = config_.fetch_string ("decay_isotope");

    _set_decay_isotope_ (decay_isotope);

    if (_decay_type_ == DECAY_TYPE_DBD) {
      const std::vector<int> & dbdmwer
        = utils::get_dbd_modes_with_energy_range ();
      if (std::find (dbdmwer.begin (), dbdmwer.end (),_decay_dbd_mode_) != dbdmwer.end ()) {
        if (config_.has_key ("energy_unit")) {
          string unit_str = config_.fetch_string ("energy_unit");
          energy_unit = datatools::units::get_energy_unit_from (unit_str);
        }

        if (config_.has_key ("energy_max")) {
          emax = config_.fetch_real ("energy_max");
          DT_THROW_IF (emax < 0.0, logic_error, "Invalid maximum value !");
          emax *= energy_unit;
          _energy_max_ = emax;
        }

        if (config_.has_key ("energy_min")) {
          emin = config_.fetch_real ("energy_min");
          DT_THROW_IF (emin < 0.0, logic_error, "Invalid minimum value !");
          emin *= energy_unit;
          _energy_min_ = emin;
        }

      }
    }

    if (datatools::is_valid (_energy_min_)) {
      if (! datatools::is_valid (_energy_max_)) {
        _energy_max_ = utils::DEFAULT_ENERGY_RANGE_MAX;
      }
    }
    if (datatools::is_valid (_energy_max_)) {
      if (! datatools::is_valid (_energy_min_)) {
        _energy_min_ = utils::DEFAULT_ENERGY_RANGE_MIN;
      }
    }
    DT_THROW_IF (_energy_min_ >= _energy_max_, logic_error, "Invalid energy range !");

    if (has_external_random ()) {
      DT_LOG_NOTICE(get_logging_priority(),
                    "Installing the external PRNG as the global PRNG for the fortran interface...");
      genbb::rng::set_genbb_external_prng (grab_external_random ());
    } else if (use_local_prng ()) {
      DT_LOG_NOTICE(get_logging_priority(),"Initializing the local PRNG...");
      _random_.init ("taus2", _seed_);
      DT_LOG_NOTICE(get_logging_priority(),"Installing the local PRNG as the global PRNG for the fortran interface...");
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
    DT_THROW_IF (sz >= 15, logic_error, "Decay isotope string too long !");
    for (int i = 0; i < sz; i++) {
      _c_decay_isotope_[i] = di_[i];
    }
    return;
  }

  void wgenbb::_load_next (primary_event & event_,
                           bool compute_classification_)
  {
    DT_LOG_TRACE (get_logging_priority(),"Entering...");
    DT_THROW_IF (! _initialized_, logic_error, "Not initialized !");
    event_.reset ();

    int error;
    int start = 1;

    // reset Fortran common block:
    genevent_.reset ();

    genbbsub_ (&_decay_type_,
              _c_decay_isotope_,
              &_decay_dbd_level_,
              &_decay_dbd_mode_,
              &start,
              &error);
    DT_THROW_IF (error != 0, logic_error, "genbbsub failed !");

    int    evnum;
    double time;
    int    npart;

    evnum = 0;
    event_.set_time (genevent_.tevst * CLHEP::second);
    double part_time = 0.0; // GENBB time unit : second
    npart = genevent_.npfull;
    /*
      clog << "DEVEL: " << "genbb::wgenbb::_load_next: "
      << "npart=" << npart << endl;
    */
    for (int i = 0; i < npart; i++) {
      primary_particle pp;
      int part_type;
      double x, y ,z, time_shift;
      part_type = genevent_.npgeant[i];
      /*
        clog << "DEVEL: " << "genbb::wgenbb::_load_next: "
        << "part_type=" << part_type << endl;
      */
      x = genevent_.pmoment[i][0];
      y = genevent_.pmoment[i][1];
      z = genevent_.pmoment[i][2];
      /*
        clog << "DEVEL: " << "genbb::wgenbb::_load_next: "
        << " px=" << x
        << " py=" << y
        << " pz=" << z
        << endl;
      */
      time_shift = genevent_.ptime[i]; // GENBB time unit : second
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

    if (compute_classification_) {
      event_.compute_classification ();
    }

    event_.set_genbb_weight (1.0);
    DT_LOG_DEBUG(get_logging_priority(), "GENBB weight = " << get_to_all_events ());
    if (get_to_all_events () > 1.0) {
      event_.set_genbb_weight (1.0 / get_to_all_events ());
    }

    //event_.dump (clog , "GENBB event: ", "DEVEL: ");
    _event_count_++;
    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return;
  }

  size_t wgenbb::get_event_count () const
  {
    return _event_count_;
  }

  double wgenbb::get_to_all_events () const
  {
    return 1.0 * enrange_.toallevents;
  }

  void wgenbb::_init_ ()
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");

    DT_THROW_IF (_decay_type_ == DECAY_TYPE_UNDEFINED,
                 logic_error, "Decay type is not defined !");
    DT_THROW_IF (_decay_type_ == DECAY_TYPE_DBD && _decay_dbd_mode_ == DBD_MODE_INVALID,
                 logic_error, "Invalid DBD mode !");

    enrange_.reset ();
    const std::vector<int> & dbdmwer
      = utils::get_dbd_modes_with_energy_range ();
    DT_LOG_DEBUG(get_logging_priority(), "Decay DBD mode : " << _decay_dbd_mode_);
    if (std::find (dbdmwer.begin (), dbdmwer.end (),_decay_dbd_mode_) != dbdmwer.end ()) {
      if (datatools::is_valid (_energy_min_)) {
        DT_LOG_NOTICE(get_logging_priority(), "Setting energy min to "
                      << _energy_min_ / CLHEP::MeV << " MeV");
        enrange_.ebb1 = (float) (_energy_min_ / CLHEP::MeV);
      }
      if (datatools::is_valid (_energy_max_)) {
        DT_LOG_NOTICE(get_logging_priority(), "Setting energy max to "
                      << _energy_max_ / CLHEP::MeV << " MeV");
        enrange_.ebb2 = (float) (_energy_max_ / CLHEP::MeV);
      }
      DT_THROW_IF (enrange_.ebb1 >= enrange_.ebb2,logic_error,
                   "Invalid energy range !");
    } else {
      DT_LOG_DEBUG(get_logging_priority(),"Not a DBD energy range mode.");
    }

    if (! _use_local_prng_) {
      int genbb_seed = _seed_;
      set_genbb_random_seed_ (&genbb_seed);
    }
    int error = 0;
    int start = -1; // special initialization value without event generation
    genbbsub_ (&_decay_type_,
              _c_decay_isotope_,
              &_decay_dbd_level_,
              &_decay_dbd_mode_,
              &start,
              &error);
    DT_THROW_IF (error != 0, logic_error, "genbbsub failed !");
    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
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
    if (datatools::is_valid (_energy_min_)) {
      out_ << tag << "energy_min       : " << _energy_min_ / CLHEP::keV << " keV" << endl;
    }
    if (datatools::is_valid (_energy_max_)) {
      out_ << tag << "energy_max       : " << _energy_max_ / CLHEP::keV << " keV" << endl;
    }
    out_ << tag << "use local PRNG   : " << _use_local_prng_ << endl;
    out_ << last_tag << "seed             : " << _seed_ << endl;
    return;
  }

} // end of namespace genbb

// end of wgenbb.cc
