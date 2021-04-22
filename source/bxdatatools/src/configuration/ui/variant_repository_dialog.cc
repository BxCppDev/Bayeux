// datatools/configuration/ui/variant_repository_dialog.cc
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
#include <datatools/configuration/ui/variant_repository_dialog.h>

// Standard library:
#include <string>
#include <iostream>
#include <fstream>

// This project:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/configuration/io.h>
#include <datatools/configuration/ui/variant_repository_viewer.h>
#include <datatools/configuration/variant_repository.h>

// Third party:
// - Qt:
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QWidget>

namespace datatools {

  namespace configuration {

    namespace ui {

      void variant_repository_dialog::_set_default()
      {
        _repository_ = 0;
        _hide_disabled_ = false;
        _viewer_ = 0;
        _bottom_buttons_bar_ = 0;
        _snapshot_button_ = 0;
        _restore_button_ = 0;
        _load_button_ = 0;
        _store_button_ = 0;
        _cancel_button_ = 0;
        _ok_button_ = 0;
        _main_layout_ = 0;
        return;
      }

      bool variant_repository_dialog::are_disabled_hidden() const
      {
        return _hide_disabled_;
      }

      variant_repository_dialog::variant_repository_dialog(QWidget * parent_) :
        QDialog(parent_)
      {
        _set_default();
        return;
      }

      variant_repository_dialog::variant_repository_dialog(variant_repository & repository_,
                                                           QWidget * parent_) :
        QDialog(parent_)
      {
        _set_default();
        set_repository(repository_);
        return;
      }

      variant_repository_dialog::variant_repository_dialog(variant_repository & repository_,
                                                           uint32_t flags_,
                                                           QWidget * parent_) :
        QDialog(parent_)
      {
        _set_default();
        if (flags_ & FLAG_HIDE_DISABLED) {
          _hide_disabled_ = true;
        }
        set_repository(repository_);
        return;
      }

      variant_repository_dialog::~variant_repository_dialog()
      {
        // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "Destruction.");
        return;
      }

      bool variant_repository_dialog::has_repository() const
      {
        return _repository_;
      }

      void variant_repository_dialog::set_repository(variant_repository & repository_)
      {
        DT_THROW_IF(has_repository(), std::logic_error,
                    "Repository is already defined!");
        _repository_ = &repository_;
        _construct();
        return;
      }

      bool variant_repository_dialog::has_restore_buffer() const
      {
        return _restore_buffer_.get() ? true : false;
      }

      void variant_repository_dialog::reset_restore_buffer()
      {
        _restore_buffer_.reset();
        return;
      }

      void variant_repository_dialog::_construct()
      {
        setWindowTitle("Bayeux - Configuration variant repository dialog");

        // Main layout of the dialog window:
        _main_layout_ = new QVBoxLayout(this);

        // Create the viewer:
        // _viewer_ = new variant_repository_viewer;
        _viewer_ = new variant_repository_viewer(this);
        _viewer_->set_repository(*_repository_);

        // Create the cancel button:
        _cancel_button_ = new QPushButton(tr("&Cancel"));
        // Create the ok button:
        _ok_button_ = new QPushButton(tr("&Ok"));
        // Create the load button:
        _load_button_ = new QPushButton(tr("&Load"));
        // Create the store button:
        _store_button_ = new QPushButton(tr("&Store"));
        // Create the restorereset button:
        _restore_button_ = new QPushButton(tr("&Restore"));
        // Create the snapshot button:
        _snapshot_button_ = new QPushButton(tr("S&napshot"));

        // Create the bottom bar:
        _bottom_buttons_bar_ = new QHBoxLayout;
        _bottom_buttons_bar_->addStretch();
        _bottom_buttons_bar_->addWidget(_load_button_);
        _bottom_buttons_bar_->addWidget(_store_button_);
        _bottom_buttons_bar_->addWidget(_restore_button_);
        _bottom_buttons_bar_->addWidget(_snapshot_button_);
        _bottom_buttons_bar_->addWidget(_cancel_button_);
        _bottom_buttons_bar_->addWidget(_ok_button_);

        _main_layout_->addWidget(_viewer_);
        _main_layout_->addLayout(_bottom_buttons_bar_);

        QObject::connect(_viewer_, SIGNAL(sig_read_only_changed(bool)),
                         this,     SLOT(slot_update_bottom_buttons(bool))
                         );

        QObject::connect(_load_button_,  SIGNAL(clicked()),
                         _viewer_,       SLOT(slot_load_from_file())
                         );
        QObject::connect(_store_button_, SIGNAL(clicked()),
                         _viewer_,       SLOT(slot_store_to_file())
                         );
        QObject::connect(_restore_button_, SIGNAL(clicked()),
                         this,             SLOT(slot_restore())
                         );
        QObject::connect(_snapshot_button_, SIGNAL(clicked()),
                         this,              SLOT(slot_snapshot())
                         );
        QObject::connect(_cancel_button_, SIGNAL(clicked()),
                         this,            SLOT(slot_cancel())
                         );
        QObject::connect(_ok_button_, SIGNAL(clicked()),
                         this,        SLOT(accept())
                         );

        // Snapshot its current state:
        slot_snapshot();
        slot_update_bottom_buttons(_viewer_->is_read_only());

        return;
      }

      void variant_repository_dialog::slot_cancel()
      {
        if (has_restore_buffer()) {
          slot_restore();
        }
        reject();
        return;
      }

      void variant_repository_dialog::slot_update_bottom_buttons(bool read_only_)
      {
        bool write_buttons_enabled = ! read_only_;
        if (_restore_button_) {
          if (! has_restore_buffer()) {
            _restore_button_->setEnabled(false);
          } else {
            _restore_button_->setEnabled(write_buttons_enabled);
          }
        }
        if (_load_button_) {
          _load_button_->setEnabled(write_buttons_enabled);
        }
        if (_cancel_button_) {
          // bool enabled = true;
          // if (!_viewer_->has_repository()) {
          //   enabled = false;
          // } else {
          //   if (_viewer_->get_repository().is_locked()) {
          //     enabled = false;
          //   }
          // }
          // _cancel_button_->setEnabled(enabled);
          _cancel_button_->setEnabled(write_buttons_enabled);
        }
        return;
      }

      void variant_repository_dialog::slot_snapshot()
      {
        if (! has_restore_buffer()) {
          _restore_buffer_.reset(new std::string);
        }
        if (_viewer_) {
          _viewer_->slot_snapshot(_restore_buffer_.get());
        }
        return;
      }

      void variant_repository_dialog::slot_restore()
      {
        if (_viewer_) {
          if (has_restore_buffer()) {
            _viewer_->slot_restore(_restore_buffer_.get());
          }
        }
        return;
      }

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools
