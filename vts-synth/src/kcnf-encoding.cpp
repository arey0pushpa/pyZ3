#include<vts.h>
#include<z3-util.h>

/*********   N [here = 3] CNF  Encoding *************************************
 *
 *   lit_list ::= a1 C1 || a2 C2 || ... || an Cn 
 *   cl_list  ::= a1 + a2 + a3 + ... + an <= N
 *   il_list  ::= !Coeff (C1) || !Coeff (C2) 
 *   
 *   outer_list ::=  Sum_i { [lit_list] && [cl_list] && [il_list] } 
 * 
 * * *************************************************************************/

// Bool e :: edge or not (node) : [arg_list_2d, N, M, 0/1, N, E_arity]
z3::expr vts::literal_cnf ( Vec3Expr s, unsigned i, unsigned k, bool e, unsigned j = 0, unsigned q = 0 ) {
  z3::expr_vector outer_list(ctx);
  z3::expr_vector lit_listC(ctx);
  z3::expr_vector lit_listI(ctx);
  // d :: conjunction level/depth
  for ( unsigned d = 0; d < D; d++ ) {
    z3::expr_vector il_list(ctx);
    z3::expr_vector cl_list(ctx);
    z3::expr_vector inner_list(ctx);
    for ( unsigned k1 = 0; k1 < M; k1++ ) {
      if ( k1 == k )  continue;
      if ( e == true ) 
        inner_list.push_back( ( s[k][k1][d] && presence_edge[i][j][q][k1] ) || ( s[k][k1+M][d] && !presence_edge[i][j][q][k1] ) );
      else 
        inner_list.push_back( ( s[k][k1][d] && nodes[i][k1] ) || ( s[k][k1+M][d] && !nodes[i][k1] ) );
      cl_list.push_back( s[k][k1][d] );
      cl_list.push_back( s[k][k1+M][d] );
      il_list.push_back( !s[k][k1][d] || !s[k][k1+M][d] ) ;
    }
    // At most 2 
    auto cConst =  ! (at_least_two ( cl_list, cl_list.size() ) );
    lit_listC.push_back( cConst );
    auto iConst = mk_and ( il_list );
    lit_listI.push_back( iConst  );
    auto cons = mk_or ( inner_list ); 
    outer_list.push_back( cons && iConst && cConst );
  }
  auto cons = mk_and ( outer_list );
  return cons;
}

/*** Node activity constraint: a[i][k]  ***/
z3::expr vts::node_cnf ( Vec3Expr& s ) {
  z3::expr_vector main_list(ctx);
  for ( unsigned i = 0; i < N; i++ ) {
    for ( unsigned k = 0; k < M; k++ ) {
      auto cnf = (active_node[i][k] == literal_cnf (s, i, k, false) );
      main_list.push_back ( cnf ); 
    }
  }
  auto cons = mk_and ( main_list );
  return cons;
}

/*** Edge activity constraint: b[i][j][q][k]  ***/
z3::expr vts::edge_cnf ( Vec3Expr& t ) {
  z3::expr_vector main_list(ctx);
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if ( i == j )  continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        for ( unsigned k = 0; k < M; k++ ) { 
          //z3::expr_vector outer_list(ctx);
          auto cnf =  ( active_edge[i][j][q][k] == literal_cnf (t, i, k, true, j, q) ) ;  
          main_list.push_back ( cnf ); 
        }
      }
    }
  }
  auto cons = mk_and ( main_list );
  return cons;
}

/** Function has a restricted form with Three CNF  **/ 
z3::expr vts::cnf_function ( Vec3Expr& s_var, Vec3Expr& t_var ) { 
  z3::expr nodeCnf = node_cnf ( s_var );
  //std::cout << nodeCnf << "\n";
  z3::expr edgeCnf = edge_cnf ( t_var ); 
  //std::cout << edgeCnf << "\n";
  auto cons = nodeCnf && edgeCnf;
  return cons;
}
