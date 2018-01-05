#ifndef VTS_H
#define VTS_H

#include <z3++.h>
#include <z3-util.h>

enum model_type {
  MODEL_1,
  MODEL_2
};


class vts{
public:
  vts( z3::context& _ctx,
       unsigned _M, unsigned _N, unsigned _Q, model_type _V, unsigned _C )
    : ctx(_ctx), mols(_M), nodes(_N), e_arity(_Q), mod(_V), conn(_C)
  {
    init_vts();
  }

private:
  z3::context& ctx;

  unsigned   mols;    // Molecules
  unsigned   nodes;    // Nodes
  unsigned   e_arity;    // Edge arity
  model_type mod;  // Variation
  unsigned   conn;    // connectedness

  void init_vts();

  //variables
  Vec3Expr edges;
  Vec3Expr dump1;
  Vec2Expr r1;
  Vec2Expr active_node;


  //flat version of variables
  VecExpr flat_edges;
  

  // Vec3Expr dump2;

  //constraints

};

#endif
