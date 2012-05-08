/*
 * =====================================================================================
 *
 *       Filename:  floyd-warshall.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/08/2012 07:32:50 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <iterator>
#include <algorithm>
#include <exception>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <boost/graph/graphviz.hpp>

struct EdgeProperty {
    int weight;
};

struct VertexProperty {
    int id;
    double x, y;
};


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  floyd_warshall
 *  Description:  find all-pairs shortest paths
 * =====================================================================================
 */
template <typename Graph, typename DistanceMatrix, typename PredecessorMatrix>
bool floyd_warshall (Graph& g, DistanceMatrix& d, PredecessorMatrix& p)
{
    typename boost::graph_traits<Graph>::vertex_iterator
                i, lasti, j, lastj, k, lastk;
    typename boost::graph_traits<Graph>::out_edge_iterator ei, ei_end;

    using boost::tie;
    using boost::vertices;
    using boost::out_edges;

    const int infinity = std::numeric_limits<int>::max();

    // init
    for(tie(i, lasti) = vertices(g); i != lasti; ++i) {
        for(tie(j, lastj) = vertices(g); j != lastj; ++j) {
            d[*i][*j] = infinity;
            p[*i][*j] = -1;
        }

        for(tie(ei, ei_end) = out_edges(*i, g); ei != ei_end; ++ei) {
            d[*i][boost::target(*ei, g)] = g[*ei].weight;
            p[*i][boost::target(*ei, g)] = *i;
        }

        d[*i][*i] = 0;
        p[*i][*i] = *i;
    }

    // find shortest paths
    for(tie(k, lastk) = vertices(g); k != lastk; ++k)
        for(tie(i, lasti) = vertices(g); i != lasti; ++i)
            if(d[*i][*k] != infinity)
                for(tie(j, lastj) = vertices(g); j != lastj; ++j)
                    if( (d[*k][*j] != infinity) &&
                            (d[*i][*j] > d[*i][*k] + d[*k][*j]) ) 
                    {
                        d[*i][*j] = d[*i][*k] + d[*k][*j];
                        p[*i][*j] = p[*k][*j];
                    }
    
    // check
    for(tie(i, lasti) = vertices(g); i != lasti; ++i)
        if(d[*i][*i] < 0)
            return false; // negative cycle

    return true;
}
/* -----  end of template function floyd_warshall  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  print_paths
 *  Description:
 * =====================================================================================
 */
template <typename Graph, typename DistanceMatrix, typename PredecessorMatrix>
void print_paths ( Graph& g, DistanceMatrix& d, PredecessorMatrix& p) {
    
    typename boost::graph_traits<Graph>::vertex_iterator i, lasti, j, lastj;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor;
    typename boost::graph_traits<Graph>::out_edge_iterator ei, ei_end;

    using boost::tie;
    using boost::vertices;
    using boost::out_edges;

    const vertex_descriptor no_pred = static_cast<vertex_descriptor>(-1);
    
    std::cout << "Đường đi ngắn nhất:\n";
    for(tie(i, lasti) = vertices(g); i != lasti; ++i) {
        std::cout << "Từ đỉnh (" << *i << "):\n";
        for(tie(j, lastj) = vertices(g); j != lastj; ++j)
            if( (p[*i][*j] != no_pred) && (*i != *j) ) {
                std::cout << "\t" << *i << " --> " << *j << " : ";

                // reconstruct path
                std::deque<vertex_descriptor> path;
                vertex_descriptor pred = *j;
                do {
                    pred = p[*i][pred];
                    path.push_front(pred);
                } while(pred != *i);

                // print path
                std::ostream_iterator<vertex_descriptor> oi(std::cout, "->");
                std::copy(path.begin(), path.end(), oi);
                std::cout << *j << "\n\tĐộ dài đường đi: " << d[*i][*j] << "\n\n";
            }
        std::cout << "\n";
    }
}
/* -----  end of function print_paths  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cerr << "Tham số không hợp lệ\n";
        return -1;
    }

    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, 
            VertexProperty, EdgeProperty> graph_t;
    typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_d;
    typedef std::pair<int, int> edge_t;

    graph_t g;
    
    boost::dynamic_properties dp;
    dp.property("weight", boost::get(&EdgeProperty::weight, g));
    dp.property("id", boost::get(&VertexProperty::id, g));
    dp.property("x", boost::get(&VertexProperty::x, g));
    dp.property("y", boost::get(&VertexProperty::y, g));

    std::ifstream in(argv[1]);
    try {
        boost::read_graphviz(in, g, dp, "id");
    } catch (std::exception &ex) {
        std::cerr << ex.what() << '\n';
        return -1;
    }
    
    size_t num_vertices = boost::num_vertices(g);

    std::vector<std::vector<int> > 
        distance_matrix(num_vertices, std::vector<int>(num_vertices));
    std::vector<std::vector<vertex_d> > 
        predecessor_matrix(num_vertices, std::vector<vertex_d>(num_vertices));

    bool success = floyd_warshall(g, distance_matrix, predecessor_matrix);
    
    if(!success) {
        std::cerr << "Đồ thị có chu trình âm\n";
        return -1;
    }

    print_paths(g, distance_matrix, predecessor_matrix);

    return 0;
}
