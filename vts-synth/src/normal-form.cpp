#include "z3-util.h"
#include <vector>
#include <numeric>
#include <utility>
#include <iterator>
#include <fstream>      // std::ofstream
#include <map>

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

// todo: check type of second argument?
// What is the difference between the Vector of <z3::expr> 
// ND z3::expr_vector
z3::expr_vector visit( std::vector <z3::expr>& cnf_fml, std::map <z3::expr, int>& var_id_map ) {
    
    //std::set<z3::expr> r;   
    z3::expr_vector r;
    
    void collect( z3::exp& e ) {
      if ( e.is_bool() ) {
         z3::func_decl f = e.decl();
         if( f.decl_kind() == Z3_OP_OR) {
            unsigned num = e.num_args();
            for (unsigned i = 0; i < num; i++) {
              collect( e.arg(i) ) 
           }
         }
         
         if( f.decl_kind() == Z3_OP_NOT) {
            collect( e.arg(0) );
          }

         else {
            assert(e.is_var());
            // Check if element present in the list.
            if (var_id_map.count == 0) {
               var_id_map[e] = (var_id_map.size) + 1;
               r.push_back (e);  // check types

            }
         } 
      }
    }
    
    for (z3::expr& e : fml) {
        collect( e );
    }
    return r
}

std::vector<z3::expr_vector> cnf_converter( z3::expr& fml,
                        std::vector<z3::expr_vector>& m_vars ) {
  z3::context& c = fml.ctx();
  z3::solver s(c);
  z3::goal g(c);
  g.add( fml );

   z3::tactic t1(c, "simplify");
   z3::tactic t2(c, "tseitin-cnf");
   z3::tactic t = t1 & t2;
   z3::apply_result r = t(g);
   z3::goal r_g = r[0];

   return r[0];
  // return r[0].as_expr();
}

void qdimacs_printer(std::vector<z3::expr_vector>& cnf_fml,
                        std::vector<z3::expr_vector>& m_var ) {
  // PREPROCESSING ..
      // Flattern the lists of list of quantiofier variables. 
    //auto bar = std::accumulate(m_vars.begin(), m_vars.end(), decltype(m_vars)::value_type{},
      //      [](auto& dest, auto& src) {
       // dest.insert(dest.end(), src.begin(), src.end());
       // return dest;
    //});
    z3::expr_vector fresh_vars(c);
    auto var_list = flatten( m_vars );   
    // Create a Map from var to id, var: id
    std::map <z3::expr, int> var_id_map;
   
    // Map variable to id in the dictionary. 
    unsigned int id = 1;
    for (z3::expr& key: var_list) {  // todo: check the type of key
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

    for (auto& e: m_vars){
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
              ofs << "a" << " ";
              for (unsigned int i = 0; i < e.size(); i++) {
                  ofs << var_id_map ( e.at(i) ) << " "; 
              } 
          }
          else {  // Exists case.
              ofs << "e" << " ";
              for (unsigned int i = 0; i < e.size(); i++) {
                  ofs << var_id_map ( e.at(i) ) << " "; 
              }
          }
          index = index + 1;
      }
    }

    // Last part of cnf to index output.
    for (z3::expr& e: cnf_fml) {
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
                    wvar = var_id_map[e.arg(i).arg(0)]; 
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
                    wvar = var_id_map[e.arg(i).arg(0)]; 
                    ofs << "-" << wvar << " "; 
                }
            }
            ofs << "0" << "\n";
        } // close case of only variable  
      }
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
