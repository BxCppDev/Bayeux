/// \file datatools/qt/interface.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-11-30
 * Last modified: 2014-12-02
 *
 * License:
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
 *  Qt interface singleton.
 *
 */

// This code is inspired from the G4Qt class by Laurent Garnier,
// from the Geant4 Collaboration.
//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#ifndef DATATOOLS_QT_INTERFACE_H
#define DATATOOLS_QT_INTERFACE_H

// Forward declaration:
class QApplication;

namespace datatools {

  namespace qt {

    class interface_impl;

    /// \brief Interface to Qt
    class interface
    {

    private:

      /// Constructor
      interface(int argc_ = 0, char ** argv_ = 0, const char * name_ = 0);

    public:

      /// Destructor
      virtual ~interface();

      /// Initialize and return the singleton
      static interface & instance();

      /// Initialize and return the singleton
      static interface & instance(int argc_, char ** argv_, const char * name_ = 0);

      /// Check initialization status
      bool is_initialized() const;

      /// Check if Qt application is external
      bool is_external_app() const;

    private:

      // Should we PIMPL ?
      // boost::scope_ptr<interface_impl> _impl_;

      int _argc_;            //!< Local copy of the command line argument count
      char ** _argv_;        //!< Local copy of the command line arguments
      bool _arg_owned_;      //!< Flag for command line arguments ownership
      bool _external_app_;   //!< Flag for external Qt application
      bool _qt_initialized_; //!< Flag for Qt initialization
      QApplication * _qt_app_; //!< Handle to the local Qt application

    };

  } // end of namespace qt

} // end of namespace datatools

#endif // DATATOOLS_QT_INTERFACE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
