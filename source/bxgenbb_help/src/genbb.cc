// -*- mode: c++; -*- 
// genbb.cc
/*
 * Copyright 2007-2012, F. Mauger
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <sstream>
#include <cstdlib>

#include <boost/filesystem.hpp>

#include <datatools/utils/utils.h>
 
#include <genbb_help/genbb.h>
#include <genbb_help/genbb_utils.h>
#include <boost/algorithm/string/split.hpp> 
#include <boost/algorithm/string/classification.hpp> 

namespace genbb {
 
  using namespace std;

  bool genbb::is_initialized () const
  {
    return _initialized_;
  }

  bool genbb::is_debug () const
  {
    return _debug_;
  }
 
  void genbb::set_debug (bool d_)
  {
    _debug_ = d_;
    return;
  }

  void genbb::set_delete_conf_file (bool d_)
  {
    _delete_conf_file_ = d_;
    return;
  }

  void genbb::set_delete_log_files (bool d_)
  {
    _delete_log_files_ = d_;
    return;
  }

  void genbb::set_delete_data_files (bool d_)
  {
    _delete_data_files_ = d_;
    return;
  }

  void genbb::set_delete_tmp_dir (bool d_)
  {
    _delete_tmp_dir_ = d_;
    return;
  }

  bool genbb::can_external_random () const
  {
    return true;
  }

  // void genbb::set_external_random (mygsl::rng & rng_)
  // {
  //   if (! rng_.is_initialized ())
  //     {
  //       std::ostringstream message;
  //       message << "genbb::genbb::set_external_random: External radom generator is not initialized !";
  //       throw logic_error (message.str());      
  //     }
  //   _set_external_random (rng);
  //   return;
  // }


  const mygsl::rng & genbb::get_random () const
  {
    if (has_external_random ())
      {
        return get_external_random ();
      }
    return _random_;
  }


  mygsl::rng & genbb::get_random ()
  {
    return grab_random ();
  }

  mygsl::rng & genbb::grab_random ()
  {
    if (has_external_random ())
      {
        return grab_external_random ();
      }
    return _random_;
  }

  void genbb::_clean_ ()
  {
    if (_genbb_in_ != 0)
      {
        if (_debug_)
          {
            clog << "debug: " << "genbb::genbb::_clean_: "
                 << "Deleting current GENBB data input file stream instance..." << endl;
          }
        delete _genbb_in_;
        _genbb_in_ = 0;
      }
    if (! _genbb_data_.empty () && boost::filesystem::exists (_genbb_data_.c_str ()))
      {
        if (_delete_data_files_)
          {
            if (_debug_)
              {
                clog << "debug: " << "genbb::genbb::_clean_: "
                     << "Removing previous GENBB data file '" << _genbb_data_ << "'..."
                     << endl;
              }
            unlink (_genbb_data_.c_str ());
          }
        _genbb_data_ = "";
      }
    if (! _genbb_log_.empty () && boost::filesystem::exists (_genbb_log_.c_str ()))
      {
        if (_delete_log_files_)
          {
            if (_debug_)
              {
                clog << "debug: " << "genbb::genbb::_clean_: "
                     << "Removing previous GENBB log file '" << _genbb_log_ << "'..."
                     << endl;
              }
            unlink (_genbb_log_.c_str ());
          }
        _genbb_log_ = "";
      }
    _genbb_data_ = "";
    _genbb_log_ = "";
    _buffer_count_++;
    return;
  }
  
  // ctor:
  genbb::genbb () : i_genbb ()
  {
    _initialized_ = false;
    _debug_ = false;
    _test_ = false;

    _buffer_size_ = DEFAULT_BUFFER_SIZE;
    _buffer_item_ = 0;
    _event_count_ = 0;
    _delete_conf_file_ = false;
    _delete_log_files_  = false;
    _delete_data_files_ = true;
    _delete_tmp_dir_ = false;

    _genbb_in_ = 0;
    _genbb_data_ = "";
    _genbb_conf_ = "";
    _genbb_log_ = "";
    _tmp_base_dir_ = "/tmp/${USER}";

    _decay_type_ = utils::INVALID_DECAY_TYPE;
    _decay_isotope_ = "";  
    _decay_dbd_level_ = 0;  
    _decay_dbd_mode_ = 1;
    _use_energy_range_ = false;
    _energy_min_ = utils::DEFAULT_ENERGY_RANGE_MIN; 
    _energy_max_ = utils::DEFAULT_ENERGY_RANGE_MAX;
    _seed_ = 0;
    _tmp_dir_[0] = 0;
    _buffer_count_ = 0;

    _genbb_weight_ = 1.0;

    return;
  }
  
  // dtor:
  genbb::~genbb ()
  {
    _clean_ ();
    if (_initialized_)
      {
        reset ();
      }
    return;
  }

  void genbb::set_tmp_dir (const string & td_)
  {
    if (_initialized_)
      {
        throw logic_error ("genbb::genbb::set_tmp_base_dir: Operation prohibited ! Object is locked !");
      }
    _forced_tmp_dir_ = td_;
    return;
  }

  void genbb::set_tmp_base_dir (const string & tbd_)
  {
    if (_initialized_)
      {
        throw logic_error ("genbb::genbb::set_tmp_base_dir: Operation prohibited ! Object is locked !");
      }
    _tmp_base_dir_ = tbd_;
    return;
  }

  const string & genbb::get_tmp_base_dir () const
  {
    return _tmp_base_dir_;
  }

  string genbb::get_tmp_dir () const
  {
    if (_forced_tmp_dir_.empty ())
      {
        return string(_tmp_dir_);
      }
    return _forced_tmp_dir_;
  }

  void genbb::reset ()
  {
    if (! _initialized_)
      {
        throw logic_error ("genbb::genbb::reset: Object is not initialized !");
      }
    _initialized_ = false;
    _clean_ ();
    _test_ = false;
    _buffer_size_ = DEFAULT_BUFFER_SIZE;
    _buffer_item_ = 0;
    _buffer_count_ = 0;
    if (_genbb_in_ != 0)
      {
        delete _genbb_in_;
      }
    _genbb_in_ = 0;
    _event_count_ = 0;

    if (! _genbb_conf_.empty () && boost::filesystem::exists (_genbb_conf_.c_str ()))
      {
        if (_delete_conf_file_)
          {
            unlink (_genbb_conf_.c_str ());
          }
      }
    if (_delete_tmp_dir_)
      {
        clog << "WARNING: genbb::reset: "
             << "Temporary working directory deletion not implemented yet !"
             << endl;
      }
    _tmp_dir_[0] = 0;
    _forced_tmp_dir_;

    _tmp_base_dir_ = "/tmp/${USER}";
    _decay_type_ = utils::INVALID_DECAY_TYPE;
    _decay_isotope_ = "";  
    _decay_dbd_level_ = 0;  
    _decay_dbd_mode_ = 1;
    _use_energy_range_ = false;
    _energy_min_ = utils::DEFAULT_ENERGY_RANGE_MIN; 
    _energy_max_ = utils::DEFAULT_ENERGY_RANGE_MAX;

    _seed_ = 0;
    _random_.reset ();
    _genbb_conf_ = "";
    _genbb_conf_file_.close ();
    _genbb_data_ = "";
    _genbb_log_ = "";
    _delete_conf_file_ = false;
    _delete_log_files_  = false;
    _delete_data_files_ = true;
    _delete_tmp_dir_ = false;

    _genbb_weight_ = 1.0;

    return;
  }

  void genbb::initialize (const datatools::utils::properties & config_)
  {
    if (_initialized_)
      {
        throw logic_error ("genbb::genbb::reset: Object is already initialized !");
      }

    if (config_.has_flag ("debug"))
      {
        _debug_ = true;
      }

    if (! has_external_random ())
      {
        
        if (config_.has_key ("seed"))
          {
            long seed = config_.fetch_integer ("seed");
            if (seed < 0)
              {
                throw logic_error ("genbb::genbb::initialize: Invalid seed value (>=0) !");
              }
            _seed_ = seed;
          }
        
        if (config_.has_flag ("test"))
          {
            _test_ = true;
          }
      }

    if (config_.has_key ("buffer_size"))
      {
        int bs = config_.fetch_integer ("buffer_size");
        if (bs <= 0)
          {
            throw logic_error ("genbb::genbb::initialize: Invalid buffer size property !");
          }
        if (bs >= MAX_BUFFER_SIZE)
          {
            throw logic_error ("genbb::genbb::initialize: Buffer size is too large !");
          }
        _buffer_size_ = bs;
      }

    if (config_.has_key ("tmp_base_dir"))
      {
        _tmp_base_dir_ = config_.fetch_string ("tmp_base_dir");
      }

    if (config_.has_flag ("delete_conf_file"))
      {
        set_delete_conf_file (true);
      }

    if (config_.has_flag ("preserve_conf_file"))
      {
        set_delete_conf_file (false);
      }

    if (config_.has_flag ("delete_log_files"))
      {
        set_delete_log_files (true);
      }

    if (config_.has_flag ("preserve_log_files"))
      {
        set_delete_log_files (false);
      }

    if (config_.has_flag ("delete_data_files"))
      {
        set_delete_data_files (true);
      }

    if (config_.has_flag ("preserve_data_files"))
      {
        set_delete_data_files (false);
      }

    if (config_.has_key ("decay_type"))
      {
        string tmp = config_.fetch_string ("decay_type");
        if ((tmp != "DBD") && (tmp != "background"))
          {
            ostringstream message;
            message << "genbb::genbb::initialize: Invalid decay type '"
                    << tmp << "' !";
            throw logic_error (message.str());
          }
        if (tmp == "background") 
          {
            _decay_type_ = utils::DECAY_TYPE_BACKGROUND;

            if (! config_.has_key ("decay_isotope"))
              {
                ostringstream message;
                message << "genbb::genbb::initialize: Missing background isotope !";
                throw logic_error (message.str());
              }
            _decay_isotope_ = config_.fetch_string ("decay_isotope");
          }

        if (tmp == "DBD") 
          {
            _decay_type_ = utils::DECAY_TYPE_DBD;

            if (! config_.has_key ("decay_isotope"))
              {
                ostringstream message;
                message << "genbb::genbb::initialize: Missing DBD isotope !";
                throw logic_error (message.str());
              }
            _decay_isotope_ = config_.fetch_string ("decay_isotope");

            if (! config_.has_key ("decay_dbd_level"))
              {
                ostringstream message;
                message << "genbb::genbb::initialize: Missing DBD decay level !";
                throw logic_error (message.str());
              }
            _decay_dbd_level_ = config_.fetch_integer ("decay_dbd_level");

            if (! config_.has_key ("decay_dbd_mode"))
              {
                ostringstream message;
                message << "genbb::genbb::initialize: Missing DBD decay mode !";
                throw logic_error (message.str());
              }
            _decay_dbd_mode_ = config_.fetch_integer ("decay_dbd_mode");
          }

     }

    if (_decay_type_ == utils::DECAY_TYPE_DBD)
      {
        double energy_unit = CLHEP::MeV;
        _use_energy_range_ = false;
        const std::vector<int> & dbdmwer 
          = utils::get_dbd_modes_with_energy_range ();
        if (std::find (dbdmwer.begin (), dbdmwer.end (),_decay_dbd_mode_) != dbdmwer.end ())
          {
            if (config_.has_key ("energy_min"))
              {
                _energy_min_ = config_.fetch_real ("energy_min");
                _energy_min_ *= energy_unit;
              }
            if (config_.has_key ("energy_max"))
              {
                _energy_max_ = config_.fetch_real ("energy_max");
                _energy_max_ *= energy_unit;
              }
            if (_energy_min_ < utils::DEFAULT_ENERGY_RANGE_MIN)
              {
                _energy_min_ = 0.0;
              }
            if (_energy_min_ > utils::DEFAULT_ENERGY_RANGE_MIN)
              {
                _use_energy_range_ = true;
              }
            if (_energy_max_ <= _energy_min_)
              {
              }
            if (_energy_max_ < utils::DEFAULT_ENERGY_RANGE_MAX)
              {
                _use_energy_range_ = true;
              }
            if (_energy_min_ >= _energy_max_)
              {
                ostringstream message;
                message << "genbb::genbb::initialize: Invalid energy range !";
                throw logic_error (message.str());
              }
            
          }
      }
    
    if (! has_external_random ())
      {
        _random_.init ("taus2", _seed_);
      }

    /***************/

    // Temporary directory setup :
    {
      if (_forced_tmp_dir_.empty ())
        {
          if (_tmp_base_dir_.empty ())
            {
              ostringstream message;
              message << "genbb::genbb::initialize: Missing base temporary directory ! ";
              throw logic_error (message.str());
            }
          datatools::utils::fetch_path_with_env (_tmp_base_dir_);
          {
            if (! boost::filesystem::is_directory (_tmp_base_dir_))
              {
                ostringstream message;
                message << "genbb::genbb::initialize: Base temporary directory '"
                        << _tmp_base_dir_ << "' does not exist ! you should first create it !";
                _clean_ ();
                throw logic_error (message.str());
              }
          }
        
          ostringstream oss;
          oss << _tmp_base_dir_ << '/' << "genbb_help.genbb.XXXXXX";
          int i;
          for (i = 0; i < oss.str ().size (); i++)
            {
              _tmp_dir_[i] = oss.str ()[i];
            }
          _tmp_dir_[i] = 0;
        
          char * ret = mkdtemp (_tmp_dir_);
          if (ret == NULL)
            {
              ostringstream message;
              message << "genbb::genbb::initialize: Cannot create temporary directory in '"
                      << _tmp_base_dir_ << "' !";
              throw logic_error (message.str());
            }
        }
      else
        {
          datatools::utils::fetch_path_with_env (_forced_tmp_dir_);
          if (_forced_tmp_dir_.length () > (TMP_DIR_BUFSZ - 1))
            {
              ostringstream message;
              message << "genbb::genbb::initialize: Temporary directory name is too long (<" << TMP_DIR_BUFSZ << ")!";
              throw logic_error (message.str());
              
            }
          if (! boost::filesystem::is_directory (_forced_tmp_dir_))
            {
              ostringstream message;
              message << "genbb::genbb::initialize: Temporary directory '"
                      << _forced_tmp_dir_ << "' does not exist ! You should first create it !";
              _clean_ ();
              throw logic_error (message.str());
            }
          int i;
          for (i = 0; i < _forced_tmp_dir_.size (); i++)
            {
              _tmp_dir_[i] = _forced_tmp_dir_[i];
            }
          _tmp_dir_[i] = 0;
        }

      {
        ostringstream oss;
        oss << _tmp_dir_ << '/' << "genbb.conf";
        _genbb_conf_ = oss.str ();
      }
 
      {
        _genbb_conf_file_.open (_genbb_conf_.c_str ());
        if (! _genbb_conf_file_)
          {
            ostringstream message;
            message << "genbb::genbb::initialize: Cannot create GENBB config file in '"
                    << _tmp_dir_ << "' !";
            throw logic_error (message.str());
          }
        _genbb_conf_file_ << "type=" << _decay_type_ << endl;
        if (_decay_type_ == utils::DECAY_TYPE_BACKGROUND)
          {
            _genbb_conf_file_ << "nucleide=" << _decay_isotope_ << endl;
          }
        else if (_decay_type_ == utils::DECAY_TYPE_DBD)
          {
            _genbb_conf_file_ << "betabeta-nucleide=" << _decay_isotope_ << endl;
            _genbb_conf_file_ << "betabeta-level=" << _decay_dbd_level_ << endl;
            _genbb_conf_file_ << "betabeta-mode=" << _decay_dbd_mode_ << endl;
            const std::vector<int> & dbdmwer 
              = utils::get_dbd_modes_with_energy_range ();
            if (std::find (dbdmwer.begin (), dbdmwer.end (), _decay_dbd_mode_) != dbdmwer.end ())
              {
                std::clog << "DEVEL: restrict_energy_range = " << _use_energy_range_ << endl;
                std::clog << "DEVEL: energy_min = " << _energy_min_ << " MeV" << endl;
                std::clog << "DEVEL: energy_max = " << _energy_max_ << " MeV" << endl;
                _genbb_conf_file_ << "restrict-energy=" << (_use_energy_range_ ? "y" : "n") << endl;
                _genbb_conf_file_ << "energy-min=" << _energy_min_ / CLHEP::MeV << endl;
                _genbb_conf_file_ << "energy-max=" << _energy_max_ / CLHEP::MeV << endl;
              }
          }
        _genbb_conf_file_ << "number-of-events=" << _buffer_size_ << endl;
        _genbb_conf_file_ << "first-event=1" << endl;
        _genbb_conf_file_ << "write-file=y" << endl;
        _genbb_conf_file_ << "output-file=" << "out.genbb" << endl;

      }
      
    }

    _init_ ();

    _initialized_ = true;
    return;
  }

  bool genbb::has_next ()
  {
    return true;
  }

  void genbb::_load_next (primary_event & event_, 
                          bool compute_classification_)
  {
    bool local_debug = _debug_;
    local_debug = false;
    if (local_debug)
      {
        clog << "debug: " << "genbb::genbb::_load_next: "
             << "Entering..."
             << endl;
      }
    event_.reset ();

    if (_buffer_item_ == _buffer_size_)
      {
        if (local_debug)
          {
            clog << "debug: " << "genbb::genbb::_load_next: "
                 << "Buffer is full !"
                 << endl;
          }
        _clean_ ();
        _init_ ();
      }
    std::string line;
    while (true)
      {
        std::string token;
        std::getline (*_genbb_in_, line);
        {
          std::istringstream line_iss (line);
          line_iss >> token;
        }
        if (token.empty ())
          {
            // skip white lines :
            continue;
          }
        if (token[0] != '#')
          {
            break;
          }
        if (token.length () == 1)
          {
            // skip empty comment lines :
            continue;
          }
        // parse special comment :
        if (token[1] != '@')
          {
            // skip pure comment lines :
            continue;
          }
        typedef vector<string> split_vector_type;
        split_vector_type svec;
        boost::algorithm::split(svec, line, 
                                boost::algorithm::is_any_of("="), 
                                boost::algorithm::token_compress_on );
        if (svec.size () != 2)
          {
            ostringstream message;
            message << "genbb::genbb::_load_next_genbb_: Invalid syntax ("
                    << line << ") !";
            throw logic_error (message.str ());
          } 
        if (svec[0] == "#@toallevents")
          {
            std::istringstream iss (svec[1]);
            double toallevents = 1.0;
            iss >> toallevents;
            if (! iss)
              {
                std::ostringstream message;
                message << "genbb::genbb::_load_next_genbb_: " 
                        << "Invalid format for 'toallevents' weight ("
                        << line << ") !";
                throw logic_error (message.str ());
              }
            if (toallevents < 1.0)
              {
                std::ostringstream message;
                message << "genbb::genbb::_load_next_genbb_: " 
                        << "Invalid value for 'toallevents' weight ("
                        << line << ") !";
                throw logic_error (message.str ());
              } 
            _genbb_weight_ = 1.0  / toallevents;
            std::clog << "NOTICE: genbb::genbb::_load_next_genbb_: "
                      << "Load GENBB event weight = " 
                      << _genbb_weight_ << std::endl;
          }
      }
    int evnum;
    double time;
    int npart;
    {
      std::istringstream line_iss (line);
      line_iss >> ws >> evnum >> time >> npart >> ws;
      if (! line_iss)
        {
          ostringstream message;
          message << "genbb::genbb_mgr::_load_next_genbb_: Format error !";
          throw logic_error (message.str ());
        }
    }
    /*
    *_genbb_in_ >> ws >> evnum >> time >> npart;
    if (! *_genbb_in_)
      {
        std::ostringstream message;
        message << "genbb::genbb::_load_next_genbb_: " 
                << "Invalid format at line '"
                << line << "' !";
        throw logic_error (message.str ());
            {
      std::istringstream line_iss (line);
      line_iss >> ws >> evnum >> time >> npart >> ws;
      if (! line_iss)
        {
          ostringstream message;
          message << "genbb::genbb_mgr::_load_next_genbb_: Format error !";
          throw logic_error (message.str ());
        }
    }

      }
    */
    event_.time = time * CLHEP::second;
    event_.set_genbb_weight (_genbb_weight_);
    double part_time = 0.0;
    for (int i = 0; i < npart; i++)
      {
        primary_particle pp;
        int part_type;
        double px, py ,pz, time_shift;
        *_genbb_in_ >> std::ws >> part_type >> px >> py >> pz >> time_shift;
        part_time += time_shift; 
        pp.set_type (part_type);
        pp.set_time (part_time * CLHEP::second); // GENBB unit is s
        geomtools::vector_3d p (px, py, pz);
        p *= CLHEP::MeV; // GENBB unit is MeV/c
        pp.set_momentum (p);
        event_.add_particle (pp);
      }
    *_genbb_in_ >> ws;
    _buffer_item_++;

    if (compute_classification_)
      {
        event_.compute_classification ();
      }
    _event_count_++;
    if (local_debug)
      {
        clog << "debug: " << "genbb::genbb::_load_next: "
             << "Exiting."
             << endl;
      }
    return;
  }

  size_t genbb::get_buffer_size () const
  {
    return _buffer_size_;
  }

  size_t genbb::get_event_count () const
  {
    return _event_count_;
  }

  void genbb::_init_ ()
  {
    if (_debug_)
      {
        clog << "debug: " << "genbb::genbb::_init_: "
             << "Entering..."
             << endl;
      }
    
    if (_decay_type_ == utils::INVALID_DECAY_TYPE)
      {
        ostringstream message;
        message << "genbb::genbb::_init_: " 
                << "Missing decay type !";
        _clean_ ();
        throw logic_error (message.str());      
      }

    if (_decay_isotope_.empty ())
      {
        ostringstream message;
        message << "genbb::genbb::_init_: " 
                << "Missing decay isotope !";
        _clean_ ();
        throw logic_error (message.str());      
      }

    if (_decay_type_ == utils::DECAY_TYPE_DBD)
      {
        const std::vector<std::string> & dbdnucs = utils::get_dbd_nuclides ();
        if (std::find(dbdnucs.begin (), dbdnucs.end (), _decay_isotope_) == dbdnucs.end ())
          {
            ostringstream message;
            message << "genbb::genbb::_init_: " 
                    << "Unknown isotope (" << _decay_isotope_ << ") !";
            _clean_ ();
            throw logic_error (message.str());          
          }
      }
    else if (_decay_type_ == utils::DECAY_TYPE_BACKGROUND)
      {
      }
    else
      {
        ostringstream message;
        message << "genbb::genbb::_init_: " 
                << "Invalid decay type !";
        _clean_ ();
        throw logic_error (message.str());      
      }

    // build the Genbb/Decay0 output data file name : 
    {
      ostringstream oss;
      oss << _tmp_dir_ << '/' << "data_" << _buffer_count_ << ".genbb";
      _genbb_data_ = oss.str ();
    }

    // reset counter:
    _buffer_item_ = 0; 
    datatools::utils::fetch_path_with_env (_genbb_data_);
 
    if (_debug_)
      {
        clog << "debug: " << "genbb::genbb::_init_: "
             << "Generate a new GENBB buffer data file '" << _genbb_data_ << "'..."
             << endl;
      }
   
    bool test = _test_;
    if (test)
      {
        if (_debug_)
          {
            clog << "debug: " << "genbb::genbb::_init_: "
                 << "Generate fake GENBB buffer data file '"
                 << _genbb_data_ << "'..."
                 << endl;
          }
        // generate fake GENBB buffer data:
        ofstream ofs (_genbb_data_.c_str ());
        for (int i = 0; i < _buffer_size_; i++)
          {
            ofs <<  "0  0.00000       2" << endl;
            ofs <<  "3  0.918076E-01 -0.958966      0.390933      0.00000  " << endl;
            ofs <<  "3   2.03066      -1.14024       1.57686      0.00000  " << endl;
          }
      }
    else
      {
        ostringstream genbb_cmd;
        string genbb_script = "genbb";
        const char * c = getenv ("GENBB_HELP_GENBB_SCRIPT");
        if (c != 0)
          {
            genbb_script = c;
            std::clog << "NOTICE: genbb::genbb::_init_: "
                      << "GENBB script : '" << genbb_script << "'."
                      << std::endl;
          }
        // else
        //   {
        //     ostringstream message;
        //     message << "genbb::genbb::_init_: " 
        //             << "Cannot find 'GENBB_HELP_GENBB_SCRIPT' environ variable !";
        //     _clean_ ();
        //     throw logic_error (message.str());
        //   }
        datatools::utils::fetch_path_with_env (genbb_script);
        
        unsigned long genbb_seed = grab_random ().uniform_int (0xFFFFFFF);

        ostringstream genbb_log_file_ss;
        genbb_log_file_ss << _tmp_dir_ << '/' << "genbb_" << _buffer_count_ << ".log";
        _genbb_log_ = genbb_log_file_ss.str ();
        genbb_cmd << "bash " << genbb_script << " "
                  << "--temp-directory" << " "
                  << _tmp_dir_ << " "
                  << _genbb_conf_ << " " 
                  << genbb_seed << " "
                  << _buffer_size_ << " "
                  << _genbb_data_ << " "
                  << " > " << _genbb_log_ << " 2>&1 ";
        std::clog << "DEVEL: " << "genbb::genbb::_init_: "
                  << "genbb_cmd='" << genbb_cmd.str () << "'"
                  << std::endl;
        int ret = system (genbb_cmd.str ().c_str ());
        if (ret != 0)
          {
            ostringstream message;
            message << "genbb::genbb::_init_: genbb command failed ! Check log file '"
                    << _genbb_log_ << "' !";
            _clean_ ();
            throw logic_error (message.str());
          }
      }

    if (_debug_)
      {
        clog << "debug: " << "genbb::genbb::_init_: "
             << "Opening GENBB buffer data file '"
             << _genbb_data_ << "'..."
             << endl;
      }
    _genbb_in_ = 0;
    _genbb_in_ = new ifstream ();
    _genbb_in_->open (_genbb_data_.c_str ());
    if (! *_genbb_in_)
      {
        ostringstream message;
        message << "genbb::genbb::_init_: Cannot open file '"
                << _genbb_data_ << "' !";
        if (_genbb_in_ != 0)
          {
            delete _genbb_in_;
            _genbb_in_ = 0;
          }
        _genbb_data_ = "";
        throw logic_error (message.str());
      }
    if (_debug_)
      {
        clog << "debug: " << "genbb::genbb::_init_: "
             << "Exiting."
             << endl;
      }
    return;
  }

} // end of namespace genbb

// end of genbb.cc
