//! \file  datatools/ui/shell_tree_command.h
//! \brief Shell tree command
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef DATATOOLS_UI_SHELL_TREE_COMMAND_H
#define DATATOOLS_UI_SHELL_TREE_COMMAND_H

// This project:
#include <datatools/command_utils.h>
#include <datatools/ui/target_command.h>
#include <datatools/ui/basic_shell.h>

namespace datatools {

  namespace ui {

    //! \brief Shell tree command
    struct shell_tree_command
      : public const_target_command<basic_shell>
    {
      shell_tree_command(const basic_shell &);
      ~shell_tree_command() override;
    protected:
      void _init(const datatools::properties & config_) override;
      void _run(datatools::command::returned_info & cri_, uint32_t flags_ = 0) override;

      //! Recursive tree print
      void _tree_print_children(std::ostream & out_,
                                const std::string & path_,
                                const std::string & indent_ = "");

      enum format_mode_type {
        FM_ORIGINAL = 0,
        FM_FULLPATH = 1,
        FM_BASENAME = 2
      };

      std::string _format_full_path(const std::string & full_path_,
                                    format_mode_type);

    private:

      bool _all_;
      bool _classify_;
      bool _longf_;
      bool _colored_;
      bool _dirs_only_;
      bool _dirs_first_;
      bool _full_path_;
      bool _prune_empty_dirs_;
      uint32_t _max_level_;
      uint32_t _level_;

    };

  } // namespace ui

} // namespace datatools

#endif // DATATOOLS_UI_SHELL_TREE_COMMAND_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
