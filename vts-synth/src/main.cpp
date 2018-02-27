#include <stdio.h>
#include <z3++.h>
#include <vector>
#include <stdlib.h>

#include "z3-util.h"
#include <vts.h>

int main() {

    z3::context c;
    
    // vts: v [context, Molecule, Nodes, Edge_arity, Version, Connectivity ]
    vts  v( c, 2, 3, 2, MODEL_4, 3 );

    //z3::model mdl = v.get_vts_for_prob1();
    //z3::model qbf_mdl = v.get_vts_for_qbf();

    //v.dump_dot("/tmp/vts.dot", mdl );
    //v.dump_dot("/tmp/vts.dot", qbf_mdl );

  try {

    z3::expr t = c.bool_val( true  );
    z3::expr fal = c.bool_val( false );
    VecExpr edgeQuant;

    z3::expr x = c.bool_const("x");
    z3::expr y = c.bool_const("y");
    z3::expr z = c.bool_const("z");
    z3::expr w = c.bool_const("w");
    z3::expr f = v.get_qbf_formula( edgeQuant );


     
    //std::cout << f << "\n";
   // z3::expr f =  x && y;
   // z3::expr f = exists( x, forall( z, x || ( z && forall( y, exists( w, implies( y, w) && x && z) )) ) );
    //z3::expr f = forall(x, exists( y, !(x && y ) ) ) ;
    //z3::expr f = forall( x, exists( w, w && forall ( y,  x&& y ) ) ) ;
    //z3::expr f = forall( x, exists ( y,  forall (w, exists (z, x && y && w && z)  )) );
    // z3::expr f =  t;
   //z3::expr f = forall( x, forall ( y, exists (z,  z == x ||  z == y) )) ; 
    //std::cout << "The sort of the formula f is: " << Z3_get_sort( c, f ) << "\n";
    //exit(0);

//  std::cout << "At least this is working \n.";
    //auto fml_f = negform ( c, f ); 
    //negform ( c, f ); 
    
    VecsExpr qs;
    auto prenex_f = prenex( f, qs );
    /* Print the formaula in pcnf  Avoid printing now ! */
    //std::cout << "Prenexed f : " << prenex_f << "\n";
    // std::cout << "Quants :\n";
   
   /* Avoid printing now !
    for(auto& q : qs ) {
      for( auto& e : q )
        std::cout << e << " ";
      std::cout << "\n";
    }
*/
    auto cnf_f = cnf_converter( prenex_f );

    /* Avoid printing for now! 
    std::cout << "CNF body :\n";
    for( auto& cl : cnf_f )
        std::cout << cl << "\n";
*/
   // std::cout << "CNF f : " << cnf_f << "\n";
    std::cout << "Printing qdimacs at /tmp/myfile.qdimacs \n";
    qdimacs_printer( cnf_f, qs ); 
    //std::cout << "Creating depqbf input file at /tmp/depqbf.c \n";
    std::cout << "Creating depqbf input file at ./build/depqbf/examples/depqbf.c  \n";
    depqbf_file_creator(edgeQuant);
    
   // Call Bash script to run depqbf 
    int systemRet = system("./src/bash_script.sh");
    if(systemRet == -1){
      std::cout << "SYTEM ERROR !!!\n"; 
    }
    //system("./src/bash_script.sh");
    //run_command();
  }
  
  catch (z3::exception & ex) {
        std::cout << "Nuclear Missles are launched ----> " << ex << "\n";
  }
 
  return 0;
}


