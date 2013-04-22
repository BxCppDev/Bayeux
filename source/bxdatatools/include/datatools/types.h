// -*- mode: c++; -*-
/* datatools/types.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-03-20
 * Last modified: 2013-03-22
 */

#ifndef DATATOOLS_TYPES_H_
#define DATATOOLS_TYPES_H_

#include <boost/bimap.hpp>

namespace datatools {

  enum basic_type
  {
    TYPE_NONE    = 0x0,
    TYPE_BOOLEAN = 0x1,
    TYPE_INTEGER = 0x2,
    TYPE_REAL    = 0x3,
    TYPE_STRING  = 0x4,
  };

  const boost::bimap< int, std::string > & get_type_to_labels_map();

  const std::string get_label_from_type(int type_);

  const int get_type_from_label(const std::string & label_);


} // end of namespace datatools

#endif // DATATOOLS_TYPES_H_

