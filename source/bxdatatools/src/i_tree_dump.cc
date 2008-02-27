/* i_tree_dump.cc */

#include <datatools/utils/i_tree_dump.h>

namespace datatools {

  namespace utils {

    const std::string i_tree_dumpable::TAG           = "|-- ";
    const std::string i_tree_dumpable::LAST_TAG      = "`-- ";
    const std::string i_tree_dumpable::SKIP_TAG      = "|   ";
    const std::string i_tree_dumpable::LAST_SKIP_TAG = "    ";

    std::ostream & i_tree_dumpable::last_skip_tag( std::ostream & out_ )
    {
      out_ << LAST_SKIP_TAG;
      return out_;
    }
  
    std::ostream & i_tree_dumpable::skip_tag( std::ostream & out_ )
    {
      out_ << SKIP_TAG;
      return out_;
    }

    std::ostream & i_tree_dumpable::last_tag( std::ostream & out_ )
    {
      out_ << LAST_TAG;
      return out_;
    }

    std::ostream & i_tree_dumpable::tag( std::ostream & out_ )
    {
      out_ << TAG;
      return out_;
    }

    i_tree_dumpable::inherit_tag::inherit_tag( bool inherit_ ) 
      : __inherit(inherit_) 
    {
    }
  
    std::ostream & operator<<( std::ostream & out_, 
			       const i_tree_dumpable::inherit_tag & lt_ ) 
    {
      if ( lt_.__inherit ) out_ << i_tree_dumpable::tag;
      else out_ << i_tree_dumpable::last_tag;
      return out_;
    }
  
    i_tree_dumpable::inherit_skip_tag::inherit_skip_tag( bool inherit_ ) 
      : __inherit(inherit_) 
    {
    }
  
    std::ostream & operator<<( std::ostream & out_, 
			       const i_tree_dumpable::inherit_skip_tag & lt_ ) 
    {
      if ( lt_.__inherit ) out_ << i_tree_dumpable::skip_tag;
      else out_ << i_tree_dumpable::last_skip_tag;
      return out_;
    }
  

  } // end of namespace utils 

} // end of namespace datatools 

/* end of i_tree_dump.cc */
