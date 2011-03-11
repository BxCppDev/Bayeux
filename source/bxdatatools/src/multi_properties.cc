// multi_properties.cc
  
#include <datatools/utils/multi_properties.h>

BOOST_CLASS_EXPORT_IMPLEMENT(datatools::utils::multi_properties) 

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

    const string & multi_properties::get_serial_tag () const
    {
      return multi_properties::SERIAL_TAG;
    }

    const string & multi_properties::entry::get_serial_tag () const
    {
      return multi_properties::entry::SERIAL_TAG;
    }

    const properties & multi_properties::entry::get_properties () const
    {
      return __properties;
    }

    properties & multi_properties::entry::get_properties ()
    {
      return __properties;
    }

    const string & multi_properties::entry::get_key () const
    {
      return __key;
    }

    void multi_properties::entry::set_key (const string & key_)
    {
      if (key_.empty ())
	{
	  ostringstream message;
	  message << "multi_properties::entry::set_key: Missing key !";
	  throw runtime_error (message.str ());
	}
      __key = key_;
    }

    const string & multi_properties::entry::get_meta () const
    {
     return __meta;
    }

    void multi_properties::entry::set_meta (const string & meta_)
    {
      __meta = meta_;
    }

    bool multi_properties::entry::has_meta () const
    {
      return (! __meta.empty ());
    }

    multi_properties::entry::entry (const string & key_, 
				    const string & meta_)
    {
      if (! key_.empty ())
	{
	  set_key (key_);
	}
      set_meta (meta_);
    }

    multi_properties::entry::~entry ()
    {
      __properties.clear ();
    }
 	
    void multi_properties::entry::tree_dump (ostream & out_, 
					     const string & title_,
					     const string & indent_,
					     bool inherit_) const
    {
      namespace du = datatools::utils;
      string indent;
      if (! indent_.empty ()) indent = indent_;
      if (! title_.empty ()) 
	{
	  out_ << indent << title_ << endl;
	}

      out_ << indent << du::i_tree_dumpable::tag 
	   << "Key         : \"" <<  __key << "\"" << endl;

      out_ << indent << du::i_tree_dumpable::tag 
	   << "Meta        : \"" <<  __meta << "\"" << endl;
      
      {
	out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
	     << "Properties : ";
	if ( __properties.size () == 0) 
	  {
	    out_ << "<empty>"; 
	  }
	out_ << endl;
	{
	  ostringstream indent_oss;
	  indent_oss << indent;
	  indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	  __properties.tree_dump (out_,"",indent_oss.str ());
	}
      }
      return;
    }


    /****************************************************************/

    bool multi_properties::is_debug () const
    {
      return __debug;
    }
    
    void multi_properties::set_debug (bool debug_)
    {
      __debug = debug_;
    }

    const string & multi_properties::get_description () const
    {
      return __description;
    }

    void multi_properties::set_description (const string & description_)
    {
      __description = description_;
    }

    void multi_properties::set_key_label (const string & key_label_)
    {
      if (key_label_.empty ())
	{
	  throw runtime_error ("multi_properties::set_key_label: Empty key label is not allowed !");
	}
      __key_label = key_label_;
    }

    const string & multi_properties::get_key_label () const
    {
      return __key_label;
    }

    void multi_properties::set_meta_label (const string & meta_label_)
    {
     __meta_label = meta_label_;
    }

    const string & multi_properties::get_meta_label () const
    {
      return __meta_label;
    }
	
    size_t multi_properties::size () const
    {
      if (__ordered_entries.size () != __entries.size ()) 
	{
	  throw runtime_error ("multi_properties::size: Containers are broken !");
	}
      return __entries.size ();
    }

    void multi_properties::reset ()
    {
      __key_label = DEFAULT_KEY_LABEL;
      __meta_label = DEFAULT_META_LABEL;
      __ordered_entries.clear ();
      __entries.clear ();
    }

    void multi_properties::clear ()
    {
      reset ();
    }

    const multi_properties::entries_col_t & multi_properties::entries () const
    {
      return __entries;
    }

    const multi_properties::entries_ordered_col_t & 
    multi_properties::ordered_entries () const
    {
      return __ordered_entries;
    }

    // ctor:
    multi_properties::multi_properties (const string & key_label_,
					const string & meta_label_,
					const string & description_,
					bool debug_)
    { 
      __debug = debug_;
      __key_label = DEFAULT_KEY_LABEL;
      __meta_label = DEFAULT_META_LABEL;
      if (! key_label_.empty ()) set_key_label (key_label_);
      //if (! meta_label_.empty ()) 
      set_meta_label (meta_label_);
      set_description (description_);
    }

    // dtor:
    multi_properties::~multi_properties ()
    {
      reset ();
    }

    bool multi_properties::has_key (const string & key_) const
    {
      entries_col_t::const_iterator found = __entries.find (key_);
      return found != __entries.end ();
    }

    const multi_properties::entry & 
    multi_properties::get (const string & key_) const
    {
      entries_col_t::const_iterator found = __entries.find (key_);
      if (found == __entries.end ())
	{
	  ostringstream message;
	  message << "multi_properties::get: Key '"
		  <<  key_ << "' is not used !";
	  throw runtime_error (message.str ());
	}
      return found->second;
    }
    
    multi_properties::entry & multi_properties::get (const string & key_)
    {
      entries_col_t::iterator found = __entries.find (key_);
      if (found == __entries.end ())
	{
	  ostringstream message;
	  message << "multi_properties::get: Key '"
		  <<  key_ << "' is not used !";
	  throw runtime_error (message.str ());
	}
      return found->second;
    }

    void multi_properties::__remove (const string & key_)
    {
      bool devel = __debug;
      //devel = true;
      entries_ordered_col_t::iterator found = __ordered_entries.end ();
       if (devel)
	{
	  clog << "DEVEL: multi_properties::__add: "
	       << "Removing key '" << key_ << "'";
	  clog << endl;
	}
       for (entries_ordered_col_t::iterator i = __ordered_entries.begin ();
	   i != __ordered_entries.end ();
	   i++)
	{
	  entry * e = *i;
	  if (e->get_key () == key_)
	    {
	      found = i;
	      break;
	    }
	}
      if (found != __ordered_entries.end ())
	{
	  __ordered_entries.erase (found);
	  if (devel)
	    {
	      clog << "DEVEL: multi_properties::__add: "
		   << "Removing ordered key '" << key_ << "'...";
	      clog << endl;
	    }
	}
      size_t n = __entries.erase (key_);
      return;
    }

    void multi_properties::remove (const string & key_)
    {
      entries_col_t::iterator found = __entries.find (key_);
      if (found == __entries.end ())
	{
	  ostringstream message;
	  message << "multi_properties::remove: Key '"
		  <<  key_ << "' is not used !";
	  throw runtime_error (message.str ());
	}
      multi_properties::__remove (key_);
      return;
    }

    void multi_properties::__add (const string & key_, 
				  const string & meta_)
    {
      bool devel = __debug;
      //devel = true;
      if (__entries.find (key_) != __entries.end ())
	{
	  ostringstream message;
	  message << "multi_properties::__add: Key '"
		  <<  key_ << "' is already used !";
	  throw runtime_error (message.str ());
	}
      if (devel)
	{
	  clog << "DEVEL: multi_properties::__add: "
	       << "Adding key '" << key_ << "'";
	  if (! meta_.empty()) clog << " with meta '" << meta_ << "'";
	  clog << endl;
	}
      __entries[key_] = entry (key_, meta_);
      __ordered_entries.push_back (&__entries[key_]);
      return;
    }

    void multi_properties::add (const string & key_, 
				const string & meta_)
    {
      __add (key_, meta_);
      return;
    }

    void multi_properties::add (const string & key_, 
				const properties & props_)
    {
      __add (key_);
      __entries[key_].get_properties () = props_;
      return;
    }

    void multi_properties::add (const string & key_, 
				const string & meta_,
				const properties & props_)
    {
      __add (key_, meta_);
      __entries[key_].get_properties () = props_;
      return;
    }

    void multi_properties::write (const string & filename_, 
				  bool header_footer_,
				  bool write_private_) const
    {
      bool devel = multi_properties::g_debug;
      if (devel) 
	{
	  clog << "DEVEL: multi_properties::write: Entering..." << endl;
	  clog << "DEVEL: multi_properties::write: Filename : '"
	       << filename_ << "'." << endl;
	}
      ofstream fout (filename_.c_str ());
      if (! fout)
	{
	  ostringstream message;
	  message << "multi_properties::write: Cannot open file '"
		  <<  filename_ << "'!";
	  throw runtime_error (message.str ());
	}
 
      properties::config pcfg (true, 
			       properties::config::MODE_BARE, 
			       true);

      if (header_footer_)
	{
	  fout << "# List of multi-properties (datatools::utils::multi_properties):" << endl;
	  fout << endl;
	}

      fout << "#@description " << get_description () << endl;
      fout << "#@key_label   " << '"' << get_key_label () << '"' << endl;
      fout << "#@meta_label  " << '"' << get_meta_label ()<< '"' << endl;
      fout << endl;

      for (entries_col_t::const_iterator i = __entries.begin ();
	   i != __entries.end ();
	   i++)
	{
	  const string & name = i->first;
	  const entry & an_entry = i->second;

	  if (devel) clog << "DEVEL: multi_properties::write: "
			  << "write_private = '" << write_private_ << "'" << endl;
	  
	  if (! write_private_)
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

      if (header_footer_)
	{
	  fout << "# End of list of multi-properties." << endl;
	}
      return;
    }
    
    void multi_properties::read (const string & filename_,
				 bool skip_private_)
    {
      bool devel = false;
      if (devel) 
	{
	  clog << "DEVEL: multi_properties::read: Entering..." << endl;
	  clog << "DEVEL: multi_properties::read: Filename : '"
	       << filename_ << "'." << endl;
	}
      ifstream fin (filename_.c_str ());
      if (! fin)
	{
	  ostringstream message;
	  message << "multi_properties::read: Cannot open file '"
		  <<  filename_ << "'!";
	  throw runtime_error (message.str ());
	}

      __read (fin, skip_private_);
      return;
    } 

    void multi_properties::__read (istream & in_,
				   bool skip_private_)
    {
      bool devel = multi_properties::g_debug;
      //devel = true;
      if (devel)
	{
	  clog << "DEVEL: multi_properties::__read: Entering..." << endl;
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
			      throw runtime_error ("multi_properties::__read: Unquoted value for 'key_label'");
			    }
			  string tmp;
			  getline (iss, tmp);
			  if (! key_label.empty ()) 
			    {
			      mprop_key_label = key_label;
			      if (__key_label.empty ())
				{
				  set_key_label (mprop_key_label);
				}
			      else
				{
				  if (__key_label != mprop_key_label)
				    {
				      ostringstream message;
				      message << "multi_properties::__read: Incompatible key label '"
					      << mprop_key_label << "' with setup '" << __key_label << "' !";
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
			      throw runtime_error ("multi_properties::__read: Unquoted value for 'meta_label'");
			    }
			  string tmp;
			  getline (iss, tmp);
			  if (meta_label.empty ()) 
			    {
			      if (! __meta_label.empty ())
				{
				  ostringstream message;
				  message << "multi_properties::__read: " 
					  << "Missing meta label with setup '" << __meta_label << "' !";
				  throw runtime_error (message.str ());
				}
			    }
			  else
			    {
			      mprop_meta_label = meta_label;
			      if (__meta_label.empty ())
				{
				  set_meta_label (mprop_meta_label);
				}
			      else
				{
				  if (__meta_label != mprop_meta_label)
				    {
				      ostringstream message;
				      message << "multi_properties::__read: Incompatible meta label '"
					      << mprop_meta_label << "' with setup '" << __meta_label << "' !";
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
			  message << "multi_properties::__read: Incompatible key label '"
				  << key_label << "' with setup '" << __key_label << "' !";
			  throw runtime_error (message.str ());
			}
		      if (! properties::config::read_quoted_string (iss, new_key)) 
			{
			  ostringstream message;
			  message << "multi_properties::__read: "
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
				  message << "multi_properties::__read: Incompatible meta label '"
					  << meta_label << "' with setup '" << __meta_label << "' !";
				  throw runtime_error (message.str ());
				}
			      if (! properties::config::read_quoted_string (iss, new_meta)) 
				{
				  ostringstream message;
				  message << "multi_properties::__read: "
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
				  message << "multi_properties::__read: "
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
			  message << "multi_properties::__read: "
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
			  clog << "DEVEL: multi_properties::__read: " 
			       << "Append line " << "'"  << line << "'" << endl;
			}
		      current_block_oss << line << endl;
		    }
		  if (devel)
		    {
		      clog << "DEVEL: multi_properties::__read: " 
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
		  if (skip_private_)
		    {
		      if (properties::key_is_private (current_key))
			{
			  if (devel)
			    {
			      clog << "DEVEL: multi_properties::__read: Skip loading of '" 
				   << current_key << "'" << endl;
			    }
			  load_it = false;
			}
		    }

		  if (load_it)
		    {
		      if (devel)
			{
			  clog << "DEVEL: multi_properties::__read: Add '" 
			       << current_key << "'" << endl;
			}
		      add (current_key, current_meta);
		      multi_properties::entry & e = get (current_key);
		      properties::config pcr;
		      if (devel)
			{
			  clog << "DEVEL: multi_properties::__read: Block:" << endl;
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
    } /* end of multi_properties::__read */
 
    void multi_properties::dump (ostream & out_) const
    {
      tree_dump (out_, "datatools::utils::multi_properties:");
      return;
    }
	
    void multi_properties::tree_dump (ostream & out_, 
				      const string & title_,
				      const string & indent_,
				      bool inherit_) const
    {
      namespace du = datatools::utils;
      string indent;
      if (! indent_.empty ()) indent = indent_;
      if (! title_.empty ()) 
	{
	  out_ << indent << title_ << endl;
	}
     
      if (! __description.empty ()) 
	{
	  out_ << indent << du::i_tree_dumpable::tag 
	       << "Description  : " <<  get_description () << endl;
	}

      if (! __key_label.empty ()) 
	{
	  out_ << indent << du::i_tree_dumpable::tag 
	       << "Key label    : \"" <<  get_key_label () << "\"" << endl;
	}

      if (! __meta_label.empty ()) 
	{
	  out_ << indent << du::i_tree_dumpable::tag 
	       << "Meta label   : \"" <<  get_meta_label () << "\"" << endl;
	}

      {
	out_ << indent << du::i_tree_dumpable::tag
	     << "Entries      : ";
	if (__entries.size () == 0) 
	  {
	    out_ << "<empty>"; 
	  }
	else
	  {
	    out_ << "[" << __entries.size () << "]";
	  }
	out_ << endl;
	for (entries_col_t::const_iterator i = __entries.begin ();
	     i != __entries.end () ;
	     i++) 
	  {
	    const string & key = i->first;
	    const entry & a_entry = i->second;
	    out_ << indent;
	    ostringstream indent_oss;
	    indent_oss << indent;
	    entries_col_t::const_iterator j = i; 
	    j++;
	    //out_ << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	    //indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	    out_ << du::i_tree_dumpable::skip_tag;
	    indent_oss << du::i_tree_dumpable::skip_tag;
	    if (j == __entries.end ()) 
	      {
		out_ << du::i_tree_dumpable::last_tag;
		indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	      }
	    else 
	      {
		out_ << du::i_tree_dumpable::tag;
		indent_oss << du::i_tree_dumpable::skip_tag;
	      }
	    out_ << "Entry : " << '"' << key << '"';
	    if (properties::key_is_private (key))
	      {
		out_ << " [private]";
	      }
	    out_ << endl;
	    a_entry.tree_dump (out_, "", indent_oss.str ());
	  }
      }

      {
	int rank = 0;
	out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
	     << "Ordered entries      : ";
	if (__ordered_entries.size () == 0) 
	  {
	    out_ << "<empty>"; 
	  }
	else
	  {
	    out_ << "[" << __ordered_entries.size () << "]";
	  }
	out_ << endl;
	for (entries_ordered_col_t::const_iterator i = __ordered_entries.begin ();
	     i != __ordered_entries.end () ;
	     i++) 
	  {
	    const entry * p_entry = *i;
	    out_ << indent;
	    ostringstream indent_oss;
	    indent_oss << indent;
	    entries_ordered_col_t::const_iterator j = i; 
	    j++;
	    out_ << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	    indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	    if (j == __ordered_entries.end ()) 
	      {
		out_ << du::i_tree_dumpable::last_tag;
		indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	      }
	    else 
	      {
		out_ << du::i_tree_dumpable::tag;
		indent_oss << du::i_tree_dumpable::skip_tag;
	      }
	    string key = p_entry->get_key ();
	    out_ << "Entry [rank=" << rank << "] : " << '"' << key << '"';
	    if (properties::key_is_private (key))
	      {
		out_ << " [private]";
	      }
	    out_ << endl;
	    rank++;
	  }
      }
 
      return;
    }

  } // end of namespace utils 

} // end of namespace datatools

// end of multi_properties.cc
