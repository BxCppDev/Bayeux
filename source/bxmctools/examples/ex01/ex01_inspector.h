// ex01_inspector.h

#ifndef MCTOOLS_EX01_INSPECTOR_H_
#define MCTOOLS_EX01_INSPECTOR_H_ 1

#include <geomtools/manager.h>
#include <mctools/simulated_data.h>

namespace mctools {
  namespace ex01 {

    class simulated_data_inspector {

    public:

      simulated_data_inspector();

      void set_interactive(bool);

      void set_dump_simulated_data(bool);

      void set_dump_hits(bool);

      void set_with_visualization(bool);

      void set_geometry_manager(const geomtools::manager &);

      bool inspect(const mctools::simulated_data & sd_);

      void display(const mctools::simulated_data & sd_);

    private:

      bool _interactive_;
      bool _dump_simulated_data_;
      bool _dump_hits_;
      bool _with_visualization_;
      const geomtools::manager * _geometry_manager_;

    };

  } // namespace ex01
} // namespace mctools

#endif // MCTOOLS_EX01_INSPECTOR_H_
