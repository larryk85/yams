#include <string>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <yams/yams.hpp>
#include <yams/compile_time_string.hpp>

using namespace yams;

template <auto T>
struct foo {
   static void print_t() {
      std::cout << std::string_view(T.data(), T.size()) << '\n';
   }
};

TEST_CASE("Test", "[test_tests]") {
   std::cout << "hello\n";
   REQUIRE( true == true );

   constexpr auto s = ct_str<5>("hello");

   foo<s>::print_t();
}