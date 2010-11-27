// -*- mode: c++ ; -*-
/* id_mgr.cc
 */

#include <geomtools/id_mgr.h>

namespace geomtools {

  using namespace std;

  const string id_mgr::CATEGORY_KEY_LABEL = "category";
  const string id_mgr::TYPE_META_LABEL    = "type";

  bool id_mgr::g_devel = false;

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

  size_t id_mgr::category_info::get_by_depth () const
  {
    return extends_by.size ();
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
	 << "Category  : \"" << category << "\"" << endl;
    out_ << indent << du::i_tree_dumpable::tag
	 << "Type      : " << type << endl;
    if (! inherits.empty ())
      {
	out_ << indent << du::i_tree_dumpable::tag
	     << "Inherits  : \"" << inherits << "\"" << endl;
      }
    if (! ancestors.empty ())
      {
	out_ << indent << du::i_tree_dumpable::tag
	     << "Ancestors ["
	     <<  ancestors.size () << "] :";
	for (int i = 0; i < ancestors.size (); i++)
	  {
	    out_ << ' ' << '"' << ancestors[i] << '"';
	  }
	out_ << endl;
     }
    if (! extends.empty ())
      {
	out_ << indent << du::i_tree_dumpable::tag
	     << "Extends   : \"" << extends << "\" by ["
	     <<  extends_by.size () << "] : ";
	for (int i = 0; i < extends_by.size (); i++)
	  {
	    out_ << ' ' << '"' << extends_by[i] << '"';
	  }
	out_ << endl;
     }
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
	 << "Addresses [" << get_depth () << "] :";
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

  void id_mgr::category_info::create (geom_id & id_) const
  {
    id_.reset ();
    id_.make (type, get_depth ());
  }

  /*
  void id_mgr::category_info::make (geom_id & id_,
				    uint32_t si0_,
				    uint32_t si1_,
				    uint32_t si2_,
				    uint32_t si3_,
				    uint32_t si4_,
				    uint32_t si5_,
				    uint32_t si6_,
				    uint32_t si7_,
				    uint32_t si8_,
				    uint32_t si9_) const
  {

  }
  */

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
    bool devel = g_devel;
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
		    clog << "DEVEL: id_mgr::init_from: inherits..." << endl;
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
		cat_entry.ancestors = inherits_entry.ancestors;
		cat_entry.add_ancestor (inherits);
	      }
	    else if (props.has_key ("extends"))
	      {
		string extends = props.fetch_string ("extends");
		if (devel)
		  {
		    clog << "DEVEL: id_mgr::init_from: "
			 << "extends category '" << extends << "'" << endl;
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
		    clog << "DEVEL: id_mgr::init_from: "
			 << "found category: " << endl;
		    extends_entry.dump ();
		  }
		cat_entry.addresses = extends_entry.addresses;
		cat_entry.ancestors = extends_entry.ancestors;
		cat_entry.add_ancestor (extends);
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
		    cat_entry.extends_by.push_back (by[i]);
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
	    clog << "DEVEL: id_mgr::init_from: " << endl;
	    cat_entry.dump (clog);
	  }
	__categories_by_name[cat_entry.category] = cat_entry;
	categories_by_name_col_t::const_iterator i_entry
	  = __categories_by_name.find (cat_entry.category);
	__categories_by_type[cat_entry.type] = &(i_entry->second);
      } // for
    return;
  }

  void id_mgr::load (const string & filename_)
  {
    datatools::utils::multi_properties mp (CATEGORY_KEY_LABEL, TYPE_META_LABEL);
    string fn = filename_;
    datatools::utils::fetch_path_with_env (fn);
    mp.read (fn);
    init_from (mp);
  }

  void id_mgr::reset ()
  {
    __categories_by_type.clear ();
    __categories_by_name.clear ();
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

  bool id_mgr::check_inheritance (const geom_id & mother_id_,
				  const geom_id & id_) const
  {
    categories_by_type_col_t::const_iterator found
      = __categories_by_type.end ();

    // check geom ID:
    int type = id_.get_type ();
    found = __categories_by_type.find (type);
    if (found == __categories_by_type.end ())
      {
	ostringstream message;
	message << "id_mgr::check_inheritance: "
		<< "No type '" << type << "' !";
	  throw runtime_error (message.str ());
      }
    const category_info & ci = *(found->second);

    // check mother geom ID:
    int mother_type = mother_id_.get_type ();
    found = __categories_by_type.find (mother_type);
    if (found == __categories_by_type.end ())
      {
	ostringstream message;
	message << "id_mgr::check_inheritance: "
		<< "No mother type '" << mother_type << "' !";
	throw runtime_error (message.str ());
      }
    const category_info & mci = *(found->second);
    if (ci.has_ancestor (mci.get_category ())) return true;
    /*
    if (ci.is_inherited ())
      {
	return (ci.get_inherits () == mci.get_category ());
      }
    else
      {
	if (ci.is_extension ())
	  {
	    return (ci.get_extends () == mci.get_category ());
	  }
      }
    */
    return false;
  }

  void id_mgr::extract (const geom_id & child_id_, geom_id & mother_id_) const
  {
    if (! check_inheritance (mother_id_, child_id_))
      {
	//clog << "DEVEL: id_mgr::extract: NO !" << endl;
	// if child ID does not inherit/extend the mother ID:
	mother_id_.reset_address ();
	return;
      }
    mother_id_.extract_from (child_id_);
    return;
  }

  void id_mgr::make_id (geom_id & id_,
			uint32_t address_) const
  {
    id_.set (0, address_);
    return;
  }

  void id_mgr::make_id (const string & category_, geom_id & id_) const
  {
    id_.reset ();
    if (! has_category_info (category_))
      {
	ostringstream message;
	message << "id_mgr::make_id: "
		<< "No category '" << category_ << "' !";
	throw runtime_error (message.str ());
      }
    const category_info & ci = get_category_info (category_);
    id_.set_type (ci.get_type ());
    id_.set_depth (ci.get_depth ());
    return;
  }

  bool id_mgr::is_category (const geom_id & id_,
			    const string & category_) const
  {
    int id_type = id_.get_type ();
    const category_info & ci = get_category_info (id_type);
    return ci.get_category () == category_;
  }

  bool id_mgr::has (const geom_id & id_, const string & what_) const
  {
    int id_type = id_.get_type ();
    categories_by_type_col_t::const_iterator found =
      __categories_by_type.find (id_type);
    const category_info & ci = *found->second;
    int i = -1;
    for (int j = 0; j < ci.addresses.size (); j++)
      {
	if (ci.addresses[j] == what_)
	  {
	    return true;
	  }
      }
    return false;
  }

  const string & id_mgr::get_category (const geom_id & id_) const
  {
    int id_type = id_.get_type ();
    categories_by_type_col_t::const_iterator found =
      __categories_by_type.find (id_type);
    if (found == __categories_by_type.end ())
      {
	ostringstream message;
	message << "id_mgr::get_category: "
		<< "No category associated to ID '" << id_ << "' !";
	throw runtime_error (message.str ());
      }
    const category_info & ci = *found->second;
    return ci.get_category ();
  }


  int id_mgr::get (const geom_id & id_, const string & what_) const
  {
    int id_type = id_.get_type ();
    categories_by_type_col_t::const_iterator found =
      __categories_by_type.find (id_type);
    const category_info & ci = *found->second;
    int i = -1;
    for (int j = 0; j < ci.addresses.size (); j++)
      {
	if (ci.addresses[j] == what_)
	  {
	    i = j;
	    break;
	  }
      }
    if (i >= 0)
      {
	return id_.get (i);
      }
    else
      {
	ostringstream message;
	message << "id_mgr::get: "
		<< "Invalid address label '" << what_ << "' "
		<< "for category '" << ci.category << "' !";
	throw runtime_error (message.str ());
      }
    return geom_id::INVALID_ADDRESS;
  }

  void id_mgr::set (geom_id & id_, const string & what_, uint32_t value_) const
  {
    int id_type = id_.get_type ();
    categories_by_type_col_t::const_iterator found =
      __categories_by_type.find (id_type);
    const category_info & ci = *found->second;
    int i = -1;
    for (int j = 0; j < ci.addresses.size (); j++)
      {
	//clog << "DEVEL: id_mgr::set: what=" << what_ << endl;
	if (ci.addresses[j] == what_)
	  {
	    i = j;
	    //clog << "DEVEL: id_mgr::set: break!" << endl;
	    break;
	  }
      }
    if (i >= 0)
      {
	return id_.set (i, value_);
      }
    else
      {
	ostringstream message;
	message << "id_mgr::set: "
		<< "Invalid address label '" << what_ << "' "
		<< "for category '" << ci.category << "' !";
	throw runtime_error (message.str ());
      }
    return;
  }

  string id_mgr::id_to_human_readable_format_string (const geom_id & id_) const
  {
    string s;
    id_to_human_readable_format_string (id_, s);
    return s;
  }

  void id_mgr::id_to_human_readable_format_string (const geom_id & id_, string & format_) const
  {
    ostringstream oss;
    if (! has_category_info (id_.get_type ()))
      {
        oss << "category=`" << geom_id::IO_TYPE_INVALID << "' : ";
	for (int i = 0; i < id_.get_depth (); i++)
	  {
	    oss << ' ' << "address[" << i << "]" << "=";
	    if (id_.get (i) == geom_id::INVALID_ADDRESS)
	      {
		oss << geom_id::IO_ADDRESS_INVALID;
	      }
	    else
	      {
		oss << id_.get (i);
	      }
	  }
      }
    else
      {
	const category_info & ci = get_category_info (id_.get_type ());
	oss << "category=`" << ci.category << "' : ";
	for (int i = 0; i < id_.get_depth (); i++)
	  {
	    oss << ' ' << ci.addresses[i] << "=";
	    if (id_.get (i) == geom_id::INVALID_ADDRESS)
	      {
		oss << geom_id::IO_ADDRESS_INVALID;
	      }
	    else
	      {
		oss << id_.get (i);
	      }
	  }
      }
    format_ = oss.str ();
  }

  bool id_mgr::validate_id (const geom_id & id_) const
  {
    if (! has_category_info (id_.get_type ()))
      {
	return false;
      }
    const category_info & ci = get_category_info (id_.get_type ());
    if (id_.get_depth () != ci.get_depth ())
      {
	return false;
      }
    return id_.is_valid ();
  }

  int id_mgr::compute_id_from_info (geom_id & id_,
				    const geom_id & mother_id_,
				    const string & id_info_,
				    uint32_t nitem0_,
		 		    uint32_t nitem1_,
				    uint32_t nitem2_) const
  {
    vector<uint32_t> items_index;
    if (nitem0_ != geom_id::INVALID_ADDRESS)
      {
	items_index.push_back (nitem0_);
	if (nitem1_ != geom_id::INVALID_ADDRESS)
	  {
	    items_index.push_back (nitem1_);
	    if (nitem2_ != geom_id::INVALID_ADDRESS)
	      {
		items_index.push_back (nitem2_);
	      }
	  }
      }
    return compute_id_from_info (id_, mother_id_, id_info_, items_index);
  }

  int id_mgr::compute_id_from_info (geom_id & id_,
				    const geom_id & mother_id_,
				    const string & id_info_,
				    const vector<uint32_t> & items_index_) const
  {
    bool devel = g_devel;
    //devel = true;
    // parse ID info:
    string id_info = id_info_;
    boost::trim (id_info);
    if (devel) clog << "id_mgr::compute_id_from_info: Entering..." << endl;

    if (devel) clog << "id_mgr::compute_id_from_info: "
		    << "ID info is `" << id_info << "'" << endl;
    if (id_info.size () < 5)
      {
	ostringstream message;
	message << "id_mgr::compute_id_from_info: "
		<< "Invalid syntax for ID info `"
		<< id_info_ << "' !";
	throw runtime_error (message.str ());
      }
    if (id_info[0] != '[')
      {
	ostringstream message;
	message << "id_mgr::compute_id_from_info: "
		<< "Invalid syntax for ID info `"
		<< id_info_ << "'; missing opening character !";
	throw runtime_error (message.str ());
      }
    if (id_info[id_info.size () - 1 ] != ']')
      {
	ostringstream message;
	message << "id_mgr::compute_id_from_info: "
		<< "Invalid syntax for ID info `"
		<< id_info_ << "'; missing closing character !";
	throw runtime_error (message.str ());
      }
    string core_info = id_info.substr (1, id_info.size () - 2);
    if (devel) clog << "id_mgr::compute_id_from_info: "
		    << "Core info is `" << core_info << "'" << endl;

    typedef vector<string> split_vector_type;
    split_vector_type split_vec;
    boost::split (split_vec, core_info, boost::algorithm::is_any_of (":"));
    if (split_vec.size () < 1 || split_vec.size () > 2)
      {
	ostringstream message;
	message << "id_mgr::compute_id_from_info: "
		<< "Invalid syntax for ID info `"
		<< id_info_ << "'; Invalid number of `:' separator !";
	throw runtime_error (message.str ());
      }

    // Parse the category:
    string category = split_vec[0];
    boost::trim (category);
    if (category.empty ())
      {
	ostringstream message;
	message << "id_mgr::compute_id_from_info: "
		<< "Missing category token !";
	throw runtime_error (message.str ());
      }
    if (devel) clog << "id_mgr::compute_id_from_info: "
	 << "Category is `" << category << "'" << endl;

    if (! has_category_info (category))
      {
	ostringstream message;
	message << "id_mgr::compute_id_from_info: "
		<< "Unknown category `" << category << "' !";
	throw runtime_error (message.str ());
      }
    const category_info & ci = get_category_info (category);
    if (devel) ci.tree_dump (clog, "Category info:");
    ci.create (id_);
    if (devel) clog << "id_mgr::compute_id_from_info: "
		    << "Preliminary geometry ID is `" << id_ << "'" << endl;

    // Eventually import mother ID stuff:
    size_t current_address_index = 0;
    string mother_category;
    if (mother_id_.is_valid ())
      {
	const category_info & mother_ci = get_category_info (mother_id_.get_type ());
	if (devel) mother_ci.tree_dump (clog, "Mother category info:");
	if (devel) clog << "id_mgr::compute_id_from_info: "
			<< "Mother ID `" << mother_id_ << "'" << endl;
	bool import_mother = false;
	if (ci.has_ancestor (mother_ci.get_category ()))
	  {
	    import_mother = true;
	    mother_category = mother_ci.get_category ();
	  }
	if (import_mother)
	  {
	    id_.inherits_from (mother_id_);
	    current_address_index = mother_id_.get_depth ();
	  }
      } // end of mother ID import.
    if (devel) clog << "id_mgr::compute_id_from_info: "
	 << "Preliminary geometry ID after mother import is `"
	 << id_ << "' (" << current_address_index << " addresses already set)" << endl;

    size_t number_of_remaining_addresses = id_.get_depth () - current_address_index;
    if (devel) clog << "id_mgr::compute_id_from_info: "
		    << "Number of remaining addresses = " << number_of_remaining_addresses
		    << endl;

    // Parse the address rules:
    bool parse_address = false;
    string addresses_token;
    if (split_vec.size() > 1)
      {
	addresses_token = split_vec[1];
	boost::trim (addresses_token);
	if (! addresses_token.empty ())
	  {
	    parse_address = true;
	  }
      }
    if (parse_address)
      {
	string addresses_token = split_vec[1];
	if (devel) clog << "id_mgr::compute_id_from_info: "
			<< "Addresses info is `" << addresses_token << "'" << endl;
	split_vector_type split_addr;
	boost::split (split_addr, addresses_token, boost::algorithm::is_any_of (","));
	if (split_addr.size () != number_of_remaining_addresses)
	  {
	    ci.tree_dump (clog, "Category info:");
	    clog << "id_mgr::compute_id_from_info: "
		 << "Number of remaining addresses = " << number_of_remaining_addresses << endl;
	    ostringstream message;
	    message << "id_mgr::compute_id_from_info: "
		    << "Invalid number of remaining addresses info for category `"
		    << category << "' from token `" << addresses_token << "' "
		    << " with mother ID " << mother_id_ << " (mother=`" << mother_category << "') [";
	    for (vector<uint32_t>::const_iterator ii = items_index_.begin ();
		 ii !=  items_index_.end ();
		 ii++)
	      {
		message << " " << *ii;
	      }
	    message << "] !";
	    throw runtime_error (message.str ());
	  }
	// parse each address rule:
	for (int i = 0; i < split_addr.size (); i++)
	  {
	    string add_info = split_addr[i];
	    boost::trim (add_info);
	    split_vector_type split_add_info;
	    boost::split (split_add_info, add_info, boost::algorithm::is_any_of ("=+"));
	    bool skip = false;
	    if (add_info.find ('+') != add_info.npos) skip = true;

	    if (split_add_info.size () != 2)
	      {
		ostringstream message;
		message << "id_mgr::compute_id_from_info: "
			<< "Invalid address information `" << add_info << "' !";
		throw runtime_error (message.str ());
	      }
	    string addr_label = split_add_info[0];
	    if (ci.addresses[i + current_address_index] != addr_label)
	      {
		ostringstream message;
		message << "id_mgr::compute_id_from_info: "
			<< "Invalid address label `" << addr_label << "' !";
		throw runtime_error (message.str ());
	      }
	    string addr_val_str = split_add_info[1];
	    boost::trim (addr_val_str);
	    uint32_t addr_val = 0;
	    {
	      istringstream iss (addr_val_str);
	      iss >> addr_val;
	      if (! iss)
		{
		  ostringstream message;
		  message << "id_mgr::compute_id_from_info: "
			  << "Invalid address value `" << addr_val_str << "' !";
		  throw runtime_error (message.str ());
		}
	    }
	    uint32_t address_value = addr_val;
	    if (skip)
	      {
		if (items_index_.size () > i && items_index_[i] >= 0)
		  {
		    address_value += items_index_[i];
		  }
		else
		  {
		    ostringstream message;
		    message << "id_mgr::compute_id_from_info: "
			    << "Missing address at index `" << i << "' !";
		    throw runtime_error (message.str ());
		      }
	      }
	    id_.set (i + current_address_index, address_value);
	  }
      }

    if (devel) clog << "id_mgr::compute_id_from_info: Exiting." << endl << endl;
    return EXIT_SUCCESS;
  }

} // end of namespace geomtools

// end of id_mgr.cc
