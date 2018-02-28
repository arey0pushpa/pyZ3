#include <string>
#include <iostream>
#include <fstream>
#include <vts.h>
#include <z3-util.h>
#include <vector>

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

// Helper function to assign values to edge varables
void preprocess ( VecExpr edgeQuant ) {
  //for (auto& var : edgeQuant ) {
    //unsigned int step = 0;
  //  unsigned int i=0, j=0, q=0, val=0;
//    for( char c : var ) {
//       if ( c == "_" || c == "e" )  {
//         continue;
//       }
//       else {
//         val = int(c); 
//         step == 0 ? i = val : (step == 1? j = val : q = val); 
//         step += 1;
//       }
//    }
    // Define interpretation
    //edge[i][j][k] = interpretation [var];
 // }
}

// Print depqbf Graph
void vts::print_graph( std::string filename, VecExpr& edgeQuant, unsigned int denotation[] ) {
    std::string style = "solid";
    std::string color = "blue";
    std::string node_vec; 
    std::vector<std::string> vecElem;
    unsigned int step = 0;

    std::cout << "dumping dot file: " << filename << "\n";

    std::ofstream ofs;
    ofs.open ( filename, std::ofstream::out );

    ofs << "digraph prog {\n";
    for( unsigned i = 0 ; i < N; i++ ) {
      node_vec = std::to_string(i);
      //node_vec = node_vec ;
      ofs << std::to_string(i) << "[label=\"" << node_vec << "\"]\n";
    }
    
    for (auto& var : edgeQuant ) {
      vecElem.push_back( Z3_ast_to_string ( ctx,  var ));
    }

   //for (unsigned int i = 0; i < vecElem.size(); i++ ) {
    //  std::cout << vecElem[i] << "\n";
   // }

    std::ifstream myfile ( "/tmp/out.txt" );
    std::string line;
    if ( myfile ) {
      while (std::getline( myfile, line )) {
        //std::cout << "The current denotation is " << line << "\n";
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
      }
      
      myfile.close();
    }
    else {
      std::cout << "No file named out.txt in temp folder... \n" << std::endl;
    }

    ofs << "}\n";
    
} //end function
