#ifndef VTS_Z3_UTIL
#define VTS_Z3_UTIL

#include <vector>
#include <z3++.h>

template <class T>
inline
std::vector<T> to_vector( const std::vector<std::vector<T>>& double_vec ) {
  std::vector<T> output;
  for(typename std::vector<std::vector<T>>::size_type i=0; i < double_vec.size(); ++i) {
    output.insert(output.end(), double_vec[i].begin(), double_vec[i].end());
  }
  return output;
}

typedef std::vector< std::vector<z3::expr> > VecsExpr;

// function returns prenex body
// and quantified variables in list of list qs.
// The first entry in qs is for exists
z3::expr prenex(z3::expr& fml, VecsExpr& qs );

// Simplify the given z3 formula and convert it into nnf
//void negform ( z3::context& c, z3::expr& fml );

// Simplify the given z3 formula and convert it into cnf

//std::vector<z3::expr_vector> cnf_converter( z3::expr& fml );
std::vector<z3::expr> cnf_converter( z3::expr& fml );

//void qdimacs_printer( z3::expr& fml, std::vector<z3::expr_vector>& m_var );

void qdimacs_printer(std::vector<z3::expr>& cnf_fml, VecsExpr& m_var );

#endif
