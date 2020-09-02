# Using Graph Nodes

The following code uses the macros provided in Graph/Node.h.  These macros will test if a variable is provided in the Graph Node structure (that results from calling KAMLParser or JSONParser), and if the value is provided it will set the value into the variable specified in the 3rd parameter.

```
auto& doc = rootNode[0];
const auto* settingsPtr = doc("settings");
const auto* configPtr   = doc("config");
const auto* pluginsPtr  = doc("plugins");

NODE_GET_INIT;
NODE_GET_STR(doc, "description", descText);
if (descText != "") {
    _logger->info("Loaded: %s", descText.c_str());
}
// Settings
if (settingsPtr !=  nullptr) {
    const auto& settings = *settingsPtr;
    bool newLog2con = log2con;
    NODE_GET_BOOL(settings, "log2con",     newLog2con);
    if (newLog2con != log2con) {
        log2con = newLog2con;
        MainController::getInstance()->configureLogging();
    }
    NODE_GET_BOOL(settings, "noprompt",        noprompt);
    NODE_GET_STR(settings,  "data_path",       dataPath);
    NODE_GET_STR(settings,  "lob_subpath",     lobSubPath);
    NODE_GET_STR(settings,  "ext_volume",      externalVolume);
    NODE_GET_STR(settings,  "ext_data_path",   externalDataPath);
    NODE_GET_STR(settings,  "ext_lob_subpath", externalLobSubPath);
    NODE_GET_STR(settings,  "ip_addr",         webServAddr);
    NODE_GET_INT(settings,  "port",            webServPort);

    auto* cache = Mind::getInstance()->getMemory()->getCache();
    auto maxThingPages = cache->getThingFileMaxCachePages();
    auto maxRefPages   = cache->getRefFileMaxCachePages();
    NODE_GET_INT(settings, "max_thing_pages", maxThingPages);
    NODE_GET_INT(settings, "max_ref_pages",   maxRefPages);
    cache->setThingFileMaxCachePages(maxThingPages);
    cache->setRefFileMaxCachePages(maxRefPages);

    auto thingPurgePerc = cache->getThingFilePurgePercent();
    auto refPurgePerc   = cache->getRefFilePurgePercent();
    NODE_GET_FLOAT(settings, "thing_purge_perc", thingPurgePerc);
    NODE_GET_FLOAT(settings, "ref_purge_perc",   refPurgePerc);
    cache->setThingFilePurgePercent(thingPurgePerc);
    cache->setRefFilePurgePercent(refPurgePerc);
}

// Config
if (configPtr != nullptr) {
    const auto& config = *configPtr;
    NODE_GET_STR(config, "required_path",   configPathRequired);
    NODE_GET_STR(config, "auto_path",       configPathAuto);
}

bool requiredWasLoaded = false;

// Plugins
if (pluginsPtr != nullptr) {
    const auto& plugins = *pluginsPtr;
    NODE_GET_STR(plugins, "required_path",   pluginsPathRequired);
    NODE_GET_STR(plugins, "auto_path",       pluginsPathAuto);
    auto* pluginMgr = PluginManager::getInstance();

    auto* requiredPluginsPtr = plugins("required");
    if (requiredPluginsPtr != nullptr) {
        auto& requiredPlugins = *requiredPluginsPtr;
        if (ProcessConfigFile::loadPluginsSpecifiedInConfig(requiredPlugins, pluginsPathRequired, LogLevel::ERROR)) {
            requiredWasLoaded = true;
        }
    }
    if (doRequired && !requiredWasLoaded) {
        _logger->fatal("Required plugins failed to load...aborting...");
        MainController::gracefulAbort();
    }

    auto* autoPluginsPtr = plugins("auto");
    if (autoPluginsPtr !=  nullptr) {
        auto& autoPlugins = *autoPluginsPtr;
        if (!ProcessConfigFile::loadPluginsSpecifiedInConfig(autoPlugins, pluginsPathAuto, LogLevel::INFO)) {
        }
    }
}

```

Here is an example of using the Graph Node features without the macros shown above.

There are 3 ways to access a node using overloaded operators.  First you have the "const" with brackets:

```const auto& doc2 = config[1]```

This requires that the node exist, and if it does not, an exception will be thrown.

The second way is without the "const":

```auto& doc2 = config[1]```

In this case if the node exists it will be returned, but if it does not exist, a new node will be added and then returned.

Finally, there is the method of using parentheses instead of brackets:

```auto* doc2 = config(1);```

This method returns a pointer instead of a reference, and if the node exists it will be returned, otherwise nullptr will be returned.  This allows your code to test if a node is present.

Below is some example code showing how to access nodes, and particularly how to add new nodes programmatically.  Using the () at the end of a node accessing/updating expression returns the value of the node as a String.  Also, "jsonising" a String converts embedded control characters (like /n and /r) into actual string equivalents, thereby allowing that String to be in a JSON object.  The JSONParser knows how to convert those back to the real control character when you parse the JSON text into the Graph Nodes.

```
auto& doc2   = config[1];
auto& test   = doc2["test"];

String str = config[0][2]();
if (test("elem1a") != nullptr) {
    long val = test["elem1a"].asInt();
    cout << "Integer1:   " << val << endl;
}
cout << "String1:    " << test["elem7"]["sub5"]() << endl;
cout << "String2:    " << test["elem7"]["sub6"]() << endl;
cout << "String3:    " << test["elem7"]["sub7"]() << endl;
cout << "String4:    " << endl << test["elem9_1"]["c2"].asCString();    // Returns a "C" string
cout << "elem10.1.1: " << test["elem10"][1][1]() << endl;

auto& record = test["user"]["record"];
record["name"]               = "Ken Kopelson";
record["age"]                = 45;
record["category"]           = 'T';
record["rate"]               = 45.67;
record["otherName1"]         = String("Boris Johnson");
record["otherName2"]         = "James Johnson";
record["citizen"]            = true;
record["auxData"]            = nullptr;
record["history"][0]["date"] = Time("2020-06-01T06:00:00");
record["history"][1]["date"] = Timestamp("2020-06-01T06:00:00");

auto testStr = test["elem9_1"]["c2"]();
JSONGenerator::jsonise(testStr);
cout << "jsonised:   " << testStr.c_str() << endl;
JSONGenerator::unjsonise(testStr);
cout << "unjsonised: " << testStr.c_str() << endl;

auto   gen    = JSONGenerator();
String outStr = gen.generateJson(config, true);
cout << outStr << endl;
outStr = gen.generateJson(config);
cout << outStr << endl;

auto* jsonRoot = Node::newNullNode();
if (kaml.parseJSON(outStr, jsonRoot)) {
    outStr = gen.generateJson(*jsonRoot);
    cout << endl << "From JSON:" << endl << outStr << endl << endl;
}

```