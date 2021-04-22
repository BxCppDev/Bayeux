// Ourselves
#include <datatools/make_configuration.h>

// This Project
#include <datatools/multi_properties.h>
#include <datatools/multi_properties_config.h>
#include <datatools/properties.h>
#include <datatools/properties_config.h>

namespace datatools {
  // read/write for datatools::properties
  void read_config(const std::string & filename_, properties & props_,
                   uint32_t options_)
  {
    properties_config r(options_);
    r.read(filename_, props_);
    return;
  }

  void write_config(const std::string & filename_, const properties & props_,
                    uint32_t options_)
  {
    properties_config writer(options_);
    writer.write(filename_, props_);
    return;
  }

  // read/write for datatools::multi_properties
  void read_config(const std::string & filename_, multi_properties & props_,
                   uint32_t options_)
  {
    multi_properties_config r(options_);
    r.read(filename_, props_);
    return;
  }

  void write_config(const std::string & filename_, const multi_properties & props_,
                    uint32_t options_)
  {
    multi_properties_config w(options_);
    w.write(filename_, props_);
    return;
  }

}