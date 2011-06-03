// -*- mode: c++; -*- 
/* id_selector.cc 
 */

#include <geomtools/id_selector.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <boost/algorithm/string.hpp>

namespace geomtools {
  

  bool id_selector::is_initialized () const
  {
    return (__cat_info != 0);
  }

  const id_mgr::category_info & id_selector::get_category_info () const
  {
    if (__cat_info == 0)
      {
 	ostringstream message;
	message << "id_selector::set_id_mgr: "
		<< "Missing category info ! ID selector is not initalized !";
	throw runtime_error (message.str ());	
     }
    return *__cat_info;
  }

  void id_selector::set_id_mgr (const id_mgr & id_mgr_)
  {
    if (is_initialized ())
      {
	ostringstream message;
	message << "id_selector::set_id_mgr: "
		<< "Selector is already initialized !";
	throw runtime_error (message.str ());	
      }
    __id_mgr = &id_mgr_;
    return;
  }

  id_selector::id_selector ()
  {
    __id_mgr = 0;
    __cat_info = 0;
  }

  id_selector::id_selector (const id_mgr & id_mgr_)
  {
    __id_mgr = &id_mgr_;
    __cat_info = 0;
  }

  id_selector::~id_selector ()
  {
    reset ();
    __cat_info = 0;
    __id_mgr = 0;
  }

  void id_selector::initialize (const string & rules_)
  {
    if (is_initialized ()) 
      {
	__cat_info = 0;
      }
    if (__id_mgr == 0)
      {
 	ostringstream message;
	message << "id_selector::initialize: Missing ID manager !";
	throw runtime_error (message.str ());
     }
    istringstream rules_iss (rules_);
    
    string cat_token;
    rules_iss >> cat_token >> ws;
    if (! rules_iss)
      {
	ostringstream message;
	message << "id_selector::initialize: Missing category rule !";
	throw runtime_error (message.str ());
      }
    typedef vector<string> split_vector_type;
    split_vector_type cat_token_vec; 
    boost::split (cat_token_vec, cat_token, boost::algorithm::is_any_of ("="));
    if (cat_token_vec.size() != 2)
      {
	ostringstream message;
	message << "id_selector::initialize: Invalid category rule format !";
	throw runtime_error (message.str ());
      }
    if (cat_token_vec[0] != "category")
      {
	ostringstream message;
	message << "id_selector::initialize: Cannot find category label !";
	throw runtime_error (message.str ());
      }
    string category = cat_token_vec[1];
    if (category.empty ())
      {
	ostringstream message;
	message << "id_selector::initialize: Missing category name label !";
	throw runtime_error (message.str ());
      }
    if (! __id_mgr->has_category_info (category))
      {
	ostringstream message;
	message << "id_selector::initialize: Unknown category name '" 
		<< category << "' !";
	throw runtime_error (message.str ());
      }

    __cat_info = &(__id_mgr->get_category_info  (category));
    size_t parsed_addr_rules = 0;
    while (! rules_iss.eof () && (parsed_addr_rules < __cat_info->get_depth ()))
      {
	string addr_token;
	rules_iss >> addr_token >> ws;
	if (! rules_iss)
	  {
	    ostringstream message;
	    message << "id_selector::initialize: Invalid address token !";
	    throw runtime_error (message.str ());
	  }
	split_vector_type addr_token_vec; 
	boost::split (addr_token_vec, addr_token, boost::algorithm::is_any_of ("="));
	if (addr_token_vec.size () != 2)
	  {
	    ostringstream message;
	    message << "id_selector::initialize: Invalid address rule format !";
	    throw runtime_error (message.str ());
	  }

	string addr_name = addr_token_vec[0];
	if (addr_name.empty ())
	  {
	    ostringstream message;
	    message << "id_selector::initialize: Cannot find address label !";
	    throw runtime_error (message.str ());
	  }
	string addr_rule = addr_token_vec[1];
	if (addr_rule.empty ())
	  {
	    ostringstream message;
	    message << "id_selector::initialize: Address rule is empty !";
	    throw runtime_error (message.str ());
	  }

	int addr_index = -1;
	for (int i = 0; i < __cat_info->addresses.size (); i++)
	  {
	    if (__cat_info->addresses[i] == addr_name)
	      {
		addr_index = i;
		break;
	      }
	  }

	if (addr_index < 0)
	  {
	    ostringstream message;
	    message << "id_selector::initialize: Cannot find address label '"
		    << addr_name << "' in category '" 
		    << category << "' !";
	    throw runtime_error (message.str ());
	  }
	if (__addr_sets.find (addr_index) != __addr_sets.end ())
	  {
	    ostringstream message;
	    message << "id_selector::initialize: Rule for address index " 
		    << addr_index << " in category '" 
		    << category << "'is already set !";
	    throw runtime_error (message.str ());	    
	  }
	address_set as;
	istringstream addr_rule_iss (addr_rule);
	addr_rule_iss >> as;
	if (! addr_rule_iss)
	  {
	    ostringstream message;
	    message << "id_selector::initialize: Cannot parse address rule '"
		    << addr_rule << "' for address '"
		    << addr_name << "' in category '" 
		    << category << "' !";
	    throw runtime_error (message.str ());
	  }
	__addr_sets[addr_index] = as;
	parsed_addr_rules++;
      }

    return;
  } 

  void id_selector::dump (ostream & out_, const string & title_) const
  {
    if (! title_.empty ())
      {
	out_ << title_ << ": " << endl;
      }
    if (! is_initialized ())
      {
	out_ << "`-- Initialized:   '" << "No" << "'" << endl;
	return;
      }
    else
      {
	out_ << "|-- Initialized:   '" << "Yes" << "'" << endl;
      }
    out_ << "|-- Category:   '" << __cat_info->get_category () << "' (type=" 
	 << __cat_info->get_type () 
	 << ", depth=" <<  __cat_info->get_depth () << ")" << endl;
     for (map<int, address_set>::const_iterator i = __addr_sets.begin ();
	 i != __addr_sets.end ();
	 i++)
      {
	map<int, address_set>::const_iterator j = i; 
	j++;
	if (j == __addr_sets.end ())
	  {
	    out_ << "`-- ";
	  }
	else
	  {
	    out_ << "|-- ";
	  }
	out_ << "Address index=" << i->first
	     << " (" << __cat_info->addresses[i->first] << ")"
	     << " : address set=" << i->second 
	     << endl;
      }
     return;
  } 

  void id_selector::reset ()
  {
    __cat_info = 0;
    __addr_sets.clear ();
  }

  bool id_selector::match (const geom_id & gid_) const
  {
    if (! is_initialized ())
      {
	ostringstream message;
	message << "id_selector::match: " 
		<< "ID selector is not initialized by any rule !";
	throw runtime_error (message.str ());	
      }
    if (gid_.get_type () != __cat_info->get_type ())
      {
	return false;
      }
    bool matched = true;
    for (map<int, address_set>::const_iterator i = __addr_sets.begin ();
	 i != __addr_sets.end ();
	 i++)
      {
	int addr_index = i->first;
	uint32_t addr = gid_.get (addr_index);
	if (! i->second.match (addr)) 
	  {
	    matched = false;
	    break;
	  }
      }
    return matched;
  }
 
} // end of namespace geomtools

// end of id_selector.cc
