#ifndef VTS_LOAD_H
#define VTS_LOAD_H

#include <fstream>

#include "utils.h"
#include "vts.h"

#define load_error( M, Line ) vts_error( "", "line " << Line << ":"<< M )

class load_vts {

private:
  z3::context& c;
  std::string file_name;
  std::ifstream in;
  vts_ptr v = nullptr;
  unsigned line_num = 0;
  bool initialized_vts = false;

  //model parameters
  unsigned int  M = 0;
  unsigned int  N = 0;
  unsigned int  Q = 0;
  model_version V = UN_INIT;
  unsigned int  D = 0;
  std::vector< std::string > mol_vars;

  char peek_skip_space();
  unsigned read_num_line();
  char read_char_line();
  std::string read_string_line();
  char get_command();
  void get_label( std::vector<unsigned>& mols,
                            std::vector<bool>& activity);
  void get_mol_num();
  void get_node_num();
  void get_max_edge_num();
  void get_model_version();

  void get_node(z3::expr_vector& knownNodes);
  void get_edge(z3::expr_vector& knownEdges, z3::expr_vector& knownPresenceEdges, z3::expr_vector& knownActiveEdges);
  void get_pairing(z3::expr_vector& knownPairingMatrix);
  void get_node_function();
  void get_edge_function();
public:
  load_vts( z3::context& c_, std::string file_name_ )
    : c(c_)
    , file_name(file_name_) {}
    
//  void load();
  void load(  z3::expr_vector& knownNodes,  z3::expr_vector& knownActiveNodes,  z3::expr_vector& knownEdges,
                                                 z3::expr_vector&  knownPresenceEdges,  z3::expr_vector& knownActiveEdges, z3::expr_vector& knownPairingMatrix ) ;
  inline vts_ptr get_vts() { return v; };
};

#endif