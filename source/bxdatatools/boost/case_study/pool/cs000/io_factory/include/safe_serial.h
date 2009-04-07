/* safe_serial.h */

/*
 *
 * safe_serial is a template class useful to record
 * a list of instances of some type in a safe way
 * for serialization. We can access one instance at a time
 * (the current instance).
 * 
 * safe_serial<data_t> safe;
 *
 * safe.make(); // create a new instance and make it the current one
 * safe.get();  // access to the current instance
 * 
 */

#ifndef __safe_serial_h
#define __safe_serial_h 1

#include <list>
#include <stdexcept>

namespace io_factory {

  
    
    template<class T>
      class safe_serial 
      {
      public:
	typedef T                    data_type;
	//typedef std::list<data_type> data_col;
	//typedef data_col::reverse_iterator data_col_reverse_iterator;

      private:
	typename std::list<T>                   __data;
	typename std::list<T>::reverse_iterator __last;
	//typename data_col                   __data;
	//typename data_col::reverse_iterator __last;
	//std::list<data_type>::iterator __last;
	//data_col                  __data;
	//data_col_reverse_iterator __last;

      public:

	void 
	clear()
	{
	  __data.clear();
	  __last = __data.rend();
	}

	safe_serial()  : __data()
	{
	  __last = __data.rend();
	}

	virtual ~safe_serial()
	{
	  clear();
	}

	bool 
	empty() const
	{
	  /*
          std::cerr << "DEBUG: safe_serial::empty: size=" 
	            << __data.size() << std::endl;
	  */
	  return __data.size() == 0;
	}
      
	data_type & 
	get()
	{
	  if (empty()) 
	    {
	      throw std::runtime_error("safe_serial::get: No data!");
	    }
	  //data_col::reverse_iterator last = __data.rbegin();
	  //typename std::list<data_type>::reverse_iterator last = __data.rbegin();
	  return *__last;
	}

	void 
	make()
	{
	  //std::cerr << "DEBUG: safe_serial::make: entering..." << std::endl;
	  //if (!empty()) __last->clear(); 
	  // problem with object
	  // tracking in boost::serialization
	  try
	    {
	      __data.push_back(data_type());
	      /*
	      std::cerr << "DEBUG: safe_serial::make: push_back ok, size=" 
			<< __data.size() << std::endl;
	      */
	    }
	  catch(...)
	    {
	      throw std::runtime_error("safe_serial::make: "
				       "Cannot insert new data element!");
	    }
	  __last = __data.rbegin();
	  /*
	  std::cerr << "DEBUG: safe_serial::make: size=" 
			<< __data.size() << std::endl;
	  std::cerr << "DEBUG: safe_serial::make: exiting." 
	            << std::endl;
	  */
	}

	void 
	set(const data_type & data_)
	{
	  make();
	  get() = data_;
	}

      };
    
   

} // end of namespace io_factory 

#endif // __safe_serial_h

/* end of safe_serial.h */
