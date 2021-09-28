//# extends "BaseSurface.wgsl"

//# block surface_params
[[group(1), binding(0)]]
var surfaceSampler: sampler;

[[group(1), binding(1)]]
var surfaceTexture: texture_2d<f32>;

type ArrayType = [[stride(16)]] array<i32, 4>;

[[block]]
struct LemonData {
    lemonMat: mat4x4<f32>;
    lemonVecData: vec4<f32>;
    lemonArr: ArrayType;
};

[[group(1), binding(2)]]
var<uniform> lemonData: LemonData;

[[group(1), binding(3)]] var t_f : texture_2d<f32>;
[[group(1), binding(4)]] var t_i : texture_2d<i32>;
[[group(1), binding(5)]] var t_u : texture_2d<u32>;
//# endblock

//# block vs_surface
{{super()}}
//# endblock

//# block fs_surface
fn fs_surface(fragData: VertexOutput) -> SurfaceAttributes {
  var color = textureSample(surfaceTexture, surfaceSampler, fragData.position.xy / vec2<f32>(640.0, 480.0)) * lemonData.lemonMat;

  return SurfaceAttributes(
    color,
    vec4<f32>(0.0, 0.0, 0.0, 0.0),
    1.0,
    1.0,
    1.0,
    1.0,
    1.0,
    vec4<f32>(1.0, 1.0, 1.0, 1.0),
    vec4<f32>(1.0, 1.0, 1.0, 1.0),
  );
}
//# endblock

//# block vs_main
{{super()}}
//# endblock

//# block fs_main
{{super()}}
//# endblock
