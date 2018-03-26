#include<vts.h>
#include<z3-util.h>
//#include <vector>
//#include <iterator>

z3::expr vts::create_qbf_formula ( bool flagC ) {

  /** Build Constrint Of the Form := [[1]] && [[2]] && [[3]]
   * [[1]] :: Connectivity Constraint : kConnected Graph
   * [[2]] :: V5 Constraint : Self edge not allowed
   * [[3]] :: Function Requirements :
   *          - No Boolean func exists.
   *          - Function structure: 3 CNF. 
   */

  /***** Building [[1]] ****/
  VecExpr ee_set = flattern_3d ( edges );
  //edgeQuant = ee_set;

  z3::expr_vector setR1 = flattern2d ( d_reach1, N, N, true );
  z3::expr_vector setR2 = flattern2d ( d_reach2, N, N, true );
  
  // Create:  Exists (setR1, reach_d1 && d1_all-conn )
  z3::expr is_reach1 = exists( setR1, reachability_under_drop_def( d_reach1 , drop1, 0 ) && remains_connected( d_reach1 )  ); 
  z3::expr is_reach2 = exists( setR2, reachability_under_drop_def( d_reach2 , drop2, 1 ) && gets_disconnected( d_reach2 )  );
  
  z3::expr_vector setD1 = flattern3d ( drop1, N, N, E_arity, true );
  z3::expr_vector setD2 = flattern3d ( drop2, N, N, E_arity, true );
  
  z3::expr k_min_1_connected = forall ( setD1, implies 
		  (  (exactly_k_drops ( C-1, drop1 ) && only_present_edges_can_be_dropped ( drop1 )), is_reach1 ) );
 
  z3::expr k_not_connected = exists ( setD2,  
		    (exactly_k_drops ( C, drop2 ) && only_present_edges_can_be_dropped ( drop2 )) && is_reach2 ) ;
 
  z3::expr at_least_k_edges = at_least_three( ee_set, ee_set.size() );
  
  // [[1]] : K Connected Graph 
  // EXISTS [ setE, kedges && k-1Conn && knotConn ]
  z3::expr_vector setE = flattern3d ( edges, N, N, E_arity, true );
  //z3::expr kconnectedConstraint = exists ( setE, at_least_k_edges && k_min_1_connected && k_not_connected );
  z3::expr kconnectedConstraint = at_least_k_edges && k_min_1_connected && k_not_connected;

  /***** Building [[2]] ****/
  // [[2]] : V5  
  z3::expr V5 = no_self_edges();                              

  /*** Return K connected Graph **
  z3::expr kconnectedConstraint = exists ( setE, at_least_k_edges && k_min_1_connected && k_not_connected && V5 );
  return kconnectedConstraint; 
  */
    
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
  
  // Basic constraints with stability excluding V5 :: self edge
  z3::expr vtsBasicStability = vts_basic_constraints() && vts_stability_constraint();
  z3::expr vtsFusion = vts_fusion_constraint(); 

  // [3]: Not a function constraint.
  z3::expr notaFunction = not_a_function( nodes, active_node );
  
  /* Final Qbf Constraint: [[1]] && [[2]] && [[3]] */
  if ( flagC == true ) {
    // Populate xtra var s_var : var for node function
    popl3 ( s_var, M, 2 * M, D, "s" );
    // Populate xtra var t_var : var for node function 
    popl3 ( t_var, M, 2 * M, D, "t" );
  
    // [3]: N-CNF function 
    auto setSvar = flattern3d ( s_var, M, 2*M, D, false );
    auto setTvar = flattern3d ( t_var, M, 2*M, D, false );
  
    z3::expr cnfCons = cnf_function( s_var, t_var );

    z3::expr func3cnf  = forall ( setN, (forall (setActiveN, ( forall (setPresentE, (forall ( setActiveE, (forall (setPairingM, (forall ( setReach, implies ( vtsBasicStability && cnfCons, vtsFusion ))))))))))) );

    z3::expr qbfCons = exists ( setTvar, exists (setSvar, (exists (setE, kconnectedConstraint && V5 && func3cnf )))  );  
   // z3::expr qbfCons = exists ( setTvar, exists (setSvar, (exists (setE, ( V5 && func3cnf )))) );  
    return qbfCons;
  }
  else {
    // No function possible constraint [[3]]
    // FORALL [ qvars, basicConst => notafunc ]
    z3::expr noFunctionPossible = forall ( setN, (forall (setActiveN, ( forall (setPresentE, (forall ( setActiveE, (forall (setPairingM, (forall ( setReach , implies ( vtsBasicStability &&  vtsFusion, notaFunction ) )))))))))) );

    z3::expr qbfCons = exists ( setE, kconnectedConstraint && V5 && noFunctionPossible );   
    return qbfCons;
  } 
  
  /*** Print the set of built edges and the formula ***/
  // std::cout << "Expected first level quant: " << set_edges << "\n";
  //std::cout << cons << "\n";
  //return kconnectedConstraint;
  
  //return qbfCons;

}

// Z3 home made QBf solver and return model
void vts::use_z3_qbf_solver ( z3::expr cons ) {

  // Print the input formula.
  // std::cout << cons << "\n";
  
  z3::solver s(ctx);

  // making sure model based quantifier instantiation is enabled.
  z3::params p(ctx);
  p.set("mbqi", true);
  s.set(p);

  s.add( cons );
  
 if( s.check() == z3::sat ) {
   std::cout << "The formula is SAT \n";
   z3::model m = s.get_model();
   //std::cout << m << "\n";
   std::cout << m << "\n";
   //return m;
  }else{
    std::cout << "model is not feasible!";
    assert(false);
 }
  //return s.get_model();
}
