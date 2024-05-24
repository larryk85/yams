#include <string>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <yams/base64.hpp>

using namespace yams;

TEST_CASE("Base64 Encoding Tests", "[base64_encoding_tests]") {
   REQUIRE( true == true );
   
   auto encoded = base64::encode("hello");

   std::string expected = "aGVsbG8=";

   REQUIRE( std::string_view(reinterpret_cast<const char*>(encoded.data()), encoded.size()) == expected );

   auto decoded = base64::decode(encoded);

   std::cout << "Decoded: " << decoded << '\n';
}