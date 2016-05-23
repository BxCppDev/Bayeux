// kernel.cc

// Standard Library:
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <map>
#include <string>
#include <cstring>
#include <clocale>

// Ourselves:
#include <datatools/kernel.h>

// Third Party:
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

// This project:
#include <datatools/library_info.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/io.h>
#include <datatools/configuration/utils.h>
#include <datatools/exception.h>
#include <datatools/version.h>
#include <datatools/command_utils.h>
#include <datatools/utils.h>
#include <datatools/logger.h>

#if DATATOOLS_WITH_QT_GUI == 1
#include <QStyleFactory>
#include <QApplication>
#include <datatools/qt/interface.h>
#include <datatools/configuration/ui/variant_repository_dialog.h>
#endif // DATATOOLS_WITH_QT_GUI == 1

namespace datatools {

  // static
  kernel * kernel::_instance_ = 0;

#if DATATOOLS_WITH_QT_GUI == 1
  // static
  bool kernel::is_qt_gui_activated() const
  {
    return _activate_qt_gui_;
  }
#endif // DATATOOLS_WITH_QT_GUI == 1

  int kernel::get_argc() const
  {
    return _argc_;
  }

  char ** kernel::get_argv() const
  {
    return _argv_;
  }

  char * kernel::get_argv(int i_) const
  {
    DT_THROW_IF(i_ < 0 || i_ >= _argc_, std::range_error,
                "Invalid command line argument index!");
    return _argv_[i_];
  }

  bool kernel::is_library_info_register_activated() const
  {
    return _activate_library_info_register_;
  }

  bool kernel::is_variant_repository_activated() const
  {
    return _activate_variant_repository_;
  }

  void kernel::_set_defaults()
  {
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

    _locale_category_ = "";
    {
      std::string lc;
      char * e = getenv("DATATOOLS_KERNEL_LC");
      if (e) {
        lc = e;
      }
      if (lc.empty()) {
        lc  = "C";
      }
      set_locale_category(lc);
    }

    _argc_ = 0;
    _argv_ = NULL;
    _activate_variant_repository_ = true;
    _activate_library_info_register_ = true;
#if DATATOOLS_WITH_QT_GUI == 1
    _activate_qt_gui_ = true;
    {
      char * e = getenv("DATATOOLS_KERNEL_NO_QT_GUI");
      if (e) {
        std::string dknoqtgui = e;
        _activate_qt_gui_ = false;
      }
    }
#endif // DATATOOLS_WITH_QT_GUI == 1
    _activate_library_info_register_ = true;
    _activate_variant_repository_ = true;
    return;
  }

  // basic construction
  void kernel::_construct_()
  {
    _set_defaults();
    DT_THROW_IF(_instance_, std::runtime_error,
                "An instance of the datatools kernel already exists !");
    DT_LOG_TRACE(_logging_, "Kernel is now constructed.");
    _instance_ = this;
    return;
  }

  kernel::kernel()
  {
    _initialized_ = false;
    _construct_();
    return;
  }

  kernel::kernel(int argc_, char * argv_[])
  {
    _initialized_ = false;
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

      ("datatools::locale-category",
       po::value<std::string>(&params_.locale_category)
       ->value_name("locale"),
       "Set the datatools kernel's local category.              \n"
       "Example :                                               \n"
       "  --datatools::locale-category=\"C\"                      "
       )

      ("datatools::logging",
       po::value<std::string>(&params_.logging_label)
       ->value_name("level")
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
       ->value_name("level")
       ->default_value("warning"),
       "Set the datatools kernel's library info logging priority threshold.  \n"
       "Example :                                                            \n"
       "  --datatools::libinfo::logging=\"trace\"                              "
       )

      ("datatools::resource-path",
       po::value< std::vector<std::string> >(&params_.resource_paths)
       ->value_name("rule"),
       "Register a resource path associated to a library or module.          \n"
       "Example :                                                            \n"
       "  Register the \"path1/subdir1\" path as the root directory of       \n"
       "  the \"foo\" library and the \"path2/subdir2\" path as the          \n"
       "  root directory of the \"bar\" software module:                     \n"
       "    --datatools::resource-path=\"foo@path1/subdir1\"                 \n"
       "    --datatools::resource-path=\"bar@path2/subdir2\"                   "
       )

      ("datatools::resource_path",
       po::value< std::vector<std::string> >()
       ->value_name("rule"),
       "Deprecated option, please use '--datatools::resource-path'    \n"
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
       po::value< std::string >(&params_.variant_config)
       ->value_name("file"),
       "The system variant repository configuration filename.                \n"
       "Example :                                                            \n"
       "  Register the \"config/variance.conf\" file to initialize the       \n"
       "  repository for some application:                                   \n"
       "    --datatools::variant-config=\"config/variance.conf\"               "
       )

      ("datatools::variant-registry-config",
       po::value< std::vector<std::string> >(&params_.variant_registry_configs)
      ->value_name("file(s)"),
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
       po::value<std::string>(&params_.variant_load)
       ->value_name("file"),
       "Load the values of the variant parameters from a file.               \n"
       "Example :                                                            \n"
       "  Load parameters' values from the \"config/saved_variants.rep\" file\n"
       "    --datatools::variant-load=\"config/saved_variants.rep\"            "
       )

      ("datatools::variant-set",
       po::value< std::vector<std::string> >(&params_.variant_sets)
       ->value_name("rule(s)"),
       "Set the values of a set of configuration variant parameters associated  \n"
       "to some registry.                                                       \n"
       "Example :                                                               \n"
       "  Set the \"foo:detector0=1\" flag and the associated                   \n"
       "  \"foo:detector0/if_detector/material\" and                            \n"
       "  \"foo:detector0/if_detector/width\" parameters                        \n"
       "  in the proper variant registry:                                       \n"
       "    --datatools::variant-set=\"foo:detector0=1\"                        \n"
       "    --datatools::variant-set=\"foo:detector0/if_detector/material=Silicium\" \n"
       "    --datatools::variant-set=\"foo:detector0/if_detector/width=3 cm\"        "
       )

#if DATATOOLS_WITH_QT_GUI == 1
      ("datatools::inhibit-qt-gui",
       po::value<bool>(&params_.inhibit_qt_gui)
       ->zero_tokens()
       ->default_value(false),
       "Inhibit Qt GUI:                                                     \n"
       "Example :                                                           \n"
       "   --datatools::inhibit-qt-gui                                        "
       )
#endif // DATATOOLS_WITH_QT_GUI == 1

#if DATATOOLS_WITH_QT_GUI == 1
      ("datatools::variant-qt-gui",
       po::value<bool>(&params_.variant_qt_gui)
       ->zero_tokens()
       ->default_value(false),
       "Display the variant Qt GUI dialog:                                  \n"
       "Example :                                                           \n"
       "   --datatools::variant-qt-gui                                        "
       )

      ("datatools::variant-qt-gui-style",
       po::value<std::string>(&params_.variant_qt_gui_style)
       ->value_name("style"),
       "Set the style of variant Qt GUI dialog:                             \n"
       "Example :                                                           \n"
       "   --datatools::variant-qt-gui-style=\"plastique\"                    "
       )
       //      ->default_value(std::string("plastique")),
#endif // DATATOOLS_WITH_QT_GUI == 1

      ("datatools::variant-store",
       po::value<std::string>(&params_.variant_store)
       ->value_name("file"),
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

#if DATATOOLS_WITH_QT_GUI == 1
    out_ << indent_ << "Inhibit Qt GUI: " << inhibit_qt_gui << std::endl;
#endif // DATATOOLS_WITH_QT_GUI == 1

    out_ << indent_ << "Resource paths: '" << resource_paths.size() << "'"  << std::endl;

    out_ << indent_ << "Variant config: '" << variant_config << "'" << std::endl;

    out_ << indent_ << "Variant registry configs: " << variant_registry_configs.size() << std::endl;

    out_ << indent_ << "Variant load: '" << variant_load << "'"  << std::endl;

    out_ << indent_ << "Variant store: '" << variant_store << "'"  << std::endl;

    out_ << indent_ << "Variant sets: " << variant_sets.size() << std::endl;

    out_ << indent_ << "Splash: " << splash << std::endl;

#if DATATOOLS_WITH_QT_GUI == 1
    out_ << indent_ << "Variant Qt GUI: " << variant_qt_gui << std::endl;
    out_ << indent_ << "Variant Qt GUI style: '" << variant_qt_gui_style << "'" << std::endl;
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
    locale_category.clear();
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
    inhibit_qt_gui = false;
    variant_qt_gui = false;
    variant_qt_gui_style = "plastique";
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
                                              "activated" : "no") << "\n";
    out_ << "   Configuration variant repository : " << (kern.has_variant_repository()?
                                              "activated" : "no") << "\n";
    if (kern.has_library_info_register()) {
      // On board registered libraries...
    }
    if (kern.has_variant_repository()) {
      // On board variant repository...
    }
    out_ << "\n";
    return;
  }

  bool kernel::has_locale_category() const
  {
    return !_locale_category_.empty();
  }

  const std::string & kernel::get_locale_category() const
  {
    return _locale_category_;
  }

  void kernel::set_locale_category(const std::string & lc_)
  {
    _locale_category_ = lc_;
    if (! _locale_category_.empty()) {
      // See also: http://bastian.rieck.ru/blog/posts/2013/qapplication_and_the_locale/
      std::setlocale(LC_ALL, _locale_category_.c_str());
      std::setlocale(LC_NUMERIC, _locale_category_.c_str());
      setenv("LANG", _locale_category_.c_str(), 1);
      setenv("LC_NUMERIC", _locale_category_.c_str(), 1);
      setenv("LC_ALL", _locale_category_.c_str(), 1);
    } else {
      std::setlocale(LC_ALL, "");
    }
    return;
  }

  void kernel::_initialize()
  {
    DT_LOG_TRACE_ENTERING(_logging_);

    if (! _params_.locale_category.empty()) {
      set_locale_category(_params_.locale_category);
    }

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

#if DATATOOLS_WITH_QT_GUI == 1
    if (_params_.inhibit_qt_gui) {
      DT_LOG_TRACE(_logging_, "Inhibit the Qt based GUI.");
      this->_activate_qt_gui_ = false;
    } else {
      DT_LOG_TRACE(_logging_, "The Qt based GUI is activated.");
    }
#endif // DATATOOLS_WITH_QT_GUI == 1

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
    DT_LOG_TRACE(_logging_, "Kernel's library info register is now destroyed.");

    _unrecognized_args_.clear();
    for (int i = 0; i < _argc_; i++) {
      if (_argv_[i]) {
        free(_argv_[i]);
      }
    }
    if (_argv_) {
      free(_argv_);
    }
    _application_name_.clear();
    _locale_category_.clear();

    // // Restore the local environment:
    // set_locale_category("");

    // Revert to default idle status:
    _set_defaults();

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

#if DATATOOLS_WITH_QT_GUI == 1
  struct run_variant_repository_gui
  {
    /// Constructor
    run_variant_repository_gui(::datatools::configuration::variant_repository & vrep_,
                               const std::string & gui_style_ = "plastique")
      : _vrep_(&vrep_), _gui_style_(gui_style_)
    {
      return;
    }
    /// Running method
    void operator()()
    {
      // See also: http://qt-project.org/wiki/boost_thread_qt_application
      //DT_LOG_TRACE_ENTERING(_logging_);

      datatools::qt::interface::instance();
      // QApplication::setStyle(QStyleFactory::create(QString::fromStdString(_gui_style_)));
      const kernel & krnl = kernel::const_instance();
      datatools::qt::interface & iqt = datatools::qt::interface::instance(krnl.get_argc(),
                                                                          krnl.get_argv(),
                                                                          krnl.get_application_name().c_str());
      DT_THROW_IF(!iqt.is_initialized(),
                  std::runtime_error,
                  "Qt is not initialized!");
      if (krnl.has_locale_category()) {
        std::setlocale(LC_NUMERIC, krnl.get_locale_category().c_str());
      }
      DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "Qt is initialized...");
      datatools::configuration::ui::variant_repository_dialog vrep_dialog(*_vrep_);
      int ret = vrep_dialog.exec();
      if (ret == QDialog::Rejected) {
        DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Variant repository dialog was rejected!");
      }
      /*
      datatools::configuration::ui::variant_repository_dialog * vrep_dialog
        = new datatools::configuration::ui::variant_repository_dialog(*_vrep_);
      vrep_dialog->setAttribute(Qt::WA_DeleteOnClose);
      int ret = vrep_dialog->exec();
      // if (ret == QDialog::Accepted) {
      //   DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "QDialog::Accepted");
      // } else  {
      //   DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "QDialog::Rejected");
      // }
      vrep_dialog->close();
      // Questionnable !
      //delete vrep_dialog;
      */
      // DT_LOG_TRACE_EXITING(datatools::logger::PRIO_ALWAYS);
      return;
    }
    ::datatools::configuration::variant_repository * _vrep_; //!< Handle to the variant repository
    std::string _gui_style_;
  };
#endif // DATATOOLS_WITH_QT_GUI == 1

  void kernel::register_configuration_variant_registries()
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    if (_variant_repository_) {

      bool rep_locked = false;
      // Initialize the system variant repository from a configuration file:
      if (! _params_.variant_config.empty()) {
        std::string config_filename =_params_.variant_config;
        datatools::properties config;
        DT_LOG_TRACE(_logging_, "Variant configuration file '"
                     << config_filename << "'...");
        uint32_t reader_opts = 0;
        reader_opts |= multi_properties::config::FORBID_VARIANTS;
        datatools::properties::config reader(reader_opts);
        reader.read(config_filename, config);
        // _variant_repository_->set_organization("");
        // _variant_repository_->set_application("");
        _variant_repository_->unlock();
        if (!_variant_repository_->has_organization()) {
          // Fetch organization:
          if (config.has_key("organization")) {
            const std::string & org = config.fetch_string("organization");
            _variant_repository_->set_organization(org);
          }
        }
        if (!_variant_repository_->has_application()) {
          if (config.has_key("application")) {
            const std::string & app = config.fetch_string("application");
            _variant_repository_->set_application(app);
          }
        }
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
        unsigned int ioflags = datatools::configuration::ascii_io::IO_DEFAULT;
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
        for (size_t i = 0; i < variant_sets.size(); i++) {
          DT_LOG_TRACE(_logging_, "Unordered variant parameter set : '" << variant_sets[i] << "'");
        }

        configuration::variant_parameter_set_comparator comp(*_variant_repository_);
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
      if (!_params_.inhibit_qt_gui) {
        //
      }
#endif // DATATOOLS_WITH_QT_GUI == 1

#if DATATOOLS_WITH_QT_GUI == 1
      if (_params_.variant_qt_gui) {
        DT_LOG_TRACE(_logging_, "Running Qt-based GUI for variant...");
        if (_activate_qt_gui_) {
          bool save_locked = _variant_repository_->is_locked();
          if (!save_locked) {
            _variant_repository_->lock();
          }
          run_variant_repository_gui RVRG(*_variant_repository_, _params_.variant_qt_gui_style);
          RVRG();
          if (!save_locked) {
            _variant_repository_->unlock();
          }
        } else{
          DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS, "GUI is inhibited!");
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
        unsigned int ioflags = datatools::configuration::ascii_io::IO_DEFAULT;
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
      _params_.inhibit_qt_gui = false;
      _params_.variant_qt_gui = false;
      _params_.variant_qt_gui_style.clear();
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

    // Local copy of the command line arguments:
    _argc_ = argc_;
    _argv_ = (char **) malloc( (argc_ + 1) * sizeof(char *));
    for (int i = 0; i < argc_; i++) {
      _argv_[i] = (char *) malloc((std::strlen(argv_[i]) + 2) * sizeof(char));
      std::strcpy(_argv_[i], argv_[i]);
    }
    _argv_[argc_] = 0; //'\0';

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
      .allow_unregistered()
      .run();
    _unrecognized_args_ = po::collect_unrecognized(parsed.options,
                                                   po::include_positional);
    po::store(parsed, vm);
    po::notify(vm);

    if (_params_.help) {
      print_opt_desc(opts, std::cout);
    }

    // Check:
    if (vm.count("datatools::resource_path")) {
      DT_THROW(std::logic_error,
               "The datatools kernel option '--datatools::resource_path {}' is not supported anymore!"
               << " Please use '--datatools::resource-path {}'!");
    }

    // Initialize internals:
    _initialize();

    _initialized_ = true;
    if (_logging_ == datatools::logger::PRIO_TRACE) {
      this->tree_dump(std::cerr, "Kernel: ", "trace: ");
    }
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
    return _library_info_register_.get() != 0;
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
    return _variant_repository_.get() != 0;
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
         << "Logging   : '"
         << datatools::logger::get_priority_label(_logging_) << "'"
         << std::endl;

    out_ << indent << i_tree_dumpable::tag
         << "Locale category : '" << _locale_category_ << "'" << std::endl;

    out_ << indent << i_tree_dumpable::tag
         << "Application name  : '" << _application_name_ << "'" << std::endl;

   out_ << indent << i_tree_dumpable::tag
        << "Command line argument count : " << _argc_  << std::endl;

   out_ << indent << i_tree_dumpable::tag
        << "Command line arguments : ";
   if (_argc_ == 0) out_ << "<none>";
   out_ << std::endl;
   for (int i = 0; i < _argc_; i++) {
     out_ << indent << i_tree_dumpable::skip_tag;
     if (i < _argc_ - 1) {
       out_ << i_tree_dumpable::tag;
     } else {
       out_ << i_tree_dumpable::last_tag;
     }
     out_ << "Argument #" << i << " : '" << _argv_[i] << "'" << std::endl;
   }

    out_ << indent << i_tree_dumpable::tag
         << "Library info register : "
         << (_activate_library_info_register_ ? "<activated>" : "<not activated>") << std::endl;
    if (has_library_info_register()) {
      std::ostringstream indent2;
      indent2 << indent << i_tree_dumpable::skip_tag;
      get_library_info_register().tree_dump(out_, "", indent2.str());
    }

    out_ << indent << i_tree_dumpable::tag
         << "Configuration variant repository : "
         << (_activate_variant_repository_ ? "<activated>" : "<not activated>") << std::endl;
    if (has_variant_repository()) {
      std::ostringstream indent2;
      indent2 << indent << i_tree_dumpable::skip_tag;
      get_variant_repository().tree_dump(out_, "", indent2.str());
    }

#if DATATOOLS_WITH_QT_GUI == 1
    out_ << indent << i_tree_dumpable::tag
         << "Qt GUI : "
         << (_activate_qt_gui_ ? "<activated>" : "<not activated>") << std::endl;
#endif // DATATOOLS_WITH_QT_GUI == 1

    out_ << indent << i_tree_dumpable::inherit_tag(inherit_)
         << "Kernel singleton instance at : " << _instance_ << std::endl;

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

  bool kernel::has_configuration_registries() const
  {
    if (has_variant_repository()) {
      return get_variant_repository().get_registries().size() > 0;
    }
    return false;
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
        int rank  = -1;
        if (rep_.is_ranked(reg_name)) {
          rank = rep_.get_rank(reg_name);
        }
        grab_variant_repository().registration_external(reg, reg_name, rank);
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
        // Allocate the new global kernel and initialize it:
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
