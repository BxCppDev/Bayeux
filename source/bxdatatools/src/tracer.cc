// ioutils.cc

// Ourselves:
#include <datatools/tracer.h>

// Standard Library:
#include <sstream>
#include <map>

// Third Party:
// - Boost:
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

// This Project:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace datatools {

  tracer::tracer()
  {
    _id_ = -1;
    _counter_ = 0;
    _preserved_file_ = false;
  }

  tracer::tracer(int id_,
                 const std::string & filename_,
                 const std::string & label_)
  {
    _id_ = -1;
    _counter_ = 0;
    _preserved_file_ = false;
    set_id(id_);
    set_filename(filename_);
    set_label(label_);
  }


  tracer::~tracer()
  {
    if (is_initialized()) {
      reset();
    }
  }

  void tracer::set_id(int id_)
  {
    DT_THROW_IF(id_ < 0, std::logic_error, "Invalid tracer ID '" << id_ << "' !");
    _id_ = id_;
  }

  int tracer::get_id() const
  {
    return _id_;
  }

  void tracer::set_label(const std::string & label_)
  {
    _label_ = label_;
  }

  const std::string & tracer::get_label() const
  {
    return _label_;
  }

  void tracer::set_filename(const std::string & fn_)
  {
    _filename_ = fn_;
  }

  const std::string &tracer::get_filename() const
  {
    return _filename_;
  }

  void tracer::set_preserved_file(bool pf_)
  {
    _preserved_file_ = pf_;
  }

  bool tracer::is_preserved_file() const
  {
    return _preserved_file_;
  }

  bool tracer::is_initialized() const
  {
    return _fout_.get() != 0;
  }

  void tracer::initialize(int id_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Tracer is already initialized !");
    set_id(id_);
    if (_filename_.empty()) {
      std::ostringstream oss_fn;
      oss_fn << "tracer_" << id_ << ".log";
      set_filename(oss_fn.str());
    }
    initialize();
  }

  void tracer::initialize(int id_, const std::string & filename_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Tracer is already initialized !");
    set_id(id_);
    set_filename(filename_);
    initialize();
  }

  void tracer::initialize(int id_, const std::string & filename_, bool preserved_file_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Tracer is already initialized !");
    set_id(id_);
    set_filename(filename_);
    set_preserved_file(preserved_file_);
    initialize();
  }


  void tracer::initialize()
  {
    datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
    DT_THROW_IF (is_initialized(), std::logic_error, "Tracer is already initialized !");
    DT_LOG_NOTICE(logging, "Initializing tracer ID=" << _id_ << " with file '" << _filename_<< "'...");
    std::string fn = _filename_;
    datatools::fetch_path_with_env(fn);
    if (is_preserved_file()) {
      DT_THROW_IF (boost::filesystem::exists(fn), std::runtime_error,
                   "Tracker file '" << fn << "' already exists !");
    }
    _fout_.reset(new std::ofstream(fn.c_str()));
    std::ofstream & fout = *_fout_.get();
    DT_THROW_IF (! fout, std::runtime_error, "Cannot open tracer file '" << fn << "' !");
    *_fout_.get() << "#@tracer_id=" << _id_ << std::endl;
    *_fout_.get() << "#@tracer_label=" << _label_ << std::endl;
    *_fout_.get() << "#@tracer_filename=" << _filename_ << std::endl;
    _fout_.get()->precision(15);
  }

  void tracer::reset()
  {
    DT_THROW_IF (! is_initialized(), std::logic_error, "Tracer is not initialized !");
    _fout_.reset();
    _filename_.clear();
    _label_.clear();
    _id_ = -1;
    _counter_ = 0;
  }

  std::ofstream & tracer::out(bool increment_)
  {
    if (! is_initialized()) initialize();
    if (increment_) {
      _counter_++;
      *_fout_.get() << _counter_;
    }
    //*_fout_.get() << ' ';
    return *_fout_.get();
  }

  // static
  typedef boost::shared_ptr<tracer> tracer_ptr;

  std::map<int,tracer_ptr> & _get_global_tracers()
  {
    datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
    static boost::scoped_ptr<std::map<int,tracer_ptr> > _g_tracers(0);
    if (_g_tracers.get() == 0) {
      DT_LOG_NOTICE(logging, "Creating the global dictionnary of tracers...");
      _g_tracers.reset(new std::map<int, tracer_ptr>);
    }
    return *_g_tracers.get();
  }

  // static
  datatools::tracer & tracer::grab_global_tracer(int id_,
                                                 const std::string & filename_)
  {
    datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
    DT_THROW_IF(id_ < 0, std::logic_error, "Invalid tracer ID '" << id_ << "' !");
    std::map<int, tracer_ptr> & tracers = _get_global_tracers();
    if (tracers.find(id_) == tracers.end()) {
      DT_LOG_NOTICE(logging, "Creating the tracer with ID = " << id_ << " ...");
      tracer_ptr dummy(new tracer);
      dummy.get()->set_id(id_);
      if (! filename_.empty()) {
        DT_LOG_NOTICE(logging, "Initializing the tracer with ID = " << id_ << " ...");
        dummy.get()->set_filename(filename_);
        dummy.get()->initialize();
      }
      tracers[id_] = dummy;
    }
    tracer_ptr & pt = tracers.find(id_)->second;
    return *pt.get();
  }

} // namespace datatools
