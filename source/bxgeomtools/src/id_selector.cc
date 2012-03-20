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
  

  using namespace std;  

  bool id_selector::is_initialized () const
  {
    return (_cat_info_ != 0);
  }

  const id_mgr::category_info & id_selector::get_category_info () const
  {
    if (_cat_info_ == 0)
      {
        ostringstream message;
        message << "id_selector::set_id_mgr: "
                << "Missing category info ! ID selector is not initalized !";
        throw logic_error (message.str ());   
      }
    return *_cat_info_;
  }

  void id_selector::set_id_mgr (const id_mgr & id_mgr_)
  {
    if (is_initialized ())
      {
        ostringstream message;
        message << "id_selector::set_id_mgr: "
                << "Selector is already initialized !";
        throw logic_error (message.str ());   
      }
    _id_mgr_ = &id_mgr_;
    return;
  }

  id_selector::id_selector ()
  {
    _id_mgr_ = 0;
    _cat_info_ = 0;
  }

  id_selector::id_selector (const id_mgr & id_mgr_)
  {
    _id_mgr_ = &id_mgr_;
    _cat_info_ = 0;
  }

  id_selector::~id_selector ()
  {
    reset ();
    _cat_info_ = 0;
    _id_mgr_ = 0;
  }

  void id_selector::initialize (const string & rules_)
  {
    if (is_initialized ()) 
      {
        _cat_info_ = 0;
      }
    if (_id_mgr_ == 0)
      {
        ostringstream message;
        message << "id_selector::initialize: Missing ID manager !";
        throw logic_error (message.str ());
      }
    istringstream rules_iss (rules_);
    
    string cat_token;
    rules_iss >> cat_token >> ws;
    if (! rules_iss)
      {
        ostringstream message;
        message << "id_selector::initialize: Missing category rule !";
        throw logic_error (message.str ());
      }
    typedef vector<string> split_vector_type;
    split_vector_type cat_token_vec; 
    boost::split (cat_token_vec, cat_token, boost::algorithm::is_any_of ("="));
    if (cat_token_vec.size() != 2)
      {
        ostringstream message;
        message << "id_selector::initialize: Invalid category rule format !";
        throw logic_error (message.str ());
      }
    if (cat_token_vec[0] != "category")
      {
        ostringstream message;
        message << "id_selector::initialize: Cannot find category label !";
        throw logic_error (message.str ());
      }
    string category = cat_token_vec[1];
    if (category.empty ())
      {
        ostringstream message;
        message << "id_selector::initialize: Missing category name label !";
        throw logic_error (message.str ());
      }
    if (! _id_mgr_->has_category_info (category))
      {
        ostringstream message;
        message << "id_selector::initialize: Unknown category name '" 
                << category << "' !";
        throw logic_error (message.str ());
      }

    _cat_info_ = &(_id_mgr_->get_category_info  (category));
    size_t parsed_addr_rules = 0;
    while (! rules_iss.eof () && (parsed_addr_rules < _cat_info_->get_depth ()))
      {
        string addr_token;
        rules_iss >> addr_token >> ws;
        if (! rules_iss)
          {
            ostringstream message;
            message << "id_selector::initialize: Invalid address token !";
            throw logic_error (message.str ());
          }
        split_vector_type addr_token_vec; 
        boost::split (addr_token_vec, addr_token, boost::algorithm::is_any_of ("="));
        if (addr_token_vec.size () != 2)
          {
            ostringstream message;
            message << "id_selector::initialize: Invalid address rule format !";
            throw logic_error (message.str ());
          }

        string addr_name = addr_token_vec[0];
        if (addr_name.empty ())
          {
            ostringstream message;
            message << "id_selector::initialize: Cannot find address label !";
            throw logic_error (message.str ());
          }
        string addr_rule = addr_token_vec[1];
        if (addr_rule.empty ())
          {
            ostringstream message;
            message << "id_selector::initialize: Address rule is empty !";
            throw logic_error (message.str ());
          }

        int addr_index = -1;
        for (int i = 0; i < _cat_info_->addresses.size (); i++)
          {
            if (_cat_info_->addresses[i] == addr_name)
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
            throw logic_error (message.str ());
          }
        if (_addr_sets_.find (addr_index) != _addr_sets_.end ())
          {
            ostringstream message;
            message << "id_selector::initialize: Rule for address index " 
                    << addr_index << " in category '" 
                    << category << "'is already set !";
            throw logic_error (message.str ());           
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
            throw logic_error (message.str ());
          }
        _addr_sets_[addr_index] = as;
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
    out_ << "|-- Category:   '" << _cat_info_->get_category () << "' (type=" 
         << _cat_info_->get_type () 
         << ", depth=" <<  _cat_info_->get_depth () << ")" << endl;
    for (map<int, address_set>::const_iterator i = _addr_sets_.begin ();
         i != _addr_sets_.end ();
         i++)
      {
        map<int, address_set>::const_iterator j = i; 
        j++;
        if (j == _addr_sets_.end ())
          {
            out_ << "`-- ";
          }
        else
          {
            out_ << "|-- ";
          }
        out_ << "Address index=" << i->first
             << " (" << _cat_info_->addresses[i->first] << ")"
             << " : address set=" << i->second 
             << endl;
      }
    return;
  } 

  void id_selector::reset ()
  {
    _cat_info_ = 0;
    _addr_sets_.clear ();
  }

  bool id_selector::match (const geom_id & gid_) const
  {
    if (! is_initialized ())
      {
        ostringstream message;
        message << "id_selector::match: " 
                << "ID selector is not initialized by any rule !";
        throw logic_error (message.str ());   
      }
    if (gid_.get_type () != _cat_info_->get_type ())
      {
        return false;
      }
    bool matched = true;
    for (map<int, address_set>::const_iterator i = _addr_sets_.begin ();
         i != _addr_sets_.end ();
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
