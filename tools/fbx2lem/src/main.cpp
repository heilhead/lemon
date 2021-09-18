//#include <memory>
#include <string>
#include <cxxopts.hpp>
#include <filesystem>
#include <lemon/utils/utils.h>
//#include <folly/experimental/coro/BlockingWait.h>

#include "fbx.h"

cxxopts::ParseResult
initOptions(int argc, char** argv) {
    using namespace cxxopts;

    Options opts("fbx2lem", "Convert FBX models to LEM format");

    auto threadNum = std::to_string(std::thread::hardware_concurrency());

    // clang-format off
    opts.add_options()
        ("i,input", "Input FBX file.", value<std::string>())
        ("o,output", "Output LEM file.", value<std::string>())
        ("h,help", "Print usage.");
    // clang-format on

    auto result = opts.parse(argc, argv);
    if (result.count("help")) {
        lemon::utils::halt(opts.help().c_str(), 0);
    }

    return result;
}

int
main(int argc, char* argv[]) {
    auto opts = initOptions(argc, argv);

    std::filesystem::path inFile;
    if (opts.count("input")) {
        inFile = opts["input"].as<std::string>();
    } else {
        lemon::utils::halt("input file missing");
    }

    std::filesystem::path outFile;

    if (opts.count("output")) {
        outFile = opts["output"].as<std::string>();
    } else {
        outFile = inFile;
        outFile = outFile.replace_extension(".lem");
    }

    lemon::converter::convert(inFile, outFile);

    return 0;
}