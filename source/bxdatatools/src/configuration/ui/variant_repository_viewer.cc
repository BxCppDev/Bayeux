// datatools/configuration/ui/variant_repository_viewer.cc
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
#include <datatools/configuration/ui/variant_repository_viewer.h>

// Standard library:
#include <string>
#include <iostream>
#include <fstream>

// This project:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/io.h>
#include <datatools/configuration/ui/variant_registry_tree_model.h>
#include <datatools/configuration/ui/variant_registry_viewer.h>
#include <datatools/qt/led.h>

// Third party:
// - Boost:
#include <boost/algorithm/string/predicate.hpp>
// - Qt:
#include <QTabWidget>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QFileDialog>
#include <QIcon>
// #include <QMessageBox>

namespace datatools {

  namespace configuration {

    namespace ui {

      registry_model_wrapper::registry_model_wrapper(QWidget * parent_)
      {
        _flags_ = 0;
        _parent_ = parent_;
        _model_ = 0;
        _viewer_ = 0;
        return;
      }

      registry_model_wrapper::~registry_model_wrapper()
      {
        reset();
        return;
      }

      void registry_model_wrapper::reset()
      {
        if (_viewer_) {
          delete _viewer_;
          _viewer_ = 0;
        }
        if (_model_) {
          delete _model_;
          _model_ = 0;
        }
        _flags_ = 0;
        return;
      }

      bool registry_model_wrapper::has_model() const
      {
        return _model_ != 0;
      }

      const variant_registry_tree_model &
      registry_model_wrapper::get_model() const
      {
        return *_model_;
      }

      variant_registry_tree_model &
      registry_model_wrapper::grab_model()
      {
        return *_model_;
      }

      bool registry_model_wrapper::has_viewer() const
      {
        return _viewer_ != 0;
      }

      const variant_registry_viewer &
      registry_model_wrapper::get_viewer() const
      {
        return *_viewer_;
      }

      variant_registry_viewer &
      registry_model_wrapper::grab_viewer()
      {
        return *_viewer_;
      }

      variant_registry_viewer *
      registry_model_wrapper::construct_registry_viewer(variant_registry & registry_, bool sealed_)
      {
        _model_ = new variant_registry_tree_model;
        _model_->set_sealed(sealed_);
        _model_->construct(registry_);
        _viewer_ = new variant_registry_viewer(_parent_);
        _viewer_->set_registry_tree_model(*_model_);
        return _viewer_;
      }

      void variant_repository_viewer::_construct_devel()
      {
        _dump_button_ = new QPushButton("&Dump: ", this);
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
        QObject::connect(_logging_combo_, SIGNAL(currentIndexChanged(const QString &)),
                         this, SLOT(slot_update_logging(const QString &)));
        QObject::connect(_dump_button_, SIGNAL(clicked()),
                         this, SLOT(slot_dump()));

        return;
      }

      variant_repository_viewer::variant_repository_viewer(QWidget * parent_)
        : QWidget(parent_)
      {
        _devel_mode_ = false;
        _logging_ = datatools::logger::PRIO_FATAL;
        _repository_ = nullptr;
        _read_only_ = true;

        _repository_name_title_label_ = new QLabel(this);
        _repository_name_title_label_->setText("Configuration repository:");
        _repository_name_display_label_ = new QLabel(this);
        _repository_org_title_label_ = new QLabel(this);
        _repository_org_title_label_->setText("Organization:");
        _repository_org_display_label_ = new QLabel(this);
        _repository_app_title_label_ = new QLabel(this);
        _repository_app_title_label_->setText("Application:");
        _repository_app_display_label_ = new QLabel(this);

        _accomplished_label_ = new QLabel(this);
        _accomplished_led_ = new datatools::qt::led(this);
        _read_only_cb_ = new QCheckBox("Read-only", this);
        _registry_tabs_ = new QTabWidget;

        setWindowTitle(tr("Repository Viewer"));
        return;
      }

      variant_repository_viewer::~variant_repository_viewer()
      {
        return;
      }

      void variant_repository_viewer::set_logging(datatools::logger::priority p_)
      {
        _logging_ = p_;
        return;
      }

      bool variant_repository_viewer::has_repository() const
      {
        return _repository_ != nullptr;
      }

      void variant_repository_viewer::set_repository(variant_repository & repository_)
      {
        DT_THROW_IF(!repository_.is_initialized(),
                    std::logic_error,
                    "Repository '" << repository_.get_name() << "' is not initialized!");
        DT_THROW_IF(!repository_.is_locked(),
                    std::logic_error,
                    "Repository '" << repository_.get_name() << "' is not locked!");
        _repository_ = &repository_;
        _construct();
        return;
      }

      variant_repository & variant_repository_viewer::grab_repository()
      {
        DT_THROW_IF(_repository_ == nullptr, std::logic_error, "No repository!");
        return *_repository_;
      }

      const variant_repository & variant_repository_viewer::get_repository() const
      {
        DT_THROW_IF(_repository_ == nullptr, std::logic_error, "No repository!");
        return *_repository_;
      }

      bool variant_repository_viewer::is_read_only() const
      {
        return _read_only_;
      }

      void variant_repository_viewer::set_read_only(bool ro_)
      {
        if (_read_only_ != ro_) {
          _read_only_ = ro_;
          emit sig_read_only_changed(_read_only_);
        }
        return;
      }

      bool variant_repository_viewer::has_restore_buffer() const
      {
        return _restore_buffer_.get() ? true : false;
      }

      void variant_repository_viewer::reset_restore_buffer()
      {
        _restore_buffer_.reset();
        return;
      }

      void variant_repository_viewer::_construct()
      {
        if (!_repository_) {
          return;
        }
        _read_only_ = !_repository_->get_ui_config().writable_at_start;
        
        // Top:
        _repository_name_display_label_->setAlignment(Qt::AlignLeft);
        _repository_name_display_label_->setTextFormat(Qt::PlainText);
        _repository_name_display_label_->setStyleSheet("QLabel { background-color : white; }");
        _repository_name_display_label_->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
        if (_repository_->has_display_name()) {
          _repository_name_display_label_->setText(QString::fromStdString(_repository_->get_display_name()));
        } else if (_repository_->has_name()) {
          _repository_name_display_label_->setText(QString::fromStdString(_repository_->get_name()));
        }
        if (_repository_->has_terse_description()) {
          _repository_name_display_label_->setToolTip(QString::fromStdString(_repository_->get_terse_description()));
        }

        if (_repository_->has_organization()) {
          _repository_org_display_label_->setTextFormat(Qt::PlainText);
          _repository_org_display_label_->setStyleSheet("QLabel { background-color : white; }");
          _repository_org_display_label_->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
          _repository_org_display_label_->setText(QString::fromStdString(_repository_->get_organization()));
        }

        if (_repository_->has_application()) {
          _repository_app_display_label_->setTextFormat(Qt::PlainText);
          _repository_app_display_label_->setStyleSheet("QLabel { background-color : white; }");
          _repository_app_display_label_->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
          _repository_app_display_label_->setText(QString::fromStdString(_repository_->get_application()));
        }

        _accomplished_label_->setText("Accomplished:");
        _accomplished_led_->set_shape(datatools::qt::led::Square);
        _accomplished_led_->set_on_color(datatools::qt::led::Green);
        _accomplished_led_->set_off_color(datatools::qt::led::Red);
        _accomplished_led_->show();
        _accomplished_led_->setMinimumSize(12, 12);
        _accomplished_led_->setMaximumSize(14, 14);
        _accomplished_led_->resize(12, 12);

        slot_update_read_only_cb(is_read_only());

        QHBoxLayout * top_layout = new QHBoxLayout;
        top_layout->addWidget(_repository_name_title_label_);
        top_layout->addWidget(_repository_name_display_label_);
        top_layout->addStretch();
        top_layout->addWidget(_accomplished_label_);
        top_layout->addWidget(_accomplished_led_);
        top_layout->addWidget(_read_only_cb_);

        QHBoxLayout * top_layout2 = new QHBoxLayout;
        top_layout2->addWidget(_repository_org_title_label_);
        top_layout2->addWidget(_repository_org_display_label_);
        top_layout2->addWidget(_repository_app_title_label_);
        top_layout2->addWidget(_repository_app_display_label_);

        std::vector<std::string> registry_keys;
        _repository_->build_ordered_registry_keys(registry_keys);

        // For each registry in the repository:
        for (unsigned int ikey = 0; ikey < registry_keys.size(); ikey++) {
          const std::string & reg_name = registry_keys[ikey];
          bool sealedRegistry = _repository_->is_sealed_registry(reg_name);
          variant_registry & var_reg = _repository_->grab_registry(reg_name);
          // Add a new registry UI wrapper in the viewer:
          {
            registry_model_wrapper dummy(this);
            _models_[reg_name] = dummy;
          }
          registry_model_wrapper & reg_model_wrap = _models_.find(reg_name)->second;
          variant_registry_viewer * reg_viewer = reg_model_wrap.construct_registry_viewer(var_reg, sealedRegistry);
          QString tab_title;
          if (var_reg.has_display_name()) {
            tab_title = QString::fromStdString(var_reg.get_display_name());
          } else if (var_reg.has_name()) {
            tab_title = QString::fromStdString(var_reg.get_name());
          }
          _registry_tabs_->addTab(reg_viewer, tab_title);
          _tab_indexes_[reg_name] = ikey;

          QObject::connect(this,       SIGNAL(sig_repository_changed()),
                           reg_viewer, SIGNAL(sig_model_changed()));

          QObject::connect(reg_viewer, SIGNAL(sig_model_changed()),
                           this,       SLOT(slot_update()));

          QObject::connect(&reg_viewer->grab_registry_tree_model(), SIGNAL(sig_registry_data_changed(std::string,std::string)),
                           this,       SLOT(slot_at_registry_data_changed(std::string,std::string)));

          QObject::connect(&reg_viewer->grab_registry_tree_model(), SIGNAL(sig_registry_changed(std::string)),
                           this,       SLOT(slot_at_registry_changed(std::string)));
        }

        QObject::connect(this, SIGNAL(sig_read_only_changed(bool)),
                         this, SLOT(slot_update_read_only_cb(bool))
                         );

        QObject::connect(this, SIGNAL(sig_read_only_changed(bool)),
                         this, SLOT(slot_at_set_read_only(bool))
                         );

        QObject::connect(_read_only_cb_, SIGNAL(stateChanged(int)),
                         this,           SLOT(slot_update_read_only_from_check_state(int)));

        // Main:
        QVBoxLayout * main_layout = new QVBoxLayout;
        main_layout->addLayout(top_layout);
        main_layout->addLayout(top_layout2);
        main_layout->addWidget(_registry_tabs_);

        {
          _null_icon_ = new QIcon;
          _unaccomplished_icon_ = new QIcon;
          std::string icon_filename("@datatools:qt/led/square_red.svg");
          datatools::fetch_path_with_env(icon_filename);
          QString s(icon_filename.c_str());
          QSize sz(14, 14);
          _unaccomplished_icon_->addFile(s, sz, QIcon::Normal, QIcon::Off);
        }

        slot_update();

        if (_devel_mode_) {
          _construct_devel();
          // Bottom:
          QHBoxLayout * bottom_layout = new QHBoxLayout;
          bottom_layout->addStretch();
          bottom_layout->addWidget(_dump_button_);
          bottom_layout->addWidget(_logging_label_);
          bottom_layout->addWidget(_logging_combo_);
          main_layout->addLayout(bottom_layout);
          // slot_update_logging_combo(_logging_);
        }

        setLayout(main_layout);
        return;
      }

      void variant_repository_viewer::slot_at_registry_changed(std::string /*changed_registry_name_*/)
      {
        // std::cerr << "DEVEL: " << "variant_repository_viewer::slot_at_registry_changed: "
        //           << "Registry '" << changed_registry_name_ << "' has changed!"
        //           << std::endl;
        slot_update();
        return;
      }

      void variant_repository_viewer::slot_update()
      {
        slot_update_tabs();
        slot_update_leds();
        return;
      }

      void variant_repository_viewer::slot_update_tabs()
      {
        datatools::logger::priority logging = _logging_;
        DT_LOG_TRACE_ENTERING(logging);
        std::vector<std::string> registry_keys;
        _repository_->build_ordered_registry_keys(registry_keys);
        for (unsigned int ikey = 0; ikey < registry_keys.size(); ikey++) {
          const std::string & the_registry_name = registry_keys[ikey];
          const variant_registry & vreg = _repository_->get_registry(the_registry_name);
          int reg_tab_index = _tab_indexes_.find(the_registry_name)->second;
          bool active = _repository_->is_active_registry(the_registry_name);
          _registry_tabs_->setTabEnabled(reg_tab_index, active);
          if (!vreg.is_accomplished()) {
            _registry_tabs_->setTabIcon(reg_tab_index, *_unaccomplished_icon_);
          } else {
            _registry_tabs_->setTabIcon(reg_tab_index, *_null_icon_);
          }
          if (active) {
            if (_repository_->has_dependency_model()) {
              QWidget * w = _registry_tabs_->widget(reg_tab_index);
              variant_registry_viewer * reg_viewer = dynamic_cast<variant_registry_viewer*>(w);
              reg_viewer->slot_update_leds();
            }
          }
        }
        DT_LOG_TRACE_EXITING(_logging_);
        return;
      }

      void variant_repository_viewer::slot_at_registry_data_changed(std::string changed_registry_name_,
                                                                    std::string changed_data_path_)
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        DT_LOG_TRACE(_logging_,
                     "Data '" << changed_data_path_ << "' in registry '"
                     << changed_registry_name_ << "' has changed!");

        std::vector<std::string> registry_keys;
        _repository_->build_ordered_registry_keys(registry_keys);

        // Compute consequence on depender registries...
        for (unsigned int ikey = 0; ikey < registry_keys.size(); ikey++) {
          const std::string & the_registry_name = registry_keys[ikey];
          const variant_registry & var_reg = _repository_->grab_registry(the_registry_name);
          if (the_registry_name == changed_registry_name_) {
            // Do not process the changed registry itself:
            continue;
          }
          // const variant_repository::registry_entry & re = ireg->second;
          // const variant_registry & vreg = re.get_registry();
          if (!var_reg.has_dependency_model()) {
            // No local dependency model for this registry:
            continue;
          }
        }
        DT_LOG_TRACE_EXITING(_logging_);
        return;
      }

      void variant_repository_viewer::slot_update_read_only_from_check_state(int check_state_)
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        if (check_state_ == Qt::Checked ) {
          DT_LOG_TRACE(_logging_, "Read-only ON");
          set_read_only(true);
        } else {
          DT_LOG_TRACE(_logging_, "Read-only OFF");
          set_read_only(false);
        }
        DT_LOG_TRACE(_logging_, "Read-only = " << is_read_only());
        DT_LOG_TRACE_EXITING(_logging_);
        return;
      }

      void variant_repository_viewer::slot_update_read_only_cb(bool ro_)
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        if (_read_only_cb_) {
          if (ro_) {
            _read_only_cb_->setCheckState(Qt::Checked);
          } else {
            _read_only_cb_->setCheckState(Qt::Unchecked);
          }
        }
        DT_LOG_TRACE_EXITING(_logging_);
        return;
      }

      void variant_repository_viewer::slot_at_set_read_only(bool ro_)
      {
        DT_LOG_TRACE_ENTERING(_logging_);

        std::vector<std::string> registry_keys;
        _repository_->build_ordered_registry_keys(registry_keys);
        for (unsigned int ikey = 0; ikey < registry_keys.size(); ikey++) {
          const std::string & reg_name = registry_keys[ikey];
          registry_model_wrapper & reg_model_wrap = _models_.find(reg_name)->second;
          variant_registry_viewer & reg_viewer = reg_model_wrap.grab_viewer();
          reg_viewer.grab_registry_tree_model().set_read_only(ro_);
        }
        return;
      }

      void variant_repository_viewer::slot_update_leds()
      {
        if (_repository_) {
          bool ria = _repository_->is_accomplished();
          if (_accomplished_led_) {
            if (ria) {
              _accomplished_led_->set_value(true);
            } else {
              _accomplished_led_->set_value(false);
            }
          }
        }
        return;
      }

      void variant_repository_viewer::slot_dump() const
      {
        if (!_repository_) {
          return;
        }
        _repository_->tree_dump(std::clog, "Variant repository: ");
        return;
      }

      void variant_repository_viewer::slot_update_logging(const QString & text_)
      {
        std::string logging_label = text_.toUtf8().constData();
        datatools::logger::priority logging_prio = datatools::logger::get_priority(logging_label);
        if (logging_prio != datatools::logger::PRIO_UNDEFINED) {
          set_logging(logging_prio);
        }
        return;
      }

      void variant_repository_viewer::slot_snapshot(std::string * buffer_)
      {
        if (!has_repository()) {
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
        ascii_io rep_io(ascii_io::IO_DEFAULT);
        rep_io.store_repository(out, this->get_repository());
        *buf = out.str();
        return;
      }

      void variant_repository_viewer::slot_restore(std::string * buffer_)
      {
        if (! has_repository()) {
          return;
        }
        bool process = true;
        if (this->get_repository().is_locked()) {
          process = false;
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Repository is locked!");
          // 2016-06-15 FM: The following segfaults after at button click!!!
          // QMessageBox * msgBox = new QMessageBox(this);
          // msgBox->setIcon(QMessageBox::Warning);
          // msgBox->setText("The variant repository is locked.");
          // msgBox->setInformativeText("Please unlock the repository before restoring its state from the restoration buffer!");
          // // msgBox.setStandardButtons(QMessageBox::Ok);
          // msgBox->show();
          // process = false;
          // int ret = msgBox->exec();
          // switch (ret) {
          // case QMessageBox::Ok:
          //   // Ok was clicked
          //   process = false;
          //   break;
          // default:
          //   // should never be reached
          //   process = false;
          //   break;
          // }
          // delete msgBox;
        }
        if (process) {
          bool was_locked = this->get_repository().is_locked();
          if (this->get_repository().is_locked()) {
            this->grab_repository().unlock();
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
          ascii_io rep_io(ascii_io::IO_DEFAULT);
          int error = rep_io.load_repository(in, this->grab_repository());
          if (error) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Failed to load repository from restore buffer!");
          } else {
            emit sig_repository_changed();
          }
          if (was_locked) {
            this->grab_repository().lock();
          }
        } else {
          std::cerr << "DEVEL: Do not process..." << std::endl;
        }
        return;
      }

      void variant_repository_viewer::slot_load_from_file()
      {
        QString in_rep_filename = QFileDialog::getOpenFileName(this,
                                                               tr("Open Variant Repository File"),
                                                               ".",
                                                               tr("Variant Repository Files (*.rep)")
                                                               );
        std::string rep_file_name = in_rep_filename.toStdString();
        // std::cerr << "DEVEL: rep_file_name='" << rep_file_name << "'" << std::endl;
        if (rep_file_name.empty()) {
           return;
        }
        bool save_rep_lock = this->get_repository().is_locked();
        if (save_rep_lock) {
          this->grab_repository().unlock();
        }
        try {
          std::ifstream rep_file;
          rep_file.open(rep_file_name.c_str());
          if (!rep_file) {
            DT_THROW_IF(true, std::runtime_error, "Cannot open file '"  << rep_file_name << "'!");
          }
          //this->beginResetModel();
          ascii_io rep_io(ascii_io::IO_DEFAULT);
          // rep_io.set_logging(logger::PRIO_TRACE);
          int error = rep_io.load_repository(rep_file, this->grab_repository());
          if (error) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                         "Failed to load repository from file '" << rep_file_name << "'");
          } else {
            emit sig_repository_changed();
          }
        } catch (std::exception & error) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                         "Failed to load repository : " << error.what());
        }
        if (save_rep_lock) {
          this->grab_repository().lock();
        }
        // std::cerr << "DEVEL: repository was loaded from file '" << rep_file_name << "'" << std::endl;
        //this->endResetModel();
        return;
      }

      void variant_repository_viewer::slot_store_to_file()
      {
        QString out_rep_filename = QFileDialog::getSaveFileName(const_cast<variant_repository_viewer*>(this),
                                                                tr("Save Variant Repository File"),
                                                                ".",
                                                                tr("Variant Repository Files (*.rep)")
                                                                );
        std::string rep_file_name = out_rep_filename.toStdString();
        // std::cerr << "DEVEL: rep_file_name='" << rep_file_name << "'" << std::endl;
        if (rep_file_name.empty()) {
          return;
        }
        try {
          std::ofstream rep_file;
          rep_file.open(rep_file_name.c_str());
          if (!rep_file) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR,"Cannot open file '"  << rep_file_name << "'!");
            return;
          }
          // std::cerr << "DEVEL: variant_repository_viewer::slot_store_to_file: ..." << std::endl;
          ascii_io rep_io(ascii_io::IO_DEFAULT);
          rep_io.store_repository(rep_file, this->get_repository());
          //std::cerr << "DEVEL: repository was stored in file '" << rep_file_name << "'" << std::endl;
        } catch (std::exception & error) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                       "Failed to load repository : " << error.what());
        }
        return;
      }

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools
