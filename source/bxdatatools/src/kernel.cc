// kernel.cc

// Standard Library:
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <map>
#include <string>

// Ourselves:
#include <datatools/kernel.h>

// Third Party:
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

// This project:
#include <datatools/library_info.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/io.h>
#include <datatools/configuration/utils.h>
#include <datatools/exception.h>
#include <datatools/version.h>
#include <datatools/command_utils.h>
#include <datatools/utils.h>

#if DATATOOLS_WITH_QT_GUI == 1
#include <QStyleFactory>
#include <QApplication>
#include <datatools/configuration/ui/variant_repository_dialog.h>
#endif // DATATOOLS_WITH_QT_GUI == 1

namespace datatools {

  // static
  kernel * kernel::_instance_ = 0;

  // basic construction
  void kernel::_construct_()
  {
    _initialized_ = false;
    _logging_ = datatools::logger::PRIO_FATAL;
    {
      char * e = getenv("DATATOOLS_KERNEL_LOGGING");
      if (e) {
        std::string dklogging = e;
        datatools::logger::priority p = datatools::logger::get_priority(_params_.logging_label);
        if (p != datatools::logger::PRIO_UNDEFINED) {
          // Set the kernel logging priority threshold:
           _logging_ = p;
        }
      }
    }
    DT_THROW_IF(_instance_, std::runtime_error,
                "An instance of the datatools kernel already exists !");
    _activate_library_info_register_ = true;
    _activate_variant_repository_ = true;
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
       "  --datatools::logging=\"trace\"                          "
       )

      ("datatools::nolibinfo",
       po::value<bool>(&params_.inhibit_library_info_register)
       ->zero_tokens()
       ->default_value(false),
       "Inhibit the use of the library/component information register.  \n"
       "Example :                                                       \n"
       "  --datatools::nolibinfo                                          "
       )

      ("datatools::libinfo::logging",
       po::value<std::string>(&params_.library_info_logging_label)
       ->default_value("warning"),
       "Set the datatools kernel's library info logging priority threshold.  \n"
       "Example :                                                            \n"
       "  --datatools::libinfo::logging=\"trace\"                              "
       )

      ("datatools::resource-path",
       po::value< std::vector<std::string> >(&params_.resource_paths),
       "Register a resource path associated to a library or module.          \n"
       "Example :                                                            \n"
       "  Register the \"path1/subdir1\" path as the root directory of       \n"
       "  the \"foo\" library and the \"path2/subdir2\" path as the          \n"
       "  root directory of the \"bar\" software module:                     \n"
       "    --datatools::resource-path=\"foo@path1/subdir1\"                 \n"
       "    --datatools::resource-path=\"bar@path2/subdir2\"                   "
       )

      ("datatools::novariant",
       po::value<bool>(&params_.inhibit_variant_repository)
       ->zero_tokens()
       ->default_value(false),
       "Inhibit the use of the configuration variant repository.  \n"
       "Example :                                                 \n"
       "  --datatools::novariant                                    "
       )

      ("datatools::variant-config",
       po::value< std::string >(&params_.variant_config),
       "The system variant repository configuration filename.                 \n"
       "Example :                                                            \n"
       "  Register the \"config/variance.conf\" file to initialize the       \n"
       "  repository for some application:                                   \n"
       "    --datatools::variant-config=\"config/variance.conf\"             \n"
       )

      ("datatools::variant-registry-config",
       po::value< std::vector<std::string> >(&params_.variant_registry_configs),
       "Register a configuration variant registry in the system variant      \n"
       "repository.                                                          \n"
       "Example :                                                            \n"
       "  Register the \"config/registry.conf\" file to initialize a         \n"
       "  registry of configuration variants for some \"foo\" application:   \n"
       "    --datatools::variant-registry-config=\"foo@config/registry.conf\"\n"
       "  or                                                                 \n"
       "    --datatools::variant-registry-config=\"config/registry.conf\"    \n"
       "  if the file contains the registry name 'foo'.                        "
       )

      ("datatools::variant-load",
       po::value<std::string>(&params_.variant_load),
       "Load the values of the variant parameters from a file.               \n"
       "Example :                                                            \n"
       "  Load parameters' values from the \"config/saved_variants.rep\" file\n"
       "    --datatools::variant-load=\"config/saved_variants.rep\"            "
       )

      ("datatools::variant-set",
       po::value< std::vector<std::string> >(&params_.variant_sets),
       "Set the values of a set of configuration variant parameters associated  \n"
       "to some registry.                                                       \n"
       "Example :                                                               \n"
       "  Set the \"foo:detector0=1\" flag and the associated                   \n"
       "  \"foo:detector0/if_detector/material\" and                            \n"
       "  \"foo:detector0/if_detector/width\" parameters                        \n"
       "  in the proper variant registry:                                       \n"
       "    --datatools::variant-set=\"foo:detector0=1\"                        \n"
       "    --datatools::variant-set=\"foo:detector0/if_detector/material=Silicium\" \n"
       "    --datatools::variant-set=\"foo:detector0/if_detector/width=3 cm\"        \n"
       )

#if DATATOOLS_WITH_QT_GUI == 1
      ("datatools::variant-gui",
       po::value<bool>(&params_.variant_gui)
       ->zero_tokens()
       ->default_value(false),
       "Display the variant GUI dialog:                                     \n"
       "Example :                                                           \n"
       "   --datatools::variant-gui                                           "
       )

      ("datatools::variant-gui-style",
       po::value<std::string>(&params_.variant_gui_style),
       "Set the style of variant GUI dialog:                                \n"
       "Example :                                                           \n"
       "   --datatools::variant-gui-style=\"plastique\"                       "
       )
       //      ->default_value(std::string("plastique")),
#endif // DATATOOLS_WITH_QT_GUI == 1

      ("datatools::variant-store",
       po::value<std::string>(&params_.variant_store),
       "Store the values of the variant parameters in a file:                \n"
       "Example :                                                            \n"
       "  Store parameters' values from the \"var/backup_variants.rep\" file \n"
       "    --datatools::variant-store=\"var/backup_variants.rep\"             "
       )

      ;

    return;
  }

  void kernel::param_type::print(std::ostream & out_,
                                 const std::string & title_,
                                 const std::string & indent_)
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << "Help: " << help << std::endl;

    out_ << indent_ << "Logging label: '" << logging_label << "'" << std::endl;

    out_ << indent_ << "Library info logging label: '" << library_info_logging_label << "'"  << std::endl;

    out_ << indent_ << "Inhibit library info register: " << inhibit_library_info_register << std::endl;

    out_ << indent_ << "Inhibit variant repository: " << inhibit_variant_repository << std::endl;

    out_ << indent_ << "Unrecognized args: '" << unrecognized_args.size() << "'"  << std::endl;

    out_ << indent_ << "Resource paths: '" << resource_paths.size() << "'"  << std::endl;

    out_ << indent_ << "Variant config: '" << variant_config << "'" << std::endl;

    out_ << indent_ << "Variant registry configs: " << variant_registry_configs.size() << std::endl;

    out_ << indent_ << "Variant load: '" << variant_load << "'"  << std::endl;

    out_ << indent_ << "Variant store: '" << variant_store << "'"  << std::endl;

    out_ << indent_ << "Variant sets: " << variant_sets.size() << std::endl;

    out_ << indent_ << "Splash: " << splash << std::endl;

#if DATATOOLS_WITH_QT_GUI == 1
    out_ << indent_ << "Variant GUI: " << variant_gui << std::endl;
    out_ << indent_ << "Variant GUI style: '" << variant_gui_style << "'" << std::endl;
#endif // DATATOOLS_WITH_QT_GUI == 1

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
    logging_label = "fatal";
    library_info_logging_label = "fatal";
    inhibit_library_info_register = false;
    inhibit_variant_repository = false;
    unrecognized_args.clear();
    resource_paths.clear();
    variant_config.clear();
    variant_registry_configs.clear();
    variant_load.clear();
    variant_sets.clear();
    variant_store.clear();
    splash = false;
#if DATATOOLS_WITH_QT_GUI == 1
    variant_gui = false;
    variant_gui_style = "plastique";
#endif // DATATOOLS_WITH_QT_GUI == 1
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
    const datatools::kernel & kern = datatools::kernel::const_instance();
    out_ << "   Library info register : " << (kern.has_library_info_register()?
                                              "activated" : "NA") << "\n";
    out_ << "   Configuration variant repository : " << (kern.has_variant_repository()?
                                              "activated" : "NA") << "\n";
    if (kern.has_library_info_register()) {
      // On board registered libraries...
    }
    if (kern.has_variant_repository()) {
      // On board variant repository...
    }
    out_ << "\n";
    return;
  }

  void kernel::_initialize()
  {
    DT_LOG_TRACE_ENTERING(_logging_);

    // Set the kernel logging priority threshold:
    this->_logging_ = datatools::logger::get_priority(_params_.logging_label);

    if (_params_.inhibit_library_info_register) {
      DT_LOG_TRACE(_logging_, "Inhibit the library info register...");
      this->_activate_library_info_register_ = false;
    }

    if (_params_.inhibit_variant_repository) {
      DT_LOG_TRACE(_logging_, "Inhibit the variant repository...");
      this->_activate_variant_repository_ = false;
    }

    // Splash some fancy screen:
    if (_params_.splash) {
      kernel::print_splash(std::clog);
    }

    _initialize_library_info_register_();

    _initialize_configuration_variant_repository_();

    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void kernel::_shutdown()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    // Terminate internals:
    if (_variant_repository_) {
      _variant_repository_.reset(0);
    }
    DT_LOG_TRACE(_logging_, "Kernel's configuration variant repository is now destroyed.");

    if (_library_info_register_) {
      _library_info_register_.reset(0);
    }
    DT_LOG_TRACE(_logging_, "Kernel's library info registered is now destroyed.");

    // Revert to default idle status:
    _activate_variant_repository_ = true;
    _activate_library_info_register_ = true;
    _application_name_.clear();

    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void kernel::_initialize_library_info_register_()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    DT_LOG_INFORMATION(_logging_, "Initializing kernel's library info register...");

    // Instantiate the kernel library information register:
    if (this->_activate_library_info_register_) {
      _library_info_register_.reset(new library_info);
      _library_info_register_->set_logging(datatools::logger::get_priority(_params_.library_info_logging_label));
      _library_info_register_->initialize();
      DT_LOG_TRACE(_logging_, "Kernel's library info register is now created.");
      if (_logging_ == logger::PRIO_TRACE) {
        _library_info_register_->tree_dump(std::cerr, "Kernel's library info register:", "TRACE: ");
      }
    } else {
      DT_LOG_TRACE(_logging_, "Kernel's library info register is not created.");
    }
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void kernel::_initialize_configuration_variant_repository_()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    DT_LOG_INFORMATION(_logging_, "Initializing kernel's configuration variant repository...");
    // Instantiate the kernel variant repository:
    if (this->_activate_variant_repository_) {
      _variant_repository_.reset(new configuration::variant_repository);
      _variant_repository_->set_name("__system__");
      _variant_repository_->set_display_name("System Repository");
      _variant_repository_->set_terse_description("The Bayeux/datatools' kernel configuration variant repository");
      _variant_repository_->initialize_simple();

      DT_LOG_TRACE(_logging_, "Kernel's configuration variant repository is now created.");
      if (_logging_ == logger::PRIO_TRACE) {
        _variant_repository_->tree_dump(std::cerr, "Kernel's configuration variant repository:", "TRACE: ");
      }
    } else {
      DT_LOG_TRACE(_logging_, "Kernel's configuration variant repository is not created.");
    }

    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void kernel::register_resource_paths()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    DT_LOG_INFORMATION(_logging_, "Registration of resource paths...");

    if (_library_info_register_) {
      DT_LOG_TRACE(_logging_, "Number of resource paths = " << _params_.resource_paths.size());
      if (_logging_ == datatools::logger::PRIO_TRACE) {
        _params_.print(std::cerr, "Kernel's setup parameters: ", "TRACE: ");
      }
      // Parse some special directives to load arbitrary resource path associated
      // to some library or some software component identified by their names:
      for (size_t i = 0; i < _params_.resource_paths.size(); i++) {
        std::string resource_path_registration = _params_.resource_paths[i];
        DT_LOG_TRACE(_logging_, "Resource path registration : '"
                     << resource_path_registration << "'");
        // format is :
        //   "foo@path1/subdir/..."
        size_t apos = resource_path_registration.find('@');
        DT_THROW_IF(apos == resource_path_registration.npos,
                    std::logic_error,
                    "Invalid syntax in resource path registration directive ('"
                    << resource_path_registration << "' !");
        std::string lib_name = resource_path_registration.substr(0, apos);
        // boost::filesystem::path
        std::string lib_resource_path = resource_path_registration.substr(apos+1);
        DT_LOG_TRACE(_logging_, "Library " << lib_name << "' resource path '"
                     << lib_resource_path << "' registration...");
        DT_THROW_IF(_library_info_register_->has(lib_name),
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
      _params_.resource_paths.clear();
    } // _library_info_register_
    else {
      DT_LOG_TRACE(_logging_, "No kernel's library info register is available!");
    }
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void kernel::register_configuration_variant_registries()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    if (_variant_repository_) {

      bool rep_locked = false;
      // Initialize the system variant repository from a configuration file:
      if (! _params_.variant_config.empty()) {
        std::string config_filename =_params_.variant_config;
        datatools::fetch_path_with_env(config_filename);
        datatools::properties config;
        DT_LOG_TRACE(_logging_, "Variant configuration file '"
                     << config_filename << "'...");
        config.read_configuration(config_filename);
        _variant_repository_->set_organization("");
        _variant_repository_->set_application("");
        _variant_repository_->unlock();
        DT_LOG_TRACE(_logging_, "About to load registries...");
        _variant_repository_->load_registries(config);
        if (_logging_ >= datatools::logger::PRIO_TRACE) {
          _variant_repository_->tree_dump(std::cerr, "Configuration Variant Repository: ", "TRACE: ");
        }
        rep_locked = _variant_repository_->is_locked();
      }

      if (_params_.variant_registry_configs.size()) {
        if (rep_locked) {
          // Unlock to add more registries:
           _variant_repository_->unlock();
        }
        // Parse some special directives to load the definitions of configuration
        // variant registry from a file:

        for (size_t i = 0; i < _params_.variant_registry_configs.size(); i++) {
          std::string variant_config_registration = _params_.variant_registry_configs[i];
          DT_LOG_TRACE(_logging_, "Variants' configuration : '"
                       << variant_config_registration << "'");
          // format is :
          //   "path1/subdir/file.conf"
          // or:
          //   "foo@path1/subdir/file.conf"
          std::string variant_name;
          std::string variant_mgr_config_file;
          size_t apos = variant_config_registration.find('@');
          if (apos != std::string::npos) {
            variant_name = variant_config_registration.substr(0, apos);
            variant_mgr_config_file = variant_config_registration.substr(apos + 1);
          } else {
            variant_mgr_config_file = variant_config_registration;
          }
          datatools::fetch_path_with_env(variant_mgr_config_file);
          DT_LOG_TRACE(_logging_, "Variant " << variant_name << "' configuration file '"
                       << variant_mgr_config_file << "' registration...");
          DT_THROW_IF(_variant_repository_->has_registry(variant_name),
                      std::logic_error,
                      "Variant registry '" << variant_name << "'is already registered !");
          _variant_repository_->registration_embedded(variant_mgr_config_file, "", variant_name);
        }
        if (rep_locked) {
          // Relock to add more registries:
          _variant_repository_->lock();
        }
        if (_logging_ >= datatools::logger::PRIO_TRACE) {
          _variant_repository_->tree_dump(std::cerr, "Configuration Variant Repository: ", "TRACE: ");
        }
      }

      if (! _params_.variant_load.empty()) {
        // Load the variant setup from a file:
        std::string variant_load = _params_.variant_load;
        datatools::fetch_path_with_env(variant_load);
        std::ifstream load_file;
        load_file.open(variant_load.c_str());
        DT_THROW_IF(!load_file, std::runtime_error,
                    "Cannot open configuration variant file '" << variant_load << "'!");
        unsigned int ioflags = 0;
        datatools::configuration::ascii_io repository_io(ioflags);
        int error = repository_io.load_repository(load_file, *_variant_repository_);
        DT_THROW_IF(error != 0, std::runtime_error,
                    "Failed to load repository from file '" << variant_load << "'!");
        DT_LOG_NOTICE(_logging_, "Variant repository was loaded from configuration file '"
                      << variant_load << "'.");
      }

      // Parse the directives that set values of specific configuration variant parameters
      // in a given variant registry:

      if (_params_.variant_sets.size()) {
        // Reorder the variant sets directives to garantee the dependencies
        // between active parameters.
        // Example: given the following unordered list of variant set directives:
        //
        //  foo:value/case_small/color/case_red/bar=false
        //  foo:value/case_small/color=red
        //  foo:value=1
        //  foo:value/case_small/count/case_zero/dummy=3
        //  foo:value/case_small/count=10
        //
        // We need to make sure that the following order is used:
        //
        //  foo:value=1
        //  foo:value/case_small/color=red
        //  foo:value/case_small/count=10
        //  foo:value/case_small/color/case_red/bar=false
        //  foo:value/case_small/count/case_non_zero/dummy=3
        //
        // This is because the foo registry respect the dependencies
        // while setting values. Here, the "foo:value/case_small/color"
        // variant parameter can only be set after the "foo:value" has been set to "1",
        // triggering the "foo:value/case_small" variant that in turns publishes the
        // "foo:value/case_small/color" and "foo:value/case_small/count" parameters.
        //
        std::vector<std::string> variant_sets = _params_.variant_sets;

        configuration::variant_parameter_set_comparator comp;
        std::sort(variant_sets.begin(), variant_sets.end(), comp);
        for (size_t i = 0; i < variant_sets.size(); i++) {
          DT_LOG_TRACE(_logging_, "Ordered variant parameter set : '" << variant_sets[i] << "'");
        }

        // Process all ordered variant sets directives:
        for (size_t i = 0; i < variant_sets.size(); i++) {
          std::string variant_set = variant_sets[i];
          DT_LOG_TRACE(_logging_, "Variant parameter set : '" << variant_set << "'");
          configuration::variant_parameter_set_type vps;
          // Format is : "foo:param0/var0/key0=value"
          vps.parse(variant_set);
          DT_LOG_TRACE(_logging_, "vps.registry_key    : '" << vps.registry_key << "'");
          DT_LOG_TRACE(_logging_, "vps.param_key       : '" << vps.param_key << "'");
          DT_LOG_TRACE(_logging_, "vps.param_value_str : '" << vps.param_value_str << "'");
          command::returned_info cri =
            _variant_repository_->cmd_set_parameter(vps.registry_key,
                                                    vps.param_key,
                                                    vps.param_value_str);
          DT_THROW_IF(cri.is_failure(),
                      std::logic_error,
                      cri.get_error_message());
        }
      }

#if DATATOOLS_WITH_QT_GUI == 1
      if (_params_.variant_gui) {
        QApplication::setStyle(QStyleFactory::create(QString::fromStdString(_params_.variant_gui_style)));
        int argc = 1;
        const char * argv[] = { "Bayeux - Configuration Variant Repository Dialog" };
        QApplication app(argc, (char**) argv);
        bool save_locked = _variant_repository_->is_locked();
        if (!save_locked) {
          _variant_repository_->lock();
        }
        datatools::configuration::ui::variant_repository_dialog vrep_dialog(*_variant_repository_);
        vrep_dialog.show();
        int ret = app.exec();
        if (!save_locked) {
          _variant_repository_->unlock();
        }
      }
#endif // DATATOOLS_WITH_QT_GUI == 1

      if (! _params_.variant_store.empty()) {
        // Store the variant setup in a file:
        std::string variant_store = _params_.variant_store;
        datatools::fetch_path_with_env(variant_store);
        std::ofstream store_file;
        store_file.open(variant_store.c_str());
        DT_THROW_IF(!store_file, std::runtime_error,
                    "Cannot open configuration variant file '" << variant_store << "'!");
        unsigned int ioflags = 0;
        datatools::configuration::ascii_io repository_io(ioflags);
        repository_io.store_repository(store_file, *_variant_repository_);
        DT_LOG_NOTICE(_logging_, "Variant repository was stored in configuration file '"
                      << variant_store << "'.");
      }

      // Clear processed variant related parameters:
      _params_.variant_config.clear();
      _params_.variant_registry_configs.clear();
      _params_.variant_load.clear();
      _params_.variant_sets.clear();
#if DATATOOLS_WITH_QT_GUI == 1
      _params_.variant_gui = false;
      _params_.variant_gui_style.clear();
#endif // DATATOOLS_WITH_QT_GUI == 1
      _params_.variant_store.clear();

    } else {
      DT_LOG_TRACE(_logging_, "No variant repository in the kernel.");
    } // _variant_repository_

    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void kernel::initialize(int argc_, char * argv_[])
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    if (_initialized_) return;

    // Fetch the application name:
    if ( argc_ >= 1 ) {
      _application_name_= argv_[0];
    }
    _params_.reset();

    // Parse command line options:
    namespace po = boost::program_options;
    po::options_description opts("datatools' kernel options ");
    build_opt_desc(opts, _params_);
    po::positional_options_description args;
    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options(opts) // Only options to be parsed.
      //.positional(args) // This crashes so we don't use it here !
      .allow_unregistered()
      .run();
    _params_.unrecognized_args = po::collect_unrecognized(parsed.options,
                                                          po::include_positional);
    po::store(parsed, vm);
    po::notify(vm);
    if (_params_.help) {
      print_opt_desc(opts, std::cout);
    }

    // Initialize internals:
    _initialize();

    _initialized_ = true;
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void kernel::shutdown()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    if (! _initialized_) return;
    _initialized_ = false;
    _shutdown();
    _params_.reset();
    DT_LOG_TRACE(_logging_, "Kernel has shutdown.");
    DT_LOG_TRACE_EXITING(_logging_);
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
                "The datatools kernel has no library info register !");
    return *_library_info_register_;
  }

  const library_info & kernel::get_library_info_register() const
  {
    DT_THROW_IF(!_library_info_register_, std::logic_error,
                "The datatools kernel has no library info register !");
    return *_library_info_register_;
  }

  bool kernel::has_variant_repository() const
  {
    return _variant_repository_;
  }

  const configuration::variant_repository & kernel::get_variant_repository() const
  {
    DT_THROW_IF(!_variant_repository_, std::logic_error,
                "The datatools kernel has no configuration variant repository !");
    return *_variant_repository_;
  }

  configuration::variant_repository & kernel::grab_variant_repository()
  {
    DT_THROW_IF(!_variant_repository_, std::logic_error,
                "The datatools kernel has no configuration variant repository !");
    return *_variant_repository_;
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
         << "Library info register : '"
         << (_activate_library_info_register_? "activated" : "not activated") << "'" <<std::endl;
    if (has_library_info_register()) {
      std::ostringstream indent2;
      indent2 << indent << i_tree_dumpable::skip_tag;
      get_library_info_register().tree_dump(out_, "", indent2.str());
    }

    out_ << indent << i_tree_dumpable::tag
         << "Configuration variant repository : '"
         << (_activate_variant_repository_? "activated" : "not activated") << "'" <<std::endl;
    if (has_variant_repository()) {
      std::ostringstream indent2;
      indent2 << indent << i_tree_dumpable::skip_tag;
      get_variant_repository().tree_dump(out_, "", indent2.str());
    }

    out_ << indent << i_tree_dumpable::inherit_tag(inherit_)
         << "Kernel instance at : " << _instance_ << std::endl;

    return;
  }

  void kernel::import_configuration_repository(configuration::variant_repository & rep_)
  {
    DT_THROW_IF(!has_variant_repository(), std::logic_error,
                "The datatools kernel has no configuration variant repository !");
    DT_THROW_IF(&rep_ == &grab_variant_repository(), std::logic_error,
                "The datatools kernel cannot import registries from itself !");
    if (grab_variant_repository().is_locked()) {
      grab_variant_repository().unlock();
    }
    grab_variant_repository().set_organization(rep_.get_organization());
    grab_variant_repository().set_application(rep_.get_application());
    import_configuration_registry(rep_);
    grab_variant_repository().lock();
    return;
  }

  void kernel::clear_configuration_repository()
  {
    DT_THROW_IF(!has_variant_repository(), std::logic_error,
                "The datatools kernel has no configuration variant repository !");
    if (grab_variant_repository().is_locked()) {
      grab_variant_repository().unlock();
    }
    grab_variant_repository().clear_registries();
    return;
  }

  void kernel::clear_imported_configuration_repository()
  {
    DT_THROW_IF(!has_variant_repository(), std::logic_error,
                "The datatools kernel has no configuration variant repository !");
    if (grab_variant_repository().is_locked()) {
      grab_variant_repository().unlock();
    }
    grab_variant_repository().external_registries_unregistration();
    return;
  }

  bool kernel::has_external_configuration_registry(const std::string & registry_name_) const
  {
    return get_variant_repository().has_registry(registry_name_)
      && get_variant_repository().get_registries().find(registry_name_)->second.is_external();
  }

  void kernel::clear_configuration_registry(const std::string & registry_name_)
  {
    configuration::variant_repository::registry_dict_type::iterator found =
      grab_variant_repository().grab_registries().find(registry_name_);
    DT_THROW_IF(found == grab_variant_repository().grab_registries().end(),
                std::logic_error,
                "No configuration registry named '" << registry_name_ << "'!");
    grab_variant_repository().grab_registries().erase(found);
    return;
  }

  void kernel::import_configuration_registry(configuration::variant_repository & rep_,
                                             const std::string & registry_name_)
  {
    DT_THROW_IF(!has_variant_repository(), std::logic_error,
                "The datatools kernel has no configuration variant repository !");
    DT_THROW_IF(&rep_ == &grab_variant_repository(), std::logic_error,
                "The datatools kernel cannot import registries from itself !");
    for (configuration::variant_repository::registry_dict_type::iterator i
           = rep_.grab_registries().begin();
         i != rep_.grab_registries().end();
         i++) {
      const std::string & reg_name = i->first;
      configuration::variant_registry & reg = i->second.grab_registry();
      bool import_it = false;
      if (registry_name_.empty()) {
        import_it = true;
      } else if (registry_name_ == reg_name) {
        import_it = true;
      }
      if (import_it) {
        grab_variant_repository().registration_external(reg);
      }
    }
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
