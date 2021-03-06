/*
 *  Created by Phil on 13/5/2013.
 *  Copyright 2014 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"
#include "internal/catch_test_spec_parser.h"
#include "internal/catch_config.hpp"
#include "internal/catch_commandline.h"

#ifdef __clang__
#   pragma clang diagnostic ignored "-Wc++98-compat"
#endif

inline Catch::TestCase fakeTestCase(const char* name, const char* desc = "") { return Catch::makeTestCase(nullptr, "", { name, desc }, CATCH_INTERNAL_LINEINFO); }

TEST_CASE( "Parse test names and tags" ) {

    using Catch::parseTestSpec;
    using Catch::TestSpec;

    Catch::TestCase tcA = fakeTestCase( "a" );
    Catch::TestCase tcB = fakeTestCase( "b", "[one][x]" );
    Catch::TestCase tcC = fakeTestCase( "longer name with spaces", "[two][three][.][x]" );
    Catch::TestCase tcD = fakeTestCase( "zlonger name with spacesz" );

    SECTION( "Empty test spec should have no filters" ) {
        TestSpec spec;
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
    }

    SECTION( "Test spec from empty string should have no filters" ) {
        TestSpec spec = parseTestSpec( "" );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches(tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
    }

    SECTION( "Test spec from just a comma should have no filters" ) {
        TestSpec spec = parseTestSpec( "," );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
    }

    SECTION( "Test spec from name should have one filter" ) {
        TestSpec spec = parseTestSpec( "b" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
    }

    SECTION( "Test spec from quoted name should have one filter" ) {
        TestSpec spec = parseTestSpec( "\"b\"" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
    }

    SECTION( "Test spec from name should have one filter" ) {
        TestSpec spec = parseTestSpec( "b" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == false );
    }

    SECTION( "Wildcard at the start" ) {
        TestSpec spec = parseTestSpec( "*spaces" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
        CHECK( parseTestSpec( "*a" ).matches( tcA ) == true );
    }
    SECTION( "Wildcard at the end" ) {
        TestSpec spec = parseTestSpec( "long*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
        CHECK( parseTestSpec( "a*" ).matches( tcA ) == true );
    }
    SECTION( "Wildcard at both ends" ) {
        TestSpec spec = parseTestSpec( "*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == true );
        CHECK( parseTestSpec( "*a*" ).matches( tcA ) == true );
    }
    SECTION( "Redundant wildcard at the start" ) {
        TestSpec spec = parseTestSpec( "*a" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == false );
    }
    SECTION( "Redundant wildcard at the end" ) {
        TestSpec spec = parseTestSpec( "a*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == false );
    }
    SECTION( "Redundant wildcard at both ends" ) {
        TestSpec spec = parseTestSpec( "*a*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == false );
    }
    SECTION( "Wildcard at both ends, redundant at start" ) {
        TestSpec spec = parseTestSpec( "*longer*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == true );
    }
    SECTION( "Just wildcard" ) {
        TestSpec spec = parseTestSpec( "*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == true );
    }

    SECTION( "Single tag" ) {
        TestSpec spec = parseTestSpec( "[one]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == false );
    }
    SECTION( "Single tag, two matches" ) {
        TestSpec spec = parseTestSpec( "[x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == true );
    }
    SECTION( "Two tags" ) {
        TestSpec spec = parseTestSpec( "[two][x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
    }
    SECTION( "Two tags, spare separated" ) {
        TestSpec spec = parseTestSpec( "[two] [x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
    }
    SECTION( "Wildcarded name and tag" ) {
        TestSpec spec = parseTestSpec( "*name*[x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "Single tag exclusion" ) {
        TestSpec spec = parseTestSpec( "~[one]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
    }
    SECTION( "One tag exclusion and one tag inclusion" ) {
        TestSpec spec = parseTestSpec( "~[two][x]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == false );
    }
    SECTION( "One tag exclusion and one wldcarded name inclusion" ) {
        TestSpec spec = parseTestSpec( "~[two]*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == true );
    }
    SECTION( "One tag exclusion, using exclude:, and one wldcarded name inclusion" ) {
        TestSpec spec = parseTestSpec( "exclude:[two]*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == true );
    }
    SECTION( "name exclusion" ) {
        TestSpec spec = parseTestSpec( "~b" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == true );
    }
    SECTION( "wildcarded name exclusion" ) {
        TestSpec spec = parseTestSpec( "~*name*" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "wildcarded name exclusion with tag inclusion" ) {
        TestSpec spec = parseTestSpec( "~*name*,[three]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "wildcarded name exclusion, using exclude:, with tag inclusion" ) {
        TestSpec spec = parseTestSpec( "exclude:*name*,[three]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == true );
        CHECK( spec.matches( tcB ) == true );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "two wildcarded names" ) {
        TestSpec spec = parseTestSpec( "\"longer*\"\"*spaces\"" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == true );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "empty tag" ) {
        TestSpec spec = parseTestSpec( "[]" );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "empty quoted name" ) {
        TestSpec spec = parseTestSpec( "\"\"" );
        CHECK( spec.hasFilters() == false );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == false );
    }
    SECTION( "quoted string followed by tag exclusion" ) {
        TestSpec spec = parseTestSpec( "\"*name*\"~[.]" );
        CHECK( spec.hasFilters() == true );
        CHECK( spec.matches( tcA ) == false );
        CHECK( spec.matches( tcB ) == false );
        CHECK( spec.matches( tcC ) == false );
        CHECK( spec.matches( tcD ) == true );
    }

}

TEST_CASE( "Process can be configured on command line", "[config][command-line]" ) {

#ifndef CATCH_CONFIG_DISABLE_MATCHERS
    using namespace Catch::Matchers;
#endif

    Catch::ConfigData config;
    auto cli = Catch::makeCommandLineParser(config);

    SECTION("empty args don't cause a crash") {
        auto result = cli.parse({""});
        CHECK(result);
        CHECK(config.processName == "");
    }


    SECTION("default - no arguments") {
        auto result = cli.parse({"test"});
        CHECK(result);
        CHECK(config.processName == "test");
        CHECK(config.shouldDebugBreak == false);
        CHECK(config.abortAfter == -1);
        CHECK(config.noThrow == false);
        CHECK(config.reporterNames.empty());

        Catch::Config cfg(config);
        CHECK_FALSE(cfg.hasTestFilters());
    }

    SECTION("test lists") {
        SECTION("1 test", "Specify one test case using") {
            auto result = cli.parse({"test", "test1"});
            CHECK(result);

            Catch::Config cfg(config);
            REQUIRE(cfg.hasTestFilters());
            REQUIRE(cfg.testSpec().matches(fakeTestCase("notIncluded")) == false);
            REQUIRE(cfg.testSpec().matches(fakeTestCase("test1")));
        }
        SECTION("Specify one test case exclusion using exclude:") {
            auto result = cli.parse({"test", "exclude:test1"});
            CHECK(result);

            Catch::Config cfg(config);
            REQUIRE(cfg.hasTestFilters());
            REQUIRE(cfg.testSpec().matches(fakeTestCase("test1")) == false);
            REQUIRE(cfg.testSpec().matches(fakeTestCase("alwaysIncluded")));
        }

        SECTION("Specify one test case exclusion using ~") {
            auto result = cli.parse({"test", "~test1"});
            CHECK(result);

            Catch::Config cfg(config);
            REQUIRE(cfg.hasTestFilters());
            REQUIRE(cfg.testSpec().matches(fakeTestCase("test1")) == false);
            REQUIRE(cfg.testSpec().matches(fakeTestCase("alwaysIncluded")));
        }

    }

    SECTION("reporter") {
        SECTION("-r/console") {
            CHECK(cli.parse({"test", "-r", "console"}));

            REQUIRE(config.reporterNames[0] == "console");
        }
        SECTION("-r/xml") {
            CHECK(cli.parse({"test", "-r", "xml"}));

            REQUIRE(config.reporterNames[0] == "xml");
        }
        SECTION("-r xml and junit") {
            CHECK(cli.parse({"test", "-r", "xml", "-r", "junit"}));

            REQUIRE(config.reporterNames.size() == 2);
            REQUIRE(config.reporterNames[0] == "xml");
            REQUIRE(config.reporterNames[1] == "junit");
        }
        SECTION("--reporter/junit") {
            CHECK(cli.parse({"test", "--reporter", "junit"}));

            REQUIRE(config.reporterNames[0] == "junit");
        }
    }


    SECTION("debugger") {
        SECTION("-b") {
            CHECK(cli.parse({"test", "-b"}));

            REQUIRE(config.shouldDebugBreak == true);
        }
        SECTION("--break") {
            CHECK(cli.parse({"test", "--break"}));

            REQUIRE(config.shouldDebugBreak);
        }
    }


    SECTION("abort") {
        SECTION("-a aborts after first failure") {
            CHECK(cli.parse({"test", "-a"}));

            REQUIRE(config.abortAfter == 1);
        }
        SECTION("-x 2 aborts after two failures") {
            CHECK(cli.parse({"test", "-x", "2"}));

            REQUIRE(config.abortAfter == 2);
        }
        SECTION("-x must be numeric") {
            auto result = cli.parse({"test", "-x", "oops"});
            CHECK(!result);

#ifndef CATCH_CONFIG_DISABLE_MATCHERS
            REQUIRE_THAT(result.errorMessage(), Contains("convert") && Contains("oops"));
#endif
        }
    }

    SECTION("nothrow") {
        SECTION("-e") {
            CHECK(cli.parse({"test", "-e"}));

            REQUIRE(config.noThrow);
        }
        SECTION("--nothrow") {
            CHECK(cli.parse({"test", "--nothrow"}));

            REQUIRE(config.noThrow);
        }
    }

    SECTION("output filename") {
        SECTION("-o filename") {
            CHECK(cli.parse({"test", "-o", "filename.ext"}));

            REQUIRE(config.outputFilename == "filename.ext");
        }
        SECTION("--out") {
            CHECK(cli.parse({"test", "--out", "filename.ext"}));

            REQUIRE(config.outputFilename == "filename.ext");
        }
    }

    SECTION("combinations") {
        SECTION("Single character flags can be combined") {
            CHECK(cli.parse({"test", "-abe"}));

            CHECK(config.abortAfter == 1);
            CHECK(config.shouldDebugBreak);
            CHECK(config.noThrow == true);
        }
    }


    SECTION( "use-colour") {

        using Catch::UseColour;

        SECTION( "without option" ) {
            CHECK(cli.parse({"test"}));

            REQUIRE( config.useColour == UseColour::Auto );
        }

        SECTION( "auto" ) {
            CHECK(cli.parse({"test", "--use-colour", "auto"}));

            REQUIRE( config.useColour == UseColour::Auto );
        }

        SECTION( "yes" ) {
            CHECK(cli.parse({"test", "--use-colour", "yes"}));

            REQUIRE( config.useColour == UseColour::Yes );
        }

        SECTION( "no" ) {
            CHECK(cli.parse({"test", "--use-colour", "no"}));

            REQUIRE( config.useColour == UseColour::No );
        }

        SECTION( "error" ) {
            auto result = cli.parse({"test", "--use-colour", "wrong"});
            CHECK( !result );
#ifndef CATCH_CONFIG_DISABLE_MATCHERS
            CHECK_THAT( result.errorMessage(), Contains( "colour mode must be one of" ) );
#endif
        }
    }
}
