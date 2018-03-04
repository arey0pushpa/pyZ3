#include <stdio.h>
#include <z3++.h>
#include <vector>
#include <stdlib.h>
#include <future>

#include <iostream>
#include <unistd.h>
#include <chrono>
#include <ctime>
//#include <boost>

#include "z3-util.h"
#include <vts.h>

/*
// use Boost to parse command line option
namespace po = boost::program_options;
// Declare the supported options.
po::options_description desc("Allowed options");
desc.add_options()
    ("help", "produce help message")
    ("compression", po::value<std::string>(), "Specify option to run depqbf")
;

po::variables_map vm;
po::store(po::parse_command_line(ac, av, desc), vm);
po::notify(vm);    

if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
}

if (vm.count("use-qdimacs")) {
    cout << "Use Qdimacs File for getting the Model " 
 << vm["use-qdimacs"].as<std::string>() << ".\n";
} else {
    cout << "No Option given.\n";
}

*/

//int main() {
int main(int argc, char** argv) {

    int opt;
    std::string input = "";
    bool flagA = false;
    bool flagB = false;

    // Retrieve the (non-option) argument:
    if ( (argc <= 1) || (argv[argc-1] == NULL) || (argv[argc-1][0] == '-') ) {  // there is NO input...
        std::cerr << "No argument provided!" << std::endl;
        //return 1;
    }
    else {  // there is an input...
        input = argv[argc-1];
    }

    //std::cout << "input = " << input << std::endl;

    // Shut GetOpt error messages down (return '?'): 
    opterr = 0;

    // Retrieve the options:
    while ( (opt = getopt(argc, argv, "qb")) != -1 ) {  // for each option...
        switch ( opt ) {
            case 'q':
                    flagA = true;
                break;
            case 'b':
                    flagB = true;
                break;
            case '?':  // unknown option...
                std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
                break;
        }
    }

    //std::cout << "flagA = " << flagA << std::endl;
    //std::cout << "flagB = " << flagB << std::endl;
 
    //if (flagA == true) 
    z3::context c;
    
    // vts: v [context, Molecule, Nodes, Edge_arity, Version, Connectivity ]
    unsigned int N = 3;
    unsigned int M = 2;
    unsigned int Q = 2;

    vts  v( c, M, N, Q, MODEL_4, 3 );

    //z3::model mdl = v.get_vts_for_prob1();
    //z3::model qbf_mdl = v.get_vts_for_qbf();

    //v.dump_dot("/tmp/vts.dot", mdl );
    //v.dump_dot("/tmp/vts.dot", qbf_mdl );

  try {

    z3::expr t = c.bool_val( true  );
    z3::expr fal = c.bool_val( false );
    VecExpr edgeQuant;
    unsigned int equant_len = N * (N - 1) * Q;
    unsigned int denotation[equant_len];

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
    depqbf_file_creator(edgeQuant, equant_len);

    //bool timedout = false;
    std::future<int> future = std::async(std::launch::async, [](){ 

        auto retVal  = system ("cd ./build/depqbf/examples; ../depqbf --qdo --no-dynamic-nenofex  /tmp/myfile.qdimacs > /tmp/out.txt");
       /* 
        auto retVal = system("cd ./build/depqbf/examples; gcc -o depqbf-file depqbf-file.c -L.. -lqdpll; ./depqbf-file" );
        //std::this_thread::sleep_for(std::chrono::seconds(3));
        */
        return retVal;  
        //system("./src/bash_script.sh");
    }); 
    
    //std::cout << "Running depqbf ... " << "\n";
    std::future_status status;

    status = future.wait_for(std::chrono::seconds(10));

    if ( status == std::future_status::timeout ) { 
      std::cout << "TimeOut! \n";
      exit(0);
      std::terminate();
      return 1;
    }
    if ( status == std::future_status::ready ) 
      std::cout << "Sucess! \n";
    /*
    try {
      depqbf_run_with_timeout (); 
    }
    catch(std::runtime_error& e) {
      std::cout << e.what() << std::endl;
      timedout = true;
      //std::terminate();
      //return 0;
      //exit(0);
    }

    if(!timedout)
      std::cout << "Success" << std::endl;
    //std::terminate();
*/
    // Call Bash script to run depqbf 
    //int systemRet = system("./src/bash_script.sh");
    //if(ret == -1){
    //  std::cout << "SYTEM ERROR !!!\n"; 
    //}
    //system("./src/bash_script.sh");
    
    // Get graph for depqbf-file
    if (flagA == true || flagB == true) { 
      v.print_graph( "/tmp/dep_vts.dot", edgeQuant, denotation); 
      auto retVal = system("xdot /tmp/dep_vts.dot");
      if(retVal == -1) 
        std::cout << "SYTEM ERROR !!!\n"; 
    }

    //std::cout << "\nPrinting depqbf graph at /tmp/dep_vts.dot \n";
  }
  
  catch (z3::exception & ex) {
        std::cout << "Nuclear Missles are launched ----> " << ex << "\n";
  }
 
  return 0;
}


