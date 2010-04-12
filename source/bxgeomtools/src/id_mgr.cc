// -*- mode: c++ ; -*- 
/* id_mgr.cc
 */

#include <geomtools/id_mgr.h>

namespace geomtools {

  using namespace std;

  const string id_mgr::CATEGORY_KEY_LABEL = "category";
  const string id_mgr::TYPE_META_LABEL     = "type";

  id_mgr::category_info::category_info ()
  {
    category = "";
    type = geom_id::INVALID_TYPE;
    extends = "";
    inherits = "";
  }

  size_t id_mgr::category_info::get_depth () const
  {
    return addresses.size ();
  }

  void id_mgr::category_info::tree_dump (ostream & out_, 
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
	 << "Category  : \"" <<  category << "\"" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Type      : " <<  type  << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Inherits  : \"" <<  inherits << "\"" << endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "Extends   : \"" <<  extends << "\"" << endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
	 << "Addresses: [" << get_depth () << "]";
    for (int i = 0; i < addresses.size (); i++)
      {
	out_ << ' ' << '"' << addresses[i] << '"';
      }
    out_ << endl;
    return;
  }

  void id_mgr::category_info::dump (ostream & out_) const
  {
    tree_dump (out_, "id_mgr::category_info: ");
  }

  void id_mgr::category_info::make (geom_id & id_) const
  {
    id_.reset ();
    id_.make (type, get_depth ());
  }

  /***************************************/
  
  bool id_mgr::is_debug () const
  {
    return __debug;
  }
  
  void id_mgr::set_debug (bool new_value_)
  {
    __debug = new_value_;
  }
  
  const id_mgr::categories_by_type_col_t & id_mgr::categories_by_type () const
  {
    return __categories_by_type;
  }

  const id_mgr::categories_by_name_col_t & id_mgr::categories_by_name () const
  {
    return __categories_by_name;
  }
  
  // ctor:
  id_mgr::id_mgr ()
  {
    __debug = false;
  }
  
  // dtor:
  id_mgr::~id_mgr ()
  {
  }
  
  void id_mgr::init_from (const datatools::utils::multi_properties & mp_)
  {
    bool devel = false;
    //devel = true;
    if (mp_.get_key_label () != id_mgr::CATEGORY_KEY_LABEL)
      {
	ostringstream message;
	message << "id_mgr::init_from: Key label '" << mp_.get_key_label ()
		<< "' is not valid !";
	throw runtime_error (message.str ());
      }

    if (mp_.get_meta_label () != id_mgr::TYPE_META_LABEL)
      {
	ostringstream message;
	message << "id_mgr::init_from: Meta label '" << mp_.get_meta_label ()
		<< "' is not valid !";
	throw runtime_error (message.str ());
      }

    using namespace datatools::utils;
    for (multi_properties::entries_ordered_col_t::const_iterator i =
	   mp_.ordered_entries ().begin ();
	 i != mp_.ordered_entries ().end ();
	 i++)
      {
	const multi_properties::entry & the_entry = *(*i);
	if (devel) 
	  {
	    the_entry.tree_dump (clog, "Entry:", "DEVEL: id_mgr::init_from: ");
	  }

	const string & category = the_entry.get_key ();
	const string & type_meta = the_entry.get_meta ();
	const properties & props = the_entry.get_properties ();

	if (__categories_by_name.find (category) != __categories_by_name.end ())
	  {
	    ostringstream message;
	    message << "id_mgr::init_from: Category '" << category << "' already exists !";
	    throw runtime_error (message.str ());
	  }
	if (type_meta.empty ())
	  {
	    ostringstream message;
	    message << "id_mgr::init_from: Missing '" << TYPE_META_LABEL << "' info !";
	    throw runtime_error (message.str ());
	  }
	int type = geom_id::INVALID_TYPE;
	{
	  istringstream iss (type_meta);
	  iss >> type;
	  if (! iss)
	    {
	      ostringstream message;
	      message << "id_mgr::init_from: Invalid format for type '" << type_meta << "' !";
	      throw runtime_error (message.str ());
	    }
	  if (type < 0)
	    {
	      ostringstream message;
	      message << "id_mgr::init_from: Invalid value for type '" << type << "' !";
	      throw runtime_error (message.str ());
	    }
	}

	if (__categories_by_type.find (type) != __categories_by_type.end ())
	  {
	    ostringstream message;
	    message << "id_mgr::init_from: Type '" << type << "' already exists !";
	    throw runtime_error (message.str ());
	  }

	category_info cat_entry;
	cat_entry.category = category;
	cat_entry.type     = type;
	if (props.has_key ("addresses"))
	  {
	    if (props.has_key ("addresses"))
	    props.fetch ("addresses", cat_entry.addresses);
	  }
	else
	  {
	    if (props.has_key ("inherits"))
	      {
		if (devel)
		  {
		    clog << "******* DEVEL: inherits..." << endl;
		  }
		string inherits = props.fetch_string ("inherits");
		categories_by_name_col_t::const_iterator i_inherits 
		  = __categories_by_name.find (inherits);
		if (devel)
		  {
		    clog << "DEVEL: id_mgr::init_from: "
			 << "inherits='" << inherits << "'"
			 << endl;
		  }
		if (i_inherits == __categories_by_name.end ())
		  {
		    ostringstream message;
		    message << "id_mgr::init_from: Category '" << inherits << "' does not exist !";
		    throw runtime_error (message.str ());
		  }
		const category_info & inherits_entry = i_inherits->second;
		cat_entry.inherits = inherits;
		cat_entry.addresses = inherits_entry.addresses;
	      }
	    else if (props.has_key ("extends"))
	      {
		string extends = props.fetch_string ("extends");
		if (devel)
		  {
		    clog << "******* DEVEL: extends category '" << extends << "'" << endl;
		  }
		categories_by_name_col_t::const_iterator i_extends 
		  = __categories_by_name.find (extends);
		if (i_extends == __categories_by_name.end ())
		  {
		    ostringstream message;
		    message << "id_mgr::init_from: Category '" << extends << "' does not exist !";
		    throw runtime_error (message.str ());
		  }
		const category_info & extends_entry = i_extends->second;
		if (devel)
		  {
		    clog << "******* DEVEL: found category: " << endl;
		    extends_entry.dump ();
		  }
		cat_entry.addresses = extends_entry.addresses;
		if (! props.has_key ("by"))
		  {
		    ostringstream message;
		    message << "id_mgr::init_from: Missing '" << "by" << "' address extension property !";
		    throw runtime_error (message.str ());
		  }
		cat_entry.extends = extends;
		vector<string> by;
		props.fetch ("by", by);
		for (int i = 0; i < by.size (); i++)
		  {
		    cat_entry.addresses.push_back (by[i]);
		  }
		if (devel)
		  {
		    cat_entry.dump (clog);
		  }
	      }
	  }
	if (devel)
	  {
	    clog << "******* DEVEL: " << endl;
	    cat_entry.dump (clog);
	  }
	__categories_by_name[cat_entry.category] = cat_entry;
	categories_by_name_col_t::const_iterator i_entry 
		  = __categories_by_name.find (cat_entry.category);
	__categories_by_type[cat_entry.type] = &(i_entry->second);
      } // for
  }

  void id_mgr::load (const string & filename_)
  {
    datatools::utils::multi_properties mp (CATEGORY_KEY_LABEL, TYPE_META_LABEL);
    string fn = filename_;
    datatools::utils::fetch_path_with_env (fn);
    mp.read (fn);
    init_from (mp);
  }

  void id_mgr::tree_dump (ostream & out_, 
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
	 << "Debug  : " <<  __debug << endl;

    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
	 << "Categories      : ";
    if (__categories_by_name.size () == 0) 
      {
	out_ << "<empty>"; 
      }
    else
      {
	out_ << "[" << __categories_by_name.size () << "]";
      }
    out_ << endl;
    for (categories_by_name_col_t::const_iterator i = __categories_by_name.begin ();
	 i != __categories_by_name.end () ;
	 i++) 
      {
	const string & name = i->first;
	const category_info & a_entry = i->second;
	out_ << indent;
	ostringstream indent_oss;
	indent_oss << indent;
	categories_by_name_col_t::const_iterator j = i; 
	j++;
	out_ << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	if (j == __categories_by_name.end ()) 
	  {
	    out_ << du::i_tree_dumpable::last_tag;
	    indent_oss << du::i_tree_dumpable::inherit_skip_tag (inherit_);
	  }
	else 
	  {
	    out_ << du::i_tree_dumpable::tag;
	    indent_oss << du::i_tree_dumpable::skip_tag;
	  }
	out_ << "Category : " << '"' << name << '"';
	out_ << endl;
	a_entry.tree_dump (out_, "", indent_oss.str ());
      }
  }

  bool id_mgr::has_category_info (int type_) const
  {
    return __categories_by_type.find (type_) != __categories_by_type.end ();
  }
  
  bool id_mgr::has_category_info (const string & name_) const
  {
    return __categories_by_name.find (name_) != __categories_by_name.end ();
  }
  
  const id_mgr::category_info & 
  id_mgr::get_category_info (int type_) const
  {
    categories_by_type_col_t::const_iterator found = __categories_by_type.find (type_);
    if (found == __categories_by_type.end ())
      {
	ostringstream message;
	message << "id_mgr::get_category_info: " 
		<< "Unregistered category with type '" << type_ << "' !";
	throw runtime_error (message.str ());
      }
    return *(found->second);
  }
  
  const id_mgr::category_info & 
  id_mgr::get_category_info (const string & name_) const
  {
    categories_by_name_col_t::const_iterator found = __categories_by_name.find (name_);
    if (found == __categories_by_name.end ())
      {
	ostringstream message;
	message << "id_mgr::get_category_info: " 
		<< "Unregistered category with name '" << name_ << "' !";
	throw runtime_error (message.str ());
      }
    return found->second;
  }
 
} // end of namespace geomtools

// end of id_mgr.cc
