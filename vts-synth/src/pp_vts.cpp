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

#include <boost/algorithm/string.hpp>
 

#define toDigit(c) (c-'0')

// Print the graph with coloured edges ##
//void print_graph( z3::model mdl, unsigned int qval ) {
//  for( unsigned q = 0; q < E_arity; q++ ) {
//    if ( qval == 0 ) {
//	    std::string clr1 = "black";
//	    std::string clr2 = "green";
//	    std::string clr3 = "red";
//    } else {
//	    std::string clr1 = "yellow";
//	    std::string clr2 = "pink";
//	    std::string clr3 = "blue";
//    }
//
//    for( unsigned m = 0; m < M; m++ ) {
//      style = "solid";
//      std::string label = std::to_string( m );
//      if( is_true( presence_edge[i][j][q][m], mdl ) ) {
//	// if( is_true( edges[i][j][q], mdl ) ) {
//        color = clr1;
//	if( is_true( active_edge[i][j][q][m], mdl ) ) {
//	  color = clr2;
//          for( unsigned m1 = 0; m1 < M; m1++ ) {
//            if( is_true( active_node[j][m1], mdl ) and
//                is_true( pairing_m[m][m1], mdl ) ) {
//                color = clr3;
//                break;
//             }
//          }
//        }
//        ofs << std::to_string(i) << "-> " << std::to_string(j)
//            <<  "[label="  << label << ",color=" << color
//            << ",style=" << style << "]\n";
//        }
//     }
//        
//   }
//}

// Printing the Graph ##########
void vts::dump_dot( std::string filename, z3::model mdl) {
  std::string style = "solid";
  std::string color = "blue";
  std::string node_vec;

  std::cout << "dumping dot file: " << filename << "\n";

  std::ofstream ofs;
  ofs.open ( filename, std::ofstream::out );

  ofs << "digraph prog {\n";
  for( unsigned i = 0 ; i < N; i++ ) {
    node_vec = std::to_string(i) + ":";
    for ( unsigned m = 0; m < M; m++ ) {
      if( is_true( nodes[i][m], mdl ) ) {
        node_vec = node_vec + "1";
      }else if ( is_false ( nodes[i][m], mdl ) ) {
        node_vec = node_vec + "0";
      }else{
        node_vec = node_vec + "*";
      }
      if( is_true( active_node[i][m], mdl) ) {
        node_vec = node_vec + "-";
      }
    }
    ofs << std::to_string(i) << "[label=\"" << node_vec << "\"]\n";

    for( unsigned j = 0 ; j < N; j++ ) {
      if (i == j)
        continue;
      for( unsigned q = 0; q < E_arity; q++ ) {
        if ( q == 0 ) {
          if ( is_true(edges[i][j][q], mdl ) ) {
            style = "solid";
            color = "black";

            std::string label = "m";
            ofs << std::to_string(i) << "-> " << std::to_string(j)
              <<  "[label="  << label << ",color=" << color
              << ",style=" << style << "]\n";
          }
        }

        if ( q == 1 ) {
          if ( is_true( edges[i][j][q], mdl ) ) {
            style = "solid";
            color = "yellow";
            std::string label =  "m" ;
            ofs << std::to_string(i) << "-> " << std::to_string(j)
              <<  "[label="  << label << ",color=" << color
              << ",style=" << style << "]\n";
          }
        }	  
      } // end q
    } // end j
  } // end i

  ofs << "}\n";

} //end function


// Get xy 
std::pair<int, int> getxy (std::string var) {
  std::pair<int, int> p2( toDigit(var[2]) , toDigit(var[4]) );
  return p2;
}

// Use Boost for delimiter
std::vector<std::string> get_coordinates ( std::string text ) {
  std::vector<std::string> results;
  boost::split(results, text, [](char c){ return c == '_'; });
  return results;
}


void print_func_node ( unsigned int M, std::vector < std::vector< std::vector <int> > >  tVarStr, int D ) {
  //std::vector < std::vector<int> > node_func_arg( M, std::vector<int>( M ));
  std::vector < std::vector< std::vector <int> > > node_func_arg( M, std::vector< std::vector <int> > ( M, std::vector<int>( D ) ) );
  // Fix values of node_func
  for ( unsigned m = 0; m < M; m++ ) {
    for ( unsigned k = 0; k < M; k++ ) { 
      for ( int d = 0; d < D; d ++ ) { 
        if ( tVarStr[m][k][d] == 1 ) { 
          node_func_arg[m][k][d] = 1;
        } else if ( tVarStr[m][k+M][d] == 1 ) {
          node_func_arg[m][k][d] = -1;
        }
      }
    }
  }
  
  // Printing: todo: check if the sVar needs [M, 2M, D] or [M, D, 2M] 
  for ( unsigned m = 0; m < M; m ++ )  {
    std::string func_val;
    for ( int d = 0; d < D; d ++ ) {
      std::string depth_var;
      for ( unsigned k = 0; k < M; k++ ) {
        auto var = node_func_arg[m][k][d]; 
        if ( var == 1 ) {
          if ( k == M-1 ) { 
            depth_var = depth_var + "arg" + std::to_string (k);
          }
          else { 
            depth_var = depth_var + "arg" + std::to_string (k) + " || ";
          }
        }
        else if ( var == -1 ) {
          if ( k == M-1 ) {
            depth_var = depth_var + "!arg" + std::to_string (k);
          }
          else { 
            depth_var = depth_var + "!arg" + std::to_string (k) + " || ";
          }
        }
      }
      if ( d == D-1 ) 
        func_val = func_val + "[" + depth_var + "]";
      else 
        func_val = func_val + "[" + depth_var + "] && ";
    }
    std::cout <<  "a_fun" + std::to_string (m) << " = " << "func ( " + func_val + " ) \n";
  }
}

void print_denotation_console ( std::map<std::string,int> denotation_map, unsigned M, int D ) {

  std::vector < std::vector< std::vector <int> > > tVarStr( M, std::vector< std::vector <int> > ( 2*M, std::vector<int>( D ) ) );
  std::vector < std::vector< std::vector <int> > > sVarStr( M, std::vector< std::vector <int> > ( 2*M, std::vector<int>( D ) ) );
  //std::vector < std::vector<int> > sVarStr( M, std::vector<int>( 2*M ));
  //  std::vector < std::vector<int> > tVarStr( M, std::vector<int>( 2*M ));
   
  for ( auto& dm : denotation_map ) {
    std::string val;  
    dm.second >= 0 ? val = "True" : val = "False";
    // todo: problem here
    //std::cout << "< " << dm.first << ", " << dm.second << " > \n";
    std::cout << dm.first << " = " << val << "\n";
    if ( dm.first[0] == 't' ) {
      auto coord = get_coordinates( dm.first );
      auto func_mol = std::stoi( coord[1] );
      auto dept_mol = std::stoi( coord[2] );
      auto depth_id = std::stoi( coord[3] );
      if ( dm.second > 0 ) {
        tVarStr[func_mol][dept_mol][depth_id] = 1;
      } else {
        tVarStr[func_mol][dept_mol][depth_id] = -1;
      }
    }else if ( dm.first[0] == 's' ) {
      auto coord = get_coordinates( dm.first );
      auto func_mol = std::stoi( coord[1] );
      auto dept_mol = std::stoi( coord[2] );
      auto depth_id = std::stoi( coord[3] );
      if ( dm.second > 0 ) {
        sVarStr[func_mol][dept_mol][depth_id] = 1;
      } else {
        sVarStr[func_mol][dept_mol][depth_id] = -1;
      }
    }
  }
  
  print_func_node( M, sVarStr, D );
  //print_func_edge( M );
}


void create_map ( z3::context& c, std::map<std::string,int>& denotation_map, std::string& depqbfRun, Tup3Expr& nodeT, Tup3Expr& activeNodeT, Tup3Expr& edgeT, Tup4Expr& presenceEdgeT, Tup4Expr& activeEdgeT, VecsExpr qs  ) {
  unsigned int step = 0;
  std::ifstream myfile ( "/tmp/out.txt" );
  std::string line;

  // Gives list of first level existential quantifier
  auto firstLvlQuant = qs[0];

  /*
     for ( auto& i : firstLvlQuant ) {
     std::cout << i << "\n";
     } */

  if ( myfile ) {
    while (std::getline( myfile, line )) {
      //std::stringstream  stream(line);
      //std::string  word;
      if (step == 0) { 
        int lit = toDigit ( line.at(6) ); 
        if ( lit == 0 ) {
          depqbfRun = "Formula is UNSAT";
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
        //std::cout << denotation_map[ var ] << "\n";

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

// Print depqbf Graph
void vts::print_graph( z3::context& c, std::string filename, 
                       VecsExpr qs, bool printModel, bool displayGraph ) {
  std::string style = "solid";
  std::string color = "blue";
  std::string node_vec; 

  // denotation_map: print the variable assignment
  std::map<std::string,int> denotation_map;
  std::string depqbfRun;

  // xyPair: edge 
  //std::vector< std::pair <int, int> > xyPair;

  // edge tuple
  Tup3Expr edgeT;
  // presence edge tuple
  Tup4Expr presenceEdgeT;
  // active edge tuple
  Tup4Expr activeEdgeT;
  // active node tuple
  Tup3Expr activeNodeT;
  // presence node tuple
  Tup3Expr nodeT;

  create_map ( c, denotation_map, depqbfRun, nodeT, 
                  activeNodeT, edgeT, presenceEdgeT, activeEdgeT, qs);

  std::cout << depqbfRun << "\n"; 

  //for (auto& var: denotation_map ) {
  //  std::cout << "<" <<  var.first << "," << var.second  << ">" <<"\n" ;
  //}
  //for (auto& var: xyPair ) {
  //  std::cout << "<" <<  var.first << "," << var.second  << ">" <<"\n" ;
  //}

  if ( printModel == true ) {
    print_denotation_console ( denotation_map, M, D );
  }

  if ( displayGraph == true ) {
    std::cout << "dumping dot file: " << filename << "\n";

    std::ofstream ofs;
    ofs.open ( filename, std::ofstream::out );

    ofs << "digraph prog {\n";

    unsigned node_id = 0;
    node_vec = std::to_string(0) + ":";

    if ( !activeNodeT.empty() ) { 
      for ( unsigned i = 0; i < activeNodeT.size(); i++ ) {
        unsigned x = std::get<0>( activeNodeT[i] ); 
        //auto y = std::get<1>( activeNodeT[i] ); 
        unsigned z = std::get<2>( activeNodeT[i] ); 

        if ( x != node_id ) {
          ofs << std::to_string(node_id) << "[label=\"" << node_vec << "\"]\n";
          node_vec = std::to_string(x) + ":";
          node_id = x;  
        }

        if ( z == 0 ) {
          if  ( (std::get<2> (nodeT[i]) )  == 0) 
            node_vec = node_vec + "0"; 
          else  
            node_vec = node_vec + "1";
        } else {
          node_vec = node_vec + "1" + "-";
        }
      }
      ofs << std::to_string(node_id) << "[label=\"" << node_vec << "\"]\n";

    } else {
      for( unsigned i = 0 ; i < N; i++ ) {
        node_vec = std::to_string(i);
        ofs << std::to_string(i) << "[label=\"" << node_vec << "\"]\n";
      }
    }

    if ( !activeEdgeT.empty() ) {
      for (auto& i : activeEdgeT ) {
        auto x = std::get<0>( i ); 
        auto y = std::get<1>( i ); 
        auto q = std::get<2>( i ); 
        auto k = std::get<3>( i ); 
        //std::cout << " [ " << x << ", " << y << ", " << q << ", " << k << " ]\n";  

        if ( q == 0 ) {
          color = "green";
        } else {
          color = "pink";
        }

        std::string label = std::to_string(k);

        ofs << std::to_string (x) << "-> " <<  std::to_string (y)
          <<  "[label="  << label << ",color=" << color
          << ",style=" << style << "]\n";

      }
    } 
    if ( !presenceEdgeT.empty() ) {
      for (auto& i : presenceEdgeT ) {
        auto x = std::get<0>( i ); 
        auto y = std::get<1>( i ); 
        auto q = std::get<2>( i ); 
        auto k = std::get<3>( i ); 
        

        if ( std::find( activeEdgeT.begin(), activeEdgeT.end(), i ) != activeEdgeT.end() ) { 
          continue;
        }else {
          if ( q == 0 ) {
            color = "black";
          } else {
            color = "yellow";
          }

          std::string label = std::to_string(k);

          ofs << std::to_string (x) << "-> " <<  std::to_string (y)
            <<  "[label="  << label << ",color=" << color
            << ",style=" << style << "]\n";
        }

      }

    } else {

      //std::cout << "Edge set is : " << edgeT.empty() << "\n";
      if ( !edgeT.empty() ) {
        for (auto& i : edgeT ) {
          auto x = std::get<0>( i ); 
          auto y = std::get<1>( i ); 
          auto z = std::get<2>( i ); 

          //std::cout << z << "\n";

          if ( z == 0 ) {
            color = "red";
          } else {
            color = "blue";
          }

          std::string label = "M";

          ofs << std::to_string (x) << "-> " <<  std::to_string (y)
            <<  "[label="  << label << ",color=" << color
            << ",style=" << style << "]\n";
        }
      }
    }
    ofs << "}\n";
  }

} //end function
