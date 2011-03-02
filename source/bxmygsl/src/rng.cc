// mygsl::rng.c
/*
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 *
 */

#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string.h>

#include <mygsl/rng.h>

namespace mygsl {

  bool rng::g_debug = false;

  const std::string rng::DEFAULT_RNG_TYPE = "taus2";
  //const std::string rng::DEFAULT_RNG_TYPE = "mt19937";

  void rng::default_setup ()
  {
    gsl_rng_env_setup ();
    return;
  }
 
  rng::initializer::initializer ()
  {
    if (g_debug) 
      {
				std::cerr << "DEBUG: rng::initializer::ctor: " 
									<< "initialize dictionnary." << std::endl;
      }
    const gsl_rng_type ** t0 = gsl_rng_types_setup ();
    for (const gsl_rng_type ** t = t0; *t != 0; t++) 
      {
				std::string id ((*t)->name);
				dict[id] = *t;
      }
    return;
  }

  rng::initializer::~initializer ()
  {
    dict.clear();
    return;
  }

  /*************************/

	void rng::dump (ostream & out_) const
	{
		out_ << "mygsl::rng::dump: " << endl;
		if (is_initialized ())
			{
				out_ << "|-- " << "Initialized   : " << is_initialized () << endl;
				out_ << "|-- " << "PRNG name     : " << name () << endl;
				out_ << "`-- " << "Internal state size : " << get_internal_state_size ()
						 << endl;
			}
		else
			{
				out_ << "`-- " << "Initialized   : " << is_initialized () << endl;
			}
		return;
	}

  bool rng::is_initialized () const
  {
    return __r != 0;
  }

  void rng::print_dict (std::ostream & out_)
  {
    if (g_debug) 
      {
				std::cerr << "Available GSL random number generators: " 
									<< std::endl;
      }
    std::map<std::string,const gsl_rng_type *>::const_iterator i;
    for (i = rng::g__initializer.dict.begin (); 
				 i != rng::g__initializer.dict.end (); 
				 i++) 
      {
				out_ << i->first << std::endl; 
      } 
    return;
  }

  rng::initializer rng::g__initializer;

  void rng::set_seed (unsigned long int seed_)
  {
    if (__r == 0) 
      {
				std::ostringstream message;
				message << "gsl::rng::set_seed: Generator is not initialized ! Invoke 'rng::init' first !";
				throw std::runtime_error (message.str ());
			}
    gsl_rng_set (__r, seed_);    
    return;
  }

  void rng::initialize (const std::string & id_, 
												unsigned long int seed_)
  {
    this->init (id_, seed_);
    return;
  }
 
  void rng::init (const std::string & id_, 
									unsigned long int seed_)
  {
    if (__r != 0) 
      {
				reset ();
      }
    if (g__initializer.dict.find (id_) == g__initializer.dict.end ()) 
      {
				std::ostringstream message;
				message << "gsl::rng::init: Cannot find the '" 
								<< id_ << "' generator!";
				throw std::runtime_error (message.str ());
      }
    __r = gsl_rng_alloc (g__initializer.dict[id_]);
    if (__r == 0) 
      {
				std::ostringstream message;
				message << "gsl::rng::init: Cannot allocate the '" 
								<< id_ << "' generator!";
				throw std::runtime_error (message.str ());
      }
    gsl_rng_set (__r, seed_);
    return;
  }

  /*
		rng::rng ()
		{
    __r = 0;
    return;
		}
  */

  // ctor:
  rng::rng (const std::string & id_, unsigned long int seed_)
  {
    __r = 0;
    init (id_, seed_);
    return;
  }

  void rng::reset ()
  {
    if (g_debug) 
      {
				std::clog << "DEBUG: rng::reset: " 
									<< "Entering." << std::endl;
      }
    if (__r != 0) gsl_rng_free (__r);
    __r = 0;
    return;
  }
   
  size_t rng::get_internal_state_size () const
  {
    if (__r == 0)
      {
				std::ostringstream message;
				message << "gsl::rng::get_internal_state_size: Generator is not initialized !";
				throw std::runtime_error (message.str ());	
      }
    size_t sz = gsl_rng_size (__r); 
    return sz;
  }

  // dtor:
  rng::~rng ()
  {
    reset ();
    return;
  }

  unsigned long int rng::get ()
  {
    return gsl_rng_get (__r);
  }

  double rng::uniform ()
  {
    return gsl_rng_uniform (__r);
  }

  double rng::uniform_pos ()
  {
    return gsl_rng_uniform_pos (__r);
  }

  unsigned long int rng::uniform_int (unsigned long int n_)
  {
    return gsl_rng_uniform_int (__r, n_);
  }

  std::string rng::name () const
  {
    if (__r == 0)
      {
				std::ostringstream message;
				message << "gsl::rng::name: Generator is not initialized !";
				throw std::runtime_error (message.str ());	
      }
    return (std::string (gsl_rng_name (__r)));
  }

  unsigned long int rng::min () const
  {
    return (gsl_rng_min (__r));
  }

  unsigned long int rng::max () const
  {
    return (gsl_rng_max (__r));
  }

  void rng::store (const std::string & filename_) const
  {
    FILE * stream = fopen (filename_.c_str (), "w");
    if (stream == NULL) 
      {
				int errid=errno;
				std::ostringstream message;
				message << "gsm::rng::store: Cannot open file '"
								<< filename_ << "'!";
				throw std::runtime_error (message.str ());
      }
    int ret=gsl_rng_fwrite (stream, __r);
    if (ret == GSL_EFAILED) 
      {
				std::ostringstream message;
				message << "gsm::rng::store: Cannot store state in file '"
								<< filename_ << "'!";
				throw std::runtime_error (message.str ());
      }
    fclose (stream);
    return;
  }

  void rng::load (const std::string & filename_)
  {
    FILE * stream = fopen (filename_.c_str (), "r");
    if (stream == NULL) 
      {
				int errid=errno;
				std::ostringstream message;
				message << "gsm::rng::load: Cannot open file '"
								<< filename_ << "'!";
				throw std::runtime_error (message.str ());
      }
    int ret=gsl_rng_fread (stream, __r);
    if (ret == GSL_EFAILED)  
      {
				std::ostringstream message;
				message << "gsm::rng::load: Cannot load state from file '"
								<< filename_ << "'!";
				throw std::runtime_error (message.str ());
      }
    ret = fclose (stream);
    if (ret != 0) 
      {
				int errid = errno;
				std::ostringstream message;
				message << "gsm::rng::load: Cannot close file '"
								<< filename_ << "'!";
				throw std::runtime_error (message.str ());
      }
    return;
  }

  void rng::to_buffer (rng::state_buffer_t & buffer_) const
  {
    if (__r == 0) 
      {
				std::ostringstream message;
				message << "gsl::rng::to_buffer: Generator is not initialized !";
				throw std::runtime_error (message.str ());
			}    
    void * state = gsl_rng_state (__r);
    size_t n = gsl_rng_size (__r);
    const unsigned char * b = (const unsigned char *) state;
    buffer_.reserve (n);
    buffer_.clear ();
    for (int i = 0; i < n; i++) 
      {
				buffer_.push_back (*b);
				b++;
      }
		/*
			std::clog << "DEVEL: " << "gsl::rng::to_buffer: "
			<< "Buffer : " << buffer_.size () << '/' << buffer_.capacity () << std::endl;
		*/
		return;
  }

  void rng::to_stream (std::ostream & out_) const
  {
    void * state = gsl_rng_state (__r);
    size_t n = gsl_rng_size (__r);
    const unsigned char * b = (const unsigned char *) state;
    out_ << this->name () << ' ' << n;
    for (int i = 0; i < n; i++) 
      {
				unsigned int c = (unsigned int) *b;
				out_ << ' ' << std::dec << c;
				b++;
      }
    return;
  }

  void rng::from_buffer (const rng::state_buffer_t & buffer_)
  {
    if (__r == 0) 
      {
				std::ostringstream message;
				message << "gsl::rng::from_buffer: Generator is not initialized !";
				throw std::runtime_error (message.str ());
			}    
    void * state = gsl_rng_state (__r);
    size_t n = gsl_rng_size (__r);
		if ((buffer_.size () != 0) && (n != buffer_.size ()))
      {
				std::ostringstream message;
				message << "gsl::rng::from_buffer: " 
								<< "Size of the state buffer does not match the size of the internal state of the '"
								<< this->name () << "' generator !";
				throw std::runtime_error (message.str ());
      }
    unsigned char * b = static_cast<unsigned char *> (state);
    for (int i = 0; i < buffer_.size (); i++) 
      {
				*b = buffer_[i];
				b++;
      }
    return;
  }

  void rng::from_stream (std::istream & in_) 
  {
    std::string token;
    in_ >> token;
    if (! in_) 
      {
				throw std::runtime_error ("gsl::rng::from_stream: Cannot read generator name from stream!");      
      }
    if (g_debug) {
      std::cerr << "DEBUG: rng::from_stream: name='" 
								<< token << "'" << std::endl;
    }
    init (token);
    int n;
    in_ >> n;
    if (! in_) 
      {
				throw std::runtime_error ("gsl::rng::from_stream: Cannot read generator size from stream!");      
      }
    if (n != gsl_rng_size (__r)) 
      {
				throw std::runtime_error ("gsl::rng::from_stream: Invalid  generator size!");      
      }
    if (g_debug) 
      {
				std::cerr << "DEBUG: rng::from_stream: n='" 
									<< n << "'" << std::endl;
      }
    //unsigned char * s = 0;
    //s = new unsigned char[n];
    const size_t SBUFSZ = 65536;
    if (n > SBUFSZ)
      {
				throw std::runtime_error ("gsl::rng::from_stream: Not enough room to store the PRNG state !");      
      }
    unsigned char s[SBUFSZ];
    unsigned char * ps = s;
    for (int i = 0; i < n; i++) 
      {
				unsigned int c;
				in_ >> c ;
				if (! in_) 
					{
						throw std::runtime_error ("gsl::rng::from_stream: Cannot read state byte from stream !");      
					}
				/*
					if (g_debug) {
					std::cerr << "DEBUG: rng::from_stream: i=" 
					<< i << "   c='" 
					<< c << "'" << std::endl;
					}
				*/
				*ps = (unsigned char) c;
				ps++;
				in_ >> std::ws;
      }
    void * state = gsl_rng_state (__r);
    memcpy (state, s, n);
    //delete[] s;
    return;
  }
  
  // specific distributions:
 
  double rng::flat (double min_, double max_) const
  {
    return gsl_ran_flat (__r, min_, max_);
  }
  
  double rng::gaussian (double sigma_) const
  {
    return gsl_ran_gaussian_ziggurat (__r, sigma_);
  }
  
  double rng::gaussian (double mu_, double sigma_) const
  {
    return mu_ + gaussian (sigma_);
  }

  double rng::gaussian_tail (double min_, double sigma_) const
  {
    return gsl_ran_gaussian_tail (__r, min_, sigma_);
  }

  double rng::exponential (double sigma_) const
  {
    return gsl_ran_exponential (__r, sigma_);
  }

  double rng::chisquare (double nu_) const
  {
    return gsl_ran_chisq (__r, nu_);
  }

  unsigned long int rng::poisson (double mu_) const
  {
    return gsl_ran_poisson (__r, mu_);
  }

  unsigned long int rng::bernoulli (double p_) const
  {
    return gsl_ran_bernoulli (__r, p_);
  }

  unsigned long int rng::binomial (double p_, 
																	 unsigned long int n_) const
  {
    return gsl_ran_binomial (__r, p_, n_);
  }
      
  /*
		gsl_rng * rng::operator () (void)
		{
    return __r;
		}
  
		const gsl_rng * rng::operator () (void) const
		{
    return __r;
		}
  */

  double rng::operator () (void)
  {
    return this->uniform ();
  }

}

// end of mygsl::rng.c
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
