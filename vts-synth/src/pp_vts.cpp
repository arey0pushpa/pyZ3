#include <string>
#include <iostream>
#include <fstream>
#include <vts.h>
#include <z3-util.h>
#include <vector>
#include <utility>      // std::pair, std::make_pair
#include <map>

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

void create_map ( std::map<std::string,int>& denotation_map, std::vector<std::string> vecElem, std::string& depqbfRun, std::vector< std::pair <int,int> >& xyPair  ) {
    unsigned int step = 0;
    std::ifstream myfile ( "/tmp/out.txt" );
    std::string line;
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
        else if ( step > vecElem.size() ){
          break;
        }
        else {
          std::stringstream  stream(line);
          std::string  word;
          stream >> word; stream >> word;
          int lit = std::stoi( word ); 
          //std::cout << lit << "\n";
          //std::cout << line.at(1) << "\n";
          std::string var = vecElem[step - 1]; 
          denotation_map [ var ] = lit; 
          // Can be avoided by checking only if negative.
          xyPair.push_back( std::make_pair( toDigit (var[2]) , toDigit(var[4]) ) );
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
void vts::print_graph( std::string filename, VecExpr& edgeQuant, bool flagB, bool flagC ) {
    std::string style = "solid";
    std::string color = "blue";
    std::string node_vec; 
    std::vector<std::string> vecElem;
    std::map<std::string,int> denotation_map;
    std::string depqbfRun;
    unsigned int dstep = 0;

    for (auto& var : edgeQuant ) {
      vecElem.push_back( Z3_ast_to_string ( ctx,  var ));
    }

    //for ( auto& var: vecElem ) {
    //  std::cout << var << "\n";
    //}
    
    std::vector< std::pair <int, int> > xyPair;
    
    create_map ( denotation_map, vecElem , depqbfRun, xyPair );

    std::cout << depqbfRun << "\n"; 

    //for (auto& var: denotation_map ) {
    //  std::cout << "<" <<  var.first << "," << var.second  << ">" <<"\n" ;
    //}
    //for (auto& var: xyPair ) {
    //  std::cout << "<" <<  var.first << "," << var.second  << ">" <<"\n" ;
    //}

    if ( flagC == true ) {
      print_denotation_console ( denotation_map );
    }

    if (flagB == true) {
      std::cout << "dumping dot file: " << filename << "\n";

      std::ofstream ofs;
      ofs.open ( filename, std::ofstream::out );

      ofs << "digraph prog {\n";
      for( unsigned i = 0 ; i < N; i++ ) {
        node_vec = std::to_string(i);
        ofs << std::to_string(i) << "[label=\"" << node_vec << "\"]\n";
      }

      for ( auto& val: denotation_map) {
        auto sign = val.second;
        //auto y = pr.second;
        //std::cout << sign << "\n";
        if ( sign >= 0 ) {
          auto x = xyPair[dstep].first; 
          auto y = xyPair[dstep].second; 
          //auto x = pr.first; 
          //std::cout << "Value of x = " << x << "\nValue of y = " << y << "\n";  
          std::string label = "M";
          ofs << std::to_string (x) << "-> " <<  std::to_string (y)
              <<  "[label="  << label << ",color=" << color
              << ",style=" << style << "]\n";
          dstep += 1;
        }
        else {
          dstep += 1;
        }
      }
      ofs << "}\n";
    }
    
} //end function



/* Code of graph printing using C Front end 
if ( line == "1 " ) {
  unsigned int dstep = 0;
  unsigned int x=0, y=0;
  std::string var = vecElem[step]; 
  //std::cout << "The value of var = " << var << "\n"; 
  for( char c : var ) {
    if ( c == '_' || c == 'e' )  {
     continue;
    }
    else {
      //std::cout << "The value of c = " << c << "\n";
      auto val = toDigit(c); 
      //std::cout << "The value of val = " << val << "\n";
      dstep == 0 ? x = val : (dstep == 1 ? y = val: true) ; 
      dstep += 1;
    }
  }
  //std::cout << "Value of x = " << x << "\nValue of y = " << y << "\n";  
  std::string label = "M";
  ofs << std::to_string(x) << "-> " << std::to_string(y)
            <<  "[label="  << label << ",color=" << color
            << ",style=" << style << "]\n";
  step += 1;
}
else { 
  step += 1;
}
*/
