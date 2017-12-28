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



void hoist(expr_ref& fml) {
  proof_ref pr(m);
  label_rewriter rw(m);
  rw.remove_labels(fml, pr);
  quantifier_hoister hoist(m);
  app_ref_vector vars(m);
  bool is_forall = false;        
  m_pred_abs.get_free_vars(fml, vars);
  m_vars.push_back(vars);
  vars.reset();
  if (m_mode != qsat_sat) {
    is_forall = true;
    hoist.pull_quantifier(is_forall, fml, vars);
    m_vars.push_back(vars);
    filter_vars(vars);
  } else {
    hoist.pull_quantifier(is_forall, fml, vars);
    m_vars.back().append(vars);
    filter_vars(vars);
  }
  do {
    is_forall = !is_forall;
    vars.reset();
    hoist.pull_quantifier(is_forall, fml, vars);
    m_vars.push_back(vars);
    filter_vars(vars);
  }
  while (!vars.empty());
  SASSERT(m_vars.back().empty()); 
  initialize_levels();
  TRACE("qe", tout << fml << "\n";);
}
