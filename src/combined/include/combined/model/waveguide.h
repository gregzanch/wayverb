#pragma once

#include "combined/model/member.h"

#include "waveguide/simulation_parameters.h"

#include "hrtf/multiband.h"

#include "cereal/types/base_class.hpp"

namespace wayverb {
namespace combined {
namespace model {

class single_band_waveguide final : public basic_member<single_band_waveguide> {
public:
    single_band_waveguide() = default;

    void set_cutoff(double cutoff);
    void set_usable_portion(double usable);

    waveguide::single_band_parameters get() const;

    template <typename Archive>
    void serialize(Archive& archive) {
        archive(data_.cutoff, data_.usable_portion);
    }

private:
    waveguide::single_band_parameters data_{500, 0.6};
};

////////////////////////////////////////////////////////////////////////////////

class multiple_band_waveguide final
        : public basic_member<multiple_band_waveguide> {
public:
    multiple_band_waveguide() = default;

    void set_bands(size_t bands);
    void set_cutoff(double cutoff);
    void set_usable_portion(double usable);

    waveguide::multiple_band_constant_spacing_parameters get() const;

    template <typename Archive>
    void serialize(Archive& archive) {
        archive(data_.bands, data_.cutoff, data_.usable_portion);
    }

private:
    static const frequency_domain::edges_and_width_factor<9> band_params_;

    void maintain_valid_cutoff();

    waveguide::multiple_band_constant_spacing_parameters data_{2, 500, 0.6};
};

////////////////////////////////////////////////////////////////////////////////

class waveguide final : public owning_member<waveguide,
                                             single_band_waveguide,
                                             multiple_band_waveguide> {
public:
    enum class mode { single, multiple };

    void set_mode(mode mode);
    mode get_mode() const;

    double get_sampling_frequency() const;

    template <typename Archive>
    void serialize(Archive& archive) {
        archive(cereal::base_class<base_type>(this), mode_);
    }

    using single_band_t = single_band_waveguide;
    using multiple_band_t = multiple_band_waveguide;

    shared_value<single_band_t>& single_band();
    const shared_value<single_band_t>& single_band() const;

    shared_value<multiple_band_t>& multiple_band();
    const shared_value<multiple_band_t>& multiple_band() const;

private:
    mode mode_ = mode::single;
};

double compute_sampling_frequency(const waveguide& waveguide);

}  // namespace model
}  // namespace combined
}  // namespace wayverb
