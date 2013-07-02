// -*- mode: c++ ; -*-
/* run_action.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-10
 * Last modified: 2013-03-09
 *
 * License:
 *
 * Description:
 *
 *   G4 user run action class
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_RUN_ACTION_H_
#define MCTOOLS_G4_RUN_ACTION_H_ 1

#include <string>

#include <boost/cstdint.hpp>

// Utilities :
#include <datatools/properties.h>

// Reader/writer :
#include <datatools/io_factory.h>
#include <brio/writer.h>

// G4 stuff :
#include <G4UserRunAction.hh>

#include <mctools/g4/loggable_support.h>

namespace mctools {

  namespace g4 {

    class manager;

    class run_action : public G4UserRunAction,
                       public loggable_support
    {
    public:

      static const int NUMBER_EVENTS_MODULO_NONE    = 0;
      static const int NUMBER_EVENTS_MODULO_DEFAULT = 100;

      bool is_initialized () const;

      bool using_run_header_footer () const;

      void set_use_run_header_footer (bool);

      bool has_number_events_modulo () const;

      void set_number_events_modulo (int);

      int get_number_events_modulo () const;

      int32_t get_number_of_processed_events () const;

      void increment_number_of_processed_events ();

      void reset_number_of_processed_events ();

      int32_t get_number_of_saved_events () const;

      void increment_number_of_saved_events ();

      void reset_number_of_saved_events ();

      void set_output_file (const std::string & filename_);

      bool save_data () const;

      brio::writer & grab_brio_writer ();

      const brio::writer & get_brio_writer () const;

      datatools::data_writer & grab_writer ();

      const datatools::data_writer & get_writer () const;

      manager & grab_manager ();

      const manager & get_manager () const;

      run_action (manager & a_mgr);

      virtual ~run_action ();

      void initialize (const datatools::properties & config_);

      void dump (std::ostream & out_ = std::clog) const;

      // Geant4 interface:
      void BeginOfRunAction (const G4Run *);

      void EndOfRunAction (const G4Run *);

    protected:

      virtual void _build_run_header ();

      virtual void _build_run_footer ();

    private:

      bool _initialized_;
      bool _use_run_header_footer_;
      int  _number_events_modulo_;
      int32_t   _number_of_processed_events_;
      int32_t   _number_of_saved_events_;
      manager * _manager_;
      datatools::properties _run_header_;
      datatools::properties _run_footer_;

      // I/O :
      bool        _save_data_;
      bool        _output_file_preserve_;
      std::string _output_file_dir_;
      std::string _output_file_prefix_;
      std::string _output_file_format_;
      std::string _output_file_compression_;
      std::string _output_file_;
      datatools::data_writer _writer_;
      brio::writer           _brio_writer_;
      std::string _brio_general_info_store_label_;         // default "GI"
      std::string _brio_plain_simulated_data_store_label_; // default "PSD"

      // event manager...

    };

  } // end of namespace g4

} // end of namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::run_action)

#endif // MCTOOLS_G4_RUN_ACTION_H_

// end of run_action.h
