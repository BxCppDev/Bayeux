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

  bool id_selector::is_initialized() const
  {
    return (_cat_info_ != 0);
  }

  const id_mgr::category_info & id_selector::get_category_info() const
  {
    DT_THROW_IF(_cat_info_ == 0, logic_error,
                "Missing category info ! ID selector is not initalized !");
    return *_cat_info_;
  }

  void id_selector::set_id_mgr(const id_mgr & id_mgr_)
  {
    DT_THROW_IF(is_initialized(), logic_error, "Selector is already initialized !");
    _id_mgr_ = &id_mgr_;
    return;
  }

  id_selector::id_selector()
  {
    _id_mgr_ = 0;
    _cat_info_ = 0;
    return;
  }

  id_selector::id_selector(const id_mgr & id_mgr_)
  {
    _id_mgr_ = &id_mgr_;
    _cat_info_ = 0;
    return;
  }

  id_selector::~id_selector()
  {
    reset();
    _cat_info_ = 0;
    _id_mgr_ = 0;
    return;
  }

  void id_selector::initialize(const datatools::properties & config_)
  {
    if (is_initialized()) {
      _cat_info_ = 0;
    }
    DT_THROW_IF(_id_mgr_ == 0, logic_error, "Missing ID manager !");

    // config_.tree_dump(std::cerr, "Configuration: ", "DEVEL: id_selector::initialize: ");

    // Search for an single rule, example:
    //   rule : string  = " category='detector' row=[0;3] column={*} "
    if (config_.has_key("rules")) {
      const std::string & rules = config_.fetch_string("rules");
      initialize(rules);
      return;
    }

    // Search for the category, example:
    //   category : string = "detector"
    DT_THROW_IF(! config_.has_key("category"), std::logic_error,
                "Missing 'category' property!");
    std::string category = config_.fetch_string("category");
    DT_THROW_IF(! _id_mgr_->has_category_info(category), logic_error,
                "Unknown category name '" << category << "' !");

    _cat_info_ = &(_id_mgr_->get_category_info(category));
    for (size_t addr_index = 0; addr_index < _cat_info_->get_addresses().size(); addr_index++) {
      bool addr_processed = false;
      const std::string & addr_name = _cat_info_->get_addresses()[addr_index];
      if (!addr_processed) {
        // Search for a unique value for a given address, examples:
        //   address.row    : integer = 3
        //   address.column : integer = 7
        std::ostringstream unique_addr_name_oss;
        unique_addr_name_oss << "address." << addr_name;
        if (config_.has_key(unique_addr_name_oss.str())) {
          addr_processed = true;
          int unique_addr = config_.fetch_positive_integer(unique_addr_name_oss.str());
          DT_THROW_IF(unique_addr < 0, std::logic_error, "Invalid unique address '" << addr_name << "' in category '" << category << "'!");
          address_set as;
          as.set_mode_list();
          as.add_to_list(unique_addr);
          _addr_sets_[addr_index] = as;
          addr_processed = true;
        }
      }

      if (!addr_processed) {
        // Search for a set of values for a given address, examples:
        //   address.row.set : string = "[0;3]"      # range
        //   address.row.set : string = "{0;1;7,8}"  # list
        //   address.row.set : string = "{*}"        # all
        std::ostringstream set_addr_name_oss;
        set_addr_name_oss << "address." << addr_name << ".set";
        if (config_.has_key(set_addr_name_oss.str())) {
          std::string set_str = config_.fetch_string(set_addr_name_oss.str());
          address_set as;
          std::istringstream iss(set_str);
          iss >> as;
          DT_THROW_IF(!iss, std::logic_error,
                      "Invalid set format for address '" << addr_name << "' in category '" << category << "'!");
          DT_THROW_IF(!as.is_mode_all() &&
                      !as.is_mode_range() &&
                      !as.is_mode_list(),
                      std::logic_error,
                      "Invalid set for address '" << addr_name << "' in category '" << category << "'!");
          _addr_sets_[addr_index] = as;
          addr_processed = true;
        }
      }

      if (!addr_processed) {
        // Default to all:
        address_set as;
        as.set_mode_all();
        _addr_sets_[addr_index] = as;
        addr_processed = true;
      }

    }
    return;
  }

  void id_selector::initialize(const std::string & rules_)
  {
    if (is_initialized ()) {
      _cat_info_ = 0;
    }
    DT_THROW_IF(_id_mgr_ == 0, logic_error, "Missing ID manager !");
    std::istringstream rules_iss (rules_);

    // Extract the geometry category token :
    std::string cat_token;
    rules_iss >> cat_token >> ws;
    DT_THROW_IF(! rules_iss, logic_error, "Missing category rule !");
    typedef std::vector<std::string> split_vector_type;
    split_vector_type cat_token_vec;
    boost::split (cat_token_vec, cat_token, boost::algorithm::is_any_of("="));
    DT_THROW_IF(cat_token_vec.size() != 2, logic_error,
                "Invalid category rule format !");
    DT_THROW_IF(cat_token_vec[0] != "category", logic_error,
                "Cannot find category label !");
    std::string category = cat_token_vec[1];
    DT_THROW_IF(category.empty (), logic_error,
                "Missing category name label !");
    if (category[0] == '\'') {
      DT_THROW_IF(category.length() < 3, logic_error,
                  "Invalid quoted category name label '"
                  << category << "' !");
      DT_THROW_IF(category[category.length()-1] != '\'', logic_error,
                  "Missing end quote for category name label '"
                  << category << "' !");
      category = category.substr(1, category.length()-2);
    }
    DT_THROW_IF(! _id_mgr_->has_category_info (category), logic_error,
                "Unknown category name '" << category << "' !");

    _cat_info_ = &(_id_mgr_->get_category_info(category));
    size_t parsed_addr_rules = 0;
    while (! rules_iss.eof () && (parsed_addr_rules < _cat_info_->get_depth ())) {
      string addr_token;
      rules_iss >> addr_token >> ws;
      // Only throw if stream is bad and NOT at eof. Some C++ libraries
      // (Apple clang on Mavericks) will set bad/failbit at eof. Seems
      // not to be C++11 standard compliant, but is still present!
      DT_THROW_IF(! rules_iss && !rules_iss.eof(),
                  logic_error,
                  "Invalid address token !");
      split_vector_type addr_token_vec;
      boost::split (addr_token_vec, addr_token, boost::algorithm::is_any_of ("="));
      DT_THROW_IF(addr_token_vec.size () != 2, logic_error, "Invalid address rule format !");
      string addr_name = addr_token_vec[0];
      DT_THROW_IF(addr_name.empty (), logic_error, "Cannot find address label !");
      string addr_rule = addr_token_vec[1];
      DT_THROW_IF(addr_rule.empty (), logic_error, "Address rule is empty !");

      int addr_index = -1;
      for (size_t i = 0; i < _cat_info_->get_addresses().size (); i++) {
        if (_cat_info_->get_addresses()[i] == addr_name) {
          addr_index = i;
          break;
        }
      }

      DT_THROW_IF(addr_index < 0, logic_error,"Cannot find address label '"
                  << addr_name << "' in category '"
                  << category << "' !");
      DT_THROW_IF(_addr_sets_.find (addr_index) != _addr_sets_.end (), logic_error,
                  "Rule for address index "
                  << addr_index << " in category '"
                  << category << "'is already set !");
      address_set as;
      std::istringstream addr_rule_iss (addr_rule);
      addr_rule_iss >> as;
      DT_THROW_IF(! addr_rule_iss, logic_error,
                  "Cannot parse address rule '"
                  << addr_rule << "' for address '"
                  << addr_name << "' in category '"
                  << category << "' !");
      _addr_sets_[addr_index] = as;
      parsed_addr_rules++;
    }

    return;
  }

  void id_selector::dump(std::ostream & out_, const std::string & title_) const
  {
    tree_dump(out_, title_, "");
    return;
  }

  void id_selector::tree_dump(std::ostream & out_, const std::string & title_, const std::string & indent_) const
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << ": " << endl;
    }
    if (! is_initialized()) {
      out_ << indent_ << "`-- Initialized:   '" << "No" << "'" << endl;
      return;
    } else {
      out_ << indent_ << "|-- Initialized:   '" << "Yes" << "'" << endl;
    }
    out_ << indent_ << "|-- Category:   '" << _cat_info_->get_category() << "' (type="
         << _cat_info_->get_type()
         << ", depth=" <<  _cat_info_->get_depth() << ")" << endl;
    for (map<int, address_set>::const_iterator i = _addr_sets_.begin();
         i != _addr_sets_.end();
         i++) {
      std::map<int, address_set>::const_iterator j = i;
      j++;
      out_ << indent_;
      if (j == _addr_sets_.end()) {
        out_ << "`-- ";
      } else {
        out_ << "|-- ";
      }
      out_ << "Address index=" << i->first
           << " (" << _cat_info_->get_addresses()[i->first] << ")"
           << " : address set=" << i->second
           << endl;
    }
    return;
  }

  void id_selector::reset()
  {
    _cat_info_ = 0;
    _addr_sets_.clear();
    return;
  }

  bool id_selector::match(const geom_id & gid_) const
  {
    DT_THROW_IF(! is_initialized(), logic_error, "ID selector is not initialized !");
    if (gid_.get_type() != _cat_info_->get_type()) {
      return false;
    }
    bool matched = true;
    for (std::map<int, address_set>::const_iterator i = _addr_sets_.begin();
         i != _addr_sets_.end();
         i++) {
      int addr_index = i->first;
      uint32_t addr = gid_.get(addr_index);
      if (! i->second.match(addr)) {
        matched = false;
        break;
      }
    }
    return matched;
  }

} // end of namespace geomtools
