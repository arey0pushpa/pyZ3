#include<vts.h>
#include<z3-util.h>

// define n dimentional vectors
//#include "boost/multi_array.hpp"
//#include <cassert>

// Define the value of the num_nodes, total_molecules etc.
// #define n 2    // num of nodes.
// #define m 2    // num of molecules.
// #define q 1    // num of parellel edges.
// #define v 1    // model variation.
// #define c 2    // connectivity.

// Create a 3D array that is 3 x 4 x 2
//typedef boost::multi_array<z3::expr, 3> array_type;
//typedef array_type::index index;
//array_type A(boost::extents[3][4][2]);

//============================================================================
// Populates the vectors with z3 variables

void vts::popl1 ( VecExpr& m, unsigned arg1,
                  std::string prefix ) {
  for ( unsigned int i = 0; i < arg1; i++ ) {
    std::string name = prefix + "_" + std::to_string(i);
    m.push_back( make_bool( ctx, name ) );
  }
}

void vts::popl2 ( Vec2Expr& m, unsigned arg1, unsigned arg2,
                  std::string prefix) {
  m.resize(arg1);
  for ( unsigned int i = 0; i < arg1; i++ )
    popl1( m[i], arg2, prefix+"_"+ std::to_string(i) );
}

void vts::popl3 ( Vec3Expr& m, unsigned arg1, unsigned arg2,
                  unsigned arg3, std::string prefix) {
  m.resize(arg1);
  for ( unsigned int i = 0; i < arg1; i++ )
    popl2( m[i], arg2, arg3, prefix+"_"+ std::to_string(i) );
}

void vts::popl4 ( Vec4Expr& m, unsigned arg1, unsigned arg2,
                  unsigned arg3, unsigned arg4, std::string prefix) {
  m.resize(arg1);
  for ( unsigned int i = 0; i < arg1; i++ )
    popl3( m[i], arg2, arg3, arg4, prefix+"_"+ std::to_string(i) );
}

void vts::init_vts() {

// Populate edges: e(i,j,q)
  popl3( edges, N, N, E_arity, "e" );

// Populate nodes: n(i,j)
  popl2 ( nodes, N, M, "n" );

// Populate active_node (i, k)
  popl2 ( active_node, N, M, "a" );

// Populate presence_edge(i,j,q,k)
  popl4 ( presence_edge, N, N, E_arity, M, "e" );

// Populate active_edge(i,j,q,k)
  popl4 ( active_edge, N, N, E_arity, M, "b" );

// Populate pairing_m(k,k1)
  popl2 ( pairing_m, M, M, "p" );

// Populate node_funcs : Currently not handled.
// Populate edge_funcs : Currently not handled.

// Populate reach(i,j,k,z)
  popl4 ( reach, N, N, M, N-1, "r" );

// Populate drop1(i,j,q)
  popl3 ( drop1, N, N, E_arity, "d1");

//Populate d_reach(i,j)
  popl2 ( d_reach, N , N, "r1" );


}


z3::expr vts::no_self_edges() {                              //V5
  z3::expr_vector ls(ctx);
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned q = 0 ; q < E_arity; q++ ) {
      ls.push_back( !edges[i][i][q] );
    }
  }
  return z3::mk_and( ls );
}

