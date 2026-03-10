#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/ISISDataDictionary.hpp>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsTagSearch.hpp>

TEST_CASE( "ISISDataDictionary Default Test", "[isis][data][dictionary][default]") {
    psmrts::ISISDataDictionary dict("../psmrts/core/tests/data/IsisPreferences");

    // Block count
    CHECK( dict.size() == 8 );

    // has_group
    CHECK( dict.has_group("UserInterface") );
    CHECK( dict.has_group("DataDirectory") );
    CHECK( dict.has_group("ErrorFacility") );
    CHECK( dict.has_group("SessionLog") );
    CHECK( dict.has_group("CubeCustomization") );
    CHECK( dict.has_group("Performance") );
    CHECK( dict.has_group("Plugins") );
    CHECK_FALSE( dict.has_group("ShapeModel") );   // commented-out in file

    // group() map sizes
    CHECK( dict.group("DataDirectory").size() == 38 );
    CHECK( dict.group("UserInterface").size()  == 10  );
    CHECK( dict.group("ErrorFacility").size()  == 3  );
    CHECK( dict.group("SessionLog").size()     == 4  );
    CHECK( dict.group("CubeCustomization").size() == 4 );
    CHECK( dict.group("Performance").size()    == 2  );
    CHECK( dict.group("Plugins").size()        == 1  );

    // UserInterface values
    CHECK( dict.value("UserInterface", "ProgressBarPercent").value() == "1"         );
    CHECK( dict.value("UserInterface", "ProgressBar").value()        == "On"        );
    CHECK( dict.value("UserInterface", "GuiHelpBrowser").value()     == "open"      );
    CHECK( dict.value("UserInterface", "GuiFontName").value()        == "helvetica" );
    CHECK( dict.value("UserInterface", "GuiFontSize").value()        == "10"        );
    CHECK( dict.value("UserInterface", "GuiWidth").value()           == "460"       );
    CHECK( dict.value("UserInterface", "GuiHeight").value()          == "600"       );
    CHECK( dict.value("UserInterface", "HistoryRecording").value()   == "On"        );
    CHECK( dict.value("UserInterface", "HistoryLength").value()      == "10"        );

    // ErrorFacility values
    CHECK( dict.value("ErrorFacility", "FileLine").value()   == "Off"      );
    CHECK( dict.value("ErrorFacility", "Format").value()     == "Standard" );
    CHECK( dict.value("ErrorFacility", "StackTrace").value() == "Off"      );

    // SessionLog values
    CHECK( dict.value("SessionLog", "TerminalOutput").value() == "Off"       );
    CHECK( dict.value("SessionLog", "FileOutput").value()     == "On"        );
    CHECK( dict.value("SessionLog", "FileName").value()       == "print.prt" );
    CHECK( dict.value("SessionLog", "FileAccess").value()     == "Append"    );

    // CubeCustomization values
    CHECK( dict.value("CubeCustomization", "Overwrite").value()   == "Allow"    );
    CHECK( dict.value("CubeCustomization", "Format").value()      == "Attached" );
    CHECK( dict.value("CubeCustomization", "History").value()     == "On"       );
    CHECK( dict.value("CubeCustomization", "MaximumSize").value() == "200"      );

    // Performance values
    CHECK( dict.value("Performance", "CubeWriteThread").value() == "Optimized" );
    CHECK( dict.value("Performance", "GlobalThreads").value()   == "Optimized" );

    // DataDirectory — spot-check several missions
    CHECK( dict.value("DataDirectory", "ISIS3DATA").value()  == "$ISISDATA"                     );
    CHECK( dict.value("DataDirectory", "Apollo15").value()   == "$ISISDATA/apollo15"             );
    CHECK( dict.value("DataDirectory", "Base").value()       == "$ISISDATA/base"                 );
    CHECK( dict.value("DataDirectory", "Lro").value()        == "$ISISDATA/lro"                  );
    CHECK( dict.value("DataDirectory", "Mro").value()        == "$ISISDATA/mro"                  );
    CHECK( dict.value("DataDirectory", "Clipper").value()    == "$ISISDATA/../datalocal/clipper"  );
    CHECK( dict.value("DataDirectory", "Temporary").value()  == "."                              );

    // Case-insensitive group and key lookups
    CHECK( dict.value("datadirectory", "lro").value()         == "$ISISDATA/lro" );
    CHECK( dict.value("USERINTERFACE", "PROGRESSBAR").value() == "On"            );

    // value_or — key present returns stored value
    CHECK( dict.value_or("SessionLog",    "FileAccess", "Overwrite") == "Append"  );

    // value_or — key absent returns the default
    CHECK( dict.value_or("UserInterface", "GuiStyle",   "default")   == "default" );

    // Missing group / key returns nullopt
    CHECK_FALSE( dict.value("ShapeModel",    "RayTraceEngine").has_value() );
    CHECK_FALSE( dict.value("UserInterface", "NonExistentKey").has_value() );

    // Unknown group name returns empty map without crashing
    CHECK( dict.group("Bogus").empty() );

    // Multi-line continuation (CSMDirectory) is collapsed into one value
    CHECK( dict.value("Plugins", "CSMDirectory")->find("csmplugins") != std::string::npos );   CHECK( dict.value("Plugins", "CSMDirectory")->find("csm3.0.3")   != std::string::npos );

    // operator[] — first block in file order is UserInterface
    CHECK( dict[0].group_name == "UserInterface" );

    // all_blocks() count matches size()
    CHECK( dict.all_blocks().size() == dict.size() );

    // translations() is accessible and translate_path is callable without throwing
    CHECK_NOTHROW( dict.translations().translate_path("$ISISDATA/lro") );
}
