// mapping_utils.cc

// Ourselves:
#include <geomtools/mapping_utils.h>

// Standard library:
#include <sstream>

namespace geomtools {

  //using namespace std;

  // static
  const std::string & mapping_utils::mapping_prefix()
  {
    static std::string prefix = "mapping.";
    return prefix;
  }

  const std::string & mapping_utils::mapping_category_property()
  {
    static std::string property = "category";
    return property;
  }

  const std::string & mapping_utils::mapping_address_property()
  {
    static std::string property = "address";
    return property;
  }

  const std::string & mapping_utils::mapping_id_property()
  {
    static std::string property = "id";
    return property;
  }

  const std::string & mapping_utils::mapping_daughter_id_property()
  {
    static std::string property = "daughter_id";
    return property;
  }

  void mapping_utils::extract(const datatools::properties & source_,
                              datatools::properties & target_)
  {
    source_.export_starting_with(target_, mapping_utils::mapping_prefix());
    return;
  }

  std::string mapping_utils::make_key(const std::string & key_)
  {
    std::ostringstream key_oss;
    key_oss << mapping_utils::mapping_prefix() << key_;
    return key_oss.str();
  }

  bool mapping_utils::has_key(const datatools::properties & config_,
                              const std::string & key_)
  {
    return (config_.has_key(mapping_utils::make_key(key_)));
  }

  bool mapping_utils::has_flag(const datatools::properties & config_,
                               const std::string & key_)
  {
    return (config_.has_flag(mapping_utils::make_key(key_)));
  }

  bool mapping_utils::has_id(const datatools::properties & config_)
  {
    return (config_.has_key(mapping_utils::make_key(mapping_utils::mapping_id_property())));
  }

  void mapping_utils::fetch_id(const datatools::properties & config_,
                               std::string & id_info_)
  {
    config_.fetch(mapping_utils::make_key(mapping_utils::mapping_id_property()), id_info_);
    return;
  }

  bool mapping_utils::has_daughter_id(const datatools::properties & config_,
                                      const std::string & daughter_name_)
  {
    return has_labelled_info(config_, mapping_utils::mapping_daughter_id_property(), daughter_name_);
  }

  void mapping_utils::fetch_daughter_id(const datatools::properties & config_,
                                        const std::string & daughter_name_,
                                        std::string & id_info_)
  {
    fetch_labelled_info(config_, mapping_utils::mapping_daughter_id_property(), daughter_name_, id_info_);
    return;
  }

  bool mapping_utils::has_labelled_info(const datatools::properties & config_,
                                        const std::string & label_,
                                        const std::string & name_)
  {
    std::ostringstream labelled_name;
    labelled_name << mapping_utils::make_key(label_) << "." << name_;
    return (config_.has_key(labelled_name.str()));
  }

  void mapping_utils::fetch_labelled_info(const datatools::properties & config_,
                                          const std::string & label_,
                                          const std::string & name_,
                                          std::string & id_info_)
  {
    std::ostringstream labelled_name;
    labelled_name << mapping_utils::make_key(label_) << "." << name_;
    config_.fetch(labelled_name.str(), id_info_);
    return;
  }

} // end of namespace geomtools
