//! \file  datatools/ui/reflective_command.h
//! \brief Reflective command
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_UI_REFLECTIVE_COMMAND_H
#define DATATOOLS_UI_REFLECTIVE_COMMAND_H

// Third Party:
// - Camp:
#include <camp/function.hpp>
#include <camp/value.hpp>

// This project:
#include <datatools/ui/utils.h>
#include <datatools/detail/command_macros.h>
#include <datatools/ui/target_command.h>
#include <datatools/introspection/method.h>

namespace datatools {

  namespace ui {

    //! \brief Reflective command for a target object
    template <typename Type>
    class reflective_command : public target_command<Type>
    {
    public:

      //! Default constructor
      reflective_command();

      //! Constructor
      reflective_command(Type & target_,
                         const std::string & func_name_,
                         const std::string & name_ = "",
                         const std::string & description_ = "",
                         const version_id & vid_ = version_id::invalid());

      //! Destructor
      virtual ~reflective_command();

      //! Set the function name
      void set_func_name(const std::string & func_name_);

      static void repr(std::ostream & out_,
                       const camp::Value & value_,
                       const datatools::introspection::argument & arg_);

    protected:

      virtual void _init(const datatools::properties & config_);

      virtual void _run(datatools::command::returned_info & cri_,
                        uint32_t flags_ = 0);

      virtual void _fini();

    private:

      std::string      _func_name_; ///< Name of the function/method
      datatools::introspection::method _method_; ///< Method description
      const camp::Function * _func_ = nullptr; ///< Handle the reflective metafunction object

    };

  } // namespace ui

} // namespace datatools

#include <datatools/ui/reflective_command-inl.h>

#endif // DATATOOLS_UI_REFLECTIVE_COMMAND_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
