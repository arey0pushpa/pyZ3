#include <string>
#include <iostream>
#include <fstream>
#include <vts.h>
#include <z3-util.h>
#include <vector>
#include <utility>      // std::pair, std::make_pair
#include <map>
#include <tuple>
#include <algorithm>

//#include <boost/algorithm/string.hpp>

#define toDigit(c) (c-'0')

// Get xy 
std::pair<int, int> getxy (std::string var) {
  std::pair<int, int> p2( toDigit(var[2]) , toDigit(var[4]) );
  return p2;
}

std::vector<std::string> get_coordinates ( std::string text ) {
  std::vector<std::string> results;
  boost::split(results, text, [](char c){ return c == '_'; });
  return results;
}
/*
// Use Boost for delimiter
std::vector<std::string> vts::get_coordinates ( std::string text ) {
  std::vector<std::string> results;
  boost::split(results, text, [](char c){ return c == '_'; });
  return results;
}
*/
void dVar_update ( unsigned k1, unsigned M, std::string& depth_var, bool lit, bool g ) {
  std::string var;
  if ( lit == true ) 
    var = "arg";
  else 
    var = "!arg";
    
  if ( k1 == M-1 || g == true )
    depth_var = depth_var + var + std::to_string (k1) + " ";
  else 
    depth_var = depth_var + var + std::to_string (k1) + " || ";
}

void picked_var ( unsigned M, unsigned arg3, unsigned k, unsigned m,
                      std::vector < std::vector< std::vector <int> > >& func_arg, std::string& depth_var ) {
  for ( unsigned k1 = 0; k1 < arg3;  k1++ ) {
        if ( k1 == k ) continue;
        int var = func_arg[k][m][k1]; 
        if ( k1 > M ) {
           if ( var != 1 ) 
              continue;
           else {
             if ( k1 == arg3 - 2 ) 
               depth_var = depth_var + "true";
             if ( k1 == arg3 - 1 )
                depth_var = depth_var + "false";
          }
         } else {  
          if ( var == 1 ) 
            dVar_update ( k1, M, depth_var, true, true );
          else if ( var == -1 ) 
            dVar_update ( k1, M, depth_var, false, true );
         } 
      } 
}

void chosen_gates ( unsigned noOfGates, unsigned k, unsigned m, 
                    std::vector < std::vector< std::vector <int> > > wVarStr, std::string& gate_set ) {
  if ( wVarStr[k][m][0] == 1 ) 
    gate_set = gate_set + "AND";
  else if ( wVarStr[k][m][1] == 1 )
    gate_set = gate_set + "OR";
}  


void print_func_gates ( unsigned arg1, unsigned arg2, unsigned arg3, std::vector < std::vector< std::vector <int> > > func_arg, 
                                std::vector < std::vector< std::vector <int> > >  tVarStr,
                                  std::vector < std::vector< std::vector <int> > > uVarStr, bool e ) {
                                    
  for ( unsigned k = 0; k < arg1; k ++ )  {

    std::string f_val;

    unsigned noOfGates = 2;
    
    std::string var_set;    
    std::string gate_set;        
    
    for ( unsigned m = 0; m < arg2; m++ ) {
       if ( m == k ) continue;
       picked_var ( arg1, arg3, k, m, func_arg, var_set );
    } 
    
    for (unsigned g = 0; g < arg1-1; g++ ) {
        if ( e == true )
          chosen_gates ( noOfGates, k, g, tVarStr, gate_set );
        else
          chosen_gates ( noOfGates, k, g, uVarStr, gate_set );
    }
    
    f_val =  "[" + var_set + "]" + " [ " +  gate_set + " ] ";
      
    if ( e != true ) 
      std::cout <<  "a_fun" + std::to_string (k) << " = " << "func ( " + f_val + " ) \n";
    else 
      std::cout <<  "e_fun" + std::to_string (k) << " = " << "func ( " + f_val + " ) \n";
  }
} 


void print_func_cnf ( unsigned M, int D, std::vector < std::vector< std::vector <int> > > func_arg, bool e ) {

 // Printing: todo: check if the sVar needs [M, 2M, D] or [M, D, 2M] 
  for ( unsigned k = 0; k < M; k ++ )  {
    std::string func_val;
    for ( int d = 0; d < D; d ++ ) {
      std::string depth_var;
      for ( unsigned k1 = 0; k1 < M; k1++ ) {
        if ( k1 == k ) continue;
        auto var = func_arg[k][k1][d]; 
        if ( var == 1 ) 
          dVar_update ( k1, M, depth_var, true, false );
        else if ( var == -1 ) 
          dVar_update ( k1, M, depth_var, false, false );
        }
      
      if ( d == D-1 ) 
        func_val = func_val + "[" + depth_var + "]";
      else 
        func_val = func_val + "[" + depth_var + "] && ";
    }
    if ( e != true ) {
     std::cout <<  "a_fun" + std::to_string (k) << " = " << "func ( " + func_val + " ) \n";
    } else {
     std::cout <<  "e_fun" + std::to_string (k) << " = " << "func ( " + func_val + " ) \n";
    } 
  }
}

void final_map ( unsigned arg1, unsigned arg2, unsigned arg3,
                 std::vector < std::vector< std::vector <int> > >& tVarStr,
                 std::vector < std::vector< std::vector <int> > >& func_arg,  
                 bool e , int synthVar ) {
  // [2M -> M] conversion. Now: mapping [M,M,D]
  // Take care of neg case of a var 
  for ( unsigned k = 0; k < arg1; k++ ) {
    for ( unsigned k1 = 0; k1 < arg2; k1++ ) { 
      if (k == k1 ) continue;
      for ( unsigned d = 0; d < arg3; d ++ ) {
        
        if ( synthVar == 4 && d >= arg1 ) {
          if ( tVarStr[k][k1][d] == 1) {
            func_arg[k][k1][d] = 1;
            continue;
          } else {
            func_arg[k][k1][d] = 0;
            continue;
          }
        }
        
        if ( tVarStr[k][k1][d] == 1 ) { 
          func_arg[k][k1][d] = 1;
        } else if ( tVarStr[k][k1+arg1][d] == 1 ) {
          func_arg[k][k1][d] = -1;
        } else {
          func_arg[k][k1][d] = 0;
        }
      }
      
    }
  }
}

/*
void model_map_2 ( std::vector < std::vector <int> >& tVarStr, std::string fst, int snd ) {
  auto coord = get_coordinates( fst );
  
  auto func_mol = std::stoi( coord[1] );
  auto dept_mol = std::stoi( coord[2] );
    if ( snd > 0 ) {
      tVarStr[func_mol][dept_mol] = 1;
    } else {
    // Maping from depqbf model to new variables.
      tVarStr[func_mol][dept_mol] = -1;
    }
}
*/

void model_map_3 ( std::vector < std::vector< std::vector <int> > >& wVarStr, std::string fst, int snd ) {
  
  auto coord = get_coordinates( fst );
  //std::cout << "The passed variable is : " << fst << "\n";
  auto func_mol = std::stoi( coord[1] );
  auto dept_mol = std::stoi( coord[2] );
  auto depth_id = std::stoi( coord[3] );
  
  std::cout << "[" << func_mol << ", " << dept_mol << ", " << depth_id << "] \n"; 
  if ( snd > 0 ) {
    // Maping from depqbf model to new variables.
    // Mapping with < M, 2M, D > 
      wVarStr[func_mol][dept_mol][depth_id] = 1;
  } else {
    //std::cout << fst << "\n";
    //std::cout << snd << "\n";
    // Maping from depqbf model to new variables.
      wVarStr[func_mol][dept_mol][depth_id] = -1;
  }
}
 
void vts::print_denotation_console ( std::map<std::string,int> denotation_map, int synthVar ) {
 
    unsigned typeOfGates = 2;

    std::vector < std::vector< std::vector <int> > > sVarStr( M, std::vector< std::vector <int> > ( 2*M, std::vector<int>( D ) ) );
    std::vector < std::vector< std::vector <int> > > tVarStr( M, std::vector< std::vector <int> > ( 2*M, std::vector<int>( D ) ) );  
    std::vector < std::vector< std::vector <int> > > sGVarStr( M, std::vector< std::vector <int> > ( M, std::vector<int>( 2 * M ) ) );
    std::vector < std::vector< std::vector <int> > > tGVarStr( M, std::vector< std::vector <int> > ( M, std::vector<int>( 2 * M ) ) ); 
    std::vector < std::vector< std::vector <int> > > uVarStr ( M, std::vector< std::vector <int> > ( M-1, std::vector<int>( typeOfGates ) ) );
    std::vector < std::vector< std::vector <int> > > vVarStr ( M, std::vector< std::vector <int> > ( M-1, std::vector<int>( typeOfGates ) ) );
    
    for ( auto& dm : denotation_map ) {
      //std::cout << " dm is : [" << dm.first << ", " << dm.second << "]\n";
      std::string val;  
      dm.second >= 0 ? val = "True" : val = "False";

      std::cout << dm.first << " = " << val << "\n";  

      if ( synthVar == 3 ) {
        if ( dm.first[0] == 't' ) {
          model_map_3 ( tVarStr, dm.first, dm.second );
        } else if ( dm.first[0] == 's' ) {
          model_map_3 ( sVarStr, dm.first, dm.second );
        } 
      }
      
      // In case of synth Var 4 you have to create model-mapping for additioanal var u, v
      if ( synthVar == 4 ) {
        if ( dm.first[0] == 't' ) {
          // Third parameter describes how many co-ord to pullout  
          model_map_3 ( tGVarStr, dm.first, dm.second );
        } else if ( dm.first[0] == 's' ) {
          model_map_3 ( sGVarStr, dm.first, dm.second );
        } else if ( dm.first[0] == 'u' ) {
          model_map_3 ( uVarStr, dm.first, dm.second );
        } else if ( dm.first[0] == 'v' ) {
          model_map_3 ( vVarStr, dm.first, dm.second );
        }
     }
     // std::cout << dm.first << "\n";     
   }
   
    if ( synthVar == 3 ) {
      
      std::vector < std::vector< std::vector <int> > > func_arg_n ( M, std::vector< std::vector <int> > ( M, std::vector<int>( D ) ) );
      final_map ( M, M, D, sVarStr, func_arg_n, false, synthVar );
      print_func_cnf ( M, D, func_arg_n, false );
      
      std::vector < std::vector< std::vector <int> > > func_arg_e ( M, std::vector< std::vector <int> > ( M, std::vector<int>( D ) ) );
      final_map ( M, M, D, tVarStr, func_arg_e, true, synthVar );
      print_func_cnf ( M, D, func_arg_e, true );
    }
    else if ( synthVar == 4 ){

      std::vector < std::vector< std::vector <int> > > func_arg_n ( M, std::vector< std::vector <int> > ( M, std::vector<int>( M + 2 ) ) );
      final_map ( M, M, M+2, sGVarStr, func_arg_n, false, synthVar );
      print_func_gates ( M, M, M+2, func_arg_n, sGVarStr, uVarStr, false );
      
      std::vector < std::vector< std::vector <int> > > func_arg_e ( M, std::vector< std::vector <int> > ( M, std::vector<int>( M + 2 ) ) );
      final_map ( M, M, M+2, tGVarStr, func_arg_e, true, synthVar );
      print_func_gates ( M, M, M+2, func_arg_e, tGVarStr, vVarStr, true );
    }
}

void vts::create_map ( z3::context& c, std::map<std::string,int>& denotation_map, std::string& depqbfRun, Tup3Expr& nodeT, Tup3Expr& activeNodeT, Tup3Expr& edgeT, Tup4Expr& presenceEdgeT, Tup4Expr& activeEdgeT, VecsExpr qs  ) {
  unsigned int step = 0;
  std::ifstream myfile ( "/tmp/out.txt" );
  std::string line;

  // Gives list of first level existential quantifier
  auto firstLvlQuant = qs[0];

  /*
     for ( auto& i : firstLvlQuant ) {
     std::cout << i << "\n";
     }
  */

  if ( myfile ) {
    while (std::getline( myfile, line )) {
      //std::stringstream  stream(line);
      //std::string  word;
      if (step == 0) { 
        int lit = toDigit ( line.at(6) ); 
        if ( lit == 0 ) {
          depqbfRun = "Formula is UNSAT";
          exit(0);
          break;
        }else {
          depqbfRun = "Formula is SAT";
        }
        step += 1;
        continue;
      }
      else {
        std::stringstream  stream(line);
        std::string  word;
        stream >> word; stream >> word;
        int lit = std::stoi( word ); 
        //std::cout << "LINE IS : "  << lit << "\n";
        //std::cout << "AT line : " << line.at(1) << "\n";

        std::string var = Z3_ast_to_string  ( c, firstLvlQuant [step - 1] );
        //std ::cout << "Var is " << var << "\n";

        denotation_map [ var ] = lit; 
        //std::cout << "Denotation map is " << denotation_map[ var ] << "\n";

        //std::cout << "Var2 = " << toDigit(var[2]) << "\t Var4 = " << toDigit( var[4] ) << "\n";
        // Variable is e and denotation(e) is True.

        unsigned x, y, k, q;
        if ( var[0] == 'n' && var[1] == '_' ) {
          unsigned active = 0;
          if (lit > 0) {
            active = 1; 
          } 
          nodeT.emplace_back( toDigit( var[2] ), toDigit( var[4] ), active );
        }

        else if ( var[0] == 'a' && var[1] == '_' ) {
          unsigned active = 0;
          if (lit > 0) {
            active = 1; 
          } 
          activeNodeT.emplace_back( toDigit( var[2] ), toDigit( var[4] ), active );
        }

        else if ( var[0] == 'z' && var[1] == '_' && lit > 0 ) {
          edgeT.emplace_back( toDigit(var[2]), toDigit(var[4]), toDigit(var[6]) );
        }

        else if ( var[0] == 'e' && var[1] == '_' && lit > 0 ) {
          auto coord = get_coordinates( var );
          x = std::stoi( coord[1] );
          y = std::stoi( coord[2] );
          q = std::stoi( coord[3] );
          k = std::stoi( coord[4] );
          presenceEdgeT.emplace_back( x, y, q, k );
        }

        else if ( var[0] == 'b' && var[1] == '_' && lit > 0 ) {
          auto coord = get_coordinates( var );
          x = std::stoi( coord[1] );
          y = std::stoi( coord[2] );
          q = std::stoi( coord[3] );
          k = std::stoi( coord[4] );
          activeEdgeT.emplace_back( x, y, q, k );
        }

        step += 1;
      }
    }

    myfile.close();
  }else {
    std::cout << "No file named out.txt in temp folder... \n" << std::endl;
  }
  myfile.close();
}
