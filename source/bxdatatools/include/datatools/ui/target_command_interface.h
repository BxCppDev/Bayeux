//! \file  datatools/ui/target_command_interface.h
//! \brief Target command interface
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

#ifndef DATATOOLS_UI_TARGET_COMMAND_INTERFACE_H
#define DATATOOLS_UI_TARGET_COMMAND_INTERFACE_H

// This project:
#include <datatools/ui/base_command_interface.h>

namespace datatools {

  namespace ui {

    //! \brief Base command interface for a target object
    //!
    //! The target object (of arbitrary template type)
    //! cannot be set after the command interface object
    //! has been initialized.
    template <typename Type>
    class target_command_interface
      : public base_command_interface
    {
    public:

      //! Default constructor
      target_command_interface()
        : base_command_interface(),
          _target_(nullptr)
      {
        return;
      }

      //! Constructor
      target_command_interface(const std::string & name_,
                               const std::string & description_ = "",
                               const version_id & vid_ = version_id::invalid())
        : base_command_interface(name_, description_, vid_),
          _target_(nullptr)
      {
        return;
      }

      //! Constructor
      target_command_interface(Type & target_,
                               const std::string & name_,
                               const std::string & description_ = "",
                               const version_id & vid_ = version_id::invalid())
        : base_command_interface(name_, description_, vid_),
          _target_(nullptr)
      {
        _set_target(target_);
        return;
      }

      //! Destructor
      ~target_command_interface() override
      {
        return;
      }

      //! Check if the target is set
      bool has_target() const
      {
        return _target_ != nullptr;
      }

      //! Set the target object
      void set_target(Type & target_)
      {
        DT_THROW_IF(is_initialized(), std::logic_error,
                    "Target command interface is already initialized!");
        _set_target(target_);
        return;
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
        this->base_command_interface::tree_dump(out_, title_, indent_, true);

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

    //! \brief Base command interface for a const target object
    //!
    //! The target object (of arbitrary template type)
    //! cannot be set after the command interface object
    //! has been initialized.
    template <typename Type>
    class const_target_command_interface
      : public base_command_interface
    {
    public:

      //! Default constructor
      const_target_command_interface()
        : base_command_interface(),
          _target_(nullptr)
      {
        return;
      }

      //! Constructor
      const_target_command_interface(const std::string & name_,
                                     const std::string & description_ = "",
                                     const version_id & vid_ = version_id::invalid())
        : base_command_interface(name_, description_, vid_),
          _target_(nullptr)
      {
        return;
      }

      //! Constructor
      const_target_command_interface(const Type & target_,
                                     const std::string & name_,
                                     const std::string & description_ = "",
                                     const version_id & vid_ = version_id::invalid())
        : base_command_interface(name_, description_, vid_),
          _target_(nullptr)
      {
        _set_target(target_);
        return;
      }

      //! Destructor
      ~const_target_command_interface() override
      {
        return;
      }

      //! Check if the target is set
      bool has_target() const
      {
        return _target_ != nullptr;
      }

      //! Set the target object
      void set_target(const Type & target_)
      {
        DT_THROW_IF(is_initialized(), std::logic_error,
                    "Target command interface is already initialized!");
        _set_target(target_);
        return;
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
        this->base_command_interface::tree_dump(out_, title_, indent_, true);

        out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
             << "Target : [@" << _target_ << "]" << std::endl;

        return;
      }

    protected:

      //! Return the target
      const Type & _get_target()
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

#endif // DATATOOLS_UI_TARGET_COMMAND_INTERFACE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
