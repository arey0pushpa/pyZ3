#ifndef VTS_Z3_UTIL_H
#define VTS_Z3_UTIL_H

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

typedef std::vector< z3::expr > VecExpr;
typedef std::vector< VecExpr > Vec2Expr;
typedef std::vector< Vec2Expr > Vec3Expr;
typedef std::vector< Vec3Expr > Vec4Expr;

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

// Add function for the depqbf file creator.
void depqbf_file_creator(VecExpr& edgeQuant);

inline z3::expr make_bool(z3::context& c, std::string& name) {
  return c.bool_const( name.c_str() );
}

inline z3::expr mk_and( z3::context& c, VecExpr& es ) {
  z3::expr_vector args(c);
  for (unsigned i = 0; i < es.size(); i++)
    args.push_back( es[i] );
  return z3::mk_and( args );
}

inline z3::expr mk_or( z3::context& c, VecExpr& es ) {
  z3::expr_vector args(c);
  for (unsigned i = 0; i < es.size(); i++)
    args.push_back( es[i] );
  return z3::mk_or( args );
}

inline bool is_true( z3::expr e,  z3::model m ) {
  z3::expr v = m.eval( e );
  return ( Z3_get_bool_value( v.ctx(), v)  == Z3_L_TRUE );
}

inline bool is_false( z3::expr e,  z3::model m ) {
  z3::expr v = m.eval( e );
  return ( Z3_get_bool_value( v.ctx(), v)  == Z3_L_FALSE );
}


#endif
