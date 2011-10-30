// -*- mode: c++; -*- 
/* safe_serial.h */

/** 
 *
 * safe_serial is a template class useful to record
 * a list of instances of some type in a safe way
 * for serialization. We can access one instance at a time
 * (the current instance) but previous instances are accumulated in
 * some internal container, being never erased.
 *
 * \code
 * datatools::serialization::safe_serial<data_t> safe;
 * 
 * safe.make(); // create a new instance and make it the current one
 * safe.get();  // access to the current instance
 * \endcode
 */

#ifndef __datatools__serialization__safe_serial_h
#define __datatools__serialization__safe_serial_h 1

#include <list>
#include <stdexcept>

namespace datatools {

  namespace serialization {
    
    template<class T>
    class safe_serial 
    {
    public:

      typedef T data_type;

    private:

      typename std::list<T>                   _data_;
      typename std::list<T>::reverse_iterator _last_;

    public:

      void clear ()
      {
        _data_.clear ();
        _last_ = _data_.rend ();
        return;
      }

      safe_serial ()  : _data_ ()
      {
        _last_ = _data_.rend ();
        return;
      }

      virtual ~safe_serial ()
      {
        clear ();
        return;
      }

      bool empty () const
      {
        return _data_.size () == 0;
      }
      
      data_type & get ()
      {
        if (empty ()) 
          {
            throw std::logic_error ("safe_serial::get: No data!");
          }
        return *_last_;
      }

      void make ()
      {
        try
          {
            _data_.push_back (data_type ());
          }
        catch (...)
          {
            throw std::logic_error ("safe_serial::make: "
                                    "Cannot insert new data element!");
          }
        _last_ = _data_.rbegin ();
        return;
      }

      void set (const data_type & data_)
      {
        make ();
        get () = data_;
        return;
      }

    };
    
  } // end of namespace serialization 

} // end of namespace datatools 

#endif // __datatools__serialization__safe_serial_h

/* end of safe_serial.h */
