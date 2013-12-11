/* kernel.cc */
// Ourselves
#include <datatools/kernel.h>

// Third Party
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

// This project
#include <datatools/library_info.h>
#include <datatools/exception.h>
#include <datatools/version.h>

namespace datatools {

  // static
  kernel * kernel::_instance_ = 0;

  // basic construction
  void kernel::_construct_()
  {
    _initialized_ = false;
    _logging_ = datatools::logger::PRIO_WARNING;
    DT_THROW_IF(_instance_, std::runtime_error,
                "An instance of the datatools kernel already exists !");
    _activate_library_info_register_ = true;
    DT_LOG_TRACE(_logging_, "Kernel is now constructed.");
    _instance_ = this;
    return;
  }

  kernel::kernel()
  {
    _construct_();
    return;
  }

  kernel::kernel(int argc_, char * argv_[])
  {
    _construct_();
    initialize(argc_, argv_);
    return;
  }

  kernel::~kernel()
  {
    if (_initialized_) {
      this->shutdown();
    }
    DT_LOG_TRACE(_logging_, "Kernel is now destroyed.");
    _instance_ = 0;
    return;
  }

  bool kernel::is_initialized() const
  {
    return _initialized_;
  }

  void kernel::build_opt_desc(boost::program_options::options_description & opts_,
                              param_type & params_)
  {
    namespace po = boost::program_options;

    opts_.add_options()
      ("datatools::help",
       po::value<bool>(&params_.help)
       ->zero_tokens()
       ->default_value(false),
       "Print help for the datatools kernel configuration options.  \n"
       "Example :                                                   \n"
       "  --datatools::help                                           "
       )
      ("datatools::splash",
       po::value<bool>(&params_.splash)
       ->zero_tokens()
       ->default_value(false),
       "Print splash screen at datatools kernel loading.            \n"
       "Example :                                                   \n"
       "  --datatools::splash                                         "
       )
      ("datatools::logging",
       po::value<std::string>(&params_.logging_label)
       ->default_value("warning"),
       "Set the datatools kernel's logging priority threshold.  \n"
       "Example :                                               \n"
       "  --datatools::logging \"trace\"                          "
       )
      ("datatools::libinfo::logging",
       po::value<std::string>(&params_.library_info_logging_label)
       ->default_value("warning"),
       "Set the datatools kernel's library info logging priority threshold.  \n"
       "Example :                                                            \n"
       "  --datatools::libinfo::logging \"trace\"                              "
       )
      ("datatools::nolibinfo",
       po::value<bool>(&params_.inhibit_library_info_register)
       ->zero_tokens()
       ->default_value(false),
       "Inhibit the use of the library/component information register.  \n"
       "Example :                                                       \n"
       "  --datatools::nolibinfo                                          "
       )
      ("datatools::resource_path",
       po::value< std::vector<std::string> >(&params_.resource_paths),
       "Register some resource paths associated to some libraries or modules.\n"
       "Example :                                                            \n"
       "  Register the \"foo@path1/subdir1\" path as the root directory of   \n"
       "  the \"foo\" library and the \"bar@path2/subdir2\" path as the      \n"
       "  root directory of the \"bar\" software module:                     \n"
       "    --datatools::resource_path \"foo@path1/subdir1\"                 \n"
       "    --datatools::resource_path \"bar@path2/subdir2\"                   "
       )
      ;

    return;
  }

  kernel::param_type::param_type()
  {
    reset();
    return;
  }

  void kernel::param_type::reset()
  {
    help = false;
    logging_label = "warning";
    library_info_logging_label = "warning";
    inhibit_library_info_register = false;
    splash = false;
    return;
  }

  // static
  void kernel::print_opt_desc(const boost::program_options::options_description & opts_,
                              std::ostream & out_,
                              const std::string& /*indent_*/)
  {
    //out_ << "\ndatatools -- Kernel configuration options" << std::endl;
    out_ << std::endl;
    out_ << opts_ << std::endl;
    out_ << std::endl;
    return;
  }

  // static
  void kernel::print_splash(std::ostream & out_)
  {
    out_ << "\n";
    out_ << "   D A T A T O O L S    K E R N E L\n";
    out_ << "\n";
    out_ << "   Version: " << datatools::version::get_version() << "\n";
    out_ << "\n";
    const datatools::kernel & kern =  datatools::kernel::const_instance();
    out_ << "   Library info register : " << (kern.has_library_info_register()?
                                              "activated" : "NA") << "\n";
    if (kern.has_library_info_register()) {
      // On board registered libraries...
    }
    out_ << "\n";
    return;
  }

  void kernel::_initialize(const param_type & params_)
  {
    if (params_.inhibit_library_info_register) {
      this->_activate_library_info_register_ = false;
    }

    // Set the kernel logging priority threshold:
    this->_logging_ = datatools::logger::get_priority(params_.logging_label);

    // Instantiate the kernel library information register:
    if (this->_activate_library_info_register_) {
      _library_info_register_.reset(new library_info);
      _library_info_register_->set_logging(datatools::logger::get_priority(params_.library_info_logging_label));
      _library_info_register_->initialize();
      DT_LOG_TRACE(_logging_, "Kernel's library info registered is now created.");
    } else {
      DT_LOG_TRACE(_logging_, "Kernel's library info registered is not created.");
    }

    // Splash some fancy screen:
    if (params_.splash) {
      kernel::print_splash(std::clog);
    }

    // Parse some special directives to load arbitrary resource path associated
    // to some library or some software component identified by their names:
    for (size_t i = 0; i < params_.resource_paths.size(); i++) {
      std::string resource_path_registration = params_.resource_paths[i];
      DT_LOG_TRACE(_logging_, "Resource path registration : '"
                   << resource_path_registration << "'");
      // format is :
      //   "foo@path1/subdir/..."
      size_t apos = resource_path_registration.find('@');
      DT_THROW_IF(apos == resource_path_registration.npos,
                  std::logic_error,
                  "Invalid syntax in resource path registration directive ('"
                  << resource_path_registration << "' !");
      std::string lib_name = resource_path_registration.substr(0,apos);
      // boost::filesystem::path
      std::string lib_resource_path = resource_path_registration.substr(apos+1);
      DT_LOG_TRACE(_logging_, "Library " << lib_name << "' resource path '"
                   << lib_resource_path << "' registration...");
      DT_THROW_IF(_library_info_register_->has("lib_name"),
                  std::logic_error,
                  "Library info '"<< lib_name << "'is already registered !");
      datatools::properties & lib_infos
        = _library_info_register_->registration(lib_name);
      lib_infos.store_string(datatools::library_info::keys::install_resource_dir(),
                             lib_resource_path
                             );
    }
    if (_logging_ >= datatools::logger::PRIO_TRACE) {
      _library_info_register_->tree_dump(std::cerr, "Library information register: ");
    }
    return;
  }

  void kernel::initialize(int argc_, char * argv_[])
  {
    // Fetch the application name:
    if ( argc_ >= 1 ) {
      _application_name_= argv_[0];
    }

    // Parse command line options:
    namespace po = boost::program_options;
    param_type params;
    po::options_description opts("datatools' kernel options ");
    build_opt_desc(opts, params);
    po::positional_options_description args;
    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options(opts) // Only options to be parsed.
      //.positional(args) // This crashes so we don't use it here !
      .allow_unregistered()
      .run();
    params.unrecognized_args = po::collect_unrecognized(parsed.options,
                                                        po::include_positional);
    po::store(parsed, vm);
    po::notify(vm);
    if (params.help) {
      print_opt_desc(opts, std::cout);
    }

    // Initialize internals:
    _initialize(params);

    _initialized_ = true;
    DT_LOG_TRACE(_logging_, "Kernel is instantiated and initialized.");
    return;
  }

  void kernel::shutdown()
  {
    if (! _initialized_) return;

    // Terminate internals:
    if (_library_info_register_) {
      _library_info_register_->reset();
      _library_info_register_.reset(0);
    }
    DT_LOG_TRACE(_logging_, "Kernel's library info registered is now destroyed.");

    // Revert to default idle status:
    _activate_library_info_register_ = true;
    _application_name_.clear();

    _initialized_ = false;
    DT_LOG_TRACE(_logging_, "Kernel has shutdown.");
    return;
  }

  const std::string & kernel::get_application_name() const
  {
    return _application_name_;
  }

  bool kernel::has_application_name() const
  {
    return ! _application_name_.empty();
  }

  logger::priority kernel::get_logging() const
  {
    return _logging_;
  }

  void kernel::set_logging(logger::priority logging_)
  {
    _logging_ = logging_;
  }

  bool kernel::has_library_info_register() const
  {
    return _library_info_register_;
  }

  library_info & kernel::grab_library_info_register()
  {
    DT_THROW_IF(!_library_info_register_, std::logic_error,
                "The datatools kernel has not library info register !");
    return *_library_info_register_;
  }

  const library_info & kernel::get_library_info_register() const
  {
    DT_THROW_IF(!_library_info_register_, std::logic_error,
                "The datatools kernel has not library info register !");
    return *_library_info_register_;
  }

  void kernel::tree_dump(std::ostream& out_,
                         const std::string& title_,
                         const std::string& indent_,
                         bool inherit_) const
  {
    std::string indent;
    if (!indent_.empty()) indent = indent_;
    if (!title_.empty()) {
      out_ << indent << title_ << std::endl;
    }
    out_ << indent << i_tree_dumpable::tag
         << "Initialized   : " << _initialized_ << std::endl;
    out_ << indent << i_tree_dumpable::tag
         << "Application   : '" << _application_name_ << "'" << std::endl;
    out_ << indent << i_tree_dumpable::tag
         << "Logging   : '"
         << datatools::logger::get_priority_label(_logging_) << "'"
         << std::endl;
    out_ << indent << i_tree_dumpable::tag
         << "Library info register : '" << (_activate_library_info_register_? "activated" : "not activated") << "'" <<std::endl;
    if (has_library_info_register()) {
      std::ostringstream indent2;
      indent2 << indent << i_tree_dumpable::skip_tag;
      get_library_info_register().tree_dump(out_, "", indent2.str());
    }
    out_ << indent << i_tree_dumpable::inherit_tag(inherit_)
         << "Kernel instance at : " << _instance_ << std::endl;
    return;
  }

  // static
  bool kernel::is_instantiated()
  {
    return _instance_ != 0;
  }

  // static
  kernel & kernel::instantiate()
  {
    if (! kernel::is_instantiated()) {
      static boost::scoped_ptr<datatools::kernel> _kernel_handler;
      if (! _kernel_handler) {
        // Allocate the new global kernel and initialize it
        _kernel_handler.reset(new datatools::kernel);
      }
    }
    return kernel::instance();
  }

  // static
  kernel & kernel::instance()
  {
    return *_instance_;
  }

  // static
  const kernel & kernel::const_instance()
  {
    return *_instance_;
  }

} // end of namespace datatools
