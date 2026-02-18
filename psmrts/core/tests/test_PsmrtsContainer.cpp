#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PsmrtsContainer.hpp>
#include <psmrts/core/products/ProductOption.hpp>

/**
  * PSMRTS Container Default Test
  * 
  * Tests PsmrtsContainer functionality utilizing psmrts::ProductOptions objects.
  * 
  * Methods tested:
  *   default constructor - PsmrtsContainer( )
  *   copy constructor    - PsmrtsContainer( const PsmrtsContainer &other )
  *   equals operator     - PsmrtsContainer &operator=( const PsmrtsContainer &other )
  *   name();
  *   size();
  *   add();
  *   replace();
  *   remove();
  *   contains();
  *   find();
  *   begin();
  *   end();
  * 
  */
TEST_CASE( "PSMRTS Container Default Test", "[container][default]") {
  
  // contruct PsmrtsContainer; validate size = 0 and default name = "data"
  psmrts::PsmrtsContainer<psmrts::ProductOption> container;
  CHECK( container.size() == 0 );
  CHECK( container.name() == "data");

  // create ProductOption objects (int double, string, size_t, plus arrays of each type)
  psmrts::ProductOption i( "int", -1 );
  psmrts::ProductOption d( "double", 1.1 );
  psmrts::ProductOption s( "string", "one" );
  psmrts::ProductOption sizet( "size_t", 1 );
  psmrts::ProductOption i_array("ints", {-1, 0, 1, 2, 3} );
  psmrts::ProductOption d_array("doubles", {1.1, 2.2, 3.3} );
  psmrts::ProductOption s_array("strings", {"one","two","three"} );
  psmrts::ProductOption sizet_array("size_ts", {1, 2, 3} );

  // add to container
  container.add(i);
  container.add(d);
  container.add(s);
  container.add(sizet);
  container.add(i_array);
  container.add(d_array);
  container.add(s_array);
  container.add(sizet_array);

  // try to add an element that has already been added,
  // should fail and return false
  CHECK( container.add(i_array) == false );

  // validate container size and contents
  CHECK( container.size() == 8 );
  CHECK( container.contains( "int" ) == true );
  CHECK( container.contains( "double" ) == true );
  CHECK( container.contains( "string" ) == true );
  CHECK( container.contains( "size_t" ) == true );
  CHECK( container.contains( "ints" ) == true );
  CHECK( container.contains( "doubles" ) == true );
  CHECK( container.contains( "strings" ) == true );
  CHECK( container.contains( "size_ts" ) == true );
  CHECK( container.contains( "orangutans" ) == false );

  // try to find element that ISN'T in the container, should throw error
  psmrts::ProductOption findPO;
  CHECK_THROWS( findPO = container.find( "orangutans" ) );

  // try to find element that IS in the container and validate
  findPO = container.find( "size_ts" );
  CHECK( findPO.name() == "size_ts" );

  // remove an element, validate new size
  container.remove("int");
  CHECK( container.size() == 7 );

  // replace entry that exists, should return true if replaced
  psmrts::ProductOption dnew( "double", 2.2 );
  CHECK( container.replace( dnew ) == true );

  // try to replace entry that doesn't exist, should append and return false
  CHECK( container.replace( i ) == false );

  // retrieve/validate container keys
  std::vector<std::string> keys = container.keys();
  CHECK( keys.size() == 8 );
  CHECK( keys[0] == "double" );
  CHECK( keys[1] == "string" );
  CHECK( keys[2] == "size_t" );
  CHECK( keys[3] == "ints" );
  CHECK( keys[4] == "doubles" );
  CHECK( keys[5] == "strings" );
  CHECK( keys[6] == "size_ts" );
  CHECK( keys[7] == "int" );

  // use const iterator to cycle through entries and validate names
  int position = 0;
  for (auto it = container.begin(); it != container.end(); ++it) {
    CHECK( it->name() == container.data()[position].name() );
    position++;
  }

  // create copy of container; validate size and contents
  psmrts::PsmrtsContainer<psmrts::ProductOption> copy( container );
  CHECK( copy.size() == container.size() );
  CHECK( copy.contains( "int" ) == true );
  CHECK( copy.contains( "double" ) == true );
  CHECK( copy.contains( "string" ) == true );
  CHECK( copy.contains( "size_t" ) == true );
  CHECK( copy.contains( "ints" ) == true );
  CHECK( copy.contains( "doubles" ) == true );
  CHECK( copy.contains( "strings" ) == true );
  CHECK( copy.contains( "size_ts" ) == true );

  // create copy of container using equal operator; validate size and contents
  psmrts::PsmrtsContainer<psmrts::ProductOption> copy1 = container;
  CHECK( copy1.size() == container.size() );
  CHECK( copy1.contains( "int" ) == true );
  CHECK( copy1.contains( "double" ) == true );
  CHECK( copy1.contains( "string" ) == true );
  CHECK( copy1.contains( "size_t" ) == true );
  CHECK( copy1.contains( "ints" ) == true );
  CHECK( copy1.contains( "doubles" ) == true );
  CHECK( copy1.contains( "strings" ) == true );
  CHECK( copy1.contains( "size_ts" ) == true );

  // clear containers, validate sizes are 0
  container.clear();
  CHECK( container.size() == 0 );
  copy.clear();
  CHECK( copy.size() == 0 );
  copy1.clear();
  CHECK( copy1.size() == 0 );
}

/**
  * PSMRTS Container Constructors Test
  * 
  * Tests PsmrtsContainer additional constructors.
  * 
  * Construct PsmrtsContainer objects providing ...
  *   1) a string name
  *   2) a std::initializer_list of psmrts::ProductOption objects
  *   3) a std::vector of psmrts::ProductOption objects
  */
TEST_CASE( "PSMRTS Container Constructors Test", "[container][constructors]") {
  
  // contruct PsmrtsContainer; validate size = 0 and given name = "fred"
  psmrts::PsmrtsContainer<psmrts::ProductOption> container1( "fred" );
  CHECK( container1.size() == 0 );
  CHECK( container1.name() == "fred");

  // create ProductOption objects (int double, string, size_t, plus arrays of each type)
  // and add to initializer_list
  psmrts::ProductOption i( "int", -1 );
  psmrts::ProductOption d( "double", 1.1 );
  psmrts::ProductOption s( "string", "one" );
  psmrts::ProductOption sizet( "size_t", 1 );
  psmrts::ProductOption i_array("ints", {-1, 0, 1, 2, 3} );
  psmrts::ProductOption d_array("doubles", {1.1, 2.2, 3.3} );
  psmrts::ProductOption s_array("strings", {"one","two","three"} );
  psmrts::ProductOption sizet_array("size_ts", {1, 2, 3} );

  // construct Container from a std::initializer_list of psmrts::ProductOption objects
  std::initializer_list<psmrts::ProductOption>
      freddyData{i, d, s, sizet, i_array, d_array, s_array, sizet_array};

  psmrts::PsmrtsContainer container2( "freddy", freddyData );

  CHECK( container2.size() == 8 );
  CHECK( container2.name() == "freddy");

  // construct Container from a std::vector of psmrts::ProductOption objects
  std::vector<psmrts::ProductOption>
      fredrickData{i, d, s, sizet, i_array, d_array, s_array, sizet_array};

  psmrts::PsmrtsContainer container3( "frederick", fredrickData );

  CHECK( container3.size() == 8 );
  CHECK( container3.name() == "frederick");

  // clear containers, validate size is 0
  container1.clear();
  CHECK( container1.size() == 0 );

  container2.clear();
  CHECK( container2.size() == 0 );

  container3.clear();
  CHECK( container3.size() == 0 );
}
