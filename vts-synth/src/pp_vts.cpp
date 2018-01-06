#include <string>
#include <iostream>
#include <vts.h>
#include <z3-util.h>

// Printing the Graph ##########
void vts::dump_dot( string filename, z3::model mdl) {
    std::string style = "solid"; 
    std::string color = "blue";
    std::string node_vec;
  
    std::cout << "dumping dot file: " << filename;
    ofs.open ( filename, std::ofstream::out );
    
    of << "digraph prog {\n";
    for( unsigned i = 0 ; i < N; i++ ) {
        node_vec = to_string(i) + ":";
        for ( unsigned m = 0; m < M; m++ ) {
            if is_true( nodes[i][m], mdl ) {
                node_vec = node_vec + "1";
            }
            else if ( is_false ( nodes[i][m], mdl ) {
                node_vec = node_vec + "0";
            }
            if is_true( active_node[i][m], mdl) {
                node_vec = node_vec + "-";
            }
        }
        ofs << to_string(i) << "\n";
        
        for( unsigned j = 0 ; j < M; j++ ) {
            if (i == j)
                continue;
            for( unsigned q = 0; q < E_arity; q++ ) {
                style = "solid";
                if is_true( edges[i][j][q]) {
                    color = "black";
                   // if is_true( dump2[i][j][q] ) {
                   //     color = "red";
                    //}
                    ofs << to_string(i) << "-> " << to_string(j) <<  "[color=" << color  << "]\n";
    ofs << "}\n";
                }
            }
        }
    }

} //end function
