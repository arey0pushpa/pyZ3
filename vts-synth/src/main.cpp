#include <stdio.h>
#include <z3++.h>
#include <vector>

#include "z3-util.h"

int main(int argc, char* argv[]){
  try {
    z3::context c;

    z3::expr t = c.bool_val( true  );
    z3::expr fal = c.bool_val( false );

    z3::expr x = c.bool_const("x");
    z3::expr y = c.bool_const("y");
    z3::expr z = c.bool_const("z");
    z3::expr w = c.bool_const("w");
    // z3::expr f =  z || (x && y );
    z3::expr fml = exists( x, forall( z, x && z && forall( y, exists( w, implies( y, w) && x && z) ) ) );
    //z3::expr f = forall(x, exists( y, !(x && y ) ) ) ;
    //z3::expr f = forall( x, exists( w, w && forall ( y,  x&& y ) ) ) ;
    //z3::expr f = forall( x, exists ( y,  forall (w, exists (z, x && y && w && z)  )) );
    // z3::expr f =  t;
   //z3::expr f = forall( x, forall ( y, exists (z,  z == x||  z == y) )) ; 
    //std::cout << "The sort of the formula f is: " << Z3_get_sort( c, f ) << "\n";
    //exit(0);

//  std::cout << "At least this is working \n.";
    //auto fml_f = negform ( c, f ); 
    //negform ( c, f ); 
    std::vector<z3::expr_vector> qs;
    auto prenex_fml = prenex( fml, qs );

    std::cout << "Prenexed f : " << prenex_fml << "\n";

    for(auto& q : qs ) {
      std::cout << q << "\n";
    }
    auto cnf_fml = cnf_converter(prenex_fml, qs);

    std::cout << "CNF f : " << cnf_fml << "\n";
    qdimacs_printer( cnf_fml );

  }
  
  catch (z3::exception & ex) {
        std::cout << "Nuclear Missles are launched ----> " << ex << "\n";
  }
 
  return 0;
}
