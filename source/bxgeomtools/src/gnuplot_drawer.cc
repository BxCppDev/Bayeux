// -*- mode: c++ ; -*-
/* gnuplot_drawer.cc
 */

#include <geomtools/gnuplot_drawer.h>

#include <stdexcept>
#include <cstdlib>

#include <datatools/i_tree_dump.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

#include <geomtools/model_factory.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/color.h>
#include <geomtools/visibility.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/display_data.h>
#include <geomtools/geom_id.h>
#include <geomtools/geom_info.h>
#include <geomtools/mapping.h>
#include <geomtools/manager.h>

namespace geomtools {

  using namespace std;

  const string gnuplot_drawer::VIEW_KEY   = "view";
  const string gnuplot_drawer::VIEW_2D_XY = "xy";
  const string gnuplot_drawer::VIEW_2D_XZ = "xz";
  const string gnuplot_drawer::VIEW_2D_YZ = "yz";
  const string gnuplot_drawer::VIEW_3D    = "xyz";
  const string gnuplot_drawer::VIEW_3D_FREE_SCALE = "xyz_free";
  const string gnuplot_drawer::DEFAULT_VIEW = gnuplot_drawer::VIEW_3D;

  const int    gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT = 1000;

  const string gnuplot_drawer::MODE_WIRED = "wired";
  const string gnuplot_drawer::MODE_SOLID = "solid";
  const string gnuplot_drawer::DEFAULT_MODE = MODE_WIRED;

  const string gnuplot_drawer::FORCE_SHOW_PROPERTY_NAME          = "visibility.force_show";
  const string gnuplot_drawer::FORCE_SHOW_ENVELOP_PROPERTY_NAME  = "visibility.force_show_envelop";
  const string gnuplot_drawer::FORCE_SHOW_CHILDREN_PROPERTY_NAME = "visibility.force_show_children";

  const std::string gnuplot_drawer::WORLD_NAME_KEY = "world_name";


  bool gnuplot_drawer::g_devel = false;

  gnuplot_drawer::cstream::cstream ()
  {
    oss = 0;
    return;
  }

  /**********************************************************************/

  bool gnuplot_drawer::dd_entry::is_valid() const
  {
    return _dd_address_ != 0 && _pl_.is_valid ();
  }

  gnuplot_drawer::dd_entry::dd_entry()
  {
    _dd_address_ = 0;
    return;
  }

  void gnuplot_drawer::dd_entry::reset()
  {
    _dd_address_ = 0;
    _pl_.reset();
    return;
  }

  void gnuplot_drawer::dd_entry::set_placement(const placement & pl_)
  {
    _pl_ = pl_;
    return;
  }

  void gnuplot_drawer::dd_entry::set_display_data(const display_data & dd_)
  {
    _dd_address_ = &dd_;
    return;
  }

  const placement & gnuplot_drawer::dd_entry::get_placement() const
  {
    return _pl_;
  }

  const display_data & gnuplot_drawer::dd_entry::get_display_data() const
  {
    return *_dd_address_;
  }

  gnuplot_drawer::has_dd_addr::has_dd_addr(const display_data& dd_) : _dd_address_(&dd_)
  {
    return;
  }

  bool gnuplot_drawer::has_dd_addr::operator()(const dd_entry & dde_) const
  {
    DT_THROW_IF (! dde_.is_valid(), std::logic_error, "Invalid dd_entry instance !");
    return _dd_address_ == &dde_.get_display_data();
  }

  /**********************************************************************/

  gnuplot_drawer::range::range (char axis_)
  {
    axis = 0;
    set_axis (axis_);
    min = std::numeric_limits<double>::quiet_NaN ();
    max = std::numeric_limits<double>::quiet_NaN ();
    return;
  }

  void gnuplot_drawer::range::set_axis (char axis_)
  {
    axis = axis_;
    return;
  }

  void gnuplot_drawer::range::reset ()
  {
    min = std::numeric_limits<double>::quiet_NaN ();
    max = std::numeric_limits<double>::quiet_NaN ();
    return;
  }

  void gnuplot_drawer::range::print (std::ostream & out_) const
  {
    out_ << "set " << axis << "range [" << min << ':' << max << "]";
    return;
  }


  void gnuplot_drawer::wait_for_key ()
  {
    /*
      #if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
      clog << endl << "Press ENTER to continue..." << endl;
      cin.clear ();
      cin.ignore (cin.rdbuf ()->in_avail ());
      cin.get ();
      #endif
    */
    clog << endl << "Press [Enter] to continue..." << endl;
    std::string s;
    std::getline (std::cin, s);
    return;
  }

  void gnuplot_drawer::set_view (const string & view_)
  {
    _view_ = view_;
    return;
  }

  const string & gnuplot_drawer::get_view () const
  {
    return _view_;
  }

  void gnuplot_drawer::set_mode (const string & mode_)
  {
    _mode_ = mode_;
    return;
  }

  const string & gnuplot_drawer::get_mode () const
  {
    return _mode_;
  }

  datatools::properties & gnuplot_drawer::grab_properties ()
  {
    return _props_;
  }

  const datatools::properties & gnuplot_drawer::get_properties () const
  {
    return _props_;
  }

  bool gnuplot_drawer::is_view_2d () const
  {
    return ! is_view_3d ();
  }

  bool gnuplot_drawer::is_view_3d () const
  {
    return _view_ == VIEW_3D;
  }

  bool gnuplot_drawer::is_solid () const
  {
    return _mode_ == MODE_SOLID;
  }

  bool gnuplot_drawer::is_wired () const
  {
    return _mode_ == MODE_WIRED;
  }

  ostringstream & gnuplot_drawer::_get_stream (const string & section_)
  {
    bool devel = g_devel;
     cstreams_col_type::iterator i = _cstreams_.find (section_);
    if (i == _cstreams_.end ())
      {
        cstream new_cs;
        new_cs.label = section_;
        new_cs.filename = "";
        new_cs.oss = new ostringstream;
        new_cs.color = color::get_color (section_);
        _cstreams_[section_] = new_cs;
        return *(new_cs.oss);
      }
    cstream & cs = i->second;
    return *(cs.oss);
  }

  void gnuplot_drawer::set_labels (bool labels_)
  {
    _labels_ = labels_;
    return;
  }

  bool gnuplot_drawer::use_labels () const
  {
    return _labels_;
  }

  void gnuplot_drawer::add_display_data(const display_data & dd_)
  {
    const placement pl;
    add_display_data(dd_,pl);
  }

  void gnuplot_drawer::add_display_data(const display_data & dd_, const placement & pl_)
  {
    // gnuplot_drawer::has_dd_addr dd_addr_predicate (dd_);
    // if (std::find_if(_display_data_.begin(),
    //               _display_data_.end(),
    //               dd_addr_predicate) == _display_data_.end())
    //   {
        {
          dd_entry dummy;
          _display_data_.push_back(dummy);
        }
        dd_entry & dde = _display_data_.back();
        dde.set_placement(pl_);
        dde.set_display_data(dd_);
    // }
    return;
  }

  void gnuplot_drawer::reset_display_data()
  {
    _display_data_.clear();
    return;
  }

  // ctor:
  gnuplot_drawer::gnuplot_drawer ()
  {
    _initialized_ = false;
    _view_ = gnuplot_drawer::DEFAULT_VIEW;
    _mode_ = gnuplot_drawer::DEFAULT_MODE;
    _labels_ = true;
    _xrange_.set_axis ('x');
    _yrange_.set_axis ('y');
    _zrange_.set_axis ('z');
    return;
  }

  gnuplot_drawer::~gnuplot_drawer ()
  {
    reset ();
    return;
  }

  void gnuplot_drawer::reset_cstreams ()
  {
    //clog << "DEVEL: gnuplot_drawer::reset_cstreams: Entering..." << endl;
    if (_cstreams_.size () > 0)
      {
        for (cstreams_col_type::iterator i = _cstreams_.begin ();
             i != _cstreams_.end ();
             i++)
          {
            //clog << "DEVEL: gnuplot_drawer::reset_cstreams: Label == " << i->first << endl;
            cstream & cs = i->second;
            if (cs.oss != 0)
              {
                cs.oss->flush ();
                //clog << "DEVEL: gnuplot_drawer::reset_cstreams: oss to be deleted..." << cs.oss << endl;
                delete cs.oss;
                cs.oss = 0;
                //clog << "DEVEL: gnuplot_drawer::reset_cstreams: done." << endl;
              }
          }
        _cstreams_.clear ();
      }
    //clog << "DEVEL: gnuplot_drawer::reset_cstreams: Exiting." << endl;
    return;
  }

  void gnuplot_drawer::reset ()
  {
    reset_cstreams ();
    _view_ = gnuplot_drawer::DEFAULT_VIEW;
    _mode_ = gnuplot_drawer::DEFAULT_MODE;
    _labels_ = true;
    _initialized_ = false;
    return;
  }

  /****************************************************/

  void gnuplot_drawer::_draw_ (const logical_volume & log_,
                               const placement & p_,
                               int max_display_level_)
  {
    bool devel = g_devel;
    int max_display_level = max_display_level_;
    if (max_display_level_ < 0)
      {
        max_display_level = 0;
      }
    const geomtools::logical_volume & log = log_;
    try
      {
        int display_level = 0;

        datatools::properties log_visu_config;
        visibility::extract (log.parameters (), log_visu_config);

        // if (devel)
        //   {
        //     ostringstream toss;
        //     toss << "Logical '" << log.get_name () << "' visibility properties:";
        //     log_visu_config.tree_dump (cerr, toss.str (), " DEVEL: ");
        //   }

        bool shown = true;
        bool shown_envelop = true;
        if (visibility::is_shown (log_visu_config))
          {
            shown = true;
          }
        if (visibility::is_hidden (log_visu_config))
          {
            shown = false;
          }

        if (get_properties ().has_key (FORCE_SHOW_PROPERTY_NAME))
          {
            shown = get_properties ().fetch_boolean (FORCE_SHOW_PROPERTY_NAME);
          }


        if (visibility::is_hidden_envelop (log_visu_config))
          {
            shown_envelop = false;
          }
        // if (devel)
        //   {
        //     cerr << "DEVEL: gnuplot_drawer::_draw_: Show         = " << shown << endl;
        //     cerr << "DEVEL: gnuplot_drawer::_draw_: Show envelop = " << shown_envelop << endl;
        //   }

        if (get_properties ().has_key (FORCE_SHOW_ENVELOP_PROPERTY_NAME))
          {
            shown_envelop = get_properties ().fetch_boolean (FORCE_SHOW_ENVELOP_PROPERTY_NAME);
          }

        if (shown && shown_envelop)
          {
            if (is_wired ())
              {
                string color_label = color::constants::instance ().default_color;
                if (visibility::has_color (log_visu_config))
                  {
                    color_label = visibility::get_color (log_visu_config);
                    // if (devel)
                    //   {
                    //     cerr << "DEVEL: gnuplot_drawer::_draw_: Found color '" << color_label
                    //          << "' for logical '" << log.get_name () << "'..." << endl;
                    //   }
                  }
                else
                  {
                    // if (devel)
                    //   {
                    //     cerr << "DEVEL: gnuplot_drawer::_draw_: No color specification for logical '" << log.get_name () << "' !" << endl;
                    //   }
                  }
                // if (devel)
                //   {
                //     cerr << "DEVEL: gnuplot_drawer::_draw_: color_label  = '" << color_label << "'" << endl;
                //   }

                ostringstream & colored_oss = _get_stream (color_label);
                // if (devel)
                //   {
                //     cerr << "DEVEL: gnuplot_drawer::_draw_: "
                //          << "gnuplot_draw::draw: &colored_oss = " << &colored_oss << endl;
                //     cerr << "DEVEL: gnuplot_drawer::_draw_: gnuplot_draw::draw..." << endl;
                //     /*
                //     // test:
                //     colored_oss << "0 0 0" << endl;
                //     colored_oss << "1 0 0" << endl << endl;
                //     colored_oss << "0 0 0" << endl;
                //     colored_oss << "0 1 0" << endl << endl;
                //     colored_oss << "0 0 0" << endl;
                //     colored_oss << "0 0 1" << endl << endl;
                //     */
                //   }

                unsigned long mode = gnuplot_draw::MODE_NULL;
                if (visibility::is_wired_cylinder (log_visu_config))
                  {
                    mode |= gnuplot_draw::MODE_WIRED_CYLINDER;
                  }
                gnuplot_draw::draw (colored_oss,
                                    p_,
                                    log.get_shape (),
                                    mode);

               }
            else
              {
                // if (devel)
                //   {
                //     cerr << "WARNING: gnuplot_drawer::_draw_: "
                //          << "Display mode '" << _mode_ << "'is not implemented !"
                //          << endl;
                //   }
              }
          }

        // draw children:
        bool draw_children = true;

        // check the display level of the geometry tree:
        if ((display_level < max_display_level)
            && (log.get_physicals ().size () > 0))
          {
            // default is 'drawing children':
            draw_children = true;
          }
        else
          {
            draw_children = false;
          }

        // test if it is forbidden by the visibility properties:
        if (! shown)
          {
            draw_children = false;
          }
        else if (visibility::is_daughters_hidden (log_visu_config))
          {
            draw_children = false;
          }

        if (get_properties ().has_key (FORCE_SHOW_CHILDREN_PROPERTY_NAME))
          {
            draw_children = get_properties ().fetch_boolean (FORCE_SHOW_CHILDREN_PROPERTY_NAME);
          }

        // if (devel)
        //   {
        //     cerr << "DEVEL: gnuplot_drawer::_draw_: "
        //          << "Drawing children..."
        //          << endl;
        //   }
        display_level++;
        for (geomtools::logical_volume::physicals_col_type::const_iterator i
               = log.get_physicals ().begin ();
             i != log.get_physicals ().end (); i++)
          {
            bool draw_it = true;
            const physical_volume & phys = *(i->second);
            const geomtools::logical_volume & log_child = phys.get_logical ();
            if (! draw_children)
              {
                draw_it = false;
              }
            if (log_child.parameters ().has_flag ("visibility.shown"))
              {
                draw_it = true;
              }
            if (log_child.parameters ().has_flag ("visibility.hidden"))
              {
                draw_it = false;
              }
            if (! draw_it)
              {
                continue;
              }
            const geomtools::i_placement * pp = &(phys.get_placement ());
            size_t npp = 0;
            npp = pp->get_number_of_items ();
            for (int i = 0; i < npp; i++)
              {
                geomtools::placement p;
                // get placement from the daughter physical #i:
                pp->get_placement (i, p);
                //pp->tree_dump (clog, "item placement (child)", "    ");
                geomtools::placement pt = p;
                // compute the placement relative to the mother volume:
                p_.child_to_mother (p, pt);
                //pt.tree_dump (clog, "item placement (mother)", "    ");

                // recursive invocation of the visualization data generation
                // for daughter #i:
                gnuplot_drawer::_draw_ (log_child,
                                        pt,
                                        max_display_level - 1);
              }
          }
      }
    catch (GnuplotException ge)
      {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR, ge.what ());
      }
    catch (exception & x)
      {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR, x.what ());
      }
    catch (...)
      {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Unexpected error !");
      }
    // if (devel)
    //   {
    //     cerr << "DEVEL: gnuplot_drawer::_draw_: Exiting." << endl;
    //   }
    return;
  }

  /****************************************************/

  void gnuplot_drawer::draw (const logical_volume & log_,
                             const placement & p_,
                             int max_display_level_,
                             const string & name_)
  {
    bool devel = g_devel;
    //devel = true;
    int max_display_level = max_display_level_;
    if (max_display_level_ < 0)
      {
        max_display_level = 0;
      }
    const geomtools::logical_volume & log = log_;

    datatools::properties visu_config;
    visibility::extract (log_.parameters (), visu_config);

    bool shown = true;
    bool shown_envelop = true;
    if (visibility::is_hidden (visu_config))
      {
        shown = false;
      }
    if (visibility::is_shown (visu_config))
      {
        shown = true;
      }
    if (visibility::is_hidden_envelop (visu_config))
      {
        shown_envelop = false;
      }

    int color = 1;
    int former_color = color::get_color (color::constants::instance().default_color);
    if (visibility::has_color (visu_config))
      {
        color = color::get_color (visibility::get_color (visu_config));
        // if (devel)
        //   {
        //     cerr << "DEVEL: **** Found color '"
        //          << visibility::get_color (visu_config)
        //          << "' color is : " << color << endl;
        //   }
      }

    gnuplot_draw::g_current_color = color;
    gnuplot_draw::g_current_color = former_color;

    // Instantiate a XYZ range computer :
    gnuplot_draw::xyz_range * xyzr = gnuplot_draw::xyz_range::instance ('i');


    if (shown)
      {
        gnuplot_drawer::_draw_ (log_, p_, max_display_level);
      }

    _xrange_.min = xyzr->x_range.get_min ();
    _xrange_.max = xyzr->x_range.get_max ();
    _yrange_.min = xyzr->y_range.get_min ();
    _yrange_.max = xyzr->y_range.get_max ();
    _zrange_.min = xyzr->z_range.get_min ();
    _zrange_.max = xyzr->z_range.get_max ();

    double dx = _xrange_.max - _xrange_.min;
    double dy = _yrange_.max - _yrange_.min;
    double dz = _zrange_.max - _zrange_.min;
    double amax = std::max (dx, dy);
    amax = std::max (amax, dz);
    amax *= 1.1;
    _xrange_.min = xyzr->x_range.get_median () - 0.5 * amax;
    _xrange_.max = xyzr->x_range.get_median () + 0.5 * amax;
    _yrange_.min = xyzr->y_range.get_median () - 0.5 * amax;
    _yrange_.max = xyzr->y_range.get_median () + 0.5 * amax;
    _zrange_.min = xyzr->z_range.get_median () - 0.5 * amax;
    _zrange_.max = xyzr->z_range.get_median () + 0.5 * amax;

    //std::clog << "X-range = ";
    //_xrange_.print (std::clog);
    //std::clog << "\nY-range = ";
    //_yrange_.print (std::clog);
    //std::clog << "\nZ-range = ";
    //_zrange_.print (std::clog);
    //    std::clog << "\nMax width = " << amax << std::endl;

    // Clear the XYZ range computer :
    gnuplot_draw::xyz_range::instance ('c');

    for (cstreams_col_type::iterator i = _cstreams_.begin ();
         i != _cstreams_.end ();
         i++)
      {
        const string & label = i->first;
        cstream & cs = i->second;
        char tmp[256];
        ostringstream oss;
        oss << ".tmp_gnuplot_drawer_" << label <<"_XXXXXX";
        sprintf (tmp, "%s", oss.str ().c_str ());
        char * c = mktemp (tmp);
        string tmp_filename = tmp;
        cs.filename = tmp;
        ofstream tmp_file (cs.filename.c_str ());
        tmp_file << "# label='" << label << "'" << endl;
        tmp_file << cs.oss->str ();
        tmp_file << "# end of label='" << label << "'" << endl;
        tmp_file.flush ();
        tmp_file.close ();
      }
    usleep (200);

    std::string view = _view_;

    Gnuplot g1 ("lines");
    g1.cmd ("set terminal x11 persist size 500,500");
    g1.cmd ("set size ratio -1");
    std:ostringstream title_oss;
    title_oss << "Logical '" << name_ << "'";
    g1.set_title (title_oss.str ());

    std::ostringstream cmdstr;

    int col1 = 1;
    int col2 = 2;
    int col3 = 3;
    size_t plot_dim = 2;
    bool labels = _labels_;
    if (! labels)
      {
        g1.cmd ("unset border");
        g1.cmd ("unset xtics");
        g1.cmd ("unset ytics");
        g1.cmd ("unset ztics");
      }
    if (view == gnuplot_drawer::VIEW_2D_XY)
      {
        if (labels)
          {
            g1.set_xlabel ("x").set_ylabel ("y");
            g1.cmd ("set xtics");
            g1.cmd ("set ytics");
          }
        col1 = 1;
        col2 = 2;
      }

    if (view == gnuplot_drawer::VIEW_2D_XZ)
      {
        if (labels)
          {
            g1.set_xlabel ("x").set_ylabel ("z");
            g1.cmd ("set xtics");
            g1.cmd ("set ztics");
          }
        col1 = 1;
        col2 = 3;
      }

    if (view == gnuplot_drawer::VIEW_2D_YZ)
      {
        if (labels)
          {
            g1.set_xlabel ("y").set_ylabel ("z");
            g1.cmd ("set ytics");
            g1.cmd ("set ztics");
          }
        col1 = 2;
        col2 = 3;
      }

    if (view == gnuplot_drawer::VIEW_3D)
      {
        plot_dim = 3;
        g1.cmd ("set view equal xyz");
        {
          std::ostringstream xr_oss;
          _xrange_.print (xr_oss);
          g1.cmd (xr_oss.str ());
        }
        {
          std::ostringstream yr_oss;
          _yrange_.print (yr_oss);
          g1.cmd (yr_oss.str ());
        }
        {
          std::ostringstream zr_oss;
          _zrange_.print (zr_oss);
          g1.cmd (zr_oss.str ());
        }
        //g1.cmd ("set iso 100, 100");
        //g1.cmd ("set view 30,60,1.");
        std::ostringstream cmd_oss;
        cmd_oss << "set xyplane at " << _zrange_.min;
        g1.cmd (cmd_oss.str ());
        if (labels)
          {
            g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
            g1.cmd ("set xtics");
            g1.cmd ("set ytics");
            g1.cmd ("set ztics");
          }
        col1 = 1;
        col2 = 2;
        col3 = 3;
      }

    if (view == gnuplot_drawer::VIEW_3D_FREE_SCALE)
      {
        plot_dim = 3;
        if (labels)
          {
            g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
            g1.cmd ("set xtics");
            g1.cmd ("set ytics");
            g1.cmd ("set ztics");
          }
        col1 = 1;
        col2 = 2;
        col3 = 3;
      }

    if (_cstreams_.size ())
      {
        if (plot_dim == 2)
          {
            g1.set_grid ();
            g1.cmd ("set size ratio -1");
            cmdstr << "plot ";
          }
        else
          {
            cmdstr << "splot ";
          }
        for (cstreams_col_type::const_iterator i = _cstreams_.begin ();
             i != _cstreams_.end ();
             i++)
          {
            const cstream & cs = i->second;
            cmdstr << "\"" << cs.filename << "\" using " << col1 << ":" << col2;
            int color = cs.color;
            if (plot_dim == 3)
              {
                cmdstr << ":" << col3;
              }
            cmdstr << " notitle";
            cmdstr << " with lines lt " << color << ' ' ;
            cmdstr << " lw " << 0.5 << ' ' ;

            // if there is another one:
            {
              cstreams_col_type::const_iterator j = i;
              j++;
              if (j != _cstreams_.end ())
                {
                  cmdstr << ", ";
                }
            }
          }
      }
    // if (devel)
    //   {
    //     cerr << "DEVEL: gnuplot_drawer::draw: GNUPLOT command is :" << endl
    //          << cmdstr.str () << endl;
    //   }
    if (! cmdstr.str ().empty ())
      {
        g1.cmd (cmdstr.str ());
        g1.showonscreen (); // window output
        wait_for_key ();
        usleep (200);
      }
    else
      {
        DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "Nothing to display !");
      }

    // remove tmp files:
    for (cstreams_col_type::iterator i = _cstreams_.begin ();
         i !=  _cstreams_.end ();
         i++)
      {
        cstream & cs = i->second;
        unlink (cs.filename.c_str ());
      }
    // if (devel)
    //   {
    //     cerr << "DEVEL: gnuplot_drawer::draw: "
    //          << "reset_cstreams..." << endl;
    //   }
    reset_cstreams ();
    return;
  }

  /****************************************************/

  void gnuplot_drawer::draw_logical (const model_factory & mf_,
                                     const string & logical_name_,
                                     const placement & p_,
                                     int max_display_level_)
  {
    geomtools::logical_volume::dict_type::const_iterator found;
    found = mf_.get_logicals ().find (logical_name_);
    DT_THROW_IF (found ==  mf_.get_logicals ().end (),
                 logic_error,
                 "Cannot find logical volume with name '" << logical_name_ << "' !");
    const geomtools::logical_volume & log = *(found->second);
    draw (log, p_, max_display_level_, log.get_name ());
    return;
  }

  int gnuplot_drawer::draw (const manager & geo_mgr_,
                            const std::string & what_,
                            int max_display_level_)
  {
    std::string what = what_;
    if (what.empty())
      {
        what = geo_mgr_.get_world_name () ;
      }
    DT_THROW_IF (what.empty(), std::logic_error,
                 "Missing object label to be displayed !");
    std::istringstream visu_gid_iss(what);
    geomtools::geom_id visu_gid;
    visu_gid_iss >> visu_gid;
    if (visu_gid_iss)
      {
        if (! geo_mgr_.get_mapping().validate_id(visu_gid))
          {
            ostringstream message;
            message << "geomtools::gnuplot_drawer::draw: "
                    << "Cannot find geometry volume with GID='" << visu_gid << "' in mapping dictionnary !";
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                         "Cannot find geometry volume with GID='" << visu_gid << "' in mapping dictionnary !");
            return EXIT_FAILURE;
          }
        draw_from_gid (geo_mgr_.get_factory(),
                       visu_gid,
                       geo_mgr_.get_mapping(),
                       max_display_level_);
      }
    else
      {
        //DT_LOG_DEBUG(datatools::logger::PRIO_NOTICE,"Label '" << what << "' is not a GID !");
        geomtools::placement visu_placement;
        const std::string & visu_model_name = what;
        models_col_type::const_iterator found = geo_mgr_.get_factory ().get_models ().find (visu_model_name);
        if (found == geo_mgr_.get_factory ().get_models ().end ())
          {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                         "Cannot find geometry model with name='" << visu_model_name
                         << "' in the geometry model factory !");
            return EXIT_FAILURE;
          }
        draw (geo_mgr_.get_factory (),
              visu_model_name,
              visu_placement,
              max_display_level_);
      }
    return EXIT_SUCCESS;
  }

  void gnuplot_drawer::draw_from_gid (const model_factory & mf_,
                                      const geom_id & gid_,
                                      const mapping & mapping_,
                                      int max_display_level_)
  {
    DT_THROW_IF (! mapping_.validate_id(gid_),
                 logic_error,
                 "Cannot find object with GID='" << gid_ << "' in the mapping dictionnary !");
    const geom_info & ginfo = mapping_.get_geom_info (gid_);
    const placement & wpl = ginfo.get_world_placement ();
    const logical_volume & log = ginfo.get_logical ();

    bool draw_dd = true;
    if (draw_dd) {
      _draw_display_data(mf_, wpl);
    }

    draw (log, wpl, max_display_level_, log.get_name ());
    return;
  }

  void gnuplot_drawer::draw (const model_factory & mf_,
                             const std::string & name_,
                             const placement & p_,
                             int max_display_level_)
  {
    bool devel = g_devel;
    //devel = true;
    models_col_type::const_iterator found = mf_.get_models ().find (name_);
    DT_THROW_IF (found ==  mf_.get_models ().end (),
                 logic_error,
                 "Cannot find geometry model with name '" << name_ << "' !");
    const i_model & model = *(found->second);
    const geomtools::logical_volume & log = model.get_logical ();

    bool draw_dd = true;
    std::string world_name;
    if (get_properties().has_key(gnuplot_drawer::WORLD_NAME_KEY)
        && get_properties().is_string(gnuplot_drawer::WORLD_NAME_KEY))
      {
        world_name = get_properties().fetch_string(gnuplot_drawer::WORLD_NAME_KEY);
      }
    if (world_name.empty())
      {
        world_name = geomtools::model_factory::DEFAULT_WORLD_LABEL;
      }
    if (draw_dd && (name_ == world_name))
      {
        _draw_display_data(mf_, p_);
      }

    draw (log, p_, max_display_level_, log.get_name ());

    return;
  }

  void gnuplot_drawer::_draw_display_data (const model_factory & mf_,
                                          const placement & p_)
  {
    /*
    std::cerr << "DEVEL: " << "geomtools::gnuplot_drawer::_draw_display_data: "
              << "Entering..."
              << std::endl;
    */
    bool draw_static = true;
    int draw_frame_index = -1;
    std::string draw_group;
    std::string draw_name;
    for (int idd = 0; idd < _display_data_.size(); idd++)
      {
        const display_data & dd = _display_data_[idd].get_display_data();
        const placement & dd_pl = _display_data_[idd].get_placement();

        //dd.tree_dump(std::cerr, "Display data: ", "DEVEL: ");
        for (int icolor = 0; icolor < dd.get_colors().size(); icolor++)
          {
            const std::string & draw_color = dd.get_colors()[icolor];
            /*
            std::cerr << "DEVEL: " << "geomtools::gnuplot_drawer::_draw_display_data: "
                      << "Color = " << draw_color
                      << std::endl;
            */
            std::ostringstream & colored_out = _get_stream(draw_color);
            geomtools::gnuplot_draw::draw_display_data(colored_out,
                                                       dd_pl.get_translation(),
                                                       dd_pl.get_rotation(),
                                                       dd,
                                                       draw_static,
                                                       draw_frame_index,
                                                       draw_color,
                                                       draw_group,
                                                       draw_name);
          }
      }
    return;
  }

} // end of namespace geomtools

// end of gnuplot_drawer.cc
