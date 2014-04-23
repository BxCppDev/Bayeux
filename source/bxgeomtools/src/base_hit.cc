/* base_hit.cc
 */

// Ourselves:
#include <geomtools/base_hit.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/ioutils.h>

namespace geomtools {

  using namespace std;

  // serial tag for datatools::serialization::i_serializable interface :
  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(base_hit, "snemo::core::model::base_hit")

  base_hit & base_hit::i_measurement::operator () (base_hit & a_hit)
  {
    do_measurement (a_hit);
    return a_hit;
  }

  base_hit & base_hit::measure (i_measurement & a_measurement)
  {
    return a_measurement (*this);
  }

  bool base_hit::has_hit_id () const
  {
    return _hit_id_ > INVALID_HIT_ID;
  }

  int32_t base_hit::get_hit_id () const
  {
    return _hit_id_;
  }

  void base_hit::set_hit_id (int32_t a_hit_id)
  {
    if (a_hit_id > INVALID_HIT_ID)
      {
        _store |= STORE_HIT_ID;
        _hit_id_ = a_hit_id;
      }
    else
      {
        base_hit::invalidate_hit_id ();
      }
    return;
  }

  void base_hit::invalidate_hit_id ()
  {
    _hit_id_ = INVALID_HIT_ID;
    _store &= ~STORE_HIT_ID;
    return;
  }

  bool base_hit::has_geom_id () const
  {
    return _geom_id_.is_valid ();
  }

  void base_hit::invalidate_geom_id ()
  {
    _geom_id_.invalidate ();
    _store &= ~STORE_GEOM_ID;
    return;
  }

  const geomtools::geom_id & base_hit::get_geom_id () const
  {
    return _geom_id_;
  }

  geomtools::geom_id & base_hit::grab_geom_id ()
  {
    // 2012-06-15, FM : fix missing _store update.
    _store |= STORE_GEOM_ID;
    return _geom_id_;
  }

  void base_hit::set_geom_id (const geomtools::geom_id & a_gid)
  {
    _store |= STORE_GEOM_ID;
    _geom_id_ = a_gid;
    return;
  }

  // Check if there are stored auxiliary properties
  bool base_hit::has_auxiliaries () const
  {
    return (_store & STORE_AUXILIARIES) && ! _auxiliaries_.empty();
  }

  const datatools::properties &
  base_hit::get_auxiliaries () const
  {
    return _auxiliaries_;
  }

  datatools::properties &
  base_hit::grab_auxiliaries ()
  {
    _store |= STORE_AUXILIARIES;
    return _auxiliaries_;
  }

  void base_hit::set_auxiliaries (const datatools::properties & a_)
  {
    _auxiliaries_ = a_;
  }

  void base_hit::invalidate_auxiliaries ()
  {
    _auxiliaries_.clear ();
    _store &= ~STORE_AUXILIARIES;
    return;
  }

  base_hit::base_hit ()
  {
    _store = STORE_NOTHING;
    _hit_id_ = INVALID_HIT_ID;
    _geom_id_.invalidate ();
    return;
  }

  base_hit::~base_hit ()
  {
    this->base_hit::reset ();
    return;
  }

  bool base_hit::is_valid () const
  {
    return (_hit_id_ != INVALID_HIT_ID) && _geom_id_.is_valid ();
  }

  void base_hit::reset ()
  {
    invalidate_hit_id ();
    invalidate_geom_id ();
    invalidate_auxiliaries ();
    //_store = STORE_NOTHING;
    return;
  }

  void base_hit::invalidate ()
  {
    base_hit::reset ();
    return;
  }

  void base_hit::clear ()
  {
    base_hit::reset ();
    return;
  }

  void base_hit::tree_dump (ostream & a_out,
                            const string & a_title,
                            const string & a_indent,
                            bool a_inherit) const
  {
    string indent;
    if (! a_indent.empty ())
      {
        indent = a_indent;
      }
    if ( ! a_title.empty () )
      {
        a_out << indent << a_title << endl;
      }

    namespace du = datatools;

    a_out << indent << du::i_tree_dumpable::tag
          << "Store       : " << datatools::io::to_binary (_store) << endl;

    a_out << indent << du::i_tree_dumpable::tag
          << "Hit ID      : ";
    if (has_hit_id ())
      {
        a_out << _hit_id_;
      }
    else
      {
        a_out << "No";
      }
    a_out << endl;
    a_out << indent << du::i_tree_dumpable::tag
          << "Geometry ID : ";
    if (_geom_id_.is_valid ())
      {
        a_out << _geom_id_;
      }
    else
      {
        a_out << "No";
      }
    a_out << endl;

    a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)
          << "Auxiliaries : ";
    if (_auxiliaries_.empty ())
      {
        a_out << "<empty>";
      }
    a_out << endl;
    {
      ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << du::i_tree_dumpable::inherit_skip_tag (a_inherit) ;
      _auxiliaries_.tree_dump (a_out, "", indent_oss.str ());
    }
    return;
  }

  void base_hit::dump () const
  {
    tree_dump (clog, "geomtools::base_hit");
    return;
  }

} // end of namespace geomtools
