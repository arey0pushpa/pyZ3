#ifndef VTS_H
#define VTS_H

#include <string>
#include <z3++.h>
#include <z3-util.h>

enum model_type {
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
  Vec2Expr node;
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

  void filler1( Vec3Expr& m, unsigned d1 ,
                std::string& prefix) {
  }

  void filler2( Vec3Expr& m, unsigned d1 , unsigned d2,
                std::string& prefix) {
  }

  void filler3( Vec3Expr& m, unsigned d1 , unsigned d2, unsigned d3,
                std::string& prefix) {
  }

  void filler4( Vec3Expr& m, unsigned d1 , unsigned d2, unsigned d3, unsigned d4,
                std::string& prefix) {
  }

};

#endif
