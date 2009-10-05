// mygsl::param_entry.h

#ifndef __mygsl__param_entry_h
#define __mygsl__param_entry_h 1

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <limits>

#include <mygsl/best_value.h>

using namespace std;

namespace mygsl {

  class param_entry
    {
    public:

      static const double NO_MIN_VALUE;
      static const double NO_MAX_VALUE;
      static const double NO_VALUE;
      static const double DEFAULT_STEP;
      static const double AUTO_STEP;
      static const string AUTO_LABEL;
      static const string FREE_LABEL;
      static const string CONST_LABEL;

      enum limit_t
	{
	  LIMIT_NO    = 0x0,
	  LIMIT_MIN   = 0x1,
	  LIMIT_MAX   = 0x2,
	  LIMIT_RANGE = LIMIT_MIN | LIMIT_MAX
	};

      enum type_t
	{
	  TYPE_NULL  = 0x0,
	  TYPE_FREE  = 0x1,
	  TYPE_CONST = 0x2,
	  TYPE_AUTO  = 0x4,
	  TYPE_AUTO_COMPUTED = 0x8
	};

    private:

      string __name;
      int    __type;
      int    __limit;
      double __min;
      double __max;
      double __value;
      double __step;
      string __comment;
      best_value __best_value;

    private:

      param_entry (const string & name_ = "parameter");

    public:

      bool has_step () const
	{
	  return __step == __step;
	}

      bool has_value () const
	{
	  return __value == __value;
	}

      const string & get_name () const
	{
	  return __name;
	}

      const string & get_comment () const
	{
	  return __comment;
	}

      void set_name (const string & name_)
	{
	  __name = name_;
	}

      void set_comment (const string & comment_)
	{
	  __comment = comment_;
	}

      void set_best_value (const best_value & best_value_);

      void reset_best_value ();

      bool is_auto_computed () const;

      void set_auto_computed (bool = true);

      bool has_best_value () const;

      const best_value & get_best_value () const
	{
	  return __best_value;
	}

      best_value & get_best_value ()
	{
	  return __best_value;
	}

      bool is_const () const;

      bool is_free () const;

      bool is_auto () const;

      bool has_limit () const {return __limit != LIMIT_NO; }

      bool has_no_limit () const {return __limit == LIMIT_NO; }

      bool has_min () const {return __limit & LIMIT_MIN; }

      bool has_max () const {return __limit & LIMIT_MAX; }

      //bool is_in_range (double value_) const;

      bool is_in_range () const;

      bool is_in_safe_range () const;

      //bool is_in_safe_range (double value_) const;

      bool is_in_range_but_close_to_min () const;

      bool is_in_range_but_close_to_max () const;

      double get_overflow () const;

      double get_underflow () const;

      double get_dist_to_min () const;

      double get_dist_to_max () const;

      double get_dist_to_limit () const;

      double get_sign_limit () const;

      double get_value () const;

      double get_value_safe () const;

      double get_value_safe (double & dist_) const;

      double get_step () const;

      double get_min () const;

      double get_max () const;

      void set_min_max (double min_, double max_);

      void set_min (double min_);

      void set_no_min ();

      void set_max (double max_);

      void set_no_max ();

      void set_step (double step_);

      void set_value (double value_);

      void set_value_no_check (double value_);
  
      bool check_value () const;
    
      static param_entry make_auto (const string & name_,
				    double step_ = AUTO_STEP);
      
      static param_entry make_auto_range (const string & name_,
					  double min_,
					  double max_,
					  double step_ = AUTO_STEP); 
      
      static param_entry make_auto_min (const string & name_,
					double min_,
					double step_ = AUTO_STEP); 
      
      static param_entry make_auto_max (const string & name_,
					double max_,
					double step_ = AUTO_STEP); 
      
      static param_entry make_free (const string & name_,
				    double value_,
				    double step_ = AUTO_STEP);
      
      static param_entry make_free_range (const string & name_,
					  double min_,
					  double max_,
					  double value_,
					  double step_ = AUTO_STEP);	
      
      static param_entry make_free_min (const string & name_,
					double min_,
					double value_,
					double step_ = AUTO_STEP);	
      
      static param_entry make_free_max (const string & name_,
					double max_,
					double value_,
					double step_ = AUTO_STEP);	
      
      static param_entry make_const (const string & name_,
				     double value_);
      
      void print (ostream & out_ = clog, 
		  const string & title_ = "Parameter", 
		  const string & indent_ = "") const;

      void print_status (ostream & out_ = clog) const;

    };

  // predicate:
  struct param_has_name : unary_function<bool,param_entry>
  {
    string __name;

    param_has_name (const string & name_) : __name (name_) 
    {
    }

    bool operator () (const param_entry & pe_)
    {
      return pe_.get_name () == __name;
    }
  };

  // predicate:
  struct param_ptr_has_name : unary_function<bool,param_entry *>
  {
    string __name;

    param_ptr_has_name (const string & name_) : __name (name_) 
    {
    }

    bool operator () (const param_entry * pe_)
    {
      return pe_->get_name () == __name;
    }
  };

  // predicate:
  struct param_is_free : unary_function<bool,param_entry>
  {
    param_is_free ()  
    {
    }

    bool operator () (const param_entry & pe_)
    {
      return pe_.is_free ();
    }
  };

  // predicate:
  struct param_is_const : unary_function<bool,param_entry>
  {
    param_is_const ()  
    {
    }

    bool operator () (const param_entry & pe_)
    {
      return pe_.is_const ();
    }
  };

  // predicate:
  struct param_is_auto : unary_function<bool,param_entry>
  {
    param_is_auto ()  
    {
    }

    bool operator () (const param_entry & pe_)
    {
      return pe_.is_auto ();
    }
  };
  
} // namespace mygsl

#endif // __mygsl__param_entry_h

// end of mygsl::param_entry.h
