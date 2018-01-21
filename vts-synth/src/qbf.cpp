#include<vts.h>
#include<z3-util.h>


z3::model vts::get_vts_for_qbf() {

  z3::expr basic_constraints_with_stability = get_basic_constraints();
  std::cout << "mama mia .. let me go..."; 
  exit(0);
  //std::cout << basic_constraints_with_stability;

  z3::solver s(ctx);
  s.add(basic_constraints_with_stability);
  return s.get_model();

  // Create:  Exists (setR1, reach_d1 && d1_all-conn )
  //VecExpr d1 = flattern_2d ( d_reach );

  //z3::expr is_reach = exists( d1, only_present_edges_can_be_dropped (drop1) && reachability_under_drop_def( d_reach , drop1, 0 ) && remains_connected( d_reach )  );

  //z3::expr k_1_connected = forall ( flattern_3d( drop1 ), z3::implies ( exactly_k_drops ( C-1, drop1 ), is_reach ) );

}
