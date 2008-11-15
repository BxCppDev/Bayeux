// -*- mode: c++; -*- 
/* event_id.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-02-15
 * Last modified: 2008-02-15
 * 
 * License: 
 * 
 * 
 * Description: 
 *  A generic event id with run number and event number
 *
 * History: 
 * 
 */

#ifndef __datatools__event__event_id_h
#define __datatools__event__event_id_h 1

#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include <boost/cstdint.hpp>

#include <datatools/serialization/i_serializable.h>
#include <datatools/serialization/serialization.h>
//#include <boost/serialization/access.hpp>
//#include <boost/serialization/nvp.hpp>

#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/i_clear.h>

namespace datatools {

  namespace event {

    class event_id : public datatools::utils::i_tree_dumpable ,
		     public datatools::utils::i_clear,
		     public datatools::serialization::i_serializable     
 
    {

    public:

      static const std::string SERIAL_TAG;
      static const int  INVALID_RUN_NUMBER   = -1;
      static const int  INVALID_EVENT_NUMBER = -1;
      static const char IO_FORMAT_SEP = '_';

    private:

      int32_t __run_number;
      int32_t __event_number;

    public:

      virtual void
      clear();

      int
      get_run_number() const;

      int
      get_event_number() const;

      void
      set_run_number(int);

      void
      set_event_number(int);

      void
      set(int, int);

      // ctor
      event_id();

      event_id(int);

      event_id(int, int);

      // dtor
      virtual ~event_id();

      bool
      is_valid() const;

      bool
      has (int, int) const;

      bool
      operator== (const event_id & id_) const;

      bool
      operator< (const event_id & id_) const;

      bool
      operator> (const event_id & id_) const;


      std::string
      to_string() const;

      void
      from_string(const std::string &);

      friend std::ostream &
      operator<<(std::ostream &, const event_id &);

      friend std::istream &
      operator>>(std::istream &, event_id &);

      virtual void
      tree_dump(std::ostream & out_         = std::cerr , 
		const std::string & title_  = "" ,
		const std::string & indent_ = "",
		bool inherit_               = false) const;

      virtual const std::string &
      get_serial_tag() const;

    private:

      friend class boost::serialization::access; 
      template<class Archive>
      void
      serialize(Archive            & ar_ , 
		const unsigned int   version_)
      {
	ar_ & boost::serialization::make_nvp("run_number",__run_number);
	ar_ & boost::serialization::make_nvp("event_number",__event_number);
      }
    };

    class event_id_equal_predicate : public std::unary_function<event_id, bool>
    {
      event_id __id;
      
    public:
      
      event_id_equal_predicate (const event_id & id_)
      {
	__id = id_;
      }
      
      bool operator() (const event_id & id_) const
      {
	return __id == id_;
      }
      
    };

  } // end of namespace event 

} // end of namespace datatools 

#endif // __datatools__event__event_id_h

// end of event_id.h
