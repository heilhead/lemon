struct CameraParams {
  matView: mat4x4<f32>;
  matProjection: mat4x4<f32>;
  zClip: vec4<f32>;
};

[[block]]
struct SceneParams {
  camera: CameraParams;
  time: vec2<f32>;
};

struct BloomParams {
  intensity: f32;
};

[[block]]
struct PostProcessParams {
  bloom: BloomParams;
};

struct VertexInput {
  [[location(0)]] position: vec3<f32>;
  [[location(3)]] uv0: vec2<f32>;
};

struct FragmentInput {
  [[builtin(position)]] position: vec4<f32>;
  [[location(0)]] uv0: vec2<f32>;
};

struct FragmentOutput {
  [[location(0)]] color: vec4<f32>;
};

[[group(0), binding(0)]]
var<uniform> sceneParams: SceneParams;

[[group(0), binding(1)]]
var<uniform> postProcessParams: PostProcessParams;

[[group(0), binding(2)]]
var sColorTargetSampler: sampler;

[[group(0), binding(3)]]
var tColorTarget: texture_2d<f32>;
