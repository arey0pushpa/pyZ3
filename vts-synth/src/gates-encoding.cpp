#include<vts.h>
#include<z3-util.h>

/** Boolean Gates Implementation ***
 * L := a G L | a          a : Bool_var   G : gate
 * G == AND | OR 
 */

/*
enum ASTNodeType 
{
  Undefined,
  OpTrue,
  OpAnd,
  OpOr,
  OpFirst,
  OpSecond,
  BoolVal
};

class ASTNode
{
  public:
    ASTNodeType Type;
    bool      Value;
    ASTNode*    Left;
    ASTNode*    Right;
    ASTNode()
    {
      Type = Undefined;
      Value = 0;
      Left = NULL;
      Right = NULL;
    }
    
    ~ASTNode()
    {
      delete Left;
      delete Right;
    }
}

// The expression can take any gates.
z3::expr gates( unsigned x, unsigned y ) {
  //VecExpr ls;
  auto g1 = x && y;  
  auto g2 = x || y;
  auto g3 = ctx.bool_val(true);
  auto g4 = x;
  auto g5 = y;
  z3::expr_vector ls(ctx) = { g1, g2, g3, g4, g5 };
  auto cons = exactly_one ( ls );
  return cons;
}

z3::expr process (z3::expr_vector) {
  return ctx.bool_val(true);
}

// Implement the same using ASt
z3::expr create_ast () {
  return ctx.bool_val(true);

}
*/

z3::expr vts::gate_fml (unsigned i, unsigned k, bool e, unsigned j = 0, unsigned q = 0) {
  z3::expr_vector ls( ctx );
  if ( e == true ) {
    for ( unsigned k1 = 0; k1 < M; k1++ ) {
      if ( k1 == k )  continue;
      ls.push_back ( nodes[i][k1] );
    }
    //create_ast ( ls );
  }else {
    for ( unsigned k1 = 0; k1 < M; k1++ ) {
      if ( k1 == k )  continue;
      ls.push_back ( presence_edge [i][j][q][k1] );
    }
    //create_ast ( ls );
  }
  // todo: fake return
  return z3::mk_and (ls); 
}

z3::expr vts::node_gate_fml ( ) {
  z3::expr_vector main_list(ctx);
  for ( unsigned i = 0; i < N; i++ ) {
    for ( unsigned k = 0; k < M; k++ ) {
      auto fml = (active_node[i][k] == gate_fml ( i, k, false ) );
      main_list.push_back ( fml );
    }
  }
  auto cons = mk_and ( main_list );
  return cons;
}

z3::expr vts::edge_gate_fml ( ) {
  z3::expr_vector main_list(ctx);
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if ( i == j )  continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        for ( unsigned k = 0; k < M; k++ ) {
          z3::expr_vector outer_list(ctx);
          auto fml =  ( active_edge[i][j][q][k] == gate_fml ( i, k, true, j, q ) ) ;
          main_list.push_back ( fml );
        }
      }
    }
  }
  auto cons = mk_and ( main_list );
  return cons;
}

z3::expr vts::logic_gates ( ) {
  z3::expr nodeGate = node_gate_fml ( );
  z3::expr edgeGate = edge_gate_fml ( );
  std::cout << edgeGate << "\n";
  auto cons = nodeGate && edgeGate;
  return cons;
}

