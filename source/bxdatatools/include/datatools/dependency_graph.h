/// \file datatools/dependency_graph.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2017-06-09
 * Last modified: 2017-06-10
 *
 * License:
 *
 * Copyright (C) 2017 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * Description:
 *
 *   Dependency graph model.
 *   http://programmingexamples.net/wiki/Boost/BGL
 *   https://theboostcpplibraries.com/boost.graph-vertices-and-edges
 *   https://stackoverflow.com/questions/2244580/find-boost-bgl-vertex-by-a-key
 *   https://cpp.developpez.com/redaction/data/pages/users/gbdivers/boost-graph/
 *   http://matthieu-brucher.developpez.com/tutoriels/cpp/boost/graph/implementation/
 *   http://matthieu-brucher.developpez.com/tutoriels/cpp/boost/Property-Map/
 *
 */

#ifndef DATATOOLS_DEPENDENCY_GRAPH_H
#define DATATOOLS_DEPENDENCY_GRAPH_H

// Standard Library:
#include <string>
#include <iostream>

// Third party:
// - Boost:
#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>

// This project:
#include <datatools/bit_mask.h>

namespace datatools {

  /// \brief Dependency graph
  class dependency_graph
  {
  public:

    struct vertex_properties_t
    {
      std::string id;
      std::string category;
      vertex_properties_t()
        : id("")
        , category("") {}
      vertex_properties_t(const std::string & id_, const std::string & category_)
        : id(id_)
        , category(category_) {}
    };

    struct edge_properties_t
    {
      std::string topic;
      edge_properties_t()
        : topic("")  {}
      edge_properties_t(const std::string & topic_)
        : topic(topic_) {}
    };

    struct graph_properties_t
    {
      std::string topic;
      graph_properties_t()
        : topic("") {}
      graph_properties_t(const std::string & topic_)
        : topic(topic_) {}
    };

    typedef typename boost::adjacency_list<
      boost::vecS,
      boost::vecS,
      boost::bidirectionalS,
      vertex_properties_t,
      edge_properties_t,
      graph_properties_t,
      boost::listS
      > graph_t;
    typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_t;
    typedef boost::graph_traits<graph_t>::edge_descriptor   edge_t;
    typedef boost::graph_traits<graph_t>::vertex_iterator   vertex_iter_t;
    typedef boost::graph_traits<graph_t>::edge_iterator     edge_iter_t;
    typedef boost::graph_traits<graph_t>::out_edge_iterator out_edge_iter_t;
    typedef boost::graph_traits<graph_t>::in_edge_iterator  in_edge_iter_t;

    /// Default constructor
    dependency_graph();

    /// Destructor
    ~dependency_graph();

    /// Check it a vertex with given id and category
    bool has_vertex(const std::string & id_,
                    const std::string & category_ = "") const;

    /// Add a vertex with given id and category
    void add_vertex(const std::string & id_,
                    const std::string & category_);

    /// Add an out edge with given topic between to vertices
    void add_out_edge(const std::string & id_from_,
                      const std::string & id_to_,
                      const std::string & topic_);

    /// Check it an edge between two vertices exists with given topic
    bool has_out_edge(const std::string & id1_,
                      const std::string & id2_,
                      const std::string & topic_ = "") const;

    /// Reset
    void reset();

    /// \bried Visitor for cycle detection
    struct cycle_detector
    // : public boost::dfs_visitor<>
    {
      cycle_detector(bool & has_cycle)
        : _has_cycle(has_cycle)
      {
      }

      template <class Edge, class Graph>
      void back_edge(Edge, Graph &)
      {
        _has_cycle = true;
      }
    protected:
      bool & _has_cycle;
    };

    /// Check if some dependency cycle is detected
    bool has_cycle() const;

    /// Smart print
    void smart_print(std::ostream & out_) const;

    /// \brief Export flags for Graphviz
    enum xgv_flags {
      XGV_NONE                 = 0,               ///< No flags
      XGV_WITH_EDGE_TOPIC      = bit_mask::bit00, ///< Print edge topic
      XGV_WITH_VERTEX_CATEGORY = bit_mask::bit01  ///< Print vertex category
    };

    /// Export graph to Graphviz dot format
    void export_graphviz(std::ostream & out_, const uint32_t flags_ = 0) const;

  private:

    vertex_t _get_vertex_(const std::string & id_) const;

  private:

    // std::string _vertex_category_;
    std::string _edge_topic_;
    graph_t     _g_;

  };


} // end of namespace datatools

#endif // DATATOOLS_DEPENDENCY_GRAPH_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
