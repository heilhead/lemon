//# include "BaseSurface.wgsl"

[[block]]
struct PacketParams {
  modelMatrix: mat4x4<f32>;
};

[[block]]
struct MaterialParams {
  tint: vec4<f32>;
};

[[group(1), binding(0)]]
var<uniform> packetParams: PacketParams;

// TODO: Consider combining packet params with material params.
[[group(1), binding(1)]]
var<uniform> materialParams: MaterialParams;

[[group(1), binding(2)]]
var surfaceSampler: sampler;

[[group(1), binding(3)]]
var tAlbedo: texture_2d<f32>;

[[group(1), binding(4)]]
var tNormal: texture_2d<f32>;

[[stage(vertex)]]
fn vs_main(vertexData: VertexInput) -> FragmentInput {
  let position: vec4<f32> = sceneParams.projection * vec4<f32>(vertexData.position.xyz, 1.0);
  var normal: vec4<f32> = sceneParams.projection * vec4<f32>(vertexData.normal.xyz, 1.0);
  normal = vec4<f32>(normal.xyz / normal.w, 1.0);
  normal = normal  * packetParams.modelMatrix;

  let tangent: vec4<f32> = vec4<f32>(1.0, 1.0, 1.0, 1.0);
  let uv0: vec2<f32> = vec2<f32>(vertexData.uv0.x, 1.0 - vertexData.uv0.y);
  let uv1: vec2<f32> = vec2<f32>(1.0, 1.0);

  return FragmentInput(
    position,
    normal,
    tangent,
    uv0,

//#if MESH_ENABLE_TEXTURE1
    uv1,
//#endif

    position.z / 2.0, // WTF is this factor?
  );
}

[[stage(fragment)]]
fn fs_main(fragData: FragmentInput) -> FragmentOutput {
//#if PIPELINE_DEPTH_ONLY
  return FragmentOutput(vec4<f32>(1.0, 1.0, 1.0, 1.0));
//#else
  let color = textureSample(tAlbedo, surfaceSampler, fragData.uv0);
  // let color = textureSample(tNormal, surfaceSampler, fragData.uv0);

  let surfaceAttributres: SurfaceAttributes = SurfaceAttributes(
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

  return FragmentOutput(vec4<f32>(surfaceAttributres.baseColor.xyz * materialParams.tint.xyz, 1.0), fragData.depth);
//#endif
}
