/// \file datatools/ui/reflective_command_interface-inl.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-08-17
 * Last modified : 2016-08-17
 */

#ifndef DATATOOLS_UI_REFLECTIVE_COMMAND_INTERFACE_INL_H
#define DATATOOLS_UI_REFLECTIVE_COMMAND_INTERFACE_INL_H

// Standard library:
#include <typeinfo>

// This project:
#include <datatools/ui/reflective_command.h>

namespace datatools {

  namespace ui {

    template <class Type>
    reflective_command_interface<Type>::reflective_command_interface()
      : target_command_interface<Type>()
    {
      return;
    }

    template <class Type>
    reflective_command_interface<Type>::reflective_command_interface(const std::string & name_,
                                                                     const std::string & description_,
                                                                     const version_id & vid_)
      : target_command_interface<Type>(name_, description_, vid_)
    {
      return;
    }

    template <class Type>
    reflective_command_interface<Type>::reflective_command_interface(Type & target_,
                                                                     const std::string & name_,
                                                                     const std::string & description_,
                                                                     const version_id & vid_)
      : target_command_interface<Type>(target_,
                                       name_,
                                       description_, vid_)
    {
      return;
    }

    template <class Type>
    reflective_command_interface<Type>::~reflective_command_interface()
    {
      if (this->reflective_command_interface<Type>::is_initialized()) {
        this->reflective_command_interface<Type>::reset();
      }
      return;
    }

    template <class Type>
    bool reflective_command_interface<Type>::is_initialized() const
    {
      return _initialized_;
    }

    template <class Type>
    void reflective_command_interface<Type>::initialize(const datatools::properties & config_,
                                                        const datatools::service_manager & /* services_ */)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Command interface is already initialized!");
      DT_THROW_IF(!this->reflective_command_interface<Type>::has_target(),
                  std::logic_error,
                  "Command interface has no target!");
      base_command_interface::_base_initialize(config_);

      if (config_.has_key("disabled_commands")) {
        std::vector<std::string> discoms;
        config_.fetch("disabled_commands", discoms);
        for (auto discom : discoms) {
          add_disabled_command(discom);
        }
      }

      _add_commands();
      _initialized_ = true;
      return;
    }

    template <class Type>
    void reflective_command_interface<Type>::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Command interface is not initialized!");
      _initialized_ = false;
      //_remove_commands();
      target_command_interface<Type>::_base_reset();
      return;
    }

    template <class Type>
    void reflective_command_interface<Type>::add_disabled_command(const std::string & name_)
    {
      _disabled_commands_.insert(name_);
      return;
    }

    template <class Type>
    void reflective_command_interface<Type>::_add_commands()
    {
      const camp::Class * metaclass = camp::classByTypeSafe<Type>();
      DT_THROW_IF(metaclass == nullptr, std::logic_error,
                  "Class '" << typeid(Type).name() << "' has no reflection support!");

      for (std::size_t ifunc = 0; ifunc < metaclass->functionCount(); ifunc++) {
        const camp::Function & func = metaclass->function(ifunc);
        if (!func.hasTag(datatools::introspection::function_builder_flag())) {
          continue;
        }
        if (_disabled_commands_.count(func.name()) != 0) {
          continue;
        }
        datatools::ui::reflective_command<Type> * cmd
          = new datatools::ui::reflective_command<Type>(this->reflective_command_interface<Type>::_grab_target(), func.name());
        cmd->initialize_simple();
        datatools::ui::reflective_command_interface<Type>::add_command(cmd, func.name());
      }

      return;
    }

    template <class Type>
    void reflective_command_interface<Type>::_remove_commands()
    {
      return;
    }

  } // end of namespace ui

} // end of namespace datatools

#endif // DATATOOLS_UI_REFLECTIVE_COMMAND_INTERFACE_INL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
