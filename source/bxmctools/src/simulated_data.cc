// -*- mode: c++ ; -*-
/* simulated_data.cc
 */

#include <mctools/simulated_data.h>

#include <boost/algorithm/string.hpp>

#include <datatools/exception.h>

#include <geomtools/utils.h>

namespace mctools {

  using namespace std;

  /*** serialization ***/
  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (simulated_data,"mctools::simulated_data")

  void simulated_data::reset_collection_type ()
  {
    DT_THROW_IF (_step_hits_dict_.size () > 0 || _plain_step_hits_dict_.size () > 0,
                 std::logic_error,
                 "Cannot reset the collection type "
                 << "for the dictionary of hit collections is not empty !");
    _collection_type_ = INVALID_HIT_COLLECTION_TYPE;
    return;
  }

  void simulated_data::set_collection_type (int a_collection_type)
  {
    DT_THROW_IF (_step_hits_dict_.size () > 0 || _plain_step_hits_dict_.size () > 0,
                 std::logic_error,
                 "Cannot set the collection type "
                 << "for the dictionary of hit collections is not empty !");
    DT_THROW_IF (_collection_type_ != INVALID_HIT_COLLECTION_TYPE,
                 std::logic_error,
                 "Cannot change the collection type !");
    DT_THROW_IF ((a_collection_type != PLAIN_HIT_COLLECTION_TYPE)
                 && (a_collection_type != HANDLE_HIT_COLLECTION_TYPE),
                 std::logic_error,
                 "Invalid collection type '" << a_collection_type << "' !");
    /*
      if (a_collection_type == PLAIN_HIT_COLLECTION_TYPE)
      {
      ostringstream message;
      message << "mctools::simulated_data::set_collection_type: "
      << "Collection type '" << "plain" << "' is not supported anymore !";
      th row logic_error (message.str ());

      }
    */
    _collection_type_ = a_collection_type;
    return;
  }

  bool simulated_data::use_plain_hit_collection () const
  {
    return _collection_type_ == PLAIN_HIT_COLLECTION_TYPE;
  }

  bool simulated_data::use_handle_hit_collection () const
  {
    return _collection_type_ == HANDLE_HIT_COLLECTION_TYPE;
  }

  bool simulated_data::has_data () const
  {
    return has_vertex ();
  }

  bool simulated_data::has_vertex () const
  {
    return geomtools::is_valid (_vertex_);
  }

  const geomtools::vector_3d & simulated_data::get_vertex () const
  {
    return _vertex_;
  }

  geomtools::vector_3d & simulated_data::grab_vertex ()
  {
    return _vertex_;
  }

  void simulated_data::set_vertex (const geomtools::vector_3d & a_v)
  {
    _vertex_ = a_v;
    return;
  }

  const simulated_data::primary_event_type & simulated_data::get_primary_event () const
  {
    return _primary_event_;
  }

  simulated_data::primary_event_type & simulated_data::grab_primary_event ()
  {
    return _primary_event_;
  }

  void simulated_data::set_primary_event (const simulated_data::primary_event_type & a_pe)
  {
    _primary_event_ = a_pe;
    return;
  }

  void simulated_data::set_properties(const datatools::properties & p)
  {
    _properties_ = p;
  }

  const datatools::properties & simulated_data::get_properties () const
  {
    return _properties_;
  }

  datatools::properties & simulated_data::grab_properties ()
  {
    return _properties_;
  }

  simulated_data::step_hits_dict_type &
  simulated_data::grab_step_hits_dict ()
  {
    return _step_hits_dict_;
  }

  const simulated_data::step_hits_dict_type &
  simulated_data::get_step_hits_dict () const
  {
    return _step_hits_dict_;
  }

  simulated_data::plain_step_hits_dict_type &
  simulated_data::grab_plain_step_hits_dict ()
  {
    return _plain_step_hits_dict_;
  }

  const simulated_data::plain_step_hits_dict_type &
  simulated_data::get_plain_step_hits_dict () const
  {
    return _plain_step_hits_dict_;
  }

  void simulated_data::get_step_hits_categories (vector<string> & the_categories,
                                                 unsigned int a_mode,
                                                 const string & a_prefix) const
  {
    the_categories.clear ();
    if (a_mode == HIT_CATEGORY_TYPE_ALL) {
      if (use_handle_hit_collection ()) {
        the_categories.assign (_step_hits_dict_.size (), "");
        int count = 0;
        for (step_hits_dict_type::const_iterator i = _step_hits_dict_.begin ();
             i != _step_hits_dict_.end ();
             i++) {
          const string & category = i->first;
          the_categories[count] = category;
          ++count;
        }
      }
      if (use_plain_hit_collection ()) {
        the_categories.assign (_plain_step_hits_dict_.size (), "");
        int count = 0;
        for (plain_step_hits_dict_type::const_iterator i = _plain_step_hits_dict_.begin ();
             i != _plain_step_hits_dict_.end ();
             i++) {
          const string & category = i->first;
          the_categories[count] = category;
          ++count;
        }
      }
    }
    else if (a_mode == HIT_CATEGORY_TYPE_PUBLIC) {
      if (use_handle_hit_collection ()) {
        the_categories.reserve (_step_hits_dict_.size ());
        for (step_hits_dict_type::const_iterator i = _step_hits_dict_.begin ();
             i != _step_hits_dict_.end ();
             i++) {
          const string & category = i->first;
          if (! boost::starts_with(category, "--")) {
            the_categories.push_back (category);
          }
        }
      }
      if (use_plain_hit_collection ()) {
        the_categories.reserve (_plain_step_hits_dict_.size ());
        for (plain_step_hits_dict_type::const_iterator i = _plain_step_hits_dict_.begin ();
             i != _plain_step_hits_dict_.end ();
             i++) {
          const string & category = i->first;
          if (! boost::starts_with(category, "--")) {
            the_categories.push_back (category);
          }
        }
      }
    }
    else if (a_mode == HIT_CATEGORY_TYPE_PRIVATE) {
      if (use_handle_hit_collection ()) {
        the_categories.reserve (_step_hits_dict_.size ());
        for (step_hits_dict_type::const_iterator i = _step_hits_dict_.begin ();
             i != _step_hits_dict_.end ();
             i++) {
          const string & category = i->first;
          if (boost::starts_with(category, "--")) {
            the_categories.push_back (category);
          }
        }
      }
      if (use_plain_hit_collection ()) {
        the_categories.reserve (_plain_step_hits_dict_.size ());
        for (plain_step_hits_dict_type::const_iterator i = _plain_step_hits_dict_.begin ();
             i != _plain_step_hits_dict_.end ();
             i++) {
          const string & category = i->first;
          if (boost::starts_with(category, "--")) {
            the_categories.push_back (category);
          }
        }
      }
    }
    else if (a_mode == HIT_CATEGORY_TYPE_PREFIX && ! a_prefix.empty ()) {
      if (use_handle_hit_collection ()) {
        the_categories.reserve (_step_hits_dict_.size ());
        for (step_hits_dict_type::const_iterator i = _step_hits_dict_.begin ();
             i != _step_hits_dict_.end ();
             i++) {
          const string & category = i->first;
          if (boost::starts_with(category, a_prefix)) {
            the_categories.push_back (category);
          }
        }
      }
      if (use_plain_hit_collection ()) {
        the_categories.reserve (_plain_step_hits_dict_.size ());
        for (plain_step_hits_dict_type::const_iterator i = _plain_step_hits_dict_.begin ();
             i != _plain_step_hits_dict_.end ();
             i++) {
          const string & category = i->first;
          if (boost::starts_with(category, a_prefix)) {
            the_categories.push_back (category);
          }
        }
      }
    }
    else {
      DT_THROW_IF(true, std::logic_error, "Invalid mode !");
    }
    return;
  }

  simulated_data & simulated_data::add_step_hits (const string & a_category, size_t a_capacity)
  {
    if (use_handle_hit_collection ()) {
      if (has_step_hits (a_category)) {
        return *this;
      }
      hit_handle_collection_type dummy;
      _step_hits_dict_[a_category] = dummy;
      if (a_capacity > 0) {
        _step_hits_dict_[a_category].reserve (a_capacity);
      }
    }
    if (use_plain_hit_collection ()) {
      if (has_step_hits (a_category)) {
        return *this;
      }
      hit_collection_type dummy;
      _plain_step_hits_dict_[a_category] = dummy;
      if (a_capacity > 0) {
        _plain_step_hits_dict_[a_category].reserve (a_capacity);
      }
    }
    return *this;
  }

  simulated_data & simulated_data::remove_step_hits (const string & a_category)
  {
    if (use_handle_hit_collection ()) {
      step_hits_dict_type::iterator found = _step_hits_dict_.find (a_category);
      if (found == _step_hits_dict_.end ()) {
        return *this;
      }
      _step_hits_dict_.erase (found);
    }
    if (use_plain_hit_collection ()) {
      plain_step_hits_dict_type::iterator found = _plain_step_hits_dict_.find (a_category);
      if (found == _plain_step_hits_dict_.end ()) {
        return *this;
      }
      _plain_step_hits_dict_.erase (found);
    }
    return *this;
  }

  base_step_hit &
  simulated_data::add_step_hit (const string & a_category)
  {
    if (use_handle_hit_collection ()) {
      step_hits_dict_type::iterator found
        = _step_hits_dict_.find (a_category);
      DT_THROW_IF (found == _step_hits_dict_.end (),
                   std::logic_error,
                   "No collection of hits with category '" << a_category << "' !");
      found->second.push_back (hit_handle_type (new base_step_hit));
      return found->second.back ().grab ();
    }
    if (use_plain_hit_collection ()) {
      plain_step_hits_dict_type::iterator found
        = _plain_step_hits_dict_.find (a_category);
      DT_THROW_IF (found == _plain_step_hits_dict_.end (),
                   std::logic_error,
                   "No collection of hits with category '" << a_category << "' !");
      base_step_hit dummy;
      found->second.push_back (dummy);
      return found->second.back ();
    }
  }

  size_t simulated_data::get_number_of_step_hits (const string & a_category) const
  {
    if (use_handle_hit_collection ()) {
      step_hits_dict_type::const_iterator found
        = _step_hits_dict_.find (a_category);
      DT_THROW_IF (found == _step_hits_dict_.end (),
                   std::logic_error,
                   "No collection of hits with category '" << a_category << "' !");
      return found->second.size ();
    }
    if (use_plain_hit_collection ()) {
      plain_step_hits_dict_type::const_iterator found
        = _plain_step_hits_dict_.find (a_category);
      DT_THROW_IF (found == _plain_step_hits_dict_.end (),
                   std::logic_error,
                   "No collection of hits with category '" << a_category << "' !");
      return found->second.size ();
    }
  }

  const base_step_hit &
  simulated_data::get_step_hit (const string & a_category, int a_hit_index) const
  {
    if (use_handle_hit_collection ()) {
      step_hits_dict_type::const_iterator found
        = _step_hits_dict_.find (a_category);
      DT_THROW_IF (found == _step_hits_dict_.end (),
                   std::logic_error,
                   "No collection of handles of hits with category '" << a_category << "' !");
      DT_THROW_IF (a_hit_index < 0 || a_hit_index >= found->second.size (),
                   std::logic_error,
                   "Invalid hit index in category '" << a_category << "' !");
      DT_THROW_IF (! found->second[a_hit_index].has_data (),
                   std::logic_error,
                   "null handle at index " << a_hit_index << " in category '" << a_category << "' !");
      return found->second[a_hit_index].get ();
    }
    if (use_plain_hit_collection ()) {
      plain_step_hits_dict_type::const_iterator found
        = _plain_step_hits_dict_.find (a_category);
      DT_THROW_IF (found == _plain_step_hits_dict_.end (),
                   std::logic_error,
                   "No collection of plain hits with category '" << a_category << "' !");
      DT_THROW_IF (a_hit_index < 0 || a_hit_index >= found->second.size (),
                   std::logic_error,
                   "Invalid hit index in category '" << a_category << "' !");
      return found->second[a_hit_index];
    }
  }

  base_step_hit &
  simulated_data::grab_step_hit (const string & a_category, int a_hit_index)
  {
    if (use_handle_hit_collection ()) {
      step_hits_dict_type::iterator found
        = _step_hits_dict_.find (a_category);
      DT_THROW_IF (found == _step_hits_dict_.end (),
                   std::logic_error,
                   "No collection of handles of hits with category '" << a_category << "' !");
      DT_THROW_IF (a_hit_index < 0 || a_hit_index >= found->second.size (),
                   std::logic_error,
                   "Invalid hit index in category '" << a_category << "' !");
      DT_THROW_IF (! found->second[a_hit_index].has_data (),
                   std::logic_error,
                   "Null handle at index " << a_hit_index << " in category '" << a_category << "' !");
      return found->second[a_hit_index].grab ();
    }
    if (use_plain_hit_collection ()) {
      plain_step_hits_dict_type::iterator found
        = _plain_step_hits_dict_.find (a_category);
      DT_THROW_IF (found == _plain_step_hits_dict_.end (),
                   std::logic_error,
                   "No collection of plain hits with category '" << a_category << "' !");
      DT_THROW_IF (a_hit_index < 0 || a_hit_index >= found->second.size (),
                   std::logic_error,
                   "Invalid hit index in category '" << a_category << "' !");
      return found->second[a_hit_index];
    }
  }

  bool simulated_data::has_step_hits (const string & a_category) const
  {
    if (use_handle_hit_collection ()) {
      return _step_hits_dict_.find (a_category) != _step_hits_dict_.end ();
    }
    if (use_plain_hit_collection ()) {
      return _plain_step_hits_dict_.find (a_category) != _plain_step_hits_dict_.end ();
    }
    return false;
  }

  simulated_data::hit_handle_collection_type &
  simulated_data::grab_step_hits (const string & a_category)
  {
    DT_THROW_IF (! use_handle_hit_collection (),
                 std::logic_error,
                 "Invalid mode !");
    step_hits_dict_type::iterator found = _step_hits_dict_.find (a_category);
    DT_THROW_IF (found == _step_hits_dict_.end (),
                 std::logic_error,
                 "No collection of hits with category '" << a_category << "' !");
    return found->second;
  }

  const simulated_data::hit_handle_collection_type &
  simulated_data::get_step_hits (const string & a_category) const
  {
    DT_THROW_IF (! use_handle_hit_collection (),
                 std::logic_error,
                 "Invalid mode !");
    step_hits_dict_type::const_iterator found = _step_hits_dict_.find (a_category);
    DT_THROW_IF (found == _step_hits_dict_.end (),
                 std::logic_error,
                 "No collection of hits with category '" << a_category << "' !");
    return found->second;
  }


  simulated_data::hit_collection_type &
  simulated_data::grab_plain_step_hits (const string & a_category)
  {
    DT_THROW_IF (! use_plain_hit_collection (),
                 std::logic_error,
                 "Invalid mode !");
    plain_step_hits_dict_type::iterator found = _plain_step_hits_dict_.find (a_category);
    DT_THROW_IF (found == _plain_step_hits_dict_.end (),
                 std::logic_error,
                 "No collection of plain hits with category '" << a_category << "' !");
    return found->second;
  }

  const simulated_data::hit_collection_type &
  simulated_data::get_plain_step_hits (const string & a_category) const
  {
    DT_THROW_IF (! use_plain_hit_collection (),
                 std::logic_error,
                 "Invalid mode !");
    plain_step_hits_dict_type::const_iterator found = _plain_step_hits_dict_.find (a_category);
    DT_THROW_IF (found == _plain_step_hits_dict_.end (),
                 std::logic_error,
                 "No collection of plain hits with category '" << a_category << "' !");
    return found->second;
  }

  // ctor:
  simulated_data::simulated_data ()
  {
    geomtools::invalidate (_vertex_);
    _collection_type_ = HANDLE_HIT_COLLECTION_TYPE;
    return;
  }

  // ctor:
  simulated_data::simulated_data (int a_collection_type)
  {
    geomtools::invalidate (_vertex_);
    _collection_type_ = INVALID_HIT_COLLECTION_TYPE;
    set_collection_type (a_collection_type);
    return;
  }

  // dtor:
  simulated_data::~simulated_data ()
  {
    reset ();
    return;
  }

  simulated_data & simulated_data::reset (bool a_reset_collection_type)
  {
    geomtools::invalidate (_vertex_);
    _primary_event_.reset ();
    _properties_.clear ();
    if (use_handle_hit_collection ()) {
      _step_hits_dict_.clear ();
    }
    if (use_plain_hit_collection ()) {
      _plain_step_hits_dict_.clear ();
    }
    if (a_reset_collection_type) {
      _collection_type_ = INVALID_HIT_COLLECTION_TYPE;
    }
    return *this;
  }

  void simulated_data::clear ()
  {
    reset (false);
    return;
  }

  /*** tree_dump ***/

  void simulated_data::tree_dump (ostream & a_out,
                                  const string & a_title,
                                  const string & a_indent,
                                  bool a_inherit) const
  {
    string indent;
    if (! a_indent.empty ()) {
      indent = a_indent;
    }
    if ( !a_title.empty () ) {
      a_out << indent << a_title << endl;
    }
    namespace du = datatools;

    // Properties:
    {
      a_out << indent << du::i_tree_dumpable::tag
            << "Properties : ";
      if (_properties_.size () == 0) {
        a_out << "<empty>";
      }
      a_out << endl;
      {
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << du::i_tree_dumpable::skip_tag;
        _properties_.tree_dump (a_out, "", indent_oss.str ());
      }
    }

    // Collection type :
    a_out << indent << du::i_tree_dumpable::tag
          << "Collection type : " << (int) _collection_type_ << endl;

    // Step hits collections (handle type):
    if (use_handle_hit_collection ()) {
      a_out << indent << du::i_tree_dumpable::tag
            << "Handle step hit collections : ";
      if (_step_hits_dict_.size () == 0) {
        a_out << "None";
      }
      a_out << endl;
      for (step_hits_dict_type::const_iterator i = _step_hits_dict_.begin ();
           i != _step_hits_dict_.end ();
           i++) {
        const string & category = i->first;
        size_t no_hits = i->second.size ();
        step_hits_dict_type::const_iterator j = i;
        j++;
        a_out << indent << du::i_tree_dumpable::skip_tag;
        if (j == _step_hits_dict_.end ()) {
          a_out << du::i_tree_dumpable::last_tag;
        }
        else {
          a_out << du::i_tree_dumpable::tag;
        }
        a_out << "Category '" << category << "' has "
              << no_hits << " hit(s)"  << " [capacity="
              << i->second.capacity () << ']' << endl;
      }
    }

    // Step hits collections (plain type):
    if (use_plain_hit_collection ())
      {
        a_out << indent << du::i_tree_dumpable::tag
              << "Plain step hit collections : ";
        if (_plain_step_hits_dict_.size () == 0) {
          a_out << "None";
        }
        a_out << endl;
        for (plain_step_hits_dict_type::const_iterator i = _plain_step_hits_dict_.begin ();
             i != _plain_step_hits_dict_.end ();
             i++) {
          const string & category = i->first;
          size_t no_hits = i->second.size ();
          plain_step_hits_dict_type::const_iterator j = i;
          j++;
          a_out << indent << du::i_tree_dumpable::skip_tag;
          if (j == _plain_step_hits_dict_.end ()) {
            a_out << du::i_tree_dumpable::last_tag;
          }
          else {
            a_out << du::i_tree_dumpable::tag;
          }
          a_out << "Category '" << category << "' has "
                << no_hits << " hit(s)"  << " [capacity="
                << i->second.capacity () << ']' << endl;
        }
      }

    // Primary event:
    {
      a_out << indent << du::i_tree_dumpable::tag
            << "Primary event : " << endl;
      {
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << du::i_tree_dumpable::skip_tag;
        _primary_event_.tree_dump (a_out, "", indent_oss.str ());
      }
    }

    // Vertex:
    {
      a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)
            << "Vertex : " << _vertex_ / CLHEP::mm  << " mm"<< endl;
    }

    return;
  }

} // end of namespace mctools

// end of simulated_data.cc
