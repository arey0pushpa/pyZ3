#include <vector>
#include <numeric>
#include <utility>
#include <iterator>
#include <fstream>      
#include <map>

#include "z3-util.h"

// template <template<typename...> class R=std::vector, 
//           typename Top, 
//           typename Sub = typename Top::value_type> 
//    R<typename Sub::value_type> flatten(Top const& all)
// {
//     using std::begin;
//     using std::end;

//     R<typename Sub::value_type> accum;
    
//     for(auto& sub : all)
//         std::copy(begin(sub), end(sub), std::inserter(accum, end(accum)));
        
//     return accum;
// }



//use Z3_ast
void collect( z3::expr e ,
              std::vector<z3::expr>& r,
              std::map <Z3_ast, int>& var_id_map ) {
  if ( e.is_bool() ) {
    z3::func_decl f = e.decl();
    if ( f.decl_kind() == Z3_OP_OR) {
      unsigned num = e.num_args();
      for (unsigned i = 0; i < num; i++) {
        collect(  e.arg(i), r,  var_id_map);
      }
    }else if ( f.decl_kind() == Z3_OP_NOT) {
      collect( e.arg(0), r, var_id_map  );
    }else{
      // Check if element present in the list.
      if( var_id_map.count(e) == 0 ) {
        var_id_map[e] = var_id_map.size() + 1;
        std::cout << var_id_map[e];
        r.push_back (e);  // check types
      }
    }
  }
}

// todo: check type of second argument?
std::vector <z3::expr> visit( std::vector <z3::expr>& cnf_fml, std::map <Z3_ast, int>& var_id_map ) {
    //std::set<z3::expr> r;
    std::vector <z3::expr> r;

    for( auto& e : cnf_fml) {
        collect( e, r , var_id_map);
    }
    
    return r;
}


inline void print_variable( z3::expr& v, std::ofstream& o,
                            std::map <Z3_ast, int>& vmap ) {
  if( v.is_app() && v.num_args() == 0 && v.is_bool() ) {
    o << vmap.at( v ) << " ";
  }else{
    assert(false);
  }
}

inline void print_variables( std::vector<z3::expr>& vs, std::ofstream& o,
                             std::map <Z3_ast, int>& vmap ) {
  for (unsigned int i = 0; i < vs.size(); i++) {
    z3::expr arg = vs.at(i);
    print_variable( arg , o, vmap );
  }
  o << "0\n";
}

inline void print_literal( z3::expr& v, std::ofstream& o,
                    std::map <Z3_ast, int>& vmap ) {
  z3::func_decl f = v.decl();
  if ( f.decl_kind() == Z3_OP_NOT) {
    z3::expr arg = v.arg(0);
    o << "-";print_variable( arg, o, vmap);
  }else{
    print_variable( v, o, vmap);
  }
}


inline void print_clause( z3::expr& cl, std::ofstream& o,
                          std::map <Z3_ast, int>& vmap ) {
  z3::func_decl f = cl.decl();
  if ( f.decl_kind() == Z3_OP_OR) {
      for( unsigned i =0; i < cl.num_args(); i++ ) {
        z3::expr arg = cl.arg(i);
        print_literal( arg, o, vmap );
      }
  }else{
    print_literal(cl,o,vmap);
  }
  o << "0\n";
}



void qdimacs_printer(std::vector<z3::expr>& cnf_fml,
                        VecsExpr& m_vars ) {

    std::vector <z3::expr> fresh_vars;
    // auto var_list = flatten( m_vars );
    auto var_list = to_vector( m_vars );

    // Create a Map from var to id, var: id
    std::map <Z3_ast, int> var_id_map;

    // Map variable to id in the dictionary.
    unsigned int id = 1;
    for (auto& key: var_list) {  // todo: check the type of key
        var_id_map[key] = id++;
    }
    

    //todo: get fresh vars
    fresh_vars =  visit( cnf_fml , var_id_map); 

    // Add fresh variables in the m_vars
    if( m_vars.size() % 2 == 0 ) {
      //last row is forall. So, add a new row
      m_vars.push_back( fresh_vars );
    }else{
      //last row is exists. So, push fresh in the same row
      for( unsigned i = 0; i < fresh_vars.size(); i++ ) {
        m_vars.back().push_back( fresh_vars[i] );
      }
    }

    // BEGIN QDIMACS PRINITNG  
    std::ofstream ofs;
    ofs.open ("/tmp/myfile.qdimacs", std::ofstream::out );
    // First part of a comment.
    ofs << "c This is a QDIMACS file output \n";

    // Print quantifier and total claused information.     
    auto q_var = var_id_map.size();
    auto num_clause = cnf_fml.size(); // not correct check plz 
    ofs << "p" << " cnf " << q_var << " "<< num_clause << "\n";

    unsigned int index = 0;
    for (auto& e: m_vars ){
      if (index == 0) {
        if (!e.empty()){
          ofs << "e" << " ";
          print_variables( e , ofs, var_id_map );
        }
      }else{
          // For all quantifier case.
        std::string q_t = index % 2 == 0 ? "e " : "a ";
        ofs << q_t;
        print_variables( e , ofs, var_id_map );
      }
      index = index + 1;
    }

    // Last part of cnf to index output.
    for (auto& e: cnf_fml) {
      print_clause( e, ofs, var_id_map );
    }
    ofs.close();
}


/*
 Example :
  p cnf 4 2
  e 1 2 3 4 0
  -1  2 0
   2 -3 -4 0

*/
