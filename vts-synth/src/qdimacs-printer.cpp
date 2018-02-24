#include <vector>
#include <numeric>
#include <utility>
#include <iterator>
#include <fstream>      
#include <map>

#include <iostream>
#include <sstream>
#include <string>
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
        //std::cout << var_id_map[e];
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
    //ofs.open ("myfile.qdimacs", std::ofstream::out );
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

void add_vars( std::string s, std::ofstream& ofs) {
  std::stringstream iss(s);
  do 
  {
    std::string subs;
    iss >> subs;
    ofs << "qdpll_add (depqbf," << subs << ");\n"; 
    if ( subs == "0" ) {
      ofs << "\n";
      break;
    }
  } while (iss);
}

void depqbf_file_creator() {

  // Begin the printing basic structure
  std::ofstream ofs;
  ofs.open( "/tmp/depqbf.c", std::ofstream::out );

  ofs << "#include <string.h>\n";
  ofs << "#include <stdlib.h>\n";
  ofs << "#include <stdio.h>\n";
  ofs << "#include <assert.h>\n";
  ofs << "#include \"../qdpll.h\"\n\n";

  ofs << "int main (int argc, char** argv) \n";
  ofs << "{ \n";

  ofs << "/* Create solver instance. */\n";
  ofs << "QDPLL *depqbf = qdpll_create ();\n";

  ofs << "/* Use the linear ordering of the quantifier prefix. */\n";
  ofs << "qdpll_configure (depqbf, \"--dep-man=simple\");\n";
  ofs << "/* Enable incremental solving. */\n";
  ofs << "qdpll_configure (depqbf, \"--incremental-use\");\n\n";

  //Open the qdimacs file.
  //std::ifstream infile("/tmp/myfile.qdimacs");
  std::ifstream file( "/tmp/myfile.qdimacs" );
  unsigned int qid = 1;
  unsigned int oquant = 0;
  std::string line;
  std::string outquantvar;
  while( std::getline( file , line ) ) 
  {
    std::string s1 = line.substr(0, line.find(' '));
    //std::cout << "The valuse of s1 is : " << s1 << "\n";
    if ( s1 == "c" || s1 == "p" ) {
      continue;
    }
    if ( s1 == "e" || s1 == "a" ) {
      std::string quant;
      if ( s1 == "e" ) { 
        ofs << "// Add a new leftmost existential quantifier at nested level " << qid << "\n"; 
        ofs << "qdpll_new_scope_at_nesting (depqbf, QDPLL_QTYPE_EXISTS," << qid << ");\n";
        quant = "existential";
        oquant = 5;
      }
      else {
        ofs << "// Add a new leftmost universal quantifier at nested level " << qid << "\n"; 
        ofs << "qdpll_new_scope_at_nesting (depqbf, QDPLL_QTYPE_FORALL," << qid << "); \n";
        quant = "universal";
        oquant = 1;
      }
      // Remove e for the line.
      std::string::size_type n = 0;
      n = line.find_first_of( " \t", n );
      line.erase( 0,  line.find_first_not_of( " \t", n ) );
      outquantvar = line;
      ofs << "/* Add fresh variables to " + quant + " quantifier. \n"; 
      ofs << "\t \t" << line << " */ \n";
      add_vars(line, ofs);
      qid += 1;
    }
    else {
      // We don't care abt outtermost quant variabl...
      ofs << "// Add clause: " << line << "\n"; 
      add_vars(line, ofs);
    }
  }
  // Print "print formula"
  ofs <<  "/* Print formula. */ \n" <<  "qdpll_print (depqbf, stdout);\n";
  ofs << "QDPLLResult res = qdpll_sat (depqbf);\n";
  ofs << "/* Expecting that the formula is satisfiable. */\n";
  ofs << "assert (res == QDPLL_RESULT_SAT);\n";
  ofs << "/* res == 10 means satisfiable, res == 20 means unsatisfiable. */\n";

  ofs << "printf (\"result is: %d\", res);\n";
  ofs << "printf (\"\\n\");\n";
  // Get a countermodel
  if ( oquant == 5 ) {
    // give the assignments to the variables.
    std::stringstream s(outquantvar);
    std::string word;
    std::string var;

    ofs << "\n // Printing the assignments \n";
    for (int i = 0; s >> word; i++) {
      if (word == "0") {
        break;
      }
      var = "a" + std::to_string(i);
      ofs << "QDPLLAssignment " << var << " = qdpll_get_value (depqbf," << word <<");\n";
      ofs << "printf (\"partial model - value of " << word << " : %s\\n\", " << var << " == QDPLL_ASSIGNMENT_UNDEF ? \"undef\" : " << " (" << var << " == QDPLL_ASSIGNMENT_FALSE ? \"false\" : \"true\")); \n\n";
    }
    
    std::cout << outquantvar << "\n";
  }
  else {
    std::cout << "Sorry! OuterMost Quantifier is Not Exists. No Assignments.\n";
  }

  // Delete Solver and end main
  ofs << "/* Delete solver instance. */\n";
  ofs << "qdpll_delete (depqbf);\n";
  ofs << "\n} //End main";

  ofs.close();
}


/*
 Example :
  p cnf 4 2
  e 1 2 3 4 0
  -1  2 0
   2 -3 -4 0

*/

