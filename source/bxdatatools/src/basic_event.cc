// -*- mode: c++; -*- 
/* basic_event.cc
 */


#include <datatools/event/basic_event.h>

namespace datatools {
  
  namespace event {
    
    void basic_event::id( const event_id & id_ )
    {
      __id=id_;
    }
    
    const event_id & basic_event::id() const
    {
      return __id;
    }
    
    event_id & basic_event::id()
    {
      return __id;
    }
    
    const datatools::utils::properties & basic_event::properties() const
    {
      return __properties;
    }
    
    datatools::utils::properties & basic_event::properties()
    {
      return __properties;
    }

    void basic_event::clear()
    {
      __id.clear();
      __properties.clear();
    }
    
    basic_event::basic_event()
    {
    }
    
    basic_event::basic_event( const event_id & id_ ) : __id(id_)
    {
    }
    
    basic_event::~basic_event()
    {
      basic_event::clear();
    }

    void basic_event::tree_dump( std::ostream & out_         , 
				 const std::string & title_  ,
				 const std::string & indent_ ,
				 bool  inherit_               ) const
    {
      std::string indent;
      if ( !indent_.empty() ) indent = indent_;
      if ( !title_.empty() ) {
	out_ << indent << title_ << std::endl;
      }
      namespace du = datatools::utils;
      out_ << indent << du::i_tree_dumpable::tag 
	   << "Id : " << std::endl;
      {
	std::ostringstream indent_oss;
	indent_oss << indent;
	indent_oss << du::i_tree_dumpable::skip_tag;
	__id.tree_dump(out_,"",indent_oss.str());
      }

      out_ << indent << du::i_tree_dumpable::inherit_tag(inherit_)
	   << "Properties : ";
      if ( __properties.size() == 0 ) { out_ << "<empty>"; }
      out_ << std::endl;
      {
	std::ostringstream indent_oss;
	indent_oss << indent;
	indent_oss << du::i_tree_dumpable::inherit_skip_tag(inherit_);
	__properties.tree_dump(out_,"",indent_oss.str());
      }
    }
    
  } // end of namespace event 

} // end of namespace datatools

// end of basic_event.cc
