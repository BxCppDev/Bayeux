// -*- mode: c++; -*-
// id_selector.cc

// Ourselves:
#include <geomtools/id_selector.h>

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace geomtools {

  using namespace std;

  bool id_selector::is_initialized () const
  {
    return (_cat_info_ != 0);
  }

  const id_mgr::category_info & id_selector::get_category_info () const
  {
    DT_THROW_IF(_cat_info_ == 0, logic_error,
                "Missing category info ! ID selector is not initalized !");
    return *_cat_info_;
  }

  void id_selector::set_id_mgr (const id_mgr & id_mgr_)
  {
    DT_THROW_IF (is_initialized (), logic_error,
                 "Selector is already initialized !");
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
    if (is_initialized ()) {
      _cat_info_ = 0;
    }
    DT_THROW_IF (_id_mgr_ == 0, logic_error, "Missing ID manager !");
    istringstream rules_iss (rules_);

    // Extract the geometry category token :
    string cat_token;
    rules_iss >> cat_token >> ws;
    DT_THROW_IF (! rules_iss, logic_error,
                 "Missing category rule !");
    typedef vector<string> split_vector_type;
    split_vector_type cat_token_vec;
    boost::split (cat_token_vec, cat_token, boost::algorithm::is_any_of ("="));
    DT_THROW_IF (cat_token_vec.size() != 2, logic_error,
                 "Invalid category rule format !");
    DT_THROW_IF (cat_token_vec[0] != "category", logic_error,
                 "Cannot find category label !");
    string category = cat_token_vec[1];
    DT_THROW_IF (category.empty (), logic_error,
                 "Missing category name label !");
    if (category[0] == '\'') {
      DT_THROW_IF (category.length() < 3, logic_error,
                   "Invalid quoted category name label '"
                   << category << "' !");
      DT_THROW_IF (category[category.length()-1] != '\'', logic_error,
                   "Missing end quote for category name label '"
                   << category << "' !");
      category = category.substr(1, category.length()-2);
    }
    DT_THROW_IF (! _id_mgr_->has_category_info (category), logic_error,
                 "Unknown category name '" << category << "' !");

    _cat_info_ = &(_id_mgr_->get_category_info  (category));
    size_t parsed_addr_rules = 0;
    while (! rules_iss.eof () && (parsed_addr_rules < _cat_info_->get_depth ())) {
      string addr_token;
      rules_iss >> addr_token >> ws;
      // Only throw if stream is bad and NOT at eof. Some C++ libraries
      // (Apple clang on Mavericks) will set bad/failbit at eof. Seems
      // not to be C++11 standard compliant, but is still present!
      DT_THROW_IF (! rules_iss && !rules_iss.eof(),
                   logic_error,
                   "Invalid address token !");
      split_vector_type addr_token_vec;
      boost::split (addr_token_vec, addr_token, boost::algorithm::is_any_of ("="));
      DT_THROW_IF (addr_token_vec.size () != 2, logic_error, "Invalid address rule format !");
      string addr_name = addr_token_vec[0];
      DT_THROW_IF (addr_name.empty (), logic_error, "Cannot find address label !");
      string addr_rule = addr_token_vec[1];
      DT_THROW_IF (addr_rule.empty (), logic_error, "Address rule is empty !");

      int addr_index = -1;
      for (size_t i = 0; i < _cat_info_->addresses.size (); i++) {
        if (_cat_info_->addresses[i] == addr_name) {
          addr_index = i;
          break;
        }
      }

      DT_THROW_IF (addr_index < 0, logic_error,"Cannot find address label '"
                   << addr_name << "' in category '"
                   << category << "' !");
      DT_THROW_IF (_addr_sets_.find (addr_index) != _addr_sets_.end (), logic_error,
                   "Rule for address index "
                   << addr_index << " in category '"
                   << category << "'is already set !");
      address_set as;
      istringstream addr_rule_iss (addr_rule);
      addr_rule_iss >> as;
      DT_THROW_IF (! addr_rule_iss, logic_error,
                   "Cannot parse address rule '"
                   << addr_rule << "' for address '"
                   << addr_name << "' in category '"
                   << category << "' !");
      _addr_sets_[addr_index] = as;
      parsed_addr_rules++;
    }

    return;
  }

  void id_selector::dump (ostream & out_, const string & title_) const
  {
    if (! title_.empty ()) {
      out_ << title_ << ": " << endl;
    }
    if (! is_initialized ()) {
      out_ << "`-- Initialized:   '" << "No" << "'" << endl;
      return;
    } else {
      out_ << "|-- Initialized:   '" << "Yes" << "'" << endl;
    }
    out_ << "|-- Category:   '" << _cat_info_->get_category () << "' (type="
         << _cat_info_->get_type ()
         << ", depth=" <<  _cat_info_->get_depth () << ")" << endl;
    for (map<int, address_set>::const_iterator i = _addr_sets_.begin ();
         i != _addr_sets_.end ();
         i++) {
      map<int, address_set>::const_iterator j = i;
      j++;
      if (j == _addr_sets_.end ()) {
        out_ << "`-- ";
      } else {
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
    DT_THROW_IF (! is_initialized (), logic_error, "ID selector is not initialized by any rule !");
    if (gid_.get_type () != _cat_info_->get_type ()) {
      return false;
    }
    bool matched = true;
    for (map<int, address_set>::const_iterator i = _addr_sets_.begin ();
         i != _addr_sets_.end ();
         i++) {
      int addr_index = i->first;
      uint32_t addr = gid_.get (addr_index);
      if (! i->second.match (addr)) {
        matched = false;
        break;
      }
    }
    return matched;
  }

} // end of namespace geomtools

// end of id_selector.cc
