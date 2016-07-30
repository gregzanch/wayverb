#include "raytracer/reflector.h"
#include "raytracer/random_directions.h"
#include "raytracer/scene_buffers.h"

#include "common/conversions.h"
#include "common/scene_data.h"

#include <random>

namespace {

aligned::vector<cl_float> get_direction_rng(size_t num) {
    aligned::vector<cl_float> ret;
    ret.reserve(2 * num);
    std::default_random_engine engine{std::random_device()()};

    for (auto i = 0u; i != num; ++i) {
        const raytracer::direction_rng rng(engine);
        ret.push_back(rng.get_z());
        ret.push_back(rng.get_theta());
    }

    return ret;
}

aligned::vector<Ray> get_random_rays(size_t num, const glm::vec3& source) {
    aligned::vector<Ray> ret;
    ret.reserve(num);
    std::default_random_engine engine{std::random_device()()};

    auto src = to_cl_float3(source);

    for (auto i = 0u; i != num; ++i) {
        const raytracer::direction_rng rng(engine);
        ret.push_back(Ray{
                src, to_cl_float3(sphere_point(rng.get_z(), rng.get_theta()))});
    }
    return ret;
}

}  // namespace

//----------------------------------------------------------------------------//

namespace raytracer {

reflector::reflector(const cl::Context& context,
                     const cl::Device& device,
                     const glm::vec3& source,
                     const glm::vec3& receiver,
                     size_t rays)
        : context(context)
        , device(device)
        , kernel(raytracer_program(context, device).get_reflections_kernel())
        , receiver(to_cl_float3(receiver))
        , rays(rays)
        , ray_buffer(
                  load_to_buffer(context, get_random_rays(rays, source), false))
        , reflection_buffer(load_to_buffer(
                  context,
                  aligned::vector<Reflection>(rays,
                                              Reflection{cl_float3{},
                                                         cl_float3{},
                                                         cl_ulong{},
                                                         cl_char{true},
                                                         cl_char{}}),
                  false))
        , rng_buffer(context, CL_MEM_READ_WRITE, rays * 2 * sizeof(cl_float)) {}

aligned::vector<Reflection> reflector::run_step(scene_buffers& buffers) {
    //  get some new rng and copy it to device memory
    auto rng = get_direction_rng(rays);
    cl::copy(buffers.get_queue(), std::begin(rng), std::end(rng), rng_buffer);

    //  get the kernel and run it
    kernel(cl::EnqueueArgs(buffers.get_queue(), cl::NDRange(rays)),
           ray_buffer,
           receiver,
           buffers.get_voxel_index_buffer(),
           buffers.get_global_aabb(),
           buffers.get_side(),
           buffers.get_triangles_buffer(),
           buffers.get_vertices_buffer(),
           buffers.get_surfaces_buffer(),
           rng_buffer,
           reflection_buffer);

    aligned::vector<Reflection> ret(rays);
    cl::copy(buffers.get_queue(),
             reflection_buffer,
             std::begin(ret),
             std::end(ret));
    return ret;
}

}  // namespace raytracer