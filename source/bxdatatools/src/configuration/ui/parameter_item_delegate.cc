// datatools/configuration/ui/parameter_item_delegate.cc
/*
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

// see :
// Qt source: qitemeditorfactory.h
// http://qt-project.org/doc/qt-4.8/dialogs-standarddialogs-dialog-cpp.html

// Ourselves:
#include <datatools/configuration/ui/parameter_item_delegate.h>

// Standard library:
#include <limits>
#include <cmath>
#include <typeinfo>

// Third party:
// - Qt:
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QString>
#include <QVariant>
#include <QLineEdit>
#include <QFileDialog>
#include <QPushButton>

// This project:
#include <datatools/exception.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/configuration/ui/variant_registry_tree_model.h>
#include <datatools/configuration/variant_record.h>
#include <datatools/configuration/parameter_model.h>

namespace datatools {

  namespace configuration {

    namespace ui {

      RealDialogLauncherButton::RealDialogLauncherButton(const QModelIndex & index_,
                                                         const QString & button_text_,
                                                         QWidget * parent_)
        : QPushButton(button_text_, parent_)
      {
        // std::cerr << "DEVEL: "
        //           << "RealDialogLauncherButton::CTOR: "
        //           << "Entering..."
        //           << std::endl;
        _index_ = 0;
        _real_dialog_ = 0;
        _real_record_ = 0;
        _param_model_ = 0;
        _unit_label_ = "";
        _unit_value_ = 1.0;
        _value_ = std::numeric_limits<double>::quiet_NaN();

        _index_ = &index_;

        const variant_registry_tree_model * model
          = dynamic_cast<const variant_registry_tree_model *>(_index_->model());
        if (!model) {
          // DT_LOG_FATAL(_logging_,"NULL model!");
          return;
        }
        const tree_item * node = model->node_from_index(index_);
        if (!node) {
          // DT_LOG_FATAL(_logging_,"NULL item node!");
          return;
        }
        const variant_record & rec = node->get_record();
        if (!rec.is_parameter()) {
          // DT_LOG_FATAL(_logging_,"NOT a parameter!");
          return;
        }
        _real_record_ = &rec;
        _param_model_ = &_real_record_->get_parameter_model();

        if (_param_model_->is_real()) {
          if (_param_model_->is_free() || _param_model_->is_interval()) {
            _real_dialog_ = new QInputDialog(this);
            _real_dialog_->setWindowTitle("Real value input dialog");
            _real_dialog_->setInputMode(QInputDialog::DoubleInput);
            if (_param_model_->has_real_unit_label()) {
              _unit_label_ = _param_model_->get_real_unit_label();
              bool unit_ok = _param_model_->fetch_real_display_unit(_unit_symbol_, _unit_value_);
              if (!unit_ok) {
                _unit_symbol_.clear();
                _unit_value_ = 1.0;
              }
            }
            connect(this, SIGNAL(clicked()),
                    this, SLOT(slot_launch_real_dialog()));
            // std::cerr << "DEVEL: "
            //           << "RealDialogLauncherButton::CTOR: "
            //           << "QInputDialog is created."
            //           << std::endl;
          }
        }

        // std::cerr << "DEVEL: "
        //           << "RealDialogLauncherButton::CTOR: "
        //           << "Exiting."
        //           << std::endl;
        return;
      }

      RealDialogLauncherButton::~RealDialogLauncherButton()
      {
        // std::cerr << "DEVEL: "
        //           << "RealDialogLauncherButton::DTOR: "
        //           << "Entering..."
        //           << std::endl;
        // std::cerr << "DEVEL: "
        //           << "RealDialogLauncherButton::DTOR: "
        //           << "Exiting."
        //           << std::endl;
        return;
      }

      double RealDialogLauncherButton::get_value() const
      {
        return _value_;
      }

      double RealDialogLauncherButton::get_unit_value() const
      {
        return _unit_value_;
      }

      const std::string & RealDialogLauncherButton::get_unit_symbol() const
      {
        return _unit_symbol_;
      }

      void RealDialogLauncherButton::slot_launch_real_dialog()
      {
        // DT_LOG_TRACE(_logging_,"Entering...");
        if (_real_dialog_) {
          // DT_LOG_TRACE(_logging_,"show QInputDialog...");
          _real_dialog_->setModal(true);

          {
            int ndec = 4;
            if (_param_model_->has_real_precision()) {
              double prec = _param_model_->get_real_precision() / _unit_value_;
              ndec = -std::log10(prec) + 1;
            }
            // std::cerr << "DEVEL: ndec=" << ndec << std::endl;
            _real_dialog_->setDoubleDecimals(ndec);
          }

          {
            std::ostringstream label_oss;
            label_oss << _real_record_->get_name();
            if (!_unit_symbol_.empty()) {
              label_oss << " [" << _unit_symbol_ << "]";
            }
            _real_dialog_->setLabelText(QString::fromStdString(label_oss.str()));
            if (_param_model_->is_free()) {
              _real_dialog_->setDoubleMinimum(-std::numeric_limits<double>::max());
              _real_dialog_->setDoubleMaximum(std::numeric_limits<double>::max());
            }
            if (_param_model_->is_interval()) {
              const real_range & real_domain = _param_model_->get_real_domain();
              _real_dialog_->setDoubleMinimum(-std::numeric_limits<double>::max());
              _real_dialog_->setDoubleMaximum(std::numeric_limits<double>::max());
              // double scale_min = -1;
              // double scale_max = -1;
              if (_param_model_->has_real_precision()) {
                _real_dialog_->setDoubleDecimals(4);
              }
              if (real_domain.is_lower_bounded()) {
                _real_dialog_->setDoubleMinimum(real_domain.first() / _unit_value_);
                //scale_min = std::abs(real_domain.first() / _unit_value_);
              }
              if (real_domain.is_upper_bounded()) {
                _real_dialog_->setDoubleMaximum(real_domain.last() / _unit_value_);
                // scale_max = std::abs(real_domain.last() / _unit_value_);
              }
              //double scale = std::min(scale_min, scale_max);
            } // is_interval
            double current_real;
            command::returned_info cri = _real_record_->get_real_value(current_real);
            if (cri.is_success()) {
              double display_val = current_real / _unit_value_;
              // std::cerr << "DEVEL: display_val=" << display_val << std::endl;
              _real_dialog_->setDoubleValue(display_val);
            }
          }

          connect(_real_dialog_, SIGNAL(accepted()),
                  this, SLOT(slot_set_value()));
          _real_dialog_->show();
        }

        // DT_LOG_TRACE(_logging_,"Exiting.");
        return;
      }

      void RealDialogLauncherButton::slot_set_value()
      {
        // DT_LOG_TRACE(_logging_,"Entering...");
        if (_real_dialog_) {
          _value_ = _real_dialog_->doubleValue();
          // DT_LOG_TRACE(_logging_,"Real value is '" << _value_ << "'");
        }
        // DT_LOG_TRACE(_logging_,"Exiting.");
        return;
      }


      /* ------------------------------------*/

      FileDialogLauncherButton::FileDialogLauncherButton(const QModelIndex & index_,
                                                         const QString & button_text_,
                                                         QWidget * parent_)
        : QPushButton(button_text_, parent_)
      {
        // DT_LOG_TRACE(_logging_,"Entering...");
        // std::cerr << "DEVEL: "
        //           << "FileDialogLauncherButton::CTOR: "
        //           << "Entering..."
        //           << std::endl;
        _index_ = 0;
        _path_dialog_ = 0;
        _path_record_ = 0;
        _param_model_ = 0;

        _index_ = &index_;
        const variant_registry_tree_model * model
          = dynamic_cast<const variant_registry_tree_model *>(_index_->model());
        if (!model) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR,"NULL model!");
          return;
        }
        const tree_item * node = model->node_from_index(index_);
        if (!node) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR,"NULL item node!");
          return;
        }
        const variant_record & rec = node->get_record();
        if (!rec.is_parameter()) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR,"NOT a parameter!");
          return;
        }
        _path_record_ = &rec;
        _param_model_ = &_path_record_->get_parameter_model();
        _path_dialog_ = new QFileDialog(this);
        _path_dialog_->setViewMode(QFileDialog::Detail);
        if (_param_model_->is_path_input()) {
          _accept_mode_ = QFileDialog::AcceptOpen;
          _path_dialog_->setAcceptMode(QFileDialog::AcceptOpen);
          _path_dialog_->setFileMode(QFileDialog::ExistingFile);
        } else if (_param_model_->is_path_output()) {
          _accept_mode_ = QFileDialog::AcceptSave;
          _path_dialog_->setAcceptMode(QFileDialog::AcceptSave);
          _path_dialog_->setFileMode(QFileDialog::AnyFile);
        }
        // std::cerr << "DEVEL: "
        //           << "FileDialogLauncherButton::CTOR: "
        //           << "QFileDialog is created."
        //           << std::endl;
        connect(this, SIGNAL(clicked()),
                this, SLOT(slot_launch_file_dialog()));

        // std::cerr << "DEVEL: "
        //           << "FileDialogLauncherButton::CTOR: "
        //           << "Exiting."
        //           << std::endl;
        return;
      }

      FileDialogLauncherButton::~FileDialogLauncherButton()
      {
        // std::cerr << "DEVEL: "
        //           << "FileDialogLauncherButton::DTOR: "
        //           << "Entering..."
        //           << std::endl;
        // std::cerr << "DEVEL: "
        //           << "FileDialogLauncherButton::DTOR: "
        //           << "Exiting."
        //           << std::endl;
        return;
      }

      bool FileDialogLauncherButton::is_open() const
      {
        return _accept_mode_ == QFileDialog::AcceptOpen;
      }

      bool FileDialogLauncherButton::is_save() const
      {
        return _accept_mode_ == QFileDialog::AcceptSave;
      }

      const QString & FileDialogLauncherButton::get_path() const
      {
        return _path_;
      }

      void FileDialogLauncherButton::slot_set_path()
      {
        // std::cerr << "DEVEL: "
        //           << "FileDialogLauncherButton::slot_set_path: "
        //           << "Entering..."
        //           << std::endl;
        if (_path_dialog_) {
          QStringList paths = _path_dialog_->selectedFiles();
          if (paths.size() > 0) {
            _path_ = paths.at(0);
            // std::cerr << "DEVEL: "
            //           << "FileDialogLauncherButton::slot_set_path: "
            //           << "Path is '" << _path_.toStdString() << "'"
            //           << std::endl;
          }
        }
        // std::cerr << "DEVEL: "
        //           << "FileDialogLauncherButton::slot_set_path: "
        //           << "Exiting."
        //           << std::endl;
        return;
      }

      void FileDialogLauncherButton::slot_launch_file_dialog()
      {
        // std::cerr << "DEVEL: "
        //           << "FileDialogLauncherButton::slot_launch_file_dialog: "
        //           << "Entering..."
        //           << std::endl;

        if (_path_dialog_) {
          // std::cerr << "DEVEL: "
          //           << "FileDialogLauncherButton::slot_launch_file_dialog: "
          //           << "show QFileDialog...!"
          //           << std::endl;
          _path_dialog_->setModal(true);
          std::string current_path;
          command::returned_info cri = _path_record_->get_string_value(current_path);
          if (cri.is_success()) {
            if (!current_path.empty()) {
              datatools::fetch_path_with_env(current_path);
              _path_dialog_->selectFile(QString::fromStdString(current_path));
            }
          }
          connect(_path_dialog_, SIGNAL(accepted()),
                  this, SLOT(slot_set_path()));
          _path_dialog_->show();
        }

        // emit sig_launch_file_dialog(*_index_);
        // std::cerr << "DEVEL: "
        //           << "FileDialogLauncherButton::slot_launch_file_dialog: "
        //           << "Exiting."
        //           << std::endl;
        return;
      }

      /* -------------------------------- */

      void parameter_item_delegate::set_logging(datatools::logger::priority p_)
      {
        _logging_ = p_;
        return;
      }

      datatools::logger::priority parameter_item_delegate::get_logging() const
      {
        return _logging_;
      }

      parameter_item_delegate::parameter_item_delegate(QObject * parent_)
        : QStyledItemDelegate(parent_) // , _model_(&model_)
      {
        _logging_ = datatools::logger::PRIO_FATAL;
        return;
      }

      parameter_item_delegate::~parameter_item_delegate()
      {
        return;
      }

      QWidget * parameter_item_delegate::createEditor(QWidget * parent_,
                                                      const QStyleOptionViewItem & option_,
                                                      const QModelIndex & index_) const
      {
        QWidget * editor = 0;

        // Only for value column:
        if (index_.column() == tree_item::CI_VALUE) {
          const variant_registry_tree_model * model = dynamic_cast<const variant_registry_tree_model *>(index_.model());
          tree_item * node = model->node_from_index(index_);
          const variant_record & rec = node->get_record();

          // Only for parameters :
          if (rec.is_parameter()) {

            // std::cerr << "DEVEL: parameter_item_delegate::createEditor: CI_VALUE >>>" << std::endl;

            const parameter_model & param_model = rec.get_parameter_model();
            // Only for variable parameters :
            if (param_model.is_variable()) {

              // Boolean:
              if (param_model.is_boolean()) {
                // std::cerr << "DEVEL: parameter_item_delegate::createEditor: boolean..." << std::endl;
                QComboBox * boolean_combo = new QComboBox(parent_);
                boolean_combo->setAutoFillBackground(true);
                boolean_combo->insertItem(0, QString::fromStdString("false"));
                boolean_combo->insertItem(1, QString::fromStdString("true"));
                if (rec.value_is_set()) {
                  bool val = false;
                  command::returned_info cri = rec.get_boolean_value(val);
                  if (val) {
                    boolean_combo->setCurrentIndex(1);
                  } else {
                    boolean_combo->setCurrentIndex(0);
                  }
                }
                editor = boolean_combo;
              } // is_boolean

              // Integer:
              if (param_model.is_integer()) {
                // std::cerr << "DEVEL: parameter_item_delegate::createEditor: integer..." << std::endl;

                if (param_model.is_free()) {
                  QSpinBox * integer_sb = new QSpinBox(parent_);
                  integer_sb->setAutoFillBackground(true);
                  integer_sb->setSingleStep(1);
                  integer_sb->setMinimum(std::numeric_limits<int>::min());
                  integer_sb->setMaximum(std::numeric_limits<int>::max());
                  editor = integer_sb;
                }

                if (param_model.is_enumeration()) {
                  QComboBox * integer_combo = new QComboBox(parent_);
                  integer_combo->setAutoFillBackground(true);
                  std::vector<int> vints;
                  param_model.fetch_integer_enumeration(vints);
                  int current_index = -1;
                  for (int i = 0; i < (int) vints.size(); i++) {
                    if (rec.value_is_set()) {
                      int intval;
                      command::returned_info cri = rec.get_integer_value(intval);
                      if (cri.is_success()) {
                        if (vints[i] == intval) {
                          current_index = i;
                        }
                      }
                      std::ostringstream valint_oss;
                      valint_oss << vints[i];
                      integer_combo->insertItem(i, QString::fromStdString(valint_oss.str()));
                    }
                  } //for
                  editor = integer_combo;
                } // is_enumeration

                if (param_model.is_interval()) {
                  QSpinBox * integer_sb = new QSpinBox(parent_);
                  integer_sb->setAutoFillBackground(true);
                  const integer_range & idomain = param_model.get_integer_domain();
                  if (idomain.is_lower_bounded()) {
                    integer_sb->setMinimum(idomain.first());
                  }
                  if (idomain.is_upper_bounded()) {
                    integer_sb->setMaximum(idomain.last());
                  }
                  integer_sb->setSingleStep(1);
                  editor = integer_sb;
                } // is_interval

              } // is_integer

              // Real:
              if (param_model.is_real()) {
                // std::cerr << "DEVEL: parameter_item_delegate::createEditor: real..." << std::endl;

                if (param_model.is_free()) {

                  RealDialogLauncherButton * real_editor_button
                    = new RealDialogLauncherButton(index_, "Choose value", parent_);
                  editor = real_editor_button;

                } // is_free

                if (param_model.is_interval()) {

                  RealDialogLauncherButton * real_editor_button
                    = new RealDialogLauncherButton(index_, "Choose value", parent_);
                  editor = real_editor_button;

                } // is_interval

                if (param_model.is_enumeration()) {
                  QComboBox * real_combo = new QComboBox(parent_);
                  real_combo->setAutoFillBackground(true);
                  std::vector<double> vreals;
                  double display_unit = 1.0;
                  std::string unit_symbol;
                  std::string unit_label;
                  if (param_model.has_real_preferred_unit()) {
                    unit_symbol = param_model.get_real_preferred_unit();
                  } else if (param_model.has_real_unit_label()) {
                    unit_symbol = units::get_default_unit_symbol_from_label(param_model.get_real_unit_label());
                    std::string unit_label2;
                    bool found_unit = units::find_unit(unit_symbol, display_unit, unit_label2);
                    // DT_THROW_IF(found_unit, std::logic_error, "Cannot find unit !");
                  }
                  param_model.fetch_real_enumeration(vreals);
                  for (int i = 0; i < (int) vreals.size(); i++) {
                    std::ostringstream rval_oss;
                    io::write_real_number(rval_oss, vreals[i], 15, unit_symbol, unit_label);
                    real_combo->insertItem(i, QString::fromStdString(rval_oss.str()));
                  } // for
                  editor = real_combo;
                } // is_enumeration

              } // is_real

              // String:
              if (param_model.is_string()) {

                bool path = param_model.is_path();

                if (param_model.is_free()) {

                  if (param_model.is_path()) {
                    FileDialogLauncherButton * path_editor_button
                      = new FileDialogLauncherButton(index_, "Choose file path", parent_);
                    editor = path_editor_button;
                  }

                } // is_free

                if (param_model.is_enumeration()) {
                  QComboBox * string_combo = new QComboBox(parent_);
                  string_combo->setAutoFillBackground(true);
                  std::vector<std::string> vstrings;
                  param_model.fetch_string_enumeration(vstrings);
                  for (int i = 0; i < (int) vstrings.size(); i++) {
                    string_combo->insertItem(i, QString::fromStdString(vstrings[i]));
                  }
                  editor = string_combo;
                } // is_enumeration

              } // is_string

              if (!editor) {
                editor = QStyledItemDelegate::createEditor(parent_, option_, index_);
              }
            } // variable

            // std::cerr << "DEVEL: parameter_item_delegate::createEditor: CI_VALUE <<<" << std::endl;
          } // parameter

        } // value column


        return editor; //QStyledItemDelegate::createEditor(parent_, option_, index_);
      }

      void parameter_item_delegate::setEditorData(QWidget * editor_,
                                                  const QModelIndex & index_) const
      {
        bool sed = false;
        if (index_.column() == tree_item::CI_VALUE) {

          // Fetch the current displayed value:
          QVariant value = index_.model()->data(index_, Qt::DisplayRole);

          const variant_registry_tree_model * model = dynamic_cast<const variant_registry_tree_model *>(index_.model());
          const tree_item * node = model->node_from_index(index_);
          const variant_record & rec = node->get_record();
          if (rec.is_parameter()) {
            // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: CI_VALUE/is_parameter >>>" << std::endl;
            const parameter_model & param_model = rec.get_parameter_model();

            // Boolean:
            if (param_model.is_boolean()) {
              // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: Boolean" << std::endl;

              // Enumeration:
              QComboBox * boolean_combo = static_cast<QComboBox*>(editor_);
              boolean_combo->setCurrentIndex(boolean_combo->findText(value.toString()));
              sed = true;
            } // Boolean

            // Integer:
            if (param_model.is_integer()) {
              // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: Integer" << std::endl;

              // Free:
              if (param_model.is_free()) {
                QSpinBox * integer_sb = static_cast<QSpinBox*>(editor_);
                QString current_qstring_value = value.toString();
                integer_sb->setValue(value.toInt());
                sed = true;
              }

              // Enumeration:
              if (param_model.is_enumeration()) {
                QComboBox * integer_combo = static_cast<QComboBox*>(editor_);
                integer_combo->setCurrentIndex(integer_combo->findText(value.toString()));
                sed = true;
              }

              // Interval:
              if (param_model.is_interval()) {
                QSpinBox * integer_sb = static_cast<QSpinBox*>(editor_);
                QString current_qstring_value = value.toString();
                // std::cerr << "DEVEL: "
                //           << "parameter_item_delegate::setEditorData: "
                //           << "int value = '"
                //           << current_qstring_value.toStdString()
                //           << "'"
                //           << std::endl;
                integer_sb->setValue(value.toInt());
                sed = true;
              }

            } // Integer

            // Real:
            if (param_model.is_real()) {
              // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: Real" << std::endl;

              // Free/interval:
              if (param_model.is_free() || param_model.is_interval()) {
                // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: Free = " << param_model.is_free() << std::endl;
                // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: Interval = " << param_model.is_interval() << std::endl;
               // QLineEdit * real_line_edit = static_cast<QLineEdit*>(editor_);
                // QString current_qstring_value = value.toString();
                // real_line_edit->setText(current_qstring_value);
                RealDialogLauncherButton * real_editor_button
                  = static_cast<RealDialogLauncherButton*>(editor_);
                real_editor_button->setText("Choose value");
                sed = true;
              } // Free | interval

              // Enumeration:
              if (param_model.is_enumeration()) {
                QComboBox * real_combo = static_cast<QComboBox*>(editor_);
                real_combo->setCurrentIndex(real_combo->findText(value.toString()));
                sed = true;
              } // Enumeration

            } // Real

            // String:
            if (param_model.is_string()) {
              // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: String" << std::endl;

              // Free:
              if (param_model.is_free()) {
                // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: Free" << std::endl;

                // Path:
                if (param_model.is_path()) {
                  // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: Path" << std::endl;

                  FileDialogLauncherButton * path_editor_button
                    = static_cast<FileDialogLauncherButton*>(editor_);
                  if (path_editor_button->is_open()) {
                    path_editor_button->setText("Choose input file");
                  } else if (path_editor_button->is_save()) {
                    path_editor_button->setText("Choose output file");
                  } else {
                    path_editor_button->setText("Choose file");
                  }
                  // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: "
                  //           << "CI_VALUE/is_path = "
                            // << path_editor_button->text().toStdString()
                            // << std::endl;
                  sed = true;

                } else {

                  QLineEdit * string_line_edit = static_cast<QLineEdit*>(editor_);
                  QString current_qstring_value = value.toString();
                  string_line_edit->setText(current_qstring_value);
                  sed = true;

                }
              }

              // Enumeration:
              if (param_model.is_enumeration()) {
                // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: Enumeration" << std::endl;
                QComboBox * string_combo = static_cast<QComboBox*>(editor_);
                QString current_qstring_value = value.toString();
                // std::cerr << "DEVEL: "
                //           << "parameter_item_delegate::setEditorData: "
                //           << "string value = '"
                //           << current_qstring_value.toStdString()
                //           << "'"
                //           << std::endl;
                string_combo->setCurrentIndex(string_combo->findText(current_qstring_value));
                sed = true;
              }

            } // String
            // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: CI_VALUE/is_parameter <<<" << std::endl;

          } // is_parameter
        } // CI_VALUE

        if (!sed) {
          // http://cep.xray.aps.anl.gov/software/qt4-x11-4.2.2-browser/d9/da2/class_q_default_item_editor_factory.html
          // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: "
          //           << "editor = '"
          //           << editor_->QWidget::objectName().toStdString() << "'" << std::endl;
          // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: "
          //           << "editor = '"
          //           << typeid(*editor_).name() << "'" << std::endl;
          QStyledItemDelegate::setEditorData(editor_, index_);
        }
        return;
      }

      void parameter_item_delegate::setModelData(QWidget * editor_,
                                                 QAbstractItemModel * model_,
                                                 const QModelIndex & index_) const
      {
        bool sdm = false;

        // Only for "Value" column:
        if (index_.column() == tree_item::CI_VALUE) {
          variant_registry_tree_model * model = dynamic_cast<variant_registry_tree_model *>(model_);
          tree_item * node = model->node_from_index(index_);
          variant_record & rec = node->grab_record();

          // Only for parameters:
          if (rec.is_parameter()) {
            // std::cerr << "DEVEL: parameter_item_delegate::setModelData: CI_VALUE >>>" << std::endl;
            const parameter_model & param_model = rec.get_parameter_model();

            // Boolean:
            if (param_model.is_boolean()) {
              // sdm = true;
            } // boolean

            // Integer:
            if (param_model.is_integer()) {
              // sdm = true;
            } // integer

            // Real:
            if (param_model.is_real()) {
              if (param_model.is_free() || param_model.is_interval()) {

                RealDialogLauncherButton * real_editor_button
                  = static_cast<RealDialogLauncherButton*>(editor_);
                std::ostringstream real_value_oss;
                real_value_oss.precision(15);
                real_value_oss << real_editor_button->get_value();
                if (! real_editor_button->get_unit_symbol().empty()) {
                  real_value_oss << ' ' << real_editor_button->get_unit_symbol();
                }
                QString real_qstring = QString::fromStdString(real_value_oss.str());
                // std::cerr << "DEVEL: parameter_item_delegate::setModelData: "
                //           << "real_qstring = '" << real_qstring.toStdString() << "'"
                //           << std::endl;
                bool ok = model->setData(index_, real_qstring, Qt::EditRole);
                if (ok) {
                  sdm = true;
                } else {
                  // Error
                }

              } // is_free | is_interval
            } // real

            // String:
            if (param_model.is_string()) {

              if (param_model.is_path()) {

                FileDialogLauncherButton * path_editor_button
                  = static_cast<FileDialogLauncherButton*>(editor_);
                const QString & path = path_editor_button->get_path();
                if (!path.isEmpty()) {
                  bool ok = model->setData(index_, path, Qt::EditRole);
                  if (ok) {
                    sdm = true;
                  } else {
                    // Error
                  }
                }
              } // is_path

            } // string

            if (!sdm) {
              QStyledItemDelegate::setModelData(editor_, model_, index_);
            }
            // std::cerr << "DEVEL: parameter_item_delegate::setModelData: CI_VALUE <<<" << std::endl;
          } // is_parameter

        } // CI_VALUE
        return;
      }

      void parameter_item_delegate::updateEditorGeometry(QWidget * editor_,
                                                         const QStyleOptionViewItem & option_,
                                                         const QModelIndex & index_) const
      {
        editor_->setGeometry(option_.rect);
        // QStyledItemDelegate::updateEditorGeometry(editor_, option_, index_);
        return;
      }

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools
