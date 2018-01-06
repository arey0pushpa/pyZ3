#include<vts.h>
// define n dimentional vectors 
//#include "boost/multi_array.hpp"
//#include <cassert>

// Define the value of the num_nodes, total_molecules etc.
#define n 2    // num of nodes.
#define m 2    // num of molecules.
#define q 1    // num of parellel edges.
#define v 1    // model variation.
#define c 2    // connectivity.

// Create a 3D array that is 3 x 4 x 2
//typedef boost::multi_array<z3::expr, 3> array_type;
//typedef array_type::index index;
//array_type A(boost::extents[3][4][2]);


void popl1 ( VecExpr& m, unsigned arg1,
                std::string& prefix) {
  // Create a 1 dim vector m with dimentions arg1.
  VecExpr m ( arg1 ); 
  for ( unsigned int i = 0; i < arg1; i++ ) {
    m[i] = prefix + "_" + to_string(arg1);
  }
}
  
void popl2 ( Vec2Expr& m, unsigned arg1, unsigned arg2, 
                            std::string& prefix) {
  
  // Create a 2 dim vector m  with dimentions arg1 arg2.
  Vec2Expr m ( arg1, VectExpr ( arg2 ) ); 
  
  // Populate the vector. 
  for ( unsigned int i = 0; i < arg1; i++ ) {
    for ( unsigned int j = 0; j < arg2; j++ ) {
        m[i][j]  = prefix + "_" + to_string(arg1) + "_" + to_string(arg2);
    }
  }
}

void popl3 ( Vec3Expr& m, unsigned arg1, unsigned arg2, 
                            unsigned arg3, std::string& prefix) {
  // Create a 3 dim vector m  with dimention arg1 arg2 arg3.
  Vec3Expr m ( arg1, Vec2Expr ( arg2, VecExpr (arg3) ) );
 
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

void popl4 ( Vec3Expr& m, unsigned arg1, unsigned arg2, 
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
  edges = popl3 ( edges, n, n, q );

// Populate nodes: n(i,j)
  nodes = popl2 ( nodes, n, m );

// Populate active_node (i, k)
  active_node = popl2 ( active_node, n, m );

// Populate presence_edge(i,j,q,k)
  presence_edge = popl4 ( presence_edge, n, n, q, m );

// Populate active_edge(i,j,q,k)
  active_edge = popl4 ( active_edge, n, n, q, m );
  
// Populate pairing_m(k,k1)
  pairing_m = popl2 ( pairing_m, m, m );

// Populate node_funcs : Currently not handled.
// Populate edge_funcs : Currently not handled.

// Populate reach(i,j,k,z)
  reach = popl4 ( n, n, m, n-1 );

// Populate drop1(i,j,q)
  drop = popl3 ( n, n, q );

//Populate d_reach(i,j)
  d_reach = popl2 ( n , n ); 

}  
