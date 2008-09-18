// gsl::histogram.h

#ifndef __gsl__histogram_h
#define __gsl__histogram_h 1

#include <gsl/gsl_histogram.h>
#include <gsl/gsl_vector.h>

#include <iostream>
#include <vector>

namespace mygsl {

  class histogram
    {
    public:

      enum copy_mode_t
	{
	  RESET = -1,
	  COPY  = 0
	};
      
      static bool g_debug;

    private:
      double          __bin_spacing;
      double          __underflow;
      double          __overflow;
      gsl_histogram * __h;

    public:

      bool is_uniform_bin_spacing () const;

      double get_uniform_bin_spacing () const;

      void init (size_t n_ = 10 , double min_ = 0.0 , double max_ = 1.0);

      void init (const std::vector<double> & ranges_);

      histogram (size_t n_ = 10 , 
		 double min_ = 0.0 , double max_ = 1.0);
      
      histogram (const std::vector<double> & ranges_);
      
      virtual ~histogram ();

      histogram (const histogram & , int mode_ = COPY);

      histogram & operator= (const histogram &);

      void accumulate (double x_ , double weight_ = 1.0);

      void increment (double x_);

      void fill (double x_ , double weight_ = 1.0);

      double underflow () const;

      double overflow () const;

      double min () const;

      double max () const;

      size_t bins () const;

      double get (size_t i_) const;

      double at (size_t i_) const;
      
      double operator[] (size_t i_) const;

      bool find (double x_ , size_t & i_) const; 

      void reset ();

      bool can_rebin (size_t new_bins_) const;

      void rebin (size_t new_bins_);

      double min_val () const;

      double max_val () const;

      size_t min_bin () const;

      size_t max_bin () const;

      double mean () const;

      double sigma () const;

      double sum () const;

      void to_stream (std::ostream &) const;

      void from_stream (std::istream &);

      void print (std::ostream & , int precision_ = 9) const;
 
      void dump (std::ostream & , int precision_ = 9) const;

      bool has_size (size_t bins_) const;

      std::pair<double,double> get_range (size_t i_) const;

      void shift (double s_);

      void scale (double s_);

      void negate ();

      void zero ();

      bool same (const histogram &) const;

      void add (const histogram &);

      void sub (const histogram &);

      void mul (const histogram &);

      void div (const histogram &);

      histogram & operator+= (const histogram &);

      histogram & operator-= (const histogram &);

      histogram & operator*= (const histogram &);

      histogram & operator/= (const histogram &);

      histogram & operator+= (double);

      histogram & operator-= (double);

      histogram & operator- ();

      histogram & operator*= (double);

      histogram & operator/= (double);

      friend histogram operator+ (const histogram & , 
				  const histogram &);
      
      friend histogram operator- (const histogram & , 
				  const histogram &);
      
      friend histogram operator* (const histogram & , 
				  const histogram &);
      
      friend histogram operator/ (const histogram & , 
				  const histogram &);

      friend histogram operator* (double , 
				  const histogram &);
      
      friend histogram operator/ (double , 
				  const histogram &);

      friend histogram operator* (const histogram & , 
				  double);
      
      friend histogram operator/ (const histogram & , 
				  double);

      class pdf
	{
	  gsl_histogram_pdf * __pdf;

	public:

	  pdf (size_t n_ = 10);

	  pdf (const histogram & h_);

	  virtual ~pdf ();

	  void init (const histogram & h_);

	  void reset ();

	  double sample (double r_);

	  template<class _ran_functor_t>
	    double sample (_ran_functor_t & ran_)
	    {
	      return  gsl_histogram_pdf_sample (__pdf, ran_());
	    }
	  
	};

    };

  
}

#endif // __gsl__histogram_h

// end of gsl::histogram.h
