// -*- mode: c++; -*-
//! \file datatools/i_serializable-reflect.h
#ifndef DATATOOLS_I_SERIALIZABLE_REFLECT_H_
#define DATATOOLS_I_SERIALIZABLE_REFLECT_H_

// Standard Library

// Third Party

// Ourselves
#include <datatools/i_serializable.h>

// Export the introspection code for the 'datatools::i_serializable' class:
DR_CLASS_EXPORT (::datatools::i_serializable, "datatools::i_serializable")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::datatools::i_serializable, tag_)
      {
        DR_CLASS_DECLARE (::datatools::i_serializable)
          /*
          .DR_CLASS_METHOD_CONST("get_serial_tag",
                                 ::datatools::i_serializable,
                                 ::datatools::i_serializable::get_serial_tag,
                                 const std::string &)
          */
          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_I_SERIALIZABLE_REFLECT_H_
