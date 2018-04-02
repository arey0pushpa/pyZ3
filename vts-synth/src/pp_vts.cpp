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

void print_denotation_console ( std::map<std::string,int> denotation_map ) {
  for (auto& t : denotation_map) {
    std::string val;  
    t.second >= 0 ? val = "True" : val = "False";
    std::cout << t.first << " = " << val << "\n";
  }
}

void create_map ( z3::context& c, std::map<std::string,int>& denotation_map, std::string& depqbfRun, Tup2Expr& nodeT, Tup2Expr& activeNodeT, Tup3Expr& edgeT, Tup4Expr& presenceEdgeT, Tup4Expr& activeEdgeT, VecsExpr qs  ) {
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

        if ( var[0] == 'n' && var[1] == '_' && lit > 0) {
          nodeT.emplace_back( toDigit( var[2] ), toDigit( var[4] ) );
        }

        else if ( var[0] == 'a' && var[1] == '_' && lit > 0) {
          activeNodeT.emplace_back( toDigit( var[2] ), toDigit( var[4] ) );
        }

        else if ( var[0] == 'z' && var[1] == '_' && lit > 0) {
          edgeT.emplace_back( toDigit(var[2]), toDigit(var[4]), toDigit(var[6]) );
        }

        else if ( var[0] == 'e' && var[1] == '_' && lit > 0) {
          presenceEdgeT.emplace_back( toDigit( var[2] ), toDigit( var[4] ), toDigit( var[6] ), toDigit( var[8] ) );
        }

        else if ( var[0] == 'b' && var[1] == '_' && lit > 0) {
          activeEdgeT.emplace_back( toDigit( var[2] ), toDigit( var[4] ), toDigit( var[6] ), toDigit( var[8] ) );
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
void vts::print_graph( z3::context& c, std::string filename, VecsExpr qs, bool printModel, bool displayGraph ) {
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
  Tup2Expr activeNodeT;
  // presence node tuple
  Tup2Expr nodeT;

  create_map ( c, denotation_map, depqbfRun, nodeT, activeNodeT, edgeT, presenceEdgeT, activeEdgeT, qs);

  std::cout << depqbfRun << "\n"; 

  //for (auto& var: denotation_map ) {
  //  std::cout << "<" <<  var.first << "," << var.second  << ">" <<"\n" ;
  //}
  //for (auto& var: xyPair ) {
  //  std::cout << "<" <<  var.first << "," << var.second  << ">" <<"\n" ;
  //}

  if ( printModel == true ) {
    print_denotation_console ( denotation_map );
  }

  if ( displayGraph == true ) {
    std::cout << "dumping dot file: " << filename << "\n";

    std::ofstream ofs;
    ofs.open ( filename, std::ofstream::out );

    ofs << "digraph prog {\n";

    /*
       if ( !activeNodeT.empty() ) {
       for (unsigned i = 0; i < activeNodeT.size(); i++ ) {
       auto x = std::get<0>( activeNodeT(step[i]) ); 
       auto x = std::get<1>( activeNodeT(step[i]) ); 
       std::string label = "M";
       }
       }
       */

    for( unsigned i = 0 ; i < N; i++ ) {
      node_vec = std::to_string(i);
      ofs << std::to_string(i) << "[label=\"" << node_vec << "\"]\n";
    }

    if ( !activeEdgeT.empty() ) {
      for (auto& i : activeEdgeT ) {
        auto x = std::get<0>( i ); 
        auto y = std::get<1>( i ); 
        auto q = std::get<2>( i ); 
        auto k = std::get<3>( i ); 
        
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
    } else if ( !presenceEdgeT.empty() ) {
      for (auto& i : presenceEdgeT ) {
        auto x = std::get<0>( i ); 
        auto y = std::get<1>( i ); 
        auto q = std::get<2>( i ); 
        auto k = std::get<3>( i ); 

        if ( std::find( activeEdgeT.begin(), activeEdgeT.end(), i ) != activeEdgeT.end() ) { 

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
