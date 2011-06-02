/* things.cc
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

#include <datatools/utils/things.h>

namespace datatools {

  namespace utils {

		using namespace std;

		bad_things_cast::bad_things_cast (const string & a_msg)
			: std::exception (), m_message (a_msg)
		{
			return;
		}

		bad_things_cast::~bad_things_cast () throw ()
		{
			return;
		}

		const char * bad_things_cast::what () const throw ()
		{
			return m_message.c_str ();
		}

		/***************************************************/

		things::entry_t::entry_t ()
		{
			description = "";
			flags       = 0x0;
			handle      = 0;
			return;
		}

		bool things::entry_t::is_not_const () const
		{
      return (flags & MASK_CONST) == 0;
		}

		bool things::entry_t::is_const () const
		{
			return ! is_not_const ();
		}

		void things::entry_t::set_const (bool a_const)
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

		void things::entry_t::set_description (const std::string & a_desc)
		{
			description = a_desc;
			return;
		}

		const std::string & things::entry_t::get_description () const
		{
			return description;
		}

		bool things::entry_t::has_description () const
		{
			return ! description.empty ();
		}

    void things::entry_t::tree_dump (std::ostream & a_out,
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

		const string things::SERIAL_TAG = "datatools::utils::things";

		const string & things::get_serial_tag () const
		{
			return things::SERIAL_TAG;
		}

		const string & things::get_name () const
		{
			return m_name;
		}

		things & things::set_name (const string & a_name)
		{
			m_name = a_name;
			return *this;
		}

		const string & things::get_description () const
		{
			return m_description;
		}

		things & things::set_description (const string & a_desc)
		{
			m_description = a_desc;
			return *this;
		}

		void things::get_names (vector<string> & the_names) const
		{
			the_names.clear ();
			if (m_things.size () > the_names.capacity ())
				{
					the_names.reserve (m_things.size ());
				}
			for (dict_t::const_iterator i = m_things.begin ();
					 i != m_things.end ();
					 i++)
				{
					the_names.push_back (i->first);
				}
			return;
		}

		bool things::has (const string & a_name) const
		{
			if (a_name.empty ())
				{
					ostringstream message;
					message << "datatools::utils::things::has: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::const_iterator found = m_things.find (a_name);
			return (found != m_things.end ());
		}

		bool things::is_constant (const string & a_name) const
		{
			if (a_name.empty ())
				{
					ostringstream message;
					message << "datatools::utils::things::is_constant: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::const_iterator found = m_things.find (a_name);
			if (found == m_things.end ())
				{
					ostringstream message;
					message << "datatools::utils::things::is_constant: No stored object has name '" << a_name << "' !";
					throw logic_error (message.str ());
				}
			return found->second.is_const ();
		}


		void things::set_constant (const string & a_name, bool a_const)
		{
			bool not_implemented_yet = true;
			if (not_implemented_yet)
				{
					ostringstream message;
					message << "datatools::utils::things::set_constant: Not implemented !";
					throw logic_error (message.str ());
				}
			if (a_name.empty ())
				{
					ostringstream message;
					message << "datatools::utils::things::set_constant: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::iterator found = m_things.find (a_name);
			if (found == m_things.end ())
				{
					ostringstream message;
					message << "datatools::utils::things::set_constant: No stored object has name '" << a_name << "' !";
					throw logic_error (message.str ());
				}
			found->second.set_const (a_const);
			return;
		}

		const string & things::get_description (const string & a_name) const
		{
			if (a_name.empty ())
				{
					ostringstream message;
					message << "datatools::utils::things::get_description: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::const_iterator found = m_things.find (a_name);
			if (found == m_things.end ())
				{
					ostringstream message;
					message << "datatools::utils::things::get_description: No stored object has name '" << a_name << "' !";
					throw logic_error (message.str ());
				}
			return found->second.description;
		}

		void things::set_description (const string & a_name, const string & a_desc)
		{
			if (a_name.empty ())
				{
					ostringstream message;
					message << "datatools::utils::things::set_description: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::iterator found = m_things.find (a_name);
			if (found == m_things.end ())
				{
					ostringstream message;
					message << "datatools::utils::things::set_description: No stored object has name '" << a_name << "' !";
					throw logic_error (message.str ());
				}
			found->second.description = a_desc;
			return;
		}

		size_t things::size () const
		{
			return m_things.size ();
		}

		bool things::empty () const
		{
			return m_things.size () == 0;
		}

		void things::add_ (const string & a_name,
												datatools::serialization::i_serializable * a_obj,
												const string & a_desc,
												bool a_const)
		{
			if (a_obj == 0)
				{
					ostringstream message;
					message << "datatools::utils::things::add_: Cannot add a NULL pointed object !";
					throw runtime_error (message.str ());
				}
			if (a_name.empty ())
				{
					ostringstream message;
					message << "datatools::utils::things::add_: Cannot add an object with an empty name !";
					if (a_obj != 0)
						{
							delete a_obj;
	  				}
					throw logic_error (message.str ());
				}
			dict_t::const_iterator found = m_things.find (a_name);
			if (found != m_things.end ())
				{
					ostringstream message;
					message << "datatools::utils::things::add_: An object with name '" << a_name
									<< "' is already stored in the dictionnary !";
					throw logic_error (message.str ());
				}
			m_things[a_name].set_description (a_desc);
			m_things[a_name].set_const (a_const);
			m_things[a_name].handle = a_obj;
			return;
		}

		void things::remove (const string & a_name)
		{
			if (a_name.empty ())
				{
					return;
				}
			dict_t::iterator found = m_things.find (a_name);
			if (found == m_things.end ())
				{
					ostringstream message;
					message << "datatools::utils::things::remove: No object with name '" << a_name
									<< "' is stored in the dictionnary !";
					throw logic_error (message.str ());
				}
			if (found->second.handle != 0)
				{
					delete found->second.handle;
					found->second.handle = 0;
				}
			m_things.erase (found);
			return;
		}

		void things::erase (const string & a_name)
		{
			remove (a_name);
			return;
		}

		void things::dump (ostream & a_out) const
		{
			tree_dump (a_out, "datatools::utils::things:");
			return;
		}

		void things::reset ()
		{
			this->clear ();
			m_description.clear ();
			m_name.clear ();
			return;
		}

		void things::clear ()
		{
			for (dict_t::iterator i = m_things.begin ();
					 i != m_things.end ();
					 i++)
				{
					datatools::serialization::i_serializable * s = i->second.handle;
					if (s != 0)
						{
							delete s;
							i->second.handle = 0;
						}
				}
			m_things.clear ();
			return;
		}

    // ctor:
		things::things () : datatools::serialization::i_serializable ()
		{
			return;
		}

		// ctor:
		things::things (const string & a_name, const string & a_description)
		{
			set_name (a_name);
			set_description (a_description);
			return;
		}

    // dtor:
		things::~things ()
		{
			reset ();
			return;
		}

    void things::tree_dump (std::ostream & a_out,
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
      if (m_things.size () == 0)
				{
					a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)
							 << "<no property>" << std::endl;
				}
      else
				{
					for (dict_t::const_iterator i = m_things.begin ();
							 i != m_things.end () ;
							 i++)
						{
							const std::string & key = i->first;
							const entry_t & a_entry = i->second;
							a_out << indent;
							std::ostringstream indent_oss;
							indent_oss << indent;
							dict_t::const_iterator j = i;
							j++;
							if (j == m_things.end ())
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

// end of things.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
