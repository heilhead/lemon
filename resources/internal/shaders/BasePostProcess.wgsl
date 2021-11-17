{{require("base/PostProcess.wgsl")}}
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
var sBloomSampler: sampler;

[[group(1), binding(1)]]
var tBloom: texture_2d<f32>;

[[stage(vertex)]]
fn VSMain(vertexData: VertexInput) -> FragmentInput {
  return FragmentInput(
    vec4<f32>(vertexData.position, 1.0),
    vertexData.uv0,
  );
}

[[stage(fragment)]]
fn FSMain(fragData: FragmentInput) -> FragmentOutput {
  let colColorTarget = vec4<f32>(textureSample(tColorTarget, sColorTargetSampler, fragData.uv0).xyz, 1.0);
  let colBloom = vec4<f32>(textureSample(tBloom, sBloomSampler, fragData.uv0).xyz, 1.0);

  return FragmentOutput(colColorTarget + colBloom);
}
