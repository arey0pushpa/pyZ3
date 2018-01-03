#define _NO_OMP_
#define _MP_INTERNAL

#include "ast/ast.h"
#include "ast/ast.cpp"
#include "ast/expr_abstract.h"
#include "ast/ast_util.h"
#include "ast/rewriter/quant_hoist.h"
#include "ast/ast_pp.h"
#include "ast/rewriter/th_rewriter.h"
#include "ast/rewriter/expr_replacer.h"
#include "model/model_v2_pp.h"
#include "model/model_evaluator.h"
#include "smt/smt_kernel.h"
#include "smt/params/smt_params.h"
#include "smt/smt_solver.h"
#include "solver/solver.h"
#include "solver/mus.h"
#include "qe/qsat.h"
#include "qe/qe_mbp.h"
#include "qe/qe.h"
#include "ast/rewriter/label_rewriter.h"
#include "api/api_context.h"
#include "ast/rewriter/expr_safe_replace.h"
#include <string>
#include <sstream>
#include <iostream>
#include <stdint.h>
#include <vector>

#include "z3-util.h"

ast_manager& get_ast_manager( z3::context& c ) {
  Z3_context z3_ctx = c;
  ast_manager& m = mk_c(z3_ctx)->m();
  return m;
}

ast_manager& get_ast_manager( z3::expr& f ) {
  auto& c = f.ctx();
  return get_ast_manager( c );
}

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
            sargs.reset();
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

//z3::expr cnf-converter ( z3::context& c, z3::expr& fml, vector<app_ref_vector>& var_lists ) {
//
//    z3::expr fml = f;
//    z3::solver s(c);
//    z3::goal g(c);
//    g.add( f );
//
//    z3::tactic t1(c, "simplify");
//    z3::tactic t2(c, "tseitin-cnf");
//    z3::tactic t = t1 & t2;
//    z3::apply_result r = t(g);
//    std::cout << r << "\n";
//    return r;
//
//}
