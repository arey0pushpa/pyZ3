#include "z3-util.h"

z3::expr parseFormula( z3::context& c, std::string str,
                       const std::vector< std::string >& names ) {
  unsigned s = names.size();
  std::cout << s << "\n";
  Z3_symbol* symbols = new Z3_symbol[s];
  Z3_func_decl* decls = new Z3_func_decl[s];
  unsigned i = 0;
  for (unsigned i=0; i<s; i++) {
    symbols[i] = Z3_mk_string_symbol(c,names[i].c_str());
    z3::expr v = c.bool_const( names[i].c_str() );
    decls[i] = v.decl();
    // todo: the folloing inc causes memory leak. If enabled, the
    // corresponding dec_ref causes segmentation fault. !!!
    // Z3_inc_ref(c, reinterpret_cast<Z3_ast>(decls[i]));
  }
  str = sanitise_string(str);
  std::string cmd = str.find_first_of(' ')!=std::string::npos ? "(assert (" + str + "))" : "(assert " + str + ")";
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
  // adjust reference counter for variable
  for (unsigned j=0; j<i; j++) {
    Z3_dec_ref(c, (Z3_ast)decls[j]);
  }

  return ast;
}
