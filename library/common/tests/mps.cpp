#include "common/hilbert.h"

#include "frequency_domain/fft.h"

#include "gtest/gtest.h"

#include <cmath>

TEST(mps, arbitrary_spectrum) {
    std::vector<std::complex<float>> spec{
            {1, 0}, {1, 0}, {1, 0}, {0, 0}, {0, 0}, {0, 0}, {1, 0}, {1, 0}};
    const auto minimum_phase_spectrum{mps(spec)};

    frequency_domain::dft_1d ifft{
            frequency_domain::dft_1d::direction::backwards, spec.size()};

    const auto i_spec{run(ifft, spec.data(), spec.data() + spec.size())};
    const auto i_fft_spec{
            run(ifft,
                minimum_phase_spectrum.data(),
                minimum_phase_spectrum.data() + minimum_phase_spectrum.size())};
}

TEST(mps, front_dirac) {
    const std::vector<float> sig{1, 0, 0, 0, 0, 0, 0, 0};

    frequency_domain::dft_1d fft{frequency_domain::dft_1d::direction::forwards,
                                 sig.size()};
    auto spec{run(fft, sig.data(), sig.data() + sig.size())};

    for (auto i : spec) {
        ASSERT_EQ(i.real(), 1);
        ASSERT_EQ(i.imag(), 0);
    }

    const auto minimum_phase_spectrum{mps(spec)};

    ASSERT_EQ(spec.size(), minimum_phase_spectrum.size());

    for (auto i{0u}; i != minimum_phase_spectrum.size(); ++i) {
        ASSERT_EQ(spec[i], minimum_phase_spectrum[i]);
    }
}

TEST(mps, zero_spectrum) {
    std::vector<std::complex<float>> spec{
            {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
    ASSERT_THROW(mps(spec), std::runtime_error);
}

std::vector<std::complex<float>> round_trip(
        std::vector<std::complex<float>> sig) {
    frequency_domain::dft_1d fft{frequency_domain::dft_1d::direction::forwards,
                                 sig.size()};

    const auto spec{run(fft, sig.data(), sig.data() + sig.size())};
    const auto min_phase{mps(spec)};

    frequency_domain::dft_1d ifft{
            frequency_domain::dft_1d::direction::backwards, min_phase.size()};

    auto ret{run(ifft, min_phase.data(), min_phase.data() + min_phase.size())};
    for (auto& i : ret) {
        i /= ret.size();
    }
    return ret;
}

TEST(mps, round_trip) {
    const std::vector<std::complex<float>> sig{0, 1, 0, 0, 0, 0, 0, 0};

    const auto t0{round_trip(sig)};
    const auto t1{round_trip(t0)};

    ASSERT_EQ(t0, t1);
}

TEST(mps, arbitrary_dirac) {
    const auto test_sig{[](const auto& sig) {
        frequency_domain::dft_1d fft{
                frequency_domain::dft_1d::direction::forwards, sig.size()};
        auto spec{run(fft, sig.data(), sig.data() + sig.size())};
        const auto minimum_phase_spectrum{mps(spec)};
        ASSERT_EQ(spec.size(), minimum_phase_spectrum.size());
        for (auto i : minimum_phase_spectrum) {
            ASSERT_NEAR(std::abs(i), 1, 0.00001);
        }
        frequency_domain::dft_1d ifft{
                frequency_domain::dft_1d::direction::backwards,
                minimum_phase_spectrum.size()};
        auto synth{run(
                ifft,
                minimum_phase_spectrum.data(),
                minimum_phase_spectrum.data() + minimum_phase_spectrum.size())};
        for (auto& i : synth) {
            i /= synth.size();
        }
    }};

    test_sig(std::vector<float>{0, 1, 0, 0, 0, 0, 0, 0});
    test_sig(std::vector<float>{0, 1, 0, 0, 0, 0, 0});
    test_sig(std::vector<float>{0, 0, 1, 0, 0, 0, 0, 0});
    test_sig(std::vector<float>{0, 0, 1, 0, 0, 0, 0});
    test_sig(std::vector<float>{0, 0, 0, 1, 0, 0, 0, 0});
    test_sig(std::vector<float>{0, 0, 0, 1, 0, 0, 0});
    test_sig(std::vector<float>{0, 0, 0, 0, 1, 0, 0, 0});
    test_sig(std::vector<float>{0, 0, 0, 0, 1, 0, 0});
    test_sig(std::vector<float>{0, 0, 0, 0, 0, 1, 0, 0});
    test_sig(std::vector<float>{0, 0, 0, 0, 0, 1, 0});
    test_sig(std::vector<float>{0, 0, 0, 0, 0, 0, 1, 0});
    test_sig(std::vector<float>{0, 0, 0, 0, 0, 0, 1});
    test_sig(std::vector<float>{0, 0, 0, 0, 0, 0, 0, 1});
}

TEST(mps, sin) {
    const std::vector<float> sig{0, 1, 0, -1, 0, 1, 0, -1};
    frequency_domain::dft_1d fft{frequency_domain::dft_1d::direction::forwards,
                                 sig.size()};
    auto spec{run(fft, sig.data(), sig.data() + sig.size())};
    for (auto i : spec) {
        std::cout << i << ", ";
    }
    std::cout << '\n';

    const auto minimum_phase_spectrum{mps(spec)};
    for (auto i : minimum_phase_spectrum) {
        std::cout << i << ", ";
    }
    std::cout << '\n';

    frequency_domain::dft_1d ifft{
            frequency_domain::dft_1d::direction::backwards,
            minimum_phase_spectrum.size()};
    auto synth{
            run(ifft,
                minimum_phase_spectrum.data(),
                minimum_phase_spectrum.data() + minimum_phase_spectrum.size())};
    for (auto& i : synth) {
        i /= synth.size();
    }

    for (auto i : synth) {
        std::cout << i << ", ";
    }
    std::cout << '\n';
}