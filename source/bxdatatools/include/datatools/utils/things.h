/* things.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-03-08
 * Last modified : 2011-06-29
 * 
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 * 
 * Description: 
 *
 *   A serializable container for arbitrary serializable objects
 * 
 *   Storable objects must :
 *   - have a default constructor
 *   - inherit from the datatools::serialization::i_serializable interface
 *   - have a 'serialize' template method (ala Boost)
 *  
 * History: 
 * 
 */

#ifndef __datatools__utils__things_h
#define __datatools__utils__things_h 1

#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#include <stdexcept>
#include <map>
#include <vector>
#include <typeinfo>

#include <boost/cstdint.hpp>
#include <boost/utility.hpp>
#include <boost/serialization/access.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/serialization/i_serializable.h>

#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/i_clear.h>

//! \file datatools/utils/things.h

namespace datatools {
  
  namespace utils {

    //using   namespace std;

    /* class bad_things_cast */
    
    //! \brief An exception for invalid cast operation within the \class things class.
    class bad_things_cast : public std::exception
    {
    private:
      
      std::string _message_;
      
    public:
      
      explicit bad_things_cast (const std::string & msg_);
      
      virtual ~bad_things_cast () throw ();
      
      virtual const char * what () const throw ();
      
    };

    /* class things */

    //! \brief A generic serializable and noncopyable container for arbitrary serializable objects.
    class things : public datatools::serialization::i_serializable,
                   public datatools::utils::i_tree_dumpable,
                   public datatools::utils::i_clear,
                   private boost::noncopyable
    {
    public:

      static const bool constant     = true;
      static const bool non_constant = ! constant;
      static const bool copyable     = true;
      static const bool noncopyable  = ! copyable;

      //! \brief Internal entry for serializable object storing in the \class thing class.
      struct entry_t : public datatools::utils::i_tree_dumpable
      {
        static const char MASK_CONST = 0x1;

      private:

        friend class boost::serialization::access; 
        BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

      public:

        entry_t ();

        bool is_not_const () const;

        bool is_const () const;

        void set_const (bool a_const = true);

        void set_description (const std::string &);
        
        const std::string & get_description () const;

        bool has_description () const;

        virtual void tree_dump (std::ostream & a_out         = std::clog, 
                                const std::string & a_title  = "",
                                const std::string & a_indent = "",
                                bool a_inherit               = false) const;

      public:

        std::string                                     description;
        uint8_t                                    flags;
        datatools::serialization::i_serializable * handle;

      };

      typedef std::map<std::string, entry_t> dict_t;

      DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()

    public: 
 
      const std::string & get_name () const;

      things & set_name (const std::string &);
 
      const std::string & get_description () const;

      things & set_description (const std::string &);

      // ctor:
      things ();
      
      // ctor:
      things (const std::string & a_name, const std::string & a_description = "");
      
      // dtor:
      virtual ~things ();
      
    private:
    
      things (const things &);

      things & operator= (const things &);
  
    public: 

      void reset ();

      virtual void clear ();

      size_t size () const;

      bool empty () const;
      
      bool has (const std::string & a_name) const;

      bool has_serial_tag (const std::string & a_name, const std::string & a_serial_tag) const;
      
      bool is_constant (const std::string & a_name) const;
      
      bool is_mutable (const std::string & a_name) const;
      
      //! not implemented :
      void set_constant (const std::string & a_name, bool a_const = true);
      
      const std::string & get_description (const std::string & a_name) const;
      
      void set_description (const std::string & a_name, const std::string & a_desc);

      void get_names (std::vector<std::string> & the_names) const;

    private:

      void _add_ (const std::string & a_name, 
                  datatools::serialization::i_serializable * a_obj,
                  const std::string & a_desc = "",
                  bool a_const = false);

    public:

      template<class T>
      T & add (const std::string & a_name, 
               const std::string & a_desc = "", 
               bool a_const = false)
      {
        T * new_obj = 0;
        new_obj = new T;
        if (new_obj == 0)
          {
            std::ostringstream message;
            message << "datatools::utils::things::add<T>: " 
                    << "Cannot allocate object '" << a_name << "' !";
            throw std::logic_error (message.str ());
          }
        datatools::serialization::i_serializable * new_cast 
          = dynamic_cast<datatools::serialization::i_serializable *> (new_obj); 
        if (new_cast == 0)
          {
            if (new_obj != 0) 
              {
                delete new_obj;
              }
            const std::type_info & ti = typeid (T);
            std::ostringstream message;
            message << "datatools::utils::things::add<T>: Request type '" 
                    << ti.name () 
                    << "' does not inherit from the '" 
                    << "datatools::serialization::i_serializable"  
                    << "' base class !";
            throw bad_things_cast (message.str ());
          }
        this->_add_ (a_name, new_cast, a_desc, a_const);
        return *new_obj;
      }

      void remove (const std::string & a_name);

      void erase (const std::string & a_name);

      template<class T>
      T * pop (const std::string & a_name)
      {
        dict_t::iterator found = _things_.find (a_name);
        if (found == _things_.end ())
          {
            std::ostringstream message;
            message << "datatools::utils::things::pop: No stored object has name '" 
                    << a_name << "' !";
            throw std::logic_error (message.str ());
            return 0;
          }
        const std::type_info & ti = typeid(T);
        const std::type_info & tf = typeid(*(found->second.handle));
        if (ti != tf)
          {
            T tmp;
            std::ostringstream message;
            message << "datatools::utils::things::pop: Request type '" << ti.name () 
                    << "' ('" << tmp.get_serial_tag () 
                    << "') does not match the type '" << tf.name () 
                    << "' of the stored object named '" << a_name 
                    << "' ('" << found->second.handle->get_serial_tag () << "') !";
            throw bad_things_cast (message.str ());
          }
        T * ptr = dynamic_cast<T *>(found->second.handle);
        found->second.handle = 0;
        _things_.erase (found);
        return ptr;
      }

      template<class T>
      T & grab (const std::string & a_name)
      {
        dict_t::iterator found = _things_.find (a_name);
        if (found == _things_.end ())
          {
            std::ostringstream message;
            message << "datatools::utils::things::grab: No stored object has name '" 
                    << a_name << "' !";
            throw std::logic_error (message.str ());
          }
        const std::type_info & ti = typeid(T);
        const std::type_info & tf = typeid(*(found->second.handle));
        if (ti != tf)
          {
            T tmp;
            std::ostringstream message;
            message << "datatools::utils::things::grab: Request type '" << ti.name () 
                    << "' ('" << tmp.get_serial_tag () 
                    << "') does not match the type '" << tf.name () 
                    << "' of the stored object named '" << a_name 
                    << "' ('" << found->second.handle->get_serial_tag () << "') !";
            throw bad_things_cast (message.str ());
          }
        if (found->second.is_const ())
          {
            std::ostringstream message;
            message << "datatools::utils::things::grab: Object named '" 
                    << a_name << "' is constant !";
            throw std::logic_error (message.str ());
          }
        return *(dynamic_cast<T *>(found->second.handle));
      } 

      template<class T>
      bool is_a (const std::string & a_name) const
      {
        dict_t::const_iterator found = _things_.find (a_name);
        if (found == _things_.end ())
          {
            std::ostringstream message;
            message << "datatools::utils::things::is_a: No object named '" << a_name << "' !";
            throw std::logic_error (message.str ());
          }
        const std::type_info & ti = typeid(T);
        const std::type_info & tf = typeid(*found->second.handle);
        return (ti == tf); 
      }

      template<class T>
      const T & get (const std::string & a_name) const
      {
        dict_t::const_iterator found = _things_.find (a_name);
        if (found == _things_.end ())
          {
            std::ostringstream message;
            message << "datatools::utils::things::get: No object named '" << a_name << "' !";
            throw std::logic_error (message.str ());
          }
        const std::type_info & ti = typeid(T);
        const std::type_info & tf = typeid(*found->second.handle);
        if (ti != tf)
          {
            T tmp;
            std::ostringstream message;
            message << "datatools::utils::things::get: Request type '" << ti.name () 
                    << "' ('" << tmp.get_serial_tag () 
                    << "') does not match the type '" << tf.name () 
                    << "' of the stored object named '" << a_name 
                    << "' ('" << found->second.handle->get_serial_tag () << "') !";
            throw bad_things_cast (message.str ());
          }
        return *(dynamic_cast<const T *>(found->second.handle));
      } 

      virtual void tree_dump (std::ostream & a_out         = std::clog, 
                              const std::string & a_title  = "",
                              const std::string & a_indent = "",
                              bool a_inherit               = false) const;

      void dump (std::ostream & a_out = std::clog) const;

    private:

      // serialization :
      friend class boost::serialization::access;
      BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

    private:

      // Attributes : 
      std::string _name_;        //!< The name of the container
      std::string _description_; //!< The description of the container
      dict_t      _things_;      //!< The internal dictionary of objects

    };

  }  // end of namespace utils

}  // end of namespace datatools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(datatools::utils::things, "datatools::utils::things")

#endif // __datatools__utils__things_h

// end of things.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
