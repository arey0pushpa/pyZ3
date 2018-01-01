#define _NO_OMP_
#define _MP_GMP

#include <stdio.h>
#include <z3++.h>
#include <vector>

// #include "z3-util.h"

// class ast_manager;

// using namespace z3;

// void filter_vars( qe::pred_abs& m_pred_abs, app_ref_vector const& vars ) {
//   for (unsigned i = 0; i < vars.size(); ++i) {
//     m_pred_abs.fmc()->insert(vars[i]->get_decl());
//   }
// }

// void hoist(ast_manager& m, expr_ref& fml) {

//   qe::pred_abs m_pred_abs(m); //some function on this object must be called???
//   vector<app_ref_vector> m_vars;

//   // todo: why this?
//   proof_ref pr(m);
//   label_rewriter rw(m);
//   rw.remove_labels(fml, pr);

//   quantifier_hoister hoist(m);
//   app_ref_vector vars(m);
//   bool is_forall = false;        
//   m_pred_abs.get_free_vars(fml, vars);
//   m_vars.push_back(vars);
//   vars.reset();
//   hoist.pull_quantifier(is_forall, fml, vars);
//   m_vars.back().append(vars);
//   filter_vars(m_pred_abs, vars);
//   // }
//   do {
//     is_forall = !is_forall;
//     vars.reset();
//     hoist.pull_quantifier(is_forall, fml, vars);
//     m_vars.push_back(vars);
//     filter_vars( m_pred_abs, vars );
//   }
//   while (!vars.empty());
//   SASSERT(m_vars.back().empty()); 

//   // todo: do we need it??
//   // initialize_levels();

// }

// void visit(expr const & e) {
//     if (e.is_app()) {
//         unsigned num = e.num_args();
//         for (unsigned i = 0; i < num; i++) {
//             visit(e.arg(i));
//         }
//         // do something
//         // Example: print the visited expression
//         func_decl f = e.decl();
//         std::cout << "application of " << f.name() << ": " << e << "\n";
//     }
//     else if (e.is_quantifier()) {
//         visit(e.body());
//         // do something
//     }
//     else {
//         assert(e.is_var());
//         // do something
//     }
// }


// private static void qdimacs(Context ctx, Solver slv)
// {
//     var goal = ctx.MkGoal();
//     goal.Add(slv.Assertions);
//     var applyResult = ctx.Then(ctx.MkTactic("simplify"),
//         ctx.MkTactic("bit-blast"),
//         ctx.MkTactic("tseitin-cnf")).Apply(goal);

//     // Check the new variables due to tseiting transformation.
//     //
//     Debug.Assert(applyResult.Subgoals.Length==1);

//     var map = new Dictionary<BoolExpr,int>();
//     foreach (var f in applyResult.Subgoals[0].Formulas)
//     {
//         Debug.Assert(f.IsOr);
//         foreach (var e in f.Args)
//             if (e.IsNot)
//             {
//                 Debug.Assert(e.Args.Length==1);
//                 Debug.Assert(e.Args[0].IsConst);
//                 map[(BoolExpr)e.Args[0]] = 0;
//             }
//             else
//             {
//                 Debug.Assert(e.IsConst);
//                 map[(BoolExpr)e] = 0;
//             }
    
//     }

//     // Map the variables with their id values.
//     var id = 1;
//     foreach (var key in map.Keys.ToArray())
//         map[key] = id++;
//         // map[keys] = get_var_index()

//     using (var fos = File.CreateText("problem.cnf"))
//     {
//         fos.WriteLine("c DIMACS file format");
//         fos.WriteLine($"p cnf {map.Count} {applyResult.Subgoals[0].Formulas.Length}");
//         foreach(var f in applyResult.Subgoals[0].Formulas)
//         {
//             foreach (var e in f.Args)
//                 if (e.IsNot)
//                     fos.Write($"{map[(BoolExpr)e.Args[0]]} ");
//                 else
//                     fos.Write($"-{map[(BoolExpr)e]} ");

//             fos.WriteLine("0");
//         }
//     }
// }

//class ast_manager;
//ast_manager& get_manager() { return m; }

//ast_manager & m() const { return m_inv.get_manager(); }
int main() {

    // expr x = c.bool_const("x");
    // expr y = c.bool_const("y");
    // expr conjecture = (!(x && y)) == (!x || !y);
    
   // solver s(c);
    // // adding the negation of the conjecture as a constraint.
    // s.add(!conjecture);
    // std::cout << s << "\n";
    // std::cout << s.to_smt2() << "\n";
    // switch (s.check()) {
    // case unsat:   std::cout << "de-Morgan is valid\n"; break;
    // case sat:     std::cout << "de-Morgan is not valid\n"; break;
    // case unknown: std::cout << "unknown\n"; break;
    // }

    //std::cout << "tactic example 1\n";
    //ast_manager * mk_ast_manager();
    // Z3_ast    m_ast;

    z3::context c;

    z3::expr x = c.bool_const("x");
    z3::expr y = c.bool_const("y");
    z3::expr z = c.bool_const("z");
    z3::expr w = c.bool_const("w");
    z3::expr f = exists( x, forall( z, x && z ) && forall( y, exists( w, y && w ) ) );

    // prenex( f );

    ///printf("The Fake value of Ast is: %o \n",m_ast);
    //std::cout << Z3_get_sort (c, f);
    // std::cout << Z3_get_sort (c, f);
    //expr_ref f_ref = *f;
    
    //hoist (m, **f);
    
    // Let's assume we got a Vector of the Quantifiers and formula that do not contain quantifiers.
    //
    
    z3::solver s(c);
    z3::goal g(c);
    g.add( f );
    std::cout << g << "\n";

    // // int num = Z3_get_num_tactics(c);
    // // for( int i = 0 ; i < num; i++ ) {
    // //   std::cout << Z3_get_tactic_name( c, i ) << "\n";
    // // }

    z3::tactic t1(c, "qsat");
    // // tactic t2(c, "solve-eqs");
    // tactic t = t1 & t2;
    z3::apply_result r = t1(g);
    std::cout << r << "\n";


  return 0;
}
