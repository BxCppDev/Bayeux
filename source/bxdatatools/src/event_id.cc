// -*- mode: c++; -*- 
/* event_id.cc
 */

#include <datatools/event/event_id.h>

namespace datatools {

  namespace event {

    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (event_id,"datatools:event::event_id")

    void 
    event_id::clear ()
    {
      set_run_number (INVALID_RUN_NUMBER);
      set_event_number (INVALID_EVENT_NUMBER);
      return;
    }

    int 
    event_id::get_run_number () const
    {
      return _run_number_;
    }

    int 
    event_id::get_event_number () const
    {
      return _event_number_;
    }

    void 
    event_id::set_run_number (int a_run_number)
    {
      _run_number_ = (a_run_number < 0)? INVALID_RUN_NUMBER: a_run_number ;    
      return;
    }

    void 
    event_id::set_event_number (int a_event_number)
    {
      _event_number_ = (a_event_number < 0)? INVALID_EVENT_NUMBER: a_event_number;
      return;
    }

    void 
    event_id::set (int a_run_number, 
                   int a_event_number)
    {
      set_run_number (a_run_number);
      set_event_number (a_event_number);
    }

    event_id::event_id ()
    {
      _run_number_ = INVALID_RUN_NUMBER;
      _event_number_ = INVALID_EVENT_NUMBER;
      return;
    }

    event_id::event_id ( int a_event_number )
    {
      _run_number_ = INVALID_RUN_NUMBER;
      set_event_number (a_event_number);
      return;
    }

    event_id::event_id (int a_run_number , 
                        int a_event_number)
    {
      set (a_run_number,a_event_number);
      return;
    }
  
    event_id::~event_id ()
    {
      return;
    }

    bool
    event_id::has (int a_run_number, int a_event_number) const
    {
      return (_run_number_ == a_run_number) 
        && (_event_number_ == a_event_number);
    }

    bool 
    event_id::is_valid () const
    {
      return (_run_number_ != INVALID_RUN_NUMBER) 
        && (_event_number_ != INVALID_EVENT_NUMBER);
    }

    bool
    event_id::operator== (const event_id & a_id) const
    {
      return has (a_id._run_number_, a_id._event_number_);
    }

    bool
    event_id::operator< (const event_id & a_id) const
    {
      if (_run_number_ < a_id._run_number_) return true;
      if (_run_number_ == a_id._run_number_) 
        {
          if (_event_number_ < a_id._event_number_) return true;          
        }
      return false;      
    }

    bool
    event_id::operator> (const event_id & a_id) const
    {
      if (_run_number_ > a_id._run_number_) return true;
      if (_run_number_ == a_id._run_number_) 
        {
          if (_event_number_ > a_id._event_number_) return true;          
        }
      return false;      
    }

    std::string 
    event_id::to_string () const
    {
      std::ostringstream out;
      out << *this;
      return out.str ();
    }
  
    void 
    event_id::from_string (const std::string & a_word)
    {
      event_id id;
      std::istringstream in (a_word);
      in >> id;
      if (! in) 
        {
          throw std::runtime_error ("event_id::from_string: format error!");
        }
      *this = id;
    }

    std::ostream & 
    operator<< (std::ostream & a_out , 
                const event_id & a_id)
    {
      a_out << a_id.get_run_number () << event_id::IO_FORMAT_SEP 
            << a_id.get_event_number ();
      return a_out;
    }
    
    std::istream & 
    operator>> (std::istream & a_in,
                event_id & a_id)
    {
      int r, e;
      char c = 0;
      a_in >> r;
      if (! a_in) return a_in;
      a_in >> c;
      if (! a_in) return a_in;
      if (c != event_id::IO_FORMAT_SEP) 
        {
          a_in.setstate (std::ios_base::failbit);
          return a_in;
        }
      a_in >> e;
      if (a_in) 
        {
          a_id.set (r, e);
        }
      return a_in;
    }

    void 
    event_id::tree_dump (std::ostream & a_out, 
                         const std::string & a_title,
                         const std::string & a_indent,
                         bool a_inherit) const
    {
      namespace du = datatools::utils;
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ()) 
        {
          a_out << indent << a_title << std::endl;
        }
      a_out << indent << du::i_tree_dumpable::tag 
            << "Run number   : " << _run_number_ << std::endl;
      a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)
            << "Event number : " << _event_number_ << std::endl;
      return;
    }
      
    void event_id::dump () const
    {
      tree_dump (std::clog);
      return;
    }

  } // end of namespace event 

} // end of namespace datatools 

// end of event_id.cc
