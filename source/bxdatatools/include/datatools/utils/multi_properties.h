// -*- mode: c++; -*-
//! \file datatools/utils/multi_properties.h
/* multi_properties.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009
 * Last modified: 2011-03-09
 * 
 * License: 
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
 *   A container of properties objects implemented as a dictionnary
 * 
 * History: 
 * 
 */

#ifndef __datatools__utils__multi_properties_h
#define __datatools__utils__multi_properties_h 1

#include <string>

#include <fstream>
#include <map>
#include <list>

#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>

#include <datatools/utils/properties.h>

#include <datatools/serialization/i_serializable.h>
#include <datatools/utils/i_clear.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/i_cloneable.h>

using namespace std;

namespace datatools {

  namespace utils {

    class multi_properties :    
      public datatools::serialization::i_serializable,
      public datatools::utils::i_clear,
      public datatools::utils::i_tree_dumpable,
      public datatools::utils::i_cloneable 
    {
    public:
      static const char OPEN;
      static const char CLOSE;
      static const char COMMENT;
      static const string DEFAULT_KEY_LABEL;
      static const string DEFAULT_META_LABEL;
      static const bool with_header_footer;
      static const bool without_header_footer;
      static const bool write_public_only;
      static const bool write_private_also;
      static const bool read_public_only;
      static const bool read_private_also;
      //static const string SERIAL_TAG;

      static bool g_debug;

    public:

      class entry :    
	public datatools::serialization::i_serializable,
	public datatools::utils::i_tree_dumpable  
      {
      public:
	//static const string SERIAL_TAG;

      private:

	string     key_;
	string     meta_;
	properties properties_;

      public:

	const properties & get_properties () const;

	properties & get_properties ();

	const string & get_key () const;

	void set_key (const string &);

	const string & get_meta () const;

	void set_meta (const string &);

	bool has_meta () const;

	entry (const string & a_key = "", 
	       const string & a_meta = "");

	virtual ~entry ();

      private:

        DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()
 	  
	friend class boost::serialization::access; 
	BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

      public:
	virtual void tree_dump (ostream & a_out          = clog, 
				const string & a_title   = "",
				const string & a_oindent = "",
				bool a_oinherit          = false) const;

      };

    public:

      typedef map<string, entry> entries_col_t;
      typedef list<entry *> entries_ordered_col_t;

    private:
	
      bool                  debug_;
      string                description_;
      string                key_label_;
      string                meta_label_;
      entries_col_t         entries_;
      entries_ordered_col_t ordered_entries_;

    public:

      bool is_debug () const;

      void set_debug (bool = true);

      void set_description (const string & a_description);

      const string & get_description () const;

      void set_key_label (const string & a_key_label);

      const string & get_key_label () const;

      void set_meta_label (const string & a_meta_label);

      const string & get_meta_label () const;
	
      size_t size () const;

      void reset ();

      virtual void clear ();

      const entries_col_t & entries () const;

      const entries_ordered_col_t & ordered_entries () const;

      multi_properties (const string & a_key_label = "", 
			const string & a_meta_label = "", 
			const string & a_description = "", 
			bool a_debug = false);

      virtual ~multi_properties ();

      void dump (ostream & a_out = clog) const;

    private:

      void remove_ (const string & a_key);

      void add_ (const string & a_key, 
		  const string & a_meta = "");

    public:

      //! From the datatools::utils::i_cloneable interface :
      DATATOOLS_CLONEABLE_DECLARATION (multi_properties)

      DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()

    private:
	
      friend class boost::serialization::access; 
      BOOST_SERIALIZATION_SERIALIZE_DECLARATION()
      //BOOST_SERIALIZATION_SPLIT_MEMBER_SERIALIZE_DECLARATIONS()

    public:

      const entry & get (const string & a_key) const;

      entry & get (const string & a_key);

      bool has_key (const string & a_key) const;

      bool has_section (const string & a_key) const;

      const properties & get_section (const string & a_key) const;

      properties & get_section (const string & a_key);

      void add (const string & a_key, 
		const string & a_meta,
		const properties & a_props);

      void add (const string & a_key, 
		const properties & a_props);

      void add (const string & a_key, 
		const string & a_meta = "");

      void remove (const string & a_key);

      void write (const string & a_filename,
		  bool a_header_footer = true,
		  bool a_write_private = false) const;
	
      void read (const string & a_filename,
		 bool a_skip_private = false);

    private:
	
      void read_ (istream & a_in, bool a_skip_private);

    public:

      virtual void tree_dump (ostream & a_out         = clog, 
			      const string & a_title  = "",
			      const string & a_indent = "",
			      bool a_inherit          = false) const;
	
    };

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__multi_properties_h

/* end of multi_properties.h */
