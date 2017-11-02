Compile:
cmake CMakeLists.txt /*Not always needed, only if cache or install .txt are missing*/
make
./runTests

In order to understand more visit link: https://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/

Using google unit test for c++, that means that my tests are in c++, but the other code is built in c. Nothing changes.
TEST macros are shown in tests.cpp. 

In unit test, i check functions that return something, if a function is void, i may can check unnormal exit value.
Exit(-1) => return value is 255 not -1.

In unit test we are trying to test some cases of addition/delete/search.
