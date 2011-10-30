// -*- mode: c++; -*- 
/* basic_event.cc
 */

#include <datatools/event/basic_event.h>

namespace datatools {
  
  namespace event {

    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (basic_event,"datatools:event::basic_event")

    void basic_event::dump () const
    {
      tree_dump (std::clog);
      return;
    }
    
    void basic_event::id (const event_id & a_id)
    {
      _id_ = a_id;
      return;
    }
    
    const event_id & basic_event::id () const
    {
      return _id_;
    }

    void basic_event::set_id (const event_id & a_id)
    {
      id  (a_id);
      return;
    }
    
    event_id & basic_event::id ()
    {
      return _id_;
    }
     
    const event_id & basic_event::get_id () const
    {
      return id ();
    }
    
    const datatools::utils::properties & basic_event::properties () const
    {
      return _properties_;
    }
    
    datatools::utils::properties & basic_event::properties ()
    {
      return _properties_;
    }

    const datatools::utils::properties & basic_event::get_properties_ro () const
    {
      return properties ();
    }

    datatools::utils::properties & basic_event::get_properties_rw ()
    {
      return properties ();
    }

    void basic_event::clear ()
    {
      _id_.clear ();
      _properties_.clear ();
      return;
    }
    
    basic_event::basic_event ()
    {
      return;
    }
    
    basic_event::basic_event (const event_id & a_id) : _id_ (a_id)
    {
      return;
    }
    
    basic_event::~basic_event ()
    {
      basic_event::clear ();
      return;
    }

    void basic_event::tree_dump (std::ostream & a_out         , 
                                 const std::string & a_title  ,
                                 const std::string & a_indent ,
                                 bool  a_inherit              ) const
    {
      std::string indent;
      if (!a_indent.empty ()) indent = a_indent;
      if (!a_title.empty ()) {
        a_out << indent << a_title << std::endl;
      }
      namespace du = datatools::utils;
      a_out << indent << du::i_tree_dumpable::tag 
            << "Id : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << du::i_tree_dumpable::skip_tag;
        _id_.tree_dump (a_out,"",indent_oss.str ());
      }

      a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)
            << "Properties : ";
      if (_properties_.size () == 0) { a_out << "<empty>"; }
      a_out << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << du::i_tree_dumpable::inherit_skip_tag (a_inherit);
        _properties_.tree_dump (a_out,"",indent_oss.str ());
      }
      return;
    }
    
  } // end of namespace event 

} // end of namespace datatools

// end of basic_event.cc
