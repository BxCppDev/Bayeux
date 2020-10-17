//! \file  datatools/ui/target_command.h
//! \brief Base command for target objects
//
// Copyright (c) 2015-2016 by François Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef DATATOOLS_UI_TARGET_COMMAND_H
#define DATATOOLS_UI_TARGET_COMMAND_H

// Standard library:
#include <string>
#include <vector>
#include <memory>

// This project:
#include <datatools/ui/base_command.h>

namespace datatools {

  namespace ui {

    //! \brief Base command for a target object
    template <typename Type>
    class target_command
      : public base_command
    {
    public:

      //! Default constructor
      target_command()
        : base_command(),
          _target_(nullptr)
      {
        return;
      }

      //! Constructor
      target_command(Type & target_,
                     const std::string & name_,
                     const std::string & description_ = "",
                     const version_id & vid_ = version_id::invalid())
        : base_command(name_, description_, vid_),
          _target_(nullptr)
      {
        _set_target(target_);
        return;
      }

      //! Destructor
      ~target_command() override
      {
        return;
      }

      //! Check validity
      bool is_valid() const override
      {
        return base_command::is_valid() && has_target();
      }

      bool has_target() const
      {
        return _target_ != nullptr;
      }

      //! Return the target
      const Type & get_target() const
      {
        return *_target_;
      }

      //! Return the target
      Type & grab_target()
      {
        return _grab_target();
      }

      //! Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override
      {
        this->base_command::tree_dump(out_, title_, indent_, true);

        out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
             << "Target : [@" << _target_ << "]" << std::endl;

        return;
      }

    protected:

      //! Return the target
      Type & _grab_target()
      {
        return *_target_;
      }

      //! Set the target
      void _set_target(Type & target_)
      {
        _target_ = &target_;
        return;
      }

    private:

      Type * _target_ = nullptr; //!< The reference to the target object

    };

    //! \brief Base command for a const target object
    template <typename Type>
    class const_target_command
      : public base_command
    {
    public:

      //! Default constructor
      const_target_command()
        : base_command(),
          _target_(nullptr)
      {
        return;
      }

      //! Constructor
      const_target_command(const Type & target_,
                           const std::string & name_,
                           const std::string & description_ = "",
                           const version_id & vid_ = version_id::invalid())
        : base_command(name_, description_, vid_),
          _target_(nullptr)
      {
        _set_target(target_);
        return;
      }

      //! Destructor
      ~const_target_command() override
      {
        return;
      }

      //! Check validity
      bool is_valid() const override
      {
        return base_command::is_valid() && has_target();
      }

      bool has_target() const
      {
        return _target_ != nullptr;
      }

      //! Return the target
      const Type & get_target() const
      {
        return *_target_;
      }

      //! Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override
      {
        this->base_command::tree_dump(out_, title_, indent_, true);

        out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
             << "Target : [@" << _target_ << "]" << std::endl;

        return;
      }

    protected:

      //! Return the target
      const Type & _get_target() const
      {
        return *_target_;
      }

      //! Set the target
      void _set_target(const Type & target_)
      {
        _target_ = &target_;
        return;
      }

    private:

      const Type * _target_ = nullptr; //!< The reference to the const target object

    };

  } // namespace ui

} // namespace datatools

#endif // DATATOOLS_UI_TARGET_COMMAND_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
