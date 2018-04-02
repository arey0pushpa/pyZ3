#include<vts.h>
#include<z3-util.h>

/** Boolean Gates Implementation ***
 * L := a G L | a          a : Bool_var [args]   G : gate
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
z3::expr vts::gates( Vec2Expr u, z3::expr x, z3::expr y, unsigned k, unsigned g ) {

  z3::expr_vector g_list(ctx);
  z3::expr_vector c_list (ctx);

  //std::cout << "iG list is " << g_list;
  auto g1 = x && y;  
  auto g2 = x || y;
  //auto g3 = ctx.bool_val(true);
  //auto g4 = x;
  //auto g5 = y;
  
  g_list.push_back( ( u[k][g] && g1 )  ||  ( u[k][g+1] && g2 ) );
  
  c_list.push_back( u[k][g] );
  c_list.push_back( u[k][g+1] );
  
  auto cSum = at_least_one( c_list ) && !at_least_two( c_list ); 
  
  auto lsCons = mk_or( g_list );
  auto cons = cSum && lsCons;
  return cons;
}

z3::expr vts::gate_fml (Vec3Expr s, unsigned i, unsigned k, unsigned g, bool e, unsigned j = 0, unsigned q = 0 ) {

  z3::expr_vector ls(ctx);
  z3::expr_vector cl_list(ctx);
  z3::expr_vector il_list(ctx);

  for ( unsigned k1 = 0; k1 < M; k1++ ) {
    if ( k1 == k )  continue;
    if ( e == true ) { 
       ls.push_back(    ( s[k][g][k1] && presence_edge [i][j][q][k1] ) 
                     || ( s[k][g][k1+M] && !presence_edge[i][j][q][k1] ) );
    }
    else {  
       ls.push_back(   ( s[k][g][k1] && nodes[i][k1] )  
                    || ( s[k][g][k1+M] && !nodes[i][k1] ) ); 
    }

    cl_list.push_back( s[k][g][k1] );
    cl_list.push_back( s[k][g][k1+M] );
    il_list.push_back( !s[k][q][k1] || !s[k][g][k1+M] ) ;

  }

  ls.push_back( s[k][g][2*M] && true );
  ls.push_back( s[k][g][(2*M) + 1] && false );
  cl_list.push_back( s[k][g][2*M] );
  cl_list.push_back( s[k][g][(2*M) + 1] );
  
  auto coeffSum = at_least_one( cl_list ) && !at_least_two ( cl_list ); 
  auto varList = mk_or( ls );
  auto litList = mk_and ( il_list );

  auto cons = varList && coeffSum && litList;

  return cons;
}

z3::expr vts::process_fml ( Vec3Expr s, Vec2Expr u, unsigned i, unsigned k, bool e, unsigned j = 0, unsigned q = 0 ) {
  
  z3::expr_vector ls( ctx );
  z3::expr gfml(ctx);

  if ( e == true ) {

    for ( unsigned g = 0; g < M-2; g++ ) {
      if ( g == 0 ) {
        auto arg1 = gate_fml( s, i, k, g, e, j, q );
        auto arg2 = gate_fml( s, i, k, g+1, e, j, q ); 
        gfml = gates( u, arg1, arg2, k, g ); 
        ls.push_back( gfml );
      }
      else { 
        auto arg2 = gate_fml( s, i, k, g+1, e, j, q ); 
        auto var_fml =  gates( u, gfml, arg2, k, g+1 );
        gfml = var_fml;
        ls.push_back( gfml );
      }
    }
  }
  else {
    for ( unsigned g = 0; g < M-2; g++ ) {
      if (g == 0) {
        auto arg1 = gate_fml( s, i, k, g, e ); 
        auto arg2 = gate_fml( s, i, k, g+1, e );
        gfml = gates ( u, arg1, arg2, k, g );
        ls.push_back( gfml );
      }
      else { 
        auto arg2 = gate_fml( s, i, k, g+1, e );
        gfml = gates( u, gfml, arg2, k, g+1 );
        ls.push_back( gfml );
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
      auto cfml = process_fml( s, u, i, k, false );
      auto fml = ( active_node[i][k] == cfml ); 
      main_list.push_back ( fml );
    }
  }

  auto cons = mk_and( main_list );
  return cons;
}

z3::expr vts::edge_gate_fml ( Vec3Expr t, Vec2Expr v ) {
  z3::expr_vector d_list(ctx);
  z3::expr e_fml (ctx);
 
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if ( i == j )  continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        for ( unsigned k = 0; k < M; k++ ) {
          e_fml = process_fml ( t, v, i, k, true, j, q );
          auto fml =  ( active_edge[i][j][q][k] == e_fml ); 
          d_list.push_back ( fml );
        }
      }
    }
  }

  auto cons = mk_and ( d_list );
  return ctx.bool_val(true) ;
}

z3::expr vts::logic_gates ( Vec3Expr s_var, Vec3Expr t_var, Vec2Expr u_var, Vec2Expr v_var ) {

  auto nodeGate = node_gate_fml ( s_var, u_var );
  //std::cout << nodeGate << "\n";
  
  auto edgeGate = edge_gate_fml ( t_var, v_var );
  //std::cout << edgeGate << "\n";
  
  auto cons = nodeGate && edgeGate;
  return cons;
}

