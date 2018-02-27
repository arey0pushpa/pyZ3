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


// At least 2 
z3::expr vts::at_least_two ( VecExpr dump, unsigned L ) {
  z3::expr_vector ls(ctx);
  for ( unsigned i = 0; i < L-1; i++ ) {
    for ( unsigned j = i+1; j < L; j++) {
      ls.push_back ( dump[i] && dump[j] );  
    }
  }
  return z3::mk_or( ls );
}

// At least 3
z3::expr vts::at_least_three ( VecExpr dump, unsigned L ) {
  z3::expr_vector ls(ctx);
  z3::expr lhs(ctx);
  for ( unsigned i = 0; i < L-2; i++ ) {
    for ( unsigned j = i+1; j < L-1; j++) {
      lhs =  ( dump[i] && dump[j] );  
      for ( unsigned k = j+1; k < L; k++) {
	ls.push_back ( lhs && dump[k] );
    }
   }
  }
  return z3::mk_or( ls );
}


// At least 4. 
z3::expr vts::at_least_four ( VecExpr dump, unsigned L ) {
  z3::expr_vector ls(ctx);
  z3::expr lhs(ctx);
  z3::expr lhs1(ctx);
  for ( unsigned i = 0; i < L-3; i++ ) {
    for ( unsigned j = i+1; j < L-2; j++) {
      lhs =  ( dump[i] && dump[j] );  
      for ( unsigned k = j+1; k < L-1; k++) {
        lhs1 = (lhs && dump[k]);
        for ( unsigned x = k+1; x < L; x++) {
	  ls.push_back ( lhs1 && dump[x] );
     }
    }
   }
  }
  return z3::mk_or( ls );
}

z3::expr vts::get_qbf_formula (VecExpr& edgeQuant){

  z3::expr basic_constraints_with_stability = get_basic_constraints();
  //std::cout << basic_constraints_with_stability;

  //z3::expr not_connected = not_k_connected( C, d_reach2, drop2 );

  VecExpr ee_set = flattern_3d ( edges );
  edgeQuant = ee_set;
    //for(auto& ee : ee_set ) {
     // std::cout << ee << "\n";
    //}
  //std::cout << " ee_set : " << ee_set << "\n";

  //z3::expr_vector d1 = flattern_2d ( d_reach );
  // Create:  Exists (setR1, reach_d1 && d1_all-conn )
  z3::expr is_reach1 = exists( flattern_2d( d_reach1 ), reachability_under_drop_def( d_reach1 , drop1, 0 ) && remains_connected( d_reach1 )  );
  
  z3::expr is_reach2 = exists( flattern_2d( d_reach2 ), reachability_under_drop_def( d_reach2 , drop2, 1 ) && gets_disconnected( d_reach2 )  );
  
  //z3::expr_vector d2 = flattern3d ( drop1 );
  //std::cout << "drop 1 is = " << d2 << "\n";
  z3::expr k_min_1_connected = forall (  flattern3d (drop1), implies 
		  (  (exactly_k_drops ( C-1, drop1 ) && only_present_edges_can_be_dropped ( drop1 )), is_reach1 ) );
 
  z3::expr k_not_connected = exists (  flattern3d (drop2), implies 
		  (  (exactly_k_drops ( C, drop2 ) && only_present_edges_can_be_dropped ( drop2 )), is_reach2 ) );
 
  //z3::expr cons = basic_constraints_with_stability && not_connected && k_1_connected;
  z3::expr at_least_k_edges = at_least_three( ee_set, ee_set.size() );
  //std::cout << at_least_k_edges;
  //exit(0);
  
  //z3::expr cons = exists ( set_edges, at_least_k_edges && not_connected && k_1_connected );
  z3::expr_vector set_edges = flattern3d ( edges );
  z3::expr cons = exists ( set_edges, at_least_k_edges && k_min_1_connected && k_not_connected );
 // std::cout << "Expected first level quant: " << set_edges << "\n";
  //std::cout << cons << "\n";
  return cons;

}

z3::model vts::get_vts_for_qbf() {
  VecExpr edgeQuant;
  z3::expr cons = get_qbf_formula ( edgeQuant );
  //std::cout << cons << "\n";
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
