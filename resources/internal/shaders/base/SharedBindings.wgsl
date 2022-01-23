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

[[group(0), binding(0)]]
var<uniform> sceneParams: SceneParams;
