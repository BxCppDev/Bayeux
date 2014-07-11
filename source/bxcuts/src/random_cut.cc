// random_cut.cc

// Ourselves:
#include <cuts/random_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace cuts {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(random_cut, "cuts::random_cut");

  random_cut::random_cut(datatools::logger::priority a_logger_priority)
  : i_cut(a_logger_priority)
  {
    _seed_ = 0;
    _accept_probability_ = 0.5;
    return;
  }

  random_cut::~random_cut()
  {
    if (is_initialized()) {
      this->random_cut::reset();
    }
    return;
  }

  int random_cut::_accept()
  {
    return (*_uni_.get())() < _accept_probability_ ?  SELECTION_ACCEPTED : SELECTION_REJECTED;
  }

  void random_cut::initialize(const datatools::properties & a_configuration,
                              datatools::service_manager & /*a_service_manager*/,
                              cuts::cut_handle_dict_type & /*a_cut_dict*/)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Random cut '" << get_name() << "' is already initialized ! ");

    _common_initialize(a_configuration);

    if (a_configuration.has_key ("seed")) {
      int seed = a_configuration.fetch_integer("seed");
      DT_THROW_IF(seed < 0,
                  std::logic_error,
                  "Invalid seed value (" << seed << ") !)");
      _seed_ = seed;
    }

    if (a_configuration.has_key ("accept_probability")) {
      double accept_probability = a_configuration.fetch_real ("accept_probability");
      DT_THROW_IF(accept_probability < 0.0 || accept_probability > 1.0,
                  std::logic_error,
                  "Invalid accept probability (" << accept_probability << ") ! ");
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

  void random_cut::reset()
  {
    _uni_.reset(0);
    _uni_dist_.reset(0);
    _bg_.reset(0);
    _seed_ = 0;
    _accept_probability_ = 0.5;
    this->i_cut::_reset();
    _set_initialized(false);
    return;
  }

} // end of namespace cuts
