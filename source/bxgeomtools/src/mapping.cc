// -*- mode: c++ ; -*-
/* mapping.cc
 */

#include <geomtools/mapping.h>

#include <stdexcept>
#include <sstream>
#include <geomtools/physical_volume.h>
#include <geomtools/model_factory.h>
#include <geomtools/id_mgr.h>
#include <geomtools/mapping_utils.h>

namespace geomtools {

  using namespace std;

  bool mapping::g_devel = false;

  /*** Properties manipulation ***/

  // static 
  const mapping::constants & 
  mapping::constants::instance ()
  {
    static boost::scoped_ptr<mapping::constants> g_global_constants (0);
    if ( g_global_constants.get () == 0)
      {
        g_global_constants.reset (new mapping::constants);
      }
    return *g_global_constants.get ();
  }

  mapping::constants::constants ()
  {
    
    MAPPING_PREFIX = "mapping.";
    MAPPING_DAUGHTER_ID_PREFIX = "daughter_id.";
    return;
  }

  string mapping::make_key (const string & key_)
  {
    ostringstream key_oss;
    key_oss << mapping::constants::instance().MAPPING_PREFIX << key_;
    return key_oss.str ();
  }

  void mapping::extract (const datatools::properties & source_,
                         datatools::properties & target_)
  {
    source_.export_starting_with (target_, mapping::constants::instance().MAPPING_PREFIX);
    return;
  }

  bool mapping::has_flag (const datatools::properties & config_,
                          const string & flag_)
  {
    return (config_.has_flag (mapping::make_key (flag_)));
  }

  bool mapping::has_key (const datatools::properties & config_,
                         const string & key_)
  {
    return (config_.has_key (mapping::make_key (key_)));
  }

  /******/

  bool mapping::is_initialized () const
  {
    return _initialized_;
  }

  bool mapping::is_debug () const
  {
    return _debug_;
  }

  void mapping::set_debug(bool d_)
  {
    std::cerr << "geomtools::mapping::set_debug: ****** DEBUG ******" << std::endl;
    _debug_ = d_;
    return;
  }

  bool mapping::is_build_mode_strict_mothership () const
  {
    return _build_mode_ == BUILD_MODE_STRICT_MOTHERSHIP;
  }

  bool mapping::is_build_mode_lazy_mothership () const
  {
    return _build_mode_ == BUILD_MODE_LAZY_MOTHERSHIP;
  }

  int mapping::get_build_mode () const
  {
    return _build_mode_;
  }

  void mapping::set_build_mode (int bm_)
  {
    if (is_initialized ())
      {
        throw logic_error ("geomtools::mapping::set_build_mode: Already initialized !");
      }
    if (bm_ != BUILD_MODE_STRICT_MOTHERSHIP)
      {
        throw logic_error ("geomtools::mapping::set_build_mode: Unsupported build mode !");
      }
    _build_mode_ = bm_;
    return;
  }

  bool mapping::is_mode_none () const
  {
    return _mode_ == MODE_NONE;
  }

  bool mapping::is_mode_only () const
  {
    return _mode_ == MODE_ONLY;
  }

  bool mapping::is_mode_excluded () const
  {
    return _mode_ == MODE_EXCLUDED;
  }

  void mapping::add_only (const string & category_)
  {
    if (is_initialized ())
      {
        throw logic_error ("geomtools::mapping::add_only: Already initialized !");
      }
    if (! is_mode_only ())
      {
        _only_excluded_list_.clear ();
        _mode_ = MODE_ONLY;
      }
    _only_excluded_list_.push_back (category_);
    return;
  }

  void mapping::add_excluded (const string & category_)
  {
    if (is_initialized ())
      {
        throw logic_error ("geomtools::mapping::add_excluded: Already initialized !");
      }
    if (! is_mode_excluded ())
      {
        _only_excluded_list_.clear ();
        _mode_ = MODE_EXCLUDED;
      }
    _only_excluded_list_.push_back (category_);
    return;
  }

  void mapping::initialize (const datatools::properties & config_)
  {
    if (is_initialized ())
      {
        throw logic_error ("geomtools::mapping::initialize: Already initialized !");
      }

    /*
    config_.tree_dump(std::cerr, "Mapping configuration : ", 
                      "DEBUG: geomtools::mapping::initialize: ");
    */

    if (config_.has_flag ("mapping.debug"))
      {
        //std::cerr << "geomtools::mapping::initialize: ****** FLAG DEBUG ******" << std::endl;
        set_debug (true);
      }

   if (config_.has_flag ("mapping.no_world_mapping"))
     {
       _world_mapping_ = false; 
     }

    set_build_mode(BUILD_MODE_STRICT_MOTHERSHIP);
    if (config_.has_key ("mapping.build_mode"))
      {
        std::string build_mode = config_.fetch_string ("mapping.build_mode");
        if (build_mode == "strict_mothership")
          {
            set_build_mode(BUILD_MODE_STRICT_MOTHERSHIP);
          }
        /*
        else if (build_mode == "lazy_mothership")
          {
            set_build_mode(BUILD_MODE_LAZY_MOTHERSHIP);
          }
        */
        else 
          {
            std::ostringstream message;
            message << "geomtools::mapping::initialize: Invalid build mode property value '"
                    << build_mode << "' !";
            throw std::logic_error (message.str ());
          }
      }

    if (config_.has_key ("mapping.max_depth"))
      {
        int mdepth = config_.fetch_integer ("mapping.max_depth");
        if (mdepth > 0)
          {
            set_max_depth (mdepth);
          }
      }

    bool has_only = false;
    if (config_.has_key ("mapping.only_categories"))
      {
        has_only = true;
        vector<string> only;
        config_.fetch ("mapping.only_categories", only);
        for (int i = 0; i < only.size (); i++)
          {
            add_only (only[i]);
          }
      }

    if (config_.has_key ("mapping.excluded_categories"))
      {
        if (has_only)
          {
            throw logic_error ("geomtools::mapping::initialize: The 'mapping.excluded_categories' property is not compatible with 'mapping.only_categories' property !");
          }
        vector<string> excluded;
        config_.fetch ("mapping.excluded_categories", excluded);
        for (int i = 0; i < excluded.size (); i++)
          {
            add_excluded (excluded[i]);
          }
      }

    /*
    std::cerr << "DEVEL: " 
              << "geomtools::mapping::initialize: "
              << "****** DONE ******" << std::endl;
    */

    _initialized_ = true;
    return;
  }

  // ctor:
  mapping::mapping () : geom_map ()
  {
    _initialized_ = false;
    _debug_       = false;
    _depth_       = 0;
    _max_depth_   = NO_MAX_DEPTH;
    _world_mapping_ = true;
    _factory_     = 0;
    _top_logical_ = 0;
    _mode_        = MODE_NONE;
    _build_mode_  = BUILD_MODE_DEFAULT;
    return;
  }

  // dtor:
  mapping::~mapping ()
  {
    return;
  }

  void mapping::set_max_depth (size_t max_depth_)
  {
    if (is_initialized ())
      {
        throw logic_error ("geomtools::mapping::set_max_depth: Already initialized !");
      }
    _max_depth_ = max_depth_;
    return;
  }

  size_t mapping::get_max_depth () const
  {
    return _max_depth_;
  }

  void mapping::build_from (const model_factory & factory_, const string & mother_)
  {
    bool devel = g_devel;
    //devel = true;
    if (devel)
      {
        clog << datatools::io::devel << "geomtools::mapping::build_from: Entering..." << endl;
      }

    if (! is_initialized ())
      {
        _initialized_ = true;
        //throw logic_error ("geomtools::mapping::build_from: Not initialized !");
     }
    
    if (! factory_.is_locked ())
      {
         throw logic_error ("geomtools::mapping::build_from: Factory is not locked !");
      }

    _factory_ = &factory_;
    models_col_type::const_iterator found
      = _factory_->get_models ().find (mother_);
    if (found == _factory_->get_models ().end ())
      {
        ostringstream message;
        message << "geomtools::mapping::build_from: Cannot find model '"
                << mother_ << "' !";
        throw logic_error (message.str ());
      }
    const i_model & top_model = *(found->second);
    _top_logical_ = &(top_model.get_logical ());
    _build_ ();
    if(is_debug())
      {
        dump_dictionnary (clog);
      }
    if (devel)
      {
        clog << datatools::io::devel
             << "geomtools::mapping::build_from: Exiting." << endl;
      }
    return;
  }

  void mapping::dump_dictionnary (ostream & out_) const
  {
    out_ << "--- Geometry ID mapping --- " << endl;
    for (geom_info_dict_type::const_iterator i
           = _get_geom_infos ().begin ();
         i != _get_geom_infos ().end ();
         i++)
      {
        out_ << i->first << ": LOG=`";
        if (i->second.has_logical ())
          {
            out_ << i->second.get_logical ().get_name ();
          }
        else
          {
            out_ << "<no logical>";
          }
        out_ << "'";
        out_ << " WP=`" << i->second.get_world_placement () << "'";
        out_ << endl;
      }
    out_ << "--------------------------- " <<  endl;
    return;
  }

  void mapping::smart_print (std::ostream & out_, 
                             const std::string & indent_,
                             uint32_t flags_) const
  {
    bool print_title = flags_ & PRINT_TITLE;
    bool print_pager = flags_ & PRINT_PAGER;
    if (print_title)
      {
        out_ << indent_ << "Geometry mapping dictionary : " << std::endl;
      }

    out_ << indent_ << "Maximum depth     : "
         << _max_depth_ << std::endl;

    out_ << indent_ << "Number of entries : "
         << _get_geom_infos ().size () << std::endl;
    
    int counter = 0;

    out_ << indent_ << "Dictionary of geometry informations : " << std::endl;
    for (geom_info_dict_type::const_iterator i
           = _get_geom_infos ().begin ();
         i != _get_geom_infos ().end ();
         i++)
      {
        geom_info_dict_type::const_iterator j = i;
        out_ << indent_;
        string tag1 ="|-- "; 
        string tag2 ="|   "; 
        if (++j == _get_geom_infos().end ())
          {
            tag1 ="`-- "; 
            tag2 ="|   "; 
          }
        out_ << tag1;
        out_ << "GID = " << i->first;
        const geom_info & ginfo = i->second;
        // To be done :
        //out_ << " within category '" << "?" << "'"; 
        if (! ginfo.has_logical ())
          {
            out_ << " (no logical!!!)";
          }
        out_ << std::endl;
        if (! ginfo.has_logical ())
          {
            continue;
          }
        out_ << indent_ << tag2 << "|-- " << "Logical   : '"
             << ginfo.get_logical ().get_name () << "'"
             << std::endl;
        out_ << indent_ << tag2 << "|-- " << "Material  : '"
             << ginfo.get_logical ().get_material_ref ()<< "'" << std::endl;
        out_ << indent_ << tag2 << "|-- "<< "Shape     : '"
             << ginfo.get_logical ().get_shape ().get_shape_name () 
             << "'" << std::endl;
        out_ << indent_ << tag2 << "|-- " << "World placement : ["
             << ginfo.get_world_placement () << "]" << std::endl;
        out_ << indent_ << tag2 << "`-- " << "Physicals : "
             << ginfo.get_logical ().get_physicals ().size () << std::endl;

        counter++;       
        if (counter % 5 == 0)
          {
            if (print_pager)
              {
                std::clog << std::endl 
                          << "Hit ENTER to display the next page or type '.q' to quit : ";
                std::string user;
                std::getline (std::cin, user);
                {
                  std::string token;
                  std::istringstream token_iss (user);
                  token_iss >> token >> ws;
                  if (token == ".q" || token == ".quit")
                    {
                      break;
                    }
                }
              }
          }
      }
    
    out_ << indent_ << "Collections of geometry informations by type : " << std::endl;
    for (ginfo_collections_with_type_dict_type::const_iterator i
           = get_geom_infos_with_type_map().begin ();
         i != get_geom_infos_with_type_map().end ();
         i++)
      {
        uint32_t type =  i->first;
        const ginfo_ptr_collection_type & col = i->second;
        ginfo_collections_with_type_dict_type::const_iterator j = i;
        out_ << indent_;
        if (++j == get_geom_infos_with_type_map().end ())
          {
            out_ << "`-- ";
          }
        else
          {
            out_ << "|-- ";
          }
        out_ << "Type = " << type << " : " << col.size () << " items" << std::endl;  
      }

    return;
  }

  void mapping::_build_ ()
  {
    bool devel = g_devel;

    if (devel)
      {
        clog << datatools::io::devel
             << "geomtools::mapping::_build_: Entering..." << endl;
      }
    string world_cat_name = "world";
    if (! _get_id_manager ().has_category_info (world_cat_name))
      {
        ostringstream message;
        message << "geomtools::mapping::_build_: Unknown category '"
                << world_cat_name << "' !";
        throw logic_error (message.str ());
      }
    {
      const id_mgr::category_info & world_cat_info
        = _get_id_manager ().get_category_info (world_cat_name);
      if (devel) world_cat_info.tree_dump (clog, "Category info: ");
      geom_id world_id;
      world_cat_info.create (world_id);
      world_id.set_address (0);
      if (devel)
        {
          clog << datatools::io::devel
               <<  "geomtools::mapping::_build_:"<< "World ID = " << world_id << ' '
               << (world_id.is_valid () ? "[Valid]": "[Invalid]")<< endl;
        }
      placement top_placement (vector_3d (0.0, 0.0, 0.0),
                               0,
                               0.0);

      //        const logical_volume * world_log
      //  = _factory_->get_logicals ().find ()->second;

      geom_info world_gi (world_id,
                          top_placement,
                          *_top_logical_);
      //dump_dictionnary (clog);

      // Add world volume mapping info :
      if (_world_mapping_)
        {
          _get_geom_infos ()[world_id] = world_gi;
        }
      if (is_build_mode_strict_mothership ())
        {
          _build_logical_children_ (*_top_logical_, top_placement, world_id);
        }
      else if (is_build_mode_lazy_mothership ())
        {
          std::vector<geom_id> parent_ids;
          parent_ids.push_back(world_id);
          _build_logical_children_2_ (*_top_logical_, top_placement, parent_ids);
        }
    }
    //dump_dictionnary (clog);
    if (devel)
      {
        clog << datatools::io::devel
             << "geomtools::mapping::_build_: Exiting." << endl;
      }
    return;
  }

  void mapping::_build_logical_children_ (const logical_volume & log_,
                                          const placement & mother_world_placement_,
                                          const geom_id & mother_id_)
  {
    bool devel = g_devel;
    if (is_debug())
      {
        devel = true;
      }
    _indenter_ (++_depth_);

    if (devel) clog << datatools::io::devel << _indenter_ << "geomtools::mapping::_build_logical_children_: Entering..." << endl;

    const logical_volume & log = log_;
    if (devel) clog << datatools::io::devel << _indenter_ << "geomtools::mapping::_build_logical_children_: Log = `" << log.get_name () << "'" << endl;
    if (log.get_physicals ().size () == 0)
      {
        if (devel) clog << datatools::io::devel << _indenter_ << "geomtools::mapping::_build_logical_children_: Exiting." << endl;
        _indenter_ (--_depth_);
        return;
      }
    /*
    if (devel)
      {
        log.parameters ().tree_dump (clog,
                                     "***** logical's parameters *****",
                                     "DEVEL: ");
      }
    */

    // Loop on children physical volumes:
    for (logical_volume::physicals_col_type::const_iterator i
           = log.get_physicals ().begin ();
         i != log.get_physicals ().end ();
         i++)
      {
        const string & phys_name = i->first;
        const physical_volume & phys_vol = *(i->second);
        if (devel) clog << datatools::io::devel << _indenter_ << "Physical '" << phys_name << "' : "
                        << "'" << phys_vol.get_name () << "' " << endl;

        const logical_volume & phys_logical = phys_vol.get_logical ();


        /*
          if (devel) phys_logical.parameters ().tree_dump (clog,
          "***** logical's parameters *****",
          "DEVEL: mapping::_build_logical_children_: ");
        */

        string daughter_label
          = i_model::extract_label_from_physical_volume_name (phys_vol.get_name ());
        if (devel) clog << datatools::io::devel << _indenter_
                        << "Daughter label = '"
                        << daughter_label << "' " << endl;
        string daughter_category_info;
        if (mapping_utils::has_daughter_id (log.parameters (),
                                            daughter_label))
          {
            mapping_utils::fetch_daughter_id (log.parameters (),
                                              daughter_label,
                                              daughter_category_info);
            if (devel)
              {
                clog << datatools::io::devel << _indenter_
                     << "Found daughter ID info for physical '"
                     << phys_name << "' " << endl;
              }
          }
        else
          {
            if (devel)
              {
                clog << datatools::io::devel << _indenter_
                     << "No daughter ID info for physical '"
                     << phys_name << "' " << endl;
              }
          }

        const i_placement & phys_placement = phys_vol.get_placement ();
        if (devel) clog << datatools::io::devel << _indenter_ << "-> Number of items: " << phys_placement.get_number_of_items () << endl;
        // Loop on replicated children physical volumes:
        for (int item = 0;
             item < phys_placement.get_number_of_items ();
             item++)
          {
            if (devel) clog << datatools::io::devel << _indenter_ << "-> item #" << item << ": " << endl;

            // get the current item placement
            // in the mother coordinates system:
            placement item_placement;
            phys_placement.get_placement (item, item_placement);
            {
              string tmp;
              placement::to_string (tmp, item_placement);
              if (devel) clog << datatools::io::devel << _indenter_ << "-> child placement " << tmp << " " << endl;
            }

            // compute the current item placement
            // in the world coordinates system:
            placement world_item_placement;
            mother_world_placement_.child_to_mother (item_placement,
                                                     world_item_placement);
            {
              string tmp;
              placement::to_string (tmp, world_item_placement);
              if (devel) clog << datatools::io::devel << _indenter_ << "-> world placement " << tmp << " " << endl;
            }
            int address = item;
            geom_id propagated_world_id = mother_id_;
            if (! daughter_category_info.empty ())
              {
                geom_id item_id;

                // compute the vector of sub-addresses:
                vector<uint32_t> items_index;
                phys_placement.compute_index_map (items_index, item);
                _get_id_manager ().compute_id_from_info (item_id,
                                                         mother_id_,
                                                         daughter_category_info,
                                                         items_index);

                if (_get_id_manager ().validate_id (item_id))
                  {
                    if (devel) clog << datatools::io::devel << _indenter_ << "-> Item ID " << item_id << " is added to the map: " << endl;
                    geom_info item_gi (item_id,
                                       world_item_placement,
                                       phys_logical);
                    bool add_it = true;
                    if (! is_mode_none ())
                      {
                        // get the category associated to the item ID:
                        const string & category
                          = _get_id_manager ().get_category_info (item_id.get_type ()).get_category ();
                        if (devel) clog << datatools::io::devel << _indenter_ << "geomtools::mapping::_build_logical_children_: "
                                        << "Category = '" << category << "' (from ID=" << item_id << ")"
                                        << endl;
                        if (is_mode_only ())
                          {
                            // the list contains only the categories to be registered:
                            list<string>::const_iterator found = find (_only_excluded_list_.begin (),
                                                                       _only_excluded_list_.end (),
                                                                       category);
                            if (found == _only_excluded_list_.end ())
                              {
                                add_it = false;
                              }
                          }
                        else if (is_mode_excluded ())
                          {
                            // the list contains only the categories to be excluded:
                            list<string>::const_iterator found = find (_only_excluded_list_.begin (),
                                                                       _only_excluded_list_.end (),
                                                                       category);
                            if (found != _only_excluded_list_.end ())
                              {
                                add_it = false;
                              }
                          }
                      }
                    if (add_it)
                      {
                        _get_geom_infos ()[item_id] = item_gi;
                      }
                    propagated_world_id = item_id;
                  }
              }

            // DEBUG: stop after traverse down to depth==max_depth:
            bool build_it = true;
            if (_max_depth_ > 0)
              {
                if (_depth_ > _max_depth_)
                  {
                    build_it = false;
                  }
              }
            if (build_it)
              {
                _build_logical_children_ (phys_logical,
                                          world_item_placement,
                                          propagated_world_id);
              }
            else
              {
                if (devel)
                  {
                    clog << datatools::io::devel << _indenter_ << "-> DO NOT TRAVERSE THE GEOMETRY TREE FURTHER."
                         << endl;
                  }
              }
          }
      }

    if (devel)
      {
        clog << datatools::io::devel
             << _indenter_ << "geomtools::mapping::_build_logical_children_: Exiting."
             << endl;
      }
    _indenter_ (--_depth_);
    return;
  }

  void mapping::_build_logical_children_2_ (const logical_volume & log_,
                                            const placement & mother_world_placement_,
                                            const std::vector<geom_id> & mother_ids_)
  {
    bool devel = g_devel;
    if (is_debug())
      {
        devel = true;
      }
    _indenter_ (++_depth_);
    
    if (devel) clog << datatools::io::devel << _indenter_ << "geomtools::mapping::_build_logical_children_2_: Entering..." << endl;
    
    const logical_volume & log = log_;
    if (devel) clog << datatools::io::devel << _indenter_ << "geomtools::mapping::_build_logical_children_2_: Log = `" << log.get_name () << "'" << endl;
    if (log.get_physicals ().size () == 0)
      {
        if (devel) clog << datatools::io::devel << _indenter_ << "geomtools::mapping::_build_logical_children_2_: Exiting." << endl;
        _indenter_ (--_depth_);
        return;
      }
    // if (devel)
    //   {
    //     log.parameters ().tree_dump (clog,
    //                                  "***** logical's parameters *****",
    //                                  "DEVEL: ");
    //   }

    // Loop on children physical volumes:
    for (logical_volume::physicals_col_type::const_iterator i
           = log.get_physicals ().begin ();
         i != log.get_physicals ().end ();
         i++)
      {
        const string & phys_name = i->first;
        const physical_volume & phys_vol = *(i->second);
        if (devel) clog << datatools::io::devel << _indenter_ << "Physical '" << phys_name << "' : "
                        << "'" << phys_vol.get_name () << "' " << endl;

        const logical_volume & phys_logical = phys_vol.get_logical ();


        /*
          if (devel) phys_logical.parameters ().tree_dump (clog,
          "***** logical's parameters *****",
          "DEVEL: mapping::_build_logical_children_2_: ");
        */

        string daughter_label
          = i_model::extract_label_from_physical_volume_name (phys_vol.get_name ());
        if (devel) clog << datatools::io::devel << _indenter_
                        << "Daughter label = '"
                        << daughter_label << "' " << endl;
        string daughter_category_info;
        if (mapping_utils::has_daughter_id (log.parameters (),
                                            daughter_label))
          {
            mapping_utils::fetch_daughter_id (log.parameters (),
                                              daughter_label,
                                              daughter_category_info);
            if (devel)
              {
                clog << datatools::io::devel << _indenter_
                     << "Found daughter ID info for physical '"
                     << phys_name << "' " << endl;
              }
          }
        else
          {
            if (devel)
              {
                clog << datatools::io::devel << _indenter_
                     << "No daughter ID info for physical '"
                     << phys_name << "' " << endl;
              }
          }
       if (devel) clog << datatools::io::devel << _indenter_
                        << "Daughter category info = '"
                        << daughter_category_info << "' " << endl;

        const i_placement & phys_placement = phys_vol.get_placement ();
        if (devel) clog << datatools::io::devel << _indenter_ << "-> Number of items: " << phys_placement.get_number_of_items () << endl;

        //////////////////////////////////////////////////
        // Loop on replicated children physical volumes //
        //////////////////////////////////////////////////
        for (int item = 0;
             item < phys_placement.get_number_of_items ();
             item++)
          {
            if (devel) clog << datatools::io::devel << _indenter_ << "-> item #" << item << ": " << endl;

            // get the current item placement
            // in the mother coordinates system:
            placement item_placement;
            phys_placement.get_placement (item, item_placement);
            {
              string tmp;
              placement::to_string (tmp, item_placement);
              if (devel) clog << datatools::io::devel << _indenter_ << "-> child placement " << tmp << " " << endl;
            }

            // compute the current item placement
            // in the world coordinates system:
            placement world_item_placement;
            mother_world_placement_.child_to_mother (item_placement,
                                                     world_item_placement);
            {
              string tmp;
              placement::to_string (tmp, world_item_placement);
              if (devel) clog << datatools::io::devel << _indenter_ << "-> world placement " << tmp << " " << endl;
            }
            int address = item;
            std::vector<geom_id> propagated_world_ids = mother_ids_;
            //geom_id propagated_world_id = mother_id_;
            if (! daughter_category_info.empty ())
              {
                geom_id item_id;
                bool match = false;
                int imother = 0;
                while (! match && imother < propagated_world_ids.size())
                  {
                    // For this level of mother volume :
                    const geom_id & mother_id = propagated_world_ids[imother];
                    
                    // compute the vector of sub-addresses:
                    vector<uint32_t> items_index;
                    phys_placement.compute_index_map (items_index, item);
                    try 
                      {
                        _get_id_manager ().compute_id_from_info (item_id,
                                                                 mother_id,
                                                                 daughter_category_info,
                                                                 items_index);
                        match = true;
                      }
                    catch(std::exception & x)
                      {
                        std::cerr <<  datatools::io::warning 
                                  << "geomtools::mapping::_build_logical_children_2_: " 
                                  << "Cannot match a GID scheme for mother with GID=" << mother_id
                                  << " !"
                                  << endl;
                        imother++;
                        continue;
                      }
                    // In case of success :
                    if (_get_id_manager ().validate_id (item_id))
                      {
                        if (devel) clog << datatools::io::devel << _indenter_ << "-> Item ID " << item_id << " is added to the map: " << endl;
                        geom_info item_gi (item_id,
                                           world_item_placement,
                                           phys_logical);
                        bool add_it = true;
                        if (! is_mode_none ())
                          {
                            // get the category associated to the item ID:
                            const string & category
                              = _get_id_manager ().get_category_info (item_id.get_type ()).get_category ();
                            if (devel) clog << datatools::io::devel << _indenter_ << "geomtools::mapping::_build_logical_children_2_: "
                                            << "Category = '" << category << "' (from ID=" << item_id << ")"
                                            << endl;
                            if (is_mode_only ())
                              {
                                // the list contains only the categories to be registered:
                                list<string>::const_iterator found = find (_only_excluded_list_.begin (),
                                                                           _only_excluded_list_.end (),
                                                                           category);
                                if (found == _only_excluded_list_.end ())
                                  {
                                    add_it = false;
                                  }
                              }
                            else if (is_mode_excluded ())
                              {
                                // the list contains only the categories to be excluded:
                                list<string>::const_iterator found = find (_only_excluded_list_.begin (),
                                                                           _only_excluded_list_.end (),
                                                                           category);
                                if (found != _only_excluded_list_.end ())
                                  {
                                    add_it = false;
                                  }
                              }
                          }
                        if (add_it)
                          {
                            _get_geom_infos ()[item_id] = item_gi;
                          }
                        propagated_world_ids.insert(propagated_world_ids.begin(), item_id);
                      }
                    //
                    imother++;
                  }
                if (! match)
                  {
                    // XXX
                    bool expect_mother = true;
                    if (expect_mother)
                      {
                        std::ostringstream message;
                        message <<  datatools::io::warning 
                                << "geomtools::mapping::_build_logical_children_2_: " 
                                << "Cannot build any GID scheme from all levels of mothership from GIDs={";
                        for (int i = 0; i < propagated_world_ids.size(); i++)
                          {
                            if (i > 0) message << ", ";
                            message << propagated_world_ids[i];
                          }
                        message << "} !";
                        throw std::logic_error (message.str());
                      }
                    else
                      {
                        propagated_world_ids.clear();
                      }
                  }
              }

            // DEBUG: stop after traverse down to depth==max_depth:
            bool build_it = true;
            if (_max_depth_ > 0)
              {
                if (_depth_ > _max_depth_)
                  {
                    build_it = false;
                  }
              }
            if (build_it)
              {
                _build_logical_children_2_ (phys_logical,
                                            world_item_placement,
                                            propagated_world_ids);
              }
            else
              {
                if (devel)
                  {
                    clog << datatools::io::devel << _indenter_ << "-> DO NOT TRAVERSE THE GEOMETRY TREE FURTHER."
                         << endl;
                  }
              }
          }
      }
    
    if (devel)
      {
        clog << datatools::io::devel
             << _indenter_ << "geomtools::mapping::_build_logical_children_2_: Exiting."
             << endl;
      }
    _indenter_ (--_depth_);
    return;
  }

} // end of namespace geomtools

// end of mapping.cc
