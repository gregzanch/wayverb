#pragma once

#include "common/aligned/vector.h"
#include "common/hrtf_utils.h"
#include "waveguide/rectangular_waveguide.h"

#include "glm/glm.hpp"

namespace waveguide {
namespace attenuator {

class hrtf final {
public:
    aligned::vector<aligned::vector<float>> process(
            const aligned::vector<rectangular_waveguide::run_step_output>&
                    input,
            const glm::vec3& direction,
            const glm::vec3& up,
            HrtfChannel channel) const;
};

}  // namespace attenuator
}  // namespace waveguide
