#pragma once

#include "raytracer/raytracer.h"
#include "raytracer/simulation_parameters.h"

namespace raytracer {

std::tuple<reflection_processor::make_image_source,
           reflection_processor::make_directional_histogram,
           reflection_processor::make_visual>
make_canonical_callbacks(size_t max_image_source_order, size_t visual_items);

template <typename Histogram>
struct aural_results final {
    aligned::vector<impulse<simulation_bands>> image_source;
    Histogram stochastic;
};

template <typename Histogram>
auto make_aural_results(aligned::vector<impulse<simulation_bands>> image_source,
                        Histogram stochastic) {
    return aural_results<Histogram>{std::move(image_source),
                                    std::move(stochastic)};
}

template <typename Histogram>
struct canonical_results final {
    aural_results<Histogram> aural;
    aligned::vector<aligned::vector<reflection>> visual;
};

template <typename Histogram>
auto make_canonical_results(
        aural_results<Histogram> aural,
        aligned::vector<aligned::vector<reflection>> visual) {
    return canonical_results<Histogram>{std::move(aural), std::move(visual)};
}

template <typename Callback>
auto canonical(
        const compute_context& cc,
        const generic_scene_data<cl_float3, surface<simulation_bands>>& scene,
        const model::parameters& params,
        const simulation_parameters& sim_params,
        size_t visual_items,
        const std::atomic_bool& keep_going,
        Callback&& callback) {
    const auto directions = get_random_directions(sim_params.rays);

    auto tup =
            run(begin(directions),
                end(directions),
                cc,
                make_voxelised_scene_data(scene, 5, 0.1f),
                params,
                keep_going,
                std::forward<Callback>(callback),
                make_canonical_callbacks(sim_params.maximum_image_source_order,
                                         visual_items));
    return tup ? std::experimental::make_optional(make_canonical_results(
                         make_aural_results(std::move(std::get<0>(*tup)),
                                            std::move(std::get<1>(*tup))),
                         std::move(std::get<2>(*tup))))
               : std::experimental::nullopt;
}

}  // namespace raytracer