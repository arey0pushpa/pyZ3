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
  std::vector<z3::func_decl> node_funcs;
  std::vector<z3::func_decl> edge_funcs;

  Vec4Expr reach;
  Vec3Expr drop1;
  Vec2Expr d_reach;

  //flat version of variables
  VecExpr flat_edges;


  // Vec3Expr dump2;

  //constraints

  void popl1( VecExpr&, unsigned,  std::string);
  void popl2( Vec2Expr&, unsigned, unsigned, std::string);
  void popl3( Vec3Expr&, unsigned, unsigned, unsigned, std::string);
  void popl4( Vec4Expr&, unsigned, unsigned, unsigned, unsigned, std::string);

  void make_func ( std::vector<z3::func_decl>&, std::string);


public:
  //formula makers
  z3::expr always_active_on_node(); // f_nn
  z3::expr always_active_on_edge(); // f_ne
  z3::expr pm_dependent_activity_on_edge(); //f_se
  z3::expr func_driven_activity_on_node(); //f_bn
  z3::expr func_driven_activity_on_edge(); //f_be

  // z3::expr node_activity_constraint() {
  //   switch(V) {
  //   case MODEL_1: return ctx.bool_val(true); break;
  //   case MODEL_2: return always_active_on_node(); break;
  //   case MODEL_3: return ctx.bool_val(true); break;
  //   case MODEL_4: return break;
  //   case MODEL_5: break;
  //   case MODEL_6: break;
  //   }
  // }

  // z3::expr edge_activity_constraint() {
  // }

  z3::expr molecule_presence_require_for_present_edge(); //V1
  z3::expr active_molecule_is_present_on_edge();         //V2
  z3::expr active_molecule_is_present_on_node();         //V3
  z3::expr edge_modelecues_is_subset_of_node_molecules();//V4
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


  z3::model get_vts_for_prob1();

  //helper functions
  z3::expr is_mol_edge_present( unsigned i, unsigned j, unsigned m );
  z3::expr is_qth_edge_present( unsigned i, unsigned j, unsigned q );
  void dump_dot( std::string filename, z3::model mdl);
};


#endif
