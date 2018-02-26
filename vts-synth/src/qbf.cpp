#include<vts.h>
#include<z3-util.h>


z3::expr_vector vts::flattern_2d ( Vec2Expr& dump ) {
  z3::expr_vector d1(ctx);
  for ( unsigned int i = 0; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if (j == i) continue;
      d1.push_back( dump[i][j] );
    }	    
  }
  return d1;
}

z3::expr_vector vts::flattern3d ( Vec3Expr& dump ) {
  z3::expr_vector  d1(ctx);
  for ( unsigned int i = 0; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if (j == i) continue;
	for ( unsigned q = 0; q < E_arity; q++ ) {
	  d1.push_back( dump[i][j][q] );	
	}		       
    }
  }
 return d1;
}

z3::expr vts::get_qbf_formula(){

  z3::expr basic_constraints_with_stability = get_basic_constraints();
  //std::cout << basic_constraints_with_stability;

  z3::expr not_connected = not_k_connected( C, d_reach2, drop2 );

  z3::expr_vector set_edges = flattern3d ( edges );

  //z3::expr_vector d1 = flattern_2d ( d_reach );
  // Create:  Exists (setR1, reach_d1 && d1_all-conn )
  z3::expr is_reach = exists( flattern_2d( d_reach ), reachability_under_drop_def( d_reach , drop1, 0 ) && remains_connected( d_reach )  );
  
  //z3::expr_vector d2 = flattern3d ( drop1 );
  z3::expr k_1_connected = forall (  flattern3d (drop1), implies 
		  (  (exactly_k_drops ( C-1, drop1 ) && only_present_edges_can_be_dropped ( drop1 )), is_reach ) );
 
  //z3::expr cons = basic_constraints_with_stability && not_connected && k_1_connected;
  z3::expr at_least_k_edges = atleast( set_edges, 3);
  std::cout << at_least_k_edges;
  //exit(0);
  
  z3::expr cons = at_least_k_edges && not_connected && k_1_connected;
  return cons;

}

z3::model vts::get_vts_for_qbf() {
  z3::expr cons = get_qbf_formula ();
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

  return s.get_model();

}
