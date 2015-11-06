//! \file  datatools/introspection/method.h
//! \brief Description of a method or function
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Datatools.
//
// Datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Datatools. If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_INTROSPECTION_METHOD_H
#define DATATOOLS_INTROSPECTION_METHOD_H

// Standard library:
#include <string>
#include <iostream>
#include <vector>

// Third party:
// - Boost:
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/program_options.hpp>
// - Bayeux/datatools:
#include <datatools/enriched_base.h>

// This project:
#include <datatools/introspection/argument.h>

namespace datatools {

  namespace introspection {

    //! \brief Description of a method
    class method : public datatools::enriched_base
    {
    public:

      //! Default constructor
      method();

      //! Destructor
      virtual ~method();

      //! Check if constness is set
      bool has_constness() const;

      //! Check if the method is const
      bool is_constness() const;

      //! Set the method constness
      void set_constness(bool);

      //! Reset the method constness
      void reset_constness();

      //! Check if type identifier is set
      bool has_type_id() const;

      //! Set the type identifier
      void set_type_id(const std::string &);

      //! Reset the type identifier
      void reset_type_id();

      //! Return the type identifier
      const std::string & get_type_id() const;

      //! Check if the method has arguments
      bool has_arguments() const;

      //! Add an argument
      void add_argument(const argument & arg_, const std::string & arg_name_ = "");

      //! Check if an argument at given rank exists
      bool has_argument_by_rank(std::size_t rank_) const;

      //! Check if an argument at given name exists
      bool has_argument_by_name(const std::string & name_) const;

      //! Return argument by name
      const argument & get_argument_by_name(const std::string & name_) const;

      //! Return argument by rank
      const argument & get_argument_by_rank(std::size_t rank_) const;

      //! Build the list of input arguments' names
      void build_list_of_input_argument_names(std::vector<std::string> &) const;

      //! Build the list of output arguments' names
      void build_list_of_output_argument_names(std::vector<std::string> &) const;

      //! Check validity
      bool is_valid() const;

      //! Initialization
      void initialize();

      //! Initialization
      void initialize(const datatools::properties &);

      //! Reset
      void reset();

      //! Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

      //! Make a setter method with one unique input scalar argument
      void make_scalar_setter(const std::string & arg_name_,
                              data_type           arg_type_,
                              const std::string & arg_info_ = "",
                              const std::string & arg_info2_ = "",
                              const std::string & method_desc_ = "",
                              const std::string & method_name_ = "set");

      //! Make a getter method with one unique output scalar argument
      void make_scalar_getter(const std::string & arg_name_,
                              data_type           arg_type_,
                              const std::string & arg_info_ = "",
                              const std::string & arg_info2_ = "",
                              const std::string & method_desc_ = "",
                              const std::string & method_name_ = "get");

    private:

      boost::optional<bool> _constness_; //!< Constness flag
      boost::optional<std::string> _type_id_; //!< Type/class identifier

      // Working data:
      // PIMPL-ized data structure:
      struct pimpl_type;
      boost::scoped_ptr<pimpl_type> _priv_; //!< Pimplized internal data

    };

  } // namespace introspection

} // namespace datatools

#endif // DATATOOLS_INTROSPECTION_METHOD_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
