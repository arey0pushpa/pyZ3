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

z3::expr vts::vts_synthesis ( unsigned variation ) {
  if ( variation == 1 ) {
    create_edges ();

    /** Basic Constraints **/
    z3::expr vtsCons = create_vts_constraint();  

    z3::expr kConnCons = k_connected_graph_constraint( 3, false ); 
    z3::expr V5 = no_self_edges();

    /***** Building [[3]] ****/
    z3::expr_vector setN = node_set();
    z3::expr_vector setActiveN = active_node_set();
    z3::expr_vector setPresentE = presence_edge_set();
    z3::expr_vector setActiveE = active_edge_set(); 
    z3::expr_vector setPairingM = pairing_m_set(); 
    z3::expr_vector setReach = reach_set();

    z3::expr_vector setE = edge_set(); 

    auto qvtsCons = exists ( setN, exists (setActiveN, (exists (setPresentE,  (exists (setActiveE, (exists ( setPairingM, (exists (setReach , vtsCons ))))))))));   

    auto cons = exists( setE, qvtsCons && V5 && kConnCons ); 
    return cons;

  }else if ( variation == 2 )  {
    return ctx.bool_val(true);
  }else {
    return ctx.bool_val(true);
  }
}
