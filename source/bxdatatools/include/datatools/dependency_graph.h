/// \file datatools/dependency_graph.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2017-06-09
 * Last modified: 2017-06-18
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
#include <set>
#include <memory>

// Third party:
// - Boost:
#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/one_bit_color_map.hpp>
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
      std::string id;       ///< Unique name/identifier of the vertex
      std::string category; ///< Category of the vertex
      vertex_properties_t()
        : id("")
        , category("") {}
      vertex_properties_t(const std::string & id_, const std::string & category_)
        : id(id_)
        , category(category_) {}
    };

    struct edge_properties_t
    {
      std::string topic; ///< Topic associated to the edge
      edge_properties_t()
        : topic("")  {}
      edge_properties_t(const std::string & topic_)
        : topic(topic_) {}
    };

    struct graph_properties_t
    {
      std::string topic; ///< Topic associated to the graph
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

    /// \brief Visitor for cycle detection
    struct cycle_detector
      : public boost::dfs_visitor<>
    {
      cycle_detector(bool & has_cycle)
        : _has_cycle(&has_cycle)
        , _vertices(nullptr)
      {
        return;
      }

      cycle_detector(std::set<vertex_t> & vertices_)
        : _has_cycle(nullptr)
        , _vertices(&vertices_)
      {
        if (_vertices->size()) _vertices->clear();
        return;
      }

      void back_edge(edge_t e_, const graph_t & g_)
      {
        // Once a cycle is detected (we pass through a back-edge), tag the cycle:
        if (_vertices != nullptr) {
          _vertices->insert(boost::source(e_, g_));
          _vertices->insert(boost::target(e_, g_));
        }
        if (_has_cycle != nullptr) {
          *_has_cycle = true;
        }
        return;
      }

    protected:

      bool * _has_cycle;
      std::set<vertex_t> * _vertices = nullptr;

    };

    /// \brief Visitor for finding dependee vertices of given category
    class fvocf_visitor
      : public boost::dfs_visitor<>
    {
    public:

      struct end_of_algo_exception
        : public std::exception
      {
      };

      typedef boost::one_bit_color_map<boost::identity_property_map> color_map_type;

      fvocf_visitor(std::set<std::string> & ids_,
                    const graph_t & g_,
                    const std::string & category_)
        : _ids(ids_)
        , _g(g_)
        , _category(category_)
      {
        // _color_map_ptr.reset(new color_map_type(boost::num_vertices(_g)));
        return;
      }

      // // Vertex color is set to black
      // void initialize_vertex(vertex_t v_, const graph_t & /*g_*/)
      // {
      //   // boost::put(*_color_map_ptr,
      //   //            v_,
      //   //            boost::color_traits<boost::one_bit_color_type>::black());
      //   // std::cerr << "[devel] Initialize vertex [" << v_<< "]"<< std::endl;
      //   // if (_start == ) {
      //   //  _start = v_;
      //   // }
      //   // with color " << boost::get(*_color_map_ptr, v_) << std::endl;
      //   return;
      // }

      // // Start vertex color is set to white
      // void start_vertex(vertex_t v_, const graph_t & /*g_*/)
      // {
      //   // boost::put(*_color_map_ptr,
      //   //            v_,
      //   //            boost::color_traits<boost::one_bit_color_type>::white());
      //   // std::cerr << "[devel] Start vertex [" << v_ << "]" << std::endl;
      //   // with color " << boost::get(*_color_map_ptr, v_) << std::endl;
      //   return;
      // }

      // void back_edge(edge_t e_, const graph_t & g_)
      // {
      //   // std::cerr << "[devel] Back edge [" << boost::source(e_, g_) << "] -> [" << boost::target(e_, g_) << "]" << std::endl;
      //   // if (boost::source(e_, g_) == _start) {
      //   //   std::cerr << "[devel] Back to start vertex: STOP!" << std::endl;
      //   //   throw end_of_algo_exception();
      //   // }
      //   return;
      // }

      // void examine_edge(edge_t e_, const graph_t & g_)
      // {
      //   // std::cerr << "[devel] Found edge [" << boost::source(e_, g_) << "] -> [" << boost::target(e_, g_) << "]" << std::endl;
      //   return;
      // }

      void discover_vertex(vertex_t v_, const graph_t & g_)
      {
        // std::cerr << "[devel] Discover vertex [" << v_<< "]" << std::endl;
        if (_start == std::numeric_limits<vertex_t>::max()) {
          // Record the start vertex:
          _start = v_;
        }
        const vertex_properties_t & vp = g_[v_];
        if (vp.category == _category) {
          _ids.insert(vp.id);
          // boost::put(*_color_map_ptr,
          //            v_,
          //            boost::color_traits<boost::one_bit_color_type>::white());
        } else {

        }
        return;
      }

      void finish_vertex(vertex_t v_, const graph_t & /*g_*/)
      {
        // boost::put(*_color_map_ptr,
        //            v_,
        //            boost::color_traits<boost::one_bit_color_type>::black());
        // std::cerr << "[devel] Finish vertex [" << v_<< "]" << std::endl;
        if (v_ == _start) {
          // std::cerr << "[devel] Back to start vertex [" << v_ << "]" << std::endl;
          throw end_of_algo_exception();
        }
       return;
      }

    protected:

      std::set<std::string> & _ids;
      const graph_t & _g;
      std::string _category;
      vertex_t _start = std::numeric_limits<vertex_t>::max();
      // std::shared_ptr<color_map_type> _color_map_ptr;

    };

    /// Check if some dependency cycle is detected
    bool has_cycle() const;

    /// Find vertices belonging to cycles
    bool find_vertices_in_cycles(std::set<vertex_t> &) const;

    /// Find all vertices of a given category from a starting vertex
    std::set<std::string> find_vertices_of_category_from(const std::string & from_id_,
                                                         const std::string & category_) const;

    // /// Find depender vertices of a given category from a dependee vertex
    // std::set<std::string> find_dependers_of_category_from(const std::string & dependee_id_,
    //                                                       const std::size_t level_) const;

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

    std::string get_vertex_id(const vertex_t &) const;

  private:

    vertex_t _get_vertex_(const std::string & id_) const;

  private:

    graph_t _g_; ///< BGL embedded model

  };

} // end of namespace datatools

#endif // DATATOOLS_DEPENDENCY_GRAPH_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
