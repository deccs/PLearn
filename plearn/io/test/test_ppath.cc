#include <plearn/io/PPath.h>
#include <plearn/io/pl_log.h>
#include <plearn/base/stringutils.h>
using namespace PLearn;

#define WIDTH 60    
#define PRINT_TEST(str, __test) \
MAND_LOG\
 << left(str, WIDTH) << flush;\
  try {\
    MAND_LOG << (__test) << endl << endl;\
  }\
  catch(const PLearnError& e)\
  {\
    cerr << "FATAL ERROR: " << e.message() << endl << endl;\
  }\
  catch (...)\
  {\
    cerr << "FATAL ERROR: uncaught unknown exception" << endl << endl;\
  }

#define ASSERT(str, __test) \
MAND_LOG\
 << left(str, WIDTH) << flush;\
  try {\
    bool   __result = (__test);\
    string __success;\
    if ( __result ) __success = "True";\
    else            __success = "False";\
    MAND_LOG << __success << endl << endl;\
  }\
  catch(const PLearnError& e)\
  {\
    cerr << "FATAL ERROR: " << e.message() << endl << endl;\
  }\
  catch (...)\
  {\
    cerr << "FATAL ERROR: uncaught unknown exception" << endl << endl;\
  }


//!< void singleAssert(const string& p)
//!< {
//!<   Path path(p);
//!<   PRINT_TEST( p, p.up() );
//!<   PRINT_TEST( p, p.dirname() );
//!<   PRINT_TEST( p, p.basename() );
//!< }

inline void split_behavior( const string& test,
                            const string& dos,
                            const string& posix  )
{
  int wd = WIDTH / 2;
  string str = left("  DOS: "+dos, wd) + "POSIX: " + posix;
  
  MAND_LOG << test << endl
           << left(str, WIDTH)            << flush;
}

inline string boolstr(bool b)
{
  if ( b ) return "True";
  return "False";
}

void backslashes()
{
  split_behavior( "PPath('./foo//bar\\toto')", 
                  "== 'foo/bar/toto'",
                  "Rejected" );

  bool success = false;

#ifdef WIN32
  success = PPath("./foo//bar\\toto") == "foo/bar/toto";
#else
  try {
    PPath p("./foo//bar\\toto");
  }
  catch(const PLearnError& e)
  {
    success = true;
  }
#endif
  MAND_LOG << boolstr( success ) << endl << endl;
} 



void absolute_path()
{
  string absolute_str;
  string drive;
#ifdef WIN32
  absolute_str = "r:/dorionc"; 
  drive        = "r"; 
#else
  absolute_str = "/home/dorionc";
  drive        = "";
#endif

  PPath absolute( absolute_str );

  split_behavior( "isabs()",                  
                  "r:/dorionc",
                  "/home/dorionc" );
  
  MAND_LOG << boolstr( absolute.isabs() ) << endl << endl;

  split_behavior( "absolute('" + absolute_str + "') == ...",                  
                  "r:/dorionc", "/home/dorionc" );
  
  MAND_LOG << boolstr( absolute == absolute_str ) << endl << endl;

  split_behavior( "absolute('" + absolute_str + "').drive() == ...",                  
                  "r", "" );
  
  MAND_LOG << boolstr( absolute.drive() == drive ) << endl << endl;
}

// Should all be true
void someAsserts()
{
  MAND_LOG << plhead("Asserts") << endl;

  MAND_LOG << plhead("The special dirnames . and ..") << endl;  

  ASSERT( "PPath('./foo/bar') == 'foo/bar'",
               PPath("./foo/bar") == "foo/bar" );
    
  ASSERT( "PPath('foo/./bar') == 'foo/bar'",
               PPath("foo/./bar") == "foo/bar" );

  ASSERT( "PPath('foo/../bar') == 'bar'",
               PPath("foo/../bar") == "bar" );

  ASSERT( "PPath('./foo/bar/../bar/../../foo/./bar') == 'foo/bar'",
               PPath("./foo/bar/../bar/../../foo/./bar") == "foo/bar" );

  PRINT_TEST("PPath('././foo/bar') / PPath('../bar/../../foo/./bar') == 'foo/bar'", "")
  ASSERT( "", PPath("././foo/bar") / PPath("../bar/../../foo/./bar") == "foo/bar" );

  MAND_LOG << plhead("Operators") << endl;
    
  ASSERT( "PPath('') == ''", PPath("") == "" );

  ASSERT( "PPath('foo/bar') / '' == 'foo/bar/'",
           PPath("foo/bar") / "" == "foo/bar/" );  
  
  ASSERT( "PPath('foo/bar') / 'file.cc' == 'foo/bar/file.cc'",
           PPath("foo/bar") / "file.cc" == "foo/bar/file.cc" );  

  ASSERT( "PPath('foo/bar/') / 'file.cc' == 'foo/bar/file.cc'",
           PPath("foo/bar/") / "file.cc" == "foo/bar/file.cc" );  

  MAND_LOG << plhead("Methods up and dirname") << endl;  
  
  PRINT_TEST( "PPath('foo.cc').up()",
               PPath("foo.cc").up() ); // PLERROR
  ASSERT( "PPath('foo.cc').dirname() == ''",
          PPath("foo.cc").dirname() == "" );
    
  ASSERT( "PPath('foo/bar').up() == 'foo'",
          PPath("foo/bar").up() == "foo" );  
  ASSERT( "PPath('foo/bar').dirname() == 'foo'",
          PPath("foo/bar").dirname() == "foo" );
    
  ASSERT( "PPath('foo/bar/').up() == 'foo'",
          PPath("foo/bar/").up() == "foo" );
  ASSERT( "PPath('foo/bar/').dirname() == 'foo/bar'",
          PPath("foo/bar/").dirname() == "foo/bar" );
  
  ASSERT( "PPath('foo/bar/hi.cc').up() == 'foo'",
          PPath("foo/bar/hi.cc").up() == "foo" );  
  ASSERT( "PPath('foo/bar/hi.cc').dirname() == 'foo/bar'",
          PPath("foo/bar/hi.cc").dirname() == "foo/bar" );
}

//!< void relativePathAsserts();
//!< {
//!<   PPath home_ = PPath::home();
//!<   PPath cwd  = PPath::getcwd();

//!<   chdir( home_ );
//!<   PPath ppath = PPath("foo/bar");
//!<   MAND_LOG << PPath ppath = PPath("foo/bar"); << endl;

//!<   PRINT_TEST( "Process current working directory:", PPath::getcwd() );
//!< }

void unitTest(const string& p)
{
  MAND_LOG << plhead(p) << endl;
  
  PPath path(p);
  
  PRINT_TEST(  "path",                   path                   );
  PRINT_TEST(  "path.isAbsPath()",       path.isAbsPath()       );
  PRINT_TEST(  "path.absolute()",        path.absolute()        );
  PRINT_TEST(  "path.canonical()",       path.canonical()       );
                                   
  PRINT_TEST(  "path.protocol()",        path.protocol()        );
  PRINT_TEST(  "path.isFilePath()",      path.isFilePath()      );
  PRINT_TEST(  "path.isHttpPath()",      path.isHttpPath()      );
  PRINT_TEST(  "path.isFtpPath()",       path.isFtpPath()       );
  PRINT_TEST(  "path.addProtocol()",     path.addProtocol()     );  
  PRINT_TEST(  "path.removeProtocol()",  path.removeProtocol()  );
                                   
  PRINT_TEST(  "path.up()",              path.up()              );
  PRINT_TEST(  "path / 'toto' ",         path / "toto"          );
                                   
  PRINT_TEST(  "path.drive()",           path.drive()           );
  PRINT_TEST(  "path.extension()",       path.extension()       );  
  
  MAND_LOG << plsep << endl << endl;
}

int main()
{
  PL_Log::instance().verbosity(VLEVEL_NORMAL);
  PL_Log::instance().outmode( PStream::raw_ascii );

  someAsserts();

  MAND_LOG << plhead("Platform dependant tests.") << endl;

  backslashes();
  absolute_path();
  
// Note that platform dependant tests must not PRINT anything that is
// platform dependant...
//!< #if WIN32
//!<   dosDependant();
//!< #else
//!<   posixDependant();
//!< #endif

  return 0;
}
