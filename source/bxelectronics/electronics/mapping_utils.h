// -*- mode: c++; -*-
#ifndef ELECTRONICS_MAPPING_UTILS_H
#define ELECTRONICS_MAPPING_UTILS_H

// Standard library
#include <string>

// Third party

// This project
#include <electronics/component_info.h>

namespace datatools {
  class properties;
}

namespace electronics {

  struct mapping_utils
  {
  public:

    static const std::string & mapping_prefix();

    static const std::string & mapping_category_property();

    static const std::string & mapping_address_property();

    static const std::string & mapping_id_property();

    static const std::string & mapping_embedded_id_property();

    static void extract(const datatools::properties & source_,
			datatools::properties & target_);

    static std::string make_key(const std::string & key_);

    static bool has_key(const datatools::properties & config_,
			const std::string & key_);

    static bool has_flag(const datatools::properties & config_,
			 const std::string & key_);

    static bool has_id(const datatools::properties & config_);

    static void fetch_id(const datatools::properties & config_,
			 std::string & id_info_);

    static bool has_embedded_id(const datatools::properties & config_,
				const std::string & embedded_name_);

    static void fetch_embedded_id(const datatools::properties & config_,
				  const std::string & embedded_name_,
				  std::string & id_info_);
  };

} // end of namespace electronics

#endif // ELECTRONICS_MAPPING_UTILS_H
