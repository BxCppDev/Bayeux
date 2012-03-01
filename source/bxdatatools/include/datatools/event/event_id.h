// -*- mode: c++; -*- 
//! \file datatools/event/event_id.h
/* event_id.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/i_clear.h>

namespace datatools {

  namespace event {

    /*! \brief A simple \i event \i identifier based on a run number 
     *        and an event number. Run and event numbers are 
     *        represented by 32 bits signed integrals.
     *
     *  The event_id class is serializable through the 
     *  Boost.Serialization library.
     *  Utilities for standard I/O streams is also provided.
     *
     *
     *  Example :
     * \code
     * #include <datatools/event/event_id.h>
     * #include <iostream>
     *
     * int main (void)
     * {
     *   datatools::event::event_id eid;
     *   eid.set (45, 37);
     *   eid.tree_dump (std::cout, "The event ID :", ">>> ");
     *   return 0;
     * }
     * \endcode
     * 
     */
    class event_id : public datatools::serialization::i_serializable ,
		     public datatools::utils::i_tree_dumpable ,
		     public datatools::utils::i_clear
    {

    public:

      //! Constant value for an invalid run number.
      static const int  INVALID_RUN_NUMBER   = -1;

      //! Constant value for an invalid event number.
      static const int  INVALID_EVENT_NUMBER = -1;

      //! Format separator character used in the to_string and from_string methods.
      static const char IO_FORMAT_SEP = '_';

    public:

      //! Reset the ID to the invalid internal value (from the datatools::utils::i_clear interface).
      virtual void clear ();

      //! Returns the run number.
      /** @returns the run number.
       */
      int get_run_number () const;

      //! Returns the event number.
      /** @returns the event number.
       */
      int get_event_number () const;

      //! Set the run number.
      /** @param r_ the run number.
       */
      void set_run_number (int r_);

      //! Set the event number.
      /** @param e_ the event number.
       */
      void set_event_number (int e_);

      //! Method to set the run and event numbers.
      /** @param r_ the run number.
       *  @param e_ the event number.
       */
      void set (int r_, int e_);

      //! The default constructor.
      /**  Set internal state is initialized to the invalid value.
       */
      event_id ();

      //! A constructor that sets only the event number.
      event_id (int e_);

      //! A constructor to fully set the event_id object.
       /** @param r_ the run number.
       *   @param e_ the event number.
       */
      event_id (int r_, int e_);

      //! The destructor.
      virtual ~event_id ();

      //! Check whether the ID is valid.
      bool is_valid () const;

      //! Check if the event_id object has specific run and event numbers.
       /** @param r_ the run number.
       * @param e_ the event number.
       * @return true or false
       */
      bool has (int r_, int e_) const;

      //! Comparison operator.
      bool operator== (const event_id & id_) const;

      //! Comparison operator.
      bool operator< (const event_id & id_) const;

      //! Comparison operator.
      bool operator> (const event_id & id_) const;

      //! Output convertor.
      std::string to_string () const;

      //! Input convertor.
      void from_string (const std::string &);

      //! Output operator.
      friend std::ostream & operator<< (std::ostream &, const event_id &);

      //! Input operator.
      friend std::istream & operator>> (std::istream &, event_id &);

      //! Method for smart printing (from the datatools::utils::i_tree_dump interface).
      /** @param out_ the output stream
       *  @param title_ the title to be displayed
       *  @param indent_ the indentation string
       *  @param inherit_ the inheritance flag.
       */
      virtual void tree_dump (std::ostream & out_         = std::cerr , 
			      const std::string & title_  = "" ,
			      const std::string & indent_ = "",
			      bool inherit_               = false) const;

      //! Shortcut to the tree_dump method with default arguments.
      void dump () const;

      //! Serialization tag.
      DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()

    private:

      int32_t _run_number_;   //!< The number of the run.
      int32_t _event_number_; //!< The number of the event within the run.

    private:

      //! Boost.Serialization hook.
      friend class boost::serialization::access; 
      BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

    };

    //! Predicate to check if an event_id equals another one.
    /** May be used with the \b std::find_if standard algorithm.
     */
    class event_id_equal_predicate : public std::unary_function<event_id, bool>
    {
      event_id _id_;
      
    public:
      
      //! Constructor 
      /** @param id_ the reference event ID.
       */
      event_id_equal_predicate (const event_id & id_)
      {
	_id_ = id_;
	return;
      }
      
      //! Functionnal operator 
      /** @param id_ the event ID to be checked to the embedded reference ID.
       */
      bool operator () (const event_id & id_) const
      {
	return _id_ == id_;
      }
      
    };

  } // end of namespace event 

} // end of namespace datatools 

#endif // __datatools__event__event_id_h

// end of event_id.h
