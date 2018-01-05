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
    : ctx(_ctx), m(_M), n(_N), q(_Q), v(_V), c(_C)
  {
    init_vts();
  }

private:
  z3::context& ctx;

  unsigned m;    // Molecules
  unsigned n;    // Nodes
  unsigned q;    // Edge arity
  model_type v;  // Variation
  unsigned c;    // connectedness

  void init_vts();

  //variables
  Vec3Expr edges;
  VecExpr flat_edges;

  Vec3Expr dump1;
  Vec2Expr r1;

  // Vec3Expr dump2;

  //constraints

};

#endif
