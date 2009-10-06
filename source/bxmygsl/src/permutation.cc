// mygsl::permutation.cc

#include <mygsl/permutation.h>

namespace mygsl {

  bool permutation::__check () const
  {
    if (__perm  == 0) return false;
    if (size () == 0) return false;
    return true;
  }

  void permutation::__check_throw (size_t i_, const string & where_) const
  {
    if (! __check ())
      {
	ostringstream message;
	message << "permutation::" << where_ << ": "
		<< "Permutation is not initialized!";
	throw runtime_error (message.str ());
      }
    if (i_ >= size ())
      {
	ostringstream message;
	message << "permutation::" << where_ << ": "
		<< "Invalid permutation index " << i_ 
		<< "!";
	throw runtime_error (message.str ());
      }
  }

  bool permutation::is_initialized () const
  {
    return size () > 0;
  }

  const size_t * permutation::data () const
  {
    if (size () == 0) return 0;
    return gsl_permutation_data (__perm);
  }

  size_t permutation::inversions () const
  {
    __check_throw (0, "inversions");
    return gsl_permutation_inversions (__perm);
  }

  size_t permutation::linear_cycles () const
  {
    __check_throw (0, "linear_cycles");
    return gsl_permutation_linear_cycles (__perm);
  }

  size_t permutation::canonical_cycles () const
  {
    __check_throw (0, "canonical_cycles");
    return gsl_permutation_canonical_cycles (__perm);
  }

  size_t permutation::get (size_t i_) const
  {
    __check_throw (i_, "get");
    return gsl_permutation_get (__perm, i_);
  }
  
  void permutation::swap (size_t i_, size_t j_)
  {
    __check_throw (i_, "swap");
    __check_throw (j_, "swap");
    if (i_ == j_) return;
    gsl_permutation_swap (__perm, i_, j_);
  }

  bool permutation::is_valid () const
  {
    if (! __check ()) return false;
    return gsl_permutation_valid (__perm);
  }

  size_t permutation::size () const
  {
    if (__perm != 0)
      {
	return gsl_permutation_size (__perm);
      }
    return 0;
  }

  // ctor:
  permutation::permutation ()
  {
    __perm = 0;
  }

  // ctor:
  permutation::permutation (size_t sz_)
  {
    __perm = 0;
    init (sz_);
  }

  // ctor:
  permutation::permutation (const permutation & p_)
  {
    __perm = 0;
    size_t sz = p_.size ();
    if (sz > 0)
      {
	__perm = gsl_permutation_calloc (sz);
      }
  }

  // dtor:
  permutation::~permutation ()
  {
    reset ();
  }

  permutation & permutation::operator= (const permutation & p_)
  {
    if (this == &p_) return *this;
    size_t sz = p_.size ();
    if (size () != sz)
      {
	reset ();
      }
    if (sz > 0)
      {
	gsl_permutation_memcpy (__perm, p_.__perm);
      }
 
    return *this;
  }

  void permutation::reset ()
  {
    if (__perm != 0)
      {
	gsl_permutation_free (__perm);
	__perm = 0;
      }
  }

  void permutation::init (size_t sz_)
  {
    if (__perm != 0)
      {
	reset ();
      }
    if (sz_ > 0)
      {
	__perm = gsl_permutation_calloc (sz_);
      }
    if (__perm == 0) 
      {
	ostringstream message;
	message << "permutation::init: "
		<< "Cannot allocate permutation with size " 
		<< sz_ << "!";
	throw runtime_error (message.str ());
      }
  }

  void permutation::reverse ()
  {
    __check_throw (0, "reverse");
    gsl_permutation_reverse (__perm);
  }

  bool permutation::inverse (permutation & p_)
  {
    __check_throw (0, "inverse");
    p_.init (this->size ());
    return gsl_permutation_inverse (p_.__perm, this->__perm) == GSL_SUCCESS;
  }

  bool permutation::canonical_to_linear (permutation & p_)
  {
    __check_throw (0, "canonical_to_linear");
    p_.init (this->size ());
    return gsl_permutation_canonical_to_linear (p_.__perm, this->__perm) == GSL_SUCCESS;
  }

  bool permutation::linear_to_canonical (permutation & p_)
  {
    __check_throw (0, "linear_to_canonical");
    p_.init (this->size ());
    return gsl_permutation_linear_to_canonical (p_.__perm, this->__perm) == GSL_SUCCESS;
  }

  bool permutation::next ()
  {
    __check_throw (0, "next");
    return gsl_permutation_next (__perm) == GSL_SUCCESS;
  }

  bool permutation::previous ()
  {
    __check_throw (0, "previous");
    return gsl_permutation_prev (__perm) == GSL_SUCCESS;
  }

  permutation & permutation::operator++ ()
  {
    this->next ();
    return *this;
  }

  permutation & permutation::operator-- ()
  {
    this->previous ();
    return *this;
  }

  void permutation::print (ostream & out_, bool eol_) const
  {
    for (int i = 0; i < size (); i++)
      {
	if (i != 0)
	  {
	    out_ << ' ';
	  }
	out_ << dec << get (i);
      }
   if (eol_) out_ << endl;
  }
  
  ostream & operator<< (ostream & out_, const permutation & p_)
  {
    out_ << '(';
    for (int i = 0; i < p_.size (); i++)
      {
	if (i != 0)
	  {
	    out_ << ", ";
	  }
	out_ << p_.get (i);
      }
    out_ << ')';
    return out_;
  }

  istream & operator>> (istream & in_, permutation & p_)
  {
    p_.reset ();
    char c = 0;
    in_.get (c);
    size_t count = 0;
    list<size_t> values;
    if (! in_)
      {
	in_.putback (c);
	return in_;
      }
    if (c != '(')
      {
	in_.putback (c);
	in_.setstate (ios::failbit);
	return in_;
      }
    c = 0;
    in_.get (c);
    if (! in_)
      {
	in_.putback (c);
	return in_;
      }
    if (c != ')')
      {
	in_.putback (c);
      }
    else
      {
	return in_;
      }
    while (true)
      {
	size_t val;
	in_ >> val;
	if (! in_)
	  {
	    return in_;
	  }
	count++;
	values.push_back (val);
	c = 0;
	in_.get (c);
	if (! in_)
	  {
	    in_.putback (c);
	    return in_;
	  }
	if ((c != ',' ) && (c != ')'))
	  {
	    in_.putback (c);
	    in_.setstate (ios::failbit);
	    return in_;	    
	  }
	if (c == ')')
	  {
	    break;
	  }
      }
    if (count > 0)
      {
	p_.init (count);
	for (int i = 0; i < count; i++)
	  {
	    p_.__perm->data[i] = values.front ();
	    values.pop_front ();
	  }
      }
    return in_;
  }

}

// end of mygsl::best_value.cc

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
