// multiparameter_system.cc

#include <mygsl/multiparameter_system.h>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <limits>
#include <cmath>
#include <algorithm>

#include <datatools/exception.h>

#include <mygsl/param_entry.h>

namespace mygsl {

  using namespace std;

  void multiparameter_system::load_params (const std::string & filename_)
  {
    load_parameters (filename_);
    return;
  }

  void multiparameter_system::store_params (const std::string & filename_) const
  {
    store_parameters (filename_);
    return;
  }

  bool multiparameter_system::is_lock_params () const
  {
    return _lock_params_;
  }

  size_t multiparameter_system::get_number_of_params () const
  {
    return _params_.size ();
  }

  size_t multiparameter_system::get_number_of_free_params () const
  {
    return _free_params_.size ();
  }

  size_t multiparameter_system::get_number_of_auto_params () const
  {
    return _auto_params_.size ();
  }

  size_t multiparameter_system::get_number_of_const_params () const
  {
    return _const_params_.size ();
  }

  void multiparameter_system::init ()
  {
    lock_params ();
    for (size_t i = 0; i < _auto_params_.size (); i++)
      {
        /*
          cerr << "DEVEL: multiparameter_system::init: "
          << "check wheter automatic parameter '"
          << _auto_params_[i]->get_name ()
          << "' has been computed..." << endl;
        */
        DT_THROW_IF (! _auto_params_[i]->is_auto_computed (),
                     std::logic_error,
                     "Automatic parameter '" << _auto_params_[i]->get_name ()
                     << "' has not been computed! Please do it in the overloaded 'compute_auto_params' method !");
      }
  }

  void multiparameter_system::reset ()
  {
    bool devel = false;
    if (devel)
      {
        clog << "DEVEL: multiparameter_system::reset: "
             << "entering..."
             << endl;
      }
    if (_lock_params_)
      {
        unlock_params ();
      }
    if (devel)
      {
        clog << "DEVEL: multiparameter_system::reset: "
             << "clearing free/auto/const params arrays..."
             << endl;
      }
    _free_params_.clear ();
    _auto_params_.clear ();
    _const_params_.clear ();
    if (devel)
      {
        clog << "DEVEL: multiparameter_system::reset: "
             << "done."
             << endl;

        clog << "DEVEL: multiparameter_system::reset: "
             << "deleting params..."
             << endl;
      }
    for (size_t i = 0; i < _params_.size (); i++)
      {
        if (_params_[i] != 0)
          {
            delete _params_[i];
          }
        _params_[i] = 0;
      }
    if (devel)
      {
        clog << "DEVEL: multiparameter_system::reset: "
             << "done."
             << endl;
      }
    _params_.clear ();
  }

  void multiparameter_system::lock_params ()
  {
    if (_lock_params_)
      {
        unlock_params ();
      }
    compute_automatic_params ();
    _lock_params_ = true;
  }

  void multiparameter_system::unlock_params ()
  {
    _lock_params_ = false;
  }

  bool multiparameter_system::has_param_with_name (const string & name_) const
  {
    return find_if (_params_.begin (),
                    _params_.end (),
                    param_ptr_has_name (name_)) != _params_.end ();
  }

  const param_entry & multiparameter_system::get_param_by_name (const string & name_) const
  {
    params_col_type::const_iterator found = find_if (_params_.begin (),
                                                     _params_.end (),
                                                     param_ptr_has_name (name_));
    DT_THROW_IF (found == _params_.end (),
                 logic_error,
                 "No parameter named '" << name_ << "' !");
    return **found;
  }

  param_entry & multiparameter_system::grab_param_by_name (const string & name_)
  {
    params_col_type::iterator found = find_if (_params_.begin (),
                                               _params_.end (),
                                               param_ptr_has_name (name_));
    DT_THROW_IF (found == _params_.end (),
                 logic_error,
                 "No parameter named '" << name_ << "' !");
    return **found;
  }

  const param_entry & multiparameter_system::get_param (int i_) const
  {
    return *(_params_[i_]);
  }

  param_entry & multiparameter_system::grab_param (int i_)
  {
    return *(_params_[i_]);
  }

  const param_entry & multiparameter_system::get_free_param (int i_) const
  {
    return * (_free_params_[i_]);
  }

  param_entry & multiparameter_system::grab_free_param (int i_)
  {
    return *_free_params_.at (i_);
  }

  const param_entry & multiparameter_system::get_auto_param (int i_) const
  {
    return * (_auto_params_[i_]);
  }

  param_entry & multiparameter_system::grab_auto_param (int i_)
  {
    return *_auto_params_.at (i_);
  }

  void multiparameter_system::add_param (const param_entry & pe_, const string & comment_)
  {
    //bool devel = false;
    DT_THROW_IF (is_lock_params (),
                 std::logic_error,
                 "System is locked !");
    DT_THROW_IF ((std::find_if (_params_.begin (),
                                _params_.end (),
                                param_ptr_has_name (pe_.get_name ())) != _params_.end ()),
                 std::logic_error,
                 "Parameter '" << pe_.get_name () << "' already exist !");

    //    if (devel) pe_.print (clog, "multiparameter_system::add_param: New param", "DEVEL: ");

    param_entry * new_pe = new param_entry (pe_);
    //if (devel) new_pe->print (clog, "multiparameter_system::add_param: New entry", "DEVEL: ");
    _params_.push_back (new_pe);
    if (! comment_.empty ())new_pe->set_comment (comment_);
    params_col_type::reverse_iterator last = _params_.rbegin ();
    //clog << "DEVEL: last_pe= " << last  << endl;
    /*
    if (devel)
      {
        for (int i = 0; i < _params_.size (); i++)
          {
            _params_[i]->print (clog, "multiparameter_system::add_param: List of params", "DEVEL: ");
          }
      }
    */
    param_entry * last_pe = *last;
    /*
    if (devel)
      {
        last_pe->print (clog, "multiparameter_system::add_param: Last entry: ", "DEVEL: ");
        clog << "DEVEL: last_pe= " << last_pe << endl;
        clog << "DEVEL: parameter '" << last_pe->get_name () << "' pushed!" << endl;
        last_pe->print (clog, "multiparameter_system::add_param", "DEVEL: ");
      }
    */
    if (last_pe->is_free ())
      {
        _free_params_.push_back (last_pe);
        //  clog << "DEVEL: free '" << last_pe->get_name () << "' pushed!" << endl;
      }
    else if (last_pe->is_auto ())
      {
        _auto_params_.push_back (last_pe);
        //clog << "DEVEL: auto '" << last_pe->get_name () << "' pushed!" << endl;
      }
    else if (last_pe->is_const ())
      {
        _const_params_.push_back (last_pe);
        //clog << "DEVEL: const '" << last_pe->get_name () << "' pushed!" << endl;
      }
  }

  void multiparameter_system::compute_automatic_params ()
  {
    DT_THROW_IF (get_number_of_auto_params () > 0,
                 std::runtime_error,
                 "Not implemented! Please overload this virtual method for your own inherited class!");
  }

  // ctor:
  multiparameter_system::multiparameter_system ()
  {
    _lock_params_ = false;
  }

  // dtor:
  multiparameter_system::~multiparameter_system ()
  {
    reset ();
  }

  void multiparameter_system::print_line (std::ostream & out_) const
  {
    for (size_t i = 0; i < _params_.size (); i++)
      {
        out_ << _params_[i]->get_value () << ' ';
      }
  }

  void multiparameter_system::print (std::ostream & out_,
                                     const string & title_,
                                     const string & indent_) const
  {
    string indent = indent_;
    string tag    = "|-- ";
    string ltag   = "`-- ";
    string stag   = "|   ";
    string sltag  = "    ";
    if (! title_.empty ())
      {
        out_ << indent << title_ << ":" << endl;
      }
    out_ << indent << tag << "Locked: " << _lock_params_ << endl;
    out_ << indent << tag << "Number of parameters : " << _params_.size () << endl;
    out_ << indent << stag << tag << "Number of free parameters  : " << _free_params_.size () << endl;
    out_ << indent << stag << tag << "Number of const parameters : " << _const_params_.size () << endl;
    out_ << indent << stag << ltag << "Number of auto parameters  : " << _auto_params_.size () << endl;
    out_ << indent << ltag << "Parameters: " << endl;
    for (size_t i = 0; i < _params_.size (); i++)
      {
        string atag = tag;
        string atag2 = stag;
        if (i == (_params_.size () - 1))
          {
            atag  = ltag;
            atag2 = sltag;
          }
        ostringstream title_ss, indent_ss;
        title_ss << atag << "Parameter '" << _params_[i]->get_name () << "'";
        indent_ss << indent << sltag << atag2; // << sltag << atag2;
        out_ << indent << sltag << title_ss.str () << ":" << endl;
        _params_[i]->print (out_, "", indent_ss.str ());
      }
  }

  void multiparameter_system::print_status (ostream & out_) const
  {
    out_ << "Multi-parameter system status: " << endl;
    for (size_t i = 0; i < get_number_of_params (); i++)
      {
        get_param (i).print_status (out_);
      }
    out_ << endl;
  }

  void multiparameter_system::load_parameters (const std::string & filename_)
  {
    std::ifstream f (filename_.c_str ());
    DT_THROW_IF (! f, std::runtime_error,
                 "Cannot open file '" << filename_ << "' !");
    load_parameters (f);
    f.close ();
  }

  void multiparameter_system::load_parameters (std::istream & in_ )
  {
    bool local_debug = false;
    if (local_debug) clog << "DEVEL: multiparameter_system::load_parameters: entering..." << endl;
    this->unlock_params ();
    while (in_)
      {
        string line;
        getline (in_, line);
        if (in_.eof ())
          {
            clog << "NOTICE: multiparameter_system::load_parameters: "
                 << "End of stream has been reached." << endl;
            break;
          }
        {
          istringstream line_iss (line);
          string word;
          line_iss >> word;
          if (word.empty ())
            {
              if (local_debug) clog << "DEVEL: multiparameter_system::load_parameters: skipping blank line..." << endl;
              continue;
            }
          if (word[0] == '#')
            {
              if (local_debug) clog << "DEVEL: multiparameter_system::load_parameters: skipping commented line..." << endl;
              continue;
            }
        }
        if (local_debug) clog << "DEVEL: multiparameter_system::load_parameters: parsing line '" << line << "'" << endl;


        istringstream line_iss (line);
        string name, type, comment;
        double min, max, value, step;

        string token;
        getline (line_iss, token, ':');
        if (line_iss)
          {
            istringstream token_iss (token);
            token_iss >> name;
            DT_THROW_IF (! token_iss,
                         runtime_error,
                         "Invalid format!");
          }

        getline (line_iss, token, ':');
        if (line_iss)
          {
            istringstream token_iss (token);
            token_iss >> type;
            DT_THROW_IF (! token_iss, runtime_error, "Invalid format!");
            DT_THROW_IF ((type != param_entry::FREE_LABEL)
                         && (type != param_entry::AUTO_LABEL)
                         && (type != param_entry::CONST_LABEL),
                         runtime_error, "Invalid type!");
          }

        getline (line_iss, token, ':');
        if (line_iss)
          {
            istringstream token_iss (token);
            token_iss >> min;
            if (! token_iss)
              {
                min = param_entry::NO_MIN_VALUE;
              }
          }

        getline (line_iss, token, ':');
        if (line_iss)
          {
            istringstream token_iss (token);
            token_iss >> max;
            if (! token_iss)
              {
                max = param_entry::NO_MAX_VALUE;
              }
          }

        getline (line_iss, token, ':');
        if (line_iss)
          {
            istringstream token_iss (token);
            token_iss >> value;
            if (! token_iss)
              {
                value = param_entry::NO_VALUE;
              }
          }

        getline (line_iss, token, ':');
        if (line_iss)
          {
            istringstream token_iss (token);
            token_iss >> step;
            if (! token_iss)
              {
                step = param_entry::AUTO_STEP;
              }
          }

        getline (line_iss, comment);
        if (! line_iss)
          {
            comment = "";
          }

        if (local_debug)
          {
            clog << "DEVEL: multiparameter_system::load_parameters: " << endl;
            clog << "|-- name:    " << name    << endl;
            clog << "|-- type:    " << type    << endl;
            clog << "|-- min:     " << min     << endl;
            clog << "|-- max:     " << max     << endl;
            clog << "|-- value:   " << value   << endl;
            clog << "|-- step:    " << step    << endl;
            clog << "`-- comment: " << comment << endl;
          }

        typedef mygsl::param_entry pe;

        if (type == param_entry::CONST_LABEL)
          {
            add_param (pe::make_const (name,
                                       value),
                       comment);
          }

        if (type == param_entry::FREE_LABEL)
          {
            if (isfinite (min) && isfinite (max))
              {
                add_param (pe::make_free_range (name,
                                                min,
                                                max,
                                                value,
                                                step),
                           comment);

              }
            else if (isfinite (min) && ! isfinite (max))
              {
                add_param (pe::make_free_min (name,
                                              min,
                                              value,
                                              step),
                           comment);

              }
            else if (! isfinite (min) && isfinite (max))
              {
                add_param (pe::make_free_min (name,
                                              max,
                                              value,
                                              step),
                           comment);

              }
            else
              {
                add_param (pe::make_free (name,
                                          value,
                                          step),
                           comment);
              }
          }

        if (type == param_entry::AUTO_LABEL)
          {
            if (isfinite (min) && isfinite (max))
              {
                add_param (pe::make_auto_range (name,
                                                min,
                                                max,
                                                step),
                           comment);

              }
            else if (isfinite (min) && ! isfinite (max))
              {
                add_param (pe::make_auto_min (name,
                                              min,
                                              step),
                           comment);

              }
            else if (! isfinite (min) && isfinite (max))
              {
                add_param (pe::make_auto_min (name,
                                              max,
                                              step),
                           comment);

              }
            else
              {
                add_param (pe::make_auto (name,
                                          step),
                           comment);
              }
          }
      }
    this->lock_params ();
    if (local_debug) clog << "DEVEL: multiparameter_system::load_parameters: exiting." << endl;
  }

  void multiparameter_system::store_parameters (const std::string & filename_) const
  {
    std::ofstream f (filename_.c_str ());
    DT_THROW_IF (! f,std::runtime_error,"Cannot open file '" << filename_ << "'!");
    f << "# mygsl::multiparameter_system: list of parameters: " << endl;
    f.width (12);
    f.setf (ios::left);
    f << "# Name";

    f << " : " ;

    f.width (6);
    f << "Type";

    f << " : " ;

    f.width (10);
    f << "Minimum";

    f << " : " ;

    f.width (10);
    f << "Maximum";

    f << " : " ;

    f.width (10);
    f << "Value";

    f << " : " ;

    f.width (10);
    f << "Step";

    f << " : " ;

    f.width (10);
    f << "Comment";

    f << endl;
    store_parameters (f);
    f << "# mygsl::multiparameter_system: end of list." << endl;
    f.close ();
  }

  void multiparameter_system::store_parameters (std::ostream & out_ ) const
  {
    for (size_t i = 0; i < get_number_of_params (); i++)
      {
        // print name:
        out_.width (12);
        out_.setf (ios::left);
        out_ << get_param (i).get_name ();

        out_ << " : " ;

        // print type (free/auto/const):
        out_.width (6);
        out_ <<  (get_param (i).is_free () ? param_entry::FREE_LABEL : (get_param (i).is_const () ? param_entry::CONST_LABEL : param_entry::AUTO_LABEL));

        out_ << " : " ;

        // print minimum value:
        out_.width (10);
        if (get_param (i).has_min ())
          {
            out_ << get_param (i).get_min ();
          }
        else
          {
            out_ << ' ';
          }

        out_ << " : ";

        // print maximum value:
        out_.width (10);
        if (get_param (i).has_max ())
          {
            out_ << get_param (i).get_max ();
          }
        else
          {
            out_ << ' ';
          }

        out_ << " : ";

        out_.width (10);
        // print effective value:
        if (get_param (i).has_value ())
          {
            out_ << get_param (i).get_value ();
          }
        else
          {
            out_ << ' ';
          }

        out_ << " : ";

        // print step size:
        out_.width (10);
        if (get_param (i).has_step ())
          {
            out_ << get_param (i).get_step ();
          }
        else
          {
            out_ << ' ';
          }

        out_ << " : ";

        // print comment:
        out_ << get_param (i).get_comment ();

        out_ << std::endl;
      }
  }

}

// end of multiparameter_system.cc
