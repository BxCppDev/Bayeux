// mygsl::rng.c

#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string.h>

#include <mygsl/rng.h>

namespace mygsl {

  /****************************************************/


  bool rng::g_debug = false;

  void rng::default_setup()
    {
      gsl_rng_env_setup();
    }
 
  rng::initializer::initializer()
    {
      if ( g_debug ) {
	std::cerr << "DEBUG: rng::initializer::ctor: " 
		  << "initialize dictionnary." << std::endl;
      }
      const gsl_rng_type ** t0 = gsl_rng_types_setup();
      for ( const gsl_rng_type ** t=t0; *t!=0; t++ ) {
	std::string id((*t)->name);
	dict[id]=*t;
      }
    }

  rng::initializer::~initializer()
    {
      dict.clear();
    }

  void rng::print_dict( std::ostream & out_ )
    {
      if ( g_debug ) {
	std::cerr << "Available GSL random number generators: " 
		  << std::endl;
      }
      std::map<std::string,const gsl_rng_type *>::const_iterator i;
      for ( i = rng::g__initializer.dict.begin(); 
	    i != rng::g__initializer.dict.end(); 
	    i++ ) {
	out_ << i->first << std::endl; 
      } 
    }

  rng::initializer rng::g__initializer;

  /****************************************************/

  void rng::init( const std::string & id_ , 
		  unsigned long int seed_ )
    {
      if ( __r != 0 ) {
	reset();
      }
      if ( g__initializer.dict.find(id_) == g__initializer.dict.end() ) {
	std::ostringstream message;
	message << "gsl::rng::init: Cannot find the '" 
		<< id_ << "' generator!";
	throw std::runtime_error(message.str());
      }
      __r = gsl_rng_alloc(g__initializer.dict[id_]);
      if ( __r == 0 ) {
	std::ostringstream message;
	message << "gsl::rng::init: Cannot allocate the '" 
		<< id_ << "' generator!";
	throw std::runtime_error(message.str());
      }
      gsl_rng_set(__r,seed_);
    }

  /*
  rng::rng()
    {
      __r = 0;
      init(gsl_rng_default->name,gsl_rng_default_seed);
    }
  */

  /*
  rng::rng( const char * id_ )
    {
      __r = 0;
      init(id_);
    }
  */

  rng::rng( const std::string & id_ , unsigned long int seed_ )
    {
      __r = 0;
      init(id_,seed_);
    }

  void rng::reset()
    {
      if ( g_debug ) {
	std::cerr << "DEBUG: rng::reset: " 
		  << "Entering." << std::endl;
      }
      if ( __r != 0 ) gsl_rng_free(__r);
      __r = 0;
    }

  rng::~rng()
    {
      reset();
    }

  unsigned long int rng::get()
    {
      return gsl_rng_get(__r);
    }

  double rng::uniform()
    {
      return gsl_rng_uniform(__r);
    }

  double rng::uniform_pos()
    {
      return gsl_rng_uniform_pos(__r);
    }

  unsigned long int rng::uniform_int( unsigned long int n_ )
    {
      return gsl_rng_uniform_int(__r,n_);
    }

  std::string rng::name() const
    {
      return (std::string(gsl_rng_name(__r)));
    }

  unsigned long int rng::min() const
    {
      return (gsl_rng_min(__r));
    }

  unsigned long int rng::max() const
    {
      return (gsl_rng_max(__r));
    }

  void rng::store( const std::string & filename_ ) const
    {
      FILE * stream = fopen(filename_.c_str(),"w");
      if ( stream == NULL ) {
	int errid=errno;
	std::ostringstream message;
	message << "gsm::rng::store: Cannot open file '"
		<< filename_ << "'!";
	throw std::runtime_error(message.str());
      }
      int ret=gsl_rng_fwrite(stream,__r);
      if ( ret == GSL_EFAILED) {
	std::ostringstream message;
	message << "gsm::rng::store: Cannot store state in file '"
		<< filename_ << "'!";
	throw std::runtime_error(message.str());
      }
      fclose(stream);
    }

  void rng::load( const std::string & filename_ )
    {
      FILE * stream = fopen(filename_.c_str(),"r");
      if ( stream == NULL ) {
	int errid=errno;
	std::ostringstream message;
	message << "gsm::rng::load: Cannot open file '"
		<< filename_ << "'!";
	throw std::runtime_error(message.str());
      }
      int ret=gsl_rng_fread(stream,__r);
      if ( ret == GSL_EFAILED) {
	std::ostringstream message;
	message << "gsm::rng::load: Cannot load state from file '"
		<< filename_ << "'!";
	throw std::runtime_error(message.str());
      }
      ret = fclose(stream);
      if ( ret != 0 ) {
	int errid=errno;
	std::ostringstream message;
	message << "gsm::rng::load: Cannot close file '"
		<< filename_ << "'!";
	throw std::runtime_error(message.str());
      }
    }

  void rng::to_stream( std::ostream & out_ ) const
    {
      void * state = gsl_rng_state(__r);
      size_t n = gsl_rng_size(__r);
      const unsigned char * b=(const unsigned char * ) state;
      out_ << this->name() << ' ' << n;
      for ( int i=0; i<n; i++ ) {
	unsigned int c = (unsigned int) *b;
	out_ << ' ' << std::dec << c;
	b++;
      }
    }

  void rng::from_stream( std::istream & in_ ) {
    std::string token;
    in_ >> token;
    if ( ! in_ ) {
      throw std::runtime_error("gsl::rng::from_stream: Cannot read generator name from stream!");      
    }
    if ( g_debug ) {
      std::cerr << "DEBUG: rng::from_stream: name='" 
		<< token << "'" << std::endl;
    }
    init(token);
    int n;
    in_ >> n;
    if ( ! in_ ) {
      throw std::runtime_error("gsl::rng::from_stream: Cannot read generator size from stream!");      
    }
    if ( n != gsl_rng_size(__r) ) {
      throw std::runtime_error("gsl::rng::from_stream: Invalid  generator size!");      
    }
    if ( g_debug ) {
      std::cerr << "DEBUG: rng::from_stream: n='" 
	      << n << "'" << std::endl;
    }
    unsigned char * s=0;
    s = new unsigned char[n];
    unsigned char * ps=s;
    for ( int i=0; i < n ; i++ ) {
      unsigned int c;
      in_ >> c ;
      if ( ! in_ ) {
	throw std::runtime_error(
          "gsl::rng::from_stream: Cannot read state byte from stream!");      
      }
      /*
      if ( g_debug ) {
	std::cerr << "DEBUG: rng::from_stream: i=" 
		  << i << "   c='" 
		  << c << "'" << std::endl;
      }
      */
      *ps = (unsigned char) c;
      ps++;
      in_ >> std::ws;
    }
    void * state = gsl_rng_state(__r);
    memcpy(state,s,n);
    delete[] s;
  }
  
  // specific distributions:
 
  double rng::flat( double min_ , double max_ ) const
    {
      return gsl_ran_flat(__r,min_,max_);
    }
  
  double rng::gaussian( double sigma_ ) const
    {
      return gsl_ran_gaussian_ziggurat(__r,sigma_);
    }
  
  double rng::gaussian( double mu_ , double sigma_ ) const
    {
      return mu_+gaussian(sigma_);
    }

  double rng::gaussian_tail( double min_ , double sigma_ ) const
    {
      return gsl_ran_gaussian_tail(__r,min_,sigma_);
    }

  double rng::exponential( double sigma_ ) const
    {
      return gsl_ran_exponential(__r,sigma_);
    }

  double rng::chisquare( double nu_ ) const
    {
      return gsl_ran_chisq(__r,nu_);
    }

  unsigned long int rng::poisson( double mu_ ) const
    {
      return gsl_ran_poisson(__r,mu_);
    }

  unsigned long int rng::bernoulli( double p_ ) const
    {
      return gsl_ran_bernoulli(__r,p_);
    }

  unsigned long int rng::binomial( double p_ , 
				   unsigned long int n_ ) const
    {
      return gsl_ran_binomial(__r,p_,n_);
    }
      
  gsl_rng * rng::operator()( void )
    {
      return __r;
    }
  
  const gsl_rng * rng::operator()( void ) const
    {
      return __r;
    }


}

// end of mygsl::rng.c
