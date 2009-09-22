// gsl::rng.h

#ifndef __gsl__rng_h
#define __gsl__rng_h 1

#include <gsl/gsl_rng.h>
#include <string>
#include <map>
#include <gsl/gsl_randist.h>

namespace mygsl {

  class rng
    {
    public:
      
      static bool g_debug;

      struct initializer
      {
	std::map<std::string, const gsl_rng_type *> dict;
	initializer ();
	~initializer ();
      };

    private:

      static initializer g__initializer;

      gsl_rng * __r;

    public:

      static void default_setup ();

      static void print_dict (std::ostream &);

    public:

      void init (const std::string & id_ = "mt19937", 
		 unsigned long int seed_ = 0);

      //rng ();
      
      //rng (const char * id_, unsigned long int seed_ = 0);
      
      rng (const std::string & id_ = "mt19937", unsigned long int seed_ = 0);

      void reset ();
      
      virtual ~rng ();

      rng (const rng &); // not implemented

      rng & operator= (const rng &); // not implemented
      
      unsigned long int get ();
      
      double uniform ();
      
      double uniform_pos ();
      
      unsigned long int uniform_int (unsigned long int n_);
      
      std::string name () const;
      
      unsigned long int min () const;
      
      unsigned long int max () const;
 
      void store (const std::string & filename_) const;

      void load (const std::string & filename_);

      void to_stream (std::ostream &) const;

      void from_stream (std::istream &);
   
      // specific distributions:
 
      double flat (double a_, double b_) const;
      
      double gaussian (double sigma_ = 1.0) const;
      
      double gaussian (double mu_, double sigma_) const;

      double gaussian_tail (double min_, double sigma_ = 1.0) const;

      double exponential (double sigma_ = 1.0) const;

      double chisquare (double nu_ = 1.0) const;

      unsigned long int poisson (double mu_) const;

      unsigned long int bernoulli (double p_ = 0.5) const;

      unsigned long int binomial (double p_, 
				  unsigned long int n_) const;

      gsl_rng * operator () (void);

      const gsl_rng * operator () (void) const;

    };
  
}

#endif // __gsl__rng_h

// end of gsl::rng.h
