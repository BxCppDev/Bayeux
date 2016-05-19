// datatools/configuration/ui/variant_registry_dialog.cc
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
#include <datatools/configuration/ui/variant_registry_dialog.h>

// Standard library:
#include <string>
#include <iostream>
#include <fstream>

// This project:
#include <datatools/exception.h>
#include <datatools/configuration/io.h>
#include <datatools/configuration/ui/variant_registry_viewer.h>
#include <datatools/configuration/ui/variant_registry_tree_model.h>

// Third party:
// - Qt:
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QString>

namespace datatools {

  namespace configuration {

    namespace ui {

      void variant_registry_dialog::_set_default()
      {
        _registry_ = 0;
        _owned_model_ = false;
        _registry_tree_model_ = 0;
        _viewer_ = 0;

        _bottom_buttons_bar_ = 0;
        _snapshot_button_ = 0;
        _restore_button_ = 0;
        _load_button_ = 0;
        _store_button_ = 0;
        _cancel_button_ = 0;
        _ok_button_ = 0;

        _main_layout_ = 0;
        _restore_buffer_.reset();
        return;
      }

      bool variant_registry_dialog::has_restore_buffer() const
      {
        return _restore_buffer_.get() ? true : false;
      }

      void variant_registry_dialog::reset_restore_buffer()
      {
        _restore_buffer_.reset();
        return;
      }


      variant_registry_dialog::variant_registry_dialog(variant_registry & registry_,
                                                       QWidget * parent_) :
        QDialog(parent_)
      {
        _set_default();

        _registry_ = &registry_;
        _owned_model_ = true;
        _registry_tree_model_ = new variant_registry_tree_model(this);
        _registry_tree_model_->construct(registry_);

        // Construct the GUI:
        _construct();

        return;
      }

      variant_registry_dialog::variant_registry_dialog(variant_registry_tree_model & registry_tree_model_,
                                                       QWidget * parent_) :
        QDialog(parent_)
      {
        _set_default();
        _owned_model_ = false;
        _registry_tree_model_ = &registry_tree_model_;

        // Construct the GUI:
        _construct();

        return;
      }

      variant_registry_dialog::~variant_registry_dialog()
      {
        if (_owned_model_) {
          if (_registry_tree_model_) {
            delete _registry_tree_model_;
          }
        }
        return;
      }

      void variant_registry_dialog::_construct()
      {
        setWindowTitle("Bayeux - Configuration variant registry dialog");

        // Main layout of the dialog window:
        _main_layout_ = new QVBoxLayout(this);

        // Create the viewer:
        _viewer_ = new variant_registry_viewer;

        _viewer_->set_registry_tree_model(*_registry_tree_model_);

        // Snapshot its current state
        _viewer_->slot_snapshot();


        // Create the cancel button:
        _cancel_button_ = new QPushButton(tr("&Cancel"));
        // Create the ok button:
        _ok_button_ = new QPushButton(tr("&Ok"));
        // Create the load button:
        _load_button_ = new QPushButton(tr("&Load"));
        // Create the store button:
        _store_button_ = new QPushButton(tr("&Store"));
        // Create the restore button:
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

        slot_snapshot();
        slot_update_bottom_buttons(_viewer_->get_registry_tree_model().is_read_only());


        QObject::connect(_registry_tree_model_, SIGNAL(sig_read_only_changed(bool)),
                         this,                 SLOT(slot_update_bottom_buttons(bool))
                         );
        QObject::connect(_load_button_, SIGNAL(clicked()),
                         _viewer_,      SLOT(slot_load_from_file())
                         );
        QObject::connect(_store_button_, SIGNAL(clicked()),
                         _viewer_,       SLOT(slot_store_to_file())
                         );
        QObject::connect(_restore_button_, SIGNAL(clicked()),
                         this,         SLOT(slot_restore())
                         );
        QObject::connect(_snapshot_button_, SIGNAL(clicked()),
                         this,          SLOT(slot_snapshot())
                         );
        QObject::connect(_ok_button_, SIGNAL(clicked()),
                         this,           SLOT(accept())
                         );
        QObject::connect(_cancel_button_, SIGNAL(clicked()),
                         this,           SLOT(slot_cancel())
                         );


        return;
      }

      void variant_registry_dialog::slot_snapshot()
      {
        if (! has_restore_buffer()) {
          _restore_buffer_.reset(new std::string);
        }
        if (_viewer_ && _viewer_->has_registry_tree_model()) {
          _viewer_->grab_registry_tree_model().slot_store_to_restore_buffer(_restore_buffer_.get());
        }
        return;
      }

      void variant_registry_dialog::slot_restore()
      {
        if (_viewer_ && _viewer_->has_registry_tree_model()) {
          if (has_restore_buffer()) {
            _viewer_->grab_registry_tree_model().slot_load_from_restore_buffer(_restore_buffer_.get());
          }
        }
       return;
      }

      void variant_registry_dialog::slot_cancel()
      {
        slot_restore();
        reject();
        return;
      }

      void variant_registry_dialog::slot_update_bottom_buttons(bool read_only_)
      {
        bool write_buttons_enabled = ! read_only_;

        if (_restore_button_) {
          if (_viewer_ && _viewer_->has_registry_tree_model()) {
            if (!_registry_tree_model_ || !_registry_tree_model_->has_registry()) {
              _restore_button_->setEnabled(false);
            } else {
              if (!has_restore_buffer()) {
                _restore_button_->setEnabled(false);
              } else {
                _restore_button_->setEnabled(write_buttons_enabled);
              }
            }
          } else {
            _restore_button_->setEnabled(false);
          }
        }

        if (_load_button_) {
          _load_button_->setEnabled(write_buttons_enabled);
        }

        return;
      }

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools
