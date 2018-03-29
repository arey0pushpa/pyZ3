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
*/
/*
// Implement the same using ASt
z3::expr create_ast () {
  return ctx.bool_val(true);
}
*/

// The expression can take any gates.
z3::expr vts::gates( Vec2Expr u, z3::expr x, z3::expr y, unsigned k, unsigned k2 ) {
  z3::expr_vector coeff_list ( ctx );
  //VecExpr ls;
  auto g1 = x && y;  
  auto g2 = x || y;
  //auto g3 = ctx.bool_val(true);
  //auto g4 = x;
  //auto g5 = y;
  z3::expr_vector ls(ctx);
  ls.push_back( (u[k][k2] && g1)  ||  ( u[k][k2] && g2 ) );
  coeff_list.push_back (u[k][k2]) ;
  coeff_list.push_back (u[k][k2]) ;
  auto coeffSum = at_least_one ( coeff_list, coeff_list.size() ) && !at_least_two ( coeff_list, coeff_list.size() ); 
  auto cons = coeffSum && mk_or( ls );
  return cons;
}

z3::expr vts::gate_fml (Vec3Expr s, unsigned i, unsigned k, unsigned k2, bool e, unsigned j = 0, unsigned q = 0 ) {
  z3::expr_vector ls( ctx );
  z3::expr_vector il_list( ctx );
  //z3::expr_vector varList( ctx );
  for ( unsigned k1 = 0; k1 < M; k1++ ) {
    if ( k1 == k )  continue;
     if ( e == true )  
       ls.push_back( ( s[k][k2][k1] && presence_edge [i][j][q][k1] ) || ( s[k][k2][k1+M] && !presence_edge[i][j][q][k1+M]) );
    else  
       ls.push_back( ( s[k][k2][k1] && nodes[i][k1] )  || ( s[k][k2][k1+M] && !nodes[i][k1] ) ); 
    il_list.push_back( s[k][k2][k1] );
    il_list.push_back ( s[k][k2][k1+M] );
  }
  ls.push_back( s[k][k2][2*M] && true );
  ls.push_back( s[k][k2][(2*M) + 1] && false );
  il_list.push_back( s[k][k2][2*M] );
  il_list.push_back( s[k][k2][(2*M) + 1] );
  auto varList = mk_or( ls );
  auto coeffSum = at_least_one ( il_list, il_list.size() ) && !at_least_two ( il_list, il_list.size() ); 
  auto cons = varList && coeffSum;
  return cons;
}

z3::expr vts::process_fml ( Vec3Expr s, Vec2Expr u, unsigned i, unsigned k, bool e, unsigned j = 0, unsigned q = 0 ) {
  z3::expr_vector ls( ctx );
  z3::expr gfml(ctx);
  if ( e == true ) {
    for ( unsigned k2 = 0; k2 < M-2; k2++ ) {
      if (k2 == 0 ) {
        auto arg1 =  gate_fml( s, i, k, k2, e, j, q );
        auto arg2 = gate_fml( s, i, k, k2+1, e, j, q ); 
        gfml = gates( u, arg1, arg2, k, k2 ); 
      }
      else { 
        auto arg2 = gate_fml( s, i, k, k2+1, e, j, q ); 
        gfml = gates( u, gfml, arg2, k, k2 );
      }
    }
  }
  else {
    for ( unsigned k2 = 0; k2 < M-2; k2++ ) {
      if (k2 == 0) {
        auto arg1 = gate_fml( s, i, k, k2, e ); 
        auto arg2 = gate_fml( s, i, k, k2+1, e );
        gfml = gates ( u, arg1, arg2, k, k2 );
      }
      else { 
        auto arg2 = gate_fml( s, i, k, k2+1, e );
        gfml = gates( u, gfml, arg2, k, k2 );
      }
    }
  }
  auto cons = mk_and ( ls );
  return cons;
}

z3::expr vts::node_gate_fml ( Vec3Expr s, Vec2Expr u ) {
  z3::expr_vector main_list(ctx);
  for( unsigned i = 0; i < N; i++ ) {
    for( unsigned k = 0; k < M; k++ ) {
      auto fml = ( active_node[i][k] == process_fml( s, u, i, k, false ) );
      main_list.push_back ( fml );
    }
  }
  auto cons = mk_and( main_list );
  return cons;
}

z3::expr vts::edge_gate_fml ( Vec3Expr t, Vec2Expr v ) {
  z3::expr_vector main_list(ctx);
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if ( i == j )  continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        for ( unsigned k = 0; k < M; k++ ) {
          z3::expr_vector outer_list(ctx);
          auto fml =  ( active_edge[i][j][q][k] == process_fml ( t, v, i, k, true, j, q ) ) ;
          main_list.push_back ( fml );
        }
      }
    }
  }
  auto cons = mk_and ( main_list );
  return cons;
}

z3::expr vts::logic_gates ( Vec3Expr s_var, Vec3Expr t_var, Vec2Expr u_var, Vec2Expr v_var ) {
  z3::expr nodeGate = node_gate_fml ( s_var, u_var );
  z3::expr edgeGate = edge_gate_fml ( t_var, v_var );
  //std::cout << edgeGate << "\n";
  auto cons = nodeGate && edgeGate;
  return cons;
}

