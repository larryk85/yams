# ##################################################################################################
# Define the main test executable.
# ##################################################################################################
add_executable( 
   yams_unit_tests 
   yams.cpp
   base64_tests.cpp
)

target_link_libraries( yams_unit_tests PRIVATE yams::yams Catch2::Catch2WithMain )
catch_discover_tests(yams_unit_tests)
