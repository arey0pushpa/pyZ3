#ifndef VTS_UTILS_H
#define VTS_UTILS_H

#include <string>
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <ostream>
#include <sstream>

class vts_exception : public std::runtime_error
{
public:
  vts_exception(const char* what) : runtime_error(what) {}
  vts_exception(const std::string what) : runtime_error(what.c_str()) {}
};

// everntually remove EXTERNAL_VERSION
// should be NDEBUG == EXTERNAL_VERSION??
// #ifndef NDEBUG
#ifdef EXTERNAL_VERSION
#define triggered_at ""
#else
#define triggered_at " (triggered at " <<  __FILE__ << ", " << __LINE__ << ")"
#endif

#define issue_warning( ss )  { std::cerr << ss.str() << "\n";}

#ifndef NDEBUG
#define issue_error( ss )  { std::cerr << ss.str() << "\n"; assert( false );}
#else
#define issue_error( ss )  { throw vts_exception( ss.str() ); }
#endif


#define vts_error( M, S ) { std::stringstream ss;                   \
    ss << "# vts " << M << " Error - " << S                         \
       << triggered_at << std::endl;                                  \
    issue_error( ss ); }


#define vts_warning( M, S ) { std::stringstream ss;                   \
    ss << "# vts " << M << " Warning - " << S                         \
       << std::endl;                                                    \
    issue_warning( ss );  }


#endif // VTS_UTILS_H
