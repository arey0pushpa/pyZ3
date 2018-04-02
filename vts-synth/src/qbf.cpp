#include<vts.h>
#include<z3-util.h>

//#include <vector>
//#include <iterator>

z3::expr vts::create_qbf_formula ( int funcType ) {

  /** Build Constrint Of the Form := [[1]] && [[2]] && [[3]]
   * [[1]] :: Connectivity Constraint : kConnected Graph
   * [[2]] :: V5 Constraint : Self edge not allowed
   * [[3]] :: Function Requirements :
   *          - No Boolean func exists.
   *          - Function structure: 3 CNF. 
   */

  /***** Building [[1]] ****/
  VecExpr ee_set = flattern_3d ( edges );
  z3::expr_vector setE = edge_set();

  // Arg true -> u need edge quantified outside.
  z3::expr kconnectedConstraint = k_connected_graph_constraint ( C, false );

  /***** Building [[2]] ****/
  // [[2]] : V5  
  z3::expr V5 = no_self_edges();                              
  
  /***** Building [[3]] ****/
  z3::expr_vector setN = node_set();
  z3::expr_vector setActiveN = active_node_set(); 
  z3::expr_vector setPresentE = presence_edge_set();
  z3::expr_vector setActiveE = active_edge_set();
  z3::expr_vector setPairingM = pairing_m_set();
  z3::expr_vector setReach = reach_set();

  /* Avoid writing forall( x, forall( y, .... ))
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
  z3::expr vtsFusion = ! ( vts_fusion_constraint() ); 

  // [3]: Not a function constraint.
  z3::expr notaFunction = not_a_function( nodes, active_node );
  
  /* Final Qbf Constraint: [[1]] && [[2]] && [[3]] */
  if ( funcType == 1 ) {
    
    // Populate xtra var s_var : var for node function
    popl3 ( s_var, M, 2 * M, D, "s" );
    
    // Populate xtra var t_var : var for node function 
    popl3 ( t_var, M, 2 * M, D, "t" );
  
    // [3]: N-CNF function 
    auto setSvar = flattern3d ( s_var, M, 2*M, D, false );
    auto setTvar = flattern3d ( t_var, M, 2*M, D, false );
  
    z3::expr cnfCons = cnf_function( s_var, t_var );

    z3::expr func3cnf  = forall( setN, 
                         forall( setActiveN, 
                         forall( setPresentE, 
                         forall( setActiveE, 
                         forall( setPairingM, 
                         forall( setReach, 
                         forall( setSvar, 
                         forall( setTvar,  
                                 implies( vtsBasicStability && cnfCons, vtsFusion )))))))));

    z3::expr qbfCons = exists( setE, 
                               kconnectedConstraint && V5 && func3cnf );  

   // z3::expr qbfCons = exists ( setTvar, exists (setSvar, (exists (setE, ( V5 && func3cnf )))) );  
   
    return qbfCons;

  }
  else if ( funcType == 2 ) {
    
    // Populate xtra var s_var : var for node function
    popl3 ( s_var, M, M - 1, (2 * M) + 2, "s" );
   
    // Populate xtra var t_var : var for node function 
    popl3 ( t_var, M, M, (2 * M) + 2, "t" );
    
    // Populate parameter var
    popl2 ( u_var, M, M, "u" );
    
    // Populate parameter var
    popl2 ( v_var, M, M, "v" );
    
    // [3]: Boolean gates  function 
    auto setSvar = flattern3d ( s_var, M, M - 1, 2*M + 2, false );
    auto setTvar = flattern3d ( t_var, M, M - 1, 2*M + 2, false );
    auto setUvar = flattern2d ( u_var, M, M, false );
    auto setVvar = flattern2d ( v_var, M, M, false );
  
    z3::expr gateCons = logic_gates ( s_var, t_var, u_var, v_var );
    
    z3::expr funcGate  = forall( setN, 
                         forall( setActiveN, 
                         forall( setPresentE, 
                         forall( setActiveE, 
                         forall( setPairingM, 
                         forall( setReach, 
                         forall( setSvar, 
                         forall( setTvar,  
                         forall( setUvar, 
                         forall( setVvar, 
                                 implies ( vtsBasicStability && gateCons, vtsFusion )))))))))));

    z3::expr cons = exists( setE, 
                            kconnectedConstraint && V5 && funcGate );  

    return cons;

  }else {
    
    // No function possible constraint [[3]]
    // FORALL [ qvars, basicConst => notafunc ]
    z3::expr noFunctionPossible = forall( setN, 
                                  forall( setActiveN, 
                                  forall( setPresentE, 
                                  forall( setActiveE, 
                                  forall( setPairingM, 
                                  forall( setReach , 
                                          implies( vtsBasicStability && vtsFusion, notaFunction )))))));

    z3::expr qbfCons = exists( setE, 
                               kconnectedConstraint && V5 && noFunctionPossible );   
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
