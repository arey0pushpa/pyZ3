#include "z3-util.h"

z3::expr parseFormula( z3::context& c, std::string str,
                       const std::vector< std::string >& names ) {
  unsigned s = names.size();
  Z3_symbol* symbols = new Z3_symbol[s];
  Z3_func_decl* decls = new Z3_func_decl[s];
  std::vector<z3::expr> vars; // for keeping smart pointer alive
  for (unsigned i=0; i < s; i++) {
    const char* name = names[i].c_str();
    z3::expr v = c.bool_const( name );
    vars.push_back( v );
    decls[i] = v.decl();
    symbols[i] = Z3_mk_string_symbol(c, name);
  }
  str = sanitise_string(str);
  std::string cmd = str.find_first_of(' ')!=std::string::npos
    ? "(assert (" + str + "))" : "(assert " + str + ")";
  z3::expr ast(c);
  try {
    Z3_ast e = Z3_parse_smtlib2_string(c, cmd.c_str(), 0, NULL, NULL, s, symbols, decls);
    delete[] symbols;
    delete[] decls;
    ast = to_expr(c, e);
  }
  catch( z3::exception e ) {
    std::cerr << "Error parsing line \"" << str << "\"." << std::endl;
    throw;
  }
  return ast;
}
