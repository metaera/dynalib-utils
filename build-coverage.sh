if [[ ! -d "build" ]]; then
    mkdir build
fi
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++ -DCLANG_CODE_COVERAGE=On ..
make
./UnitTests
llvm-profdata merge -sparse default.profraw -o default.profdata
llvm-cov show -format=html -o coverage ./UnitTests -instr-profile=default.profdata
llvm-cov export -format=lcov -instr-profile=default.profdata ./UnitTests > coverage/lcov.info