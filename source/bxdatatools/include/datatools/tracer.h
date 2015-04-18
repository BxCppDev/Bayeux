//! \file    datatools/tracer.h
//! \brief   Utilities for tracing values in a log file
//! \details Tracing information from the progress of an application
//!          is useful to record the history of some values of interest
//!          for example while shooting random numbers and/or associated
//!          values.
//
// Copyright (c) 2013 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2013 by Université de Caen Basse-Normandie
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_TRACER_H
#define DATATOOLS_TRACER_H

// Standard Library:
#include <iostream>
#include <fstream>
#include <string>

// Third Party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This Project:
#include <datatools/utils.h>

namespace datatools {

  /// \brief A tracer object stores some arbitrary output in a trace file
  class tracer {
  public:

    tracer();

    tracer(int id_, const std::string & filename_, const std::string & label_ = "");

    ~tracer();

    void set_id(int);

    int get_id() const;

    void set_label(const std::string &);

    const std::string & get_label() const;

    void set_filename(const std::string &);

    const std::string & get_filename() const;

    void set_preserved_file(bool pf_);

    bool is_preserved_file() const;

    bool is_initialized() const;

    void initialize();

    void initialize(int id_);

    void initialize(int id_, const std::string & filename_);

    void initialize(int id_, const std::string & filename_,
		    bool preserved_file_);

    void reset();

    std::ofstream & out(bool increment_ = true);

    template<class Type>
    void trace(const Type & value_, bool increment_ = true) {
      out(increment_) << ' ' << value_;
      if (increment_) out(false) << '\n';
    }

    static datatools::tracer & grab_global_tracer(int id_,
                                                  const std::string & filename_ = "");

  private:

    int _id_;
    std::string _label_;
    std::string _filename_;
    bool _preserved_file_;
    boost::scoped_ptr<std::ofstream> _fout_;
    int _counter_;

  };

} // namespace datatools

#define DT_TRACER_INIT(TracerId,TracerFilename)                         \
  {                                                                     \
    ::datatools::tracer::grab_global_tracer(TracerId,TracerFilename);   \
  }
/**/

#define DT_TRACER_TRACE(TracerId,Value)                                 \
  {                                                                     \
    ::datatools::tracer & t = ::datatools::tracer::grab_global_tracer(TracerId); \
    if (! t.is_initialized()) t.initialize(TracerId);                   \
    t.trace(Value);                                                     \
  }
/**/

#define DT_TRACER_MESSAGE(TracerId,Message)                             \
  {                                                                     \
    ::datatools::tracer & t = ::datatools::tracer::grab_global_tracer(TracerId); \
    if (! t.is_initialized()) t.initialize(TracerId);                   \
    t.out(true) << ' ' << Message << std::endl ;                        \
  }
/**/

#define DT_TRACER_RESET(TracerId)                                       \
  {                                                                     \
    ::datatools::tracer::grab_global_tracer(TracerId).reset();          \
  }
/**/

#endif // DATATOOLS_TRACER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
