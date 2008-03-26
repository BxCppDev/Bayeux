// -*- mode: c++; -*- 
/* event_id.cc
 */

#include <datatools/event/event_id.h>

namespace datatools {

  namespace event {

    const std::string event_id::SERIAL_TAG = "__EVENT_ID__";

    const std::string & 
    event_id::get_serial_tag() const
    {
      return  event_id::SERIAL_TAG;
    }

    void 
    event_id::clear()
    {
      set_run_number(INVALID_RUN_NUMBER);
      set_event_number(INVALID_EVENT_NUMBER);
    }

    int 
    event_id::get_run_number() const
    {
      return __run_number;
    }

    int 
    event_id::get_event_number() const
    {
      return __event_number;
    }

    void 
    event_id::set_run_number(int run_number_)
    {
      __run_number = (run_number_<0)?INVALID_RUN_NUMBER:run_number_ ;    
    }

    void 
    event_id::set_event_number(int event_number_)
    {
      __event_number = (event_number_<0)?INVALID_EVENT_NUMBER:event_number_;
    }

    void 
    event_id::set(int run_number_, 
		  int event_number_)
    {
      set_run_number(run_number_);
      set_event_number(event_number_);
    }

    event_id::event_id()
    {
      __run_number   = INVALID_RUN_NUMBER;
      __event_number = INVALID_EVENT_NUMBER;
    }

    event_id::event_id( int event_number_ )
    {
      __run_number = INVALID_RUN_NUMBER;
      set_event_number(event_number_);
    }

    event_id::event_id(int run_number_ , 
		       int event_number_ )
    {
      set(run_number_,event_number_);
    }
  
    event_id::~event_id()
    {
    }

    bool 
    event_id::is_valid() const
    {
      return (__run_number!=INVALID_RUN_NUMBER) 
	&& (__event_number!=INVALID_EVENT_NUMBER);
    }

    std::string 
    event_id::to_string() const
    {
      std::ostringstream out;
      out << *this;
      return out.str();
    }
  
    void 
    event_id::from_string(const std::string & str_)
    {
      event_id id;
      std::istringstream in(str_);
      in >> id;
      if ( !in ) {
	throw std::runtime_error("event_id::from_string: format error!");
      }
      *this=id;
    }

    std::ostream & 
    operator<<(std::ostream & out_ , 
	       const event_id & id_)
    {
      out_ << id_.get_run_number() << event_id::IO_FORMAT_SEP 
	   << id_.get_event_number();
      return out_;
    }

    std::istream & 
    operator>>(std::istream & in_,
	       event_id & id_)
    {
      int r,e;
      char c=0;
      in_ >> r;
      if ( !in_ ) return in_;
      in_ >> c;
      if ( !in_ ) return in_;
      if ( c !=  event_id::IO_FORMAT_SEP ) {
	in_.setstate(std::ios_base::failbit);
	return in_;
      }
      in_ >> e;
      if ( in_ ) {
	id_.set(r,e);
      }
      return in_;
    }

    void 
    event_id::tree_dump(std::ostream & out_         , 
			const std::string & title_  ,
			const std::string & indent_ ,
			bool inherit_                ) const
    {
      namespace du = datatools::utils;
      std::string indent;
      if ( !indent_.empty() ) indent = indent_;
      if ( !title_.empty() ) 
	{
	  out_ << indent << title_ << std::endl;
	}
      out_ << indent << du::i_tree_dumpable::tag 
	   << "Run number   : " <<  __run_number << std::endl;
      out_ << indent << du::i_tree_dumpable::inherit_tag(inherit_)
	   << "Event number : " <<  __event_number << std::endl;
    }

  } // end of namespace event 

} // end of namespace datatools 

// end of event_id.cc
