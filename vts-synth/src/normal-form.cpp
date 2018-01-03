#include "z3-util.h"

#include <vector>

// ast_manager& get_ast_manager( z3::context& c ) {
//   Z3_context z3_ctx = c;
//   ast_manager& m = mk_c(z3_ctx)->m();
//   return m;
// }

// ast_manager& get_ast_manager( z3::expr& f ) {
//   auto& c = f.ctx();
//   return get_ast_manager( c );
// }

z3::expr ielim( z3::expr const& e ) {
    if ( e.is_app() ) {
      if ( e.is_bool() ) {
          z3::func_decl f = e.decl();
          unsigned num = e.num_args();
         // std::vector<int> first;
          std::vector<z3::expr> sargs;
          for (unsigned i = 0; i < num; i++) {
              //§std::cout << e.arg(i) << "\n";
              sargs.push_back ( ielim( e.arg(i)) );
          } 
          if( f.decl_kind() == Z3_OP_AND) {
            //return and ( sargs );
            sargs.clear();
          }
         
         if( f.decl_kind() == Z3_OP_OR) {
          }
         
         if( f.decl_kind() == Z3_OP_IMPLIES) {
          }
         
         if( f.decl_kind() == Z3_OP_EQ) {
          }
         
         if( f.decl_kind() == Z3_OP_NOT) {
          }
        //std::cout << "the value of f is : " << f << "\n";//
        //std::cout << "application of " << f.name() << ": " << e << "\n";

        }
    }

    else if (e.is_quantifier()) {
        std::cout << "I'm APP_QUANT \n";
        //Forall Case
        // get the variables vlist
        //return forall ( vlist, ielim(e.body() );
            
        // Exists Case
        // get the variables vlist
        //return exists ( vlist, ielim(e.body() );
    }
    else { 
        assert(e.is_var());
        return e;
    }
}


void nnf( z3::expr const& e ) {
    if ( e.is_app() ) {
      if ( e.is_bool() ) {
          z3::func_decl f = e.decl();
          unsigned num = e.num_args();
          for (unsigned i = 0; i < num; i++) {
              std::cout << e.arg(i) << "\n";
              nnf(e.arg(i));
         }
         if( f.decl_kind() == Z3_OP_AND) {
          }
         
         if( f.decl_kind() == Z3_OP_OR) {
          }
         
         if( f.decl_kind() == Z3_OP_IMPLIES) {
          }
         
         if( f.decl_kind() == Z3_OP_EQ) {
          }
         
         if( f.decl_kind() == Z3_OP_NOT) {
          }
        }
    }

    else if (e.is_quantifier()) {
    }
    else { 
        assert(e.is_var());
    }
}

void negform ( z3::context& c, z3::expr& f ){
    //auto& c = f.ctx();
    //auto& m = get_ast_manager(f);
    z3::expr fml = f;
    z3::solver s(c);
    z3::goal g(c);
    g.add( f );
    z3::tactic t1(c, "simplify");
    //z3::tactic t2(c, "nnf");
    //z3::tactic t = t1 & t2;
    z3::apply_result r = t1(g);
    //std::cout << r.get_ast();

    //auto res = Z3_apply_result_to_string(c, r);
    //std::cout << res.sort_kind();
    //z3::expr exxx = to_expr(c, res);
    //z3::expr result = Z3_apply_result_dec_ref(c , r);
    //z3::expr r_exp = to_expr(c, r); 
    //std::cout << r << "\n";
    //return r;
    //z3::expr iefml = nnf( f ); 
    ielim( f ); 
    //std::cout << "Implies elim'd formula: " << iefml << "\n"; 
    //z3::expr nfml =  nnf ( f );
    //std::cout << "nnf'd formula: " << nfml << "\n"; 
}

z3::expr cnf_converter( z3::expr& fml,
                        std::vector<z3::expr_vector>& var_lists ) {

  z3::context& c = fml.ctx();
   z3::solver s(c);
   z3::goal g(c);
   g.add( fml );

   z3::tactic t1(c, "simplify");
   z3::tactic t2(c, "tseitin-cnf");
   z3::tactic t = t1 & t2;
   z3::apply_result r = t(g);
   return r[0].as_expr();
}
