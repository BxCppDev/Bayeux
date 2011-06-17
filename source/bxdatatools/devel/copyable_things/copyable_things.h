/* copyable_things.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-03-08
 * Last modified : 2011-03-20
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

#ifndef __datatools__utils__copyable_things_h
#define __datatools__utils__copyable_things_h 1

#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#include <stdexcept>
#include <map>
#include <vector>
#include <typeinfo>

#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/serialization/i_serializable.h>

#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/i_clear.h>
#include <datatools/utils/i_cloneable.h>

namespace datatools {
	
  namespace utils {

		using namespace std;

		/*** class bad_copyable_things_cast ***/
		
		class bad_copyable_things_cast : public exception
		{
		private:
			
			string m_message;
			
		public:
			
			explicit bad_copyable_things_cast (const string & msg_);
			
			virtual ~bad_copyable_things_cast () throw ();
			
			virtual const char * what () const throw ();
			
		};

		/*** class copyable_things ***/

    class copyable_things : public datatools::serialization::i_serializable,
														public datatools::utils::i_cloneable,
														public datatools::utils::i_tree_dumpable,
														public datatools::utils::i_clear
    {

		public:

			static const bool constant     = true;
			static const bool non_constant = ! constant;
			static const bool copyable     = true;
			static const bool noncopyable = ! copyable;

			struct handle_t : public datatools::utils::i_cloneable, 
												public datatools::serialization::i_serializable
			{
				virtual ~handle_t () {}
			};

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

				string       description;
				uint8_t      flags;
				datatools::serialization::i_serializable * handle; // also cloneable;

			};

			//typedef map<string, datatools::serialization::i_serializable *> dict_t;
			typedef map<string, entry_t> dict_t;

			static const string SERIAL_TAG;
      
		public: 

			virtual const string & get_serial_tag () const;
 
			const string & get_name () const;

			copyable_things & set_name (const string &);
 
			const string & get_description () const;

			copyable_things & set_description (const string &);

			// ctor:
			copyable_things ();
      
			// ctor:
			copyable_things (const string & a_name, const string & a_description = "");
      
			// dtor:
			virtual ~copyable_things ();
    
			// ctor:
			copyable_things (const copyable_things &);

			copyable_things & operator= (const copyable_things &);

      //! From the datatools::utils::i_cloneable interface :
      DATATOOLS_CLONEABLE_DECLARATION (copyable_things)
  
		public: 

			void reset ();

			virtual void clear ();

			size_t size () const;

			bool empty () const;
			
			bool has (const string & a_name) const;
			
			bool is_constant (const string & a_name) const;
			
			//! not implemented :
			void set_constant (const string & a_name, bool a_const = true);
			
			const string & get_description (const string & a_name) const;
			
			void set_description (const string & a_name, const string & a_desc);

			void get_names (vector<string> & the_names) const;

		private:

			void __add (const string & a_name, 
									datatools::serialization::i_serializable * a_obj,
									const string & a_desc = "",
									bool a_const = false);

		public:

			template<class T>
			T & add (const string & a_name, 
							 const string & a_desc = "", 
							 bool a_const = false)
			{
				T * new_obj = 0;
				new_obj = new T;
				if (new_obj == 0)
					{
						ostringstream message;
						message << "datatools::utils::copyable_things::add<T>: " 
										<< "Cannot allocate object '" << a_name << "' !";
						throw runtime_error (message.str ());
					}
				datatools::serialization::i_serializable * new_cast 
					= dynamic_cast<datatools::serialization::i_serializable *> (new_obj); 
				if (new_cast == 0)
					{						
						if (new_obj != 0) 
							{
								delete new_obj;
							}
						const type_info & ti = typeid (T);
						T tmp;
						ostringstream message;
						message << "datatools::utils::copyable_things::add<T>: Request type '" << ti.name () 
										<< "' does not inherit from the '" 
										<< "datatools::serialization::i_serializable"  
										<< "' base class !";
						throw bad_copyable_things_cast (message.str ());
					}
				datatools::utils::i_cloneable * new_cast2 
					= dynamic_cast<datatools::utils::i_cloneable *> (new_obj); 
				if (new_cast2 == 0)
					{						
						if (new_obj != 0) 
							{
								delete new_obj;
							}
						const type_info & ti = typeid (T);
						T tmp;
						ostringstream message;
						message << "datatools::utils::copyable_things::add<T>: Request type '" << ti.name () 
										<< "' does not inherit from the '" 
										<< "datatools::utils::i_cloneable"  
										<< "' base class !";
						throw bad_copyable_things_cast (message.str ());
					}
				this->__add (a_name, new_cast, a_desc, a_const);
				return *new_obj;
			}

			void remove (const string & a_name);

			void erase (const string & a_name);

			template<class T>
			T * pop (const string & a_name)
			{
				dict_t::iterator found = m_copyable_things.find (a_name);
				if (found == m_copyable_things.end ())
					{
						ostringstream message;
						message << "datatools::utils::copyable_things::pop: No stored object has name '" << a_name << "' !";
						throw logic_error (message.str ());
						return 0;
					}
				const type_info & ti = typeid(T);
				const type_info & tf = typeid(*(found->second.handle));
				if (ti != tf)
					{
						T tmp;
						ostringstream message;
						message << "datatools::utils::copyable_things::pop: Request type '" << ti.name () 
										<< "' ('" << tmp.get_serial_tag () 
										<< "') does not match the type '" << tf.name () 
										<< "' of the stored object named '" << a_name 
										<< "' ('" << found->second.handle->get_serial_tag () << "') !";
						throw bad_copyable_things_cast (message.str ());
					}
				T * ptr = dynamic_cast<T *>(found->second.handle);
				found->second.handle = 0;
				m_copyable_things.erase (found);
				return ptr;
			}

			template<class T>
			T & grab (const string & a_name)
			{
				dict_t::iterator found = m_copyable_things.find (a_name);
				if (found == m_copyable_things.end ())
					{
						ostringstream message;
						message << "datatools::utils::copyable_things::grab: No stored object has name '" << a_name << "' !";
						throw logic_error (message.str ());
					}
				const type_info & ti = typeid(T);
				const type_info & tf = typeid(*(found->second.handle));
				if (ti != tf)
					{
						T tmp;
						ostringstream message;
						message << "datatools::utils::copyable_things::grab: Request type '" << ti.name () 
										<< "' ('" << tmp.get_serial_tag () 
										<< "') does not match the type '" << tf.name () 
										<< "' of the stored object named '" << a_name 
										<< "' ('" << found->second.handle->get_serial_tag () << "') !";
						throw bad_copyable_things_cast (message.str ());
					}
				if (found->second.is_const ())
					{
						ostringstream message;
						message << "datatools::utils::copyable_things::grab: Object named '" << a_name << "' is constant !";
						throw logic_error (message.str ());
					}
				return *(dynamic_cast<T *>(found->second.handle));
			} 

			template<class T>
			bool is_a (const string & a_name) const
			{
				dict_t::const_iterator found = m_copyable_things.find (a_name);
				if (found == m_copyable_things.end ())
					{
						ostringstream message;
						message << "datatools::utils::copyable_things::is_a: No object named '" << a_name << "' !";
						throw logic_error (message.str ());
					}
				const type_info & ti = typeid(T);
				const type_info & tf = typeid(*found->second.handle);
				return (ti == tf); 
			}

			template<class T>
			const T & get (const string & a_name) const
			{
				dict_t::const_iterator found = m_copyable_things.find (a_name);
				if (found == m_copyable_things.end ())
					{
						ostringstream message;
						message << "datatools::utils::copyable_things::get: No object named '" << a_name << "' !";
						throw logic_error (message.str ());
					}
				const type_info & ti = typeid(T);
				const type_info & tf = typeid(*found->second.handle);
				if (ti != tf)
					{
						T tmp;
						ostringstream message;
						message << "datatools::utils::copyable_things::get: Request type '" << ti.name () 
										<< "' ('" << tmp.get_serial_tag () 
										<< "') does not match the type '" << tf.name () 
										<< "' of the stored object named '" << a_name 
										<< "' ('" << found->second.handle->get_serial_tag () << "') !";
						throw bad_copyable_things_cast (message.str ());
					}
				return *(dynamic_cast<const T *>(found->second.handle));
			} 

			virtual void tree_dump (std::ostream & a_out         = std::clog, 
															const std::string & a_title  = "",
															const std::string & a_indent = "",
															bool a_inherit               = false) const;

			void dump (ostream & a_out = std::clog) const;

		private:

			// serialization :
			friend class boost::serialization::access;
			BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

		private:

			// Attributes : 
			string m_name;
			string m_description;
			dict_t m_copyable_things;

    };

  }  // end of namespace utils

}  // end of namespace datatools

#endif // __datatools__utils__copyable_things_h

// end of copyable_things.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
