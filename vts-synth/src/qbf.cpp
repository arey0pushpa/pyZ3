#include<vts.h>
#include<z3-util.h>
#include <vector>
#include <iterator>


z3::expr_vector vts::flattern2d ( Vec2Expr& dump, unsigned s1, unsigned s2, bool eq ) {
  z3::expr_vector d1(ctx);
  for ( unsigned i = 0; i < s1; i++ ) {
    for( unsigned j = 0 ; j < s2; j++ ) {
      if ( eq == true && j == i) continue;
      d1.push_back( dump[i][j] );
    }	    
  }
  return d1;
}

z3::expr_vector vts::flattern3d ( Vec3Expr& dump, unsigned s1, unsigned s2, unsigned s3, bool eq) {
  z3::expr_vector  d1(ctx);
  for ( unsigned i = 0; i < s1; i++ ) {
    for( unsigned j = 0 ; j < s2; j++ ) {
      if ( eq == true && j == i) continue;
      for ( unsigned q = 0; q < s3; q++ ) {
        d1.push_back( dump[i][j][q] );	
      }		       
    }
  }
 return d1;
}

z3::expr_vector vts::flattern4d ( Vec4Expr& dump, unsigned s1, unsigned s2, unsigned s3, unsigned s4, bool eq ) {
  z3::expr_vector  d1(ctx);
  for ( unsigned i = 0; i < s1; i++ ) {
    for( unsigned j = 0 ; j < s2; j++ ) {
      if ( eq == true && j == i) continue;
      for ( unsigned q = 0; q < s3; q++ ) {
        for ( unsigned k = 0; k < s4; k++ ) {
          d1.push_back( dump[i][j][q][k] );	
        }
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

z3::expr vts::get_qbf_formula ( VecExpr& edgeQuant ) {

  /* Build Constrint Of the Form := [[1]] && [[2]] && [[3]]
   * [[1]] :: Connectivity Constraint : kConnected Graph
   * [[2]] :: V5 Constraint : Self edge not allowed
   * [[3]] :: No Function Possible Constraint : No Boolean func exists 
   */

  //z3::expr basic_constraints_with_stability = get_basic_constraints();
  //z3::expr not_connected = not_k_connected( C, d_reach2, drop2 );

  /***** Building [[1]] ****/
  VecExpr ee_set = flattern_3d ( edges );
  edgeQuant = ee_set;

  z3::expr_vector setR1 = flattern2d ( d_reach1, N, N, true );
  z3::expr_vector setR2 = flattern2d ( d_reach2, N, N, true );
  
  // Create:  Exists (setR1, reach_d1 && d1_all-conn )
  z3::expr is_reach1 = exists( setR1, reachability_under_drop_def( d_reach1 , drop1, 0 ) && remains_connected( d_reach1 )  ); 
  z3::expr is_reach2 = exists( setR2, reachability_under_drop_def( d_reach2 , drop2, 1 ) && gets_disconnected( d_reach2 )  );
  
  z3::expr_vector setD1 = flattern3d ( drop1, N, N, E_arity, true );
  z3::expr_vector setD2 = flattern3d ( drop2, N, N, E_arity, true );
  
  z3::expr k_min_1_connected = forall (  setD1, implies 
		  (  (exactly_k_drops ( C-1, drop1 ) && only_present_edges_can_be_dropped ( drop1 )), is_reach1 ) );
 
  z3::expr k_not_connected = exists ( setD2,  
		    (exactly_k_drops ( C, drop2 ) && only_present_edges_can_be_dropped ( drop2 )) && is_reach2 ) ;
 
  z3::expr at_least_k_edges = at_least_three( ee_set, ee_set.size() );
  
  
  // [[1]] : K Connected Graph 
  // EXISTS [ setE, kedges && k-1Conn && knotConn ]
  z3::expr_vector setE = flattern3d ( edges, N, N, E_arity, true );
  z3::expr kconnectedConstraint = exists ( setE, at_least_k_edges && k_min_1_connected && k_not_connected );
  //z3::expr kconnectedConstraint = at_least_k_edges && k_min_1_connected && k_not_connected;

  /***** Building [[2]] ****/
  // [[2]] : V5  
  z3::expr V5 = no_self_edges();                              
    
  /***** Building [[3]] ****/
  z3::expr_vector setN = flattern2d ( nodes, N, M, false);
  z3::expr_vector setActiveN = flattern2d ( active_node, N, M, false );
  z3::expr_vector setPresentE = flattern4d ( presence_edge, N, N, E_arity, M, true );
  z3::expr_vector setActiveE = flattern4d ( active_edge, N, N, E_arity, M, true );  
  // Pairing constraint ensures (i,i) pair not allowed. ToDo: Check the effect of this. 
  z3::expr_vector setPairingM = flattern2d ( pairing_m, M, M, true );
  z3::expr_vector setReach = flattern4d ( reach, N, N, M, N-1, true );

  /*
  z3::expr_vector qvarQbf( ctx ); 

  qvarQbf.reserve( setN.size() + setActiveN.size() + setPresentE.size() + setActiveE.size() + setPairingM.size() + setReach.size() ); // preallocate memory
  
  qvarQbf.insert( qvarQbf.end(), setN.begin(), setN.end() );
  qvarQbf.insert( qvarQbf.end(), setActiveN.begin(), setActiveN.end() );
  qvarQbf.insert( qvarQbf.end(), setPresentE.begin(), setPresentE.end() );
  qvarQbf.insert( qvarQbf.end(), setActiveE.begin(), setActiveE.end() );
  qvarQbf.insert( qvarQbf.end(), setPairingM.begin(), setPairingM.end() );
  qvarQbf.insert( qvarQbf.end(), setReach.begin(), setReach.end() );
  
  */
  //for (const auto&& vect : setN ) {
  //  std::cout << vect << "\n";
  //}
  
  // Basic Constraints with stability excluding V5 :: self edge
  z3::expr basicConstraintsWithStab = get_basic_constraints();

  // Not a function constraint.
  z3::expr notaFunction = not_a_function( nodes, active_node );

  // No function possible constraint [[3]]
  // FORALL [ qvars, basicConst => notafunc ]
  z3::expr noFunctionPossible = forall ( setN, (forall (setActiveN, ( forall (setPresentE, (forall ( setActiveE, (forall (setPairingM, (forall ( setReach , implies ( basicConstraintsWithStab, notaFunction ) )))))))))) );

  /* Final Qbf Constraint: [[1]] && [[2]] && [[3]] */
  z3::expr qbfCons = kconnectedConstraint && V5 && notaFunction;   
  //z3::expr qbfCons = exists ( setE, kconnectedConstraint && V5 && notaFunction );   
  
  // std::cout << "Expected first level quant: " << set_edges << "\n";
  //std::cout << cons << "\n";
  
  // N-CNF function 
  // z3::expr setSvar = flattern2 ( s_var, N, M, false );
  // z3::expr cnfCons = at_least_three ( setSvar, setSvar.size() ); 
  // z3::expr qbfCnfCons  = forall ( setN, (forall (setActiveN, ( forall (setPresentE, (forall ( setActiveE, (forall (setPairingM, (forall ( setReach,  basicConstraintsWithStab && qbfCnfCos  )))))))))) );
  
  //return kconnectedConstraint;
  return qbfCons;

}

z3::model vts::get_vts_for_qbf( z3::expr cons) {

  // Print the input formula.
  // std::cout << cons << "\n";
  z3::solver s(ctx);

  // making sure model based quantifier instantiation is enabled.
  z3::params p(ctx);
  p.set("mbqi", true);
  s.set(p);

  s.add( cons );
  
 if( s.check() == z3::sat ) {
   z3::model m = s.get_model();
   //std::cout << m << "\n";
   std::cout << s.get_model() << "\n";
   return m;
  }else{
    std::cout << "model is not feasible!";
    assert(false);
 }
  return s.get_model();
}
