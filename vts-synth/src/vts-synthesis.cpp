#include<vts.h>
#include<z3-util.h>

/** Synthesis Variations 
 * 1. Add edge to achieve graph stability and k connected. 
 * 2. Add flow of molecules to fix fusion. 
 *   - use available molecules.
 *   - allow other type of molecule.
 * 3. KCnf 
 * 4. Cnf with low depth circuit
 * 5. Gates: And Or 
 * 6. Function dependence with var occuring once
 */

void create_edges () {
}

z3::expr vts::annotate_graph () {
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

  z3::expr kConnCons = k_connected_graph_constraint( 3, false ); 
  z3::expr V5 = no_self_edges();

  z3::expr_vector setN = node_set();
  z3::expr_vector setActiveN = active_node_set();
  z3::expr_vector setPresentE = presence_edge_set();
  z3::expr_vector setActiveE = active_edge_set(); 
  z3::expr_vector setPairingM = pairing_m_set(); 
  z3::expr_vector setReach = reach_set();

  z3::expr_vector setE = edge_set(); 

  if ( variation == 1 ) {
    //create_edges ();
    z3::expr inputCons = annotate_graph();
    auto qvtsCons = exists ( setN, exists (setActiveN, (exists (setPresentE,  (exists (setActiveE, (exists ( setPairingM, (exists (setReach , vtsCons && inputCons ))))))))));   

    auto cons = exists( setE, qvtsCons && V5 && kConnCons ); 
    return cons;

  }else if ( variation == 2 )  {
    annotate_graph();
    auto qvtsCons = exists ( setN, exists (setActiveN, (exists (setActiveE, (exists ( setPairingM, (exists (setReach , vtsCons ))))))));

    auto cons = exists ( setPresentE, exists (setE, qvtsCons && V5 && kConnCons )); 

    return ctx.bool_val(true);
  }else {
    return ctx.bool_val(true);
  }
}
