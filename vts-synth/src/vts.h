#ifndef VTS_H
#define VTS_H

#include <string>
#include <z3++.h>
#include <z3-util.h>

enum model_version {
  MODEL_1,
  MODEL_2,
  MODEL_3,
  MODEL_4,
  MODEL_5,
  MODEL_6
};


class vts{
public:
  vts( z3::context& _ctx,
       unsigned _M, unsigned _N, unsigned _Q, model_version _V, unsigned _C )
    : ctx(_ctx), M(_M), N(_N), E_arity(_Q),
      V(_V), C(_C)
  {
    init_vts();
  }

private:
  z3::context& ctx;

  unsigned   M;    // Molecules
  unsigned   N;    // Nodes
  unsigned   E_arity;    // Edge arity
  model_version V;  // Variation
  unsigned   C;    // connectedness

  void init_vts();

  //variables
  Vec3Expr edges;
  Vec2Expr nodes;
  Vec2Expr active_node;
  Vec4Expr presence_edge;
  Vec4Expr active_edge;

  Vec2Expr pairing_m;
  VecExpr node_funcs;
  VecExpr edge_funcs;

  Vec4Expr reach;
  Vec3Expr drop1;
  Vec2Expr d_reach;

  //flat version of variables
  VecExpr flat_edges;


  // Vec3Expr dump2;

  //constraints

  void popl1( VecExpr&, unsigned,  std::string&);
  void popl2( Vec2Expr&, unsigned, unsigned, std::string&);
  void popl3( Vec3Expr&, unsigned, unsigned, unsigned, std::string&);
  void popl4( Vec4Expr&, unsigned, unsigned, unsigned, unsigned, std::string&);


  //formula makers
  z3::expr always_active_on_node(); // f_nn
  z3::expr always_active_on_edge(); // f_ne
  z3::expr pm_dependent_activity(); //f_se

  // activity chosing mode
// Activity_node = True 
// Activity_edge = True
// if V == 1:
//     Activity_node = True 
//     Activity_edge = True
// elif V == 2:
//     Activity_node = f_nn()
// elif V == 3:
//     Activity_edge = f_se()
// elif V == 4:
//     Activity_node = f_nn()
//     Activity_edge = f_se()
// elif V == 5:
//     Activity_edge = f_ne()
// else:
//     Activity_node = f_nn()
//     Activity_edge = f_ne()

  z3::expr molecule_presence_require_for_present_edge(); //V1
  z3::expr active_molecule_is_present_on_edge();         //V2
  z3::expr active_molecule_is_present_on_node();         //V3
  z3::expr edge_modelecues_is_subset_of_node_molecules();//V3
  z3::expr no_self_edges();                              //V5
  z3::expr restriction_on_pairing_matrix();              //V6
  z3::expr edge_must_fuse_with_target();                 //V7
  z3::expr edge_must_not_fuse_with_noone_else();         //V8

  //study state
  z3::expr reachability_def();           //R1
  z3::expr study_state_stability_cond(); //R2

  //
  // connectivity constraints
  // todo: variables are needed to be parametrized
  z3::expr only_present_edges_can_be_dropped(); //
  z3::expr atleast_k_drops(unsigned k);         //
  z3::expr atmost_k_drops(unsigned k);          //
  z3::expr exactly_k_drops(unsigned k);         //
  z3::expr reachability_under_drop_def();       //
  z3::expr remains_connected();                 //
  z3::expr gets_disconnected();                 //

};

#endif
