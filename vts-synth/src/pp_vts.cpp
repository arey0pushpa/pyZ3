#include <string>
#include <iostream>
#include <fstream>
#include <vts.h>
#include <z3-util.h>

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
          for( unsigned m = 0; m < M; m++ ) {
            style = "solid";
            std::string label = std::to_string( m );
            if( is_true( presence_edge[i][j][q][m], mdl ) ) {
            // if( is_true( edges[i][j][q], mdl ) ) {
              color = "black";
              //if( is_true( active_edge[i][j][q][m], mdl ) ) {
               // color = "green";
		if ( is_true( drop1[i][j][q], mdl ) ) {
			color = "red";
		}
                //for( unsigned m1 = 0; m1 < M; m1++ ) {
                //  if( is_true( active_node[j][m1], mdl ) and
                 //     is_true( pairing_m[m][m1], mdl ) ) {
                  //  color = "red";

                   // break;
                 // }
                //}
             // }
              ofs << std::to_string(i) << "-> " << std::to_string(j)
                  <<  "[label="  << label << ",color=" << color
                  << ",style=" << style << "]\n";
            }
          }
        
	  }

	if ( q == 1 ) {
          for( unsigned m = 0; m < M; m++ ) {
            style = "solid";
            std::string label = std::to_string( m );
            if( is_true( presence_edge[i][j][q][m], mdl ) ) {
            // if( is_true( edges[i][j][q], mdl ) ) {
              color = "yellow";
             // if( is_true( active_edge[i][j][q][m], mdl ) ) {
              //  color = "pink";
		if ( is_true( drop1[i][j][q], mdl ) ) {
			color = "blue";
		}
               // for( unsigned m1 = 0; m1 < M; m1++ ) {
                //  if( is_true( active_node[j][m1], mdl ) and
                 //     is_true( pairing_m[m][m1], mdl ) ) {
                  //  color = "blue";
                  //  break;
                 // }
               // }
             // }
              ofs << std::to_string(i) << "-> " << std::to_string(j)
                  <<  "[label="  << label << ",color=" << color
                  << ",style=" << style << "]\n";
            }
          }
        }	  
	  } // end q
      } // end j
    } // end i

    ofs << "}\n";

} //end function
