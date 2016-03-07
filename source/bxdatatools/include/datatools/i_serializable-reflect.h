//! \file datatools/i_serializable-reflect.h
#ifndef DATATOOLS_I_SERIALIZABLE_REFLECT_H
#define DATATOOLS_I_SERIALIZABLE_REFLECT_H

// Standard Library:
#include <iostream>

// Ourselves:
#include <datatools/i_serializable.h>

// This project:
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>

// Export the introspection code for the 'datatools::i_serializable' class:
DR_CLASS_EXPORT (::datatools::i_serializable, "datatools::i_serializable")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::datatools::i_serializable, tag_ __attribute__((unused)) )
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

#endif // DATATOOLS_I_SERIALIZABLE_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
