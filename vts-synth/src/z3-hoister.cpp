#define _NO_OMP_
#define _MP_GMP

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

#include "z3-util.h"

//----------------------------------------------------------------------------
// Basic Z3 utils

ast_manager& get_ast_manager( z3::context& c ) {
  Z3_context z3_ctx = c;
  ast_manager& m = mk_c(z3_ctx)->m();
  return m;
}

ast_manager& get_ast_manager( z3::expr& f ) {
  auto& c = f.ctx();
  return get_ast_manager( c );
}

expr_ref get_z3_internal_expr_ref( z3::expr& f ) {
  auto& m = get_ast_manager( f );
  expr* f_expr = to_expr(f);
  //std::cout << &f_expr << "\n";
  expr_ref   f_ref(m);
  expr_safe_replace subst(m);
  subst(f_expr, f_ref);
  return f_ref;
}

//z3::expr exp = to_ast(f_ref);
//----------------------------------------------------------------------------
// prenex normal form

void filter_vars( qe::pred_abs& m_pred_abs, app_ref_vector const& vars ) {
  for (unsigned i = 0; i < vars.size(); ++i) {
    m_pred_abs.fmc()->insert(vars[i]->get_decl());
    //std::cout << "The filt vars are: " << vars << "\n" ;  
  }
}

void hoist(ast_manager& m, expr_ref& fml) {

  //qe::pred_abs m_pred_abs(m); //some function on this object must be called???
  vector<app_ref_vector> m_vars;

  // todo: why this?
  //proof_ref pr(m);
  //label_rewriter rw(m);
  //rw.remove_labels(fml, pr);

  quantifier_hoister hoist(m);
  app_ref_vector vars(m);
  bool is_forall = false;        
  //m_pred_abs.get_free_vars(fml, vars);
  //m_vars.push_back(vars);
  //std::cout << "The current vars are: " << vars << "\n" ;  
  vars.reset();
  hoist.pull_quantifier(is_forall, fml, vars);
  //std::cout << "At the mid run the fml is : " << fml << "\n";
  m_vars.back().append(vars);
  //std::cout << "The current vars are: " << vars << "\n" ;  
  //filter_vars(m_pred_abs, vars);
  // }
  do {
    is_forall = !is_forall;
    vars.reset();
    hoist.pull_quantifier(is_forall, fml, vars);
    //std::cout << "At the mid run the fml is : " << fml << "\n";
    m_vars.push_back(vars);
    //std::cout << "The current vars are: " << vars << "\n";  
    //filter_vars( m_pred_abs, vars );
  }
  while (!vars.empty());
  SASSERT(m_vars.back().empty()); 
  std::cout << "The resultant formula is: " << fml;  
  for( auto& m: m_vars ) {
    std::cout << m;
    //std::cout << "The sort of " << m << " is " << Z3_get_sort (c, m); 
  // todo: do we need it??
  //initialize_levels();
  }
  std::cout << "\n";
}

void prenex( z3::expr& f ) {
 // std::cout << "The sort of the formula f is: " << Z3_get_sort_name( c, f);
  auto& m = get_ast_manager(f);
  auto f_ref = get_z3_internal_expr_ref( f );
  hoist( m, f_ref );
}

