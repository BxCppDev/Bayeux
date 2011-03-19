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

		bad_things_cast::bad_things_cast (const string & msg_) 
			: std::exception (), m_message (msg_)
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

		void things::entry_t::set_const (bool const_)
		{
			if (const_)
				{
					flags |= MASK_CONST;
				}
			else
				{
					flags &= ~MASK_CONST;
				}
 			return;
		}

		void things::entry_t::set_description (const std::string & desc_)
		{
			description = desc_;
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

    void things::entry_t::tree_dump (std::ostream & out_, 
																		 const std::string & title_,
																		 const std::string & indent_,
																		 bool inherit_) const
    {
      namespace du = datatools::utils;
      std::string indent;
      if (! indent_.empty () ) indent = indent_;
      if (! title_.empty ()) 
				{
	  out_ << indent << title_ << std::endl;
				}
			
      if (! description.empty ()) 
				{
					out_ << indent << du::i_tree_dumpable::tag 
							 << "Description  : '" <<  get_description () 
							 << "'" << std::endl;
				}

      out_ << indent << du::i_tree_dumpable::tag 
					 << "Const  : " << is_const () << endl;

      out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_) 
					 << "Handle  : " << hex << handle << dec;
			if (handle != 0)
				{
					out_ << " (serial tag: '" << handle->get_serial_tag () << "')";
				}
			out_ << endl;

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

		things & things::set_name (const string & name_)
		{
			m_name = name_;
			return *this;
		}

		const string & things::get_description () const
		{
			return m_description;
		}

		things & things::set_description (const string & desc_)
		{
			m_description = desc_;
			return *this;
		}
			
		bool things::has (const string & name_) const
		{
			if (name_.empty ())
				{
					ostringstream message;
					message << "datatools::utils::things::has: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::const_iterator found = m_things.find (name_);
			return (found != m_things.end ());
		}
			
		bool things::is_constant (const string & name_) const
		{
			if (name_.empty ())
				{
					ostringstream message;
					message << "datatools::utils::things::is_constant: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::const_iterator found = m_things.find (name_);
			if (found == m_things.end ())
				{ 
					ostringstream message;
					message << "datatools::utils::things::is_constant: No stored object has name '" << name_ << "' !";
					throw logic_error (message.str ());
				}
			return found->second.is_const ();
		}

		
		void things::set_constant (const string & name_, bool const_)
		{
			bool not_implemented_yet = true;
			if (not_implemented_yet)
				{
					ostringstream message;
					message << "datatools::utils::things::set_constant: Not implemented !";
					throw logic_error (message.str ());					
				}
			if (name_.empty ())
				{
					ostringstream message;
					message << "datatools::utils::things::set_constant: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::iterator found = m_things.find (name_);
			if (found == m_things.end ())
				{
					ostringstream message;
					message << "datatools::utils::things::set_constant: No stored object has name '" << name_ << "' !";
					throw logic_error (message.str ());
				}
			found->second.set_const (const_);
			return;
		}
		
		const string & things::get_description (const string & name_) const
		{
			if (name_.empty ())
				{
					ostringstream message;
					message << "datatools::utils::things::get_description: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::const_iterator found = m_things.find (name_);
			if (found == m_things.end ())
				{
					ostringstream message;
					message << "datatools::utils::things::get_description: No stored object has name '" << name_ << "' !";
					throw logic_error (message.str ());
				}
			return found->second.description;
		}

		void things::set_description (const string & name_, const string & desc_)
		{
			if (name_.empty ())
				{
					ostringstream message;
					message << "datatools::utils::things::set_description: Empty name !";
					throw logic_error (message.str ());
				}
			dict_t::iterator found = m_things.find (name_);
			if (found == m_things.end ())
				{
					ostringstream message;
					message << "datatools::utils::things::set_description: No stored object has name '" << name_ << "' !";
					throw logic_error (message.str ());
				}
			found->second.description = desc_;
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

		void things::__add (const string & name_, 
												datatools::serialization::i_serializable * obj_	,
												const string & desc_,
												bool const_)
		{
			if (obj_ == 0)
				{
					ostringstream message;
					message << "datatools::utils::things::__add: Cannot add a NULL pointed object !";
					throw runtime_error (message.str ());
				}
			if (name_.empty ())
				{
					ostringstream message;
					message << "datatools::utils::things::__add: Cannot add an object with an empty name !";
					if (obj_ != 0)
						{
							delete obj_;
	  				}
					throw logic_error (message.str ());
				}
			dict_t::const_iterator found = m_things.find (name_);
			if (found != m_things.end ())
				{
					ostringstream message; 
					message << "datatools::utils::things::__add: An object with name '" << name_  
									<< "' is already stored in the dictionnary !";
					throw logic_error (message.str ());
				}
			m_things[name_].set_description (desc_);
			m_things[name_].set_const (const_);
			m_things[name_].handle = obj_;
			return;
		} 

		void things::remove (const string & name_)
		{
			if (name_.empty ())
				{
					return;
				}
			dict_t::iterator found = m_things.find (name_);
			if (found == m_things.end ())
				{
					ostringstream message; 
					message << "datatools::utils::things::remove: No object with name '" << name_  
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

		void things::erase (const string & name_)
		{
			remove (name_);
			return;
		}
  
		void things::dump (ostream & out_) const
		{
			tree_dump (out_, "datatools::utils::things:");
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
		things::things (const string & name_, const string & description_)
		{
			set_name (name_);
			set_description (description_);
			return;
		}

    // dtor:
		things::~things ()
		{
			reset ();
			return;
		}

    void things::tree_dump (std::ostream & out_, 
														const std::string & title_,
				const std::string & indent_,
														bool inherit_) const
    {
      namespace du = datatools::utils;
      std::string indent;
      if (! indent_.empty ()) indent = indent_;
      if (! title_.empty ()) 
				{
					out_ << indent << title_ << std::endl;
				}
			
      if (! m_name.empty ()) 
				{
					out_ << indent << du::i_tree_dumpable::tag 
							 << "Name  : " <<  get_name () << std::endl;
				}
			if (! m_description.empty ()) 
				{
					out_ << indent << du::i_tree_dumpable::tag 
							 << "Description  : " <<  get_description () << std::endl;
				}
      if (m_things.size () == 0) 
				{
					out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
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
							out_ << indent;
							std::ostringstream indent_oss;
							indent_oss << indent;
							dict_t::const_iterator j = i; 
							j++;
							if (j == m_things.end ()) 
								{
									out_ << du::i_tree_dumpable::inherit_tag (inherit_);
									indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
								}
							else 
								{
									out_ << du::i_tree_dumpable::tag;
									indent_oss << du::i_tree_dumpable::skip_tag;
								}
							out_ << "Name : " << '"' << key << '"' << std::endl;
							a_entry.tree_dump (out_, "", indent_oss.str ());
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
