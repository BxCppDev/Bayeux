
// datatools/configuration/ui/variant_repository_cli.cc
/*
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <datatools/configuration/ui/variant_repository_cli.h>

// This project:
#include <datatools/exception.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/variant_record.h>
#include <datatools/configuration/parameter_model.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/ui/variant_registry_cli.h>
#include <datatools/detail/command_macros.h>

namespace datatools {

  namespace configuration {

    namespace ui {

      variant_repository_cli::variant_repository_cli(datatools::configuration::variant_repository & repository_)
        : _logging_(datatools::logger::PRIO_FATAL)
        , _repository_(repository_)
      {
        return;
      }

      datatools::logger::priority
      variant_repository_cli::get_logging() const
      {
        return _logging_;
      }

      void variant_repository_cli::set_logging(const datatools::logger::priority p_)
      {
        _logging_ = p_;
        return;
      }

      datatools::command::returned_info
      variant_repository_cli::cmd_set_parameter(const std::string & registry_key_,
                                                const std::string & param_path_,
                                                const std::string & param_value_token_)
      {
        datatools::command::returned_info cri;
        if (_repository_.has_registry(registry_key_)) {
           variant_registry & vreg = _repository_.grab_registry(registry_key_);
           ui::variant_registry_cli vregCli(vreg);
           cri = vregCli.cmd_set_parameter_value(param_path_, param_value_token_);
        } else {
          DT_COMMAND_RETURNED_ERROR(cri, command::CEC_SCOPE_INVALID,
                                    "Variant repository has no registry named '" << registry_key_ << "' !");
        }
        return cri;
      }

      datatools::command::returned_info
      variant_repository_cli::cmd_get_parameter(const std::string & registry_key_,
                                                const std::string & param_path_,
                                                std::string & param_value_token_) const
      {
        datatools::logger::priority logging = _logging_;
        datatools::command::returned_info cri;
        DT_LOG_TRACE(logging, "registry_key = " << registry_key_);
        if (_repository_.has_registry(registry_key_)) {
          const variant_registry & vreg = _repository_.get_registry(registry_key_);
          ui::variant_registry_cli vregCli(const_cast<variant_registry&>(vreg));
          vregCli.set_logging(logging);
          DT_LOG_TRACE(logging, "param_path = " << param_path_);
          cri = vregCli.cmd_get_parameter_value(param_path_, param_value_token_);
        } else {
          DT_COMMAND_RETURNED_ERROR(cri, command::CEC_SCOPE_INVALID,
                                    "Variant repository has no registry named '" << registry_key_ << "' !");
        }
        return cri;
      }

      datatools::command::returned_info
      variant_repository_cli::cmd_has_parameter(const std::string & registry_key_,
                                                const std::string & param_path_,
                                                bool & present_) const
      {
        datatools::command::returned_info cri;
        present_ = false;
        if (_repository_.has_registry(registry_key_)) {
          const variant_registry & vreg = _repository_.get_registry(registry_key_);
          present_ = vreg.has_parameter_record(param_path_);
        } else {
          DT_COMMAND_RETURNED_ERROR(cri, command::CEC_SCOPE_INVALID,
                                    "Variant repository has no registry named '" << registry_key_ << "' !");
        }
        return cri;
      }

      datatools::command::returned_info
      variant_repository_cli::cmd_is_active_registry(const std::string & registry_key_,
                                                     bool & active_) const
      {
        datatools::command::returned_info cri;
        active_ = false;
        if (_repository_.has_registry(registry_key_)) {
          active_ = _repository_.is_active_registry(registry_key_);
        } else {
          DT_COMMAND_RETURNED_ERROR(cri, command::CEC_SCOPE_INVALID,
                                    "Variant repository has no registry named '" << registry_key_ << "' !");
        }
        return cri;
      }

      datatools::command::returned_info
      variant_repository_cli::cmd_is_active_variant(const std::string & registry_key_,
                                                    const std::string & variant_path_,
                                                    bool & active_) const
      {
        datatools::command::returned_info cri;
        active_ = false;
        if (_repository_.has_registry(registry_key_)) {
          const variant_registry & vreg = _repository_.get_registry(registry_key_);
          active_ = vreg.is_active_variant(variant_path_);
        } else {
          DT_COMMAND_RETURNED_ERROR(cri, command::CEC_SCOPE_INVALID,
                                    "Variant repository has no registry named '" << registry_key_ << "' !");
        }
        return cri;
      }

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools
