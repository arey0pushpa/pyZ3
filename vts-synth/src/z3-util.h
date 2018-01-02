#ifndef VTS_Z3_UTIL
#define VTS_Z3_UTIL

#include <z3++.h>

// function returns prenex body
// and quantified variables in list of list qs.
// The first entry in qs is for exists
z3::expr prenex(z3::expr& fml, std::vector<z3::expr_vector>& qs );


#endif
