// -*- mode: c++; -*- 
//! \file datatools/event/basic_event.h 
/* basic_event.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-02-25
 * Last modified: 2008-02-25
 * 
 * License: 
 * 
 * 
 * Description: 
 *   A generic basic event model
 *
 * History: 
 * 
 */

#ifndef __datatools__event__basic_event_h
#define __datatools__event__basic_event_h 1

// standard includes:
#include <iostream>
#include <string>


#include <datatools/serialization/i_serializable.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/i_clear.h>
#include <datatools/utils/properties.h>
#include <datatools/event/event_id.h>

namespace datatools {

  namespace event {
    
    /*! \brief A simple basic event model.
     *
     *  The basic_event class is serializable through the Boost.Serialization library.
     *  It owns an event_id instance and a properties instance.
     *
     *  Sample program: #programs/test_basic_event.cxx 
     *
     */
    class basic_event : public datatools::utils::i_tree_dumpable,
                        public datatools::utils::i_clear,
			public datatools::serialization::i_serializable     

    {
    public:
      //! Serialization tag.
      static const std::string SERIAL_TAG;

    private:
      //! Event ID.
      event_id                     __id;
      //! Dictionary of properties.
      datatools::utils::properties __properties;

    public: 

      //! Reset method (from the datatools::utils::i_clear interface).
      virtual void clear ();

      //! Set the event ID.
      void id (const event_id & id_);

      //! Set the event ID.
      void set_id (const event_id & id_)
      {
	id  (id_);
      }

      //! Get the event ID (read-only).
      const event_id & id () const;

      //! Get the event ID (read-only).
      const event_id & get_id () const
      {
	return id ();
      }

      //! Get the event ID (left value).
      event_id & id ();

      //! Get the properties (read-only)..
      const datatools::utils::properties & properties () const;

      const datatools::utils::properties & get_properties_ro () const
      {
	return properties ();
      }

      //! Get the properties (left value).
      datatools::utils::properties & properties ();

      datatools::utils::properties & get_properties_rw ()
      {
	return properties ();
      }

    public: 

      //! Default constructor.
      basic_event ();
      
      //! Constructor on event ID.
      basic_event (const event_id & id_);
      
      //! Destructor.
      virtual ~basic_event ();
      
      //! Method for smart printing (from the datatools::utils::i_tree_dump interface).
      virtual void tree_dump (std::ostream & out_         = std::cerr, 
			      const std::string & title_  = "",
			      const std::string & indent_ = "",
			      bool inherit_               = false) const;
      
      //! Shortcut to tree_dump.
      void dump () const
      {
	tree_dump (std::clog);
      }

      //! Return the serialization tag (from the datatools::serialization::i_serializable interface).
      virtual const std::string & get_serial_tag () const;

    private:

      //! Boost.Serialization hook.
      friend class boost::serialization::access; 

      //! Templatized serialization method for the Boost.Serialization library.
      template<class Archive>
      void serialize (Archive            & ar_, 
		      const unsigned int   version_)
      {
	ar_ & boost::serialization::make_nvp ("id",__id);
	ar_ & boost::serialization::make_nvp ("properties",__properties);
      }
      
    };

  } // end of namespace event 

} // end of namespace datatools

#endif // __datatools__event__basic_event_h

// end of basic_event.h
