/* i_tree_dump.h */

#ifndef __datatools__utils__i_tree_dump_h
#define __datatools__utils__i_tree_dump_h 1

#include <iostream>
#include <string>

namespace datatools {

  namespace utils {

    class i_tree_dumpable
    {
    public:

      static const std::string TAG;
      static const std::string LAST_TAG;
      static const std::string SKIP_TAG;
      static const std::string LAST_SKIP_TAG;
  
      static std::ostream & last_skip_tag (std::ostream & a_out);
  
      static std::ostream & skip_tag (std::ostream & a_out);

      static std::ostream & last_tag (std::ostream & a_out);

      static std::ostream & tag (std::ostream & a_out);

      class inherit_tag 
      {
	bool inherit_;

      public:
    
	inherit_tag (bool a_inherit); 

	friend std::ostream & operator<< (std::ostream & a_out, 
					  const inherit_tag & a_last_tag);
      };

      class inherit_skip_tag 
      {
	bool inherit_;

      public:
    
	inherit_skip_tag (bool a_inherit); 

	friend std::ostream & operator<< (std::ostream & a_out, 
					  const inherit_skip_tag & a_last_tag);
      };

      /// Main interface method for smart dump
      virtual void tree_dump (std::ostream & a_out         = std::clog, 
			      const std::string & a_title  = "",
			      const std::string & a_indent = "",
			      bool a_inherit               = false) const = 0;
    };

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__i_tree_dump_h

/* end of i_tree_dump.h */
