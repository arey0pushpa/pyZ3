#ifndef VTS_Z3_UTIL
#define VTS_Z3_UTIL

#define _NO_OMP_
#include <z3++.h>

// function returns prenex body
// and quantified variables in list of list qs.
// The first entry in qs is for exists
z3::expr prenex(z3::expr& fml, std::vector<z3::expr_vector>& qs );

// Simplify the given z3 formula and convert it into nnf 
//void negform ( z3::context& c, z3::expr& fml );

// Simplify the given z3 formula and convert it into cnf 
//z3::expr cnf-converter ( z3::context& c, z3::expr& fml, std::vector<z3::expr_vector>& qs );


#endif
