//! \file  mygsl/i_unary_function_with_parameters.h
//! \brief An abstract unary function with parameters provided by some external source
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

#ifndef MYGSL_I_UNARY_FUNCTION_WITH_PARAMETERS_H
#define MYGSL_I_UNARY_FUNCTION_WITH_PARAMETERS_H

// Standard library:
#include <string>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/types.h>

// This project:
#include <mygsl/i_unary_function.h>
#include <mygsl/parameter_store.h>

namespace mygsl {

  //! \brief Abstract interface for unary functions with derivative
  class i_unary_function_with_parameters : public i_unary_function,
                                           public parameter_store::i_subscriber
  {
  public:

    //! Default constructor
    i_unary_function_with_parameters();

    //! Constructor
    i_unary_function_with_parameters(const parameter_store & store_);

    //! Destructor
    ~i_unary_function_with_parameters() override;

    //! Check initialization status
    bool is_initialized() const override;

    //! Initialization from a container of parameters and a dictionary of functors
    void initialize(const datatools::properties & config_,
                            const unary_function_dict_type & functors_) override;

    //! Reset the functor
    void reset() override;

    //! Check if an external parameter store is set
    bool has_parameter_store() const;

    //! Set an external parameter store
    void set_parameter_store(const parameter_store & store_);

    //! Reset an external parameter store
    void reset_parameter_store();

    //! Register requested parameter
    void register_requested_parameter(const std::string & parameter_name_,
                                      datatools::basic_type parameter_type_);

    //! Return the registration index of a parameter given its name
    int get_parameter_index(const std::string & parameter_name_) const;

    //! Check if a parameter at given index is set
    bool is_parameter_set(int param_index_) const;

    //! Fetch a boolean parameter at given index
    void fetch_parameter(int param_index_, bool & value_) const;

    //! Fetch an integer parameter at given index
    void fetch_parameter(int param_index_, int & value_) const;

    //! Fetch a real parameter at given index
    void fetch_parameter(int param_index_, double & value_) const;

    //! Fetch a string parameter at given index
    void fetch_parameter(int param_index_, std::string & value_) const;

    //! Smart printing
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    //! Update parameter
    void update_parameters() override;

  protected:

    //! Evaluation
    double _eval(double x_) const override;

    //! Evaluation from parameters of the store
    virtual double _eval_from_parameters(double x_) const = 0;

    //! Action to perform when some parameters of the store have changed
    virtual void _at_parameters_change() = 0;

  private:

    const parameter_store * _params_ = nullptr; //!< Address of an external store of parameters
    std::map<std::string, datatools::basic_type> _requested_parameters_;

  };

} // end of namespace mygsl

#endif // MYGSL_I_UNARY_FUNCTION_WITH_PARAMETERS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
