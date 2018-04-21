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

void vts::add_known_edge( unsigned n1, unsigned n2, unsigned q,
                          std::vector<unsigned>& mols, std::vector<bool>& act ) {
  //edges[n1][n2][q] = true;
  for ( unsigned i = 0; i < mols.size(); i++ ) {
     //presence_edge[n1][n2][q][mols[i]] = true;
  }
}

void vts::add_known_node( unsigned n,
                          std::vector<unsigned>& mols, std::vector<bool>& act ) {
  for ( unsigned i = 0; i < mols.size(); i++ ) {
    //nodes[n][mols[i]] = true;
  } 
}

void vts::add_known_pairing( unsigned m1, unsigned m2 ) {
  //pairing_m[m1][m2] = true;
}

void vts::add_known_activity_node_function( unsigned m, z3::expr f ) {
  
}

void vts::add_known_activity_edge_function( unsigned m, z3::expr f ) {
  
}

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

void vts::make_func ( std::vector<z3::func_decl>& fs, std::string prefix ) {
  // It should be an array.
  z3::sort_vector sorts(ctx);
  // std::vector<z3::sort> sorts;
  for ( unsigned int m = 0; m < M; m++ ) {
    sorts.push_back(ctx.bool_sort()); //not sure
  }
  for ( unsigned int m = 0; m < M; m++ ) {
    std::string name = prefix + "_" + std::to_string(m);
    z3::func_decl f = ctx.function( name.c_str(), sorts, ctx.bool_sort() );
    fs.push_back( f );
  }
}

void vts::init_vts() {

// Populate edges: e(i,j,q)
  popl3( edges, N, N, E_arity, "z" );

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
/*
// Populate xtra var s_var : var for node function
  //popl2 ( s_var, 2 * (M-1), J, "s" );
  popl2 ( s_var, 2 * M, J, "s" );

// Populate xtra var t_var : var for node function 
  //popl2 ( t_var, 2 * (M-1), J, "t" );
  popl2 ( t_var, 2 * M, J, "t" );
*/
// Populate node_funcs : Currently not handled.
  make_func( node_funcs, "an" );

// Populate edge_funcs : Currently not handled.
  make_func ( edge_funcs, "en" );

// Populate reach(i,j,k,z)
  popl4 ( reach, N, N, M, N-1, "r" );

// Populate drop1(i,j,q)
  popl3 ( drop1, N, N, E_arity, "d1" );

//Populate d_reach1(i,j)
  popl2 ( d_reach1, N , N, "r1" );

// Populate drop2(i,j,q)
  popl3 ( drop2, N, N, E_arity, "d2" );

//Populate d_reach(i,j)
  popl2 ( d_reach2, N , N, "r2" );
}

z3::expr vts::is_mol_edge_present( unsigned i, unsigned j, unsigned m ) {
  z3::expr_vector p_list(ctx);
  for( unsigned q = 0; q < E_arity ; q++ ) {
    p_list.push_back( presence_edge[i][j][q][m] );
  }
  return z3::mk_or( p_list );
}

z3::expr vts::is_qth_edge_present( unsigned i, unsigned j, unsigned q ) {
  z3::expr_vector p_list(ctx);
  for( unsigned m = 0; m < M ; m++ ) {
    p_list.push_back( presence_edge[i][j][q][m] );
  }
  return z3::mk_or( p_list );
}


/* Activity Contraints ------ */

// Regulation : No regulation on the node.
// The present molecules at nodes are all active.
z3::expr vts::always_active_on_node() { // f_nn
  z3::expr_vector ls(ctx);
  z3::expr lhs(ctx);
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned m = 0 ; m < M; m++ ) {
        lhs = ( nodes[i][m] == active_node[i][m] );
        ls.push_back( lhs );
    }
  }
  return z3::mk_and( ls );
}

// Regulation : No regulation on the edge.
// The present molecules on the edge are all active.
z3::expr vts::always_active_on_edge() { // f_ne
  z3::expr_vector ls(ctx);
  z3::expr lhs(ctx);
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if (j == i)
        continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        for ( unsigned m = 0; m < M; m++ ) {
          lhs = ( presence_edge[i][j][q][m] == active_edge[i][j][q][m] );
          ls.push_back( lhs );
        }
      }
    }
  }
  return z3::mk_and( ls );
}

// Regulation : SNARE-SNARE Inhibition.
z3::expr vts::pm_dependent_activity_on_edge() { //f_se
  z3::expr_vector ls(ctx);
  z3::expr lhs(ctx);
  z3::expr l1(ctx);
  z3::expr l2(ctx);
  z3::expr x1(ctx);
  z3::expr x2(ctx);

  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if (j == i)
        continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        for ( unsigned m = 0; m < M; m++ ) {
          lhs = presence_edge[i][j][q][m];
          l1 = ctx.bool_val(false);
          l2 = ctx.bool_val(true);
          for ( unsigned m1 = 0; m1 < M; m1++ ) {
            if (j == i)
              continue;
            l1 = ( pairing_m[m][m1] || l1);
            l2 = ( implies( pairing_m[m][m1], presence_edge[i][j][q][m1]) &&  l2);
          }
          x1 = implies ( ( lhs && l1 && l2 ), !active_edge[i][j][q][m] );
          x2 = implies ( lhs && !( l1 && l2 ) , active_edge[i][j][q][m] );
          ls.push_back( x1 && x2 );
        }
      }
    }
  }
  return z3::mk_and( ls );
}


z3::expr vts::func_driven_activity_on_node() { //f_bn
  z3::expr_vector ls (ctx);
  z3::expr_vector s (ctx);
  z3::expr lhs(ctx);
 // todo : type f, f_app; 
 // todo : type f, f_app;
  for ( unsigned m = 0; m < M; m++ ) {
    auto f = node_funcs[m];
    for( unsigned i = 0 ; i < N; i++ ) {
      for ( unsigned m1 = 0; m1 < M; m1++ ) {
        if (m1 == i)
          continue;
        s.push_back( nodes[i][m1] );
      }
      auto f_app = f(s);
      lhs = implies( nodes[i][m], active_node[i][m] == f_app );
      ls.push_back( lhs );
    }
  }
  return z3::mk_and( ls );
}

// f_be: BOolean function on edge.
z3::expr vts::func_driven_activity_on_edge() { //f_be
  z3::expr_vector ls(ctx);
  z3::expr_vector s (ctx);
  z3::expr lhs(ctx);
 // // todo : type f, f_app;
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if (j == i)
        continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        for ( unsigned m = 0; m < M; m++ ) {
          auto f = edge_funcs[m];
          for ( unsigned m1 = 0; m1 < M; m1++ ) {
            if (j == i)
              continue;
            s.push_back ( presence_edge[i][j][q][m1] );
          }
          lhs = implies( presence_edge[i][j][q][m], active_edge[i][j][q][m] == f(s) ) ;
          ls.push_back ( lhs );
        }
      }
    }
  }
  return z3::mk_and( ls );
}


//todo: the following functions need correct implementation

z3::expr vts::node_activity_constraint() {
    switch(V) {
    case MODEL_1: return always_active_on_node(); break;
    //case MODEL_2: return func_driven_activity_on_node(); break;
    case MODEL_2: return ctx.bool_val(true); break;
    case MODEL_3: return always_active_on_node(); break;
    //case MODEL_4: return func_driven_activity_on_node(); break;
    case MODEL_4: return ctx.bool_val(true); break;
    case MODEL_5: return always_active_on_node(); break;
    //case MODEL_6: return func_driven_activity_on_node(); break;
    case MODEL_6: return ctx.bool_val(true); break;
    default:
      return ctx.bool_val(true);
    }
}

z3::expr vts::edge_activity_constraint() {
  switch(V) {
  case MODEL_1: return always_active_on_edge(); break;
  case MODEL_2: return always_active_on_edge(); break;
  //case MODEL_3: return func_driven_activity_on_edge(); break;
  case MODEL_3: return ctx.bool_val(true); break;
  //case MODEL_4: return func_driven_activity_on_edge(); break;
  case MODEL_4: return ctx.bool_val(true); break;
  case MODEL_5: return pm_dependent_activity_on_edge(); break;
  case MODEL_6: return pm_dependent_activity_on_edge(); break;
  default:
    return ctx.bool_val(true);
  }
}



/* Basic Constraints ... V1-V8.*/
//V1: For an edge to exist it should have one molecule present.
z3::expr vts::molecule_presence_require_for_present_edge() {
  z3::expr_vector ls(ctx);
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if (j == i)
        continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        ls.push_back( implies (is_qth_edge_present(i,j,q ), edges[i][j][q]) );
        // z3::expr rhs = ctx.bool_val(false);
        // for ( unsigned m = 0; m < M; m++ ) {
        //   rhs = presence_edge[i][j][q][m] || rhs;
        // }
        // ls.push_back( implies (rhs, edges[i][j][q]) );
      }
    }
  }
  return z3::mk_and( ls );
}

// V2: If molecule is active on an edge then it should be present on the edge.
z3::expr vts::active_molecule_is_present_on_edge() {         //V2
  z3::expr_vector ls(ctx);
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if (j == i)
        continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        for ( unsigned m = 0; m < M; m++ ) {
          z3::expr e =  implies( active_edge[i][j][q][m] , presence_edge[i][j][q][m] );
          ls.push_back( e );
        }
      }
    }
  }
  return z3::mk_and( ls );
}


// V3: A mmolecule should be present to be active.
z3::expr vts::active_molecule_is_present_on_node() {         //V3
  z3::expr_vector ls(ctx);
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned m = 0 ; m < M; m++ ) {
      z3::expr e = z3::implies( active_node[i][m], nodes[i][m] );
      ls.push_back ( e );
    }
  }
  return z3::mk_and( ls );
}

// V4: The edge label are subset of the source and target.
z3::expr vts::edge_label_subset_of_node_label() { //V4
  z3::expr_vector ls(ctx);
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if (j == i)
        continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        for( unsigned m = 0 ; m < M; m++ ) {
          z3::expr e = z3::implies( presence_edge[i][j][q][m], nodes[i][m] && nodes[j][m] );
          ls.push_back(e);
        }
      }
    }
  }
  return z3::mk_and( ls );
}


// V5 : that no self edges are allowed.
z3::expr vts::no_self_edges() {                              //V5
  z3::expr_vector ls(ctx);
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned q = 0 ; q < E_arity; q++ ) {
      ls.push_back( !edges[i][i][q] );
    }
  }
  return z3::mk_and( ls );
}

// V6: Only Q R entry has possible non zero entry.

z3::expr vts::restriction_on_pairing_matrix() {              //V6
  z3::expr_vector ls(ctx);
  z3::expr e(ctx);
  for( unsigned x = 0 ; x < M; x++ ) {
    for( unsigned y = 0 ; y < M; y++ ) {
      if ( ((x < M/2) && (y < M/2)) || ((x>=M/2) && (y >=M/2)) ) {
        e = !pairing_m[x][y];
        ls.push_back( e );
      }
    }
  }
  return z3::mk_and( ls );
}


// V7 : There should be an active pair corresponding to pairing matrix
z3::expr vts::edge_must_fuse_with_target() {                 //V7
  z3::expr_vector ls(ctx);
  z3::expr lhs(ctx);
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if (j == i)
        continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        lhs = ctx.bool_val(false);
        for ( unsigned m = 0; m < M; m++ ) {
          for ( unsigned m1 = 0; m1 < M; m1++ ) {
            if (m == m1)
              continue;
            lhs =  ( (active_edge[i][j][q][m] && active_node[j][m1] && pairing_m[m][m1]) ||  lhs );
          }
        }
        z3::expr e =  implies ( edges[i][j][q], lhs );
        ls.push_back( e );
      }
    }
  }
  return z3::mk_and( ls );
}

//  V8: Edge should not be able to potentially fuse with
//      any node other than it's target.
z3::expr vts::edge_fuse_only_with_target() {       //V8
  z3::expr_vector ls(ctx);
  z3::expr lhs(ctx);
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if (j == i)
        continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        for ( unsigned m = 0; m < M; m++ ) {
          lhs = ctx.bool_val(false);
            for( unsigned j1 = 0 ; j1 < N; j1++ ) {
              if (j1 == j)
                continue;
              for ( unsigned m11 = 0; m11 < M; m11++ ) {
              if (m == m11)
                continue;
              lhs =  ( ( active_node[j1][m11] && pairing_m[m][m11] ) ||  lhs );
              }
            }
            z3::expr e = implies( active_edge[i][j][q][m], !lhs );
            ls.push_back( e );
        }
      }
    }
  }
  return z3::mk_and( ls );
}

/*** Reachability  Definition ***/
z3::expr vts::reachability_def() {
  z3::expr_vector a_list(ctx);
  for( unsigned i = 0; i < N; i++ ) {
    for( unsigned j = 0; j < N; j++ ) {
      if( i == j ) continue;
      for( unsigned m = 0; m < M; m++ ) {
        for( unsigned z = 0; z < N-1; z++ ) {
          if( z ==0 ) {
            a_list.push_back( z3::implies( reach[i][j][m][0], is_mol_edge_present(i,j,m)) );
          }else{
            z3::expr_vector rhs_list(ctx);
            for( unsigned l = 0; l < N; l++ ) {
              if( l == i || l == j ) continue;
              rhs_list.push_back( reach[l][j][m][z-1] && is_mol_edge_present(i,l,m) );
            }
            auto rhs = z3::mk_or( rhs_list );
            a_list.push_back( z3::implies( reach[i][j][m][z], rhs ) );
          }
        }
      }
    }
  }
  return z3::mk_and( a_list );
}

z3::expr vts::steady_state_stability_cond() { //R2
  z3::expr_vector a_list(ctx);
  for( unsigned i = 0; i < N; i++ ) {
    for( unsigned j = 0; j < N; j++ ) {
      if( i == j ) continue;
      for( unsigned m = 0; m < M; m++ ) {
        z3::expr_vector path_list(ctx);
        for( unsigned z = 0; z < N-1; z++ ) {
          path_list.push_back( reach[j][i][m][z] );
        }
        z3::expr path_i_j_m = z3::mk_or( path_list );
        //todo: the following is correct most likely
        // z3::expr path_i_j_m = reach[j][i][m][N-2];
        a_list.push_back( z3::implies( is_mol_edge_present(i,j,m), path_i_j_m));
      }
    }
  }
  return z3::mk_and( a_list );
}

//-------------------------------
/// Connectivity  ----------------------------
//------------------------

// Constraint D1 -------------
// D1: Only present edges can be dropped.
z3::expr vts::only_present_edges_can_be_dropped( Vec3Expr& dump ) { //
  z3::expr_vector ls(ctx);
  z3::expr lhs(ctx);
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if ( i == j )
        continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        ls.push_back ( implies (dump[i][j][q], edges[i][j][q]) );
      }
    }
  }
  return z3::mk_and( ls );
}


// falttening the array's 

//VecExpr vts::flattern_2d ( Vec2Expr& dump ) {
//  VecExpr d1;
//  for ( unsigned int i = 0; i < N; i++ ) {
//    for( unsigned j = 0 ; j < N; j++ ) {
//      if (j == i)
//        continue;
//        d1.push_back( dump[i][j] );
//     }
//    }
//  return d1;
//}

z3::expr vts::is_undirected_dumped_edge( unsigned i, unsigned j,
                                         Vec3Expr& dump ) { //
  z3::expr_vector p_list(ctx);
  //p_list.push_back ( ctx.bool_val(false) );
  for( unsigned q = 0; q < E_arity ; q++ ) {
    p_list.push_back(  (edges[i][j][q] && !dump[i][j][q]) ||  (edges[j][i][q] && !dump[j][i][q])  ); 
  }
  return z3::mk_or( p_list );
}

// undirected reachability
z3::expr vts::reachability_under_drop_def( Vec2Expr& r_vars,
                                           Vec3Expr& dump, unsigned conn_or_not ) { //
  z3::expr_vector cond_list(ctx);
  for ( unsigned i = 0; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if (j == i) continue;
      z3::expr ud_i_j = is_undirected_dumped_edge( i, j, dump );
      z3::expr_vector paths_list(ctx);
      for( unsigned l = 0; l < N; l++ ) {
        if( l == i || l == j ) continue;
        paths_list.push_back( is_undirected_dumped_edge(i,l,dump)
                              && r_vars[l][j] );
      }
      //auto cond = z3::implies( r_vars[i][j], ud_i_j || z3::mk_or( paths_list ) );
      if (conn_or_not == 0) {
      auto cond = z3::implies( r_vars[i][j], ud_i_j || z3::mk_or( paths_list ) );
      cond_list.push_back( cond );
      } else {
      auto cond = z3::implies( ud_i_j || z3::mk_or( paths_list ), r_vars[i][j] );
      cond_list.push_back( cond );
      }

    }
  }
  
  return z3::mk_and( cond_list );
}

z3::expr vts::remains_connected( Vec2Expr& r_varas ){                 //
  z3::expr_vector cond_list(ctx);
  for ( unsigned i = 0; i < N; i++ ) {
    for( unsigned j = i+1 ; j < N; j++ ) {
      if (j == i) continue;
      cond_list.push_back( r_varas[i][j] );
    }
  }
  return z3::mk_and( cond_list );
}

z3::expr vts::gets_disconnected( Vec2Expr& r_varas ){                 //
  z3::expr_vector cond_list(ctx);
  // Might not be neccessary
  cond_list.push_back ( ctx.bool_val(false) );
  for ( unsigned i = 0; i < N; i++ ) {
    for( unsigned j = i+1; j < N; j++ ) {
      if (j == i) continue;

      cond_list.push_back( !r_varas[i][j] );
      //cond_list.push_back( r_varas[i][j] );
    }
  }
  //std::cout << z3::mk_or( cond_list );
  //exit(0);
  return z3::mk_or( cond_list );
  //return ! (z3::mk_and( cond_list ) );
}

z3::expr vts::not_k_connected( unsigned k, Vec2Expr& r_varas, Vec3Expr& dump ) {
  return only_present_edges_can_be_dropped( dump )
    && exactly_k_drops( k, dump )
    && reachability_under_drop_def( r_varas, dump, 1 )
    && gets_disconnected( r_varas );
    //&& remains_connected( r_varas );
}

z3::expr vts::k_min_1_connected( unsigned k, Vec2Expr& r_varas, Vec3Expr& dump ) {
  return only_present_edges_can_be_dropped( dump )
    && exactly_k_drops( k, dump )
    && reachability_under_drop_def( r_varas, dump, 0 )
    && remains_connected( r_varas );
}

z3::expr vts::not_a_function( Vec2Expr& nodes, Vec2Expr& active_node) {
  z3::expr_vector cond_list(ctx);
  
  for ( unsigned i = 0; i < N; i++ ) {
    for( unsigned j = i+1 ; j < N; j++ ) {
      z3::expr_vector node_eq(ctx);
      z3::expr_vector actv_node_eq(ctx);
      for ( unsigned int k = 0; k < M; k ++ ) {
        node_eq.push_back( nodes[i][k] == nodes[j][k] );
        actv_node_eq.push_back( active_node[i][k] == active_node[j][k] );
      }
      auto cond = z3::implies( z3::mk_and (node_eq), z3::mk_and (actv_node_eq)  );
      cond_list.push_back( cond );
    }
  }
  
  return (! (z3::mk_and (cond_list) ));
}

/** Create constraint formula for vts **
 *
 * 1. Basic constraints : (V1...V6) - V5 
 * 2. Self edge disallowed : V5
 * 3. Stability : R1, R2  
 * 4. Fusion : V7, V8
 * Create_vts_constraint = 1 + 2 + 3 + 4
 *
 */

z3::expr vts::vts_activity_constraint () {
  z3::expr a1 = node_activity_constraint();
  z3::expr a2 = edge_activity_constraint();
  auto cons = a1 && a2;
  return cons;
}

z3::expr vts::vts_basic_constraints () {  
  z3::expr v1 = molecule_presence_require_for_present_edge(); //V1
  z3::expr v2 = active_molecule_is_present_on_edge();         //V2
  z3::expr v3 = active_molecule_is_present_on_node();         //V3
  z3::expr v4 = edge_label_subset_of_node_label();            //V4
  z3::expr v6 = restriction_on_pairing_matrix();              //V6
  
  z3::expr base_cons = v1 && v2 && v3 && v4 && v6;
  return base_cons;
}

z3::expr vts::vts_self_edges_constraint () {
  z3::expr v5 = no_self_edges();                      //V5
  return v5;
}
  
z3::expr vts::vts_stability_constraint () {
  z3::expr r1 = reachability_def();                   //R1
  z3::expr r2 = steady_state_stability_cond();        //R2
  z3::expr stab = r1 && r2;
  return stab;
}

z3::expr vts::vts_fusion_constraint () {
  z3::expr v7 = edge_must_fuse_with_target();         //V7
  z3::expr v8 = edge_fuse_only_with_target();         //V8
  auto cons = v7 && v8;
  return cons;
}

z3::expr vts::create_vts_constraint () {
  auto cons = vts_basic_constraints() && vts_self_edges_constraint()
                                    && vts_stability_constraint()
                                    && vts_fusion_constraint();
  return cons;
}


/**
 * z3 model by solving built vts constraints
 */
z3::model vts::get_vts_for_prob1( ) {
  z3::expr v5 = no_self_edges();                              //V5
  z3::expr basic_constraints_with_stability = create_vts_constraint();
  //z3::expr cons =  basic_constraints_with_stability && edges[0][1][0] && not_k_connected();
  z3::expr cons =  basic_constraints_with_stability && edges[0][1][0];

  z3::solver s(ctx);
  s.add( cons );
  if( s.check() == z3::sat ) {
    z3::model m = s.get_model();
    std::cout << m << "\n";
    return m;
  }else{
    std::cout << "model is not feasible!";
    assert(false);
  }
  return s.get_model(); //dummy call
}
