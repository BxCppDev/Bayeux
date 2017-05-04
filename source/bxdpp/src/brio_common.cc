/* brio_common.cc
 *
 * Copyright (C) 2011-2013 François Mauger <mauger@lpccaen.in2p3.fr>
 *
 *
 */

// Ourselves:
#include <dpp/brio_common.h>

namespace dpp {

  const std::string & brio_common::general_info_store_label()
  {
    static std::string label = "GI";
    return label;
  }

  const std::string & brio_common::event_record_store_label()
  {
    static std::string label = "ER";
    return label;
  }

}  // end of namespace dpp

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
