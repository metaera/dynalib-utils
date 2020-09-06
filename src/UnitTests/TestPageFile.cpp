//
// Created by Ken Kopelson on 03/03/2018
//
#include "catch.hpp"

#include <iostream>
#include "../Utilities/PageFile.h"
using namespace std;

SCENARIO("PageFile operates properly") {
    bool doInit = true;

    GIVEN("a newly created PageFile") {
        if (doInit) {
            /**
             * Do a one-time initialization to delete the test.dat file so we are assured of always starting
             * at the same place in our test.
             */
            PageFile::deleteFile("test.dat");
            doInit = false;
        }
        PageFile*  file   = new PageFile("test.dat", 100, 1000);
        auto*      buffer = new DynaBuffer(file->getPageSize());

        WHEN("we size the file for 100 pages") {
            file->resizeFile(100);
            THEN("its new size is correct") {
                CHECK((file->getPageCount() == 100));
            }
        }
        file->close();
        delete file;
        delete buffer;
    }

    GIVEN("an existing PageFile") {
        PageFile*  file   = new PageFile("test.dat", 100, 1000);
        auto*      buffer = new DynaBuffer(file->getPageSize());
        WHEN("insert 10 pages") {
            for (int i = 0; i < 10; ++i) {
                file->getFreePage(buffer);
            }
            THEN("the pages are present and accounted for") {
                CHECK((file->getPageCount() == 110));
            }
        }
        AND_WHEN("delete 3 pages via page headers") {
            file->deletePage(30);
            file->deletePage(40);
            CHECK_THROWS(file->deletePage(40));
            file->deletePage(50);

            THEN("the pages are gone and accounted for") {
                CHECK((file->getDeletedPageCount() == 3));
                CHECK((file->getValidPageCount() == 107));
                CHECK_THROWS(file->readPage(30, buffer));
                CHECK_THROWS(file->readPage(40, buffer));
                CHECK_THROWS(file->readPage(50, buffer));
            }
        }
        AND_WHEN("insert 5 pages again") {
            for (int i = 0; i < 5; ++i) {
                file->getFreePage(buffer);
            }
            THEN("the pages are present and accounted for") {
                CHECK((file->getDeletedPageCount() == 0));
                CHECK_NOTHROW(file->readPage(30, buffer));
                CHECK_NOTHROW(file->readPage(40, buffer));
                CHECK_NOTHROW(file->readPage(50, buffer));
                CHECK((file->getValidPageCount() == 112));
            }
        }
        file->close();
        delete file;
        delete buffer;
    }
}


