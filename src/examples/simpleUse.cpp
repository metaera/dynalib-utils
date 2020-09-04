#include "../Utilities/KAML/KAML.h"

using Config = KAML::Manager;

int main(int argc, char* argv[]) {
    try {
        /**
         * The following call to load load and parse the file, either with KAML or JSON, depending on the file extension
         * of ".kaml" or ".json"
         */
        auto& file = Config::loadFile("sample.kaml");
        auto& doc  = file.getDoc();    // First document in root, always added

        // This "settings" node is "const", so it is required to be there, and will except if not
        const auto& settings = doc["settings"];

        // This "hobbies" node is not "const", so it will be added if it is not there
        auto& hobbies  = doc["hobbies"];

        // This "other" node is referenced with () instead of [], so it will return nullptr if not there
        auto* otherPtr = doc("other");

        /**
         * The following series of statements will first print out the hobbies specified, then it will
         * add a new hobby to the node structure, and print out the revised hobby list.  If you
         * comment out the entire hobby list from the file (and make sure to copy the file to the build
         * directory), this will still work because of the line that checks if the hobbies node is
         * a unTyped node, and if it is, it will set the node to be a list.
         * 
         */
        if (hobbies.isList()) {
            for (const auto* hobby : hobbies.list()) {
                cout << "Title: " << (*hobby)["title"]() << ", Years: " << (*hobby)["years"]() << endl;
            }
            cout << endl;
        }
        if (hobbies.isUntyped()) {
            // The "hobbies" node must have been commented out in the file, so let's make it a list.
            // You wouldn't have to do this except in the following section it checks to see if it is
            // a list, and also it gets the list length.
            hobbies.setList();
        }
        if (hobbies.isList()) {
            // Let's add a new hobby to the node structure
            auto len = hobbies.list().count();
            hobbies[len]["title"] = "Basketball";
            hobbies[len]["years"] = 13;

            for (const auto* hobby : hobbies.list()) {
                cout << "Title: " << (*hobby)["title"]() << ", Years: " << (*hobby)["years"]() << endl;
            }
        }

        /**
         * The following shows how you can take any node of the structure and generate JSON from that point.
         */
        if (otherPtr != nullptr) {
            auto& other = *otherPtr;
            cout << "Other: " << Config::getInstance()->genJSON(other, true) << endl;
        }

        cout << "Full: " << Config::getInstance()->genJSON(doc, true) << endl;
    }
    catch (Exception& e) {

    }
    return 0;
}
