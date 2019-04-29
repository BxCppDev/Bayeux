// regular_3d_mesh_placement.cc

// Ourselves:
#include <geomtools/regular_3d_mesh_placement.h>

// Standard library:
#include <fstream>

// Third party:
// - Boost:
//#include <boost/system/error_code.hpp>
#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/cstdint.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/i_serializable.h>
#include <datatools/archives_instantiation.h>
#include <datatools/io_factory.h>

// This project:
#include <geomtools/bounding_data.h>
#include <geomtools/box.h>
#include <geomtools/overlapping.h>
#include <geomtools/physical_volume.h>

namespace geomtools {

  /// \brief 3D discrete node coordinates
  struct node_coord_3d :  public datatools::i_serializable
  {
    node_coord_3d();
    node_coord_3d(int32_t i_, int32_t j_, int32_t k_);
    void set(int32_t i_, int32_t j_, int32_t k_);
    void invalidate();
    bool is_valid() const;
    bool operator<(const node_coord_3d & c_) const;
    bool operator==(const node_coord_3d & c_) const;
    friend std::ostream & operator<<(std::ostream &, const node_coord_3d & c_);

    int32_t i, j, k;

    DATATOOLS_SERIALIZATION_DECLARATION()
  };

  DATATOOLS_SERIALIZATION_IMPLEMENTATION(node_coord_3d, "geomtools::mesh_3d::node_coord_3d")

  std::ostream & operator<<(std::ostream & out_, const node_coord_3d & c_)
  {
    out_ << '[' << c_.i << ':' << c_.j << ':' << c_.k << ']';
    return out_;
  }

  template<class Archive>
  void node_coord_3d::serialize(Archive & ar, const unsigned int /* version_ */)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp("i", i);
    ar & boost::serialization::make_nvp("j", j);
    ar & boost::serialization::make_nvp("k", k);
    return;
  }

  node_coord_3d::node_coord_3d() {
    invalidate();
    return;
  }

  node_coord_3d::node_coord_3d(int32_t i_, int32_t j_, int32_t k_) {
    invalidate();
    set(i_, j_, k_);
    return;
  }

  void node_coord_3d::set(int32_t i_, int32_t j_, int32_t k_) {
    i = i_;
    j = j_;
    k = k_;
    return;
  }

  void node_coord_3d::invalidate() {
    i = j = k = -1;
    return;
  }

  bool node_coord_3d::is_valid() const {
    return i >= 0 && j >= 0 && k >= 0;
  }

  bool node_coord_3d::operator<(const node_coord_3d & c_) const {
    DT_THROW_IF(!c_.is_valid(), std::logic_error,
                "Invalid node 3D coordinates!");
    if (i < c_.i) {
      return true;
    } else if (i > c_.i) {
      return false;
    } else {
      if (j < c_.j) {
        return true;
      } else if (j > c_.j) {
        return false;
      } else {
        if (k < c_.k) {
          return true;
        } else if (k > c_.k) {
          return false;
        } else {
          return false;
        }
      }
    }
  }

  bool node_coord_3d::operator==(const node_coord_3d & c_) const {
    return i == c_.i && j == c_.j && k == c_.k;
  }

  /// \brief 3D mesh node:
  struct node_type : public datatools::i_serializable
  {
    int32_t       id;
    node_coord_3d coordinates;
    bool          active;
    int32_t get_id() const;
    const node_coord_3d & get_coordinates() const;
    DATATOOLS_SERIALIZATION_DECLARATION()
  };

  DATATOOLS_SERIALIZATION_IMPLEMENTATION(node_type,"geomtools::mesh_3d::node_type")

  template<class Archive>
  void node_type::serialize(Archive & ar_, const unsigned int /* version_ */)
  {
    ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar_ & boost::serialization::make_nvp("id", id);
    ar_ & boost::serialization::make_nvp("coordinates", coordinates);
    ar_ & boost::serialization::make_nvp("active", active);
    return;
  }

  int32_t node_type::get_id() const {
    return id;
  }

  const node_coord_3d & node_type::get_coordinates() const {
    return coordinates;
  }

  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(node_coord_3d)
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(node_type)

  // Tags for accessing the corresponding indices of resource_set :
  struct node_tag_id {};
  struct node_tag_coord {};

  //! Multi index container of resources
  typedef boost::multi_index_container<
    node_type,
    boost::multi_index::indexed_by<

      // The first key is the unique identifier (RID):
      boost::multi_index::ordered_unique<
        boost::multi_index::tag<node_tag_id>,
        boost::multi_index::const_mem_fun<node_type,
                                          int32_t,
                                          &node_type::get_id
                                          >
        >

      // The second key is the unique set of coordinates:
      , boost::multi_index::ordered_unique<
          boost::multi_index::tag<node_tag_coord>,
          boost::multi_index::const_mem_fun<node_type,
                                            const node_coord_3d &,
                                            &node_type::get_coordinates
                                            >
          >
      >
    > node_set;

  typedef node_set::index<node_tag_id>::type    node_set_by_id;
  typedef node_set::index<node_tag_coord>::type node_set_by_coord;

  struct regular_3d_mesh_placement::data {
    node_set nodes; ///< Set of 3D mesh nodes
  };

  void regular_3d_mesh_placement::set_logging(datatools::logger::priority p_)
  {
    _logging_ = p_;
    return;
  }

  datatools::logger::priority regular_3d_mesh_placement::get_logging() const
  {
    return _logging_;
  }

  regular_3d_mesh_placement::regular_3d_mesh_placement()
  {
    _initialized_ = false;
    _logging_ = datatools::logger::PRIO_FATAL;
    _log_vol_ = 0;
    _node_log_vol_ = 0;

    _set_defaults();
    return;
  }

  regular_3d_mesh_placement::~regular_3d_mesh_placement()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void regular_3d_mesh_placement::_set_defaults()
  {
    datatools::invalidate(_step_x_);
    datatools::invalidate(_step_y_);
    datatools::invalidate(_step_z_);
    datatools::invalidate(_start_x_);
    datatools::invalidate(_start_y_);
    datatools::invalidate(_start_z_);
    datatools::invalidate(_stop_x_);
    datatools::invalidate(_stop_y_);
    datatools::invalidate(_stop_z_);
    _nsamples_x_ = 0;
    _nsamples_y_ = 0;
    _nsamples_z_ = 0;
    _overlapping_flags_ = 0;
    return;
  }

  bool regular_3d_mesh_placement::is_valid() const
  {
    return _basic_placement_.is_valid()
      && has_log_vol();
  }

  void regular_3d_mesh_placement::invalidate()
  {
    reset();
    return;
  }

  bool regular_3d_mesh_placement::has_cache_file_path() const
  {
    return ! _cache_file_path_.empty();
  }

  const std::string & regular_3d_mesh_placement::get_cache_file_path() const
  {
    return _cache_file_path_;
  }

  void regular_3d_mesh_placement::set_cache_file_path(const std::string & path_)
  {
    _cache_file_path_ = path_;
    return;
  }

  bool regular_3d_mesh_placement::has_log_vol() const
  {
    return _log_vol_ != 0;
  }

  void regular_3d_mesh_placement::set_log_vol(const logical_volume & log_vol_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Regular 3D mesh placement is already initialized!");
    DT_THROW_IF(! log_vol_.has_shape(), std::logic_error,
                "Logical volume '" << log_vol_.get_name() << "' has no shape!");
    _log_vol_ = &log_vol_;
    return;
  }

  bool regular_3d_mesh_placement::has_node_log_vol() const
  {
    return _node_log_vol_ != 0;
  }

  void regular_3d_mesh_placement::set_node_log_vol(const logical_volume & node_log_vol_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Regular 3D mesh placement is already initialized!");
    DT_THROW_IF(! node_log_vol_.has_shape(), std::logic_error,
                "Node logical volume '" << node_log_vol_.get_name() << "' has no shape!");
    _node_log_vol_ = &node_log_vol_;
    return;
  }

  void regular_3d_mesh_placement::set_start_x(double x_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Regular 3D mesh placement is already initialized!");
    _start_x_ = x_;
    return;
  }

  void regular_3d_mesh_placement::set_start_y(double y_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Regular 3D mesh placement is already initialized!");
    _start_y_ = y_;
    return;
  }

  void regular_3d_mesh_placement::set_start_z(double z_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Regular 3D mesh placement is already initialized!");
    _start_z_ = z_;
    return;
  }

  void regular_3d_mesh_placement::set_stop_x(double x_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Regular 3D mesh placement is already initialized!");
    _stop_x_ = x_;
    return;
  }

  void regular_3d_mesh_placement::set_stop_y(double y_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Regular 3D mesh placement is already initialized!");
    _stop_y_ = y_;
    return;
  }

  void regular_3d_mesh_placement::set_stop_z(double z_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Regular 3D mesh placement is already initialized!");
    _stop_z_ = z_;
    return;
  }

  void regular_3d_mesh_placement::set_step_x(double step_x_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Regular 3D mesh placement is already initialized!");
    DT_THROW_IF(! datatools::is_valid(step_x_) || step_x_ <= 0.0 ,
                std::logic_error, "Invalid step X!");
    _step_x_ = step_x_;
    return;
  }

  void regular_3d_mesh_placement::set_step_y(double step_y_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Regular 3D mesh placement is already initialized!");
    DT_THROW_IF(! datatools::is_valid(step_y_) || step_y_ <= 0.0 ,
                std::logic_error, "Invalid step Y!");
    _step_y_ = step_y_;
    return;
  }

  void regular_3d_mesh_placement::set_step_z(double step_z_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Regular 3D mesh placement is already initialized!");
    DT_THROW_IF(! datatools::is_valid(step_z_) || step_z_ <= 0.0 ,
                std::logic_error, "Invalid step Z!");
    _step_z_ = step_z_;
    return;
  }

  void regular_3d_mesh_placement::set_steps(double step_x_, double step_y_, double step_z_)
  {
    set_step_x(step_x_);
    set_step_y(step_y_);
    set_step_z(step_z_);
    return;
  }

  void regular_3d_mesh_placement::set_starts(double x0_, double y0_, double z0_)
  {
    set_start_x(x0_);
    set_start_y(y0_);
    set_start_z(z0_);
    return;
  }

  void regular_3d_mesh_placement::set_stops(double x1_, double y1_, double z1_)
  {
    set_stop_x(x1_);
    set_stop_y(y1_);
    set_stop_z(z1_);
    return;
  }

  void regular_3d_mesh_placement::set_overlapping_flags(uint32_t of_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Regular 3D mesh placement is already initialized!");
    _overlapping_flags_ = of_;
    return;
  }

  uint32_t regular_3d_mesh_placement::get_overlapping_flags() const
  {
    return _overlapping_flags_;
  }

  bool regular_3d_mesh_placement::has_only_one_rotation() const
  {
    return true;
  }

  size_t regular_3d_mesh_placement::get_dimension() const
  {
    return 3;
  }

  bool regular_3d_mesh_placement::is_replica() const
  {
    return false;
  }

  size_t regular_3d_mesh_placement::get_number_of_items() const
  {
    return _data_->nodes.size();
  }

  // virtual
  void regular_3d_mesh_placement::get_placement(int item_, placement & p_) const
  {
    const node_set_by_id & id_index = _data_->nodes.get<node_tag_id>();
    node_set_by_id::const_iterator found = id_index.find(item_);
    if (found == id_index.end()) {
      p_.invalidate();
    }
    const node_type & n = *found;
    vector_3d dijk(n.get_coordinates().i * _step_x_,
                   n.get_coordinates().j * _step_y_,
                   n.get_coordinates().k * _step_z_);
    p_ = _basic_placement_;
    p_.grab_translation() += dijk;
    // std::cerr << "DEVEL: i,j,k=" << n.get_coordinates().i
    //           << ',' << n.get_coordinates().j
    //           << ',' << n.get_coordinates().k
    //           << " dijk=" << dijk
    //           << " t=" << p_.grab_translation()
    //           << std::endl;
    return;
  }

  // virtual
  size_t regular_3d_mesh_placement::compute_index_map(std::vector<uint32_t> & map_,
                                                      int item_) const
  {
    DT_THROW_IF((item_ < 0) || (item_ >= (int)get_number_of_items ()),
                std::domain_error,
                "Invalid item index '" << item_ << "' !");
    const node_set_by_id & id_index = _data_->nodes.get<node_tag_id>();
    node_set_by_id::const_iterator found = id_index.find(item_);
    // if (found == id_index.end()) {
    // }
    const node_type & n = *found;
    map_.clear();
    map_.push_back(n.get_coordinates().i);
    map_.push_back(n.get_coordinates().j);
    map_.push_back(n.get_coordinates().k);
    return map_.size ();
  }

  const placement & regular_3d_mesh_placement::get_basic_placement() const
  {
    return _basic_placement_;
  }

  void regular_3d_mesh_placement::initialize()
  {
    datatools::properties dummy;
    initialize(dummy);
    return;
  }

  void regular_3d_mesh_placement::initialize(const datatools::properties & config_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Regular 3D mesh placement is already initialized!");
    _init(config_);
    _initialized_ = true;
    return;
  }

  bool regular_3d_mesh_placement::is_initialized() const
  {
    return _initialized_;

  }

  void regular_3d_mesh_placement::_init(const datatools::properties & config_)
  {
    DT_LOG_TRACE_ENTERING(_logging_);
    _data_.reset(new data);

    double default_length_unit = CLHEP::mm;

    // Fetch configuration parameters:

    {
      datatools::logger::priority p =
        datatools::logger::extract_logging_configuration(config_,
                                                         _logging_,
                                                         true);
      set_logging(p);
    }

    // config_.tree_dump(std::cerr, "regular_3d_mesh_placement::_init: Config: ", "DEVEL: ");

    // Cache file :
    if (!has_cache_file_path()) {
      if (config_.has_key("cache_file_path")) {
        std::string cache_file_path = config_.fetch_string("cache_file_path");
        set_cache_file_path(cache_file_path);
      }
    }

    // Overlapping settings:
    if (_overlapping_flags_ == 0) {
      datatools::properties overlapping_config;
      config_.export_and_rename_starting_with(overlapping_config,
                                              overlapping::property_prefix(),
                                              "");
      uint32_t oflags = 0;
      if (overlapping_config.has_flag("check_old_first")) {
        oflags |= overlapping::FLAG_CHECK_OLD_FIRST;
      }
      if (overlapping_config.has_flag("check_inhibit_second")) {
        oflags |= overlapping::FLAG_CHECK_INHIBIT_SECOND;
      }
      if (overlapping_config.has_flag("overlap_all")) {
        oflags |= overlapping::FLAG_OVERLAP_ALL;
      }
      if (overlapping_config.has_flag("huge_sampling")) {
        oflags |= overlapping::FLAG_WIRES_HUGE_SAMPLING;
      } else if (overlapping_config.has_flag("very_high_sampling")) {
        oflags |= overlapping::FLAG_WIRES_VERY_HIGH_SAMPLING;
      } else if (overlapping_config.has_flag("high_sampling")) {
        oflags |= overlapping::FLAG_WIRES_HIGH_SAMPLING;
      } else if (overlapping_config.has_flag("low_sampling")) {
        oflags |= overlapping::FLAG_WIRES_LOW_SAMPLING;
      }

      if (oflags != 0) {
        // std::cerr << "DEVEL: " << "regular_3d_mesh_placement::_init: overlapping flags = [" << oflags << ']' << std::endl;
        set_overlapping_flags(oflags);
      }
    }

    // Step X, Y, Z:
    if (!datatools::is_valid(_step_x_)) {
      if (config_.has_key("step_x")) {
        double sx = config_.fetch_real("step_x");
        if (!config_.has_explicit_unit("step_x")) {
          sx *= default_length_unit;
        }
        set_step_x(sx);
      }
    }

    if (!datatools::is_valid(_step_y_)) {
      if (config_.has_key("step_y")) {
        double sy = config_.fetch_real("step_y");
        if (!config_.has_explicit_unit("step_y")) {
          sy *= default_length_unit;
        }
        set_step_y(sy);
      }
    }

    if (!datatools::is_valid(_step_z_)) {
      if (config_.has_key("step_z")) {
        // std::cerr << "DEVEL: " << "regular_3d_mesh_placement::_init: fetch step Z..." << std::endl;
        double sz = config_.fetch_real("step_z");
        if (!config_.has_explicit_unit("step_z")) {
          sz *= default_length_unit;
        }
        set_step_z(sz);
      }
    }

    // Start X, Y, Z:
    if (!datatools::is_valid(_start_x_)) {
      if (config_.has_key("start_x")) {
        double sx = config_.fetch_real("start_x");
        if (!config_.has_explicit_unit("start_x")) {
          sx *= default_length_unit;
        }
        set_start_x(sx);
      }
    }

    if (!datatools::is_valid(_start_y_)) {
      if (config_.has_key("start_y")) {
        double sy = config_.fetch_real("start_y");
        if (!config_.has_explicit_unit("start_y")) {
          sy *= default_length_unit;
        }
        set_start_y(sy);
      }
    }

    if (!datatools::is_valid(_start_z_)) {
      if (config_.has_key("start_z")) {
        double sz = config_.fetch_real("start_z");
        if (!config_.has_explicit_unit("start_z")) {
          sz *= default_length_unit;
        }
        set_start_z(sz);
      }
    }

    // Stop X, Y, Z:
    if (!datatools::is_valid(_stop_x_)) {
      if (config_.has_key("stop_x")) {
        double sx = config_.fetch_real("stop_x");
        if (!config_.has_explicit_unit("stop_x")) {
          sx *= default_length_unit;
        }
        set_stop_x(sx);
      }
    }

    if (!datatools::is_valid(_stop_y_)) {
      if (config_.has_key("stop_y")) {
        double sy = config_.fetch_real("stop_y");
        if (!config_.has_explicit_unit("stop_y")) {
          sy *= default_length_unit;
        }
        set_stop_y(sy);
      }
    }

    if (!datatools::is_valid(_stop_z_)) {
      if (config_.has_key("stop_z")) {
        double sz = config_.fetch_real("stop_z");
        if (!config_.has_explicit_unit("stop_z")) {
          sz *= default_length_unit;
        }
        set_stop_z(sz);
      }
    }

    if (has_node_log_vol()) {
      // Automatically compute the missing steps from the node shape:
      const i_shape_3d & node_shape = _node_log_vol_->get_shape();
      DT_THROW_IF(! node_shape.has_bounding_data(),
                  std::logic_error,
                  "Node shape has no bounding data!");
      const bounding_data & node_bd = node_shape.get_bounding_data();
      box node_bb;
      placement node_bb_placement;
      node_bd.compute_bounding_box(node_bb, node_bb_placement);
      if (!datatools::is_valid(_step_x_)) {
        double dx = node_bb.get_x();
        set_step_x(dx);
      }
      if (!datatools::is_valid(_step_y_)) {
        double dy = node_bb.get_y();
        set_step_y(dy);
      }
      if (!datatools::is_valid(_step_z_)) {
        double dz = node_bb.get_z();
        set_step_z(dz);
      }
    }

    // Check the validity of steps:
    DT_THROW_IF(!datatools::is_valid(_step_x_),
                std::logic_error,
                "Missing step X!");
    DT_THROW_IF(!datatools::is_valid(_step_y_),
                std::logic_error,
                "Missing step Y!");
    DT_THROW_IF(!datatools::is_valid(_step_z_),
                std::logic_error,
                "Missing step Z!");

    const i_shape_3d & mother_shape = _log_vol_->get_shape();
    DT_THROW_IF(! mother_shape.has_bounding_data(),
                std::logic_error,
                "Mother shape has no bounding data!");
    const bounding_data & bd = mother_shape.get_bounding_data();
    box bb;
    placement bb_placement;
    bd.compute_bounding_box(bb, bb_placement);

    double xmin = bb_placement.get_translation().x() - 0.5 * bb.get_x() + 0.5 * _step_x_;
    double ymin = bb_placement.get_translation().y() - 0.5 * bb.get_y() + 0.5 * _step_y_;
    double zmin = bb_placement.get_translation().z() - 0.5 * bb.get_z() + 0.5 * _step_z_;
    if (datatools::is_valid(_start_x_) && _start_x_ >= xmin) {
      xmin = _start_x_;
    }
    if (datatools::is_valid(_start_y_) && _start_y_ >= ymin) {
      ymin = _start_y_;
    }
    if (datatools::is_valid(_start_z_) && _start_z_ >= zmin) {
      zmin = _start_z_;
    }
    _basic_placement_.set_translation(xmin, ymin, zmin);
    _basic_placement_.set_orientation(0, 0, 0);
    double xmax = bb_placement.get_translation().x() + 0.5 * bb.get_x();
    double ymax = bb_placement.get_translation().y() + 0.5 * bb.get_y();
    double zmax = bb_placement.get_translation().z() + 0.5 * bb.get_z();
    if (datatools::is_valid(_stop_x_) && _stop_x_ <= xmax) {
      xmax = _stop_x_;
    }
    if (datatools::is_valid(_stop_y_) && _stop_y_ <= ymax) {
      ymax = _stop_y_;
    }
    if (datatools::is_valid(_stop_z_) && _stop_z_ <= zmax) {
      zmax = _stop_z_;
    }
    _nsamples_x_ = (int) ((xmax - xmin) / _step_x_);
    _nsamples_y_ = (int) ((ymax - ymin) / _step_y_);
    _nsamples_z_ = (int) ((zmax - zmin) / _step_z_);
    int id = 0;
    int i(0);
    overlapping OL;
    // OL.set_logging(datatools::logger::PRIO_DEBUG);

    bool has_cache_file = false;
    std::string cache_file_path;
    if (has_cache_file_path()) {
      cache_file_path = _cache_file_path_;
      datatools::fetch_path_with_env(cache_file_path);
      boost::filesystem::path cache_path = cache_file_path;
      if (boost::filesystem::exists(cache_path)) {
        DT_THROW_IF(!boost::filesystem::is_regular_file(cache_path),
                    std::logic_error,
                    "Cache file path '" << cache_file_path << "' is not a regular file!");
        has_cache_file = true;
      } else {
        boost::filesystem::path cache_dir_path = cache_path.parent_path();
        if (! boost::filesystem::exists(cache_dir_path)) {
          DT_THROW_IF(!boost::filesystem::create_directories(cache_dir_path),
                      std::runtime_error,
                      "Cache file directory '" << cache_dir_path << "' was not created!");
        }
      }
    }

    if (has_cache_file) {
      _load_cache(cache_file_path);
    } else {
      for (double x = xmin; x <= xmax + 0.1 * _step_x_; x += _step_x_) {
        int j(0);
        for (double y = ymin; y <= ymax + 0.1 * _step_y_; y += _step_y_) {
          int k(0);
          for (double z = zmin; z <= zmax + 0.1 * _step_z_; z += _step_z_) {
            vector_3d pos(x, y, z);
            DT_LOG_DEBUG(_logging_, "Checking position [" << i << ":" << j << ":" << k << "] = " << pos);
            bool reject_pos = true;
            logical_volume::locate_result lr;
            uint32_t locresult = _log_vol_->locate(pos, false, 0.0, 0.0, &lr);
            // std::cerr << "DEVEL: locresult=" << locresult << std::endl;
            if (locresult & SHAPE_DOMAIN_INSIDE
                && ! (locresult & SHAPE_DOMAIN_INSIDE_DAUGHTER)
                && ! (locresult & SHAPE_DOMAIN_ON_DAUGHTER_SURFACE) ) {
              reject_pos = false;
              DT_LOG_DEBUG(_logging_, "Position is NOT rejected!");
            } else {
              DT_LOG_DEBUG(_logging_, "Position IS rejected!");
            }

            if (! reject_pos) {
              bool map_it = true;
              if (!has_node_log_vol()) {
                // if (reject_pos == true) {
                //   map_it = false;
                // }
              } else {
                DT_LOG_DEBUG(_logging_, "Checking overlap with mother volume...");
                // First check if the candidate daughter solid overlap the mother envelope:
                // std::cerr << "DEVEL: " << "regular_3d_mesh_placement::_init: "
                //           << "Checking node volume..."
                //           << std::endl;
                // Detect overlap between the daughter solid and its mother volume:
                overlapping::report overlap_report;
                uint32_t overlapping_flags = _overlapping_flags_;

                placement node_placement(pos, 0.0, 0.0, 0.0);
                DT_LOG_DEBUG(_logging_, "Node placement " << pos);
                bool overlap = OL.check_mother_daughter_overlap(_log_vol_->get_shape(),
                                                                _node_log_vol_->get_shape(),
                                                                node_placement,
                                                                overlap_report,
                                                                overlapping_flags);
                DT_LOG_DEBUG(_logging_, "Found " << overlap_report.overlaps.size() << " overlapping vertexes!");
                if (overlap) {
                  DT_LOG_DEBUG(_logging_, "Overlap with mother WAS detected!");
                  map_it = false;
                } else {
                  DT_LOG_DEBUG(_logging_, "NO overlap with mother was detected!");
                }
                if (map_it) {
                  // Next check if the candidate daughter solid overlap another daughter solid in the mother:
                  DT_LOG_DEBUG(_logging_, "Checking overlap with daugther volumes...");
                  const logical_volume::physicals_col_type & phys_daughters = _log_vol_->get_physicals();
                  for (logical_volume::physicals_col_type::const_iterator idaughter = phys_daughters.begin();
                       idaughter != phys_daughters.end();
                       idaughter++) {
                    DT_LOG_DEBUG(_logging_, "Checking overlap with daugther '" << idaughter->first << "'...");
                    const physical_volume * phys_daughter = idaughter->second;
                    const logical_volume & log_daughter = phys_daughter->get_logical();
                    const i_placement & plcmt_daughter = phys_daughter->get_placement();
                    overlapping::report local_overlap_report;
                    uint32_t local_overlapping_flags = _overlapping_flags_;
                    bool local_overlap = OL.check_two_volumes_overlap(log_daughter.get_shape(),
                                                                plcmt_daughter,
                                                                _node_log_vol_->get_shape(),
                                                                node_placement,
                                                                local_overlap_report,
                                                                local_overlapping_flags);
                    if (local_overlap) {
                      map_it = false;
                      DT_LOG_DEBUG(_logging_, "Overlap with daugther '" << idaughter->first  << "' was detected.");
                      break;
                    }
                  } // For idaughter
                }
              }
              if (map_it) {
                node_type n;
                n.id = id++;
                n.coordinates.set(i, j, k);
                n.active = true;
                DT_LOG_DEBUG(_logging_, "Inserting node " << n.coordinates << " ...");
               _data_->nodes.insert(n);
              }
            } else {
              // reject this object
            }
            k++;
          }
          j++;
        }
        i++;
      }
      if (has_cache_file_path()) {
        _store_cache(cache_file_path);
      }
    }

    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void regular_3d_mesh_placement::_store_cache(const std::string & cache_file_path_)
  {
    DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS,
                "Storing 3D-mesh nodes cache file '" << cache_file_path_ << "'...");
    datatools::data_writer cache_writer(cache_file_path_.c_str(),
                                        datatools::using_multiple_archives);
    datatools::properties cache_header;
    cache_header.set_description("geomtools::regular_3d_mesh_placement cache file header");
    cache_header.store("cache.class", "geomtools::regular_3d_mesh_placement");
    cache_header.store("cache.what", "mesh_nodes");
    const node_set_by_id & id_index = _data_->nodes.get<node_tag_id>();
    int number_of_mesh_nodes = id_index.size();
    cache_header.store("cache.mesh_nodes.number", number_of_mesh_nodes);
    cache_writer.store(cache_header);
    for (node_set_by_id::const_iterator i = id_index.begin();
         i != id_index.end();
         i++) {
      const node_type & n = *i;
      cache_writer.store(n);
    }
    return;
  }

  void regular_3d_mesh_placement::_load_cache(const std::string & cache_file_path_)
  {
    _data_->nodes.clear();
    DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS,
                "Loading 3D-mesh nodes cache file '" << cache_file_path_ << "'...");
    datatools::data_reader cache_reader(cache_file_path_.c_str(),
                                        datatools::using_multiple_archives);
    DT_THROW_IF(!cache_reader.has_record_tag(),
                std::logic_error,
                "Cannot find record in the cache file '" << cache_file_path_ << "'!");
    DT_THROW_IF(!cache_reader.record_tag_is(datatools::properties::SERIAL_TAG),
                std::logic_error,
                "Cannot find cache header in the cache file '" << cache_file_path_ << "'!");
    datatools::properties cache_header;
    cache_reader.load(cache_header);
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Loading the header...");
    cache_header.print_tree(std::cerr);
    DT_THROW_IF(!cache_header.has_key("cache.class"),
                std::logic_error,
                "Cannot find class ID in cache header!");
    DT_THROW_IF(cache_header.fetch_string("cache.class") != "geomtools::regular_3d_mesh_placement",
                std::logic_error,
                "Class ID '" << cache_header.fetch_string("cache.class") << "' "
                << "does not match expected class '" << "geomtools::regular_3d_mesh_placement" << "'!");
    DT_THROW_IF(!cache_header.has_key("cache.what"),
                std::logic_error,
                "Cannot find description in cache header!");
    DT_THROW_IF(cache_header.fetch_string("cache.what") != "mesh_nodes",
                std::logic_error,
                "Description '" << cache_header.fetch_string("cache.what") << "' "
                << "does not match expected '" << "mesh_nodes" << "'!");
    int number_of_mesh_nodes = -1;
    DT_THROW_IF(!cache_header.has_key("cache.mesh_nodes.number"),
                std::logic_error,
                "Cannot find number of mesh nodes in cache header!");
    number_of_mesh_nodes = cache_header.fetch_integer("cache.mesh_nodes.number");
    
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Number_of_mesh_nodes=" << number_of_mesh_nodes);
    for (int inode = 0; inode < number_of_mesh_nodes; inode++) {
      // DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "inode=" << inode)
      DT_THROW_IF(!cache_reader.has_record_tag(),
                  std::logic_error,
                  "Cannot find node record in the cache file '" << cache_file_path_ << "'!");
      DT_THROW_IF(!cache_reader.record_tag_is(node_type::SERIAL_TAG),
                  std::logic_error,
                  "Record is not a 3D mesh node!");
      node_type node;
      cache_reader.load(node);
      _data_->nodes.insert(node);
    }
    return;
  }

  void regular_3d_mesh_placement::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Regular 3D mesh placement is not initialized!");
    _initialized_ = false;
    _fini();
    datatools::invalidate(_start_x_);
    datatools::invalidate(_start_y_);
    datatools::invalidate(_start_z_);
    datatools::invalidate(_step_x_);
    datatools::invalidate(_step_y_);
    datatools::invalidate(_step_z_);
    _node_log_vol_ = 0;
    _log_vol_ = 0;
    return;
  }

  void regular_3d_mesh_placement::_fini()
  {
    _data_.reset();
    _basic_placement_.invalidate();
    return;
  }

  void regular_3d_mesh_placement::tree_dump(std::ostream & out_,
                                            const std::string & title_,
                                            const std::string & indent_,
                                            bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    this->i_placement::tree_dump(out_, title_, indent, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Logging : " << datatools::logger::get_priority_label(_logging_)
         << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Logical volume : ";
    if (has_log_vol()) {
      out_ << "'" << _log_vol_->get_name() << "'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Node logical volume : ";
    if (has_node_log_vol()) {
      out_ << "'" << _node_log_vol_->get_name() << "'";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    if (has_node_log_vol()) {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Overlapping flags : "
           << _overlapping_flags_
           << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Step X : "
         << _step_x_ / CLHEP::mm << " mm"
         << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Step Y : "
         << _step_y_ / CLHEP::mm << " mm"
         << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Step Z : "
         << _step_z_ / CLHEP::mm << " mm"
         << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Basic placement : " ;
    {
      std::string bps;
      placement::to_string(bps, _basic_placement_);
      out_ << bps;
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Nodes : " ;
    out_ << '[' << _data_->nodes.size() << ']';
    out_ << std::endl;


    return;
  }

} // end of namespace geomtools
