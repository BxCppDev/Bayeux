// multi_properties.cc

#include <stdexcept>
#include <sstream>
  
#include <datatools/utils/multi_properties.h>

using namespace std;

namespace datatools {

  namespace utils {
    
    const char multi_properties::OPEN = '[';

    const char multi_properties::CLOSE = ']';

    const char multi_properties::COMMENT = '#';

    const string multi_properties::DEFAULT_KEY_LABEL = "name";

    const string multi_properties::DEFAULT_META_LABEL = "type";

    const bool multi_properties::with_header_footer = true;

    const bool multi_properties::without_header_footer = false;

    const bool multi_properties::write_public_only = false;

    const bool multi_properties::write_private_also = true;

    const bool multi_properties::read_public_only = true;

    const bool multi_properties::read_private_also = false;

    const string multi_properties::SERIAL_TAG       = "datatools::utils::multi_properties";

    const string multi_properties::entry::SERIAL_TAG = "__datatools::utils::multi_properties::entry";

    bool multi_properties::g_debug = false;

    /****************************************************************/

    const string & multi_properties::entry::get_serial_tag () const
    {
      return multi_properties::entry::SERIAL_TAG;
    }

    const properties & multi_properties::entry::get_properties () const
    {
      return properties_;
    }

    properties & multi_properties::entry::get_properties ()
    {
      return properties_;
    }

    const string & multi_properties::entry::get_key () const
    {
      return key_;
    }

    void multi_properties::entry::set_key (const string & a_key)
    {
      if (a_key.empty ())
	{
	  ostringstream message;
	  message << "multi_properties::entry::set_key: Missing key !";
	  throw runtime_error (message.str ());
	}
      key_ = a_key;
      return;
    }

    const string & multi_properties::entry::get_meta () const
    {
     return meta_;
    }

    void multi_properties::entry::set_meta (const string & a_meta)
    {
      meta_ = a_meta;
      return;
    }

    bool multi_properties::entry::has_meta () const
    {
      return (! meta_.empty ());
    }

    multi_properties::entry::entry (const string & a_key, 
				    const string & a_meta)
    {
      if (! a_key.empty ())
	{
	  set_key (a_key);
	}
      set_meta (a_meta);
      return;
    }

    multi_properties::entry::~entry ()
    {
      properties_.clear ();
      return;
    }
 	
    void multi_properties::entry::tree_dump (ostream & a_out, 
					     const string & a_title,
					     const string & indent_,
					     bool inherit_) const
    {
      namespace du = datatools::utils;
      string indent;
      if (! indent_.empty ()) indent = indent_;
      if (! a_title.empty ()) 
	{
	  a_out << indent << a_title << endl;
	}

      a_out << indent << du::i_tree_dumpable::tag 
	   << "Key         : \"" <<  key_ << "\"" << endl;

      a_out << indent << du::i_tree_dumpable::tag 
	   << "Meta        : \"" <<  meta_ << "\"" << endl;
      
      {
	a_out << indent << du::i_tree_dumpable::inherit_tag (inherit_)
	     << "Properties : ";
	if (properties_.size () == 0) 
	  {
	    a_out << "<empty>"; 
	  }
	a_out << endl;
	{
	  ostringstream indent_oss;
	  indent_oss << indent;
	  indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	  properties_.tree_dump (a_out, "", indent_oss.str ());
	}
      }
      return;
    }


    /****************************************************************/

    DATATOOLS_CLONEABLE_IMPLEMENTATION(multi_properties)

    const string & multi_properties::get_serial_tag () const
    {
      return multi_properties::SERIAL_TAG;
    }

    bool multi_properties::is_debug () const
    {
      return debug_;
    }
    
    void multi_properties::set_debug (bool a_debug)
    {
      debug_ = a_debug;
      return;
    }

    const string & multi_properties::get_description () const
    {
      return description_;
    }

    void multi_properties::set_description (const string & a_description)
    {
      description_ = a_description;
      return;
    }

    void multi_properties::set_key_label (const string & a_key_label)
    {
      if (a_key_label.empty ())
	{
	  throw runtime_error ("multi_properties::set_key_label: Empty key label is not allowed !");
	}
      key_label_ = a_key_label;
      return;
    }

    const string & multi_properties::get_key_label () const
    {
      return key_label_;
    }

    void multi_properties::set_meta_label (const string & a_meta_label)
    {
     meta_label_ = a_meta_label;
      return;
    }

    const string & multi_properties::get_meta_label () const
    {
      return meta_label_;
    }
	
    size_t multi_properties::size () const
    {
      if (ordered_entries_.size () != entries_.size ()) 
	{
	  throw runtime_error ("multi_properties::size: Containers are broken !");
	}
      return entries_.size ();
    }

    void multi_properties::reset ()
    {
      key_label_ = DEFAULT_KEY_LABEL;
      meta_label_ = DEFAULT_META_LABEL;
      ordered_entries_.clear ();
      entries_.clear ();
      return;
    }

    void multi_properties::clear ()
    {
      reset ();
      return;
    }

    const multi_properties::entries_col_t & multi_properties::entries () const
    {
      return entries_;
    }

    const multi_properties::entries_ordered_col_t & 
    multi_properties::ordered_entries () const
    {
      return ordered_entries_;
    }

    // ctor:
    multi_properties::multi_properties (const string & a_key_label,
					const string & a_meta_label,
					const string & a_description,
					bool a_debug)
    { 
      debug_ = a_debug;
      key_label_ = DEFAULT_KEY_LABEL;
      meta_label_ = DEFAULT_META_LABEL;
      if (! a_key_label.empty ()) set_key_label (a_key_label);
      //if (! a_meta_label.empty ()) 
      set_meta_label (a_meta_label);
      set_description (a_description);
      return;
    }

    // dtor:
    multi_properties::~multi_properties ()
    {
      reset ();
      return;
    }

    bool multi_properties::has_key (const string & a_key) const
    {
      entries_col_t::const_iterator found = entries_.find (a_key);
      return found != entries_.end ();
    }

    const multi_properties::entry & 
    multi_properties::get (const string & a_key) const
    {
      entries_col_t::const_iterator found = entries_.find (a_key);
      if (found == entries_.end ())
	{
	  ostringstream message;
	  message << "multi_properties::get: Key '"
		  <<  a_key << "' is not used !";
	  throw runtime_error (message.str ());
	}
      return found->second;
    }
    
    multi_properties::entry & multi_properties::get (const string & a_key)
    {
      entries_col_t::iterator found = entries_.find (a_key);
      if (found == entries_.end ())
	{
	  ostringstream message;
	  message << "multi_properties::get: Key '"
		  <<  a_key << "' is not used !";
	  throw runtime_error (message.str ());
	}
      return found->second;
    }

    void multi_properties::remove_ (const string & a_key)
    {
      bool devel = debug_;
      //devel = true;
      entries_ordered_col_t::iterator found = ordered_entries_.end ();
       if (devel)
	{
	  clog << "DEVEL: multi_properties::add_: "
	       << "Removing key '" << a_key << "'";
	  clog << endl;
	}
       for (entries_ordered_col_t::iterator i = ordered_entries_.begin ();
	   i != ordered_entries_.end ();
	   i++)
	{
	  entry * e = *i;
	  if (e->get_key () == a_key)
	    {
	      found = i;
	      break;
	    }
	}
      if (found != ordered_entries_.end ())
	{
	  ordered_entries_.erase (found);
	  if (devel)
	    {
	      clog << "DEVEL: multi_properties::add_: "
		   << "Removing ordered key '" << a_key << "'...";
	      clog << endl;
	    }
	}
      size_t n = entries_.erase (a_key);
      return;
    }

    void multi_properties::remove (const string & a_key)
    {
      entries_col_t::iterator found = entries_.find (a_key);
      if (found == entries_.end ())
	{
	  ostringstream message;
	  message << "multi_properties::remove: Key '"
		  <<  a_key << "' is not used !";
	  throw runtime_error (message.str ());
	}
      multi_properties::remove_ (a_key);
      return;
    }

    void multi_properties::add_ (const string & a_key, 
				  const string & a_meta)
    {
      bool devel = debug_;
      //devel = true;
      if (entries_.find (a_key) != entries_.end ())
	{
	  ostringstream message;
	  message << "multi_properties::add_: Key '"
		  <<  a_key << "' is already used !";
	  throw runtime_error (message.str ());
	}
      if (devel)
	{
	  clog << "DEVEL: multi_properties::add_: "
	       << "Adding key '" << a_key << "'";
	  if (! a_meta.empty()) clog << " with meta '" << a_meta << "'";
	  clog << endl;
	}
      entries_[a_key] = entry (a_key, a_meta);
      ordered_entries_.push_back (&entries_[a_key]);
      return;
    }

    void multi_properties::add (const string & a_key, 
				const string & a_meta)
    {
      add_ (a_key, a_meta);
      return;
    }

    void multi_properties::add (const string & a_key, 
				const properties & a_props)
    {
      add_ (a_key);
      entries_[a_key].get_properties () = a_props;
      return;
    }

    void multi_properties::add (const string & a_key, 
				const string & a_meta,
				const properties & a_props)
    {
      add_ (a_key, a_meta);
      entries_[a_key].get_properties () = a_props;
      return;
    }

    void multi_properties::write (const string & a_filename, 
				  bool a_header_footer,
				  bool a_write_private) const
    {
      bool devel = multi_properties::g_debug;
      if (devel) 
	{
	  clog << "DEVEL: multi_properties::write: Entering..." << endl;
	  clog << "DEVEL: multi_properties::write: Filename : '"
	       << a_filename << "'." << endl;
	}
      ofstream fout (a_filename.c_str ());
      if (! fout)
	{
	  ostringstream message;
	  message << "multi_properties::write: Cannot open file '"
		  <<  a_filename << "'!";
	  throw runtime_error (message.str ());
	}
 
      properties::config pcfg (true, 
			       properties::config::MODE_BARE, 
			       true);

      if (a_header_footer)
	{
	  fout << "# List of multi-properties (datatools::utils::multi_properties):" << endl;
	  fout << endl;
	}

      fout << "#@description " << get_description () << endl;
      fout << "#@key_label   " << '"' << get_key_label () << '"' << endl;
      fout << "#@meta_label  " << '"' << get_meta_label ()<< '"' << endl;
      fout << endl;

      for (entries_col_t::const_iterator i = entries_.begin ();
	   i != entries_.end ();
	   i++)
	{
	  const string & name = i->first;
	  const entry & an_entry = i->second;

	  if (devel) clog << "DEVEL: multi_properties::write: "
			  << "write_private = '" << a_write_private << "'" << endl;
	  
	  if (! a_write_private)
	    {
	      if (devel) clog << "DEVEL: multi_properties::write: "
			      << "name = '" << name << "'" << endl;
	      if (properties::key_is_private (name))
		{
		  if (devel) clog << "DEVEL: multi_properties::write: "
				  << "private" << endl;
		  continue;
		}
	    }

	  if (devel) clog << "DEVEL: multi_properties::write: Name = '" << name << "'" << endl;

	  fout << multi_properties::OPEN
	       << get_key_label () << '=' 
	       << '"' << name << '"';
	  if (an_entry.has_meta ())
	    {
	      fout << ' ' 
		   << get_meta_label () << '=' 
		   << '"' << an_entry.get_meta () << '"';
	    }
	  fout << multi_properties::CLOSE << endl << endl;

	  pcfg.write (fout, an_entry.get_properties ());
	  fout << endl;
	}

      if (a_header_footer)
	{
	  fout << "# End of list of multi-properties." << endl;
	}
      return;
    }
    
    void multi_properties::read (const string & a_filename,
				 bool a_skip_private)
    {
      bool devel = false;
      if (devel) 
	{
	  clog << "DEVEL: multi_properties::read: Entering..." << endl;
	  clog << "DEVEL: multi_properties::read: Filename : '"
	       << a_filename << "'." << endl;
	}
      ifstream fin (a_filename.c_str ());
      if (! fin)
	{
	  ostringstream message;
	  message << "multi_properties::read: Cannot open file '"
		  <<  a_filename << "'!";
	  throw runtime_error (message.str ());
	}

      read_ (fin, a_skip_private);
      return;
    } 

    void multi_properties::read_ (istream & in_,
				   bool a_skip_private)
    {
      bool devel = multi_properties::g_debug;
      //devel = true;
      if (devel)
	{
	  clog << "DEVEL: multi_properties::read_: Entering..." << endl;
	  tree_dump (cerr, "DUMP: " , "DEVEL: ");
	}
      string line_in;
      string mprop_description;
      string mprop_key_label;
      string mprop_meta_label;
      bool line_goon = false;
      int line_count = 0;
      ostringstream current_block_oss;
      string current_key = "";
      string current_meta = "";

      while (in_) 
	{
	  string line_get;
	  getline (in_,line_get);
	  bool line_continue = false; 
	  int sz = line_get.size ();
	  if (sz > 0 && line_get[sz-1] == '\\') 
	    {
	      line_continue = true;
	      line_get = line_get.substr (0, sz - 1);
	    }
	  if (line_goon) 
	    {
	      line_in += line_get;	
	    }
	  else 
	    {
	      // a new line
	      line_in = line_get;
	      line_count++;
	    }
	  line_goon = false;

	  // check if line should go on:
	  if (line_continue) line_goon = true;

	  bool process_block = false;
	  string new_key = "";
	  string new_meta = "";

	  if (! line_goon) 
	    {
	      bool skip_line = false;
	      string line = line_in;
	    
	      // check if line is blank:
	      istringstream check_iss (line_in);
	      string check_word;
	      check_iss >> ws >> check_word;
	      if (check_word.empty ()) 
		{
		  skip_line = true;
		}

	      // check if line is a comment:
	      if (! skip_line) 
		{
		  istringstream iss (line);
		  char c = 0;
		  iss >> c;
		  if (c == '#') 
		    {
		      iss >> ws;
		      string token;
		      iss >> token;
	      
		      if (token == "@description" && mprop_description.empty ()) 
			{
			  iss >> ws;
			  string desc;
			  getline (iss, desc);
			  if (! desc.empty ()) 
			    {
			      mprop_description = desc;
			      set_description (mprop_description);
			    }
			}
		      if (token == "@key_label" && mprop_key_label.empty ()) 
			{
			  iss >> ws;
			  string key_label;
			  if (! properties::config::read_quoted_string (iss, key_label))
			    {
			      throw runtime_error ("multi_properties::read_: Unquoted value for 'key_label'");
			    }
			  string tmp;
			  getline (iss, tmp);
			  if (! key_label.empty ()) 
			    {
			      mprop_key_label = key_label;
			      if (key_label_.empty ())
				{
				  set_key_label (mprop_key_label);
				}
			      else
				{
				  if (key_label_ != mprop_key_label)
				    {
				      ostringstream message;
				      message << "multi_properties::read_: Incompatible key label '"
					      << mprop_key_label << "' with setup '" << key_label_ << "' !";
				      throw runtime_error (message.str ());
				    }
				}
			    }
			}
		      if (token == "@meta_label" && mprop_meta_label.empty ()) 
			{
			  iss >> ws;
			  string meta_label;
			  if (! properties::config::read_quoted_string (iss, meta_label))
			    {
			      throw runtime_error ("multi_properties::read_: Unquoted value for 'meta_label'");
			    }
			  string tmp;
			  getline (iss, tmp);
			  if (meta_label.empty ()) 
			    {
			      if (! meta_label_.empty ())
				{
				  ostringstream message;
				  message << "multi_properties::read_: " 
					  << "Missing meta label with setup '" << meta_label_ << "' !";
				  throw runtime_error (message.str ());
				}
			    }
			  else
			    {
			      mprop_meta_label = meta_label;
			      if (meta_label_.empty ())
				{
				  set_meta_label (mprop_meta_label);
				}
			      else
				{
				  if (meta_label_ != mprop_meta_label)
				    {
				      ostringstream message;
				      message << "multi_properties::read_: Incompatible meta label '"
					      << mprop_meta_label << "' with setup '" << meta_label_ << "' !";
				      throw runtime_error (message.str ());
				    }
				}
			    }
			}
		      skip_line = true; 
		    }
		} // if ( ! skip_line )

	      // parse line:
	      if (! skip_line) 
		{
		  istringstream iss (line);
		  char c = 0;
		  iss >> c >> ws;
		  // search for 'key/meta' line:
		  if (c == '[')
		    {
		      // parse 'key/meta' line:
		      iss >> ws;
		      string key_label;
		      getline (iss, key_label, '=');
		      if (key_label != get_key_label ())
			{
			  ostringstream message;
			  message << "multi_properties::read_: Incompatible key label '"
				  << key_label << "' with setup '" << key_label_ << "' !";
			  throw runtime_error (message.str ());
			}
		      if (! properties::config::read_quoted_string (iss, new_key)) 
			{
			  ostringstream message;
			  message << "multi_properties::read_: "
				  << "Cannot read quoted string key value !" ;
			  throw runtime_error (message.str ());
			}
		      iss >> ws;
		      char dummy = iss.peek ();
		      if (dummy != ']')
			{
			  string meta_label;
			  getline (iss, meta_label, '=');
			  if (! meta_label.empty ())
			    {
			      if (meta_label != get_meta_label ())
				{
				  ostringstream message;
				  message << "multi_properties::read_: Incompatible meta label '"
					  << meta_label << "' with setup '" << meta_label_ << "' !";
				  throw runtime_error (message.str ());
				}
			      if (! properties::config::read_quoted_string (iss, new_meta)) 
				{
				  ostringstream message;
				  message << "multi_properties::read_: "
					  << "Cannot read quoted string meta value!" ;
				  throw runtime_error (message.str ());
				}
			    }
			}
		      else
			{
			  if (! get_meta_label ().empty ())
			    {
				  ostringstream message;
				  message << "multi_properties::read_: "
					  << "Expected meta record '" << get_meta_label () << '=' 
					  << "\"???\"" << "' is missing !" ;
				  throw runtime_error (message.str ());			      
			    }
			}
		      iss >> ws;
		      c = 0;
		      iss >> c;
		      if (c != ']')
			{
			  ostringstream message;
			  message << "multi_properties::read_: "
				  << "Cannot read 'key/meta' closing symbol !" ;
			  throw runtime_error (message.str ());
			}
		      process_block = true;
		    }
		  else 
		    {
		      // append line to the current block stream:
		      if (devel)
			{
			  clog << "DEVEL: multi_properties::read_: " 
			       << "Append line " << "'"  << line << "'" << endl;
			}
		      current_block_oss << line << endl;
		    }
		  if (devel)
		    {
		      clog << "DEVEL: multi_properties::read_: " 
			   << "'"  << line << "'" << endl;
		    }
		} // !skip_line
	      
	    } // if ( ! line_goon )

	  in_ >> ws;
	  if (in_.eof ())
	    {
	      process_block = true;
	    }
	  
	  if (process_block)
	    {
	      //
	      if (! current_key.empty ())
		{
		  bool load_it = true;
		  if (a_skip_private)
		    {
		      if (properties::key_is_private (current_key))
			{
			  if (devel)
			    {
			      clog << "DEVEL: multi_properties::read_: Skip loading of '" 
				   << current_key << "'" << endl;
			    }
			  load_it = false;
			}
		    }

		  if (load_it)
		    {
		      if (devel)
			{
			  clog << "DEVEL: multi_properties::read_: Add '" 
			       << current_key << "'" << endl;
			}
		      add (current_key, current_meta);
		      multi_properties::entry & e = get (current_key);
		      properties::config pcr;
		      if (devel)
			{
			  clog << "DEVEL: multi_properties::read_: Block:" << endl;
			  clog << ">>>" << endl;
			  clog << current_block_oss.str ();
			  clog << "<<<" << endl;
			}
		      istringstream block_iss (current_block_oss.str ());
		      pcr.read (block_iss, e.get_properties ());
		      current_block_oss.str ("");
		    }
		}
	      // update new key/meta values:
	      current_key = new_key;
	      current_meta = new_meta;
	    }
	  
	} // while ( *_in ) 

      return;
    } /* end of multi_properties::read_ */
 
    void multi_properties::dump (ostream & a_out) const
    {
      tree_dump (a_out, "datatools::utils::multi_properties:");
      return;
    }
	
    void multi_properties::tree_dump (ostream & a_out, 
				      const string & a_title,
				      const string & a_indent,
				      bool a_inherit) const
    {
      namespace du = datatools::utils;
      string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ()) 
	{
	  a_out << indent << a_title << endl;
	}
     
      if (! description_.empty ()) 
	{
	  a_out << indent << du::i_tree_dumpable::tag 
	       << "Description  : " <<  get_description () << endl;
	}

      if (! key_label_.empty ()) 
	{
	  a_out << indent << du::i_tree_dumpable::tag 
	       << "Key label    : \"" <<  get_key_label () << "\"" << endl;
	}

      if (! meta_label_.empty ()) 
	{
	  a_out << indent << du::i_tree_dumpable::tag 
	       << "Meta label   : \"" <<  get_meta_label () << "\"" << endl;
	}

      {
	a_out << indent << du::i_tree_dumpable::tag
	     << "Entries      : ";
	if (entries_.size () == 0) 
	  {
	    a_out << "<empty>"; 
	  }
	else
	  {
	    a_out << "[" << entries_.size () << "]";
	  }
	a_out << endl;
	for (entries_col_t::const_iterator i = entries_.begin ();
	     i != entries_.end () ;
	     i++) 
	  {
	    const string & key = i->first;
	    const entry & a_entry = i->second;
	    a_out << indent;
	    ostringstream indent_oss;
	    indent_oss << indent;
	    entries_col_t::const_iterator j = i; 
	    j++;
	    //a_out << du::i_tree_dumpable::inherit_skip_tag (a_inherit);
	    //indent_oss << du::i_tree_dumpable::inherit_skip_tag (a_inherit);
	    a_out << du::i_tree_dumpable::skip_tag;
	    indent_oss << du::i_tree_dumpable::skip_tag;
	    if (j == entries_.end ()) 
	      {
		a_out << du::i_tree_dumpable::last_tag;
		indent_oss << du::i_tree_dumpable::inherit_skip_tag (a_inherit);
	      }
	    else 
	      {
		a_out << du::i_tree_dumpable::tag;
		indent_oss << du::i_tree_dumpable::skip_tag;
	      }
	    a_out << "Entry : " << '"' << key << '"';
	    if (properties::key_is_private (key))
	      {
		a_out << " [private]";
	      }
	    a_out << endl;
	    a_entry.tree_dump (a_out, "", indent_oss.str ());
	  }
      }

      {
	int rank = 0;
	a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)
	     << "Ordered entries      : ";
	if (ordered_entries_.size () == 0) 
	  {
	    a_out << "<empty>"; 
	  }
	else
	  {
	    a_out << "[" << ordered_entries_.size () << "]";
	  }
	a_out << endl;
	for (entries_ordered_col_t::const_iterator i = ordered_entries_.begin ();
	     i != ordered_entries_.end () ;
	     i++) 
	  {
	    const entry * p_entry = *i;
	    a_out << indent;
	    ostringstream indent_oss;
	    indent_oss << indent;
	    entries_ordered_col_t::const_iterator j = i; 
	    j++;
	    a_out << du::i_tree_dumpable::inherit_skip_tag (a_inherit);
	    indent_oss << du::i_tree_dumpable::inherit_skip_tag (a_inherit);
	    if (j == ordered_entries_.end ()) 
	      {
		a_out << du::i_tree_dumpable::last_tag;
		indent_oss << du::i_tree_dumpable::inherit_skip_tag (a_inherit);
	      }
	    else 
	      {
		a_out << du::i_tree_dumpable::tag;
		indent_oss << du::i_tree_dumpable::skip_tag;
	      }
	    string key = p_entry->get_key ();
	    a_out << "Entry [rank=" << rank << "] : " << '"' << key << '"';
	    if (properties::key_is_private (key))
	      {
		a_out << " [private]";
	      }
	    a_out << endl;
	    rank++;
	  }
      }
 
      return;
    }

  } // end of namespace utils 

} // end of namespace datatools


// end of multi_properties.cc
