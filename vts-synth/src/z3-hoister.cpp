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

#include "z3-util.h"

void filter_vars( qe::pred_abs& m_pred_abs, app_ref_vector const& vars ) {
  for (unsigned i = 0; i < vars.size(); ++i) {
    m_pred_abs.fmc()->insert(vars[i]->get_decl());
  }
}

void hoist(ast_manager& m, expr_ref& fml) {

  qe::pred_abs m_pred_abs(m); //some function on this object must be called???
  vector<app_ref_vector> m_vars;

  // todo: why this?
  proof_ref pr(m);
  label_rewriter rw(m);
  rw.remove_labels(fml, pr);

  quantifier_hoister hoist(m);
  app_ref_vector vars(m);
  bool is_forall = false;        
  m_pred_abs.get_free_vars(fml, vars);
  m_vars.push_back(vars);
  vars.reset();
  hoist.pull_quantifier(is_forall, fml, vars);
  m_vars.back().append(vars);
  filter_vars(m_pred_abs, vars);
  // }
  do {
    is_forall = !is_forall;
    vars.reset();
    hoist.pull_quantifier(is_forall, fml, vars);
    m_vars.push_back(vars);
    filter_vars( m_pred_abs, vars );
  }
  while (!vars.empty());
  SASSERT(m_vars.back().empty()); 

  // todo: do we need it??
  // initialize_levels();

}


void prenex( z3::expr& f ) {
  auto& c = f.ctx();
  Z3_context z3_ctx = c;
  ast_manager& m = mk_c(z3_ctx)->m();

  // expr tp expr_ref
  expr* f_expr = to_expr(f);

  // hoist( m, f_expr );
}
