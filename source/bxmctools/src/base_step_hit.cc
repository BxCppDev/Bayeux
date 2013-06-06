// -*- mode: c++ ; -*-
/* base_step_hit.cc
 */

#include <mctools/base_step_hit.h>

#include <cstdlib>
#include <stdexcept>
#include <sstream>

// misc :
#include <datatools/utils.h>
#include <datatools/exception.h>

#include <mctools/utils.h>

namespace mctools {

  using namespace std;

  // serial tag for datatools::serialization::i_serializable interface :
  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(base_step_hit, "mctools::base_step_hit")

  // special properties:
  bool base_step_hit::is_primary_particle () const
  {
    return get_auxiliaries ().has_flag (mctools::track_utils::PRIMARY_PARTICLE_FLAG);
  }

  bool base_step_hit::has_track_id () const
  {
    return get_auxiliaries ().has_key (mctools::track_utils::TRACK_ID_KEY);
  }

  bool base_step_hit::has_parent_track_id () const
  {
    return get_auxiliaries ().has_key (mctools::track_utils::PARENT_TRACK_ID_KEY);
  }

  int base_step_hit::get_track_id () const
  {
    int track_id = INVALID_HIT_ID;
    if (get_auxiliaries ().has_key (mctools::track_utils::TRACK_ID_KEY))
      {
        track_id = get_auxiliaries ().fetch_integer (mctools::track_utils::TRACK_ID_KEY);
      }
    return track_id;
  }

  int base_step_hit::get_parent_track_id () const
  {
    int parent_track_id = INVALID_HIT_ID;
    if (get_auxiliaries ().has_key (mctools::track_utils::PARENT_TRACK_ID_KEY))
      {
        parent_track_id = get_auxiliaries ().fetch_integer (mctools::track_utils::PARENT_TRACK_ID_KEY);
      }
    return parent_track_id;
  }

  const geomtools::vector_3d &
  base_step_hit::get_position_start () const
  {
    return _position_start_;
  }

  void
  base_step_hit::set_position_start (const geomtools::vector_3d & new_value)
  {
    _store |= STORE_POSITION_START;
    _position_start_ = new_value;
    return;
  }

  void base_step_hit::invalidate_position_start ()
  {
    geomtools::invalidate (_position_start_);
    _store &= ~STORE_POSITION_START;
    return;
  }

  const geomtools::vector_3d &
  base_step_hit::get_position_stop () const
  {
    return _position_stop_;
  }

  void
  base_step_hit::set_position_stop (const geomtools::vector_3d & new_value)
  {
    _store |= STORE_POSITION_STOP;
    _position_stop_ = new_value;
    return;
  }

  void base_step_hit::invalidate_position_stop ()
  {
    geomtools::invalidate (_position_stop_);
    _store &= ~STORE_POSITION_STOP;
    return;
  }

  double
  base_step_hit::get_time_start () const
  {
    return _time_start_;
  }

  void
  base_step_hit::set_time_start (double new_value)
  {
    _store |= STORE_TIME_START;
    _time_start_ = new_value;
    return;
  }

  void base_step_hit::invalidate_time_start ()
  {
    datatools::invalidate (_time_start_);
    _store &= ~STORE_TIME_START;
    return;
  }

  double
  base_step_hit::get_time_stop () const
  {
    return _time_stop_;
  }

  void
  base_step_hit::set_time_stop (double new_value)
  {
    _store |= STORE_TIME_STOP;
    _time_stop_ = new_value;
    return;
  }


  void base_step_hit::invalidate_time_stop ()
  {
    datatools::invalidate (_time_stop_);
    _store &= ~STORE_TIME_STOP;
    return;
  }

  const geomtools::vector_3d &
  base_step_hit::get_momentum_start () const
  {
    return _momentum_start_;
  }

  void
  base_step_hit::set_momentum_start (const geomtools::vector_3d & new_value)
  {
    _store |= STORE_MOMENTUM_START;
    _momentum_start_ = new_value;
    return;
  }

  void base_step_hit::invalidate_momentum_start ()
  {
    geomtools::invalidate (_momentum_start_);
    _store &= ~STORE_MOMENTUM_START;
    return;
  }

  const geomtools::vector_3d &
  base_step_hit::get_momentum_stop () const
  {
    return _momentum_stop_;
  }

  void
  base_step_hit::set_momentum_stop (const geomtools::vector_3d & new_value)
  {
    _store |= STORE_MOMENTUM_STOP;
    _momentum_stop_ = new_value;
    return;
  }

  void base_step_hit::invalidate_momentum_stop ()
  {
    geomtools::invalidate (_momentum_stop_);
    _store &= ~STORE_MOMENTUM_STOP;
    return;
  }

  double
  base_step_hit::get_energy_deposit () const
  {
    return _energy_deposit_;
  }

  void
  base_step_hit::set_energy_deposit (double new_value)
  {
    _store |= STORE_ENERGY_DEPOSIT;
    _energy_deposit_ = new_value;
    return;
  }

  void base_step_hit::invalidate_energy_deposit ()
  {
    datatools::invalidate (_energy_deposit_);
    _store &= ~STORE_ENERGY_DEPOSIT;
    return;
  }

  // ctor:
  base_step_hit::base_step_hit () : geomtools::base_hit ()
  {
    base_step_hit::reset ();
    return;
  }

  // dtor:
  base_step_hit::~base_step_hit ()
  {
    base_step_hit::reset ();
    return;
  }

  bool base_step_hit::is_valid () const
  {
    if (! geomtools::base_hit::is_valid ()) return false;
    if (! datatools::is_valid (_time_start_)) return false;
    //if (! datatools::is_valid (_time_stop_)) return false;
    if (! geomtools::is_valid_vector_3d (_position_start_)) return false;
    //if (! geomtools::is_valid_vector_3d (_position_stop_)) return false;
    return true;
  }

  const string & base_step_hit::get_particle_name () const
  {
    return _particle_name_;
  }

  void base_step_hit::set_particle_name (const string & n)
  {
    _store |= STORE_PARTICLE_NAME;
    _particle_name_ = n;
    return;
  }

  void base_step_hit::invalidate_particle_name ()
  {
    _particle_name_ = "";
    _store &= ~STORE_PARTICLE_NAME;
    return;
  }

  void base_step_hit::reset ()
  {
    invalidate_position_start ();
    invalidate_position_stop ();
    invalidate_time_start ();
    invalidate_time_stop ();
    invalidate_momentum_start ();
    invalidate_momentum_stop ();
    invalidate_energy_deposit ();
    invalidate_particle_name ();
    geomtools::base_hit::reset ();
    return;
  }

  void base_step_hit::clear ()
  {
    base_step_hit::reset ();
    return;
  }

  void base_step_hit::invalidate ()
  {
    base_step_hit::reset ();
    return;
  }

  void base_step_hit::tree_dump (ostream & a_out ,
                                 const string & a_title,
                                 const string & a_indent,
                                 bool a_inherit) const
  {
    namespace du = datatools;
    string indent;
    if (! a_indent.empty ())
      {
        indent = a_indent;
      }
    geomtools::base_hit::tree_dump (a_out, a_title, indent, true);

    a_out << indent << du::i_tree_dumpable::tag
          << "Position start : " << _position_start_ / CLHEP::mm  << " mm" << endl;
    a_out << indent << du::i_tree_dumpable::tag
          << "Position stop  : " << _position_stop_ / CLHEP::mm << " mm" << endl;
    a_out << indent << du::i_tree_dumpable::tag
          << "Time start     : " << _time_start_ / CLHEP::ns << " ns" << endl;
    a_out << indent << du::i_tree_dumpable::tag
          << "Time stop      : " << _time_stop_ / CLHEP::ns << " ns" << endl;
    a_out << indent << du::i_tree_dumpable::tag
          << "Momentum start : " << _momentum_start_ / CLHEP::keV << " keV" << endl;
    a_out << indent << du::i_tree_dumpable::tag
          << "Momentum stop  : " << _momentum_stop_ / CLHEP::keV << " keV" << endl;
    a_out << indent << du::i_tree_dumpable::tag
          << "Energy deposit : " << _energy_deposit_ / CLHEP::keV << " keV" << endl;
    a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)
          << "Particle name  : `" << _particle_name_ << "'" << endl;

    return;
  }

  void base_step_hit::dump () const
  {
    tree_dump (clog);
    return;
  }

} // end of namespace mctools

// end of base_step_hit.cc
