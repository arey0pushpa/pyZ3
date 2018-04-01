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

void vts::annotate_graph () {
  /** Example taken from PLOS paper
   * N = 2, M = 6 
   * model 3: Arb on edge, Nothing on node
   */

  /** node: n[i][k] : below.  a[i][k] : use Model_3 **/
  // Node 0 : [111 110]  
  auto node_cons = !nodes[0][0] && nodes[0][1] && nodes[0][2] && nodes[0][3] && nodes[0][4] && nodes[0][5]; 

  /** edge: e[i][j][q], e[i][j][q][k]: below, b[i][j][q][k]: Model_3 **/
  //auto edge_cons1 = edges[0][1][0] && edges[1][0][0] && edges[1][0][1];
  auto edge_cons1 = edges[0][1][0] && edges[1][0][0];
  auto edge_cons2_1 = presence_edge[0][1][0][2] && presence_edge[0][1][0][3] && presence_edge[0][1][0][5]; 
  auto edge_cons2_2 = presence_edge[1][0][0][2] && presence_edge[1][0][0][3];
  auto edge_cons = edge_cons1 && edge_cons2_1 && edge_cons2_2;; 
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
    annotate_graph();
    auto qvtsCons = exists ( setN, exists (setActiveN, (exists (setPresentE,  (exists (setActiveE, (exists ( setPairingM, (exists (setReach , vtsCons ))))))))));   

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
