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

// The expression can take any gates. gth gate.
z3::expr vts::gates( Vec3Expr& u, z3::expr x, z3::expr y, unsigned k, unsigned g ) {

  z3::expr_vector g_list(ctx);
  z3::expr_vector c_list (ctx);

  //std::cout << "iG list is " << g_list;
  auto g1 = x && y;  
  auto g2 = x || y;

  //auto g3 = ctx.bool_val(true);
  //auto g4 = x;
  //auto g5 = y;
    
  c_list.push_back( u[k][g][0] );
  c_list.push_back( u[k][g][1] );
  
  // Exactly One
  auto cSum = at_least_one( c_list ) && !at_least_two( c_list ); 
    
  auto lsCons =  ( u[k][g][0] && g1 )  ||  ( u[k][g][1] && g2 );

  auto cons = cSum && lsCons;
  return cons;
}

z3::expr vts::var_fml (VecExpr& chooseVar, unsigned i, unsigned k, bool e, unsigned j = 0, unsigned q = 0 ) {
  // auto& chooseVar = chooseVars[k][m];
  z3::expr_vector ls(ctx);
  z3::expr_vector cl_list(ctx);
  z3::expr_vector il_list(ctx);

  for ( unsigned k1 = 0; k1 < M; k1++ ) {
    if ( k1 == k )  continue;
    if ( e == true ) { 
       ls.push_back(    ( chooseVar[k1] && presence_edge [i][j][q][k1] ) 
                     || ( chooseVar[k1+M] && !presence_edge[i][j][q][k1] ) );
    }
    else {  
       ls.push_back(   ( chooseVar[k1] && nodes[i][k1] )  
                    || ( chooseVar[k1+M] && !nodes[i][k1] ) ); 
    }

    cl_list.push_back( chooseVar[k1] );
    cl_list.push_back( chooseVar[k1+M] );
    il_list.push_back( !chooseVar[k1] || !chooseVar[k1+M] ) ;

  }

  ls.push_back( chooseVar[2*M] && true );
  ls.push_back( chooseVar[(2*M) + 1] && false );
  cl_list.push_back( chooseVar[2*M] );
  cl_list.push_back( chooseVar[(2*M) + 1] );
  
  auto coeffSum = at_least_one( cl_list ) && !at_least_two ( cl_list ); 
  auto varList = mk_or( ls );
  auto litList = mk_and ( il_list );

  auto cons = varList && coeffSum && litList;

  //std::cout << cons << "\n";
  return cons;
}

z3::expr_vector vts::reduce_fml ( z3::context& ctx, z3::expr_vector& main_list, unsigned mLen, Vec3Expr& u, unsigned k, unsigned& gateVar ) {
  
  z3::expr_vector cons_list ( ctx ); 
  z3::expr fml( ctx );
  
  for ( unsigned x = 0; x < mLen; x += 2 ) {

    if ( x == mLen - 1 ) {
      cons_list.push_back ( main_list[x] );
    } else {
      auto fml1 = main_list[x];
      auto fml2 = main_list[x+1];

      fml = gates ( u, fml1, fml2, k, gateVar );
      cons_list.push_back ( fml );
      gateVar += 1;
    } 
  }
  return cons_list;
}


// chooseVars : params for selecting variables at the leaf of functions
// chooseGates : params for selecting gates at the internal nodes of functions
// i : node number
// m1 : molecule
// isEdge : edge Or Node
// j : target node
// q : edge idx
z3::expr vts::build_rhs_fml ( Vec3Expr& chooseVars, Vec3Expr& chooseGates,
                              unsigned i, unsigned m1,
                              bool isEdge,
                              unsigned j = 0, unsigned q = 0 ) {
  
  z3::expr_vector ls(ctx);
  z3::expr gfml(ctx);
  
  unsigned step = 0;
  unsigned gateVar = 0;
    
  z3::expr_vector n_list( ctx );
  z3::expr_vector main_list ( ctx );
    
  // first pass. create paired expression of form: a G a | a
  for ( unsigned m = 0; m < M; m++ ) {
    if ( m == m1 )  continue;
    z3::expr vfml ( ctx );

    if ( isEdge == true ) 
      n_list.push_back ( var_fml( chooseVars[m1][m], i, m1, isEdge, j, q ) );
    else 
      n_list.push_back ( var_fml( chooseVars[m1][m], i, m1, isEdge ) );        

    // if ( isEdge == true ) 
    //   n_list.push_back ( var_fml( chooseVars, i, m1, m, isEdge, j, q ) );
    // else 
    //   n_list.push_back ( var_fml( chooseVars, i, m1, m, isEdge ) );        
      
    step += 1; 
    
    if ( step == 2 ) {
     
      main_list.push_back( gates ( chooseGates, n_list[0], n_list[1], m1, gateVar ) );
      step = 0;
      gateVar += 1;
      
      n_list.resize ( 0 );
    }
      
    if ( step == 1 && m == M - 1 ) {
      main_list.push_back( n_list[0] );
    }
  }

    // second pass. create complete formula.
    auto mLen = main_list.size();
    
    if ( mLen == 1 ) {
      return main_list[0];
    }
    
    for ( unsigned h = 0; h < ((mLen / 2) + 1); h++ ) {
      main_list = reduce_fml ( ctx, main_list, main_list.size(), chooseGates, m1, gateVar );
      //std::cout << "List size dec to " << main_list.size() << "\n";
      if ( main_list.size() == 1 ) {
        return main_list[0];
      }
    }
    
    return main_list[0];
     
    /*
    if ( gateVar != M - 1 ) {
      u[k][gateVar][0] = 0;
      u[k][gateVar][1] = 0;
    }
    */
}

z3::expr vts::node_gate_fml ( Vec3Expr& s, Vec3Expr& u ) {
  z3::expr_vector n_list(ctx);
  
  for( unsigned i = 0; i < N; i++ ) {
    for( unsigned m = 0; m < M; m++ ) {
      
      auto nfml = build_rhs_fml( s, u, i, m, false );
      auto fml = ( active_node[i][m] == nfml ); 
      
      n_list.push_back ( fml );
    }
  }

  auto cons = mk_and( n_list );
  return cons;
}

z3::expr vts::edge_gate_fml ( Vec3Expr& t, Vec3Expr& v ) {
  z3::expr_vector e_list(ctx);
 
  for( unsigned i = 0 ; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if ( i == j )  continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        for ( unsigned k = 0; k < M; k++ ) {
          
          auto efml = build_rhs_fml ( t, v, i, k, true, j, q );
          auto fml = ( active_edge[i][j][q][k] == efml ); 
          
          e_list.push_back ( fml );
        }
      }
    }
  }

  auto cons = mk_and ( e_list );
  return cons;
}

z3::expr vts::logic_gates ( Vec3Expr& s_var, Vec3Expr& t_var, Vec3Expr& u_var, Vec3Expr& v_var ) {

  //for ( 
  //auto chooseVarVector =    
  auto nodeGate = node_gate_fml ( s_var, u_var );
  //std::cout << nodeGate << "\n";
  
  auto edgeGate = edge_gate_fml ( t_var, v_var );
  //std::cout << edgeGate << "\n";
  
  auto cons = nodeGate && edgeGate;
  //std::cout << cons;
  
  return cons;
}

