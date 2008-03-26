/* i_serializable.h */

#ifndef __datatools__serialization__i_serializable_h
#define __datatools__serialization__i_serializable_h 1

namespace datatools {

  namespace serialization {
    
    class i_serializable
      {
      public:
	virtual const std::string & get_serial_tag() const = 0;
      };
    
  } // end of namespace serialization 

} // end of namespace datatools 

#endif // __datatools__serialization__i_serializable_h

/* end of i_serializable.h */
