#include<vts.h>
#include<z3-util.h>
#include<algorithm>
#include<vector>
/** Synthesis Variations 
 * 1. Add edge to achieve graph stability and k connected. 
 * 2. Add flow of molecules to fix fusion. 
 *   - use available molecules.
 *   - allow other type of molecule.
 * 3. KCnf
 *   - Low depth circuit
 * 4. Gates: And Or  
 *   - Function dependence with var occuring once
 * 5. VTS repair.
 * 6. Biological Operations
 *  - Activate
 *  - Deactivate
 */


z3::expr vts::annotate_mukund_graph ( z3::expr_vector& fixN, z3::expr_vector& fixActiveN, 
                                      z3::expr_vector& fixE, z3::expr_vector& fixPresenceE,
                                      z3::expr_vector& fixActiveE, z3::expr_vector& fixPairingP )  {

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

  /*
  // Straighten out few things.
  z3::expr_vector setE = edge_set();
  z3::expr_vector setN = node_set();
  z3::expr_vector setActiveN = active_node_set();
  z3::expr_vector setPresenceE = presence_edge_set();
  z3::expr_vector setActiveE = active_edge_set();
  z3::expr_vector setPairingM = pairing_m_set();
  z3::expr_vector setReach = reach_set();
  */

  // Fix nodes of the graph

  auto n0 = nodes[0][0];
  auto n1 = nodes[0][8];

  auto n2 = nodes[1][3];
  auto n3 = nodes[1][4];
  auto n4 = nodes[1][6];
  
  auto n5 = nodes[2][1];
  auto n6 = nodes[2][2];
  auto n7 = nodes[2][7];
  auto n8 = nodes[2][9];

  fixN.push_back(n0);
  fixN.push_back(n1);
  fixN.push_back(n2);
  fixN.push_back(n3);
  fixN.push_back(n4);
  fixN.push_back(n5);
  fixN.push_back(n6);
  fixN.push_back(n7);
  fixN.push_back(n8);

  z3::expr nodes_cons = z3::mk_and( fixN ); 

  // Fix edges of the graph 
  auto e0 = edges[0][1][0]; 
  auto e1 = edges[1][2][0]; 
  auto e2 = edges[2][0][0];
  auto e3 = edges[0][2][0];
  
  fixE.push_back(e0);
  fixE.push_back(e1);
  fixE.push_back(e2);
  fixE.push_back(e3);

  z3::expr edge_cons = z3::mk_and ( fixE );

  // Fix edge labels of the graph 
  e0 = presence_edge[0][1][0][2];
  e1 = presence_edge[0][1][0][9];   
  e2 = presence_edge[0][1][0][12];    
  e3 = presence_edge[0][1][0][13];    

  auto e4 = presence_edge[0][2][0][11];    

  auto e5 = presence_edge[1][2][0][9];    
  
  auto e6 = presence_edge[2][0][0][0];
  auto e7 = presence_edge[2][0][0][2];
  auto e8 = presence_edge[2][0][0][9];   
  auto e9 = presence_edge[2][0][0][10];    
  auto e10 = presence_edge[2][0][0][12];    
  
  fixPresenceE.push_back(e0);
  fixPresenceE.push_back(e1);
  fixPresenceE.push_back(e2);
  fixPresenceE.push_back(e3);
  fixPresenceE.push_back(e4);
  fixPresenceE.push_back(e5);
  fixPresenceE.push_back(e6);
  fixPresenceE.push_back(e7);
  fixPresenceE.push_back(e8);
  fixPresenceE.push_back(e9);
  fixPresenceE.push_back(e10);


  auto edgel_cons = z3::mk_and ( fixPresenceE );

  auto cons = nodes_cons && edge_cons && edgel_cons;
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


void unassigned_bits ( z3::expr_vector& setZ,
                       z3::expr_vector fixZ,
                       z3::expr_vector& openZ ) {
  for ( unsigned i = 0; i < setZ.size(); i++ ) {
   // auto var = setZ[i];
   /*
    if ( std::find( fixZ.begin(), fixZ.end(), var ) != fixZ.end() ) 
      continue;
    else 
      openZ.push_back( var );
  }
  */
} }


z3::expr vts::vts_synthesis ( unsigned variation ) {
  /** Basic Constraints **/
  z3::expr vtsCons = create_vts_constraint();  
  z3::expr vtsActivity = vts_activity_constraint();
  
  z3::expr_vector fixN( ctx );
  z3::expr_vector fixActiveN( ctx );
  z3::expr_vector fixE( ctx );
  z3::expr_vector fixPresenceE ( ctx );
  z3::expr_vector fixActiveE( ctx );
  z3::expr_vector fixPairingP( ctx );
  
  z3::expr_vector openN( ctx );
  z3::expr_vector openActiveN( ctx );
  z3::expr_vector openE( ctx );
  z3::expr_vector openPresenceE( ctx );
  z3::expr_vector openActiveE( ctx );
  
  /** Annotate graph : fix graph input variables **/
  //z3::expr inputCons =  annotate_plos_graph ();
  z3::expr inputCons =  annotate_mukund_graph ( fixN, fixActiveN, fixE, fixPresenceE, fixActiveE, fixPairingP );

  z3::expr kConnCons = k_connected_graph_constraint( 3, false ); 
  z3::expr V5 = no_self_edges();

  z3::expr_vector setN = node_set();
  z3::expr_vector setActiveN = active_node_set();
  z3::expr_vector setE = edge_set(); 
  z3::expr_vector setPresenceE = presence_edge_set();
  z3::expr_vector setActiveE = active_edge_set(); 
  z3::expr_vector setPairingM = pairing_m_set(); 
  z3::expr_vector setReach = reach_set();

  unassigned_bits ( setN, fixN, openN ); 
  unassigned_bits ( setActiveN, fixActiveN, openActiveN ); 
  unassigned_bits ( setE, fixE, openE ); 
  unassigned_bits ( setPresenceE, fixPresenceE, openPresenceE ); 
  unassigned_bits ( setActiveE, fixActiveE, openActiveE ); 

  // 1. Add edge to achieve graph stability and k connected. 
  if ( variation == 1 ) {
    
    //auto nodeC = ! at_least_two ( openN );
    //auto nodeActivityC = ! at_least_two ( openActiveE );
    auto edgeC = ! at_least_three ( openE );
    auto edgeActivityC = ! at_least_two ( openActiveE );
    auto edgePresenceC = ! at_least_two ( openPresenceE );
    
    // Fix rest of them to zero.
    auto fixVar0 = ! z3::mk_or ( openN )  && ! z3::mk_or ( openActiveN );
    
    auto addConstraints = edgeC && edgeActivityC && edgePresenceC  && fixVar0;

    auto qvtsCons = exists( setN, 
                    exists( setActiveN, 
                    exists( setPresenceE,  
                    exists( setActiveE, 
                    exists( setPairingM, 
                    exists( setReach, 
                            vtsCons && vtsActivity && inputCons && addConstraints ))))));   

    auto cons = exists( setE, 
                        qvtsCons && V5 && kConnCons ); 
    return cons;

  }

  // 2. Add flow of molecules to fix fusion. 
  else if ( variation == 2 )  {
    
    auto edgeActivityC = ! at_least_two ( openActiveE );
    auto edgePresenceC = ! at_least_two ( openPresenceE );

    auto fixVar0 = ! z3::mk_or ( openN )  && ! z3::mk_or ( openActiveN ) && !z3::mk_or ( openE );

    auto addConstraints = edgeActivityC && edgePresenceC && fixVar0;
    
    // fix rest of them to 0.
    z3::expr qvtsCons = exists( setN, 
                        exists( setActiveN, 
                        exists( setActiveE, 
                        exists( setPairingM, 
                        exists( setReach, 
                                vtsCons && vtsActivity && addConstraints )))));

    auto cons = exists( setPresenceE, 
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
                         exists( setPresenceE, 
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
    unsigned gateTypes = 2;
    //Populate xtra var s_var : var for node function 
    // [Molecules, Total M molecule to pick, Picking options: All molecules + True + False ]
    popl3 ( s_var, M, M, (2 * M) + 2, "s" );
    // Populate xtra var t_var : var for edge function 
    popl3 ( t_var, M, M, (2 * M) + 2, "t" );
    // Populate parameter var [ Molecules , No.Of gate to pick ]
    popl3 ( u_var, M, M-1, gateTypes, "u" );
    // Populate parameter var
    popl3 ( v_var, M, M-1, gateTypes, "v" );

    // Boolean gates  function 

    auto setSvar = flattern3d ( s_var, M, M, 2*M + 2, false );
    auto setTvar = flattern3d ( t_var, M, M, 2*M + 2, false );
    auto setUvar = flattern3d ( u_var, M, M-1, gateTypes, false );
    auto setVvar = flattern3d ( v_var, M, M-1, gateTypes, false ); 
    
    z3::expr gateCons = logic_gates ( s_var, t_var, u_var, v_var );
    
    z3::expr funcGate  = exists( setN, 
                         exists( setActiveN, 
                         exists( setPresenceE, 
                         exists( setActiveE, 
                         exists( setPairingM, 
                         exists( setReach, 
                                 gateCons && vtsCons ))))));

    z3::expr cons = exists( setSvar, 
                    exists( setTvar, 
                    exists( setUvar, 
                    exists( setVvar, 
                    exists( setE, 
                            kConnCons && V5 && inputCons && funcGate )))));

    return cons;

  } else if ( variation == 5 ) {
    // Add deletion logic
    /*
// Populate edges: e(i,j,q)
  popl3( Xedges, N, N, E_arity, "xz" );

// Populate nodes: n(i,j)
  popl2 ( Xnodes, N, M, "xn" );

// Populate active_node (i, k)
  popl2 ( Xactive_node, N, M, "xa" );

// Populate presence_edge(i,j,q,k)
  popl4 ( Xpresence_edge, N, N, E_arity, M, "xe" );

// Populate active_edge(i,j,q,k)
  popl4 ( Xactive_edge, N, N, E_arity, M, "xb" );

// Populate pairing_m(k,k1)
  popl2 ( Xpairing_m, M, M, "xp" );
  */
    auto nodeC = ! at_least_three ( openN );
    auto nodeActivityC = ! at_least_two ( openActiveE );     
    auto edgeC = ! at_least_three ( openE );
    auto edgeActivityC = ! at_least_two ( openActiveE );
    auto edgePresenceC = ! at_least_two ( openPresenceE );
    
    // Fix rest of them to zero.
    auto fixVar0 = ! z3::mk_or ( openN )  && ! z3::mk_or ( openActiveN );
    
    auto addConstraints = edgeC && edgeActivityC && edgePresenceC  && fixVar0;

    auto qvtsCons = exists( setN, 
                    exists( setActiveN, 
                    exists( setPresenceE,  
                    exists( setActiveE, 
                    exists( setPairingM, 
                    exists( setReach, 
                            vtsCons && vtsActivity && inputCons && addConstraints ))))));   

    auto cons = exists( setE, 
                        qvtsCons && V5 && kConnCons ); 
    return cons;
    
  } else {
    return ctx.bool_val(true);
  }
}

