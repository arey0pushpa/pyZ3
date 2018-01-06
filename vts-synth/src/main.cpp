#include <stdio.h>
#include <z3++.h>
#include <vector>

#include "z3-util.h"

#include<vts.h>

int main() {

    z3::context c;

    vts  v(c, 2, 2, 1, MODEL_1, 3 );

    z3::model mdl = v.get_vts_for_prob1();

    v.dump_dot("/tmp/vts.dot", mdl );

  try {


    z3::expr t = c.bool_val( true  );
    z3::expr fal = c.bool_val( false );

    z3::expr x = c.bool_const("x");
    z3::expr y = c.bool_const("y");
    z3::expr z = c.bool_const("z");
    z3::expr w = c.bool_const("w");
    // z3::expr f =  x && y;
    z3::expr f = exists( x, forall( z, x || ( z && forall( y, exists( w, implies( y, w) && x && z) )) ) );
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
    VecsExpr qs;
    auto prenex_f = prenex( f, qs );

    std::cout << "Prenexed f : " << prenex_f << "\n";
    std::cout << "Quants :\n";
    for(auto& q : qs ) {
      for( auto& e : q )
        std::cout << e << " ";
      std::cout << "\n";
    }

    auto cnf_f = cnf_converter( prenex_f );

    std::cout << "CNF body :\n";
    for( auto& cl : cnf_f )
        std::cout << cl << "\n";

   // std::cout << "CNF f : " << cnf_f << "\n";
    qdimacs_printer( cnf_f, qs ); 

  }
  
  catch (z3::exception & ex) {
        std::cout << "Nuclear Missles are launched ----> " << ex << "\n";
  }
 
  return 0;
}


