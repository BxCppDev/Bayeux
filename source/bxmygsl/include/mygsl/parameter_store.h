//! \file  mygsl/parameter_store.h
//! \brief A store for parameters
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

#ifndef MYGSL_PARAMETER_STORE_H
#define MYGSL_PARAMETER_STORE_H

// Standard library:
#include <string>
#include <map>
#include <vector>
#include <list>

// Third party:
// - Boost :
#include <boost/variant.hpp>
// - Bayeux/datatools :
#include <datatools/i_tree_dump.h>
#include <datatools/types.h>

namespace mygsl {

  //! \brief Store of parameters
  class parameter_store : public datatools::i_tree_dumpable
  {
  public:

    static const int INVALID_INDEX = -1;

    //! Type alias for parameter value
    typedef boost::variant<bool, int, double, std::string> param_type;

    //! \brief Parameter record
    struct parameter_record {
    public:

      //! Default constructor
      parameter_record();

      //! Constructor
      parameter_record(parameter_store &);

      //! Set parent store
      void set_parent(parameter_store &);

      //! Set the parameter index
      void set_index(int index_);

      //! Return the parameter index
      int get_index() const;

      //! Set the parameter name
      void set_name(const std::string & name_);

      //! Return the parameter name
      const std::string & get_name() const;

      //! Set the parameter type
      void set_type(datatools::basic_type);

      //! Return the parameter type
      datatools::basic_type get_type() const;

      //! Check if the parameter is boolean
      bool is_boolean() const;

      //! Check if the parameter is integer
      bool is_integer() const;

      //! Check if the parameter is real
      bool is_real() const;

      //! Check if the parameter is string
      bool is_string() const;

      //! Set the parameter description
      void set_description(const std::string & description_);

      //! Return the parameter description
      const std::string & get_description() const;

      //! Reset
      void reset();

      //! Value is set
      bool is_value_set() const;

      void set_boolean_value(bool, bool update_ = true);

      void set_integer_value(int, bool update_ = true);

      void set_real_value(double, bool update_ = true);

      void set_string_value(const std::string &, bool update_ = true);

      void unset_value(bool update_ = true);

      bool to_boolean() const;

      int to_integer() const;

      double to_real() const;

      const std::string & to_string() const;

    private:

      parameter_store * _parent_; //!< Parent store
      std::string _name_;        //!< Name of the parameter
      datatools::basic_type _type_; //!< Type of the parameter
      std::string _description_; //!< Description of the parameter
      int         _index_;       //!< Index of the parameter
      bool        _value_set_;   //!< Flag to check if the value is set
      param_type  _value_;       //!< Current value

      friend class parameter_store;

    };

    //! \brief Subscriber
    struct i_subscriber {
      /// Action performed when parameters have changed
      virtual void update_parameters() = 0;
    };

    //! Type alias for a dictionary of parameter records
    typedef std::map<std::string, parameter_record> parameter_dict_type;

    //! Type alias for an array of parameter record handles
    typedef std::vector<parameter_record *> indexed_parameter_col_type;

    //! Default constructor
    parameter_store();

    //! Destructor
    ~parameter_store() override;

    //! Reset the store
    void reset();

    //! Add a parameter
    int add(const std::string & name_,
            datatools::basic_type type_,
            const std::string & description_);

    //! Remove parameter
    void remove(const std::string & name_);

    //! Check if a parameter exists
    bool has(const std::string & name_) const;

    //! Return a non mutable parameter record by name
    const parameter_record & get(const std::string & name_) const;

    //! Return a mutable parameter record by name
    parameter_record & grab(const std::string & name_);

    //! Check if a parameter idnex is valid
    bool index_is_valid(int index_) const;

    //! Return a non mutable parameter record by index
    const parameter_record & get(int index_, bool do_check_ = true) const;

    //! Return a mutable parameter record by index
    parameter_record & grab(int index_, bool do_check_ = true);

    //! Return the index associated to a parameter given its name
    int get_parameter_index(const std::string & parameter_name_) const;

    //! Update
    void update();

    //! Main interface method for smart dump
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    //! Check is a subscriber object is currently a subscriber of the store
    bool is_registered_subscriber(const i_subscriber & s_) const;

    //! Register a subscriber
    void register_subscriber(i_subscriber &);

    //! Unregister a subscriber
    void unregister_subscriber(i_subscriber &);

  protected:

    //! Update parameters action
    void _update_parameters();

  private:

    parameter_dict_type        _parameters_;         //!< Dictionary of parameters
    indexed_parameter_col_type _indexed_parameters_; //!< Array of indexed parameters
    std::list<i_subscriber*>   _subscribers_;        //! List of subscribers

  };

} // end of namespace mygsl

#endif // MYGSL_PARAMETER_STORE_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
