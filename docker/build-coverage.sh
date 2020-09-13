cd $1
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ -DCLANG_CODE_COVERAGE=On $2
make
./UnitTests
llvm-profdata merge -sparse default.profraw -o default.profdata
llvm-cov show -format=html -o coverage ./UnitTests -instr-profile=default.profdata
llvm-cov export -format=lcov -instr-profile=default.profdata ./UnitTests > coverage/lcov.info