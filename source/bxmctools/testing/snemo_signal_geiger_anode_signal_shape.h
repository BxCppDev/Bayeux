#ifndef SNEMO_SIGNAL_GEIGER_ANODE_SIGNAL_SHAPE_H
#define SNEMO_SIGNAL_GEIGER_ANODE_SIGNAL_SHAPE_H

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/i_cloneable.h>
#include <datatools/exception.h>
// - Bayeux/mygsl:
#include <mygsl/i_unary_function.h>

// This project:
#include <mctools/signal/triangle_gate_signal_shape.h>

namespace snemo {

  namespace signal {

    //! \brief Geiger anode signal
    class geiger_anode_signal_shape
      : public mygsl::i_unary_function,
        public datatools::i_cloneable
    {
    public:

      //! Default constructor
      geiger_anode_signal_shape()
      {
        _set_defaults();
        return;
      }

      //! Destructor
      virtual ~geiger_anode_signal_shape()
      {
        return;
      }

      void set_amplitude(double amplitude_)
      {
        DT_THROW_IF(is_initialized(), std::logic_error, "Signal shape is already initialized!");
        DT_THROW_IF(datatools::is_valid(amplitude_)
                    && amplitude_ < 0.0,
                    std::logic_error,
                    "Invalid negative amplitude!");
        _amplitude_ = amplitude_;
        return;
      }

      double get_amplitude() const
      {
        return _amplitude_;
      }

      void set_t0(double t0_)
      {
        DT_THROW_IF(is_initialized(), std::logic_error, "Signal shape is already initialized!");
        _t0_ = t0_;
        return;
      }

      double get_t0() const
      {
        return _t0_;
      }

      void set_t_rise(double t_rise_)
      {
        DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
        DT_THROW_IF(datatools::is_valid(t_rise_) && t_rise_ < 0.0, std::logic_error, "Signal is already initialized!")
          _t_rise_ = t_rise_;
        return;
      }

      double get_t_rise() const
      {
        return _t_rise_;
      }

      void set_t1(double t1_)
      {
        DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
        _t1_ = t1_;
        return;
      }

      double get_t1() const
      {
        return _t1_;
      }

      void set_t2(double t2_)
      {
        DT_THROW_IF(is_initialized(), std::logic_error, "Signal is already initialized!");
        _t2_ = t2_;
        return;
      }

      double get_t2() const
      {
        return _t2_;
      }

      //! Initialization
      void initialize(const datatools::properties & config_,
                      const mygsl::unary_function_dict_type & functors_) override
      {
        this->mygsl::i_unary_function::_base_initialize(config_, functors_);

        static const double the_default_time_unit = mctools::signal::default_time_unit().get_value();
        // static const double the_default_charge_unit = mctools::signal::default_charge_unit().get_value();
        static const double the_default_voltage_unit = mctools::signal::default_voltage_unit().get_value();

        // Parse configuration:
        // config_.tree_dump(std::cerr, "snemo::signal::geiger_anode_signal_shape: ", "DEVEL: ");

        if (!datatools::is_valid(_t0_)) {
          if (config_.has_key("t0")) {
            double t0 = config_.fetch_real("t0");
            if (!config_.has_explicit_unit("t0")) {
              t0 *= the_default_time_unit;
            }
            // std::cerr << "DEVEL: snemo::signal::geiger_anode_signal_shape: t0 = " << t0 << std::endl;
            set_t0(t0);
          }
        }

        if (!datatools::is_valid(_t_rise_)) {
          if (config_.has_key("t_rise")) {
            double t_rise = config_.fetch_real("t_rise");
            if (!config_.has_explicit_unit("t_rise")) {
              t_rise *= the_default_time_unit;
            }
            set_t_rise(t_rise);
          }
        }

        if (!datatools::is_valid(_t1_)) {
          if (config_.has_key("t1")) {
            double t1 = config_.fetch_real("t1");
            // std::cerr << "DEVEL: snemo::signal::geiger_anode_signal_shape: t1 = " << t1 << std::endl;
            if (!config_.has_explicit_unit("t1")) {
              t1 *= the_default_time_unit;
            }
            // std::cerr << "DEVEL: snemo::signal::geiger_anode_signal_shape: t1 = " << t1 << std::endl;
            set_t1(t1);
          }
        }

        if (!datatools::is_valid(_t2_)) {
          if (config_.has_key("t2")) {
            double t2 = config_.fetch_real("t2");
            if (!config_.has_explicit_unit("t2")) {
              t2 *= the_default_time_unit;
            }
            // std::cerr << "DEVEL: snemo::signal::geiger_anode_signal_shape: t2 = " << t2 << std::endl;
            set_t2(t2);
          }
        }

        if (!datatools::is_valid(_amplitude_)) {
          if (config_.has_key("amplitude")) {
            double amplitude = config_.fetch_real("amplitude");
            if (!config_.has_explicit_unit("amplitude")) {
              amplitude *= the_default_voltage_unit;
            }
            set_amplitude(amplitude);
          }
        }

        DT_THROW_IF(!datatools::is_valid(_amplitude_), std::logic_error, "Amplitude is not set!");
        DT_THROW_IF(!datatools::is_valid(_t0_), std::logic_error, "Anode time is not set!");
        DT_THROW_IF(!datatools::is_valid(_t1_), std::logic_error, "Bottom cathode time is not set!");
        DT_THROW_IF(_t1_ <= _t0_ + get_epsilon(), std::logic_error, "Invalid bottom cathode time!");
        DT_THROW_IF(!datatools::is_valid(_t2_), std::logic_error, "Top cathode time is not set!");
        DT_THROW_IF(_t2_ <= _t0_ + get_epsilon(), std::logic_error, "Invalid top cathode time!");

        _compute_parameters();

        _bottom_cathode_shape_.set_polarity(mctools::signal::POLARITY_NEGATIVE);
        _top_cathode_shape_.set_polarity(mctools::signal::POLARITY_NEGATIVE);
        _bottom_cathode_shape_.set_amplitude(0.5 * _amplitude_);
        _top_cathode_shape_.set_amplitude(0.5 * _amplitude_);
        _bottom_cathode_shape_.set_t0(_t0_);
        _top_cathode_shape_.set_t0(_t0_);
        _bottom_cathode_shape_.set_t1(_t0_ + _t_rise_);
        _top_cathode_shape_.set_t1(_t0_ + _t_rise_);
        _bottom_cathode_shape_.set_t2(_t1_);
        _top_cathode_shape_.set_t2(_t2_);
        _bottom_cathode_shape_.set_t3(_t1_ + _t_rise_);
        _top_cathode_shape_.set_t3(_t2_ + _t_rise_);
        _bottom_cathode_shape_.initialize_simple();
        _top_cathode_shape_.initialize_simple();

        return;
      }

      //! Reset
      void reset() override
      {
        _bottom_cathode_shape_.reset();
        _top_cathode_shape_.reset();
        _set_defaults();
        this->i_unary_function::reset();
        return;
      }

      //! Check initialization status
      bool is_initialized() const override
      {
        if (!this->i_unary_function::is_initialized()) return false;
        if (!datatools::is_valid(_t0_)) return false;
        if (!datatools::is_valid(_t_rise_)) return false;
        if (!datatools::is_valid(_t1_)) return false;
        if (!datatools::is_valid(_t2_)) return false;
        if (!datatools::is_valid(_amplitude_)) return false;
        return true;
      }

    protected:

      //! Set default attributes
      void _set_defaults()
      {
        datatools::invalidate(_t0_);
        datatools::invalidate(_t_rise_);
        datatools::invalidate(_t1_);
        datatools::invalidate(_t2_);
        datatools::invalidate(_amplitude_);
        return;
      }

      //! Automatically compute working data parameters
      void _compute_parameters()
      {
        // nothing
        return;
      }

      //! Evaluation from parameters
      double _eval(double time_) const override
      {
        double sig = _bottom_cathode_shape_(time_) + _top_cathode_shape_(time_);
        return sig;
      }

    private:

      // Configuration:
      double _t0_;        ///< Anode time
      double _t_rise_;    ///< Front edge anode rising time
      double _t1_;        ///< Bottom cathode time
      double _t2_;        ///< Top cathode time
      double _amplitude_; ///< Amplitude

      // Working:
      mctools::signal::triangle_gate_signal_shape _bottom_cathode_shape_;
      mctools::signal::triangle_gate_signal_shape _top_cathode_shape_;

      //! Registration of the functor class
      MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(geiger_anode_signal_shape)

      //! Cloneable interface
      DATATOOLS_CLONEABLE_DECLARATION(geiger_anode_signal_shape)

    };

    MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(geiger_anode_signal_shape,
                                               "snemo::signal::geiger_anode_signal_shape")

    DATATOOLS_CLONEABLE_IMPLEMENTATION(geiger_anode_signal_shape)

  } // namespace signal

} // namespace snemo

#endif // SNEMO_SIGNAL_GEIGER_ANODE_SIGNAL_SHAPE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
