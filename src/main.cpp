#include "backend.h"

#include "fmt/format.h"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " database_path" << std::endl;
        return EXIT_FAILURE;
    }

    bool didProcessingSucceed = true;

    try {
        cppbackend::Backend backend(argv[1]);

        const auto result = backend.performHandshake(std::cin);
        if (!result.getSuccess()) {
            std::cerr << fmt::format("Processor failed during handshake: '{}'", result.getMessage()) << std::endl;
            return EXIT_FAILURE;
        }
        auto results = backend.readFromInput(std::cin);
        for (const auto &res : results) {
            if (!res.getSuccess()) {
                std::cerr << fmt::format("Processor failed while reading input: '{}'", res.getMessage()) << std::endl;
                didProcessingSucceed = false;
            }
        }
    } catch (std::exception& err) {
        std::cerr << fmt::format("Error in processor: {}", err.what()) << std::endl;
        return EXIT_FAILURE;
    }

    if (!didProcessingSucceed)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
