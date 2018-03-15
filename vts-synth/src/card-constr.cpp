#include<vts.h>
#include<z3-util.h>
//#include <vector>
#include <iterator>

VecExpr vts::flattern_3d ( Vec3Expr& dump ) {
  VecExpr d1;
  for ( unsigned int i = 0; i < N; i++ ) {
    for( unsigned j = 0 ; j < N; j++ ) {
      if (j == i)
        continue;
      for ( unsigned q = 0; q < E_arity; q++ ) {
        d1.push_back( dump[i][j][q] );
     }
    }
  }
  return d1;
}



// Use PbEq for exactly k.
z3::expr vts::exactly_k_drops( unsigned drop_count, Vec3Expr& dump ) { //
  // D2: Flattening the array. Avoid i == j.
  VecExpr d1 = flattern_3d( dump );
  
  // Print the flattern arrar.
  // for ( auto& i: d1 ) {
  //	  std::cout << i  << "\n";
  // }
  // exit(0);

  unsigned int L = d1.size();
  
  // Only have support for exactly 2 and 3.
  if (drop_count == 2) {
    z3::expr al = at_least_two( d1, L ); 
    z3::expr am = at_least_three( d1, L ); 
    return ( al && !am );
  } else if (drop_count == 3) {
    z3::expr al = at_least_three( d1, L ); 
    z3::expr am = at_least_four( d1, L ); 
    return ( al && !am );
 } else { // todo : fill the right code 
    z3::expr al = at_least_two( d1, L ); 
    z3::expr am = at_least_three( d1, L ); 
    return ( al && !am );
 }
  /* ite exactly 3 Implementation.
  z3::expr expr = ctx.int_val(0);
  auto tt = ctx.bool_val(true);
  for (auto& i: d1) {
    expr = expr + z3::ite( i, ctx.int_val(1), ctx.int_val(0) ) ;
  }
  //std::cout << "The total count is : " << expr;  
  return (tt && (expr == ctx.int_val(drop_count)) );
  */
}

/* Different type of Flattern */

z3::expr_vector vts::flattern2d ( Vec2Expr& dump, unsigned s1, unsigned s2, bool eq ) {
  z3::expr_vector d1(ctx);
  for ( unsigned i = 0; i < s1; i++ ) {
    for( unsigned j = 0 ; j < s2; j++ ) {
      if ( eq == true && j == i) continue;
      d1.push_back( dump[i][j] );
    }	    
  }
  return d1;
}

z3::expr_vector vts::flattern3d ( Vec3Expr& dump, unsigned s1, unsigned s2, unsigned s3, bool eq) {
  z3::expr_vector  d1(ctx);
  for ( unsigned i = 0; i < s1; i++ ) {
    for( unsigned j = 0 ; j < s2; j++ ) {
      if ( eq == true && j == i) continue;
      for ( unsigned q = 0; q < s3; q++ ) {
        d1.push_back( dump[i][j][q] );	
      }		       
    }
  }
 return d1;
}

z3::expr_vector vts::flattern4d ( Vec4Expr& dump, unsigned s1, unsigned s2, unsigned s3, unsigned s4, bool eq ) {
  z3::expr_vector  d1(ctx);
  for ( unsigned i = 0; i < s1; i++ ) {
    for( unsigned j = 0 ; j < s2; j++ ) {
      if ( eq == true && j == i) continue;
      for ( unsigned q = 0; q < s3; q++ ) {
        for ( unsigned k = 0; k < s4; k++ ) {
          d1.push_back( dump[i][j][q][k] );	
        }
      }		       
    }
  }
 return d1;
}

// At least 2 
z3::expr vts::at_least_two ( VecExpr dump, unsigned L ) {
  z3::expr_vector ls(ctx);
  for ( unsigned i = 0; i < L-1; i++ ) {
    for ( unsigned j = i+1; j < L; j++) {
      ls.push_back ( dump[i] && dump[j] );  
    }
  }
  return z3::mk_or( ls );
}

// At least 3
z3::expr vts::at_least_three ( VecExpr dump, unsigned L ) {
  z3::expr_vector ls(ctx);
  z3::expr lhs(ctx);
  for ( unsigned i = 0; i < L-2; i++ ) {
    for ( unsigned j = i+1; j < L-1; j++) {
      lhs =  ( dump[i] && dump[j] );  
      for ( unsigned k = j+1; k < L; k++) {
	      ls.push_back ( lhs && dump[k] );
    }
   }
  }
  return z3::mk_or( ls );
}


// At least 4. 
z3::expr vts::at_least_four ( VecExpr dump, unsigned L ) {
  z3::expr_vector ls(ctx);
  z3::expr lhs(ctx);
  z3::expr lhs1(ctx);
  for ( unsigned i = 0; i < L-3; i++ ) {
    for ( unsigned j = i+1; j < L-2; j++) {
      lhs =  ( dump[i] && dump[j] );  
      for ( unsigned k = j+1; k < L-1; k++) {
        lhs1 = (lhs && dump[k]);
        for ( unsigned x = k+1; x < L; x++) {
	        ls.push_back ( lhs1 && dump[x] );
     }
    }
   }
  }
  return z3::mk_or( ls );
}

// At least 4. 
z3::expr vts::at_least_four ( z3::expr_vector dump, unsigned L ) {
  z3::expr_vector ls(ctx);
  z3::expr lhs(ctx);
  z3::expr lhs1(ctx);
  for ( unsigned i = 0; i < L-3; i++ ) {
    for ( unsigned j = i+1; j < L-2; j++) {
      lhs =  ( dump[i] && dump[j] );  
      for ( unsigned k = j+1; k < L-1; k++) {
        lhs1 = (lhs && dump[k]);
        for ( unsigned x = k+1; x < L; x++) {
	        ls.push_back ( lhs1 && dump[x] );
     }
    }
   }
  }
  return z3::mk_or( ls );
}
