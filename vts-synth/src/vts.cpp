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


void vts::popl1 ( VecExpr& m, unsigned arg1,
                std::string& prefix ) {
  // Create a 1 dim vector m with dimentions arg1.
  m.resize( arg1 );
  for ( unsigned int i = 0; i < arg1; i++ ) {
    std::string name = prefix + "_" + std::to_string(arg1);
    m[i] = make_bool( ctx, name );
  }
}

void vts::popl2 ( Vec2Expr& m, unsigned arg1, unsigned arg2,
                  std::string& prefix) {

  m.resize(arg1);
  for( auto& a : m) a.resize(arg2);

  // Create a 2 dim vector m  with dimentions arg1 arg2.
  // Vec2Expr m ( arg1, VectExpr ( arg2 ) );

  // Populate the vector.
  for ( unsigned int i = 0; i < arg1; i++ ) {
    for ( unsigned int j = 0; j < arg2; j++ ) {
        m[i][j]  = prefix + "_" + to_string(arg1) + "_" + to_string(arg2);
    }
  }
}

void vts::popl3 ( Vec3Expr& m, unsigned arg1, unsigned arg2,
                            unsigned arg3, std::string& prefix) {
  m.resize(arg1);
  for( auto& a : m) {
    a.resize(arg2);
    for( auto& b : a) b.resize(arg3);
  }

  // Create a 3 dim vector m  with dimention arg1 arg2 arg3.
  // Vec3Expr m ( arg1, Vec2Expr ( arg2, VecExpr (arg3) ) );

  // Populate the vector.
  for ( unsigned int i = 0; i < arg1; i++ ) {
    for ( unsigned int j = 0; j < arg2; j++ ) {
      for ( unsigned int k = 0; k < arg3; k++) {
          m[i][j][j]  = prefix + "_" + to_string(arg1) + "_" +
                              to_string(arg2) + "_" + to_string(arg3);
      }
    }
  }
}

void vts::popl4 ( Vec3Expr& m, unsigned arg1, unsigned arg2,
                            unsigned arg3, unsigned arg4,
                            std::string& prefix) {
  // Create a 4 dim vector m  with dimention arg1 arg2 arg3 arg4.
  Vec4Expr (arg1, ExprVec3Expr m ( arg2, Vec2Expr ( arg2, VecExpr (arg4) ) );

  // Populate the vector.
  for ( unsigned int i = 0; i < arg1; i++ ) {
    for ( unsigned int j = 0; j < arg2; j++ ) {
      for ( unsigned int k = 0; k < arg3; k++) {
          m[i][j][j]  = prefix + "_" + to_string(arg1) + "_" +
                              to_string(arg2) + "_" + to_string(arg3);
      }

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
