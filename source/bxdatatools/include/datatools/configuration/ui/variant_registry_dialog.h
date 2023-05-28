/// \file datatools/configuration/ui/variant_registry_dialog.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-10-06
 * Last modified : 2014-10-06
 *
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
 * Description:
 *
 *    A dialog box for configuration variant registry.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_UI_VARIANT_REGISTRY_DIALOG_H
#define DATATOOLS_CONFIGURATION_UI_VARIANT_REGISTRY_DIALOG_H

// Standard library:
#include <memory>

// Third party:
// - Qt:
#include <QDialog>
#include <QObject>

// Forward declaration:
class QWidget;
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;

namespace datatools {

  namespace configuration {

    class variant_registry;

    namespace ui {

      // Forward declaration:
      class variant_registry_tree_model;
      class variant_registry_viewer;

      /// \brief A widget to browse/edit a configuration variant registry
      class variant_registry_dialog : public QDialog
      {
        Q_OBJECT

      public:

        /// Constructor
        variant_registry_dialog(variant_registry & registry_,
                                QWidget * parent_ = 0);

        /// Constructor
        variant_registry_dialog(variant_registry_tree_model & registry_tree_model_,
                                QWidget * parent_ = 0);

        /// Destructor
        virtual ~variant_registry_dialog();

        /// Check if a restore buffer is available
        bool has_restore_buffer() const;

        /// Reset the restore buffer is available
        void reset_restore_buffer();

      public slots:

        /// Slot for canceling the dialog interface
        void slot_cancel();

        /// Slot for updating the status of bottom buttons (load/reset)
        void slot_update_bottom_buttons(bool read_only_);

        /// Slot for snapshot
        void slot_snapshot();

        /// Slot for restore
        void slot_restore();

      protected:

        /// Construct the GUI interface
        void _construct();

        /// Set default
        void _set_default();

      private:

        variant_registry            * _registry_; //!< Handle to a registry tree model
        bool                          _owned_model_; //!< Flag for model ownership
        variant_registry_tree_model * _registry_tree_model_; //!< Handle to a registry tree model

        // Widgets:
        variant_registry_viewer * _viewer_;  //!< The embedded viewer widget
        QHBoxLayout  * _bottom_buttons_bar_; //!< Button bar for buttons
        QPushButton  * _restore_button_;     //!< The restore button
        QPushButton  * _snapshot_button_;    //!< The snapshot button
        QPushButton  * _load_button_;        //!< The load button
        QPushButton  * _store_button_;       //!< The store button
        QPushButton  * _cancel_button_;      //!< The cancel button
        QPushButton  * _ok_button_;          //!< The ok button
        QVBoxLayout  * _main_layout_;        //!< The main layout

        // Restore buffer:
       std::unique_ptr<std::string> _restore_buffer_; //!< Handle to the restore buffer

      };

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_UI_VARIANT_REGISTRY_DIALOG_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
