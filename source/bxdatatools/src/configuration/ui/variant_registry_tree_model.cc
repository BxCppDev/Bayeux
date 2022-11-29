// datatools/configuration/ui/variant_registry_tree_model.cc
/*
 * Copyright (C) 2014-2017 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

// http://www.qtcentre.org/threads/27030-Item-Delegate-for-TreeView-with-multiple-editor-widgets
// http://qt-project.org/doc/qt-4.8/model-view-programming.html#delegates

// Ourselves:
#include <datatools/configuration/ui/variant_registry_tree_model.h>

// Standard library:
#include <stdexcept>
#include <fstream>
#include <string>

// Third party:
// - Boost:
// #include <boost/filesystem/path.hpp>
// - Qt:
#include <QColor>

// This project:
#include <datatools/configuration/variant_record.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/parameter_model.h>
#include <datatools/configuration/variant_model.h>
#include <datatools/exception.h>
#include <datatools/configuration/io.h>

namespace datatools {

  namespace configuration {

    namespace ui {

      tree_item::tree_item(variant_record & record_, tree_item * parent_)
      {
        _logging_ = datatools::logger::PRIO_FATAL;
        _record_  = &record_;
        _parent_  = parent_;
        return;
      }

      tree_item::~tree_item()
      {
        qDeleteAll(_children_);
        return;
      }

      QList<tree_item *> & tree_item::grab_children()
      {
        return _children_;
      }

      const QList<tree_item *> & tree_item::get_children() const
      {
        return _children_;
      }

      void tree_item::tree_dump(std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const {
        if (!title_.empty()) {
          out_ << indent_ << title_ << " : " << std::endl;
        }

        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Address: [" << this << "] "
             << std::endl;

        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Record path : '" << _record_->get_path() << "' "
             << std::endl;

        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Leaf name: '" << _record_->get_leaf_name() << "' "
             << "[model='"
             << (_record_->is_variant() ? "variant" : "parameter")
             << "'; active=" << _record_->is_active()
             << "]"
             << std::endl;

        if (_record_->is_parameter()) {
          std::string data_value_repr;
          command::returned_info cri = _record_->value_to_string(data_value_repr);
          out_ << indent_ << datatools::i_tree_dumpable::tag
               << "Parameter data: "
               << (data_value_repr.empty() ? "?" : data_value_repr)
               << " [type='" << datatools::get_label_from_type(_record_->get_parameter_model().get_type()) << "']"
               << std::endl;
        }

        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Children: [" << get_children().count() << ']' << std::endl;
        for (QList<tree_item *>::const_iterator i = get_children().begin();
             i != get_children().end();
             i++) {
          out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
          std::ostringstream indent2;
          indent2 << indent_ << datatools::i_tree_dumpable::skip_tag;
          QList<tree_item *>::const_iterator j = i;
          if (++j == get_children().end()) {
            out_ << datatools::i_tree_dumpable::last_tag;
            indent2 << datatools::i_tree_dumpable::last_skip_tag;
          } else {
            out_ << datatools::i_tree_dumpable::tag;
            indent2 << datatools::i_tree_dumpable::skip_tag;
          }
          const tree_item * a_child = *i;
          out_ << "Child '" << a_child->data(0).toString().toStdString() << "' :" <<  std::endl;
          a_child->tree_dump(out_, "", indent2.str());
        }

        out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
             << "Parent: [" << _parent_ << ']' << std::endl;

        return;
      }

      void tree_item::append_child(tree_item * child_)
      {
        if (child_) {
          _children_.append(child_);
          child_->set_parent(this);
        }
        return;
      }

      void tree_item::set_parent(tree_item * parent_)
      {
        _parent_ = parent_;
        return;
      }

      void tree_item::set_logging(datatools::logger::priority p_)
      {
        _logging_ = p_;
        return;
      }

      datatools::logger::priority tree_item::get_logging() const
      {
        return _logging_;
      }

      tree_item * tree_item::grab_child(int row_)
      {
        return _children_.value(row_);
      }

      const tree_item * tree_item::get_child(int row_) const
      {
        return _children_.value(row_);
      }

      int tree_item::get_child_count() const
      {
        return _children_.count();
      }

      int tree_item::get_column_count() const
      {
        return 4;
      }

      variant_record & tree_item::grab_record()
      {
        return *_record_;
      }

      const variant_record & tree_item::get_record() const
      {
        return *_record_;
      }

      int tree_item::get_row() const
      {
        if (_parent_) {
          return _parent_->_children_.indexOf(const_cast<tree_item*>(this));
        }
        return 0;
      }

      tree_item * tree_item::grab_parent()
      {
        return _parent_;
      }

      bool tree_item::has_parent() const
      {
        return _parent_ != 0;
      }

      const tree_item * tree_item::get_parent() const
      {
        return _parent_;
      }

      QVariant tree_item::data(int column_) const
      {
        if (column_ == tree_item::CI_KEY) {

          // Return the key of the configuration variant record:
          return QString::fromStdString(_record_->get_leaf_name());

        } else if (column_ == tree_item::CI_TYPE) {

          if (_record_->is_variant()) {
            return QVariant();
            // return QString("[variant]");
          } else if (_record_->is_parameter()) {
            const parameter_model & par_mod = _record_->get_parameter_model();

            if (par_mod.is_boolean()) {
              return QString("boolean");
            }

            if (par_mod.is_integer()) {
              return QString("integer");
            }

            if (par_mod.is_real()) {
              return QString("real");
            }

            if (par_mod.is_string()) {
              if (par_mod.is_path()) {
                return QString("path");
              }
              return QString("string");
            }

          } // _record_->is_parameter()

        } else if (column_ == tree_item::CI_VALUE) {

          if (_record_->is_parameter()) {
            const parameter_model & par_mod = _record_->get_parameter_model();

            if (_record_->value_is_set()) {

              // Boolean:
              if (par_mod.is_boolean()) {
                bool val;
                command::returned_info cri = _record_->get_boolean_value(val);
                if (cri.is_success()) {
                 return QVariant(val);
                }
              } // is_boolean

              // Integer:
              if (par_mod.is_integer()) {
                int val;
                command::returned_info cri = _record_->get_integer_value(val);
                if (cri.is_success()) {
                  return QVariant(val);
                }
              } // is_integer

              // Real:
              if (par_mod.is_real()) {
                // double val;
                // command::returned_info cri = _record_->get_real_value(val);
                std::string val_str;
                command::returned_info cri = _record_->value_to_string(val_str);
                if (cri.is_success()) {
                  return QString::fromStdString(val_str);
                }
              } // is_real

              // String:
              if (par_mod.is_string()) {
                std::string val;
                command::returned_info cri = _record_->get_string_value(val);
                if (cri.is_success()) {
                  return QString::fromStdString(val);
                }
              } // is_string

            } /* value_is_set */ else {
              return QString(tr("__unset__")); // Use: ascii_io::unset_label() ?
            } // ! value_is_set

          } // _record_->is_parameter()

        } else if (column_ == tree_item::CI_DESCRIPTION) {

          if (_record_->is_parameter()) {
            std::string desc = _record_->get_parameter_model().get_terse_description();
            //std::cerr << "[devel] desc = " << desc << "\n";
            if (_record_->has_parent()) {
              //std::cerr << "[devel] record has parent\n";
              const variant_record & vrec = _record_->get_parent();
              const variant_model & vmod = vrec.get_variant_model();
              //std::cerr << "[devel] variant model = " << vmod.get_name() << "\n";
              if (vmod.has_parameter(_record_->get_base_name())) {
                //std::cerr << "[devel] variant model has parameter " << _record_->get_leaf_name() << "\n";
                std::string par_desc = vmod.get_parameter_description(_record_->get_base_name());
                //std::cerr << "[devel] parameter description = " << par_desc << "\n";
                if (!par_desc.empty()) {
                  desc = par_desc;
                }
              }
            }
            return QString::fromStdString(desc);
          }

          if (_record_->is_variant()) {
            std::string desc = _record_->get_variant_model().get_terse_description();
            if (_record_->has_parent()) {
              const variant_record & vrec = _record_->get_parent();
              const parameter_model & pmod = vrec.get_parameter_model();
              if (pmod.has_variant(_record_->get_leaf_name())) {
                std::string var_desc = pmod.get_variant_description(_record_->get_leaf_name());
                if (!var_desc.empty()) {
                  desc = var_desc;
                }
              }
            }
            return QString::fromStdString(desc);
          }

        }

        return QVariant();
      }

      bool tree_item::unsetData(int column_)
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        int error = 0;
        if (column_ == CI_VALUE) {
          if (_record_->is_parameter()) {
            // const parameter_model & par_mod = _record_->get_parameter_model();
            command::returned_info cri = _record_->unset_value();
            if (!cri.is_success()) {
              error = cri.get_error_code();
              // Error signal
              DT_LOG_ERROR(_logging_, cri.get_error_message());
            }
          } // _record_->is_parameter()
        } // column_ == CI_VALUE
        DT_LOG_TRACE_EXITING(_logging_);
        return (error == 0);
      }

      bool tree_item::setData(int column_, const QVariant & value_)
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        int error = 0;
        if (column_ == CI_VALUE) {
          if (_record_->is_parameter()) {
            const parameter_model & par_mod = _record_->get_parameter_model();

            if (par_mod.is_boolean()) {
              DT_LOG_DEBUG(_logging_, "Boolean...");
              bool val = value_.toBool();
              command::returned_info cri = _record_->set_boolean_value(val);
              if (!cri.is_success()) {
                error = cri.get_error_code();
                // Error signal
                DT_LOG_ERROR(_logging_, cri.get_error_message());
              }

            } // boolean

            if (par_mod.is_integer()) {
              DT_LOG_DEBUG(_logging_, "Integer...");
              bool  ok = false;
              int val = value_.toInt(&ok);
              if (ok) {
                command::returned_info cri = _record_->set_integer_value(val);
                if (!cri.is_success()) {
                  error = cri.get_error_code();
                  // Error signal
                  DT_LOG_ERROR(_logging_, cri.get_error_message());
                }
              } else {
                // Error signal
              }
            } // integer

            if (par_mod.is_real()) {
              DT_LOG_DEBUG(_logging_, "Real...");
              auto old_logging = _record_->get_logging();
              if (datatools::logger::is_debug(_logging_)) {
                _record_->set_logging(datatools::logger::PRIO_DEBUG);
              }
              std::string val_str = value_.toString().toStdString();
              DT_LOG_DEBUG(_logging_, "Real value repr = '" << val_str << "'");
              command::returned_info cri = _record_->string_to_value(val_str);
              if (!cri.is_success()) {
                error = cri.get_error_code();
                // Error signal
                DT_LOG_ERROR(_logging_, cri.get_error_message());
              }
              DT_LOG_DEBUG(_logging_, "Real parameter record: ");
              if (datatools::logger::is_debug(_logging_)) {
                _record_->tree_dump(std::cerr, "", "[debug] ");
                _record_->set_logging(old_logging);
              }
            } // real

            if (par_mod.is_string()) {
              DT_LOG_DEBUG(_logging_, "String...");
              std::string val = value_.toString().toStdString();
              command::returned_info cri = _record_->set_string_value(val);
              if (!cri.is_success()) {
                error = cri.get_error_code();
                // Error signal
                DT_LOG_ERROR(_logging_, cri.get_error_message());
              }
            } // string

          } // _record_->is_parameter()

        } // column_ == CI_VALUE

        DT_LOG_TRACE_EXITING(_logging_);
        return (error == 0);
      }

      // *******************************************************

      // static
      const std::string & variant_registry_tree_model::default_top_label()
      {
        static const std::string _tl("__top__");
        return _tl;
      }


      variant_registry_tree_model::variant_registry_tree_model(QObject * parent_)
        : QAbstractItemModel(parent_)
      {
        _logging_ = datatools::logger::PRIO_FATAL;
        _read_only_ = true;
        _registry_ = 0;
        _root_ = 0;
        return;
      }

      variant_registry_tree_model::~variant_registry_tree_model()
      {
        if (_root_) {
          delete _root_;
        }
        return;
      }

      void variant_registry_tree_model::set_registry_name(const std::string & registry_name_)
      {
        _registry_name_ = registry_name_;
        emit sig_registry_name_changed();
        return;
      }

      const std::string & variant_registry_tree_model::get_registry_name() const
      {
        return _registry_name_;
      }

      bool variant_registry_tree_model::is_read_only() const
      {
        return _read_only_;
      }

      void variant_registry_tree_model::set_read_only(bool ro_)
      {
        if (_read_only_ != ro_) {
          _read_only_ = ro_;
          emit sig_read_only_changed(_read_only_);
        }
        return;
      }

      datatools::logger::priority variant_registry_tree_model::get_logging() const
      {
        return _logging_;
      }

      void variant_registry_tree_model::set_logging(datatools::logger::priority p_)
      {
        if (_logging_ != p_) {
          _logging_ = p_;
          emit sig_logging_changed();
        }
        return;
      }

      tree_item * variant_registry_tree_model::node_from_index(const QModelIndex & index_) const
      {
        return _node_from_index_(index_);
      }

      bool variant_registry_tree_model::has_root() const
      {
        return _root_ != 0;
      }

      void variant_registry_tree_model::set_root(tree_item * node_)
      {
        delete _root_;
        _root_ = node_;
        //reset();
        return;
      }

      tree_item * variant_registry_tree_model::grab_root()
      {
        return _root_;
      }

      const tree_item * variant_registry_tree_model::get_root() const
      {
        return _root_;
      }

      QModelIndex variant_registry_tree_model::rootIndex() const
      {
        /*
        if (has_root()) {
          return createIndex(0, 0);
        }
        */
        return QModelIndex();
      }

      tree_item * variant_registry_tree_model::_node_from_index_(const QModelIndex & index_) const
      {
        tree_item * found_node = 0;
        if (! index_.isValid()) {
          found_node = _root_;
        } else {
          found_node = static_cast<tree_item *>(index_.internalPointer());
        }
        DT_LOG_TRACE(_logging_,
                     "Variant record path = '"
                     << found_node->get_record().get_path() << "'");
        return found_node;
      }

      QModelIndex variant_registry_tree_model::index(int row_,
                                                     int column_,
                                                     const QModelIndex & parent_) const
      {
        if (! hasIndex(row_, column_, parent_)) {
          return QModelIndex();
        }
        if (!_root_) {
          return QModelIndex();
        }
        tree_item * parent_node = _node_from_index_(parent_);
        tree_item * child_node = parent_node->grab_child(row_);
        if (child_node) {
          return createIndex(row_, column_, child_node);
        } else {
          return QModelIndex();
        }
      }

      QModelIndex variant_registry_tree_model::parent(const QModelIndex & index_) const
      {
        if (! index_.isValid()) {
          return QModelIndex();
        }
        tree_item * child_node = _node_from_index_(index_);
        if (! child_node) {
          return QModelIndex();
        }
        tree_item * parent_node = child_node->grab_parent();
        if (! parent_node) {
          return QModelIndex();
        }
        if (parent_node == _root_) {
          return QModelIndex();
        }
        return createIndex(parent_node->get_row(), 0, parent_node);
      }

      Qt::ItemFlags variant_registry_tree_model::flags(const QModelIndex & index_) const
      {
        if (! index_.isValid()) {
          return Qt::ItemIsEnabled;
        }
        tree_item * a_node = _node_from_index_(index_);
        
        // Qt::ItemFlags f = Qt::NoItemFlags;
        Qt::ItemFlags f;
        if (a_node->get_record().is_active()) {
          f |= Qt::ItemIsEnabled;
          // f |= Qt::ItemIsSelectable;
          if (! is_read_only()) {
            if (a_node->get_record().is_parameter()) {
              const parameter_model & par_mod = a_node->get_record().get_parameter_model();
              if (par_mod.is_variable()) {
                if (index_.column() == tree_item::CI_VALUE) {
                  f |= Qt::ItemIsEditable;
                }
              }
            }
          }
        }
        return f;
      }

      // The number of rows for a given item is simply how many children it has.
      int variant_registry_tree_model::rowCount(const QModelIndex & parent_) const
      {
        if (parent_.column() > 0) {
          return 0;
        }
        tree_item * parent_node = _node_from_index_(parent_);
        return parent_node->get_children().count();
      }

      int variant_registry_tree_model::columnCount(const QModelIndex & parent_) const
      {
        if (parent_.isValid()) {
          tree_item * parent_node = _node_from_index_(parent_);
          return parent_node->get_column_count();
        } else {
          return _root_->get_column_count();
        }
      }

      QVariant variant_registry_tree_model::data(const QModelIndex & index_,
                                                 int role_) const
      {
        if ( !index_.isValid() ) {
          return QVariant();
        }
        tree_item * a_node = _node_from_index_(index_);
        if (!a_node) {
          return QVariant();
        }

        if ( role_ == Qt::BackgroundRole ) {
          if (a_node->get_record().is_variant()) {
            if ((index_.row() % 2) == 0) {
              return QColor(210,210,255);
            } else {
              return QColor(230,230,255);
            }
          }
          if (a_node->get_record().is_parameter()) {
            if (index_.column() == tree_item::CI_VALUE) {
               if (a_node->get_record().is_active() && !a_node->get_record().value_is_set()) {
                return QColor(255,75,25);
              }
            }
            if ((index_.row() % 2) == 0) {
              return QColor(Qt::white);
            } else {
              return QColor(240,240,240);
            }
          }
        }

        if ( role_ == Qt::ForegroundRole ) {
          if (a_node->get_record().is_parameter()) {
            if (index_.column() == tree_item::CI_VALUE) {
              const parameter_model & par_mod  = a_node->get_record().get_parameter_model();
              if (!par_mod.is_variable()) {
                return QColor(192,128,128);
              }
            }
          }
        }

        if ( role_ == Qt::ToolTipRole ) {

          if (a_node->get_record().is_variant()) {
            // Variant:
            std::ostringstream tt_text;
            tt_text << "Configuration variant:\n";
            if (a_node->get_record().get_variant_model().has_terse_description()) {
              tt_text << a_node->get_record().get_variant_model().get_terse_description() << "\n";
            }
            tt_text << "Full path: '" << a_node->get_record().get_path() << "'\n";
            return QString::fromStdString(tt_text.str());
          } else if (a_node->get_record().is_parameter()) {
            // Parameter:
            const parameter_model & par_mod =
              a_node->get_record().get_parameter_model();
            double unit = 1.0;
            std::string display_unit_symbol;
            if (par_mod.is_real() && par_mod.has_real_unit_label()) {
              display_unit_symbol =
                units::get_default_unit_symbol_from_label(par_mod.get_real_unit_label());
              unit = units::get_unit(display_unit_symbol);
            } // real/unit

            if (par_mod.is_real() && par_mod.has_real_preferred_unit()) {
              display_unit_symbol = par_mod.get_real_preferred_unit();
              unit = units::get_unit(par_mod.get_real_preferred_unit());
            } // real/unit
            DT_LOG_NOTICE(_logging_, "Unit = " << unit << "!");

            std::ostringstream tt_text;
            tt_text << "Configuration parameter:\n";
            if (par_mod.has_terse_description()) {
              tt_text << par_mod.get_terse_description() << "\n";
            }
            tt_text << "Full path: '" << a_node->get_record().get_path() << "'\n";
            tt_text << "Type : ";
            if (par_mod.is_path_input()) {
              tt_text << "Input path"  << "\n";
            } else if (par_mod.is_path_output()) {
              tt_text << "Output path" << "\n";
            } else {
              tt_text << get_label_from_type(par_mod.get_type()) << "\n";
            }
            if (par_mod.is_fixed()) {
              tt_text << "Fixed value : ";
              if (par_mod.is_boolean()) {
                io::write_boolean(tt_text, par_mod.get_fixed_boolean());
              } else if (par_mod.is_integer()) {
                io::write_integer(tt_text, par_mod.get_fixed_integer());
              } else if (par_mod.is_real()){
                io::write_real_number(tt_text, par_mod.get_fixed_real(),
                                      15, par_mod.get_real_preferred_unit(),
                                      par_mod.get_real_unit_label());

                // tt_text << par_mod.get_default_real() / unit;
                // if (!display_unit_symbol.empty()) {
                //   tt_text << " " << display_unit_symbol;
                // }
              } else if (par_mod.is_string()) {
                io::write_quoted_string(tt_text, par_mod.get_fixed_string());
              }
              tt_text << "\n";

            } else if (par_mod.is_variable()) {
              tt_text << "Mutable value : ";
              if (par_mod.is_free()) {
                tt_text << "free" << "\n";
              } else if (par_mod.is_enumeration()) {
                tt_text << "enumeration" << "\n";
              } else if (par_mod.is_interval()) {
                tt_text << "interval ";
                if (par_mod.is_integer()) {
                  tt_text << par_mod.get_integer_domain() << "\n";
                }
                if (par_mod.is_real()) {
                  tt_text << par_mod.get_real_domain() << "\n";
                }
              }
              if (par_mod.has_default_value()) {
                tt_text << "Default value: ";
                if (par_mod.is_boolean()) {
                  io::write_boolean(tt_text, par_mod.get_default_boolean());
                } else if (par_mod.is_integer()) {
                  io::write_integer(tt_text, par_mod.get_default_integer());
                } else if (par_mod.is_real()){
                  io::write_real_number(tt_text, par_mod.get_default_real(),
                                        15, par_mod.get_real_preferred_unit(),
                                        par_mod.get_real_unit_label());

                  // tt_text << par_mod.get_default_real() / unit;
                  // if (!display_unit_symbol.empty()) {
                  //   tt_text << " " << display_unit_symbol;
                  // }
                } else if (par_mod.is_string()) {
                  io::write_quoted_string(tt_text, par_mod.get_default_string());
                }
                tt_text << '\n';
              } else {
                tt_text << "No default value\n";
              }
            }

            return QString::fromStdString(tt_text.str());
          } // a_node->get_record().is_parameter()

        // } else if (role_ == Qt::CheckStateRole) {
        //   if (index_.column() == tree_item::CI_VALUE) {
        //     if (a_node->get_record().is_parameter()) {
        //       // Specific handling of booleans:
        //       if (a_node->get_record().get_parameter_model().is_boolean()) {
        //         QVariant bv = a_node->data(tree_item::CI_VALUE);
        //         if (bv.toBool()) {
        //           return Qt::Checked;
        //         } else {
        //           return Qt::Unchecked;
        //         }
        //       } // is_boolean
        //     } // is_parameter
        //   } // CI_VALUE
        } else if (role_ == Qt::DisplayRole) {

          if (index_.column() == tree_item::CI_KEY) {

            return a_node->data(tree_item::CI_KEY);

          } else if (index_.column() == tree_item::CI_TYPE) {

            return a_node->data(tree_item::CI_TYPE);

          } else if (index_.column() == tree_item::CI_VALUE) {

            if (a_node->get_record().is_parameter()) {

              // Boolean:
              if (a_node->get_record().get_parameter_model().is_boolean()) {
                return a_node->data(tree_item::CI_VALUE);

                /*
                // Booleans are handled through the CheckStateRole (see below):
                QVariant bv = a_node->data(tree_item::CI_VALUE);
                if (bv.toBool()) {
                return Qt::Checked;
                } else {
                return Qt::Unchecked;
                }
                */
              }

              // Integer:
              if (a_node->get_record().get_parameter_model().is_integer()) {
                return a_node->data(tree_item::CI_VALUE);
              }

              // Real:
              if (a_node->get_record().get_parameter_model().is_real()) {
                return a_node->data(tree_item::CI_VALUE);
              }

              // String:
              if (a_node->get_record().get_parameter_model().is_string()) {
                return a_node->data(tree_item::CI_VALUE);
              }

            } // is_parameter

          } else if (index_.column() == tree_item::CI_DESCRIPTION) {

            if (a_node->get_record().is_parameter()) {
              std::string desc = a_node->get_record().get_parameter_model().get_terse_description();
              //std::cerr << "[devel] desc = " << desc << "\n";
              if (a_node->get_record().has_parent()) {
                //std::cerr << "[devel] record has parent\n";
                const variant_record & vrec = a_node->get_record().get_parent();
                const variant_model & vmod = vrec.get_variant_model();
                //std::cerr << "[devel] variant model = " << vmod.get_name() << "\n";
                std::string base_name = a_node->get_record().get_base_name();
                if (vmod.has_parameter(base_name)) {
                  //std::cerr << "[devel] variant model has parameter '" << base_name << "'\n";
                  std::string par_desc = vmod.get_parameter_description(base_name);
                  //std::cerr << "[devel] parameter description = " << par_desc << "\n";
                  if (!par_desc.empty()) {
                    desc = par_desc;
                  }
                }
              }
              return QString::fromStdString(desc);
            } // is_parameter

            if (a_node->get_record().is_variant()) {
              std::string desc = a_node->get_record().get_variant_model().get_terse_description();
              if (a_node->get_record().has_parent()) {
                const variant_record & vrec = a_node->get_record().get_parent();
                const parameter_model & pmod = vrec.get_parameter_model();
                if (pmod.has_variant(a_node->get_record().get_leaf_name())) {
                  std::string var_desc = pmod.get_variant_description(a_node->get_record().get_leaf_name());
                  if (!var_desc.empty()) {
                    desc = var_desc;
                  }
                }
              }
              return QString::fromStdString(desc);
            } // is_variant

          }

        }
        return QVariant();
      }

      bool variant_registry_tree_model::unsetData(const QModelIndex & index_,
                                                  int role_)
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        tree_item * a_node = _node_from_index_(index_);
        if (!a_node) {
          return false;
        }
        if (index_.column() != tree_item::CI_VALUE) {
          return false;
        }

        int error = 0;
        if (role_ == Qt::EditRole) {
          if (a_node->get_record().is_parameter()) {
            const parameter_model & par_mod = a_node->get_record().get_parameter_model();
            if (par_mod.is_variable()) {
              bool ok = a_node->unsetData(tree_item::CI_VALUE);
              if (!ok) {
                DT_LOG_ERROR(_logging_, "unsetData failed!");
                error = 1;
              }
            } // par_mod.is_variable()
          } // is_parameter
        }
        if (error == 0) {
          emit dataChanged(index_, index_);
        }
        return error == 0;
      }


      bool variant_registry_tree_model::setData(const QModelIndex & index_,
                                                const QVariant & value_,
                                                int role_)
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        tree_item * a_node = _node_from_index_(index_);
        if (!a_node) {
          return false;
        }
        if (index_.column() != tree_item::CI_VALUE) {
          return false;
        }

        int error = 0;
        if (role_ == Qt::EditRole) {

          if (a_node->get_record().is_parameter()) {

            const parameter_model & par_mod = a_node->get_record().get_parameter_model();

            if (par_mod.is_variable()) {

              // Boolean:
              if (par_mod.is_boolean()) {
                DT_LOG_TRACE(_logging_, "Boolean = " << value_.toString().toStdString());
                bool ok = a_node->setData(tree_item::CI_VALUE, value_);
                if (!ok) {
                  DT_LOG_ERROR(_logging_, "setData boolean failed!");
                  error = 1;
                }
                // Booleans could be handled through the CheckStateRole through QCheckBox...
              } // boolean

              // Integer:
              if (par_mod.is_integer()) {
                DT_LOG_TRACE(_logging_, "Integer = " << value_.toString().toStdString());
                bool ok = a_node->setData(tree_item::CI_VALUE, value_);
                if (!ok) {
                  DT_LOG_ERROR(_logging_, "setData integer failed!");
                  error = 1;
                }
              } // integer

              // Real:
              if (par_mod.is_real()) {
                DT_LOG_TRACE(_logging_, "Real = " << value_.toString().toStdString());
                bool ok = a_node->setData(tree_item::CI_VALUE, value_);
                if (!ok) {
                  DT_LOG_ERROR(_logging_, "setData real failed!");
                  error = 1;
                }
              } // real

              // String:
              if (par_mod.is_string()) {
                DT_LOG_TRACE(_logging_, "String = '" << value_.toString().toStdString() << "'");
                bool ok = a_node->setData(tree_item::CI_VALUE, value_);
                if (!ok) {
                  DT_LOG_ERROR(_logging_, "setData string failed!");
                  error = 1;
                }
              } // string

            } // par_mod.is_variable()

          } // is_parameter

        }
        /*
        else if (role_ == Qt::CheckStateRole) {
          if (index_.column() == tree_item::CI_VALUE) {
            if (a_node->get_record().is_parameter()) {

              // Booleans are handled through the CheckStateRole (see below):
              if (a_node->get_record().get_parameter_model().is_boolean()) {
                std::cerr << "DEVEL: "
                          << "variant_registry_tree_model::setData: "
                          << "Qt::CheckStateRole: "
                          << "Boolean = " << value_.toString().toStdString()
                          << std::endl;
                // bool ok = a_node->setData(tree_item::CI_VALUE, value_);
                // if (!ok) {
                //   error = 1;
                // }
                // QVariant bv = a_node->data(tree_item::CI_VALUE);
                // if (bv.toBool()) {
                //   return Qt::Checked;
                // } else {
                //   return Qt::Unchecked;
                // }
              }

            }
          }
        }*/

        if (error == 0) {
          emit dataChanged(index_, index_);
        }
        return error == 0;
      }

      QVariant variant_registry_tree_model::headerData(int section_,
                                                       Qt::Orientation orientation_,
                                                       int role_) const
      {
        if (orientation_ == Qt::Horizontal && role_ == Qt::DisplayRole) {
          if (section_ == tree_item::CI_KEY) {
            return tr("Key");
          } else if (section_ == tree_item::CI_TYPE) {
            return tr("Type");
          } else if (section_ == tree_item::CI_VALUE) {
            return tr("Value");
          } else if (section_ == tree_item::CI_DESCRIPTION) {
            return tr("Description");
          }
        }
        return QVariant();
      }

      tree_item * variant_registry_tree_model::_construct_node_(variant_registry & registry_,
                                                                variant_record & record_)
      {
        tree_item * the_node = 0;

        if (record_.is_variant()) {
          DT_LOG_TRACE(_logging_, "Constructing variant node '" << record_.get_path() << "'...");
          DT_LOG_TRACE(_logging_, " -> number of parameter variants = " << record_.get_daughters().size());
          the_node = new tree_item(record_);
          // Build the tree node using the ranked parameters:
          std::vector<std::string> ranked_param_records;
          record_.build_list_of_ranked_parameter_records(ranked_param_records);
          for (std::size_t irank = 0; irank < ranked_param_records.size(); irank++) {
            const std::string & param_child_leaf_name = ranked_param_records[irank];
            variant_record & child_param = record_.grab_daughters().find(param_child_leaf_name)->second.grab_record();
            DT_LOG_TRACE(_logging_, " -> parameter child = '" << param_child_leaf_name << "'");
            // if (!child_param) {
            //   continue;
            // }
            // if (!child_param->is_parameter()) {
            //   continue;
            // }
            tree_item * child_node = _construct_node_(registry_, child_param);
            if (child_node) {
              the_node->append_child(child_node);
            }
          }
          if (_logging_ == datatools::logger::PRIO_TRACE) {
            the_node->tree_dump(std::cerr, "New variant node: ", "TRACE: ");
          }
        }

        if (record_.is_parameter()) {
          DT_LOG_TRACE(_logging_, "Constructing parameter node '" << record_.get_path() << "'...");
          the_node = new tree_item(record_);
          DT_LOG_TRACE(_logging_, " -> number of children variants = " << record_.get_daughters().size());
          for (variant_record::daughter_dict_type::iterator i = record_.grab_daughters().begin();
               i != record_.grab_daughters().end();
               i++) {
            DT_LOG_TRACE(_logging_, " -> variant child = '" << i->first << "'");
            variant_record & child_variant = i->second.grab_record();
            // if (!child_variant) {
            //   continue;
            // }
            // if (!child_variant->is_variant()) {
            //   continue;
            // }
            tree_item * child_node = _construct_node_(registry_, child_variant);
            if (child_node) {
              the_node->append_child(child_node);
            }
            if (_logging_ == datatools::logger::PRIO_TRACE) {
              the_node->tree_dump(std::cerr, "New parameter node: ", "TRACE: ");
            }
          }
        }

        return the_node;
      }

      void variant_registry_tree_model::construct(variant_registry & registry_,
                                                  const std::string & registry_name_)
      {
        DT_LOG_TRACE(_logging_, "Entering...");
        if (registry_.has_parent_repository()) {
          set_read_only(! registry_.get_parent_repository().get_ui_config().writable_at_start);
        }
        std::string top_variant_name = variant_registry::default_top_variant_name();
        // if (top_variant_name.empty()) {
        //   top_variant_name = registry_.get_top_variant_name();
        // }
        DT_THROW_IF(!registry_.has_variant_record(top_variant_name),
                    std::logic_error,
                    "Registry '" << registry_.get_name() << "' does not have a variant record named '"
                    << top_variant_name << "'!");
        variant_record & top_record = registry_.grab_variant_record(top_variant_name);

        // Recursive construction:
        tree_item * root = _construct_node_(registry_, top_record);
        DT_THROW_IF(root == 0,
                    std::logic_error,
                    "Cannot construct root node from top variant record '" << top_variant_name << "'!");
        _registry_ = &registry_;
        set_root(root);
        std::string rn = registry_name_;
        if (rn.empty()) {
          if (registry_.has_name()) {
            set_registry_name(registry_.get_name());
          } else {
            set_registry_name(std::string("Variant registry [") + top_record.get_variant_model().get_name() + ']');
          }
        } else {
          set_registry_name(rn);
        }
        //slot_store_to_restore_buffer();
        // Each time a data is changed, one should signal that some status should be re-fetched
        // from the registry (example: the 'variant_registry::is_accomplished' method )
        connect(this, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
                this, SLOT(slot_broadcast_data_change(const QModelIndex &, const QModelIndex &)));
        // connect(this, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
        //         this, SIGNAL(sig_registry_data_changed()));
        DT_LOG_TRACE(_logging_, "Exiting.");
        return;
      }

      bool variant_registry_tree_model::has_registry() const
      {
        return _registry_ != nullptr;
      }

      variant_registry & variant_registry_tree_model::grab_registry()
      {
        DT_THROW_IF(!has_registry(), std::logic_error, "Model has no registry!");
        return *_registry_;
      }

      const variant_registry & variant_registry_tree_model::get_registry() const
      {
        DT_THROW_IF(!has_registry(), std::logic_error, "Model has no registry!");
        return *_registry_;
      }

      void variant_registry_tree_model::tree_dump(std::ostream & out_,
                                                  const std::string & title_,
                                                  const std::string & indent_,
                                                  bool inherit_) const {
        if (!title_.empty()) {
          out_ << indent_ << title_ << " : " << std::endl;
        }

        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Logging: '" << datatools::logger::get_priority_label(_logging_) << "' "
             << std::endl;

        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Registry name: '" << _registry_name_ << "'"
             << std::endl;

        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Registry: [" << _registry_ << "]"
             << std::endl;

        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Read-only : " << _read_only_
             << std::endl;

        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Restore buffer : " << has_restore_buffer()
             << std::endl;

        out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
             << "Root : [" << _root_ << "] "
             << std::endl;

        std::ostringstream indent2;
        indent2 << indent_ << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
        _root_->tree_dump(out_, "", indent2.str(), inherit_);

        return;
      }

      void variant_registry_tree_model::slot_broadcast_data_change(const QModelIndex & i0_,
                                                                   const QModelIndex & /*i1_*/)
      {
         // std::cerr << "DEVEL: variant_registry_tree_model::slot_broadcast_data_change: for registry='"
         //           << _registry_name_ << "'"
         //           << std::endl;
         tree_item * node = node_from_index(i0_);
         const variant_record & vrec = node->get_record();
         if (vrec.is_parameter()) {
           std::string data_path = vrec.get_path();
           // std::cerr << "DEVEL: variant_registry_tree_model::slot_broadcast_data_change: "
           //           << "data '" << data_path << "' has changed in registry='"
           //           << _registry_name_ << "'"
           //           << std::endl;
           emit sig_registry_data_changed(_registry_name_, data_path);
         }
         return;
      }

      void variant_registry_tree_model::slot_load_from_reg_file(const QString & in_reg_filename_)
      {
         std::string reg_file_name = in_reg_filename_.toStdString();
         // std::cerr << "DEVEL: reg_file_name='" << reg_file_name << "'" << std::endl;

         std::ifstream reg_file;
         reg_file.open(reg_file_name.c_str());
         if (!reg_file) {
           DT_THROW_IF(true, std::runtime_error, "Cannot open file '"  << reg_file_name << "'!");
         }
         this->beginResetModel();
         ascii_io reg_io;
         // reg_io.set_logging(logger::PRIO_TRACE);
         int error = reg_io.load_registry(reg_file, this->grab_registry());
         if (error) {
           DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Failed to load registry from file '" << reg_file_name << "'!");
         }
         // std::cerr << "DEVEL: registry was loaded from file '" << reg_file_name << "'" << std::endl;
         this->endResetModel();
         emit sig_registry_changed(_registry_name_);
         return;
      }

      void variant_registry_tree_model::slot_store_to_reg_file(const QString & out_reg_filename_) const
      {
        std::string reg_file_name = out_reg_filename_.toStdString();
        // std::cerr << "DEVEL: reg_file_name='" << reg_file_name << "'" << std::endl;

        std::ofstream reg_file;
        reg_file.open(reg_file_name.c_str());
        if (!reg_file) {
          DT_THROW_IF(true, std::runtime_error, "Cannot open file '"  << reg_file_name << "'!");
        }
        ascii_io reg_io;
        reg_io.store_registry(reg_file, this->get_registry());
        // std::cerr << "DEVEL: registry was stored in file '" << reg_file_name << "'" << std::endl;
        return;
      }

      bool variant_registry_tree_model::has_restore_buffer() const
      {
        return _restore_buffer_.get() ? true : false;
      }

      void variant_registry_tree_model::reset_restore_buffer()
      {
        _restore_buffer_.reset();
        return;
      }

      void variant_registry_tree_model::slot_store_to_restore_buffer(std::string * buffer_)
      {
        // std::cerr << "DEVEL: variant_registry_tree_model::slot_store_to_restore_buffer: Entering..." << std::endl;
        if (!has_registry()) {
          return;
        }
        std::string * buf = buffer_;
        if (! buf) {
          if (!has_restore_buffer()) {
            _restore_buffer_.reset(new std::string);
          }
          buf = _restore_buffer_.get();
        }
        std::ostringstream out;
        ascii_io rep_io;
        rep_io.store_registry(out, this->get_registry());
        *buf = out.str();
        // std::cerr << "DEVEL: variant_registry_tree_model::slot_store_to_restore_buffer: Exiting." << std::endl;
        return;
      }

      void variant_registry_tree_model::slot_load_from_restore_buffer(const std::string * buffer_)
      {
        if (!has_registry()) {
          return;
        }
        const std::string * buf = buffer_;
        if (!buf) {
          if (!has_restore_buffer()) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "No available restore buffer!");
            return;
          }
          buf =_restore_buffer_.get();
        }
        std::istringstream in(*buf);
        ascii_io rep_io;
        // std::cerr << "DEVEL: variant_registry_tree_model::slot_load_from_restore_buffer: "
        //           << "About to load the registry...."
        //           << std::endl;
        // rep_io.print(std::cerr);
        this->beginResetModel();
        int error = rep_io.load_registry(in, this->grab_registry());
        if (error) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Failed to load repository from restore buffer!");
        }
        this->endResetModel();
        emit sig_registry_changed(_registry_name_);
        return;
      }

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools
