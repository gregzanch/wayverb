#pragma once

/*
#include "core/model/receiver.h"
#include "core/orientable.h"

#include <vector>

namespace model {

struct SingleShot {
    float filter_frequency;
    float oversample_ratio;
    float speed_of_sound;
    size_t rays;
    glm::vec3 source;
    receiver receiver;
};

struct App {
    float filter_frequency{500};
    float oversample_ratio{2};
    float speed_of_sound{340};
    size_t rays{100000};
    util::aligned::vector<glm::vec3> source{glm::vec3{0}};
    util::aligned::vector<receiver> receiver{1};
};

SingleShot get_single_shot(const App& a, size_t input, size_t output);
util::aligned::vector<SingleShot> get_all_input_output_combinations(
        const App& a);

}  // namespace model
*/