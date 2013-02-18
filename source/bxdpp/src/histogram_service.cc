/* histogram_service.cc */

#include <sstream>
#include <stdexcept>

#include <dpp/histogram_service.h>

#include <boost/filesystem.hpp>

#include <dpp/dpp_config.h>

#if DPP_DATATOOLS_LEGACY == 1
#include <datatools/serialization/io_factory.h>
#include <datatools/utils/utils.h>
#include <datatools/utils/units.h>
#include <datatools/utils/ioutils.h>
#else
#include <datatools/io_factory.h>
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/ioutils.h>
#endif

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

namespace dpp {

  /** Auto-registration of this service class in a central service Db */
  DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(histogram_service,
                                           "dpp::histogram_service")

  bool histogram_service::has_output_files () const
  {
    return ! _output_files_.empty ();
  }

  const std::vector<std::string> & histogram_service::get_output_files () const
  {
    return _output_files_;
  }

  std::vector<std::string> & histogram_service::grab_output_files ()
  {
    return _output_files_;
  }

  void histogram_service::add_output_file (const std::string & fn_)
  {
    _output_files_.push_back (fn_);
    return;
  }

  const mygsl::histogram_pool & histogram_service::get_pool () const
  {
    return _pool_;
  }

  mygsl::histogram_pool & histogram_service::grab_pool ()
  {
    return _pool_;
  }

  histogram_service::histogram_service ()
    : DPP_DS::base_service ("dpp::histogram_service",
                            "A histogramming service")
  {
    _initialized_ = false;
    return;
  }

  histogram_service::~histogram_service ()
  {
    if (is_initialized ())
      {
        this->histogram_service::reset ();
      }
    return;
  }

  bool histogram_service::is_initialized () const
  {
    return _initialized_;
  }

  int histogram_service::initialize (const DPP_DU::properties    & config_,
                                     datatools::service::service_dict_type & service_dict_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::histogram_service::initialize: "
                << "Service '" << get_name () << "' is already initialized ! ";
        throw std::logic_error (message.str ());
      }

    if (config_.has_key ("output_files"))
      {
        config_.fetch ("output_files", _output_files_);
      }

    // Initialize the pool :
    DPP_DU::properties pool_setup;
    config_.export_and_rename_starting_with (pool_setup, "pool.", "");
    _pool_.initialize (pool_setup);

    _initialized_ = true;
    return EXIT_SUCCESS;
  }

  int histogram_service::reset ()
  {
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::histogram_service::reset: "
                << "Service '" << get_name () << "' is not initialized ! ";
        throw std::logic_error (message.str ());
      }

    _at_reset ();

    if (_pool_.is_initialized ())
      {
        _pool_.reset ();
      }
    _output_files_.clear ();
    _initialized_ = false;
    return EXIT_SUCCESS;
  }

  void histogram_service::store_as_boost_file (const std::string & filename_) const
  {
    std::clog << DPP_DU::io::notice
              << "dpp::histogram_service::store_as_root_file: "
              << "Exporting histograms to Boost file '" << filename_ << "'..." << std::endl;
    std::string fn = filename_;
    DPP_DU::fetch_path_with_env (fn);

    namespace ds = DPP_DSZ;
    ds::data_writer writer (fn,
                            ds::using_multi_archives,
                            ds::no_append_mode);
    writer.store (_pool_);

    return;
  }

  // static
  void histogram_service::export_to_root (const mygsl::histogram_2d & h2d_,
                                          const std::string & name_,
                                          const std::string & title_,
                                          TH2D & rh2d_)
  {
    //h2d_.tree_dump (std::clog, "export H2D... ", "===========> ");
    rh2d_.SetName (name_.c_str ());
    rh2d_.SetTitle (title_.c_str ());
    double xunit = 1.0;
    if (h2d_.get_auxiliaries ().has_key ("display.xaxis.unit"))
      {
        std::string xunit_str = h2d_.get_auxiliaries ().fetch_string ("display.xaxis.unit");
        std::string xunit_type;
        if (! DPP_DU::units::find_unit(xunit_str, xunit, xunit_type))
          {
            xunit = 1.0;
          }
      }
    double yunit = 1.0;
    if (h2d_.get_auxiliaries ().has_key ("display.yaxis.unit"))
      {
        std::string yunit_str = h2d_.get_auxiliaries ().fetch_string ("display.yaxis.unit");
        std::string yunit_type;
        if (! DPP_DU::units::find_unit(yunit_str, yunit, yunit_type))
          {
            yunit = 1.0;
          }
      }

    const size_t nxbins = h2d_.xbins ();
    const size_t nybins = h2d_.ybins ();
    {
      std::vector<double> xbins;
      xbins.assign (nxbins + 1, 0.0);
      for (int xbin_index = 0; xbin_index < nxbins; xbin_index++)
        {
          std::pair<double,double> xbin_range = h2d_.get_xrange (xbin_index);
          xbins[xbin_index] = xbin_range.first / xunit;
        }
      xbins[nxbins] = h2d_.get_xrange (nxbins - 1).second / xunit;
      std::vector<double> ybins;
      ybins.assign (nybins + 1, 0.0);
      for (int ybin_index = 0; ybin_index < nybins; ybin_index++)
        {
          std::pair<double,double> ybin_range = h2d_.get_yrange (ybin_index);
          ybins[ybin_index] = ybin_range.first / yunit;
        }
      ybins[nybins] = h2d_.get_yrange (nybins - 1).second / yunit;
      rh2d_.SetBins (nxbins, &xbins[0], nybins, &ybins[0]);
    }
    for (int xbin_index = 0; xbin_index < nxbins; xbin_index++)
      {
        for (int ybin_index = 0; ybin_index < nybins; ybin_index++)
          {
            double bin_content = h2d_.get (xbin_index, ybin_index);
            rh2d_.SetBinContent (xbin_index+1, ybin_index+1, bin_content);
          }
      }
    if (h2d_.are_underflow_overflow_available ())
      {
        rh2d_.SetBinContent (0, 0,               h2d_.underflow_x () + h2d_.underflow_y ());
        rh2d_.SetBinContent (0, nybins+1,        h2d_.underflow_x () + h2d_.overflow_y ());
        rh2d_.SetBinContent (nxbins+1, 0,        h2d_.overflow_x ()  + h2d_.underflow_y ());
        rh2d_.SetBinContent (nxbins+1, nybins+1, h2d_.overflow_x ()  + h2d_.overflow_y ());
      }
    if (h2d_.is_counts_available ())
      {
        rh2d_.SetEntries (h2d_.counts ());
      }
    else
      {
        rh2d_.SetStats (false);
      }

    std::ostringstream axis_x_label_oss;
    std::ostringstream axis_y_label_oss;
    std::ostringstream axis_z_label_oss;
    if (h2d_.get_auxiliaries ().has_key ("display.xaxis.label"))
      {
        std::string axis_x_label = h2d_.get_auxiliaries ().fetch_string ("display.xaxis.label");
        if (! axis_x_label.empty ())
          {
            axis_x_label_oss << axis_x_label;
          }
        bool axis_x_label_with_unit = true;
        if (h2d_.get_auxiliaries ().has_flag ("display.xaxis.nounit"))
          {
            axis_x_label_with_unit = false;
          }
        if (axis_x_label_with_unit && h2d_.get_auxiliaries ().has_key ("display.xaxis.unit"))
          {
            axis_x_label_oss << " [" << h2d_.get_auxiliaries ().fetch_string ("display.xaxis.unit") << "]";
          }
      }
    if (h2d_.get_auxiliaries ().has_key ("display.yaxis.label"))
      {
        std::string axis_y_label = h2d_.get_auxiliaries ().fetch_string ("display.yaxis.label");
        if (! axis_y_label.empty ())
          {
            axis_y_label_oss << axis_y_label;
          }
        bool axis_y_label_with_unit = true;
        if (h2d_.get_auxiliaries ().has_flag ("display.yaxis.nounit"))
          {
            axis_y_label_with_unit = false;
          }
        if (axis_y_label_with_unit && h2d_.get_auxiliaries ().has_key ("display.yaxis.unit"))
          {
            axis_y_label_oss << " [" << h2d_.get_auxiliaries ().fetch_string ("display.yaxis.unit") << "]";
          }
      }
    if (h2d_.get_auxiliaries ().has_key ("display.zaxis.label"))
      {
        std::string axis_z_label = h2d_.get_auxiliaries ().fetch_string ("display.zaxis.label");
        if (! axis_z_label.empty ())
          {
            axis_z_label_oss << axis_z_label;
          }
      }
    if (! axis_x_label_oss.str ().empty () )
      {
        rh2d_.SetXTitle (axis_x_label_oss.str ().c_str ());
      }
    if (! axis_y_label_oss.str ().empty () )
      {
        rh2d_.SetYTitle (axis_y_label_oss.str ().c_str ());
      }
    if (! axis_z_label_oss.str ().empty () )
      {
        rh2d_.SetZTitle (axis_z_label_oss.str ().c_str ());
      }
    return;
  }

  // static
  void histogram_service::export_to_root (const mygsl::histogram_1d & h1d_,
                                          const std::string & name_,
                                          const std::string & title_,
                                          TH1D & rh1d_)
  {
    //h1d_.tree_dump (std::clog, "export H1D... ", "===========> ");
    rh1d_.SetName (name_.c_str ());
    rh1d_.SetTitle (title_.c_str ());
    double xunit = 1.0;
    if (h1d_.get_auxiliaries ().has_key ("display.xaxis.unit"))
      {
        std::string xunit_str = h1d_.get_auxiliaries ().fetch_string ("display.xaxis.unit");
        std::string xunit_type;
        if (! DPP_DU::units::find_unit(xunit_str, xunit, xunit_type))
          {
            xunit = 1.0;
          }
      }
    const size_t nbins = h1d_.bins ();
    {
      std::vector<double> bins;
      bins.assign (nbins + 1, 0.0);
      for (size_t bin_index = 0; bin_index < nbins; bin_index++)
        {
          std::pair<double,double> bin_range = h1d_.get_range (bin_index);
          bins[bin_index] = bin_range.first / xunit;
        }
      bins[nbins] = h1d_.get_range (nbins - 1).second / xunit;
      rh1d_.SetBins (nbins, &bins[0]);
    }
    for (size_t bin_index = 0; bin_index < nbins; bin_index++)
      {
        double bin_content = h1d_.get (bin_index);
        rh1d_.SetBinContent (bin_index+1, bin_content);
      }
    // Adding under/overflow following ROOT numbering conventions
    // http://root.cern.ch/root/html/TH1.html
    if (h1d_.are_underflow_overflow_available ())
      {
        rh1d_.SetBinContent (0, h1d_.underflow ());
        rh1d_.SetBinContent (nbins+1, h1d_.overflow ());
      }
    if (h1d_.is_counts_available ())
      {
        rh1d_.SetEntries (h1d_.counts ());
      }
    else
      {
        // 2012-10-28 XG: Not sure what to do since SetStats
        // (true/false) just hide the 'stat' box in ROOT
        // histogram; Maybe invalidate value for counter i.e. -1
        // is also the best choice
        rh1d_.SetStats (false);
      }

    std::ostringstream axis_x_label_oss;
    std::ostringstream axis_y_label_oss;
    if (h1d_.get_auxiliaries ().has_key ("display.xaxis.label"))
      {
        std::string axis_x_label = h1d_.get_auxiliaries ().fetch_string ("display.xaxis.label");
        if (! axis_x_label.empty ())
          {
            axis_x_label_oss << axis_x_label;
          }
        bool axis_x_label_with_unit = true;
        if (h1d_.get_auxiliaries ().has_flag ("display.xaxis.nounit"))
          {
            axis_x_label_with_unit = false;
          }
        if (axis_x_label_with_unit && h1d_.get_auxiliaries ().has_key ("display.xaxis.unit"))
          {
            axis_x_label_oss << " [" << h1d_.get_auxiliaries ().fetch_string ("display.xaxis.unit") << "]";
          }
      }
    if (h1d_.get_auxiliaries ().has_key ("display.yaxis.label"))
      {
        std::string axis_y_label = h1d_.get_auxiliaries ().fetch_string ("display.yaxis.label");
        if (! axis_y_label.empty ())
          {
            axis_y_label_oss << axis_y_label;
          }
      }
    if (! axis_x_label_oss.str ().empty () )
      {
        rh1d_.SetXTitle (axis_x_label_oss.str ().c_str ());
      }
    if (! axis_y_label_oss.str ().empty () )
      {
        rh1d_.SetYTitle (axis_y_label_oss.str ().c_str ());
      }
    return;
  }

  void histogram_service::store_as_root_file (const std::string & filename_) const
  {
    std::clog << DPP_DU::io::notice
              << "dpp::histogram_service::store_as_root_file: "
              << "Exporting histograms to ROOT file '" << filename_ << "'..." << std::endl;
    std::string fn = filename_;
    DPP_DU::fetch_path_with_env (fn);
    TFile orf (fn.c_str (), "RECREATE");

    std::vector<std::string> histo1d_names;
    std::vector<std::string> histo2d_names;
    _pool_.names (histo1d_names, "dim=1");
    _pool_.names (histo2d_names, "dim=2");

    std::clog << DPP_DU::io::notice
              << "dpp::histogram_service::store_as_root_file: "
              << "Exporting 1D-histograms..." << std::endl;
    for (int i = 0; i < histo1d_names.size (); i++)
      {
        const std::string & hname = histo1d_names[i];
        const mygsl::histogram_1d & h1d = _pool_.get_1d (hname);
        const std::string & htitle = _pool_.get_title (hname);
        const std::string & hgroup = _pool_.get_group (hname);
        std::ostringstream title_oss;
        if (htitle.empty ())
          {
            title_oss << hname;
          }
        else
          {
            title_oss << htitle;
          }
        if (! hgroup.empty ())
          {
            title_oss << " (" << hgroup << ")";
          }
        TH1D rootH1d;
        rootH1d.SetDirectory (&orf);
        rootH1d.SetStats (false);
        export_to_root (h1d, hname, title_oss.str (), rootH1d);
        rootH1d.Write ();
      }
    std::clog << DPP_DU::io::notice
              << "dpp::histogram_service::store_as_root_file: "
              << "Exporting 2D-histograms..." << std::endl;
    for (int i = 0; i < histo2d_names.size (); i++)
      {
        const std::string & hname = histo2d_names[i];
        const mygsl::histogram_2d & h2d = _pool_.get_2d (hname);
        const std::string & htitle = _pool_.get_title (hname);
        const std::string & hgroup = _pool_.get_group (hname);
        std::ostringstream title_oss;
        if (htitle.empty ())
          {
            title_oss << hname;
          }
        else
          {
            title_oss << htitle;
          }
        if (! hgroup.empty ())
          {
            title_oss << " (" << hgroup << ")";
          }
        TH2D rootH2d;
        rootH2d.SetDirectory (&orf);
        rootH2d.SetStats (false);
        export_to_root (h2d, hname, title_oss.str (), rootH2d);
        rootH2d.Write ();
      }

    orf.Close ();
    return;
  }


  void histogram_service::_at_init ()
  {
    return;
  }

  void histogram_service::_at_reset ()
  {
    if (!has_output_files ())
      {
        std::clog << DPP_DU::io::warning
                  << "dpp::histogram_service::_at_reset: "
                  << "No output file given !"
                  << std::endl;
        return;
      }

    for (size_t i = 0; i < _output_files_.size (); ++i)
      {
        std::string fn = _output_files_.at (i);
        DPP_DU::fetch_path_with_env (fn);

        boost::filesystem::path pth (fn);
        if (pth.extension () == ".root")
          {
            store_as_root_file (fn);
          }
        else if (pth.extension () == ".brio")
          {
            std::clog << DPP_DU::io::warning
                      << "dpp::histogram_service::_at_reset: "
                      << "BRIO format is not supported yet !" << std::endl;
          }
        else if (pth.extension () == ".trio")
          {
            std::clog << DPP_DU::io::warning
                      << "dpp::histogram_service::_at_reset: "
                      << "TRIO format is not supported yet !" << std::endl;
          }
        else
          {
            int mode_guess;
            namespace ds = DPP_DSZ;
            if (ds::io_factory::guess_mode_from_filename (fn, mode_guess)
                == ds::io_factory::SUCCESS)
              {
                store_as_boost_file (fn);
              }
            else
              {
                std::ostringstream message;
                message << "dpp::histogram_service::_at_reset: "
                        << "Cannot guess mode for file '" << fn << "'!";
                throw std::logic_error (message.str ());
              }
          }
      }
    return;
  }

  void histogram_service::tree_dump (std::ostream & out_ ,
                                     const std::string & title_,
                                     const std::string & indent_,
                                     bool inherit_) const
  {
    namespace du = DPP_DU;
    this->DPP_DS::base_service::tree_dump (out_, title_, indent_, true);

    out_ << indent_ << du::i_tree_dumpable::inherit_tag (inherit_)
         << "Output files : ";
    if (_output_files_.empty ()) out_ << "<empty>";
    else                         out_ << _output_files_.size ();
    out_ << std::endl;
    for (std::vector<std::string>::const_iterator
           i = _output_files_.begin ();
         i != _output_files_.end (); ++i)
      {
        std::vector<std::string>::const_iterator j = i;
        out_ << indent_ << du::i_tree_dumpable::skip_tag;
        if (++j == _output_files_.end ()) out_ << du::i_tree_dumpable::last_tag;
        else                              out_ << du::i_tree_dumpable::tag;
        out_ << *i << std::endl;
      }

    return;
  }

}  // end of namespace dpp

// end of histogram_service.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

