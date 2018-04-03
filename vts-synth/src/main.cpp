#include <stdio.h>
#include <z3++.h>
#include <vector>
#include <stdlib.h>
#include <future>

#include <iostream>
#include <fstream>

#include <unistd.h>
#include <chrono>
#include <ctime>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>
using namespace boost;
using namespace boost::program_options;

#include <exception>

#include "z3-util.h"
#include <vts.h>

int main(int ac, char* av[])
{
  try {
    int funcType = -1;
    int synthVar = -1;
    bool useZ3 = false;
    bool displayGraph = false;
    bool printModel = false;

    options_description general("General options");
    general.add_options()
      ("help", "produce a help message")
    //  ("help-module", value<std::string>(),
    //   "produce a help for a given module")
    //  ("version", "output the version number")
      ;

    /*
    options_description gui("GUI options");
    gui.add_options()
      ("display", value<std::string>(), "display to use")
      ;
   */
    options_description variation("Variation options");
    variation.add_options()
      ("func-type", value<int>(), "fix a function type for QBF")
      ("synth-var", value<int>(), "fix a synthesis variation for vts")
      ;
    
    options_description options("Available options");
    options.add_options()
      ("use-z3", "use z3 for QBF solving")
      ("print-model", "print vts model")
      ("display-graph", "display the vts as graph")
      ;

    // Declare an options description instance which will include
    // all the options
    options_description all("Allowed options");
    //all.add(general).add(gui).add(variation);
    all.add(general).add(variation).add(options);

    // Declare an options description instance which will be shown
    // to the user
    options_description visible("VTS-Synth [version 0.0.1]. (C) Copyright 2017-2018 TIFR Mumbai. \nUsage: ./vts-synth [--options] [--variation arg] \n\nFunction types:\n  0. Arbitrary Boolean func: ackermannization. [default] \n  1. K-cnf with depth D. \n  2. Logic-gates AND OR. \n  3. Logic gate with unique arguments. \n\nSynthesis variation:\n  0. Default. \n  1. Edge synthesis.\n  2. Molecule synthesis.\n  3. K-Cnf.\n  4. Logic gates.\n  5. Activate-deactivate");
    visible.add(general).add(variation).add(options);


    variables_map vm;
    store(parse_command_line(ac, av, all), vm);

    if (vm.count("help")) 
    {
      std::cout << visible;
      return 0;
    }
    if (vm.count("help-module")) {
      const std::string& s = vm["help-module"].as<std::string>();
      //if (s == "gui") {
      //  std::cout << gui;
     // } else
      if (s == "variation") {
        std::cout << variation;
      } else if (s == "options") {
        std::cout << options;
      }else {
        std::cout << "Unknown module '" 
          << s << "' in the --help-module option\n";
        return 1;
      }
      return 0;
    }

    if (vm.count("func-type")) {
      std::cout << "The 'function type' chosen is: "
        << vm["func-type"].as<int>() << "\n";            
      funcType  =  vm["func-type"].as<int>();            
    }                           
    
    if (vm.count("synth-var")) {
      std::cout << "The 'synthesis variation' chosen is: "
        << vm["synth-var"].as<int>() << "\n";            
      synthVar  =  vm["synth-var"].as<int>();            
    }                           
    if (vm.count("use-z3")) {
      //std::cout << "The 'use-Z3' options was set to "
      //  << vm["use-z3"].as<unsigned>() << "\n";            
      useZ3 = true;            
    }                           
    if (vm.count("display-graph")) {
      //std::cout << "The 'display' options was set to "
      //  << vm["display-graph"].as<unsigned>() << "\n";            
      displayGraph =  true;            
    }                           
    if (vm.count("print-model")) {
      //std::cout << "The 'print model' options was set to "
      //  << vm["print-model"].as<unsigned>() << "\n";            
      //printModel =  vm["print-model"].as<unsigned>();            
      printModel =  true;            
    }                           
  
  //std::cout << useZ3;
  //std::cout << vm["func-model"].as<int>() << "\n";            
  //exit(0);

  z3::context c;

  // vts: v [context, Molecule, Nodes, Edge_arity, Version, Connectivity, Cnf_depth ]
  unsigned int N = 2;
  unsigned int M = 6;
  unsigned int Q = 2;
  // depth of cnf
  unsigned int D = 2;

  vts  v( c, M, N, Q, MODEL_3, 3, D );

  //z3::model mdl = v.get_vts_for_prob1();
  //z3::model qbf_mdl = v.get_vts_for_qbf();

  //v.dump_dot("/tmp/vts.dot", mdl );
  //v.dump_dot("/tmp/vts.dot", qbf_mdl );

    z3::expr t = c.bool_val( true );
    z3::expr fal = c.bool_val( false );
    z3::expr x = c.bool_const("x");
    z3::expr y = c.bool_const("y");
    z3::expr z = c.bool_const("z");
    z3::expr w = c.bool_const("w");
      
    // represent z3 vts formula
    z3::expr f( c );
      
    if( funcType != -1 ) {
      f = v.create_qbf_formula( funcType );
    } else if( synthVar != -1 ) {
      f = v.vts_synthesis( synthVar );
    }else {
      f = v.create_qbf_formula( 0 );
    }
    //std::cout << f << "\n";


    /* First Order Formula to test basic functionality  */
    //std::cout << f << "\n";
    // z3::expr f = exists( x, forall( z, x || ( z && forall( y, exists( w, implies( y, w) && x && z) )) ) );
    //z3::expr f = forall(x, exists( y, !(x && y ) ) ) ;
    //z3::expr f = forall( x, exists( w, w && forall ( y,  x&& y ) ) ) ;
    //z3::expr f = forall( x, exists ( y,  forall (w, exists (z, x && y && w && z)  )) );
    //z3::expr f = forall( x, forall ( y, exists (z,  z == x ||  z == y) )) ; 
    //std::cout << "The sort of the formula f is: " << Z3_get_sort( c, f ) << "\n";

    // UNSAT Check: sat, Unsat check passed
    //z3::expr f = forall ( x, exists (y, (x || !y) && (!x || y) && (x || y) ));   

    // Losing example : QDIMACS working fine
    //z3::expr f = exists (x, exists (y, exists (z, forall(w, exists (ww, ( (!x || ww) && (y || w || !ww) && (z || !w || !ww) && (!y || !z) ))))));  

    //auto fml_f = negform ( c, f ); 
    //negform ( c, f ); 

    /* Run Z3 home made QBF solver or DepQbf */
    if ( useZ3 == true ) { 
      v.use_z3_qbf_solver( f );
    }
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
    //depqbf_file_creator(edgeQuant, equant_len);

    //bool timedout = false;
    std::future<int> future = std::async(std::launch::async, [](){ 
        // if ( flagA == false ) {
        auto retVal  = system ("cd ./build/depqbf/examples;timeout 20s ../depqbf --qdo --no-dynamic-nenofex  /tmp/myfile.qdimacs > /tmp/out.txt");
        // std::cout << retVal << "\n";
        //  } else {
        // auto retVal = system("cd ./build/depqbf/examples; gcc -o depqbf-file depqbf-file.c -L.. -lqdpll; ./depqbf-file" );
        //    }
        return retVal;  
        //system("./src/bash_script.sh");
        }); 

    //std::cout << "Running depqbf ... " << "\n";
    std::future_status status;

    status = future.wait_for( std::chrono::seconds(20) );

    if ( status == std::future_status::timeout ) { 
      std::cout << "TimeOut! \n";
      //future.join();
      exit(0);
      std::terminate();
      return 1;
    }
    if ( status == std::future_status::ready ) { 
      std::cout << "Program run was sucessful! ";
    }

    v.print_graph( c, "/tmp/dep_vts.dot", qs, printModel, displayGraph ); 
    if ( displayGraph == true ) { 
      auto retVal = system("xdot /tmp/dep_vts.dot");
      if(retVal == -1) 
        std::cout << "SYTEM ERROR !!!\n"; 
    }

    //  }
    /*
       else {
       std::ifstream myfile ( "/tmp/out.txt" );
       std::string line;
       if ( myfile ) {
       std::string line;
       auto firstLine = std::getline( myfile, line );
    //firstLine[1]  == "1" ? std::cout << "THE FORMULA IS SAT" << "\n" : std::cout << "THE FORMULA IS UNSAT" << "\n";
    std::cout << firstLine << "\n";
    }
    */

    //std::cout << "\nPrinting depqbf graph at /tmp/dep_vts.dot \n";
    // }

}

catch (z3::exception & ex) {
std::cout << "Nuclear Missles are launched ----> " << ex << "\n";
}

return 0;
}


