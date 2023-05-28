/// \file datatools/configuration/ui/variant_repository_viewer.h
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
 *    A dialog box for configuration variant repository.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_UI_VARIANT_REPOSITORY_DIALOG_H
#define DATATOOLS_CONFIGURATION_UI_VARIANT_REPOSITORY_DIALOG_H

// Standard library:
#include <memory>

// Third party:
// - Qt:
#include <QDialog>
#include <QObject>

// This project:
#include <datatools/bit_mask.h>

// Forward declaration:
class QWidget;
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;

namespace datatools {

  namespace configuration {

    // Forward declaration:
    class variant_repository;

    namespace ui {

      // Forward declaration:
      class variant_repository_viewer;

      /// \brief A widget to browse/edit a configuration variant repository
      class variant_repository_dialog : public QDialog
      {
        Q_OBJECT

      public:

        enum flags_type
          {
           FLAG_HIDE_DISABLED = datatools::bit_mask::bit00
          };

        /// Default Constructor
        variant_repository_dialog(QWidget * parent_ = nullptr);

        /// Constructor
        variant_repository_dialog(variant_repository & repository_,
                                  QWidget * parent_ = nullptr);
        /// Constructor
        variant_repository_dialog(variant_repository & repository_,
                                  uint32_t flags_,
                                  QWidget * parent_ = nullptr);

        /// Destructor
        virtual ~variant_repository_dialog();

        bool are_disabled_hidden() const;

        /// Check if the repository handle is set
        bool has_repository() const;

        /// Set repository
        void set_repository(variant_repository & repository_);

        /// Check if a restore buffer is available
        bool has_restore_buffer() const;

        /// Reset the restore buffer is available
        void reset_restore_buffer();

      public slots:

        /// Slot for updating the status of bottom buttons (load/reset)
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

        /// Set  default
        void _set_default();

      private:

        variant_repository * _repository_ = nullptr; //!< Handle to a repository
        bool _hide_disabled_ = false;

        // Widgets:
        variant_repository_viewer * _viewer_ = nullptr; //!< The embedded viewer widget
        QHBoxLayout  * _bottom_buttons_bar_ = nullptr;  //!< Bottom bar for buttons
        QPushButton  * _restore_button_ = nullptr;      //!< The reset button
        QPushButton  * _snapshot_button_ = nullptr;     //!< The snapshot button
        QPushButton  * _load_button_ = nullptr;         //!< The load button
        QPushButton  * _store_button_ = nullptr;        //!< The store button
        QPushButton  * _cancel_button_ = nullptr;       //!< The cancel button
        QPushButton  * _ok_button_ = nullptr;           //!< The ok button
        QVBoxLayout  * _main_layout_ = nullptr;         //!< The main layout

        // Restore buffer;
        std::unique_ptr<std::string> _restore_buffer_; //!< Handle to the restore buffer

      };

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_UI_VARIANT_REPOSITORY_DIALOG_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
