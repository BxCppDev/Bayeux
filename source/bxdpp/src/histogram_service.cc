// histogram_service.cc

// Standard Library:
#include <sstream>
#include <stdexcept>

// Third Party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
// - Root:
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-long-long"
#endif
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
// - Bayeux/datatools:
#include <datatools/io_factory.h>
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/ioutils.h>
#include <datatools/exception.h>
#include <datatools/logger.h>
// - Bayeux/mygsl:
#include <mygsl/mygsl_config.h>
#if MYGSL_WITH_BIO == 1
#include <mygsl/histogram_pool.ipp>
#endif

// This project:
#include <dpp/dpp_config.h>
#include <dpp/histogram_service.h>

namespace dpp {

  /// Auto-registration of this service class in a central service Db
  DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(histogram_service, "dpp::histogram_service")

  bool histogram_service::has_output_files() const
  {
    return ! _output_files_.empty();
  }

  const std::vector<std::string> & histogram_service::get_output_files() const
  {
    return _output_files_;
  }

  std::vector<std::string> & histogram_service::grab_output_files()
  {
    return _output_files_;
  }

  void histogram_service::add_output_file(const std::string & fn_)
  {
    _output_files_.push_back(fn_);
    return;
  }

  const mygsl::histogram_pool & histogram_service::get_pool() const
  {
    return _pool_;
  }

  mygsl::histogram_pool & histogram_service::grab_pool()
  {
    return _pool_;
  }

  histogram_service::histogram_service()
  {
    _initialized_ = false;
    _root_export_stats_ = false;
    return;
  }

  histogram_service::~histogram_service()
  {
    if(is_initialized()) {
      this->histogram_service::reset();
    }
    return;
  }

  bool histogram_service::is_initialized() const
  {
    return _initialized_;
  }

  int histogram_service::initialize(const datatools::properties  & config_,
                                    datatools::service_dict_type & /*service_dict_*/)
  {
    DT_THROW_IF(is_initialized(),std::logic_error,"Service '" << get_name() << "' is already initialized !");

    base_service::_common_initialize(config_);

    if(config_.has_key("output_files")) {
      std::vector<std::string> output_files;
      config_.fetch("output_files", output_files);
      for(size_t i = 0; i < output_files.size(); i++) {
        add_output_file(output_files[i]);
      }
    }

    if(config_.has_flag("root_export.stats")) {
      _root_export_stats_ = true;
    }

    if(_root_export_title_prefix_.empty()) {
      if(config_.has_key("root_export.title_prefix")) {
        _root_export_title_prefix_ = config_.fetch_string("root_export.title_prefix");
      }
    }

    if(_root_export_name_suffix_.empty()) {
      if(config_.has_key("root_export.name_suffix")) {
        _root_export_name_suffix_ = config_.fetch_string("root_export.name_suffix");
      }
    }

    datatools::properties pool_config;
    if(config_.has_key("pool_config")) {
      std::string pool_config_path = config_.fetch_string("pool_config");
      datatools::fetch_path_with_env(pool_config_path);
      datatools::read_config(pool_config_path, pool_config);
    } else {
      config_.export_and_rename_starting_with(pool_config, "pool.", "");
    }
    // Initialize the pool :
    _pool_.initialize(pool_config);

    _initialized_ = true;
    return EXIT_SUCCESS;
  }

  int histogram_service::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Service '" << get_name() << "' is not initialized !");

    _at_reset();

    if(_pool_.is_initialized()) {
      _pool_.reset();
    }
    _output_files_.clear();
    _root_export_title_prefix_.clear();
    _root_export_name_suffix_.clear();
    _initialized_ = false;
    return EXIT_SUCCESS;
  }

  void histogram_service::store_as_boost_file(const std::string & filename_) const
  {
    DT_LOG_NOTICE(get_logging_priority(), "Exporting histograms to Boost file '" << filename_ << "'...");
    DT_LOG_DEBUG(get_logging_priority(), "Cleaning histogram templates");
    std::vector<std::string> histo_names;
    _pool_.names(histo_names);
    for(size_t i = 0; i < histo_names.size(); i++) {
      const std::string & hname = histo_names[i];
      const std::string & hgroup = _pool_.get_group(hname);
      // If histogram group starts with '__' we consider it as 'private' and do
      // not export it. This trick is particularly useful when defining
      // 'template' histogram to avoid the export of these templates.
      const bool is_private_histogram = boost::starts_with(hgroup, "__");
      if(is_private_histogram) {
        mygsl::histogram_pool * pool = const_cast<mygsl::histogram_pool*>(&_pool_);
        pool->remove(hname);
      }
    }
    std::string fn = filename_;
    datatools::fetch_path_with_env(fn);
    datatools::data_writer writer(fn,
                                  datatools::using_multi_archives,
                                  datatools::no_append_mode);
    writer.store(_pool_);
    return;
  }

  void histogram_service::load_from_boost_file(const std::string & filename_)
  {
    DT_LOG_NOTICE(get_logging_priority(), "Loading histograms from Boost file '" << filename_ << "'...");
    std::string fn = filename_;
    datatools::fetch_path_with_env(fn);
    datatools::data_reader reader(fn, datatools::using_multi_archives);
    reader.load(_pool_);
    return;
  }

  // static
  void histogram_service::export_to_root(const mygsl::histogram_2d & h2d_,
                                         const std::string & name_,
                                         const std::string & title_,
                                         TH2D & rh2d_,
                                         bool stats_)
  {
    rh2d_.SetName(name_.c_str());

    if(! title_.empty()) {
      std::vector<std::string> splitted_title_lines;
      boost::algorithm::split(splitted_title_lines,
                              title_,
                              boost::algorithm::is_any_of("\n"),
                              boost::algorithm::token_compress_on );
      rh2d_.SetTitle(splitted_title_lines[0].c_str());
      for(size_t i = 1; i < splitted_title_lines.size(); i++) {
        rh2d_.SetTitle(Form("#splitline{%s}{%s}",
                            rh2d_.GetTitle(),
                            splitted_title_lines[i].c_str()));
      }
    }

    double xunit = 1.0;
    if(h2d_.get_auxiliaries().has_key("display.xaxis.unit")) {
      std::string xunit_str = h2d_.get_auxiliaries().fetch_string("display.xaxis.unit");
      std::string xunit_type;
      if(! datatools::units::find_unit(xunit_str, xunit, xunit_type)) {
        xunit = 1.0;
      }
    }
    double yunit = 1.0;
    if(h2d_.get_auxiliaries().has_key("display.yaxis.unit")) {
      std::string yunit_str = h2d_.get_auxiliaries().fetch_string("display.yaxis.unit");
      std::string yunit_type;
      if(! datatools::units::find_unit(yunit_str, yunit, yunit_type)) {
        yunit = 1.0;
      }
    }

    const size_t nxbins = h2d_.xbins();
    const size_t nybins = h2d_.ybins();
    {
      std::vector<double> xbins;
      xbins.assign(nxbins + 1, 0.0);
      for(size_t xbin_index = 0; xbin_index < nxbins; xbin_index++) {
        std::pair<double,double> xbin_range = h2d_.get_xrange(xbin_index);
        xbins[xbin_index] = xbin_range.first / xunit;
      }
      xbins[nxbins] = h2d_.get_xrange(nxbins - 1).second / xunit;
      std::vector<double> ybins;
      ybins.assign(nybins + 1, 0.0);
      for(size_t ybin_index = 0; ybin_index < nybins; ybin_index++) {
        std::pair<double,double> ybin_range = h2d_.get_yrange(ybin_index);
        ybins[ybin_index] = ybin_range.first / yunit;
      }
      ybins[nybins] = h2d_.get_yrange(nybins - 1).second / yunit;
      rh2d_.SetBins(nxbins, &xbins[0], nybins, &ybins[0]);
    }
    for(size_t xbin_index = 0; xbin_index < nxbins; xbin_index++) {
      for(size_t ybin_index = 0; ybin_index < nybins; ybin_index++) {
        double bin_content = h2d_.get(xbin_index, ybin_index);
        rh2d_.SetBinContent(xbin_index+1, ybin_index+1, bin_content);
      }
    }
    if(h2d_.are_underflow_overflow_available()) {
      rh2d_.SetBinContent(0, 0,               h2d_.underflow_x() + h2d_.underflow_y());
      rh2d_.SetBinContent(0, nybins+1,        h2d_.underflow_x() + h2d_.overflow_y());
      rh2d_.SetBinContent(nxbins+1, 0,        h2d_.overflow_x()  + h2d_.underflow_y());
      rh2d_.SetBinContent(nxbins+1, nybins+1, h2d_.overflow_x()  + h2d_.overflow_y());
    }
    if(h2d_.is_counts_available()) {
      rh2d_.SetEntries(h2d_.counts());
    } else {
    }
    rh2d_.SetStats(stats_);

    std::ostringstream axis_x_label_oss;
    std::ostringstream axis_y_label_oss;
    std::ostringstream axis_z_label_oss;
    if(h2d_.get_auxiliaries().has_key("display.xaxis.label")) {
      std::string axis_x_label = h2d_.get_auxiliaries().fetch_string("display.xaxis.label");
      if(! axis_x_label.empty()) {
        axis_x_label_oss << axis_x_label;
      }
      bool axis_x_label_with_unit = true;
      if(h2d_.get_auxiliaries().has_flag("display.xaxis.nounit")) {
        axis_x_label_with_unit = false;
      }
      if(axis_x_label_with_unit && h2d_.get_auxiliaries().has_key("display.xaxis.unit")) {
        axis_x_label_oss << " [" << h2d_.get_auxiliaries().fetch_string("display.xaxis.unit") << "]";
      }
    }
    if(h2d_.get_auxiliaries().has_key("display.yaxis.label")) {
      std::string axis_y_label = h2d_.get_auxiliaries().fetch_string("display.yaxis.label");
      if(! axis_y_label.empty()) {
        axis_y_label_oss << axis_y_label;
      }
      bool axis_y_label_with_unit = true;
      if(h2d_.get_auxiliaries().has_flag("display.yaxis.nounit")) {
        axis_y_label_with_unit = false;
      }
      if(axis_y_label_with_unit && h2d_.get_auxiliaries().has_key("display.yaxis.unit")) {
        axis_y_label_oss << " [" << h2d_.get_auxiliaries().fetch_string("display.yaxis.unit") << "]";
      }
    }
    if(h2d_.get_auxiliaries().has_key("display.zaxis.label")) {
      std::string axis_z_label = h2d_.get_auxiliaries().fetch_string("display.zaxis.label");
      if(! axis_z_label.empty()) {
        axis_z_label_oss << axis_z_label;
      }
    }
    if(! axis_x_label_oss.str().empty() ) {
      rh2d_.SetXTitle(axis_x_label_oss.str().c_str());
    }
    if(! axis_y_label_oss.str().empty() ) {
      rh2d_.SetYTitle(axis_y_label_oss.str().c_str());
    }
    if(! axis_z_label_oss.str().empty() ) {
      rh2d_.SetZTitle(axis_z_label_oss.str().c_str());
    }
    return;
  }

  // static
  void histogram_service::export_to_root(const mygsl::histogram_1d & h1d_,
                                         const std::string & name_,
                                         const std::string & title_,
                                         TH1D & rh1d_,
                                         bool stats_)
  {
    rh1d_.SetName(name_.c_str());

    if(! title_.empty()) {
      std::vector<std::string> splitted_title_lines;
      boost::algorithm::split(splitted_title_lines,
                              title_,
                              boost::algorithm::is_any_of("\n"),
                              boost::algorithm::token_compress_on );
      rh1d_.SetTitle(splitted_title_lines[0].c_str());
      for(size_t i = 1; i < splitted_title_lines.size(); i++) {
        rh1d_.SetTitle(Form("#splitline{%s}{%s}",
                            rh1d_.GetTitle(),
                            splitted_title_lines[i].c_str()));
      }
    }

    double xunit = 1.0;
    if(h1d_.get_auxiliaries().has_key("display.xaxis.unit")) {
      std::string xunit_str = h1d_.get_auxiliaries().fetch_string("display.xaxis.unit");
      std::string xunit_type;
      if(! datatools::units::find_unit(xunit_str, xunit, xunit_type)) {
        xunit = 1.0;
      }
    }
    const size_t nbins = h1d_.bins();
    {
      std::vector<double> bins;
      bins.assign(nbins + 1, 0.0);
      for(size_t bin_index = 0; bin_index < nbins; bin_index++) {
        std::pair<double,double> bin_range = h1d_.get_range(bin_index);
        bins[bin_index] = bin_range.first / xunit;
      }
      bins[nbins] = h1d_.get_range(nbins - 1).second / xunit;
      rh1d_.SetBins(nbins, &bins[0]);
    }
    for(size_t bin_index = 0; bin_index < nbins; bin_index++) {
      double bin_content = h1d_.get(bin_index);
      rh1d_.SetBinContent(bin_index+1, bin_content);
    }
    // Adding under/overflow following ROOT numbering conventions
    // http://root.cern.ch/root/html/TH1.html
    if(h1d_.are_underflow_overflow_available()) {
      rh1d_.SetBinContent(0, h1d_.underflow());
      rh1d_.SetBinContent(nbins+1, h1d_.overflow());
    }
    if(h1d_.is_counts_available()) {
      rh1d_.SetEntries(h1d_.counts());
    } else {
      // 2012-10-28 XG: Not sure what to do since SetStats
      //(true/false) just hide the 'stat' box in ROOT
      // histogram; Maybe invalidate value for counter i.e. -1
      // is also the best choice
    }
    rh1d_.SetStats(stats_);

    std::ostringstream axis_x_label_oss;
    std::ostringstream axis_y_label_oss;
    if(h1d_.get_auxiliaries().has_key("display.xaxis.label")) {
      std::string axis_x_label = h1d_.get_auxiliaries().fetch_string("display.xaxis.label");
      if(! axis_x_label.empty()) {
        axis_x_label_oss << axis_x_label;
      }
      bool axis_x_label_with_unit = true;
      if(h1d_.get_auxiliaries().has_flag("display.xaxis.nounit")) {
        axis_x_label_with_unit = false;
      }
      if(axis_x_label_with_unit && h1d_.get_auxiliaries().has_key("display.xaxis.unit")) {
        axis_x_label_oss << " [" << h1d_.get_auxiliaries().fetch_string("display.xaxis.unit") << "]";
      }
    }
    if(h1d_.get_auxiliaries().has_key("display.yaxis.label")) {
      std::string axis_y_label = h1d_.get_auxiliaries().fetch_string("display.yaxis.label");
      if(! axis_y_label.empty()) {
        axis_y_label_oss << axis_y_label;
      }
    }
    if(! axis_x_label_oss.str().empty() ) {
      rh1d_.SetXTitle(axis_x_label_oss.str().c_str());
    }
    if(! axis_y_label_oss.str().empty() ) {
      rh1d_.SetYTitle(axis_y_label_oss.str().c_str());
    }
    return;
  }

  void histogram_service::store_as_root_file(const std::string & filename_) const
  {
    DT_LOG_NOTICE(get_logging_priority(),"Exporting histograms to ROOT file '" << filename_ << "'...");
    std::string fn = filename_;
    datatools::fetch_path_with_env(fn);
    TFile orf(fn.c_str(), "RECREATE");
    std::vector<std::string> histo1d_names;
    std::vector<std::string> histo2d_names;
    _pool_.names(histo1d_names, "dim=1");
    _pool_.names(histo2d_names, "dim=2");

    DT_LOG_NOTICE(get_logging_priority(),"Exporting 1D-histograms...");
    for (size_t i = 0; i < histo1d_names.size(); i++) {
      const std::string & hname = histo1d_names[i];
      const mygsl::histogram_1d & h1d = _pool_.get_1d(hname);
      const std::string & htitle = _pool_.get_title(hname);
      const std::string & hgroup = _pool_.get_group(hname);
      std::ostringstream title_oss;
      // If histogram group starts with '__' we consider it as 'private' and do
      // not export it. This trick is particularly useful when defining
      // 'template' histogram to avoid the export of these templates.
      const bool is_private_histogram = boost::starts_with(hgroup, "__");
      if (is_private_histogram) continue;
      if (! _root_export_title_prefix_.empty()) {
        title_oss << _root_export_title_prefix_ << std::endl;
      }
      if (htitle.empty()) {
        title_oss << hname;
      } else {
        title_oss << htitle;
      }
      if (! hgroup.empty()) {
        title_oss << "(" << hgroup << ")";
      }
      // Create ROOT directory
      TDirectory * dir = &orf;
      if (! hgroup.empty()) {
        std::string a_dir = hgroup;
        std::replace(a_dir.begin(), a_dir.end(), ':', '_');
        dir = orf.GetDirectory(a_dir.c_str());
        if (! dir) {
          orf.mkdir(a_dir.c_str());
          // 2015-12-30 XG: the mkdir function returns the top directory
          // i.e. when using some hierarchical construction such as a/b/c then
          // mkdir function returns the 'a' directory. Here we get the directory
          // address of the deepest directory
          dir = orf.GetDirectory(a_dir.c_str());
        }
      }
      dir->cd();
      TH1D rootH1d;
      rootH1d.SetStats(_root_export_stats_);
      export_to_root(h1d, hname + _root_export_name_suffix_, title_oss.str(), rootH1d,
                     _root_export_stats_);
      rootH1d.Write();
    }
    DT_LOG_NOTICE(get_logging_priority(), "Exporting 2D-histograms...");
    for (size_t i = 0; i < histo2d_names.size(); i++) {
      const std::string & hname = histo2d_names[i];
      const mygsl::histogram_2d & h2d = _pool_.get_2d(hname);
      const std::string & htitle = _pool_.get_title(hname);
      const std::string & hgroup = _pool_.get_group(hname);
      const bool is_private_histogram = boost::starts_with(hgroup, "__");
      if(is_private_histogram) continue;
      std::ostringstream title_oss;
      if(! _root_export_title_prefix_.empty()) {
        title_oss << _root_export_title_prefix_ << std::endl;
      }
      if(htitle.empty()) {
        title_oss << hname;
      } else {
        title_oss << htitle;
      }
      if(! hgroup.empty()) {
        title_oss << "(" << hgroup << ")";
      }
      // Create ROOT directory
      TDirectory * dir = &orf;
      if (! hgroup.empty()) {
        std::string a_dir = hgroup;
        std::replace(a_dir.begin(), a_dir.end(), ':', '_');
        dir = orf.GetDirectory(a_dir.c_str());
        if (! dir) {
          orf.mkdir(a_dir.c_str());
          // 2015-12-30 XG: the mkdir function returns the top directory
          // i.e. when using some hierarchical construction such as a/b/c then
          // mkdir function returns the 'a' directory. Here we get the directory
          // address of the deepest directory
          dir = orf.GetDirectory(a_dir.c_str());
        }
      }
      dir->cd();
      TH2D rootH2d;
      rootH2d.SetStats(_root_export_stats_);
      export_to_root(h2d, hname + _root_export_name_suffix_, title_oss.str(), rootH2d,
                     _root_export_stats_);
      rootH2d.Write();
    }

    orf.Close();
    return;
  }

  void histogram_service::_at_init()
  {
    return;
  }

  void histogram_service::_at_reset()
  {
    if(!has_output_files()) {
      DT_LOG_NOTICE(get_logging_priority(),"No output file is requested !");
      return;
    }

    for(size_t i = 0; i < _output_files_.size(); ++i) {
      std::string fn = _output_files_.at(i);
      datatools::fetch_path_with_env(fn);

      boost::filesystem::path pth(fn);
      if(pth.extension() == ".root") {
        store_as_root_file(fn);
      } else if(pth.extension() == ".brio") {
        DT_LOG_ERROR(get_logging_priority(),"BRIO format is not supported !");
      } else if(pth.extension() == ".trio") {
        DT_LOG_ERROR(get_logging_priority(),"TRIO format is not supported !");
      } else {
        int mode_guess;
        if(datatools::io_factory::guess_mode_from_filename(fn, mode_guess)
           == datatools::io_factory::SUCCESS) {
          store_as_boost_file(fn);
        } else {
          DT_THROW_IF(true,std::logic_error,"Cannot guess mode for file '" << fn << "' !");
        }
      }
    }
    return;
  }

  void histogram_service::tree_dump(std::ostream & out_ ,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
  {
    namespace du = datatools;
    this->datatools::base_service::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << du::i_tree_dumpable::inherit_tag(inherit_)
         << "Output files : ";
    if(_output_files_.empty()) out_ << "<empty>";
    else                         out_ << _output_files_.size();
    out_ << std::endl;
    for(std::vector<std::string>::const_iterator
          i = _output_files_.begin();
        i != _output_files_.end(); ++i) {
      std::vector<std::string>::const_iterator j = i;
      out_ << indent_ << du::i_tree_dumpable::skip_tag;
      if(++j == _output_files_.end()) out_ << du::i_tree_dumpable::last_tag;
      else                              out_ << du::i_tree_dumpable::tag;
      out_ << *i << std::endl;
    }

    return;
  }

}  // end of namespace dpp

/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::dpp::histogram_service' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::dpp::histogram_service,ocd_)
{
  ocd_.set_class_name("dpp::histogram_service");
  ocd_.set_class_description("A service of histograms");
  ocd_.set_class_library("dpp");
  ocd_.set_class_documentation("A service that hosts a pool of histograms");

  // Invoke OCD support from parent class :
  ::datatools::base_service::common_ocd(ocd_);

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("pool_config")
      .set_terse_description("The name of the histogram pool configuration file")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description("Example:                                                   \n"
                            "  |                                                        \n"
                            "  | pool_config : string as path = \"histos_pool.conf\"    \n"
                            "  |                                                        \n"
                            "See dedicated OCD support for class 'dpp::histogram_pool'. \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("output_files")
      .set_terse_description("A list of filenames where to store the histograms when the service is closed")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description("Extends former calls of :                                      \n"
                            "  dpp::histogram_service::add_output_file(...)                 \n"
                            "Supported formats are :                                        \n"
                            " * ROOT(with extension '.root')                               \n"
                            " * BRIO(with extension '.brio')                               \n"
                            " * TRIO(with extension '.trio')                               \n"
                            " * datatools Boost/archive :                                   \n"
                            "   - text  (with extension '.txt' or '.txt.gz')               \n"
                            "   - XML   (with extension '.xml' or '.xml.gz')               \n"
                            "   - binary(with extension '.data' or '.data.gz')             \n"
                            "Example :                                                      \n"
                            "  |                                                            \n"
                            "  | output_files : string[2] = \"histos.data\" \"histos.root\" \n"
                            "  |                                                            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("root_export.stats")
      .set_terse_description("Flag to save ROOT statistics")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("If the histograms are saved in a ROOT file,                     \n"
                            "this flag activates the histograms' statistics box within ROOT. \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("root_export.title_prefix")
      .set_terse_description("A title prefix for all ROOT exported histograms")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("If the histograms are saved in a ROOT file,       \n"
                            "this string is prepend to all histograms' titles. \n"
                            )
      ;
  }

  ocd_.set_configuration_hints("The histogram service uses a 'datatools::properties' object     \n"
                               "to initialize its behaviour and contents.                       \n"
                               "                                                                \n"
                               "If the 'pool_config' path property is not set, all properties   \n"
                               "the name of which starts with the prefix \"pool.\" are used     \n"
                               "to initialize the embeded histogram pool object.                \n"
                               "Example:                                                        \n"
                               "  |                                                             \n"
                               "  | pool.description : string = \"Histograms\"                  \n"
                               "  | pool.histo.setups : string[2] = \\                          \n"
                               "  |   \"${DPP_TESTING_DIR}/config/test_histos_1.conf\" \\       \n"
                               "  |   \"${DPP_TESTING_DIR}/config/test_histos_2.conf\"          \n"
                               "  | pool.histo.export_prefixes : string[1] = \\                 \n"
                               "  |   \"value.\" \\                                             \n"
                               "  |                                                             \n"
                               "See dedicated OCD support for class 'mygsl::histogram_pool' for \n"
                               "the description of histogram builing rules.                     \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(::dpp::histogram_service,"dpp::histogram_service")

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
