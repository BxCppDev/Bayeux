/* i_tree_dump.cc */

#include <datatools/utils/i_tree_dump.h>
#include <iomanip>
#include <sstream>

namespace datatools {

  namespace utils {

    const std::string i_tree_dumpable::TAG           = "|-- ";
    const std::string i_tree_dumpable::LAST_TAG      = "`-- ";
    const std::string i_tree_dumpable::SKIP_TAG      = "|   ";
    const std::string i_tree_dumpable::LAST_SKIP_TAG = "    ";

    std::ostream & i_tree_dumpable::last_skip_tag (std::ostream & a_out)
    {
      a_out << LAST_SKIP_TAG;
      return a_out;
    }
  
    std::ostream & i_tree_dumpable::skip_tag (std::ostream & a_out)
    {
      a_out << SKIP_TAG;
      return a_out;
    }

    std::ostream & i_tree_dumpable::last_tag (std::ostream & a_out)
    {
      a_out << LAST_TAG;
      return a_out;
    }

    std::ostream & i_tree_dumpable::tag (std::ostream & a_out)
    {
      a_out << TAG;
      return a_out;
    }

    i_tree_dumpable::inherit_tag::inherit_tag (bool a_inherit) 
      : inherit_ (a_inherit) 
    {
    }
  
    std::ostream & operator<< (std::ostream & a_out, 
			       const i_tree_dumpable::inherit_tag & a_last_tag) 
    {
      if (a_last_tag.inherit_) a_out << i_tree_dumpable::tag;
      else a_out << i_tree_dumpable::last_tag;
      return a_out;
    }
  
    i_tree_dumpable::inherit_skip_tag::inherit_skip_tag (bool a_inherit) 
      : inherit_ (a_inherit) 
    {
    }
  
    std::ostream & operator<< (std::ostream & a_out, 
			       const i_tree_dumpable::inherit_skip_tag & a_last_tag) 
    {
      if (a_last_tag.inherit_) a_out << i_tree_dumpable::skip_tag;
      else a_out << i_tree_dumpable::last_skip_tag;
      return a_out;
    }
  

  } // end of namespace utils 

} // end of namespace datatools 

/* end of i_tree_dump.cc */
