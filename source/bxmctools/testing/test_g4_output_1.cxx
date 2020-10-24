// -*- mode: c++ ; -*-
// test_sng4_output_1.cxx
/*
 * Copyright 2010-2012 F. Mauger
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
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/**
 * \b Description:
 *
 *   A sample program to analyze sng4 output files.
 *
 * \b Example:
 *
 *  shell> ${SNG4_BIN_DIR}/test_sng4_output_1 snemo_SD_0.txt.gz
 *
 */

#define APPNAME_PREFIX "sncore: snemo_dump_data: "

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <stdexcept>

// Utilities :
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/temporary_files.h>

#include <mygsl/histogram.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_i.h>

// SuperNEMO data models :
#include <datatools/things.h>
#include <mctools/simulated_data.h>

// Utilities for geometry :
#include <geomtools/smart_id_locator.h>
#include <geomtools/manager.h>

// Serialization code :
#include <datatools/io_factory.h>

// For Boost I/O :
// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/bio_guard.h>
#include <mygsl/bio_guard.h>
#include <geomtools/bio_guard.h>
#include <genbb_help/bio_guard.h>
#include <mctools/bio_guard.h>

using namespace std;

struct analysis_processor
{
public:
  bool debug;
  bool draw;
private:
  bool     initialized_;
  int      sequential_counter_;
  int      modulo_;
  string   SD_label_; //!< Simulated data label
  datatools::things * data_;
  geomtools::manager               * gmgr_;
  const geomtools::mapping         * mapping_;
  const geomtools::id_mgr          * id_mgr_;

  uint32_t                   module_type;
  int                        module_number_;
  geomtools::placement       module_placement_;

  uint32_t calorimeter_block_type;
  geomtools::smart_id_locator calo_block_locator;

  uint32_t xcalo_block_type;
  geomtools::smart_id_locator xcalo_block_locator;

  uint32_t gveto_block_type;
  geomtools::smart_id_locator gveto_block_locator;

  uint32_t drift_cell_type;
  geomtools::smart_id_locator drift_cell_locator;
  snemo::geometry::snemo::gg_locator module_gg_locator_;

  uint32_t source_pad_type;
  geomtools::smart_id_locator source_pad_locator;

  std::list<std::string> labels;
  std::map<std::string, mygsl::histogram> histos;

public:
  bool is_initialized () const {
    return initialized_;
  }

  analysis_processor (bool a_debug = false);

  analysis_processor (const std::string & a_SD_label,
                      bool a_debug = false);

  ~analysis_processor ();

  void set_geometry_manager (geomtools::manager &);

  void process (datatools::things & a_data);

  void initialize ();

  void finalize ();

  void reset ();
};

void analysis_processor::initialize ()
{
  if (initialized_)
    {
      throw logic_error ("analysis_processor::initialize: Already initialized !");
    }

  if (gmgr_ == 0)
    {
      throw logic_error ("analysis_processor::initialize: Missing geometry manager !");
    }

  if (module_number_ < 0)
    {
      throw logic_error ("analysis_processor::initialize: Missing module number !");
    }

  // Prepare histograms :
  labels.push_back ("calo");
  labels.push_back ("gg");
  labels.push_back ("xcalo");
  labels.push_back ("gveto");
  for (list<string>::const_iterator i = labels.begin ();
       i != labels.end ();
       ++i)
    {
      mygsl::histogram dummy (2, 0., 2.);
      ostringstream htitle;
      htitle << *i << '.' << "side";
      histos[htitle.str ()] = dummy;
    }

  {
    mygsl::histogram dummy (20, 0., 20.);
    ostringstream htitle;
    htitle << "gg" << '.' << "layer";
    histos[htitle.str ()] = dummy;
  }

  {
    mygsl::histogram dummy (2, 0., 2.);
    ostringstream htitle;
    htitle << "gg" << '.' << "first_side";
    histos[htitle.str ()] = dummy;
  }

  initialized_ = true;
  return;
}

void analysis_processor::set_geometry_manager (snemo::geometry::manager & a_gmgr)
{
  gmgr_ = &a_gmgr;
  mapping_ = &(gmgr_->get_mapping ());
  id_mgr_ = &(gmgr_->get_id_mgr ());

  const geomtools::id_mgr::categories_by_name_col_t & categories
    = gmgr_->get_id_mgr ().categories_by_name ();
  if (debug) clog << datatools::io::debug
                  << "analysis_processor::set_geometry_manager: categories ok..." << endl;

  module_type = categories.find ("module")->second.get_type ();
  if (debug) clog << datatools::io::debug
                  << "analysis_processor::set_geometry_manager: module_type="
                  << module_type << endl;

  // Calorimeter blocks:
  calorimeter_block_type
    = categories.find ("calorimeter_block")->second.get_type ();
  if (debug) clog << datatools::io::debug
                  << "analysis_processor::set_geometry_manager: calorimeter_block_type="
                  << calorimeter_block_type << endl;
  calo_block_locator.set_gmap (*mapping_);
  calo_block_locator.initialize (calorimeter_block_type);
  if (debug) clog << datatools::io::debug
                  << "analysis_processor::set_geometry_manager: calorimeter blocks locator is initialized."
                  << endl;

  // X-calorimeter blocks:
  xcalo_block_type
    = categories.find ("xcalo_block")->second.get_type ();
  if (debug) clog << datatools::io::debug
                  << "analysis_processor::set_geometry_manager: xcalo_block_type="
                  << xcalo_block_type << endl;
  xcalo_block_locator.set_gmap (*mapping_);
  xcalo_block_locator.initialize (xcalo_block_type);
  if (debug) clog << datatools::io::debug
                  << "analysis_processor::set_geometry_manager: xcalo blocks locator is initialized."
                  << endl;

  // Gamma-veto blocks:
  gveto_block_type
    = categories.find ("gveto_block")->second.get_type ();
  if (debug) clog << datatools::io::debug
                  << "analysis_processor::set_geometry_manager: gveto_block_type="
                  << gveto_block_type << endl;
  gveto_block_locator.set_gmap (*mapping_);
  gveto_block_locator.initialize (gveto_block_type);
  if (debug) clog << datatools::io::debug
                  << "analysis_processor::set_geometry_manager: gveto blocks locator is initialized."
                  << endl;

  // Drift cells:
  drift_cell_type
    = categories.find ("drift_cell_core")->second.get_type ();
  if (debug) clog << datatools::io::debug
                  << "analysis_processor::set_geometry_manager: drift_cell_type="
                  << drift_cell_type << endl;
  drift_cell_locator.set_gmap (*mapping_);
  drift_cell_locator.initialize (drift_cell_type);
  if (debug) clog << datatools::io::debug
                  << "analysis_processor::set_geometry_manager: drift cells locator is initialized."
                  << endl;

  // Source pads:
  source_pad_type
    = categories.find ("source_pad")->second.get_type ();
  if (debug) clog << datatools::io::debug
                  << "analysis_processor::set_geometry_manager: source_pad_type="
                  << source_pad_type << endl;
  source_pad_locator.set_gmap (*mapping_);
  source_pad_locator.initialize (source_pad_type);
  if (debug) clog << datatools::io::debug
                  << "analysis_processor::set_geometry_manager: source pads locator is initialized."
                  << endl;

  if (module_number_ < 0)
    {
      set_module_number (0);
    }
  return;
}

void analysis_processor::set_module_number (int a_mn)
{
  if (module_number_ != a_mn)
    {
      if (module_gg_locator_.is_initialized ())
        {
          clog << datatools::io::devel
               << "analysis_processor::set_module_number: gg locator is initialized -> reset !!!" << endl;
          module_gg_locator_.reset ();
        }
      module_number_ = a_mn;
      module_placement_.reset ();
      module_gg_locator_.set_manager (*gmgr_);
      module_gg_locator_.set_module_number (module_number_);
      module_gg_locator_.initialize ();
    }
  return;
}

void analysis_processor::reset ()
{
  if (! initialized_)
    {
      throw logic_error ("analysis_processor::reset: Not initialized !");
    }
  gmgr_ = 0;
  mapping_ = 0;
  id_mgr_ = 0;
  module_number_ = -1;
  module_placement_.reset ();
  if (module_gg_locator_.is_initialized ())
    {
      module_gg_locator_.reset ();
    }
  sequential_counter_ = -1;
  modulo_ = 1000;
  data_ = 0;
  labels.clear ();
  histos.clear ();
  initialized_ = false;
  return;
}

analysis_processor::~analysis_processor ()
{
  if (initialized_) reset ();
  return;
}

analysis_processor::analysis_processor (bool a_debug)
{
  initialized_ = false;
  debug    = a_debug;
  EH_label_ = snemo::core::model::data_info::EVENT_HEADER_LABEL;
  SD_label_ = snemo::core::model::data_info::SIMULATED_DATA_LABEL;
  CD_label_ = snemo::core::model::data_info::CALIBRATED_DATA_LABEL;
  gmgr_ = 0;
  mapping_ = 0;
  id_mgr_ = 0;
  module_number_ = -1;
  sequential_counter_ = -1;
  modulo_ = 1000;
  data_    = 0;
  return;
}

analysis_processor::analysis_processor (const string & a_EH_label,
                                        const string & a_SD_label,
                                        const string & a_CD_label,
                                        bool a_debug)
{
  initialized_ = false;
  debug     = a_debug;
  if (debug) clog << datatools::io::debug << "analysis_processor::ctor: Entering..." << endl;
  EH_label_ = a_EH_label;
  SD_label_ = a_SD_label;
  CD_label_ = a_CD_label;
  gmgr_ = 0;
  mapping_ = 0;
  id_mgr_ = 0;
  module_number_ = -1;
  sequential_counter_ = -1;
  modulo_ = 1000;
  data_    = 0;
  draw = true;
  if (debug) clog << datatools::io::debug << "analysis_processor::ctor: Exiting." << endl;
  return;
}

void analysis_processor::finalize ()
{
  if (! initialized_)
    {
      throw logic_error ("analysis_processor::finalize: Not initialized !");
    }

  if (draw)
    {
      for (map<string, mygsl::histogram>::const_iterator i = histos.begin ();
           i != histos.end ();
           ++i)
        {
          const string & histo_title = i->first;
          const mygsl::histogram & histo = i->second;
          clog << "Histogram '" << histo_title << "'" << endl;

          Gnuplot g1 ("lines");
          g1.set_title (histo_title);
          g1.set_grid ();
          g1.set_xlabel("Side").set_ylabel("Counts");
          datatools::temp_file fhisto;
          fhisto.create (".", string ("temp_") + histo_title + "_");
          fhisto.set_remove_at_destroy (false);
          histo.print (fhisto.out ());
          fhisto.close ();
          ostringstream cmdss;
          string x_range = "[0:2]";
          if (histo_title == "gg.layer") x_range = "[0:20]";
          cmdss << "plot " << x_range << "[0:*] '"
                << fhisto.get_filename () << "' "
                << "using (($1)+0.5):($3) with histeps lw 2;";
          g1.cmd (cmdss.str ());
          g1.showonscreen ();
          cout << endl << "Press ENTER to continue..." << endl;
          std::cin.clear ();
          std::cin.ignore (std::cin.rdbuf ()->in_avail ());
          std::cin.get ();

        }
    }
  return;
}

void analysis_processor::process (snemo::core::model::event_record & a_data)
{
  if (debug) clog << datatools::io::debug << "analysis_processor::process: Entering..." << endl;

  if (! initialized_)
    {
      throw logic_error ("analysis_processor::process: Not initialized !");
    }

  const snemo::core::model::event_header    * eh_ptr = 0;
  const snemo::core::model::simulated_data  * sd_ptr = 0;
  const snemo::core::model::calibrated_data * cd_ptr = 0;

  data_ = &a_data;
  sequential_counter_++;

  if (data_->has (EH_label_))
    {
      eh_ptr = &(data_->get<snemo::core::model::event_header> (EH_label_));
    }
  if (data_->has (SD_label_))
    {
      sd_ptr = &(data_->get<snemo::core::model::simulated_data> (SD_label_));
    }
  if (data_->has (CD_label_))
    {
      cd_ptr = &(data_->get<snemo::core::model::calibrated_data> (CD_label_));
    }

  if (modulo_ > 0 && (sequential_counter_ % modulo_) == 0)
    {
      clog << "Event data #" << sequential_counter_ << endl;
    }
  /*
    if (eh_ptr != 0)
    {
    const snemo::core::model::event_header & eh = *eh_ptr;
    clog << " -> " << "Event header :" << endl;
    eh.tree_dump (clog, "", "    ");
    }
    else
    {
    clog << " -> " << "No event header." << endl;
    }
  */

  if (sd_ptr != 0)
    {
      namespace scm = snemo::core::model;
      const scm::simulated_data & sd = *sd_ptr;
      /*
        clog << " -> " << "Simulated data : " << endl;
        sd.tree_dump (clog, "", "    ");
      */

      // HITS FROM THE SENSITIVE DETECTORS :

      for (list<string>::const_iterator i = labels.begin ();
           i != labels.end ();
           ++i)
        {
          const string & hit_category = *i;
          double first_gg_time;
          uint32_t first_gg_side = 0xFFFFFFFF;
          datatools::invalidate (first_gg_time);
          if (sd.has_step_hits (hit_category) && sd.get_step_hits (hit_category).size ())
            {
              const scm::simulated_data::hit_handle_collection_type & BSHC = sd.get_step_hits (hit_category);
              size_t count = 0;
              for (scm::simulated_data::hit_handle_collection_type::const_iterator i
                     = BSHC.begin ();
                   i != BSHC.end ();
                   i++)
                {
                  if (! i->has_data ()) continue;
                  const scm::base_step_hit & BSH = i->get ();
                  string particle_name = BSH.get_particle_name ();
                  const geomtools::geom_id & gid = BSH.get_geom_id ();

                  string what = "side";
                  ostringstream htitle;
                  htitle << hit_category << "." << what;
                  uint32_t side = id_mgr_->get (gid, what);
                  histos[htitle.str ()].fill (1.0 * side);
                  if (hit_category == "gg")
                    {
                      string what = "layer";
                      ostringstream htitle;
                      htitle << hit_category << "." << what;
                      uint32_t layer = id_mgr_->get (gid, what);
                      histos[htitle.str ()].fill (layer + 10.0 * side);
                      double gg_ionization_time = BSH.get_time_start ();
                      if (! datatools::is_valid (first_gg_time)
                          || gg_ionization_time < first_gg_time)
                        {
                          first_gg_time = gg_ionization_time;
                          first_gg_side = side;
                        }
                    }

                } // for
              if (hit_category == "gg")
                {
                  ostringstream htitle;
                  htitle << hit_category << "." << "first_side";
                  if (datatools::is_valid (first_gg_time))
                    {
                      histos[htitle.str ()].fill (first_gg_side);
                    }
                }

            }
        }
    }
  else
    {
      clog << " -> " << "No simulated data." << endl;
    }

  /*
    if (cd_ptr != 0)
    {
    const snemo::core::model::calibrated_data & cd = *cd_ptr;
    clog << " -> " << "Calibrated data : " << endl;
    cd.tree_dump (clog, "", "    ");
    }
    else
    {
    clog << " -> " << "No calibrated data." << endl;
    }
  */

  if (debug) clog << datatools::io::debug << "analysis_processor::process: Exiting." << endl;
  return;
}

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      bool   debug = false;
      bool   devel = false;
      bool   draw  = true;
      bool   verbose = false;
      string gmanager_config_file;
      string fname;
      string SD_label = snemo::core::model::data_info::SIMULATED_DATA_LABEL;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];
          if (token[0] == '-')
            {
              string option = token;
              if ((option == "-d") || (option == "--debug"))
                {
                  debug = true;
                }
              else if ((option == "--devel"))
                {
                  devel = true;
                }
              else if ((option == "--no-draw"))
                {
                  draw = false;
                }
              else if ((option == "-g") || (option == "--geometry"))
                {
                  ++iarg;
                  gmanager_config_file = argv_[iarg];
                  if (debug)
                    {
                      clog << datatools::io::debug << "Option parsing: geometry manager config. file : '"
                           << gmanager_config_file << "'" << endl;
                    }
                }
              else
                {
                  clog << datatools::io::warning << "Ignoring option '" << option << "' !" << endl;
                }
            }
          else
            {
              string argument = token;
              if (fname.empty ())
                {
                  fname = argument;
                }
              else
                {
                  clog << datatools::io::warning <<  "Ignoring argument '" << argument << "' !" << endl;
                }
            }
          iarg++;
        }

      if (fname.empty ())
        {
          if (devel)
            {
              fname = "${SNCORE_TESTING_DIR}/../resources/setups/snemo/samples/snemo_SD_0.txt.gz";
            }
          else
            {
              //throw logic_error ("Missing input file name !");
              fname = "${SNCORE_TESTING_DIR}/../resources/setups/snemo/samples/snemo_SD_0.txt.gz";
            }
        }
      clog << datatools::io::notice << "Input file name : '" << fname << "'" <<  endl;
      datatools::fetch_path_with_env (fname);
      if (gmanager_config_file.empty ())
        {
          gmanager_config_file = "${SNGEOMETRY_TESTING_DIR}/../resources/setups/snemo/config_1.0/manager.conf";
        }
      datatools::fetch_path_with_env (gmanager_config_file);
      clog << datatools::io::notice << "Geometry manager config. file : '"
           << gmanager_config_file << "'" << endl;

      // load properties from the configuration file:
      datatools::properties gmanager_config;
      datatools::read_config (gmanager_config_file,
                                                 gmanager_config);

      //SuperNEMO geometry manager:
      snemo::geometry::manager gmgr;
      gmgr.set_debug (debug);
      gmgr.set_verbose (verbose);

      string setup_label = gmanager_config.fetch_string ("setup_label");
      if (setup_label != "snemo")
        {
          ostringstream message;
          message << "Setup label '" << setup_label << "' is not supported !";
          throw logic_error (message.str ());
        }

      if (setup_label == "snemo")
        {
          // prepare mapping configuration with a limited set
          // of geometry categories:

          //  1. first remove the 'exclusion' property if any:
          if (gmanager_config.has_key ("mapping.excluded_categories"))
            {
              gmanager_config.erase ("mapping.excluded_categories");
            }

          //  2. then specify the geometry categories to be mapped:
          vector<string> only_categories;
          only_categories.push_back ("hall");
          only_categories.push_back ("module");
          only_categories.push_back ("source_pad");
          only_categories.push_back ("drift_cell_core");
          only_categories.push_back ("xcalo_block");
          only_categories.push_back ("gveto_block");
          only_categories.push_back ("calorimeter_block");
          gmanager_config.update ("mapping.only_categories", only_categories);

          //  3. finally activate the build of the mapping:
          gmanager_config.update ("build_mapping", true);

        }
      gmgr.initialize (gmanager_config);

      // declare the analysis processor:
      analysis_processor the_analyzer (EH_label, SD_label, CD_label, debug);
      if (debug) clog << "DEBUG: ctor done !" << endl;
      the_analyzer.set_geometry_manager (gmgr);
      if (debug) clog << "DEBUG: geometry manager done !" << endl;
      the_analyzer.set_module_number (0);
      if (debug) clog << "DEBUG: module number done !" << endl;
      the_analyzer.draw = draw;
      the_analyzer.initialize ();

      // declare a generic reader to load simulated data objects:
      datatools::data_reader the_data_reader (fname, datatools::using_multi_archives);
      the_data_reader.dump (clog);

      // Run header/footer :
      datatools::multi_properties the_run_header;
      datatools::multi_properties the_run_footer;

      // loop on simulated data from a file:
      size_t record_count = 0;
      bool goon = true;
      bool has_run_header = false;
      bool has_run_footer = false;
      do {
        if (the_data_reader.has_record_tag ())
          {
            if (debug) clog << "DEBUG: record tag..." << endl;
            bool new_format = false;
            string old_tag = "snemo::core::model::simulation_data";

            // The data model container :
            snemo::core::model::event_record the_data;

            // Attempt to deserialize data :
            if (the_data_reader.record_tag_is (datatools::multi_properties::SERIAL_TAG))
              {
                // Attempt to deserialize some possible run header/footer from the archive stream :
                if (! has_run_header)
                  {
                    the_data_reader.load (the_run_header);
                    has_run_header = true;
                  }
                else if (! has_run_footer)
                  {
                    the_data_reader.load (the_run_footer);
                    has_run_footer = true;
                  }
                else
                  {
                    ostringstream message;
                    message << "Invalid serial tag '"
                            << the_data_reader.get_record_tag ()<< "' !";
                    throw logic_error (message.str ());
                  }
              }
            else if (the_data_reader.record_tag_is (snemo::core::model::simulated_data::SERIAL_TAG))
              {
                // Attempt to deserialize 'simulated_data' objects (sng4 plain output):
                snemo::core::model::simulated_data & sim_data
                  = the_data.add<snemo::core::model::simulated_data> (SD_label);
                the_data_reader.load (sim_data);
              }
            else if (the_data_reader.record_tag_is (snemo::core::model::event_record::SERIAL_TAG))
              {
                // Attempt to deserialize 'data' objects (things):
                the_data_reader.load (the_data);
              }
            else
              {
                ostringstream message;
                message << "Invalid serial tag '"
                        << the_data_reader.get_record_tag ()<< "' !";
                throw logic_error (message.str ());
              }
            the_analyzer.process (the_data);
            record_count++;
          }
        else
          {
            if (debug) clog << "DEBUG: no record tag !" << endl;
            break;
          }
        if (goon)
          {
            if (debug) clog << datatools::io::debug << "trying to load next event..." << endl;
          }
      } while (goon);

      the_analyzer.finalize ();
      clog << datatools::io::notice
           << "Finalize is done." << endl;

      // Close data reader (not mandatory) :
      the_data_reader.reset ();

      // Terminate the processor (not mandatory) :
      the_analyzer.reset ();
      clog << datatools::io::notice
           << "analyzer is reset." << endl;

    }
  catch (exception & x)
    {
      cerr << datatools::io::error << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << datatools::io::error <<  "Unexpected error !" << endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_sng4_output_1.cxx
