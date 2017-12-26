#include <stdio.h>
#include <z3++.h>
#include<vector>

using namespace z3;

int main() {

    context c;

    expr x = c.bool_const("x");
    expr y = c.bool_const("y");
    expr conjecture = (!(x && y)) == (!x || !y);
    
    solver s(c);
    // adding the negation of the conjecture as a constraint.
    s.add(!conjecture);
    std::cout << s << "\n";
    std::cout << s.to_smt2() << "\n";
    switch (s.check()) {
    case unsat:   std::cout << "de-Morgan is valid\n"; break;
    case sat:     std::cout << "de-Morgan is not valid\n"; break;
    case unknown: std::cout << "unknown\n"; break;
    }

  // z3::context ctx;
  // auto i_sort = ctx.int_sort();
  // z3::expr x = ctx.constant( "x_name", i_sort );
  // z3::expr e = x > 3 && x < 5;

  

  printf("hello world!!");

  return 0;
}
