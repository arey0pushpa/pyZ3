#include <stdio.h>
#include <z3++.h>
#include<vector>

using namespace z3;

int main() {

    // context c;

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


    std::cout << "tactic example 1\n";
    context c;
    expr x = c.bool_const("x");
    expr y = c.bool_const("y");
    expr z = c.bool_const("z");
    expr w = c.bool_const("w");
    expr f = exists( x, forall( z, x && z ) && forall( y, exists( w, y && w ) ) );
    goal g(c);
    g.add( f );
    std::cout << g << "\n";

    // int num = Z3_get_num_tactics(c);
    // for( int i = 0 ; i < num; i++ ) {
    //   std::cout << Z3_get_tactic_name( c, i ) << "\n";
    // }

    tactic t1(c, "qsat");
    // // tactic t2(c, "solve-eqs");
    // tactic t = t1 & t2;
    apply_result r = t1(g);
    std::cout << r << "\n";

  // z3::context ctx;
  // auto i_sort = ctx.int_sort();
  // z3::expr x = ctx.constant( "x_name", i_sort );
  // z3::expr e = x > 3 && x < 5;

  

  // printf("hello world!!");

  return 0;
}
