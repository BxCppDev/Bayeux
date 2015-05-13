/* dump_module.cc
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <dpp/dump_module.h>

// Standard library:
#include <stdexcept>
#include <sstream>
#include <fstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/things.h>

namespace dpp {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(dump_module, "dpp::dump_module");

  void dump_module::set_title(const std::string & a_title)
  {
    _title_ = a_title;
    return;
  }

  void dump_module::set_indent(const std::string & a_indent)
  {
    _indent_ = a_indent;
    return;
  }

  void dump_module::set_output (const std::string & a_output,
                                const std::string & a_file)
  {
    _output_filename_.clear();
    _output_ = OUTPUT_INVALID;
    if (a_output == "clog") {
      _output_ = OUTPUT_CLOG;
    } else if (a_output == "cout") {
      _output_ = OUTPUT_COUT;
    } else if (a_output == "file") {
      _output_ = OUTPUT_FILE;
      _output_filename_ = a_file;
    }
    DT_THROW_IF(_output_ == OUTPUT_INVALID,
                std::domain_error,
                "Module '" << get_name () << "' has no output '"
                << a_output << "' !");
    DT_THROW_IF(_output_ == OUTPUT_FILE && _output_filename_.empty(),
                std::logic_error,
                "Module '" << get_name () << "' has no output filename ' !");
    return;
  }

  void dump_module::_set_defaults ()
  {
    _output_ = OUTPUT_INVALID;
    _output_filename_ = "";
    _out_ = 0;
    _fout_.reset(0);
    return;
  }

  // Constructor :
  dump_module::dump_module(datatools::logger::priority logging_priority_)
    : base_module(logging_priority_)
  {
    _out_ = 0;
    _set_defaults ();
    return;
  }

  dump_module::~dump_module()
  {
    if (is_initialized()) dump_module::reset();
    return;
  }

  // Initialization :
  void dump_module::initialize(const datatools::properties & a_config,
                                datatools::service_manager & /*a_service_manager*/,
                                dpp::module_handle_dict_type & /*a_module_dict*/)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Dump module '" << get_name () << "' is already initialized ! ");

    _common_initialize(a_config);

    if (a_config.has_key ("title")) {
      set_title(a_config.fetch_string ("title"));
    }

    if (a_config.has_key ("indent")) {
      set_indent(a_config.fetch_string ("indent"));
    }

    if (_output_ == OUTPUT_INVALID) {
      if (a_config.has_key ("output")) {
        std::string output_str = a_config.fetch_string ("output");
        if (output_str == "clog") {
          _output_ = OUTPUT_CLOG;
        } else if (output_str == "cout") {
          _output_ = OUTPUT_COUT;
        } else if (output_str == "file") {
          _output_ = OUTPUT_FILE;
          DT_THROW_IF(! a_config.has_key ("output.filename"),
                       std::logic_error,
                       "Missing 'output.filename' property in module '"
                      << get_name () << "' ! ");
          _output_filename_ = a_config.fetch_string ("output.filename");
        }
        DT_THROW_IF(_output_ == OUTPUT_INVALID,
                    std::logic_error,
                    "Invalid output label '" << output_str << " for module '" << get_name () << "' !");
      }
    }

    // Default output :
    if (_output_ == OUTPUT_INVALID) {
      _output_ = OUTPUT_DEFAULT;
    }

    // Initialize the stream :
    if (_output_ == OUTPUT_COUT) {
      _out_ = &std::cout;
    } else if (_output_ == OUTPUT_CLOG) {
      _out_ = &std::clog;
    } else if (_output_ == OUTPUT_FILE) {
      std::ofstream * output_file = new std::ofstream;
      _fout_.reset(output_file);
      _out_ = _fout_.get();
      datatools::fetch_path_with_env (_output_filename_);
      output_file->open (_output_filename_.c_str ());
      if (! *output_file) {
        _fout_.reset(0);
        DT_THROW_IF(true,
                    std::runtime_error,
                    "Cannot open output file '" << _output_filename_  << "' !");
      }
      _out_ = _fout_.get();
    }
    _set_initialized (true);
    return;
  }

  std::ostream & dump_module::_grab_output ()
  {
    return *_out_;
  }

  // Reset :
  void dump_module::reset()
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Dump module '" << get_name () << "' is not initialized !");
    _set_initialized (false);
    if (_output_ == OUTPUT_FILE) {
      DT_LOG_NOTICE(_logging,
                    "Event record dump is stored in file '" << _output_filename_ << "'.");
    }
    _out_ = 0;
    _fout_.reset(0);
    _set_defaults ();
    return;
  }

  // Processing :
  base_module::process_status
  dump_module::process(::datatools::things & the_data_record)
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Dump module '" << get_name () << "' is not initialized !");

    std::vector<std::string> bank_labels;
    the_data_record.get_names(bank_labels);

    if (!_title_.empty()) {
      _grab_output () << _indent_ << _title_ << ": " << std::endl;
    }

    for (size_t ibank = 0; ibank < bank_labels.size(); ibank++) {
      const std::string & bank_label = bank_labels[ibank];
      const datatools::i_serializable & bank = the_data_record.get(bank_label);
      std::string tag = "|-- ";
      std::string tag2 = "|   ";
      if (ibank == bank_labels.size() - 1) {
        tag = "`-- ";
        tag2 = "    ";
      }
      _grab_output () << _indent_;
      _grab_output () << tag << "Bank '" << bank_label << "'";
      if (the_data_record.is_constant(bank_label)) {
        _grab_output () << " (const)";
      }
      _grab_output () << " : ";
      _grab_output () << '"' << bank.get_serial_tag () << '"' << std::endl;
      const datatools::i_tree_dumpable *dumpable
        = dynamic_cast<const datatools::i_tree_dumpable*>(&bank);
      if (dumpable != 0) {
        std::ostringstream indent_oss;
        indent_oss << _indent_ << tag2;
        dumpable->tree_dump(_grab_output (),"", indent_oss.str());
      }
    }
    return PROCESS_SUCCESS;
  }

  void dump_module::tree_dump (std::ostream & a_out ,
                               const std::string & a_title,
                               const std::string & a_indent,
                               bool a_inherit) const
  {
    this->base_module::tree_dump (a_out, a_title, a_indent, true);

    a_out << a_indent << datatools::i_tree_dumpable::tag
      << "Output             : '" << _output_ << "'" << std::endl;

    a_out << a_indent << datatools::i_tree_dumpable::tag
      << "Output stream      : " << _out_ << std::endl;

    a_out << a_indent << datatools::i_tree_dumpable::tag
      << "Output file name   : '" << _output_filename_ << "'" << std::endl;

    a_out << a_indent << datatools::i_tree_dumpable::tag
      << "Output file handle : '" << _fout_.get() << "'" << std::endl;

    a_out << a_indent << datatools::i_tree_dumpable::tag
      << "Title              : '" << _title_ << "'" << std::endl;

    a_out << a_indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
      << "Indent             : '" << _indent_ << "'" << std::endl;

    return;
  }

}  // end of namespace dpp

// end of dump_module.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
