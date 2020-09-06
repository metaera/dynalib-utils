//
// Created by Ken Kopelson on 14/10/17.
//

#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_CONSOLE_WIDTH 300
#include "catch.hpp"

int main(int argc, char* argv[]) {
    int result = Catch::Session().run(argc, argv);
    return result;
}


