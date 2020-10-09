/// \file mygsl/multiparameter_system.h

#ifndef MYGSL_MULTIPARAMETER_SYSTEM_H
#define MYGSL_MULTIPARAMETER_SYSTEM_H 1

// Standard library:
#include <iostream>
#include <vector>

// This project:
#include <mygsl/param_entry.h>

namespace mygsl {

  /// \brief System with several parameters
  class multiparameter_system
    {
    public:

      typedef std::vector<param_entry *> params_col_type;

      bool is_lock_params () const;

      size_t get_number_of_params () const;

      size_t get_number_of_free_params () const;

      size_t get_number_of_auto_params () const;

      size_t get_number_of_const_params () const;

      void init ();

      void reset ();

      void lock_params ();

      void unlock_params ();

      bool has_param_with_name (const std::string & name_) const;

      const param_entry & get_param_by_name (const std::string & name_) const;

      param_entry & grab_param_by_name (const std::string & name_);

      const param_entry & get_param (int i_) const;

      param_entry & grab_param (int i_);

      const param_entry & get_free_param (int i_) const;

      param_entry & grab_free_param (int i_);

      const param_entry & get_auto_param (int i_) const;

      param_entry & grab_auto_param (int i_);

      void add_param (const param_entry & pe_, const std::string & comment_ = "");

      multiparameter_system ();

      virtual ~multiparameter_system ();

      void print (std::ostream & out_ = std::clog,
		  const std::string & title_ = "Multi-parameter system",
		  const std::string & indent_ = "") const;

      void print_line (std::ostream & out_ = std::clog) const;

      void print_status (std::ostream & out_ = std::clog) const;

      void load_parameters (const std::string & filename_);

      void load_params (const std::string & filename_);

      void load_parameters (std::istream & in_ );

      void store_parameters (const std::string & filename_) const;

      void store_params (const std::string & filename_) const;

      void store_parameters (std::ostream & out_) const;

      virtual void compute_automatic_params ();

      /* Any sub-class inherited from the 'multiparameter_system'
       * class must implement the virtual 'compute_automatic_params'
       * method in order to set valid values to the parameters marked
       * as 'automatic'.
       *
       * At least these automatic parameters should invoke
       * the 'set_auto_computed (true)' or the 'set_value (XXX)' methods
       *
       * Example:
       *
       * class my_system : public multiparameter_system::compute_automatic_params ()
       * {
       * public:
       *   void multiparameter_system::compute_automatic_params ()
       *     {
       *       for (int i = 0; i < _auto_params_.size (); i++)
       *	 {
       *	   _auto_params_[i]->set_value (666.);
       *	   _auto_params_[i]->set_auto_computed (true);
       *	 }
       *     }
       * };
       *
       */

    private:

      bool            _lock_params_;
      params_col_type _params_;
      params_col_type _free_params_;
      params_col_type _auto_params_;
      params_col_type _const_params_;

    };

  struct multiparameter_system_test : public multiparameter_system
  {
    void compute_automatic_params () override
    {
      for (size_t i = 0; i < get_number_of_auto_params (); i++)
	{
	  grab_auto_param (i).set_value_no_check (666.);
	  grab_auto_param (i).set_auto_computed (true);
	}
      return;
    }
  };

} // namespace mygsl

#endif // MYGSL_MULTIPARAMETER_SYSTEM_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
