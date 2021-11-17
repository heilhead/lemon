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

[[block]]
struct SurfaceAttributes {
  baseColor: vec4<f32>;
  emissiveColor: vec4<f32>;
  metallic: f32;
  specular: f32;
  roughness: f32;
  anisotropy: f32;
  ambientOcclusion: f32;
  tangent: vec4<f32>;
  normal: vec4<f32>;
};

struct VertexInput {
  [[location(0)]] position: vec3<f32>;

#if MESH_ENABLE_NORMAL
  [[location(1)]] normal: vec4<f32>;
#endif

#if MESH_ENABLE_TANGENT
  [[location(2)]] tangent: vec4<f32>;
#endif

#if MESH_ENABLE_TEXTURE0
  [[location(3)]] uv0: vec2<f32>;
#endif

#if MESH_ENABLE_TEXTURE1
  [[location(4)]] uv1: vec2<f32>;
#endif

#if MESH_ENABLE_SKINNING
  [[location(5)]] jointInfluence: vec4<u32>;
  [[location(6)]] jointWeight: vec4<f32>;
#endif
};

struct FragmentInput {
  [[builtin(position)]] position: vec4<f32>;

#if MESH_ENABLE_NORMAL
  [[location(0)]] normal: vec4<f32>;
#endif

#if MESH_ENABLE_TANGENT
  [[location(1)]] tangent: vec4<f32>;
#endif

#if MESH_ENABLE_TEXTURE0
  [[location(2)]] uv0: vec2<f32>;
#endif

#if MESH_ENABLE_TEXTURE1
  [[location(3)]] uv1: vec2<f32>;
#endif
};

struct FragmentOutput {
  [[location(0)]] color: vec4<f32>;
};

[[group(0), binding(0)]]
var<uniform> sceneParams: SceneParams;

#if MESH_ENABLE_SKINNING
// TODO: Shared skinning data bindings.
#endif

#if MATERIAL_ENABLE_LIGHTING
// TODO: Shared lighting data bindings.
#endif

fn GetTime() -> f32 {
  return sceneParams.time.x;
}

fn GetTimeFrac() -> f32 {
  return sceneParams.time.y;
}

fn LinearizeDepth(depth: f32) -> f32 {
  let near = sceneParams.camera.zClip.x;
  let far = sceneParams.camera.zClip.y;
  return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}
