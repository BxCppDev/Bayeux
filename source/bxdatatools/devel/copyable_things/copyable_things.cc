/* copyable_things.cc
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
 */
 
#include <datatools/utils/copyable_things.h>

namespace datatools {

  namespace utils {
   
		using namespace std;

		bad_copyable_things_cast::bad_copyable_things_cast (const string & a_msg) 
			: std::exception (), m_message (a_msg)
		{
			return;
		}
		
		bad_copyable_things_cast::~bad_copyable_things_cast () throw ()
		{
			return;
		}
		
		const char * bad_copyable_things_cast::what () const throw ()
		{
			return m_message.c_str ();
		}
 
		/***************************************************/

		copyable_things::entry_t::entry_t ()
		{
			description = "";
			flags       = 0x0;
			handle      = 0;
			return;
		}

		bool copyable_things::entry_t::is_not_const () const
		{
      return (flags & MASK_CONST) == 0;
		}

		bool copyable_things::entry_t::is_const () const
		{
			return ! is_not_const ();
		}

		void copyable_things::entry_t::set_const (bool a_const)
		{
			if (a_const)
				{
					flags |= MASK_CONST;
				}
			else
				{
					flags &= ~MASK_CONST;
				}
 			return;
		}

		void copyable_things::entry_t::set_description (const std::string & a_desc)
		{
			description = a_desc;
			return;
		}
				
		const std::string & copyable_things::entry_t::get_description () const
		{
			return description;
		}

		bool copyable_things::entry_t::has_description () const
		{
			return ! description.empty ();
		}

    void copyable_things::entry_t::tree_dump (std::ostream & a_out, 
																		 const std::string & a_title,
																		 const std::string & a_indent,
																		 bool a_inherit) const
    {
      namespace du = datatools::utils;
      std::string indent;
      if (! a_indent.empty () ) indent = a_indent;
      if (! a_title.empty ()) 
				{
					a_out << indent << a_title << std::endl;
				}
			
      if (! description.empty ()) 
				{
					a_out << indent << du::i_tree_dumpable::tag 
							 << "Description  : '" <<  get_description () 
							 << "'" << std::endl;
				}

      a_out << indent << du::i_tree_dumpable::tag 
					 << "Const  : " << is_const () << endl;

      a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit) 
					 << "Handle  : " << hex << handle << dec;
			if (handle != 0)
				{
					a_out << " (serial tag: '" << handle->get_serial_tag () << "')";
				}
			a_out << endl;

      return;	
    }

 
		/***************************************************/
 
		const string copyable_things::SERIAL_TAG = "datatools::utils::copyable_things";

		const string & copyable_things::get_serial_tag () const
		{
			return copyable_things::SERIAL_TAG;
		}

    DATATOOLS_CLONEABLE_IMPLEMENTATION(copyable_things)
  
		const string & copyable_things::get_name () const
		{
			return m_name;
		}

		copyable_things & copyable_things::set_name (const string & a_name)
		{
			m_name = a_name;
			return *this;
		}

		const string & copyable_things::get_description () const
		{
			return m_description;
		}

		copyable_things & copyable_things::set_description (const string & a_desc)
		{
			m_description = a_desc;
			return *this;
		}

		void copyable_things::get_names (vector<string> & the_names) const
		{
			the_names.clear ();
			if (m_copyable_things.size () > the_names.capacity ())
				{
					the_names.reserve (m_copyable_things.size ());
				}
			for (dict_t::const_iterator i = m_copyable_things.begin ();
					 i != m_copyable_things.end ();
					 i++)
				{
					the_names.push_back (i->first);
				}
			return;
		}
			
		bool copyable_things::has (const string & a_name) const
		{
			if (a_name.empty ())
				{
					ostringstream message;
					message << "datatools::utils::copyable_things::has: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::const_iterator found = m_copyable_things.find (a_name);
			return (found != m_copyable_things.end ());
		}
			
		bool copyable_things::is_constant (const string & a_name) const
		{
			if (a_name.empty ())
				{
					ostringstream message;
					message << "datatools::utils::copyable_things::is_constant: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::const_iterator found = m_copyable_things.find (a_name);
			if (found == m_copyable_things.end ())
				{ 
					ostringstream message;
					message << "datatools::utils::copyable_things::is_constant: No stored object has name '" << a_name << "' !";
					throw logic_error (message.str ());
				}
			return found->second.is_const ();
		}

		
		void copyable_things::set_constant (const string & a_name, bool a_const)
		{
			bool not_implemented_yet = true;
			if (not_implemented_yet)
				{
					ostringstream message;
					message << "datatools::utils::copyable_things::set_constant: Not implemented !";
					throw logic_error (message.str ());					
				}
			if (a_name.empty ())
				{
					ostringstream message;
					message << "datatools::utils::copyable_things::set_constant: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::iterator found = m_copyable_things.find (a_name);
			if (found == m_copyable_things.end ())
				{
					ostringstream message;
					message << "datatools::utils::copyable_things::set_constant: No stored object has name '" << a_name << "' !";
					throw logic_error (message.str ());
				}
			found->second.set_const (a_const);
			return;
		}
		
		const string & copyable_things::get_description (const string & a_name) const
		{
			if (a_name.empty ())
				{
					ostringstream message;
					message << "datatools::utils::copyable_things::get_description: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::const_iterator found = m_copyable_things.find (a_name);
			if (found == m_copyable_things.end ())
				{
					ostringstream message;
					message << "datatools::utils::copyable_things::get_description: No stored object has name '" << a_name << "' !";
					throw logic_error (message.str ());
				}
			return found->second.description;
		}

		void copyable_things::set_description (const string & a_name, const string & a_desc)
		{
			if (a_name.empty ())
				{
					ostringstream message;
					message << "datatools::utils::copyable_things::set_description: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::iterator found = m_copyable_things.find (a_name);
			if (found == m_copyable_things.end ())
				{
					ostringstream message;
					message << "datatools::utils::copyable_things::set_description: No stored object has name '" << a_name << "' !";
					throw logic_error (message.str ());
				}
			found->second.description = a_desc;
			return;
		}

		size_t copyable_things::size () const
		{
			return m_copyable_things.size ();
		}

		bool copyable_things::empty () const
		{
			return m_copyable_things.size () == 0;
		}

		void copyable_things::__add (const string & a_name, 
												datatools::serialization::i_serializable * a_obj,
												const string & a_desc,
												bool a_const)
		{
			if (a_obj == 0)
				{
					ostringstream message;
					message << "datatools::utils::copyable_things::__add: Cannot add a NULL pointed object !";
					throw runtime_error (message.str ());
				}
			if (a_name.empty ())
				{
					ostringstream message;
					message << "datatools::utils::copyable_things::__add: Cannot add an object with an empty name !";
					if (a_obj != 0)
						{
							delete a_obj;
	  				}
					throw logic_error (message.str ());
				}
			dict_t::const_iterator found = m_copyable_things.find (a_name);
			if (found != m_copyable_things.end ())
				{
					ostringstream message; 
					message << "datatools::utils::copyable_things::__add: An object with name '" << a_name  
									<< "' is already stored in the dictionnary !";
					throw logic_error (message.str ());
				}
			m_copyable_things[a_name].set_description (a_desc);
			m_copyable_things[a_name].set_const (a_const);
			m_copyable_things[a_name].handle = a_obj;
			return;
		} 

		void copyable_things::remove (const string & a_name)
		{
			if (a_name.empty ())
				{
					return;
				}
			dict_t::iterator found = m_copyable_things.find (a_name);
			if (found == m_copyable_things.end ())
				{
					ostringstream message; 
					message << "datatools::utils::copyable_things::remove: No object with name '" << a_name  
									<< "' is stored in the dictionnary !";
					throw logic_error (message.str ());
				}
			if (found->second.handle != 0)
				{
					delete found->second.handle;
					found->second.handle = 0;
				}
			m_copyable_things.erase (found);
			return;
		}

		void copyable_things::erase (const string & a_name)
		{
			remove (a_name);
			return;
		}
  
		void copyable_things::dump (ostream & a_out) const
		{
			tree_dump (a_out, "datatools::utils::copyable_things:");
			return;
		}

		void copyable_things::reset ()
		{
			this->clear ();
			m_description.clear ();
			m_name.clear ();
			return;
		}

		void copyable_things::clear ()
		{
			for (dict_t::iterator i = m_copyable_things.begin ();
					 i != m_copyable_things.end ();
					 i++)
				{
					datatools::serialization::i_serializable * s = i->second.handle;
					if (s != 0)
						{
							delete s;
						}
				}
			m_copyable_things.clear ();
			return;
		}

    // ctor:
		copyable_things::copyable_things () : datatools::serialization::i_serializable ()
		{
			return;
		}
      
		// ctor:
		copyable_things::copyable_things (const string & a_name, const string & a_description)
		{
			set_name (a_name);
			set_description (a_description);
			return;
		}

    // dtor:
		copyable_things::~copyable_things ()
		{
			reset ();
			return;
		}

		// ctor:
		copyable_things::copyable_things (const copyable_things & a_ct)
		{
			cerr << "DEVEL: copyable_things::copyable_things (copy): " << "Entering..." << endl;
			set_name (a_ct.get_name ());
			set_description (a_ct.get_description ());
			
			for (dict_t::const_iterator i = a_ct.m_copyable_things.begin ();
					 i != a_ct.m_copyable_things.end () ;
					 i++) 
				{
					const std::string & key = i->first;
					cerr << "DEVEL : Key = " << key << endl;
					const entry_t & a_entry = i->second;
					entry_t added_entry;
					m_copyable_things[key] = added_entry;
					m_copyable_things[key].description = a_entry.description;
					m_copyable_things[key].flags = a_entry.flags;
					m_copyable_things[key].handle = 0;
					if (a_entry.handle != 0)
						{
							cerr << "DEVEL :   handle     => " << a_entry.handle 
									 << " type='" << typeid (*a_entry.handle).name () << "'"
									 << endl;
							datatools::utils::i_cloneable * tmp_handle 
								= reinterpret_cast<datatools::utils::i_cloneable *> (a_entry.handle);
							cerr << "DEVEL :   tmp_handle => " << tmp_handle 
									 << " type='" << typeid (*tmp_handle).name () << "'" << endl;
							uint32_t * ptr_class_id = reinterpret_cast<uint32_t *> (a_entry.handle);
							uint32_t class_id = *ptr_class_id;
							cerr << "DEVEL :   class_id => " << class_id << endl;
							
							/*
							// force the 'datatools::utils::i_cloneable' interface here :
							datatools::utils::i_cloneable * new_handle = tmp_handle->clone ();
							cerr << "DEVEL :   new_handle => " << new_handle 
									 << " type='" << typeid (new_handle).name () << "'" << endl;
							if (new_handle == 0)
								{
									ostringstream message;
									message << "datatools::utils::copyable_things::copyable_things: " 
													<< "Cannot clone object '" << key << "' !";
									throw runtime_error (message.str ());
								}
							datatools::serialization::i_serializable * tmp_ser 
								=  dynamic_cast<datatools::serialization::i_serializable *> (new_handle); 
							*/
							//m_copyable_things[key].handle	= tmp_ser;
							//void * 
							datatools::serialization::i_serializable * new_handle 
								= reinterpret_cast<datatools::serialization::i_serializable *> (tmp_handle->clone ());
							cerr << "DEVEL :   new_handle => " << new_handle 
									 << " type='" << typeid (*new_handle).name () << "'" << endl;
							uint32_t * ref_class_id = reinterpret_cast<uint32_t *> (new_handle);
							*ref_class_id = class_id;
							cerr << "DEVEL :   new_handle => " << new_handle 
									 << " type='" << typeid (*new_handle).name () << "'" << endl;
							m_copyable_things[key].handle	= new_handle;
							cerr << "DEVEL :   handle (copy) => " << m_copyable_things[key].handle 
									 << " type='" << typeid (m_copyable_things[key].handle).name () << "'" << endl;
							
						}
				}

			return;
		}
		
		// assignment operator :
		copyable_things & copyable_things::operator= (const copyable_things & a_ct)
		{
			cerr << "DEVEL: copyable_things::operator= " << "Entering..." << endl;
			// protect against self-copy :
			if (&a_ct == this) return *this;
			this->clear ();
			set_name (a_ct.get_name ());
			set_description (a_ct.get_description ());

			for (dict_t::const_iterator i = a_ct.m_copyable_things.begin ();
					 i != a_ct.m_copyable_things.end () ;
					 i++) 
				{
					const std::string & key = i->first;
					cerr << "DEVEL : Key = " << key << endl;
					const entry_t & a_entry = i->second;
					entry_t added_entry;
					m_copyable_things[key] = added_entry;
					m_copyable_things[key].description = a_entry.description;
					m_copyable_things[key].flags = a_entry.flags;
					m_copyable_things[key].handle = 0;
					if (a_entry.handle != 0)
						{
							// force the 'datatools::utils::i_cloneable' interface here :
							cerr << "DEVEL :   handle => " << a_entry.handle << endl;
							datatools::utils::i_cloneable * new_handle 
								= (reinterpret_cast<datatools::utils::i_cloneable *> (a_entry.handle))->clone ();
							cerr << "DEVEL :   new_handle => " << new_handle << endl;
							if (new_handle == 0)
								{
									ostringstream message;
									message << "datatools::utils::copyable_things::copyable_things: " 
													<< "Cannot clone object '" << key << "' !";
									throw runtime_error (message.str ());
								}
							m_copyable_things[key].handle 
								= reinterpret_cast<datatools::serialization::i_serializable *> (new_handle);
							cerr << "DEVEL :   handle (copy) => " << m_copyable_things[key].handle << endl;
						}
				}

			return *this;
		}

    void copyable_things::tree_dump (std::ostream & a_out, 
														const std::string & a_title,
														const std::string & a_indent,
														bool a_inherit) const
    {
      namespace du = datatools::utils;
      std::string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ()) 
				{
					a_out << indent << a_title << std::endl;
				}
			
      if (! m_name.empty ()) 
				{
					a_out << indent << du::i_tree_dumpable::tag 
							 << "Name  : " <<  get_name () << std::endl;
				}
			if (! m_description.empty ()) 
				{
					a_out << indent << du::i_tree_dumpable::tag 
							 << "Description  : " <<  get_description () << std::endl;
				}
      if (m_copyable_things.size () == 0) 
				{
					a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)
							 << "<no property>" << std::endl;
				}
      else 
				{
					for (dict_t::const_iterator i = m_copyable_things.begin ();
							 i != m_copyable_things.end () ;
							 i++) 
						{
							const std::string & key = i->first;
							const entry_t & a_entry = i->second;
							a_out << indent;
							std::ostringstream indent_oss;
							indent_oss << indent;
							dict_t::const_iterator j = i; 
							j++;
							if (j == m_copyable_things.end ()) 
								{
									a_out << du::i_tree_dumpable::inherit_tag (a_inherit);
									indent_oss << du::i_tree_dumpable::inherit_skip_tag (a_inherit);
								}
							else 
								{
									a_out << du::i_tree_dumpable::tag;
									indent_oss << du::i_tree_dumpable::skip_tag;
								}
							a_out << "Name : " << '"' << key << '"' << std::endl;
							a_entry.tree_dump (a_out, "", indent_oss.str ());
						}
				}
      return;
    }

  }  // end of namespace utils

}  // end of namespace datatools
 
// end of copyable_things.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
