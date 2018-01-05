#include <vector>
#include <numeric>
#include <utility>
#include <iterator>
#include <fstream>      
#include <map>
#include "z3-util.h"

template <template<typename...> class R=std::vector, 
          typename Top, 
          typename Sub = typename Top::value_type> 
   R<typename Sub::value_type> flatten(Top const& all)
{
    using std::begin;
    using std::end;

    R<typename Sub::value_type> accum;
    
    for(auto& sub : all)
        std::copy(begin(sub), end(sub), std::inserter(accum, end(accum)));
        
    return accum;
}

void collect( z3::expr& e , std::vector <z3::expr> r, std::map <z3::expr, int>& var_id_map ) {
  if ( e.is_bool() ) {
      z3::func_decl f = e.decl();
      if ( f.decl_kind() == Z3_OP_OR) {
          unsigned num = e.num_args();
          for (unsigned i = 0; i < num; i++) {
              collect( e.arg(i) ) 
           }
      }
         
      if ( f.decl_kind() == Z3_OP_NOT) {
          collect( e.arg(0) );
      }
      else {
          assert(e.is_var());
          // Check if element present in the list.
          if (var_id_map.count() == 0) {
              var_id_map[e] = var_id_map.size() + 1;
              r.push_back (e);  // check types
          }
      } 
  }
}

// todo: check type of second argument?
std::vector <z3::expr> visit( std::vector <z3::expr>& cnf_fml, std::map <z3::expr, int>& var_id_map ) {
    
    //std::set<z3::expr> r;   
    std::vector <z3::expr> r;
    
    for (auto& e : fml) {
        collect( e, r , var_id_map);
    }

    return r
}

void qdimacs_printer(std::vector<z3::expr>& cnf_fml,
                        std::vector<z3::expr_vector>& m_vars ) {
    
    std::vector <z3::expr> fresh_vars;
    auto var_list = flatten( m_vars );   
    // Create a Map from var to id, var: id
    std::map <z3::expr, int> var_id_map;
   
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
    unsigned int index = 0;
    std::ofstream ofs;
    ofs.open ("myfile.qdimacs", std::ofstream::out | std::ofstream::app);
    // First part of a comment.
    ofs << "c This is a QDIMACS file output \n";

    // Print quantifier and total claused information.     
    auto q_var = var_id_map.size();
    auto num_clause = cnf_fml.size(); // not correct check plz 
    ofs << "p" << " cnf " << q_var << num_clause << "\n";

    for (auto& e: m_vars ){
      if (index == 0){
          if (!e.empty()) {    
            ofs << "e" << " ";
            for (unsigned int i = 0; i < e.size(); i++) {
                ofs << var_id_map ( e.at(i) ) << " ";
            } 
          }
          index = index + 1;
      }
      else {
          // For all quantifier case.
          if( index % 2 == 0 ) { 
              ofs << "e" << " ";
              for (unsigned int i = 0; i < e.size(); i++) {
                  ofs << var_id_map ( e.at(i) ) << " "; 
              } 
          }
          else {  // Exists case.
              ofs << "a" << " ";
              for (unsigned int i = 0; i < e.size(); i++) {
                  ofs << var_id_map ( e.at(i) ) << " "; 
              }
          }
          index = index + 1;
      }
    }

    // Last part of cnf to index output.
    for (auto& e: cnf_fml) {
        z3::func_decl f = e.decl();
        unsigned num = e.num_args();
        if( f.decl_kind() == Z3_OP_OR) {
            for (unsigned i = 0; i < num; i++) {
                // Just for the var and use else to got not check.
                if( e.arg(i).is_var() ) {
                    auto wvar = var_id_map[e.arg(i)];
                    ofs << wvar << " ";
                }
                else {
                  // todo: assert tht it has only one children
                    auto wvar = var_id_map[e.arg(i).arg(0)]; 
                    ofs << "-" << wvar << " "; 
                }
            }
            ofs << "0" << "\n";
        } // close the not case.
        else {
            for (unsigned i = 0; i < num; i++) {
                if( e.arg(i).is_var() ) {
                    auto wvar = var_id_map[e.arg(i)];
                    ofs << wvar;
                }
                else {
                    auto wvar = var_id_map[e.arg(i).arg(0)]; 
                    ofs << "-" << wvar << " "; 
                }
            }
            ofs << "0" << "\n";
        } // close case of only variable  
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

