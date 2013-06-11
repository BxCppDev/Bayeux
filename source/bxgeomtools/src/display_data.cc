// -*- mode: c++ ; -*-
/* display_data.cc
 */

#include <geomtools/display_data.h>

#include <datatools/exception.h>

namespace geomtools {

  using namespace std;

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(display_data::display_item,
                                                    "geomtools::display_data::display_item")

  display_data::display_item::display_item ()
  {
    return;
  }

  display_data::display_item::~display_item ()
  {
    return;
  }

  void display_data::display_item::reset ()
  {
    frame_info.clear ();
    style.clear ();
    color.clear ();
    paths.clear ();
    return;
  }

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(display_data::display_entry,
                                                    "geomtools::display_data::display_entry")

  bool display_data::display_entry::is_static () const
  {
    return entry_type == DISPLAY_STATIC;
  }

  bool display_data::display_entry::is_framed () const
  {
    return entry_type == DISPLAY_FRAMED;
  }

  display_data::display_entry::display_entry ()
  {
    entry_type  = -1;
    return;
  }

  display_data::display_entry::~display_entry ()
  {
  }

  void display_data::display_entry::reset ()
  {
    entry_type  = -1;
    group.clear ();
    items.clear ();
    auxiliaries.clear ();
    return;
  }

  std::map<int32_t, display_data::display_item> &
  display_data::display_entry::grab_items ()
  {
    return items;
  }

  const std::map<int32_t, display_data::display_item> &
  display_data::display_entry::get_items () const
  {
    return items;
  }

  const display_data::display_item &
  display_data::display_entry::get_static_item () const
  {
    DT_THROW_IF (! is_static (), std::logic_error, "Not a static entry !");
    DT_THROW_IF (items.size () < 1, std::logic_error, "No available item is this entry !");
    return items.begin ()->second;
  }

  bool
  display_data::display_entry::has_framed_item (int frame_index_) const
  {
    if (! is_framed ()) return false;
    std::map<int32_t, display_item>::const_iterator found
      = items.find (frame_index_);
    return found != items.end ();
  }

  const display_data::display_item &
  display_data::display_entry::get_framed_item (int frame_index_) const
  {
    DT_THROW_IF (! is_framed (), std::logic_error, "Not a framed entry !");
    DT_THROW_IF (items.size () < 1, std::logic_error, "No available item is this entry !");
    std::map<int32_t, display_item>::const_iterator found
      = items.find (frame_index_);
    DT_THROW_IF (found == items.end (),
                 std::logic_error,
                 "No framed item with frame index " << frame_index_ << " is this entry !");
    return found->second;
  }

  /*********************************************/

  // serial tag for datatools::serialization::i_serializable interface :
  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(display_data, "geomtools::display_data")

  void display_data::reset ()
  {
    _entries_.clear ();
    _groups_.clear ();
    _colors_.clear ();
    _frames_.clear ();
    _auxiliaries_.clear ();
    return;
  }

  // ctor:
  display_data::display_data ()
  {
    return;
  }

  // dtor:
  display_data::~display_data ()
  {
    return;
  }

  /*** interface i_clear ***/
  void display_data::clear ()
  {
    reset ();
    return;
  }

  const std::vector<std::string> & display_data::get_colors () const
  {
    return _colors_;
  }

  std::vector<std::string> & display_data::grab_colors ()
  {
    return _colors_;
  }

  const std::vector<std::string> & display_data::get_groups () const
  {
    return _groups_;
  }

  std::vector<std::string> & display_data::grab_groups ()
  {
    return _groups_;
  }

  const std::map<int32_t,std::string> & display_data::get_frames () const
  {
    return _frames_;
  }

  std::map<int32_t,std::string> & display_data::grab_frames ()
  {
    return _frames_;
  }

  void display_data::add_frame_info(int frame_index_, const std::string & frame_info_)
  {
    std::map<int32_t,std::string>::iterator frame_found =
      _frames_.find (frame_index_);
    if (frame_found != _frames_.end())
      {
        frame_found->second = frame_info_;
      }
    return;
  }

  const std::map<std::string, display_data::display_entry> &
  display_data::get_entries () const
  {
    return _entries_;
  }

  std::map<std::string, display_data::display_entry> &
  display_data::grab_entries ()
  {
    return _entries_;
  }

  const datatools::properties &
  display_data::get_auxiliaries () const
  {
    return _auxiliaries_;
  }

  datatools::properties &
  display_data::grab_auxiliaries ()
  {
    return _auxiliaries_;
  }

  display_data::display_item &
  display_data::_add_item (const std::string & name_,
                           int entry_type_,
                           int frame_,
                           const std::string & group_,
                           const std::string & color_)
  {
    std::map<std::string, display_entry>::iterator found =
      _entries_.find (name_);
    display_entry * the_entry = 0;
    if (found == _entries_.end ())
      {
        {
          display_entry de;
          _entries_[name_] = de;
        }
        display_entry & de = _entries_[name_];
        de.entry_type = entry_type_;
        de.group = group_;
        if (! group_.empty ())
          {
            if (std::find (_groups_.begin (),
                           _groups_.end (),
                           group_) == _groups_.end ())
              {
                _groups_.push_back (group_);
              }
          }
        the_entry = &de;
      }
    else
      {
        the_entry = &found->second;
        DT_THROW_IF (the_entry->entry_type != entry_type_,
                     std::logic_error,
                     "Entry type mismatch for entry '" << name_ << "' !");
      }
    if (the_entry->entry_type == DISPLAY_STATIC)
      {
        DT_THROW_IF (frame_ != -1, std::logic_error, "Static item should have frame number -1 !");
        DT_THROW_IF (the_entry->items.size () > 0,  std::logic_error,
                     "Static entry '" << name_ << "' already has an item !");
        // add a new display item with frame number -1 :
        {
          display_item di;
          the_entry->items[-1] = di;
        }
        display_item & di = the_entry->items[-1];
        if (! color_.empty ())
          {
            if (std::find (_colors_.begin (),
                           _colors_.end (),
                           color_) == _colors_.end ())
              {
                _colors_.push_back (color_);
              }
          }
        di.color = color_;
        return di;
      }
    else
      {
        DT_THROW_IF (frame_ < 0, std::logic_error, "Algo step item should have frame number >= 0 !");
        std::map<int32_t, display_item>::iterator found
          = the_entry->items.find (frame_);
        DT_THROW_IF (found != the_entry->items.end (),
                     std::logic_error,
                     "Algo step item in entry '" << name_ << "' with frame number '" << frame_ << "' already exists !");
        // add a new display item with requested frame number :
        {
          display_item di;
          the_entry->items[frame_] = di;
        }
        display_item & di = the_entry->items[frame_];
        if (! color_.empty ())
          {
            if (std::find (_colors_.begin (),
                           _colors_.end (),
                           color_) == _colors_.end ())
              {
                _colors_.push_back (color_);
              }
          }
        di.color = color_;
        std::map<int32_t,std::string>::const_iterator frame_found =
          _frames_.find (frame_);
        if (frame_found == _frames_.end())
          {
            _frames_[frame_] = std::string("");
          }
        return di;
      }
  }

  void display_data::process()
  {
    for (entries_dict_type::const_iterator i
           = get_entries ().begin ();
         i != get_entries ().end ();
         i++)
      {
        const std::string & entry_name = i->first;
        const display_entry & de = i->second;
        if (de.is_framed ())
          {
            for (std::map<int32_t, display_item>::const_iterator j =
                   de.items.begin();
                 j != de.items.end();
                 j++)
              {
                int32_t frame_index = j->first;
                std::map<int32_t,std::string>::const_iterator frame_found =
                  _frames_.find(frame_index);
                if (frame_found == _frames_.end())
                  {
                    std::ostringstream frame_info_oss;
                    frame_info_oss << "Frame #" << frame_index;
                    _frames_[frame_index] = frame_info_oss.str();
                  }
              }
          }
      }
    return;
  }

  display_data::display_item &
  display_data::add_framed_item (const std::string & name_,
                                 int frame_,
                                 const std::string & group_,
                                 const std::string & color_,
                                 const std::string & frame_info_)
  {
    return _add_item (name_,
                      DISPLAY_FRAMED,
                      frame_,
                      group_,
                      color_);
    if (! frame_info_.empty())
      {
        add_frame_info(frame_, frame_info_);
      }
  }

  display_data::display_item &
  display_data::add_static_item (const std::string & name_,
                                 const std::string & group_,
                                 const std::string & color_)
  {
    return _add_item (name_,
                      DISPLAY_STATIC,
                      -1,
                      group_,
                      color_);
  }

  /*** interface i_tree_dumpable ***/
  void display_data::tree_dump (std::ostream & a_out,
                                const std::string & a_title,
                                const std::string & a_indent,
                                bool a_inherit) const
  {
    std::string indent;
    if (! a_indent.empty ())
      {
        indent = a_indent;
      }
    if (! a_title.empty ())
      {
        a_out << indent << a_title << endl;
      }

    // Display groups:
    a_out << indent << datatools::i_tree_dumpable::tag;
    a_out << "Display groups: " << _groups_.size () << std::endl;
    for (int i = 0; i < _groups_.size (); i++)
      {
        a_out << indent << datatools::i_tree_dumpable::skip_tag;
        if (i + 1  < _groups_.size ())
          a_out << datatools::i_tree_dumpable::tag;
        else
          a_out << datatools::i_tree_dumpable::last_tag;
        a_out << "Group : '" << _groups_[i] << "'" << std::endl;
      }

    // Display colors:
    a_out << indent << datatools::i_tree_dumpable::tag;
    a_out << "Colors : " << _colors_.size () << std::endl;
    for (int i = 0; i < _colors_.size (); i++)
      {
        a_out << indent << datatools::i_tree_dumpable::skip_tag;
        if (i + 1  < _colors_.size ())
          a_out << datatools::i_tree_dumpable::tag;
        else
          a_out << datatools::i_tree_dumpable::last_tag;
        a_out << "Color : '" << _colors_[i] << "'" << std::endl;
      }

    // Display frames:
    a_out << indent << datatools::i_tree_dumpable::tag;
    a_out << "Frames : " << _frames_.size () << std::endl;
    for (std::map<int32_t,std::string>::const_iterator i = _frames_.begin();
           i != _frames_.end();
         i++)
      {
        a_out << indent << datatools::i_tree_dumpable::skip_tag;
        std::map<int32_t,std::string>::const_iterator j = i;
        j++;
        if (j != _frames_.end())
          a_out << datatools::i_tree_dumpable::tag;
        else
          a_out << datatools::i_tree_dumpable::last_tag;
        a_out << "Frame " << i->first << " : '"
              << i->second << "'" << std::endl;
      }

    // Display entries:
    a_out << indent << datatools::i_tree_dumpable::inherit_tag (a_inherit);
    a_out << "Display entries: " << _entries_.size () << std::endl;
    for (std::map<std::string, display_entry>::const_iterator i = _entries_.begin ();
         i != _entries_.end ();
         i++)
      {
        a_out << indent << datatools::i_tree_dumpable::inherit_skip_tag (a_inherit);
        std::map<std::string, display_entry>::const_iterator j = i;
        j++;
        if (j !=  _entries_.end ())
          {
            a_out << datatools::i_tree_dumpable::tag;
          }
        else
          {
            a_out << datatools::i_tree_dumpable::last_tag;
          }
        const display_entry & de = i->second;
        a_out << "Entry : '" <<  i->first << "' ";
        if (de.is_static ())
          {
            a_out << "(static) ";
          }
        else
          {
            a_out << "(framed with " << de.items.size () << " steps) ";
          }
        if (! de.group.empty ())
          {
            a_out << "in group '" << de.group << "' ";
          }
        a_out << std::endl;
        for (std::map<int32_t, display_item>::const_iterator di
               =  de.items.begin ();
             di != de.items.end ();
             di++)
          {
            a_out << indent << datatools::i_tree_dumpable::inherit_skip_tag (a_inherit);
            if (j !=  _entries_.end ())
              {
                a_out << datatools::i_tree_dumpable::skip_tag;
              }
            else
              {
                a_out << datatools::i_tree_dumpable::last_skip_tag;
              }
            std::map<int32_t, display_item>::const_iterator dj = di;
            dj++;
            if (dj != de.items.end ())
              {
                //a_out << datatools::i_tree_dumpable::last_skip_tag;
                a_out << datatools::i_tree_dumpable::tag;
              }
            else
              {
                //a_out << datatools::i_tree_dumpable::skip_tag;
                a_out << datatools::i_tree_dumpable::last_tag;
              }
            const display_item & ditem = di->second;
            if (de.is_static ())
              {
                a_out << "Unique frame";
              }
            else
              {
                a_out << "Frame #" << di->first;
              }
            a_out << " has "
                  << ditem.paths.size () << " polyline(s) ";
            if (! ditem.color.empty ())
              {
                a_out << "(" << ditem.color << ") ";
              }
            a_out << std::endl;

          }
      }

    return;
  }

} // end of namespace geomtools

// end of display_data.cc
