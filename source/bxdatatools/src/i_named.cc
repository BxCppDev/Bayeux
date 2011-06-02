/* i_named.cc */

#include <datatools/utils/i_named.h>

namespace datatools {

  namespace utils {

    using namespace std;

    const string i_named::ANONYMOUS_LABEL = "__anonymous__";

    bool i_named::has_a_name (const i_named & a_named)
    {
      if (const_cast<i_named &> (a_named).get_name ().empty ()) return false;
      if (const_cast<i_named &> (a_named).get_name () == ANONYMOUS_LABEL) return false;
      return true;
    }
    
    bool i_named::is_anonymous (const i_named & a_named)
    {
      return const_cast<i_named &> (a_named).get_name () == ANONYMOUS_LABEL;
    }

  } // end of namespace utils 

} // end of namespace datatools 

/* end of i_named.cc */
