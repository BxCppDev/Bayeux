/* i_tree_dump.h */

#ifndef __datatools__utils__i_tree_dump_h
#define __datatools__utils__i_tree_dump_h 1

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

namespace datatools {

  namespace utils {

    struct i_tree_dumpable
    {
      static const std::string TAG;
      static const std::string LAST_TAG;
      static const std::string SKIP_TAG;
      static const std::string LAST_SKIP_TAG;
  
      static std::ostream & last_skip_tag( std::ostream & out_ );
  
      static std::ostream & skip_tag( std::ostream & out_ );

      static std::ostream & last_tag( std::ostream & out_ );

      static std::ostream & tag( std::ostream & out_ );

      class inherit_tag 
      {
	bool __inherit;
      public:
    
	inherit_tag( bool inherit_ ); 

	friend std::ostream & operator<<( std::ostream & out_ , 
					  const inherit_tag & lt_ );
      };

      class inherit_skip_tag 
      {
	bool __inherit;
      public:
    
	inherit_skip_tag( bool inherit_ ); 

	friend std::ostream & operator<<( std::ostream & out_ , 
					  const inherit_skip_tag & lt_ );
      };

      virtual void tree_dump( std::ostream & out_         = std::cerr , 
			      const std::string & title_  = "" ,
			      const std::string & indent_ = "",
			      bool inherit_               = false ) const = 0;
    };

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__i_tree_dump_h

/* end of i_tree_dump.h */
