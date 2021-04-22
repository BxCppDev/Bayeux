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
#include <QStandardItem>
// - Boost:
#include <boost/lexical_cast.hpp>

// This project:
#include <datatools/exception.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/configuration/ui/variant_registry_tree_model.h>
#include <datatools/configuration/variant_record.h>
#include <datatools/configuration/parameter_model.h>
#include <datatools/configuration/variant_repository.h>

namespace datatools {

  namespace configuration {

    namespace ui {

      RealDialogLauncherButton::RealDialogLauncherButton(const QModelIndex & index_,
                                                         const QString & button_text_,
                                                         QWidget * parent_)
        : QPushButton(button_text_, parent_)
      {
        _index_ = nullptr;
        _real_dialog_ = nullptr;
        _real_record_ = nullptr;
        _param_model_ = nullptr;
        _unit_label_ = "";
        _unit_value_ = 1.0;
        _value_ = std::numeric_limits<double>::quiet_NaN();
        _index_ = &index_;

        const variant_registry_tree_model * model
          = dynamic_cast<const variant_registry_tree_model *>(_index_->model());
        if (!model) {
          return;
        }
        const tree_item * node = model->node_from_index(index_);
        if (!node) {
          return;
        }
        const variant_record & rec = node->get_record();
        if (!rec.is_parameter()) {
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
          }
        }
        return;
      }

      RealDialogLauncherButton::~RealDialogLauncherButton()
      {
        DT_LOG_DEBUG(logging,
                     "Destroying the RealDialogLauncherButton...");
        return;
      }

      double RealDialogLauncherButton::get_value() const
      {
        DT_LOG_DEBUG(logging, "Raw value   = " << _value_);
        DT_LOG_DEBUG(logging, "Unit value  = " << _unit_value_);
        DT_LOG_DEBUG(logging, "Unit symbol = '" << _unit_symbol_ << "'");
        DT_LOG_DEBUG(logging, "Value       = " << _value_ << " [" << _unit_symbol_ << "]");
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
        if (_real_dialog_) {
          _real_dialog_->setModal(true);

          {
            int ndec = 4;
            if (_param_model_->has_real_precision()) {
              double prec = _param_model_->get_real_precision() / _unit_value_;
              ndec = (int) (-std::log10(prec) + 1);
            }
            _real_dialog_->setDoubleDecimals(ndec);
          }

          {
            std::ostringstream label_oss;
            label_oss << _real_record_->get_leaf_name();
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
              if (_param_model_->has_real_precision()) {
                _real_dialog_->setDoubleDecimals(4);
              }
              if (real_domain.is_lower_bounded()) {
                _real_dialog_->setDoubleMinimum(real_domain.first() / _unit_value_);
              }
              if (real_domain.is_upper_bounded()) {
                _real_dialog_->setDoubleMaximum(real_domain.last() / _unit_value_);
              }
            } // is_interval

            double current_real;
            command::returned_info cri = _real_record_->get_real_value(current_real);
            if (cri.is_success()) {
              double display_val = current_real / _unit_value_;
              _real_dialog_->setDoubleValue(display_val);
            }
          }

          connect(_real_dialog_, SIGNAL(accepted()),
                  this, SLOT(slot_set_value()));
          _real_dialog_->show();
        }

        return;
      }

      void RealDialogLauncherButton::slot_set_value()
      {
        if (_real_dialog_) {
          DT_LOG_DEBUG(logging, "Real dialog raw value = " << _real_dialog_->doubleValue());
          DT_LOG_DEBUG(logging, "Real dialog unit value = " << _unit_value_);
          DT_LOG_DEBUG(logging, "Real dialog unit symbol = '" << _unit_symbol_ << "'");
          DT_LOG_DEBUG(logging, "Real dialog value = " << _real_dialog_->doubleValue() << " [" << _unit_symbol_ << "]");
          _value_ = _real_dialog_->doubleValue();
        }
        return;
      }

      /* ------------------------------------*/

      FileDialogLauncherButton::FileDialogLauncherButton(const QModelIndex & index_,
                                                         const QString & button_text_,
                                                         QWidget * parent_)
        : QPushButton(button_text_, parent_)
      {
        _index_ = nullptr;
        _path_dialog_ = nullptr;
        _path_record_ = nullptr;
        _param_model_ = nullptr;

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
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR,"NOT a parameter variant record!");
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
        connect(this, SIGNAL(clicked()),
                this, SLOT(slot_launch_file_dialog()));
        return;
      }

      FileDialogLauncherButton::~FileDialogLauncherButton()
      {
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
        DT_LOG_TRACE_ENTERING(logging);
        if (_path_dialog_) {
          QStringList paths = _path_dialog_->selectedFiles();
          if (paths.size() > 0) {
            _path_ = paths.at(0);
            DT_LOG_DEBUG(logging, "Path is '" << _path_.toStdString() << "'");
          }
        }
        DT_LOG_TRACE_EXITING(logging);
        return;
      }

      void FileDialogLauncherButton::slot_launch_file_dialog()
      {
        DT_LOG_TRACE_ENTERING(logging);
        if (_path_dialog_) {
          DT_LOG_DEBUG(logging, "Show QFileDialog...");
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
        DT_LOG_TRACE_EXITING(logging);
        return;
      }

      /* -------------------------------- */

      bool parameter_item_delegate::has_parent_repository() const
      {
        return _parent_repository_ != nullptr;
      }

      void parameter_item_delegate::set_parent_repository(const datatools::configuration::variant_repository & parent_repository_)
      {
        _parent_repository_ = &parent_repository_;
        return;
      }

      const datatools::configuration::variant_repository &
      parameter_item_delegate::get_parent_repository() const
      {
        DT_THROW_IF(!has_parent_repository(), std::logic_error,
                    "No parent repository handle is set !");
        return *_parent_repository_;
      }

      bool parameter_item_delegate::allowed_dynamically_enabled_values() const
      {
        return has_parent_repository();
      }

      bool parameter_item_delegate::has_max_combo_rank() const
      {
        return _max_combo_rank_ >= 0;
      }

      void parameter_item_delegate::set_max_combo_rank(int max_rank_)
      {
        if (max_rank_ > 1000 or max_rank_ < 0) {
          _max_combo_rank_ = -1;
        } else {
          _max_combo_rank_ = max_rank_;
        }
        return;
      }

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
        : QStyledItemDelegate(parent_)
      {
        _logging_ = datatools::logger::PRIO_FATAL;
        _hide_disabled_groups_ = false;
        _hide_disabled_values_ = false;
        _hide_disabled_groups_ = true;
        _hide_disabled_values_ = true;
        return;
      }

      parameter_item_delegate::~parameter_item_delegate()
      {
        return;
      }

      bool parameter_item_delegate::are_disabled_groups_hidden() const
      {
        return _hide_disabled_groups_;
      }
      
      void parameter_item_delegate::set_hide_disabled_groups(bool hdg_)
      {
        _hide_disabled_groups_ = hdg_;
        return ;
      }

      bool parameter_item_delegate::are_disabled_values_hidden() const
      {
        return _hide_disabled_values_;
      }
      
      void parameter_item_delegate::set_hide_disabled_values(bool hdv_)
      {
        _hide_disabled_values_ = hdv_;
        return ;
      }
 
      QWidget * parameter_item_delegate::_create_boolean_editor(QWidget * parent_,
                                                                const QStyleOptionViewItem & /* option_ */,
                                                                const QModelIndex & /* index_ */,
                                                                const variant_record & var_rec_) const
      {
        datatools::logger::priority logging = _logging_;
        DT_LOG_DEBUG(logging, "Entering...");
        QWidget * editor = nullptr;
        QComboBox * boolean_combo = new QComboBox(parent_);
        boolean_combo->setAutoFillBackground(true);
        const parameter_model & param_model = var_rec_.get_parameter_model();
        std::set<std::string> groups;
        param_model.build_list_of_enumerated_boolean_groups(groups);
        if (datatools::logger::is_debug(logging)) {
          DT_LOG_DEBUG(logging, "Groups: [" << groups.size() << "]");
          for (const auto & group : groups) {
            DT_LOG_DEBUG_SHORT(logging, " --> Group = '" << group << "'");
          }
        }
        typedef std::map<std::string, std::set<bool>> group_values_dict_type;
        group_values_dict_type group_values_dict;
        static const std::string NOGROUP_NAME("__nogroup__");
        std::set<bool> nogroup_values;
        param_model.build_list_of_enumerated_boolean_values_in_group("", nogroup_values);
        if (datatools::logger::is_debug(logging)) {
          DT_LOG_DEBUG(logging,
                       "List of enumerated boolean values without group: [" << nogroup_values.size() << "]");
          for (const auto & nogroup_value : nogroup_values) {
            DT_LOG_DEBUG_SHORT(logging, " --> value = '" << nogroup_value << "'");
          }
        }
        std::size_t nb_value_total = 0;
        if (nogroup_values.size()) {
          nb_value_total += nogroup_values.size();
          group_values_dict[NOGROUP_NAME] = nogroup_values;
        }
        DT_LOG_DEBUG(logging, "Done.");
        for (const auto & group : groups) {
          DT_LOG_DEBUG(logging, "Processing group '" << group << "'...");
          std::set<bool> group_values;
          param_model.build_list_of_enumerated_boolean_values_in_group(group, group_values);
          if (group_values.size()) {
            nb_value_total += group_values.size();
            group_values_dict[group] = group_values;
          }
        }

        if (datatools::logger::is_debug(logging)) {
          for (const auto & grpval : group_values_dict) {
            const std::string & group_name = grpval.first;
            DT_LOG_DEBUG(logging, "Enabled values for the enumerated boolean group '" << group_name << "' : ");
            for (const auto & value : grpval.second) {
              DT_LOG_DEBUG_SHORT(logging, " ==> Value = '" << value << "'");
            }
          }
        }

        // Build the combo list:
        std::size_t group_counter = 0;
        int index = 0;
        int set_index = -1;
        for (const auto & grpval : group_values_dict) {
          const std::string & group_name = grpval.first;
          bool enabled_group = true;
          if (group_name != NOGROUP_NAME) {
            enabled_group = var_rec_.check_enabled_group(group_name);
            QStandardItem * item = new QStandardItem(QString::fromStdString(group_name));
            item->setFlags(item->flags() & ~(/* Qt::ItemIsEnabled |*/ Qt::ItemIsSelectable));
            item->setData( "parent", Qt::AccessibleDescriptionRole );
            QFont font = item->font();
            font.setBold(true);
            item->setFont(font);
            QStandardItemModel * itemModel = (QStandardItemModel*) boolean_combo->model();
            itemModel->appendRow(item);
            ++index;
          }
          for (const auto & value : grpval.second) {
            bool enabled_value = true;
            if (!enabled_group) {
              enabled_value = false;
            } else {
              parameter_value_type param_value = value;
              enabled_value = var_rec_.check_enabled_value(param_value);
            }
            if (var_rec_.value_is_set()) {
              bool val = false;
              command::returned_info cri = var_rec_.get_boolean_value(val);
              if (val == value) {
                set_index = index;
              }
            }
            std::string bool_repr = "true";
            if (!value) bool_repr = "false";
            boolean_combo->insertItem(index, QString::fromStdString(bool_repr));
            const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(boolean_combo->model());
            QStandardItem* item = model->item(index);
            if (!enabled_value) {
              item->setFlags(item->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsSelectable));
            } else {
              item->setFlags(item->flags() & (Qt::ItemIsEnabled | Qt::ItemIsSelectable));
            }
            index++;
          }
          ++group_counter;
          if (group_counter != group_values_dict.size()) {
            boolean_combo->insertSeparator(++index);
          }
        }
        if (set_index >= 0) {
          boolean_combo->setCurrentIndex(set_index);
        }
        editor = boolean_combo;

        std::string editor_repr = "<null>";
        if ( editor != nullptr ) editor_repr = editor->QWidget::objectName().toStdString();
        DT_LOG_DEBUG(logging, "Create boolean editor = '" << editor_repr << "' [@" << editor << "]");
        return editor;
      }

      QWidget * parameter_item_delegate::_create_integer_editor(QWidget * parent_,
                                                                const QStyleOptionViewItem & /* option_ */,
                                                                const QModelIndex & /* index_ */,
                                                                const variant_record & var_rec_) const
      {
        DT_LOG_DEBUG(_logging_, "Entering...");
        QWidget * editor = nullptr;
        const parameter_model & param_model = var_rec_.get_parameter_model();

        if (param_model.is_free()) {
          // Free:
          QSpinBox * integer_sb = new QSpinBox(parent_);
          integer_sb->setAutoFillBackground(true);
          integer_sb->setSingleStep(1);
          integer_sb->setMinimum(std::numeric_limits<int>::min());
          integer_sb->setMaximum(std::numeric_limits<int>::max());
          editor = integer_sb;
        } // is_free

        if (param_model.is_interval()) {
          // Interval:
          QSpinBox * integer_sb = new QSpinBox(parent_);
          integer_sb->setAutoFillBackground(true);
          integer_sb->setMinimum(std::numeric_limits<int>::min());
          integer_sb->setMaximum(std::numeric_limits<int>::max());
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

        if (param_model.is_enumeration()) {
          // Enumeration:
          QComboBox * integer_combo = new QComboBox(parent_);
          integer_combo->setAutoFillBackground(true);
          std::set<std::string> groups;
          param_model.build_list_of_enumerated_integer_groups(groups);
          if (datatools::logger::is_debug(_logging_)) {
            DT_LOG_DEBUG(_logging_, "Groups: [" << groups.size() << "]");
            for (const auto & group : groups) {
              DT_LOG_DEBUG_SHORT(_logging_, " --> Group = '" << group << "'");
            }
          }
          typedef std::map<std::string, std::set<int>> group_values_dict_type;
          group_values_dict_type group_values_dict;
          static const std::string NOGROUP_NAME("__nogroup__");
          std::set<int> nogroup_values;
          param_model.build_list_of_enumerated_integer_values_in_group("", nogroup_values);
          if (datatools::logger::is_debug(_logging_)) {
            DT_LOG_DEBUG(_logging_,
                         "List of enumerated integer values without group: [" << nogroup_values.size() << "]");
            for (const auto & nogroup_value : nogroup_values) {
              DT_LOG_DEBUG_SHORT(_logging_, " --> value = '" << nogroup_value << "'");
            }
          }
          std::size_t nb_value_total = 0;
          if (nogroup_values.size()) {
            nb_value_total += nogroup_values.size();
            group_values_dict[NOGROUP_NAME] = nogroup_values;
          }
          DT_LOG_DEBUG(_logging_, "Done.");
          for (const auto & group : groups) {
            DT_LOG_DEBUG(_logging_, "Processing group '" << group << "'...");
            std::set<int> group_values;
            param_model.build_list_of_enumerated_integer_values_in_group(group, group_values);
            if (group_values.size()) {
              nb_value_total += group_values.size();
              group_values_dict[group] = group_values;
            }
          }

          if (datatools::logger::is_debug(_logging_)) {
            for (const auto & grpval : group_values_dict) {
              const std::string & group_name = grpval.first;
              DT_LOG_DEBUG(_logging_, "Enabled values for the enumerated integer group '" << group_name << "' : ");
              for (const auto & value : grpval.second) {
                DT_LOG_DEBUG_SHORT(_logging_, " ==> Value = '" << value << "'");
              }
            }
          }

          // Build the combo list:
          int current_value;
          if (var_rec_.value_is_set()) {
            command::returned_info cri = var_rec_.get_integer_value(current_value);
          }
          std::size_t group_counter = 0;
          int index = 0;
          for (const auto & grpval : group_values_dict) {
            const std::string & group_name = grpval.first;
            bool enabled_group = true;
            if (group_name != NOGROUP_NAME) {
              enabled_group = var_rec_.check_enabled_group(group_name);
              QStandardItem * item = new QStandardItem(QString::fromStdString(group_name));
              item->setFlags(item->flags() & ~(/* Qt::ItemIsEnabled | */ Qt::ItemIsSelectable));
              item->setData( "parent", Qt::AccessibleDescriptionRole );
              QFont font = item->font();
              font.setBold(true);
              item->setFont(font);
              QStandardItemModel * itemModel = (QStandardItemModel*) integer_combo->model();
              itemModel->appendRow(item);
              ++index;
            }
            for (const auto & value : grpval.second) {
              bool enabled_value = true;
              if (!enabled_group) {
                enabled_value = false;
              } else {
                parameter_value_type param_value = value;
                enabled_value = var_rec_.check_enabled_value(param_value);
              }
              std::ostringstream valint_oss;
              valint_oss << value;
              integer_combo->insertItem(index, QString::fromStdString(valint_oss.str()));
              const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(integer_combo->model());
              QStandardItem* item = model->item(index);
              if (!enabled_value) {
                item->setFlags(item->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsSelectable));
              } else {
                item->setFlags(item->flags() & (Qt::ItemIsEnabled | Qt::ItemIsSelectable));
              }
              if (var_rec_.value_is_set() && current_value == value) {
                integer_combo->setCurrentIndex(index);
              }
              index++;
            }
            ++group_counter;
            if (group_counter != group_values_dict.size()) {
              integer_combo->insertSeparator(++index);
            }
          }
          editor = integer_combo;

        } // is_enumeration

        std::string editor_repr = "<null>";
        if ( editor != nullptr ) editor_repr = editor->QWidget::objectName().toStdString();
        DT_LOG_DEBUG(_logging_, "Create integer editor = '" << editor_repr << "' [@" << editor << "]");
        return editor;
      }

      QWidget * parameter_item_delegate::_create_real_editor(QWidget * parent_,
                                                             const QStyleOptionViewItem & /* option_ */,
                                                             const QModelIndex & index_,
                                                             const variant_record & var_rec_) const
      {
        DT_LOG_DEBUG(_logging_, "Entering...");
        QWidget * editor = nullptr;
        const parameter_model & param_model = var_rec_.get_parameter_model();

        if (param_model.is_free()) {
          // Free:
          DT_LOG_DEBUG(_logging_, "Free real...");
          RealDialogLauncherButton * real_editor_button
            = new RealDialogLauncherButton(index_, "Choose value", parent_);
          editor = real_editor_button;
          DT_LOG_DEBUG(_logging_, "editor=[@" << editor << "]");
        } // is_free

        if (param_model.is_interval()) {
          // Interval:
          DT_LOG_DEBUG(_logging_, "Interval real...");
          RealDialogLauncherButton * real_editor_button
            = new RealDialogLauncherButton(index_, "Choose value", parent_);
          editor = real_editor_button;
          DT_LOG_DEBUG(_logging_, "editor=[@" << editor << "]");
        } // is_interval

        if (param_model.is_enumeration()) {
          // Enumeration:
          DT_LOG_DEBUG(_logging_, "Enumeration real...");
          QComboBox * real_combo = new QComboBox(parent_);
          real_combo->setAutoFillBackground(true);
          std::set<std::string> groups;
          param_model.build_list_of_enumerated_real_groups(groups);
          if (datatools::logger::is_debug(_logging_)) {
            DT_LOG_DEBUG(_logging_, "Groups: [" << groups.size() << "]");
            for (const auto & group : groups) {
              DT_LOG_DEBUG_SHORT(_logging_, " --> Group = '" << group << "'");
            }
          }
          typedef std::map<std::string, std::set<double>> group_values_dict_type;
          group_values_dict_type group_values_dict;
          static const std::string NOGROUP_NAME("__nogroup__");
          std::set<double> nogroup_values;
          param_model.build_list_of_enumerated_real_values_in_group("", nogroup_values);
          if (datatools::logger::is_debug(_logging_)) {
            DT_LOG_DEBUG(_logging_,
                         "List of enumerated real values without group: [" << nogroup_values.size() << "]");
            for (const auto & nogroup_value : nogroup_values) {
              DT_LOG_DEBUG_SHORT(_logging_, " --> value = '" << nogroup_value << "'");
            }
          }
          std::size_t nb_value_total = 0;
          if (nogroup_values.size()) {
            nb_value_total += nogroup_values.size();
            group_values_dict[NOGROUP_NAME] = nogroup_values;
          }
          DT_LOG_DEBUG(_logging_, "Done.");
          for (const auto & group : groups) {
            DT_LOG_DEBUG(_logging_, "Processing group '" << group << "'...");
            std::set<double> group_values;
            param_model.build_list_of_enumerated_real_values_in_group(group, group_values);
            if (group_values.size()) {
              nb_value_total += group_values.size();
              group_values_dict[group] = group_values;
            }
          }

          if (datatools::logger::is_debug(_logging_)) {
            for (const auto & grpval : group_values_dict) {
              const std::string & group_name = grpval.first;
              DT_LOG_DEBUG(_logging_, "Enabled values for the enumerated real group '" << group_name << "' : ");
              for (const auto & value : grpval.second) {
                DT_LOG_DEBUG_SHORT(_logging_, " ==> Value = '" << value << "'");
              }
            }
          }

          // Build the combo list:
          double current_value;
          if (var_rec_.value_is_set()) {
            command::returned_info cri = var_rec_.get_real_value(current_value);
          }
          double display_unit = 1.0;
          std::string unit_symbol;
          std::string unit_label;
          if (param_model.has_real_preferred_unit()) {
            unit_symbol = param_model.get_real_preferred_unit();
          } else if (param_model.has_real_unit_label()) {
            unit_symbol = units::get_default_unit_symbol_from_label(param_model.get_real_unit_label());
            std::string unit_label2;
            bool found_unit = units::find_unit(unit_symbol, display_unit, unit_label2);
            if (!found_unit) {
              DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS, "Cannot find unit with symbol '" << unit_symbol << "'!");
            }
            // DT_THROW_IF(!found_unit, std::logic_error, "Cannot find unit !");
          }
          std::size_t group_counter = 0;
          int index = 0;
          for (const auto & grpval : group_values_dict) {
            const std::string & group_name = grpval.first;
            bool enabled_group = true;
            if (group_name != NOGROUP_NAME) {
              enabled_group = var_rec_.check_enabled_group(group_name);
              QStandardItem * item = new QStandardItem(QString::fromStdString(group_name));
              item->setFlags(item->flags() & ~(/* Qt::ItemIsEnabled | */ Qt::ItemIsSelectable));
              item->setData( "parent", Qt::AccessibleDescriptionRole );
              QFont font = item->font();
              font.setBold(true);
              item->setFont(font);
              QStandardItemModel * itemModel = (QStandardItemModel*) real_combo->model();
              itemModel->appendRow(item);
              ++index;
            }
            for (const auto & value : grpval.second) {
              bool enabled_value = true;
              if (!enabled_group) {
                enabled_value = false;
              } else {
                parameter_value_type param_value = value;
                enabled_value = var_rec_.check_enabled_value(param_value);
              }
              std::ostringstream valreal_oss;
              io::write_real_number(valreal_oss, value, 15, unit_symbol, unit_label);
              real_combo->insertItem(index, QString::fromStdString(valreal_oss.str()));
              const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(real_combo->model());
              QStandardItem* item = model->item(index);
              if (!enabled_value) {
                item->setFlags(item->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsSelectable));
              } else {
                item->setFlags(item->flags() & (Qt::ItemIsEnabled | Qt::ItemIsSelectable));
              }
              if (var_rec_.value_is_set() && current_value == value) {
                real_combo->setCurrentIndex(index);
              }
              index++;
            }
            ++group_counter;
            if (group_counter != group_values_dict.size()) {
              real_combo->insertSeparator(++index);
            }
          }
          editor = real_combo;
          DT_LOG_DEBUG(_logging_, "editor=[@" << editor << "]");
        } // is_enumeration
        DT_LOG_DEBUG(_logging_, "editor=[@" << editor << "]");

        std::string editor_repr = "<null>";
        if ( editor != nullptr ) editor_repr = editor->QWidget::objectName().toStdString();
        DT_LOG_DEBUG(_logging_, "Create real editor = '" << editor_repr << "' [@" << editor << "]");
        return editor;
      }

      QWidget * parameter_item_delegate::_create_string_editor(QWidget * parent_,
                                                               const QStyleOptionViewItem & /* option_ */,
                                                               const QModelIndex & index_,
                                                               const variant_record & var_rec_) const
      {
        DT_LOG_DEBUG(_logging_, "Entering...");
        QWidget * editor = nullptr;
        const parameter_model & param_model = var_rec_.get_parameter_model();

        if (param_model.is_free()) {
          // Free:
          if (param_model.is_path()) {
            FileDialogLauncherButton * path_editor_button
              = new FileDialogLauncherButton(index_, "Choose file path", parent_);
            editor = path_editor_button;
          }
        } // is_free

        int starting_index = 0;
        int best_rank = -1; 
        int best_rank_index = -1; 
        if (param_model.is_enumeration()) {
          // Enumeration:
          QComboBox * string_combo = new QComboBox(parent_);
          string_combo->setAutoFillBackground(true);
          string_combo->setCurrentIndex(starting_index);
          // string_combo->setMaxVisibleItems(15);
          std::set<std::string> groups;
          param_model.build_list_of_enumerated_string_groups(groups);
          if (datatools::logger::is_debug(_logging_)) {
            DT_LOG_DEBUG(_logging_, "Groups: [" << groups.size() << "]");
            for (const auto & group : groups) {
              DT_LOG_DEBUG_SHORT(_logging_, " --> Group = '" << group << "'");
            }
          }
          typedef std::map<std::string, std::list<std::string>> group_values_dict_type;
          group_values_dict_type group_values_dict;
          static const std::string NOGROUP_NAME("__nogroup__");
          std::set<std::string> nogroup_values;
          param_model.build_list_of_enumerated_string_values_in_group("", nogroup_values);
          if (datatools::logger::is_debug(_logging_)) {
            DT_LOG_DEBUG(_logging_,
                         "List of enumerated string values without group: [" << nogroup_values.size() << "]");
            for (const auto & nogroup_value : nogroup_values) {
              DT_LOG_DEBUG_SHORT(_logging_, " --> value = '" << nogroup_value << "'");
            }
          }
          std::list<std::string> ranked_nogroup_values;
          param_model.rank_list_of_enumerated_string_values(nogroup_values, ranked_nogroup_values);
          std::size_t nb_value_total = 0;
          if (ranked_nogroup_values.size()) {
            nb_value_total += ranked_nogroup_values.size();
            group_values_dict[NOGROUP_NAME] = ranked_nogroup_values;
          }
          DT_LOG_DEBUG(_logging_, "Done.");
          for (const auto & group : groups) {
            DT_LOG_DEBUG(_logging_, "Processing group '" << group << "'...");
            std::set<std::string> group_values;
            param_model.build_list_of_enumerated_string_values_in_group(group, group_values);
            std::list<std::string> ranked_group_values;
            param_model.rank_list_of_enumerated_string_values(group_values, ranked_group_values);
            if (ranked_group_values.size()) {
              nb_value_total += ranked_group_values.size();
              group_values_dict[group] = ranked_group_values;
            }
          }

          if (datatools::logger::is_debug(_logging_)) {
            for (const auto & grpval : group_values_dict) {
              const std::string & group_name = grpval.first;
              DT_LOG_DEBUG(_logging_, "Enabled values for the enumerated string group '" << group_name << "' : ");
              for (const auto & value : grpval.second) {
                DT_LOG_DEBUG_SHORT(_logging_, " ==> Value = '" << value << "'");
              }
            }
          }

          // Build the combo list:
          std::string current_value;
          if (var_rec_.value_is_set()) {
            command::returned_info cri = var_rec_.get_string_value(current_value);
          }
          const parameter_model & paramModel = var_rec_.get_parameter_model();
          std::size_t group_counter = 0;
          int index = 0;
          for (const auto & grpval : group_values_dict) {
            const std::string & group_name = grpval.first;
            // std::cerr << "*** DEVEL *** Processing group=" << group_name << "...\n";

            // Count ranks associated to values:
            std::set<int> set_ranks;
            int max_rank = (_max_combo_rank_ == -1 ? 100000 : _max_combo_rank_);
            // Loop on the values:
            for (const auto & value : grpval.second) {
              const parameter_model::string_enum_value_metadata & strEnumMeta
                = paramModel.get_enumerated_string_value_metadata(value);
              int current_rank = strEnumMeta.get_rank();
              if (current_rank <= max_rank) {
                set_ranks.insert(current_rank);
              }
            }
            
            bool enabled_group = true;
            if (group_name != NOGROUP_NAME) {
              enabled_group = var_rec_.check_enabled_group(group_name);
            }
            if (set_ranks.size() == 0) {
              enabled_group = false;
            }

            if (group_name != NOGROUP_NAME) {
              if (are_disabled_groups_hidden() and not enabled_group) {
                // Skip this group:
                // std::cerr << "*** DEVEL *** Group=" << group_name << " is disabled\n";
                continue;
              }
              QStandardItem * item = new QStandardItem(QString::fromStdString(group_name));
              item->setFlags(item->flags() & ~(/* Qt::ItemIsEnabled | */ Qt::ItemIsSelectable));
              item->setData( "parent", Qt::AccessibleDescriptionRole );
              QFont font = item->font();
              font.setBold(true);
              item->setFont(font);
              QStandardItemModel * itemModel = (QStandardItemModel*) string_combo->model();
              itemModel->appendRow(item);
              ++index;
            }
            
            // Loop on the values:
            int last_rank = -1;
            for (const auto & value : grpval.second) {
              const parameter_model::string_enum_value_metadata & strEnumMeta
                = paramModel.get_enumerated_string_value_metadata(value);
              bool enabled_value = true;
              if (!enabled_group) {
                enabled_value = false;
              } else {
                parameter_value_type param_value = value;
                enabled_value = var_rec_.check_enabled_value(param_value);
              }
              if (are_disabled_values_hidden() and not enabled_value) {
                // Skip this value:
                // std::cerr << "*** DEVEL *** Value=" << value << " is disabled\n";
                continue;
              }
              int current_rank = strEnumMeta.get_rank();
              if (current_rank > max_rank) {
                continue;
              }
              // Process the value:
              bool start_rank_block = false;
              if (best_rank_index == -1 or current_rank < best_rank) {
                best_rank = current_rank;
                best_rank_index = index;
              }
              if (last_rank >= 0) {
                if (current_rank > last_rank) {
                  // separate groups of items with same rank
                  string_combo->insertSeparator(++index);
                  start_rank_block = true;
                }
              } else {
                start_rank_block = true;
              }
              if (set_ranks.size() > 1 and start_rank_block) {
                std::string rank_label;
                if (current_rank == parameter_model::base_enum_metadata::RANK_HIGHLIGHTED) {
                  rank_label = "Top choices";
                } else if (current_rank == parameter_model::base_enum_metadata::RANK_PRIMARY) {
                  rank_label = "Primary choices";
                } else if (current_rank == parameter_model::base_enum_metadata::RANK_SECONDARY) {
                  rank_label = "Secondary choices";
                } else if (current_rank == parameter_model::base_enum_metadata::RANK_TERNARY) {
                  rank_label = "Unlikely choices";
                }              
                if (! rank_label.empty()) {
                  QStandardItem * item = new QStandardItem(QString::fromStdString(rank_label));
                  item->setFlags(item->flags() & ~(/* Qt::ItemIsEnabled | */ Qt::ItemIsSelectable));
                  item->setData( "parent", Qt::AccessibleDescriptionRole );
                  // item->setData( Qt::AlignRight, Qt::TextAlignmentRole );
                  QFont font = item->font();
                  font.setItalic(true);
                  int pointSize = font.pointSize();
                  font.setPointSize(pointSize > 6 ? pointSize - 2 : pointSize);
                  item->setFont(font);
                  QStandardItemModel * itemModel = (QStandardItemModel*) string_combo->model();
                  itemModel->appendRow(item);
                  ++index;
                }
              } 
              string_combo->insertItem(index, QString::fromStdString(value));
              const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(string_combo->model());
              QStandardItem* item = model->item(index);
              if (!enabled_value) {
                item->setFlags(item->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsSelectable));
              } else {
                item->setFlags(item->flags() & (Qt::ItemIsEnabled | Qt::ItemIsSelectable));
              }
              if (! current_value.empty() && current_value == value) {
                string_combo->setCurrentIndex(index);
              }
              index++;
              last_rank = current_rank;
            } // Value loop
            ++group_counter;
            if (group_counter != group_values_dict.size()) {
              string_combo->insertSeparator(++index);
            }
          } // Group loop
          editor = string_combo;
          if (best_rank_index >= 0)  {
            starting_index = best_rank_index;
          }
          string_combo->setCurrentIndex(starting_index);
        } // is_enumeration

        std::string editor_repr = "<null>";
        if ( editor != nullptr ) editor_repr = editor->QWidget::objectName().toStdString();
        DT_LOG_DEBUG(_logging_, "Create string editor = '" << editor_repr << "' [@" << editor << "]");
        return editor;
      }

      QWidget * parameter_item_delegate::createEditor(QWidget * parent_,
                                                      const QStyleOptionViewItem & option_,
                                                      const QModelIndex & index_) const
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        QWidget * editor = nullptr;
        try {

          // Only for value column:
          if (index_.column() == tree_item::CI_VALUE) {
            DT_LOG_TRACE(_logging_, "Processing value column...");
            const variant_registry_tree_model * model
              = dynamic_cast<const variant_registry_tree_model *>(index_.model());
            tree_item * node = model->node_from_index(index_);
            const variant_record & rec = node->get_record();

            // Only for parameters :
            if (rec.is_parameter()) {

              const parameter_model & param_model = rec.get_parameter_model();

              // Only for variable parameters :
              if (param_model.is_variable()) {

                // Boolean:
                if (param_model.is_boolean()) {
                  DT_LOG_TRACE(_logging_, "Boolean...");
                  editor = _create_boolean_editor(parent_, option_, index_, rec);
                } // is_boolean

                // Integer:
                if (param_model.is_integer()) {
                  DT_LOG_TRACE(_logging_, "Integer...");
                  editor = _create_integer_editor(parent_, option_, index_, rec);
                } // is_integer

                // Real:
                if (param_model.is_real()) {
                  DT_LOG_TRACE(_logging_, "Real...");
                  editor = _create_real_editor(parent_, option_, index_, rec);
                } // is_real

                // String:
                if (param_model.is_string()) {
                  DT_LOG_TRACE(_logging_, "Parameter model '" << param_model.get_name() << "' with string type...");
                  editor = _create_string_editor(parent_, option_, index_, rec);
                } // is_string

                if (editor == nullptr) {
                  editor = QStyledItemDelegate::createEditor(parent_, option_, index_);
                }

              } // variable

            } // parameter

          } // value column
        } catch (std::exception & error) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
        }

        std::string editor_repr = "<null>";
        if ( editor != nullptr ) editor_repr = editor->QWidget::objectName().toStdString();
        DT_LOG_DEBUG(_logging_, "Create parameter value editor = '" << editor_repr << "' [@" << editor << "]");
        DT_LOG_TRACE_EXITING(_logging_);
        return editor;
      }

      void parameter_item_delegate::setEditorData(QWidget * editor_,
                                                  const QModelIndex & index_) const
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        std::string editor_repr = "<null>";
        if ( editor_ != nullptr ) editor_repr = editor_->QWidget::objectName().toStdString();
        DT_LOG_TRACE(_logging_, "Use parameter value editor = '" << editor_repr << "' [@" << editor_ << "]");
        bool sed = false;
        try {

          if (index_.column() == tree_item::CI_VALUE) {
            DT_LOG_TRACE(_logging_, "Processing value column...");

            // Fetch the current displayed value:
            QVariant value = index_.model()->data(index_, Qt::DisplayRole);

            const variant_registry_tree_model * model
              = dynamic_cast<const variant_registry_tree_model *>(index_.model());
            const tree_item * node = model->node_from_index(index_);
            const variant_record & var_rec = node->get_record();

            if (var_rec.is_parameter()) {

              const parameter_model & param_model = var_rec.get_parameter_model();

              // Boolean:
              if (param_model.is_boolean()) {
                DT_LOG_DEBUG(_logging_, "Boolean...");
                sed = _set_boolean_editor_data(editor_, index_, var_rec);
              } // Boolean

              // Integer:
              if (param_model.is_integer()) {
                DT_LOG_DEBUG(_logging_, "Integer...");
                sed = _set_integer_editor_data(editor_, index_, var_rec);
              } // Integer

              // Real:
              if (param_model.is_real()) {
                DT_LOG_DEBUG(_logging_, "Real...");
                sed = _set_real_editor_data(editor_, index_, var_rec);
              } // Real

              // String:
              if (param_model.is_string()) {
                DT_LOG_DEBUG(_logging_, "String...");
                sed = _set_string_editor_data(editor_, index_, var_rec);
              } // String

            } // is_parameter

          }
        } catch (std::exception & error) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
          sed = false;
        }

        if (!sed) {
          // http://cep.xray.aps.anl.gov/software/qt4-x11-4.2.2-browser/d9/da2/class_q_default_item_editor_factory.html
          DT_LOG_DEBUG(_logging_,
                       "editor = '" << editor_->QWidget::objectName().toStdString() << "'");
          DT_LOG_DEBUG(_logging_,
                       "editor = '" << typeid(*editor_).name() << "'");
          QStyledItemDelegate::setEditorData(editor_, index_);
        }

        DT_LOG_TRACE_EXITING(_logging_);
        return;
      }

      void parameter_item_delegate::setEditorData2(QWidget * editor_,
                                                   const QModelIndex & index_) const
      {
        // bool sed = false;
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
              //sed = _set_boolean_editor_data(editor_, index_);
            } // Boolean

            // Integer:
            if (param_model.is_integer()) {
              // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: Integer" << std::endl;

              // Free:
              if (param_model.is_free()) {
                QSpinBox * integer_sb = static_cast<QSpinBox*>(editor_);
                QString current_qstring_value = value.toString();
                integer_sb->setValue(value.toInt());
                //sed = true;
              }

              // Enumeration:
              if (param_model.is_enumeration()) {
                QComboBox * integer_combo = static_cast<QComboBox*>(editor_);
                integer_combo->setCurrentIndex(integer_combo->findText(value.toString()));
                //sed = true;
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
                // sed = true;
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
                //sed = true;
              } // Free | interval

              // Enumeration:
              if (param_model.is_enumeration()) {
                QComboBox * real_combo = static_cast<QComboBox*>(editor_);
                real_combo->setCurrentIndex(real_combo->findText(value.toString()));
                //sed = true;
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
                  //sed = true;

                } else {

                  QLineEdit * string_line_edit = static_cast<QLineEdit*>(editor_);
                  QString current_qstring_value = value.toString();
                  string_line_edit->setText(current_qstring_value);
                  //sed = true;

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
                // sed = true;
              }

            } // String
            // std::cerr << "DEVEL: parameter_item_delegate::setEditorData: CI_VALUE/is_parameter <<<" << std::endl;

          } // is_parameter
        } // CI_VALUE

        return;
      }

      bool parameter_item_delegate::_set_boolean_editor_data(QWidget * editor_,
                                                             const QModelIndex & index_,
                                                             const variant_record & /*var_rec_*/) const
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        DT_LOG_TRACE(_logging_, "Editor = '" << (editor_ == nullptr ? "<null>" : editor_->QWidget::objectName().toStdString()));
        bool sed = false;

        try {
          // const parameter_model & param_model = var_rec_.get_parameter_model();
          // Enumeration:
          // Fetch the current displayed value:
          QVariant value = index_.model()->data(index_, Qt::DisplayRole);
          DT_LOG_TRACE(_logging_, "Boolean value = " << value.toString().toStdString());
          QComboBox * boolean_combo = static_cast<QComboBox*>(editor_);
          DT_LOG_TRACE(_logging_, "Boolean combo # of values = " << boolean_combo->count());
          int pos = boolean_combo->findText(value.toString());
          DT_LOG_TRACE(_logging_, "pos = " << pos);
          boolean_combo->setCurrentIndex(boolean_combo->findText(value.toString()));
          sed = true;

        } catch (std::exception & error) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
        }

        DT_LOG_TRACE_EXITING(_logging_);
        return sed;
      }

      bool parameter_item_delegate::_set_integer_editor_data(QWidget * editor_,
                                                             const QModelIndex & index_,
                                                             const variant_record & var_rec_) const
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        bool sed = false;
        try {
          // Fetch the current displayed value:
          QVariant value = index_.model()->data(index_, Qt::DisplayRole);
          DT_LOG_TRACE(_logging_, "Integer value = " << value.toString().toStdString());
          const parameter_model & param_model = var_rec_.get_parameter_model();

          // Free:
          if (param_model.is_free()) {
            QSpinBox * integer_sb = static_cast<QSpinBox*>(editor_);
            integer_sb->setValue(value.toInt());
            sed = true;
          }

          // Interval:
          if (param_model.is_interval()) {
            QSpinBox * integer_sb = static_cast<QSpinBox*>(editor_);
            integer_sb->setValue(value.toInt());
            sed = true;
          }

          // Enumeration:
          if (param_model.is_enumeration()) {
            DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "Combo box for enumerated integer values is not implemented yet!");
            // QComboBox * integer_combo = static_cast<QComboBox*>(editor_);
            // XXX
            // integer_combo->setCurrentIndex(integer_combo->findText(value.toString()));
            // sed = true;
          }

        } catch (std::exception & error) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
        }
        DT_LOG_TRACE_EXITING(_logging_);
        return sed;
      }

      bool parameter_item_delegate::_set_real_editor_data(QWidget * editor_,
                                                          const QModelIndex & index_,
                                                          const variant_record & var_rec_) const
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        bool sed = false;
        try {
          // Fetch the current displayed value:
          QVariant value = index_.model()->data(index_, Qt::DisplayRole);
          DT_LOG_TRACE(_logging_, "Real value = " << value.toString().toStdString());
          const parameter_model & param_model = var_rec_.get_parameter_model();

          // Free/interval:
          if (param_model.is_free() || param_model.is_interval()) {
            DT_LOG_TRACE(_logging_, "Free = " << param_model.is_free());
            DT_LOG_TRACE(_logging_, "Interval = " << param_model.is_interval());
            RealDialogLauncherButton * real_editor_button
              = static_cast<RealDialogLauncherButton*>(editor_);
            real_editor_button->setText("Choose value");
            sed = true;
          } // Free | interval

          // Enumeration:
          if (param_model.is_enumeration()) {
            DT_LOG_TRACE(_logging_, "Enumeration");
            QComboBox * real_combo = static_cast<QComboBox*>(editor_);
            real_combo->setCurrentIndex(real_combo->findText(value.toString()));
            sed = true;
          } // Enumeration

        } catch (std::exception & error) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
        }
        DT_LOG_TRACE_EXITING(_logging_);
        return sed;
      }

      bool parameter_item_delegate::_set_string_editor_data(QWidget * editor_,
                                                            const QModelIndex & index_,
                                                            const variant_record & var_rec_) const
      {
        datatools::logger::priority logging = _logging_;
        // logging = datatools::logger::PRIO_TRACE;
        DT_LOG_TRACE_ENTERING(logging);
        bool sed = false;
        try {
          // Fetch the current displayed value:
          QVariant value = index_.model()->data(index_, Qt::DisplayRole);
          DT_LOG_TRACE(logging,
                       "Fetch the current string displayed value = '" << value.toString().toStdString() << "'");
          const parameter_model & param_model = var_rec_.get_parameter_model();

          // Free:
          if (param_model.is_free()) {
            DT_LOG_TRACE(logging, "Free string...");

            // Path:
            if (param_model.is_path()) {
              DT_LOG_TRACE(logging, "Path...");

              FileDialogLauncherButton * path_editor_button
                = static_cast<FileDialogLauncherButton*>(editor_);
              if (path_editor_button->is_open()) {
                path_editor_button->setText("Choose input file");
              } else if (path_editor_button->is_save()) {
                path_editor_button->setText("Choose output file");
              } else {
                path_editor_button->setText("Choose file");
              }
              DT_LOG_TRACE(logging, "Path value = '" << path_editor_button->text().toStdString() << "'");
              sed = true;

            } else {

              QLineEdit * string_line_edit = static_cast<QLineEdit*>(editor_);
              QString current_qstring_value = value.toString();
              DT_LOG_TRACE(logging, "String value = '" << current_qstring_value.toStdString() << "'");
              string_line_edit->setText(current_qstring_value);
              sed = true;

            }
          }

          // Enumeration:
          if (param_model.is_enumeration()) {
            DT_LOG_TRACE(logging, "Enumeration...");
            QComboBox * string_combo = static_cast<QComboBox*>(editor_);
            QString current_qstring_value = value.toString();
            DT_LOG_TRACE(logging, "Select string value in combo = '" << current_qstring_value.toStdString() << "'");
            string_combo->setCurrentIndex(string_combo->findText(current_qstring_value));
            sed = true;
          }

        } catch (std::exception & error) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR, error.what());
        }
        DT_LOG_TRACE_EXITING(logging);
        return sed;
      }

      void parameter_item_delegate::setModelData(QWidget * editor_,
                                                 QAbstractItemModel * model_,
                                                 const QModelIndex & index_) const
      {
        datatools::logger::priority logging = _logging_;
        // logging = datatools::logger::PRIO_TRACE;
        DT_LOG_TRACE_ENTERING(logging);
        bool sdm = false;

        // Only for "Value" column:
        if (index_.column() == tree_item::CI_VALUE) {
          variant_registry_tree_model * model = dynamic_cast<variant_registry_tree_model *>(model_);
          tree_item * node = model->node_from_index(index_);
          variant_record & rec = node->grab_record();

          // Only for parameters:
          if (rec.is_parameter()) {
            const parameter_model & param_model = rec.get_parameter_model();
            // if (param_model.get_name() == "source_calibration.off_on.PM") logging = datatools::logger::PRIO_DEBUG;

            // Boolean:
            if (param_model.is_boolean()) {
              DT_LOG_DEBUG(logging, "Boolean...");
            } // boolean

            // Integer:
            if (param_model.is_integer()) {
              DT_LOG_DEBUG(logging, "Integer...");
            } // integer

            // Real:
            if (param_model.is_real()) {
              DT_LOG_DEBUG(logging, "Real...");
              if (param_model.is_free() || param_model.is_interval()) {
                RealDialogLauncherButton * real_editor_button
                  = static_cast<RealDialogLauncherButton*>(editor_);
                std::ostringstream real_value_oss;
                real_value_oss.precision(15);
                real_value_oss << real_editor_button->get_value();
                if (! real_editor_button->get_unit_symbol().empty()) {
                  real_value_oss << ' ' << real_editor_button->get_unit_symbol();
                }
                DT_LOG_DEBUG(logging, "Set real value '" << real_value_oss.str() << "'");
                QString real_qstring = QString::fromStdString(real_value_oss.str());
                DT_LOG_DEBUG(logging, "real_qstring = '" << real_qstring.toStdString() << "'");
                bool ok = model->setData(index_, real_qstring, Qt::EditRole);
                if (ok) {
                  sdm = true;
                } else {
                  // Error
                  model->unsetData(index_, Qt::EditRole);
                  sdm = true;
                }
              } // is_free | is_interval
            } // real

            // String:
            if (param_model.is_string()) {
              DT_LOG_DEBUG(logging, "String...");
              if (param_model.is_path()) {
                DT_LOG_DEBUG(logging, "Path...");
                FileDialogLauncherButton * path_editor_button
                  = static_cast<FileDialogLauncherButton*>(editor_);
                const QString & path = path_editor_button->get_path();
                if (!path.isEmpty()) {
                  DT_LOG_DEBUG(logging, "new path = '" << path.toStdString() << "'");
                  bool ok = model->setData(index_, path, Qt::EditRole);
                  if (ok) {
                    sdm = true;
                  } else {
                    // Error
                    model->unsetData(index_, Qt::EditRole);
                    sdm = true;
                  }
                } else {
                  model->unsetData(index_, Qt::EditRole);
                  sdm = true;
                }
              } // is_path
            } // string

            if (!sdm) {
              // For boolean and integer parameter, we let the standard method do the job:
              QStyledItemDelegate::setModelData(editor_, model_, index_);
            }

          } // is_parameter

        } // CI_VALUE
        DT_LOG_TRACE_EXITING(logging);
        return;
      }

      void parameter_item_delegate::updateEditorGeometry(QWidget * editor_,
                                                         const QStyleOptionViewItem & option_,
                                                         const QModelIndex & /* index_ */) const
      {
        editor_->setGeometry(option_.rect);
        // QStyledItemDelegate::updateEditorGeometry(editor_, option_, index_);
        return;
      }

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools
