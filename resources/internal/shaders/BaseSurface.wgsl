{{require("base/Surface.wgsl")}}
{{require("include/ToneMapping.wgsl")}}

[[block]]
struct PacketParams {
  matModel: mat4x4<f32>;
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
fn VSMain(vertexData: VertexInput) -> FragmentInput {
  let positionWorldSpace: vec4<f32> = packetParams.matModel * vec4<f32>(vertexData.position.xyz, 1.0);
  let position: vec4<f32> = sceneParams.camera.matProjection * sceneParams.camera.matView * positionWorldSpace;
  
  var normal: vec4<f32> = sceneParams.camera.matProjection * vec4<f32>(vertexData.normal.xyz, 1.0);
  normal = vec4<f32>(normal.xyz / normal.w, 1.0);
  normal = normal  * packetParams.matModel;

  let tangent: vec4<f32> = vec4<f32>(1.0, 1.0, 1.0, 1.0);
  let uv0: vec2<f32> = vec2<f32>(vertexData.uv0.x, 1.0 - vertexData.uv0.y);
  let uv1: vec2<f32> = vec2<f32>(1.0, 1.0);

  return FragmentInput(
    position,
    normal,
    tangent,
    uv0,

#if MESH_ENABLE_TEXTURE1
    uv1,
#endif
  );
}

[[stage(fragment)]]
fn FSMain(fragData: FragmentInput) -> FragmentOutput {
#if PIPELINE_DEPTH_ONLY
  return FragmentOutput(vec4<f32>(1.0, 1.0, 1.0, 1.0));
#else
  let color = vec4<f32>(textureSample(tAlbedo, surfaceSampler, fragData.uv0).xyz * materialParams.tint.xyz, 1.0);
  // let depth = fragData.position.z / fragData.position.w;
  // let depth = linearizeDepth(fragData.position.z / fragData.position.w);
  // let color = vec4<f32>(depth, depth, depth, 1.0);

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

  let toneMappedColor = TonemapACES(surfaceAttributres.baseColor.xyz);

  return FragmentOutput(vec4<f32>(toneMappedColor, 1.0));
#endif
}
