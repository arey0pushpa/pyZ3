#include<vts.h>
#include<z3-util.h>

/** Synthesis Variations 
 * 1. Add edge to achieve graph stability and k connected. 
 * 2. Add flow of molecules to fix fusion. 
 *   - use available molecules.
 *   - allow other type of molecule.
 * 3. KCnf
 *   - Low depth circuit
 * 4. Gates: And Or  
 *   - Function dependence with var occuring once
 * 5. Biological Operations
 *  - Activate
 *  - Deactivate
 */

void create_edges () {

}

z3::expr vts::annotate_mukund_graph () {

  /* M = 14, Molecules subgraph of Mukunds VTS`
   * [ Qa2, Qa5, Qa7, Qa8 ] ::> [ M0, M1, M2, M3 ]
   * [ Qb8 ] ::> [ M4 ]
   * [ Qc7, Qc8 ] ::> [ M5, M6 ]
   * [ Qbc2, Qbc2/3, Qbc7 ] ::> [ M7, M8, M9 ]
   * [ R2, R3, R7, R8 ] ::> [ M10, M11, M12, R13 ]
   * 
   * Nodes: N = 3
   *        EE :: 0
   *        LE :: 1
   *        PM :: 2
   */

  // Fix nodes of the graph
  z3::expr n0_cons = nodes[0][0]
                   && nodes[0][8];

  z3::expr n1_cons = nodes[1][3]
                   && nodes[1][4]
                   && nodes[1][6];

  z3::expr n2_cons = nodes[2][1]
                   && nodes[2][2]
                   && nodes[2][7]
                   && nodes[2][9];

  auto nodes_cons = n0_cons && n1_cons && n2_cons; 

  // Fix edges of the graph 
  z3::expr e_cons = edges[0][1][0] 
                  && edges[1][2][0] 
                  && edges[2][0][0] 
                  && edges[0][2][0];

  // Fix edge labels of the graph 
  z3::expr e_label_01 = presence_edge[0][1][0][2]    
                      && presence_edge[0][1][0][9]    
                      && presence_edge[0][1][0][12]    
                      && presence_edge[0][1][0][13];    
  
  z3::expr e_label_02 = presence_edge[2][0][0][11];    

  z3::expr e_label_12 = presence_edge[1][2][0][9];    
  
  z3::expr e_label_20 = presence_edge[2][0][0][0]   
                        && presence_edge[2][0][0][2]    
                        && presence_edge[2][0][0][9]    
                        && presence_edge[2][0][0][10]    
                        && presence_edge[2][0][0][12];    


  auto edgel_cons = e_label_01 && e_label_02 && e_label_12 && e_label_20;

  auto cons = nodes_cons && e_cons && edgel_cons;
  return cons;

  /* 21 Molecules subgraph of Mukunds VTS' 
   * [ Qa2, Qa4, Qa5, Qa6, Qa7 ] ::> [ M0, M1, M2, M3, M4 ]
   * [ Qb1, Qb6 ] ::> [ M5, M6 ]
   * [ Qc4, Qc5 ] ::> [ M7, M8 ]
   * [ Qbc2, Qbc3, Qbc7 ] ::> [ M9, M10, M11 ] 
   * [ Qbc2, Qbc2/3, Qbc7 ] ::> [ M12, M13, M14 ]
   * [ R2, R3, R4, R6, R7, R8 ] ::> [ M15, M16, M17, M18, M19, M20 ]
   * */

}

z3::expr vts::annotate_plos_graph () {
  /** Example taken from PLOS paper
   * N = 2, M = 6 
   * model 3: Arb on edge, Nothing on node
   */

  /** node: n[i][k] : below.  a[i][k] : use Model_3 **/
  // Node 0 : [111 110]  
  z3::expr node_cons = !nodes[0][0] && nodes[0][1] && nodes[0][2] && nodes[0][3] && nodes[0][4] && nodes[0][5]; 

  /** edge: e[i][j][q], e[i][j][q][k]: below, b[i][j][q][k]: Model_3 **/
  //auto edge_cons1 = edges[0][1][0] && edges[1][0][0] && edges[1][0][1];
  z3::expr edge_cons1 = edges[0][1][0] && edges[1][0][0];
  z3::expr edge_cons2_1 = presence_edge[0][1][0][2] && presence_edge[0][1][0][3] && presence_edge[0][1][0][5]; 
  z3::expr edge_cons2_2 = presence_edge[1][0][0][2] && presence_edge[1][0][0][3];
  z3::expr edge_cons = edge_cons1 && edge_cons2_1 && edge_cons2_2; 

  /** Pairing Matrix Constraints **/
  z3::expr pairing_cons_1 = pairing_m[5][1] && pairing_m[3][0] && pairing_m[2][4]; 

  /*
     std::vector<z3::expr> vec;
     vec.push_back( pairing_m[2][4] ); 
     vec.push_back( pairing_m[3][0] ); 
     vec.push_back( pairing_m[5][1] );

     z3::expr pairing_cons_0 ( ctx );
     for (unsigned k = 0; k < M; k++ ) {
       for (unsigned k1 = 0; k1 < M; k1++) {
         auto expr = pairing_m[k][k1];
         if(std::find(vec.begin(), vec.end(), expr) == vec.end()) {
         pairing_cons_0 = pairing_cons_0 && expr;
       }
      }
     }

     std::cout << pairing_cons_0;
     exit(0);

     z3::expr pairing_cons = pairing_cons_1 && pairing_cons_0;
     */
  auto cons = edge_cons && node_cons && pairing_cons_1;
  return cons;
}

z3::expr vts::vts_synthesis ( unsigned variation ) {
  /** Basic Constraints **/
  z3::expr vtsCons = create_vts_constraint();  
  z3::expr vtsActivity = vts_activity_constraint();
  
  /** Annotate graph : fix graph input variables **/
  //z3::expr inputCons =  annotate_graph ();
  z3::expr inputCons =  annotate_mukund_graph ();

  z3::expr kConnCons = k_connected_graph_constraint( 3, false ); 
  z3::expr V5 = no_self_edges();

  z3::expr_vector setN = node_set();
  z3::expr_vector setActiveN = active_node_set();
  z3::expr_vector setPresentE = presence_edge_set();
  z3::expr_vector setActiveE = active_edge_set(); 
  z3::expr_vector setPairingM = pairing_m_set(); 
  z3::expr_vector setReach = reach_set();

  z3::expr_vector setE = edge_set(); 

  // 1. Add edge to achieve graph stability and k connected. 
  if ( variation == 1 ) {

    auto qvtsCons = exists( setN, 
                    exists( setActiveN, 
                    exists( setPresentE,  
                    exists( setActiveE, 
                    exists( setPairingM, 
                    exists( setReach, 
                            vtsCons && vtsActivity && inputCons ))))));   

    auto cons = exists( setE, 
                        qvtsCons && V5 && kConnCons ); 

    return cons;

  }

  // 2. Add flow of molecules to fix fusion. 
  else if ( variation == 2 )  {

    z3::expr qvtsCons = exists( setN, 
                        exists( setActiveN, 
                        exists( setActiveE, 
                        exists( setPairingM, 
                        exists( setReach, 
                                vtsCons && vtsActivity )))));

    auto cons = exists( setPresentE, 
                exists( setE, 
                        qvtsCons && V5 && inputCons && kConnCons )); 

    return cons;
  }
  // 3. KCnf  4. Cnf with low depth circuit
  else if ( variation == 3 ) {

    //Populate xtra var s_var : var for node function
    popl3 ( s_var, M, 2 * M, D, "s" );
    // Populate xtra var t_var : var for node function 
    popl3 ( t_var, M, 2 * M, D, "t" );
    // [3]: N-CNF function 
    auto setSvar = flattern3d ( s_var, M, 2*M, D, false );
    auto setTvar = flattern3d ( t_var, M, 2*M, D, false );

    z3::expr cnfCons = cnf_function( s_var, t_var );

    z3::expr func3cnf  = exists( setN, 
                         exists( setActiveN, 
                         exists( setPresentE, 
                         exists( setActiveE, 
                         exists( setPairingM, 
                         exists( setReach, 
                                 cnfCons && vtsCons )))))) ;

    z3::expr cons = exists( setSvar, 
                    exists( setTvar, 
                    exists( setE, 
                            kConnCons && V5 && inputCons && func3cnf )));

    return cons;
  }
  else if ( variation == 4 ) {
    //Populate xtra var s_var : var for node function
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
    
    z3::expr funcGate  = exists( setN, 
                         exists( setActiveN, 
                         exists( setPresentE, 
                         exists( setActiveE, 
                         exists( setPairingM, 
                         exists( setReach, 
                                 gateCons && vtsCons ))))));

    z3::expr cons = exists( setSvar, 
                    exists( setTvar, 
                    exists( setUvar, 
                    exists( setUvar, 
                    exists( setE, 
                            kConnCons && V5 && inputCons && funcGate )))));

    return cons;

  }else {
    return ctx.bool_val(true);
  }
}
