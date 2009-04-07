/* i_serializable.h */

#ifndef __i_serializable_h
#define __i_serializable_h 1

namespace io_factory {

  
    
    class i_serializable
      {
      public:
	virtual const std::string & get_serial_tag() const = 0;
      };
    
   

} // end of namespace io_factory 

#endif // __i_serializable_h

/* end of i_serializable.h */
