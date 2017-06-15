// datatools/dependency_graph.cc
//
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

// Standard library:
#include <sstream>

// Ourselves:
#include <datatools/dependency_graph.h>

// Third party:
// - Boost:
// #include <boost/property_map/property_map.hpp>
// #include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>

// This package:
#include <datatools/exception.h>

namespace datatools {

  dependency_graph::dependency_graph()
  {
    return;
  }

  dependency_graph::~dependency_graph()
  {
    return;
  }

  bool dependency_graph::has_vertex(const std::string & id_,
                                    const std::string & category_) const
  {
    for (std::pair<vertex_iter_t, vertex_iter_t> vertex_pair = boost::vertices(_g_);
         vertex_pair.first != vertex_pair.second;
         ++vertex_pair.first) {
      const vertex_t & v = *vertex_pair.first; //_g_[*vertex_pair.first];
      // std::cerr << "[devel] vertex = " << v << '\n';
      const vertex_properties_t & vp = _g_[v];
      if (vp.id != id_) {
        continue;
      } else {
        if (!category_.empty()) {
          if (vp.category != category_) {
            return false;
          } else {
            return true;
          }
        } else {
          return true;
        }
      }
    }
    return false;
  }

  void dependency_graph::add_vertex(const std::string & id_,
                                    const std::string & category_)
  {
    if (has_vertex(id_)) {
      DT_THROW(std::logic_error, "Dependency graph already has a vertex id '" << id_ << "'!");
    }
    vertex_t v;
    v = boost::add_vertex(_g_);
    vertex_properties_t & vp = _g_[v];
    vp.id = id_;
    vp.category = category_;
    return;
  }

  bool dependency_graph::has_out_edge(const std::string & from_id_,
                                      const std::string & to_id_,
                                      const std::string & topic_) const
  {
    vertex_t from;
    vertex_t to;
    try {
      from = _get_vertex_(from_id_);
      to = _get_vertex_(to_id_);
    } catch (std::exception &) {
      return false;
    }
    for (std::pair<edge_iter_t, edge_iter_t> edge_pair = boost::edges(_g_);
         edge_pair.first != edge_pair.second;
         ++edge_pair.first) {
      const edge_t & e = *edge_pair.first;
      vertex_t vs = boost::source(e, _g_);
      vertex_t vt = boost::target(e, _g_);
      if (from != vs) continue;
      if (to != vt) continue;
      const edge_properties_t & ep = _g_[e];
      if (!topic_.empty()) {
        if (ep.topic != topic_) {
          return false;
        } else {
          return true;
        }
      }
    }
    return false;
  }

  void dependency_graph::add_out_edge(const std::string & from_id_,
                                      const std::string & to_id_,
                                      const std::string & topic_)
  {
    if (from_id_ == to_id_) {
      DT_THROW(std::logic_error, "Dependency graph vertex '" << from_id_ << "' cannot link to itself!");
    }
    if (!has_vertex(from_id_)) {
      DT_THROW(std::logic_error, "Dependency graph does not have a from vertex id '" << from_id_ << "'!");
    }
    if (!has_vertex(to_id_)) {
      DT_THROW(std::logic_error, "Dependency graph does not have a to vertex id '" << to_id_ << "'!");
    }
    vertex_t from = _get_vertex_(from_id_);
    vertex_t to = _get_vertex_(to_id_);
    edge_t e;
    bool inserted = false;
    boost::tie(e, inserted) = boost::add_edge(from, to, _g_);
    DT_THROW_IF(!inserted, std::logic_error,
                "Dependency graph cannot add edge from vertex id '" << from_id_ << "' to vertex '" << to_id_ << "'!");
    edge_properties_t & ep = _g_[e];
    ep.topic = topic_;
    return;
  }

  void dependency_graph::reset()
  {
    _g_.clear();
    return;
  }

  bool dependency_graph::has_cycle() const
  {
    bool has_cycle = false;
    cycle_detector vis(has_cycle);
    // boost::depth_first_search(_g_, boost::visitor(vis));
    return has_cycle;
  }

  void dependency_graph::smart_print(std::ostream & out_) const
  {
    out_ << "Dependency graph:" << std::endl;
    return;
  }

  dependency_graph::vertex_t dependency_graph::_get_vertex_(const std::string & id_) const
  {
    for (std::pair<vertex_iter_t, vertex_iter_t> vertex_pair = boost::vertices(_g_);
         vertex_pair.first != vertex_pair.second;
         ++vertex_pair.first) {
      vertex_t v = *vertex_pair.first;
      const vertex_properties_t & vp = _g_[v];
      if (vp.id != id_) {
        continue;
      } else {
        return v;
      }
    }
    DT_THROW(std::logic_error, "No vertex with ID '" << id_ << "'!");
  }

  class vertex_property_writer
  {
  public:

    vertex_property_writer(const dependency_graph::graph_t & g_,
                           const bool with_vertex_category_)
      : _g_(g_)
      , _with_vertex_category_(with_vertex_category_)
    {
      return;
    }

    void operator()(std::ostream & out_, const dependency_graph::vertex_t & v_) const
    {
      const dependency_graph::vertex_properties_t & vp = _g_[v_];
      out_ << '[';
      out_ << " shape=" << '"' << "record" << '"';
      out_ << " label=" << '"'
           << vp.id;
      if (_with_vertex_category_) {
        out_ << "&#92;n" << '[' << vp.category << ']';
      }
      out_ << '"';
      out_ << ']';
      return;
    }

  private:

    const dependency_graph::graph_t & _g_;
    bool _with_vertex_category_ = false;

  };

  class edge_property_writer
  {
  public:

    edge_property_writer(const dependency_graph::graph_t & g_,
                         const bool with_edge_topic_)
      : _g_(g_)
      , _with_edge_topic_(with_edge_topic_)
    {
      return;
    }

    void operator()(std::ostream & out_, const dependency_graph::edge_t & e_) const
    {
      const dependency_graph::edge_properties_t & ep = _g_[e_];
      out_ << '[';
      if (_with_edge_topic_) {
        out_ << " label=" << '"' << ep.topic << '"';
      }
      out_ << " dir=" << '"' << "forward" << '"';
      out_ << " arrowType=" << '"' << "open" << '"';
      out_ << ']';
      return;
    }

  private:

    const dependency_graph::graph_t & _g_;
    bool _with_edge_topic_ = false;

  };

  class graph_property_writer
  {
  public:
    graph_property_writer(const dependency_graph::graph_t & g_) : _g_(g_) {}
    void operator()(std::ostream & out_) const
    {
      out_ << "graph [bgcolor=\"white\" color=\"black\"]" << std::endl;
      out_ << "node [shape=\"box\" color=\"blue\" fillcolor=\"lightgrey\" style=\"filled\"]" << std::endl;
      out_ << "edge [style=\"dashed\" color=\"black\"]" << std::endl;
    }
  private:
    const dependency_graph::graph_t & _g_;
  };

  void dependency_graph::export_graphviz(std::ostream & out_, const uint32_t flags_) const
  {
    bool with_edge_topic = false;
    bool with_vertex_category = false;
    if (flags_ & XGV_WITH_EDGE_TOPIC) {
      with_edge_topic = true;
    }
    if (flags_ & XGV_WITH_VERTEX_CATEGORY) {
      with_vertex_category = true;
    }
    vertex_property_writer vpw(_g_, with_vertex_category);
    edge_property_writer   epw(_g_, with_edge_topic);
    graph_property_writer  gpw(_g_);
    boost::write_graphviz(out_, _g_, vpw, epw, gpw);
    return;
  }

} // namespace datatools
