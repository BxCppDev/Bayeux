// point_of_interest.cc

// Ourselves:
#include <mctools/biasing/point_of_interest.h>

// Standard library:

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>

namespace mctools {

  namespace biasing {

    point_of_interest::point_of_interest()
    {
      _set_default();
      return;
    }

    void point_of_interest::set_name(const std::string & n_)
    {
      _name_ = n_;
      return;
    }

    const std::string & point_of_interest::get_name() const
    {
      return _name_;
    }

    void point_of_interest::set_position(const geomtools::vector_3d & p_)
    {
      _position_ = p_;
      return;
    }

    const geomtools::vector_3d & point_of_interest::get_position() const
    {
      return _position_;
    }

    void point_of_interest::set_attractivity(double a_)
    {
      _attractivity_ = a_;
      return;
    }

    double point_of_interest::get_attractivity() const
    {
      return _attractivity_;
    }

    void point_of_interest::set_radius(double r_)
    {
      DT_THROW_IF(r_ <= 0.0, std::domain_error,
                  "Invalid radius for point of interest '" << _name_ << "'");
      _radius_ = r_;
      return;
    }

    double point_of_interest::get_radius() const
    {
      return _radius_;
    }

    bool point_of_interest::is_valid() const
    {
      return geomtools::is_valid(_position_)
        && datatools::is_valid(_attractivity_)
        && datatools::is_valid(_radius_);
    }

    void point_of_interest::invalidate()
    {
      _set_default();
      return;
    }

    void point_of_interest::_set_default()
    {
      geomtools::invalidate(_position_);
      datatools::invalidate(_attractivity_);
      datatools::invalidate(_radius_);
      return;
    }

  } // namespace g4

} // namespace mctools
