// -*- mode: c++ ; -*- 
/* random_cut.cc
 */

#include <cuts/random_cut.h>

#include <stdexcept>
#include <sstream>

namespace cuts {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(random_cut, "cuts::random_cut");

  // ctor:
  CUT_CONSTRUCTOR_IMPLEMENT_HEAD (random_cut,
                                  a_debug_devel,
                                  "cuts::random_cut",
                                  "Random cut",
                                  "1.0")
  {
    _seed_ = 0;
    _accept_probability_ = 0.5;
    return;
  }
  
  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT (random_cut)

  CUT_ACCEPT_IMPLEMENT_HEAD(random_cut)
  {
    return (*_uni_.get())() < _accept_probability_ ?  i_cut::ACCEPTED :  i_cut::REJECTED;
  } 

  CUT_INITIALIZE_IMPLEMENT_HEAD(random_cut,
                                a_configuration,
                                a_service_manager,
                                a_dict)
  {
    using namespace std;
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "cuts::random_cut::initialize: "
                << "Cut '" << get_name () << "' is already initialized ! ";
        throw std::logic_error (message.str ());
      }
    
    if (a_configuration.has_key ("seed"))
      {
        int seed = a_configuration.fetch_integer ("seed");
        if (seed < 0)
          {
            std::ostringstream message;
            message << "cuts::random_cut::initialize: "
                    << "Invalid seed value (" << seed << ") ! ";
            throw std::logic_error (message.str ());
          }
        _seed_ = seed;
      }
   
    if (a_configuration.has_key ("accept_probability"))
      {
        double accept_probability = a_configuration.fetch_real ("accept_probability");
        if (accept_probability < 0.0 || accept_probability > 1.0)
          {
            std::ostringstream message;
            message << "cuts::random_cut::initialize: "
                    << "Invalid accept probability (" << accept_probability << ") ! ";
            throw std::logic_error (message.str ());
          }
        _accept_probability_ = accept_probability;
      }

    _bg_.reset(new base_generator_type (_seed_));
    _uni_dist_.reset(new boost::uniform_real<>(0.0, 1.0));
    _uni_.reset (new boost::variate_generator<base_generator_type&, 
                                              boost::uniform_real<> >(*_bg_.get(), 
                                                                      *_uni_dist_.get())
                 );

    _set_initialized (true);
    return;     
  }

  CUT_RESET_IMPLEMENT_HEAD (random_cut) 
  {
    this->i_cut::reset ();
    _uni_.reset (0);
    _uni_dist_.reset (0);
    _bg_.reset (0);
    _seed_ = 0;
    _accept_probability_ = 0.5;
    _set_initialized (false);
    return;
  }
  
} // end of namespace cuts

// end of random_cut.cc
