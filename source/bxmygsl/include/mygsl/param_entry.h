// -*- mode: c++; -*-
// param_entry.h

#ifndef MYGSL_PARAM_ENTRY_H_
#define MYGSL_PARAM_ENTRY_H_ 1

#include <iostream>
#include <string>

#include <mygsl/best_value.h>

namespace mygsl {

  class param_entry
  {
  public:

    static const double NO_MIN_VALUE;
    static const double NO_MAX_VALUE;
    static const double NO_VALUE;
    static const double DEFAULT_STEP;
    static const double AUTO_STEP;
    static const std::string AUTO_LABEL;
    static const std::string FREE_LABEL;
    static const std::string CONST_LABEL;

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

    bool has_step () const;

    bool has_value () const;

    const std::string & get_name () const;

    const std::string & get_comment () const;

    void set_name (const std::string & name_);

    void set_comment (const std::string & comment_);

    void set_best_value (const best_value & best_value_);

    void reset_best_value ();

    bool is_auto_computed () const;

    void set_auto_computed (bool = true);

    bool has_best_value () const;

    const best_value & get_best_value () const;

    best_value & get_best_value ();

    bool is_const () const;

    bool is_free () const;

    bool is_auto () const;

    bool has_limit () const;

    bool has_no_limit () const;

    bool has_min () const;

    bool has_max () const;

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

    static param_entry make_auto (const std::string & name_,
                                  double step_ = AUTO_STEP);

    static param_entry make_auto_range (const std::string & name_,
                                        double min_,
                                        double max_,
                                        double step_ = AUTO_STEP);

    static param_entry make_auto_min (const std::string & name_,
                                      double min_,
                                      double step_ = AUTO_STEP);

    static param_entry make_auto_max (const std::string & name_,
                                      double max_,
                                      double step_ = AUTO_STEP);

    static param_entry make_free (const std::string & name_,
                                  double value_,
                                  double step_ = AUTO_STEP);

    static param_entry make_free_range (const std::string & name_,
                                        double min_,
                                        double max_,
                                        double value_,
                                        double step_ = AUTO_STEP);

    static param_entry make_free_min (const std::string & name_,
                                      double min_,
                                      double value_,
                                      double step_ = AUTO_STEP);

    static param_entry make_free_max (const std::string & name_,
                                      double max_,
                                      double value_,
                                      double step_ = AUTO_STEP);

    static param_entry make_const (const std::string & name_,
                                   double value_);

    void print (std::ostream & out_ = std::clog,
                const std::string & title_ = "Parameter",
                const std::string & indent_ = "") const;

    void print_status (std::ostream & out_ = std::clog) const;

  private:

    param_entry (const std::string & name_ = "parameter");

  private:

    std::string _name_;
    int    _type_;
    int    _limit_;
    double _min_;
    double _max_;
    double _value_;
    double _step_;
    std::string _comment_;
    best_value _best_value_;

  };

  // predicate:
  struct param_has_name : std::unary_function<bool,param_entry>
  {
    std::string _name_;

    param_has_name (const std::string & name_) : _name_ (name_)
    {
    }

    bool operator () (const param_entry & pe_)
    {
      return pe_.get_name () == _name_;
    }
  };

  // predicate:
  struct param_ptr_has_name : std::unary_function<bool,param_entry *>
  {
    std::string _name_;

    param_ptr_has_name (const std::string & name_) : _name_ (name_)
    {
    }

    bool operator () (const param_entry * pe_)
    {
      return pe_->get_name () == _name_;
    }
  };

  // predicate:
  struct param_is_free : std::unary_function<bool,param_entry>
  {
    param_is_free ()
    {
      return;
    }

    bool operator () (const param_entry & pe_)
    {
      return pe_.is_free ();
    }
  };

  // predicate:
  struct param_is_const : std::unary_function<bool,param_entry>
  {
    param_is_const ()
    {
      return;
    }

    bool operator () (const param_entry & pe_)
    {
      return pe_.is_const ();
    }
  };

  // predicate:
  struct param_is_auto : std::unary_function<bool,param_entry>
  {
    param_is_auto ()
    {
      return;
    }

    bool operator () (const param_entry & pe_)
    {
      return pe_.is_auto ();
    }
  };

} // namespace mygsl

#endif // MYGSL_PARAM_ENTRY_H_

// end of param_entry.h
