#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../qdpll.h"

int main (int argc, char** argv)
{
  /* Create a Solver */
  QDPLL *depqbf = qdpll_create ();
  
  /* Linear ordering of the qunatified prefix. */
  qdpll_configure (depqbf,"--dep-man=simple");
  /* Enable increment solving */
  qdpll_configure (depqbf, "--incremental-use");

  /* Add and open a new leftmost universal block at nesting level 1. */
  qdpll_new_scope_at_nesting (depqbf, QDPLL_QTYPE_FORALL, 1);
  
  /* Add a fresh variable with 'id == 1' to the open block. */
  qdpll_add (depqbf, 1);
  /* Close open scope. */
  qdpll_add (depqbf, 0);

  /* Add an existing block at nesting level 2. */
  qdpll_new_scope_at_nesting (depqbf, QDPLL_QTYPE_FORALL, 2);
  /* Add a fresh variable with 'id == 2' to the open block. */
  qdpll_add (depqbf, 2);
  /* Close open scope. */
  qdpll_add (depqbf, 0);
  
  /* Add an existing block at nesting level 3. */
  qdpll_new_scope_at_nesting (depqbf, QDPLL_QTYPE_FORALL, 3);
  /* Add a fresh variable with 'id == 3' to the open block. */
  qdpll_add (depqbf, 3);
  /* Close open scope. */
  qdpll_add (depqbf, 0);
  
  /* Add rule C0 How to do use array var ? need to get how to create var.
   * e(i,j,k) -> a(i,k)*/
  qdpll_add (depqbf, -1);
  qdpll_add (depqbf, 2);
    
  qdpll_print (depqbf, stdout);

  res = qdpll_sat (depqbf);

  assert (res == QDPLL_RESULT_SAT);
  printf ("result is: %d\n": res);

  /* Delete solver instance */
  qdpll_delete (depqbf);
}
