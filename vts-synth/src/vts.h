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
};

#endif
