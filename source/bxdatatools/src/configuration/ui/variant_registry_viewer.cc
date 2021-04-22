// datatools/configuration/ui/variant_registry_viewer.cc
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

// Ourselves:
#include <datatools/configuration/ui/variant_registry_viewer.h>

// Standard library:
#include <stdexcept>

// Third party:
// - Qt:
#include <QTreeWidget>
#include <QString>
#include <QHeaderView>
#include <QStringList>
#include <QTreeView>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>
#include <QModelIndex>
#include <QTextEdit>

// This project:
#include <datatools/configuration/variant_record.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/parameter_model.h>
#include <datatools/configuration/variant_model.h>
#include <datatools/configuration/io.h>
#include <datatools/configuration/ui/variant_registry_tree_model.h>
#include <datatools/configuration/ui/parameter_item_delegate.h>
#include <datatools/qt/led.h>
#include <datatools/ioutils.h>
#include <datatools/configuration/ui/variant_repository_dialog.h>

namespace datatools {

  namespace configuration {

    namespace ui {

      variant_registry_viewer::variant_registry_viewer(QWidget * parent_)
        : QWidget(parent_)
      {
        _devel_mode_ = false;
        _inhibit_secondary_choices_ = false;
        _logging_ = datatools::logger::PRIO_FATAL;
        _registry_name_title_label_ = new QLabel(this);
        _registry_name_title_label_->setText("Configuration registry:");
        _registry_name_display_label_ = new QLabel(this);
        _accomplished_label_ = new QLabel(this);
        _accomplished_led_ = new datatools::qt::led(this);
        _read_only_cb_ = new QCheckBox("Read-only", this);
        
        _inhibit_secondary_choices_cb_ = new QCheckBox("Inhibit secondaries", this);
        QObject::connect(this, SIGNAL(sig_inhibit_secondary_choices_changed(bool)),
                         this, SLOT(slot_update_isc_cb(bool)));

        _tree_view_ = new QTreeView(this);
        _tree_view_->setDragEnabled(false);
        _tree_view_->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        _value_delegate_ = new parameter_item_delegate(parent_);
        auto * dialog = dynamic_cast<variant_repository_dialog*>(parent_);
        if (dialog != nullptr) {
          _value_delegate_->set_hide_disabled_groups(dialog->are_disabled_hidden());
          _value_delegate_->set_hide_disabled_values(dialog->are_disabled_hidden());
          slot_update_isc_delegate(_inhibit_secondary_choices_);
        }
        QObject::connect(this, SIGNAL(sig_inhibit_secondary_choices_changed(bool)),
                         this, SLOT(slot_update_isc_delegate(bool)));
       _tree_view_->setItemDelegateForColumn(2, _value_delegate_);
        //_tree_view_->header()->setResizeMode(1, QHeaderView::Stretch); // QHeaderView::ResizeToContents
        _expand_only_active_button_ = new QPushButton("Expand only &active", this);
        _expand_all_button_ = new QPushButton("&Expand all", this);
        _collapse_all_button_ = new QPushButton("&Collapse all", this);

        setWindowTitle(tr("Registry Viewer"));
        return;
      }

      variant_registry_viewer::~variant_registry_viewer()
      {
        if (_value_delegate_) {
          delete _value_delegate_;
        }
        return;
      }

      void variant_registry_viewer::_construct_devel()
      {
        _dump_button_ = new QPushButton("&Dump", this);

        _logging_label_ = new QLabel("Logging: ", this);
        _logging_combo_ = new QComboBox(this);
        _logging_combo_->insertItem(0,
                                    QString::fromUtf8(datatools::logger::get_priority_label(datatools::logger::PRIO_FATAL).c_str()));
        _logging_combo_->insertItem(1,
                                    QString::fromUtf8(datatools::logger::get_priority_label(datatools::logger::PRIO_CRITICAL).c_str()));
        _logging_combo_->insertItem(2,
                                    QString::fromUtf8(datatools::logger::get_priority_label(datatools::logger::PRIO_ERROR).c_str()));
        _logging_combo_->insertItem(3,
                                    QString::fromUtf8(datatools::logger::get_priority_label(datatools::logger::PRIO_WARNING).c_str()));
        _logging_combo_->insertItem(4,
                                    QString::fromUtf8(datatools::logger::get_priority_label(datatools::logger::PRIO_NOTICE).c_str()));
        _logging_combo_->insertItem(5,
                                    QString::fromUtf8(datatools::logger::get_priority_label(datatools::logger::PRIO_INFORMATION).c_str()));
        _logging_combo_->insertItem(6,
                                    QString::fromUtf8(datatools::logger::get_priority_label(datatools::logger::PRIO_DEBUG).c_str()));
        _logging_combo_->insertItem(7,
                                    QString::fromUtf8(datatools::logger::get_priority_label(datatools::logger::PRIO_TRACE).c_str()));

        QObject::connect(_dump_button_, SIGNAL(clicked()),
                         this, SLOT(slot_dump_registry()));
        QObject::connect(_logging_combo_, SIGNAL(currentIndexChanged(const QString &)),
                         this, SLOT(slot_update_logging(const QString &)));
        return;
      }

      void variant_registry_viewer::set_logging(datatools::logger::priority p_)
      {
        if (_logging_ != p_) {
          _logging_ = p_;
          emit sig_logging_changed(_logging_);
        }
        return;
      }

      void variant_registry_viewer::set_inhibit_secondary_choices(bool isc_)
      {
        _inhibit_secondary_choices_ = isc_;
        emit sig_inhibit_secondary_choices_changed(isc_);
        return;
      }

      bool variant_registry_viewer::is_inhibit_secondary_choices() const
      {
        return _inhibit_secondary_choices_;
      }

      bool variant_registry_viewer::has_registry_tree_model() const
      {
        return _registry_tree_model_ != nullptr;
      }

      void variant_registry_viewer::set_registry_tree_model(variant_registry_tree_model & rtm_,
                                                            bool devel_mode_)
      {
        _devel_mode_ = devel_mode_;
        _registry_tree_model_ = &rtm_;
         _construct();
        return;
      }

      const variant_registry_tree_model & variant_registry_viewer::get_registry_tree_model() const
      {
        DT_THROW_IF(!has_registry_tree_model(), std::logic_error,
                    "Viewer has no registry model!");
        return *_registry_tree_model_;
      }

      variant_registry_tree_model & variant_registry_viewer::grab_registry_tree_model()
      {
        DT_THROW_IF(!has_registry_tree_model(), std::logic_error,
                    "Viewer has no registry model!");
        return *_registry_tree_model_;
      }

      const datatools::configuration::variant_registry & variant_registry_viewer::get_registry() const
      {
        return get_registry_tree_model().get_registry();
      }

      void variant_registry_viewer::_construct()
      {
        if (!_registry_tree_model_) {
          return;
        }
        _tree_view_->setModel(_registry_tree_model_);
        _tree_view_->resizeColumnToContents(tree_item::CI_KEY);
        _tree_view_->resizeColumnToContents(tree_item::CI_VALUE);
        _registry_name_display_label_->setAlignment(Qt::AlignLeft);
        //_registry_name_display_label_->setMargin(2);
        //_registry_name_display_label_->setAutoFillBackground(true);
        _registry_name_display_label_->setTextFormat(Qt::PlainText);
        //_registry_name_display_label_->setStyle(QStyleFactory::create("cleanlooks"));
        _registry_name_display_label_->setStyleSheet("QLabel { background-color : white; }");
        _registry_name_display_label_->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
        // if (_registry_tree_model_->get_registry().has_display_name()) {
        //   _registry_name_display_label_->setText(QString::fromStdString(_registry_tree_model_->get_registry().get_display_name()));
        // } else
        if (_registry_tree_model_->get_registry().has_name()) {
          _registry_name_display_label_->setText(QString::fromStdString(_registry_tree_model_->get_registry().get_name()));
        }

        if (_registry_tree_model_->get_registry().has_terse_description()) {
          _registry_name_display_label_->setToolTip(QString::fromStdString(_registry_tree_model_->get_registry().get_terse_description()));
        }

        QObject::connect(_inhibit_secondary_choices_cb_, SIGNAL(stateChanged(int)),
                         this,           SLOT(slot_update_isc_from_check_state(int)));

        QObject::connect(_expand_all_button_, SIGNAL(clicked()),
                         this,                SLOT(slot_expand_all()));

        QObject::connect(_expand_only_active_button_, SIGNAL(clicked()),
                         this,                        SLOT(slot_expand_only_active()));

        QObject::connect(_collapse_all_button_, SIGNAL(clicked()),
                         this,                  SLOT(slot_collapse_all()));

        QObject::connect(this, SIGNAL(sig_logging_changed(datatools::logger::priority)),
                         this, SLOT(slot_update_logging_combo(datatools::logger::priority)));

        QObject::connect(_registry_tree_model_, SIGNAL(sig_registry_data_changed(std::string,std::string)),
                         this,                  SLOT(slot_update_leds()));

        QObject::connect(_registry_tree_model_, SIGNAL(sig_registry_data_changed(std::string,std::string)),
                         this,                  SIGNAL(sig_model_changed()));

        QObject::connect(_registry_tree_model_, SIGNAL(sig_registry_changed(std::string)),
                         this,                  SIGNAL(sig_model_changed()));

        QObject::connect(this, SIGNAL(sig_model_changed()),
                         this, SLOT(slot_expand_only_active()));

        QObject::connect(_registry_tree_model_, SIGNAL(dataChanged(const QModelIndex &,const QModelIndex &)),
                         this,                  SLOT(slot_expand_only_active()));

        QObject::connect(_read_only_cb_, SIGNAL(stateChanged(int)),
                         this,           SLOT(slot_update_model_read_only_from_check_state(int)));

        QObject::connect(_registry_tree_model_, SIGNAL(sig_read_only_changed(bool)),
                         this,                  SLOT(slot_update_read_only_cb(bool)));

        _accomplished_label_->setText("Accomplished:");
        _accomplished_led_->set_shape(datatools::qt::led::Square);
        _accomplished_led_->set_on_color(datatools::qt::led::Green);
        _accomplished_led_->set_off_color(datatools::qt::led::Red);
        _accomplished_led_->show();
        _accomplished_led_->setMinimumSize(12, 12);
        _accomplished_led_->setMaximumSize(14, 14);
        _accomplished_led_->resize(12, 12);

        // Top:
        QHBoxLayout * top_layout = new QHBoxLayout;
        top_layout->addWidget(_registry_name_title_label_);
        top_layout->addWidget(_registry_name_display_label_);
        top_layout->addStretch();
        top_layout->addWidget(_inhibit_secondary_choices_cb_);
        top_layout->addWidget(_accomplished_label_);
        top_layout->addWidget(_accomplished_led_);
        top_layout->addWidget(_read_only_cb_);

        // Bottom:
        QHBoxLayout * bottom_layout = new QHBoxLayout;
        bottom_layout->addStretch();
        bottom_layout->addWidget(_expand_all_button_);
        bottom_layout->addWidget(_expand_only_active_button_);
        bottom_layout->addWidget(_collapse_all_button_);

        if (_devel_mode_) {
          _construct_devel();
          bottom_layout->addStretch();
          bottom_layout->addWidget(_dump_button_);
          bottom_layout->addWidget(_logging_label_);
          bottom_layout->addWidget(_logging_combo_);
          slot_update_logging_combo(_logging_);
        }

        // Main:
        QVBoxLayout * main_layout = new QVBoxLayout;
        main_layout->addLayout(top_layout);
        main_layout->addWidget(_tree_view_);
        main_layout->addLayout(bottom_layout);

        if (get_registry().has_parent_repository()) {
          _value_delegate_->set_parent_repository(get_registry().get_parent_repository());
          set_inhibit_secondary_choices(get_registry().get_parent_repository().get_ui_config().inhibit_secondary_choices);
        }
        slot_update_read_only_cb(_registry_tree_model_->is_read_only());
        // slot_update_isc_cb(_inhibit_secondary_choices_);
        slot_update_leds();
        slot_expand_only_active();

        setLayout(main_layout);
        return;
      }

      void variant_registry_viewer::slot_update_leds()
      {
        if (_registry_tree_model_) {
          bool cia = _registry_tree_model_->get_registry().is_accomplished();
          // std::cerr << "DEVEL: variant_registry_viewer::slot_update_leds: "
          //           << "Registry '" <<  _registry_tree_model_->get_registry().get_name() << "' : cia=" << cia << "\n";
          if (_accomplished_led_) {
            if (cia) {
              _accomplished_led_->set_value(true);
            } else {
              _accomplished_led_->set_value(false);
            }
          }
        }
        return;
      }

      void variant_registry_viewer::slot_update_isc(bool isc_)
      {
        DT_LOG_TRACE(_logging_, "Entering...");
        slot_update_isc_delegate(isc_);
        slot_update_isc_cb(isc_);
        DT_LOG_TRACE(_logging_, "Exiting.");
        return;
      }

      void variant_registry_viewer::slot_update_isc_delegate(bool isc_)
      {
        DT_LOG_TRACE(_logging_, "Entering...");
        if (_value_delegate_) {
          if (isc_) {
            _value_delegate_->set_max_combo_rank(1);
          } else {
            _value_delegate_->set_max_combo_rank(-1);
          }
        }
        DT_LOG_TRACE(_logging_, "Exiting.");
        return;
      }

      void variant_registry_viewer::slot_update_isc_cb(bool isc_)
      {
        DT_LOG_TRACE(_logging_, "Entering...");
        if (_inhibit_secondary_choices_cb_) {
          if (isc_) {
            _inhibit_secondary_choices_cb_->setCheckState(Qt::Checked);
           } else {
            _inhibit_secondary_choices_cb_->setCheckState(Qt::Unchecked);
          }
        }
        DT_LOG_TRACE(_logging_, "Exiting.");
        return;
      }

      void variant_registry_viewer::slot_update_isc_from_check_state(int check_state_)
      {
        DT_LOG_TRACE(_logging_, "Entering...");
        if (check_state_ == Qt::Checked) {
          DT_LOG_TRACE(_logging_, "Inhibit secondary choices ON");
          _inhibit_secondary_choices_ = true;
          _value_delegate_->set_max_combo_rank(1);         
        } else {
          DT_LOG_TRACE(_logging_, "Inhibit secondary choices OFF");
          _inhibit_secondary_choices_ = false;
          _value_delegate_->set_max_combo_rank(-1);
        }
        DT_LOG_TRACE(_logging_, "Inhibit secondary choices = " << _inhibit_secondary_choices_);
        DT_LOG_TRACE(_logging_, "Exiting.");
        return;
      }

      void variant_registry_viewer::slot_update_read_only_cb(bool read_only_)
      {
        DT_LOG_TRACE(_logging_, "Entering...");
        if (_read_only_cb_) {
          if (read_only_) {
            _read_only_cb_->setCheckState(Qt::Checked);
          } else {
            _read_only_cb_->setCheckState(Qt::Unchecked);
          }
        }
        DT_LOG_TRACE(_logging_, "Exiting.");
        return;
      }

      void variant_registry_viewer::slot_update_model_read_only_from_check_state(int check_state_)
      {
        DT_LOG_TRACE(_logging_, "Entering...");
        if (_registry_tree_model_) {
          if (check_state_ == Qt::Checked ) {
            DT_LOG_TRACE(_logging_, "Model read-only ON");
            _registry_tree_model_->set_read_only(true);
          } else {
            DT_LOG_TRACE(_logging_, "Model read-only OFF");
            _registry_tree_model_->set_read_only(false);
          }
        }
        DT_LOG_TRACE(_logging_, "Model read-only = " << _registry_tree_model_->is_read_only());
        DT_LOG_TRACE(_logging_, "Exiting.");
        return;
      }

      void variant_registry_viewer::slot_expand_all()
      {
        _tree_view_->expandAll();
        _tree_view_->resizeColumnToContents(tree_item::CI_KEY);
        _tree_view_->resizeColumnToContents(tree_item::CI_VALUE);
        return;
      }

      void variant_registry_viewer::slot_compute_expand2(const QModelIndex & top_left_index_,
                                                         const QModelIndex & /* bottom_right_index_ */)
      {
        bool devel = false; // _devel_mode_ ;
        static io::indenter indent;
        // std::cerr << "DEVEL: variant_registry_viewer::slot_compute_expand2: "
        //           << "Entering..." << std::endl;
        // std::cerr << "DEVEL: variant_registry_viewer::slot_compute_expand2: "
        //           << "Number of rows = " <<  _registry_tree_model_->rowCount(top_left_index_) << std::endl;
        tree_item * the_node = _registry_tree_model_->node_from_index(top_left_index_);
        if (devel) std::cerr << indent
                             << "DEVEL: variant_registry_viewer::slot_compute_expand2: "
                             << "row,column = "
                             << top_left_index_.row() << "," << top_left_index_.column()
                             << " path='" << the_node->get_record().get_path() << "'"
                             << std::endl;
        int active_children_params_count = 0;
        int active_children_variants_count = 0;
        int number_of_children = _registry_tree_model_->rowCount(top_left_index_);
        for (int row = 0; row < number_of_children; row++) {
          QModelIndex child_index = _registry_tree_model_->index(row, 0, top_left_index_);
          tree_item * child_node = _registry_tree_model_->node_from_index(child_index);
          const std::string & child_path = child_node->get_record().get_path();
          if (devel) std::cerr << indent
                    << "DEVEL: variant_registry_viewer::slot_compute_expand2: "
                    << "child row=" << row
                    << " path='" << child_path << "'"
                    << std::endl;
          bool child_active = child_node->get_record().is_active();
          if (child_active) {
            if (child_node->get_record().is_parameter()) {
              active_children_params_count++;
            }
            if (child_node->get_record().is_variant()) {
              active_children_variants_count++;
            }
            _tree_view_->expand(child_index);
            indent++;
            slot_compute_expand2(child_index, child_index);
            indent--;
          } else {
            _tree_view_->collapse(child_index);
          }
        }
        if (the_node->get_record().is_variant()) {
          if (devel) std::cerr << indent
                    << "DEVEL: variant_registry_viewer::slot_compute_expand2: "
                    << "Number of active param children=" << active_children_params_count << std::endl;
        } else {
          if (devel) std::cerr << indent
                    << "DEVEL: variant_registry_viewer::slot_compute_expand2: "
                    << "Number of active variant children=" << active_children_variants_count << std::endl;
        }
        if (top_left_index_.isValid()
            && active_children_params_count == 0
            && active_children_variants_count == 0) {
          // Collapse an active node if none of its daughters are active:
          // tree_item * node = _registry_tree_model_->node_from_index(top_left_index_);
          // bool active = node->get_record().is_active();
          _tree_view_->collapse(top_left_index_);
        }

        // std::cerr << "DEVEL: variant_registry_viewer::slot_compute_expand2: "
        //           << "Exiting." << std::endl;
        return;
      }

      void variant_registry_viewer::slot_compute_expand(const QModelIndex & /* parent_index_ */)
      {
        // std::cerr << "DEVEL: variant_registry_viewer::slot_compute_expand: "
        //           << "Entering..." << std::endl;
        slot_compute_expand2(QModelIndex(), QModelIndex());
        // std::cerr << "DEVEL: variant_registry_viewer::slot_compute_expand: "
        //           << "Exiting." << std::endl;
        return;
      }

      void variant_registry_viewer::slot_expand_only_active()
      {
        if (_registry_tree_model_) {
          slot_compute_expand(QModelIndex());
          _tree_view_->resizeColumnToContents(tree_item::CI_KEY);
          _tree_view_->resizeColumnToContents(tree_item::CI_VALUE);
        }
        return;
      }

      void variant_registry_viewer::slot_collapse_all()
      {
        _tree_view_->collapseAll();
        _tree_view_->resizeColumnToContents(tree_item::CI_KEY);
        _tree_view_->resizeColumnToContents(tree_item::CI_VALUE);
        return;
      }

      void variant_registry_viewer::slot_dump_registry()
      {
        _registry_tree_model_->get_registry().tree_dump(std::clog, "Registry: ", "");
        return;
      }

      void variant_registry_viewer::slot_dump_model()
      {
        slot_dump_registry();
        _registry_tree_model_->get_registry().tree_dump(std::clog, "Registry: ", "");
        _registry_tree_model_->tree_dump(std::clog, "Registry model: ", "");
        return;
      }

      void variant_registry_viewer::slot_update_logging(const QString & text_)
      {
        std::string logging_label = text_.toUtf8().constData();
        datatools::logger::priority logging_prio = datatools::logger::get_priority(logging_label);
        if (logging_prio != datatools::logger::PRIO_UNDEFINED) {
          set_logging(logging_prio);
        } else {
          // invalid logging priority
        }
        return;
      }

      void variant_registry_viewer::slot_update_logging_combo(datatools::logger::priority logging_)
      {
        if (_logging_combo_) {
          int logging_cur_index = -1;
          for (int i = 0; i < _logging_combo_->count() ; i++) {
            const QString & qlabel = _logging_combo_->itemText(i);
            if (datatools::logger::get_priority_label(logging_)
                == std::string(qlabel.toUtf8().constData())) {
              logging_cur_index = i;
              break;
            }
          }
          if (logging_cur_index != -1) {
            _logging_combo_->setCurrentIndex(logging_cur_index);
          } else {
            // invalid combo index
          }
        }
        return;
      }

      void variant_registry_viewer::slot_snapshot()
      {
        if (_devel_mode_) std::cerr << "DEVEL: variant_registry_viewer::slot_snapshot: Entering..."
                                    << std::endl;
        grab_registry_tree_model().slot_store_to_restore_buffer();
        if (_devel_mode_) std::cerr << "DEVEL: variant_registry_viewer::slot_snapshot: Exiting."
                                    << std::endl;
        return;
      }

      void variant_registry_viewer::slot_restore()
      {
        if (_devel_mode_) std::cerr << "DEVEL: variant_registry_viewer::slot_restore: Entering..."
                                    << std::endl;
        grab_registry_tree_model().slot_load_from_restore_buffer();
        if (_devel_mode_) std::cerr << "DEVEL: variant_registry_viewer::slot_restore: Exiting."
                                    << std::endl;
        return;
      }

      void variant_registry_viewer::slot_load_from_file()
      {
        QString reg_qfile_name = QFileDialog::getOpenFileName(this,
                                                              tr("Open Variant Registry File"),
                                                              ".",
                                                              tr("Variant Registry Files (*.reg)")
                                                              );
        if (! reg_qfile_name.isEmpty()) {
          grab_registry_tree_model().slot_load_from_reg_file(reg_qfile_name);
        }
        return;
      }

      void variant_registry_viewer::slot_store_to_file() const
      {
        QString reg_ofile_name = QFileDialog::getSaveFileName(const_cast<variant_registry_viewer*>(this),
                                                              tr("Save Variant Registry File"),
                                                              ".",
                                                              tr("Variant Registry Files (*.reg)")
                                                              );
        if (! reg_ofile_name.isEmpty()) {
          get_registry_tree_model().slot_store_to_reg_file(reg_ofile_name);
        }
        return;
      }

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools
