/// This is a short program to find the impulse response of a rectangular
/// waveguide mesh with the maximum permissable courant number.
/// This signal will be written to stdout in the form of a C++ header.
/// You can redirect the output to a file and include it in your program to
/// create transparent waveguide sources.

/// This tool is included mainly so that you can see how the mesh ir is
/// generated, and to give you the option of generating a longer ir if you
/// decide that is necessary.

#include "compensation_signal/waveguide.h"

#include "utilities/progress_bar.h"

#include <iomanip>
#include <iostream>

template <typename It>
void generate_data_file(std::ostream& os, It begin, It end) {
    const auto elements{std::distance(begin, end)};

    os << R"(
//  Autogenerated file  //
#pragma once
#include <array>
const std::array<float, )"
       << elements << "> mesh_impulse_response{{\n";

    for (; begin != end; ++begin) {
        os << std::setprecision(std::numeric_limits<float>::max_digits10)
           << *begin << ",\n";
    }

    os << "}};\n";
}

int main(int argc, char** argv) {
    if (argc != 2) {
        throw std::runtime_error{"Expected a step number."};
    }

    const size_t steps = std::stoi(argv[1]);

    const wayverb::core::compute_context cc{};

    wayverb::waveguide::compressed_rectangular_waveguide waveguide{cc, steps};

    util::progress_bar pb{std::cerr};
    const std::vector<float> sig{0.0f, 1.0f};
    const auto output{
            waveguide.run_hard_source(begin(sig), end(sig), [&](auto step) {
                set_progress(pb, step, steps);
            })};

    generate_data_file(std::cout, output.begin(), output.end());

    return EXIT_SUCCESS;
}
