// -*- mode: c++; -*-
// version_check.cc
// Ourselves
#include <datatools/utils/version_check.h>

// Standard Library
#include <sstream>
#include <stdexcept>

// Third Party
// - Boost
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

// This Project
#include <datatools/utils/version_id.h>


namespace datatools {
namespace utils {

bool validate_version(const std::string& label_, const version_id& vid_,
                      const std::string& version_rules_) {
  if (label_.empty()) {
    throw std::logic_error("datatools::utils::validate_version: Missing a valid label !");
  }
  if (!vid_.is_valid()) {
    throw std::logic_error("datatools::utils::validate_version: Missing a valid version ID !");
  }

  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep("|");
  tokenizer tokens(version_rules_, sep);

  int checked_count = 0;
  for (tokenizer::iterator tok_iter = tokens.begin();
       tok_iter != tokens.end(); ++tok_iter) {
    std::string tok = *tok_iter;
    boost::trim(tok);
    boost::char_separator<char> sep2("()");
    tokenizer tokens2(tok, sep2);
    int count = 0;
    for (tokenizer::iterator tok_iter2 = tokens2.begin();
         tok_iter2 != tokens2.end(); ++tok_iter2, ++count) {}
    
    if (count > 2) {
      std::ostringstream message;
      message << "datatools::utils::validate_version: "
              << "Invalid syntax (unexpected token in '"
              << tok << "') !";
      throw std::logic_error(message.str());
    } 
    std::string label;
    std::string vid_rules;
    count = 0;
    for (tokenizer::iterator tok_iter2 = tokens2.begin();
         tok_iter2 != tokens2.end(); 
         ++tok_iter2, ++count) {
      if (count == 0) {
        std::istringstream label_iss(*tok_iter2);
        label_iss >> label >> std::ws;
        {
          std::string more;
          label_iss >> more >> std::ws;
          if (! more.empty()) {
            std::ostringstream message;
            message << "datatools::utils::validate_version: "
                    << "Invalid syntax (unexpected token '"
                    << more << "') !";
            throw std::logic_error(message.str());
          }
        }
      } else if (count == 1) {
        vid_rules = *tok_iter2;
        boost::trim(vid_rules);
      }
    }
    
    if (label != label_) continue;
    
    // process version ID rules :
    {
      bool ok_rules = true;
      boost::char_separator<char> sep3("&");
      tokenizer tokens3(vid_rules, sep3);
      int count3 = 0;
      for (tokenizer::iterator tok_iter3 = tokens3.begin();
           tok_iter3 != tokens3.end(); ++tok_iter3) {
        count3++;
      }     
      if (count3 > 1) {
        if (vid_rules[0] == '=') {
          std::ostringstream message;
          message << "datatools::utils::validate_version: "
                  << "Invalid syntax (unexpected token '"
                  << vid_rules[0] << "' in '" << vid_rules << "') !";
          throw std::logic_error(message.str());                
        }
      }
      for (tokenizer::iterator tok_iter3 = tokens3.begin();
           tok_iter3 != tokens3.end(); ++tok_iter3) {
        std::string tok3 = *tok_iter3;
        boost::trim(tok3);
        
        bool ok = vid_.matches(tok3, true);
        if (!ok) ok_rules = false;
      }

      if (ok_rules) checked_count++;
    }
  }

  return (checked_count > 0);
}

} // namespace utils
} // namespace datatools

