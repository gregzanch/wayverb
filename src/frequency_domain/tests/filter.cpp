#include "frequency_domain/filter.h"
#include "frequency_domain/envelope.h"

#include "audio_file/audio_file.h"

#include "gtest/gtest.h"

#include <random>

TEST(filter, do_nothing) {
    for (const auto &sig :
         {std::vector<float>{0},
          std::vector<float>{1},
          std::vector<float>{1, 0},
          std::vector<float>{1, 0, 0, 0, 0, 0, 0, 0},
          std::vector<float>{1, 0, -1, 0, 1, 0, -1, 0, 1, 0, -1}}) {
        frequency_domain::filter filter{sig.size()};

        auto output{sig};
        fill(output.begin(), output.end(), 0);
        filter.run(sig.begin(), sig.end(), output.begin(), [](auto cplx, auto) {
            return cplx;
        });

        for (auto i{0ul}, end{sig.size()}; i != end; ++i) {
            ASSERT_NEAR(sig[i], output[i], 0.00001);
        }
    }
}

TEST(filter, reduce_magnitude) {
    for (const auto &sig :
         {std::vector<float>{0},
          std::vector<float>{1},
          std::vector<float>{1, 0},
          std::vector<float>{1, 0, 0, 0, 0, 0, 0, 0},
          std::vector<float>{1, 0, -1, 0, 1, 0, -1, 0, 1, 0, -1}}) {
        frequency_domain::filter filter{sig.size()};

        const auto magnitude{0.3f};

        auto output{sig};
        std::fill(output.begin(), output.end(), 0);
        filter.run(
                sig.begin(), sig.end(), output.begin(), [=](auto cplx, auto) {
                    return cplx * magnitude;
                });

        for (auto i{0ul}, end{sig.size()}; i != end; ++i) {
            ASSERT_NEAR(sig[i] * magnitude, output[i], 0.00001);
        }
    }
}

TEST(filter, band_edges) {
    const auto P{0.1};

    const auto b = -P;
    const auto e = P;
    auto divisions = 100;

    for (auto l{0u}; l != 4; ++l) {
        for (auto i = 0; i != divisions; ++i) {
            const auto p = b + (i * (e - b) / divisions);
            const auto lower{frequency_domain::lower_band_edge(p, P, l)};
            const auto upper{frequency_domain::upper_band_edge(p, P, l)};
            ASSERT_NEAR(lower + upper, 1, 0.000001) << p;
        }
    }
}

TEST(filter, lopass) {
    std::default_random_engine engine{std::random_device{}()};
    std::uniform_real_distribution<float> distribution(-1, 1);

    std::vector<float> sig;
    for (auto i{0ul}; i != 44100 * 10; ++i) {
        sig.emplace_back(distribution(engine));
    }

    frequency_domain::filter filter{sig.size()};
    filter.run(sig.begin(), sig.end(), sig.begin(), [](auto cplx, auto freq) {
        return cplx *
               static_cast<float>(frequency_domain::compute_lopass_magnitude(
                       freq, 0.25, 0.05));
    });
    write("lopass_noise.wav",
          sig,
          44100,
          audio_file::format::wav,
          audio_file::bit_depth::pcm16);
}

TEST(filter, hipass) {
    std::default_random_engine engine{std::random_device{}()};
    std::uniform_real_distribution<float> distribution(-1, 1);

    std::vector<float> sig;
    for (auto i{0ul}; i != 44100 * 10; ++i) {
        sig.emplace_back(distribution(engine));
    }

    frequency_domain::filter filter{sig.size()};
    filter.run(sig.begin(), sig.end(), sig.begin(), [](auto cplx, auto freq) {
        return cplx *
               static_cast<float>(frequency_domain::compute_hipass_magnitude(
                       freq, 0.25, 0.05));
    });

    write("hipass_noise.wav",
          sig,
          44100,
          audio_file::format::wav,
          audio_file::bit_depth::pcm16);
}

TEST(filter, transients) {
    std::vector<float> sig(1 << 13);
    sig[1 << 12] = 1.0f;

    frequency_domain::filter filter{sig.size()};
    filter.run(sig.begin(), sig.end(), sig.begin(), [](auto cplx, auto freq) {
        return cplx *
               static_cast<float>(frequency_domain::compute_hipass_magnitude(
                       freq, 0.25, 0.05));
    });

    write_interleaved("transients.wav",
                      sig.data(),
                      sig.size(),
                      1,
                      44100,
                      audio_file::format::wav,
                      audio_file::bit_depth::pcm16);
}
