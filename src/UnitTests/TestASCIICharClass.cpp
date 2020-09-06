//
// Created by Ken Kopelson on 27/12/17.
//

#include <string>
#include <iostream>

using namespace std;

#include "catch.hpp"
#include "Utilities/Parser/ASCIICharClass.h"

SCENARIO("ASCII Character Class functions properly") {

    GIVEN("a newly created Character Class") {
        CharacterClass* charClass = new ASCIICharClass();

        WHEN("a set of identifier characters is added") {
            charClass->setIdentChars(String("_"));
            THEN("all class checks must be valid") {
                CHECK(charClass->isAlphaClass('a'));
                CHECK(charClass->isAlphaClass('m'));
                CHECK(charClass->isAlphaClass('z'));
                CHECK(charClass->isAlphaClass('A'));
                CHECK(charClass->isAlphaClass('M'));
                CHECK(charClass->isAlphaClass('Z'));
                CHECK(charClass->isDigitClass('0'));
                CHECK(charClass->isDigitClass('9'));
                CHECK(charClass->isHexClass('0'));
                CHECK(charClass->isHexClass('9'));
                CHECK(charClass->isHexClass('a'));
                CHECK(charClass->isHexClass('f'));
                CHECK(charClass->isHexClass('A'));
                CHECK(charClass->isHexClass('F'));
                CHECK(charClass->isVowelClass('a'));
                CHECK(charClass->isVowelClass('e'));
                CHECK(charClass->isVowelClass('i'));
                CHECK(charClass->isVowelClass('o'));
                CHECK(charClass->isVowelClass('u'));
                CHECK(charClass->isVowelClass('A'));
                CHECK(charClass->isVowelClass('E'));
                CHECK(charClass->isVowelClass('I'));
                CHECK(charClass->isVowelClass('O'));
                CHECK(charClass->isVowelClass('U'));
                CHECK(charClass->isConsonantClass('B'));
                CHECK(charClass->isConsonantClass('C'));
                CHECK(charClass->isConsonantClass('F'));
                CHECK(charClass->isConsonantClass('G'));
                CHECK(charClass->isConsonantClass('W'));
                CHECK(charClass->isConsonantClass('Z'));
                CHECK(charClass->isIdentClass('_'));
                CHECK(charClass->isIdentClass('a'));
                CHECK(charClass->isIdentClass('z'));
                CHECK(charClass->isIdentClass('A'));
                CHECK(charClass->isIdentClass('Z'));
                CHECK(charClass->isIdentClass('0'));
                CHECK(charClass->isIdentClass('9'));
            }
        }
    }
}
