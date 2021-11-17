{{require("base/PostProcess.wgsl")}}
{{require("include/ToneMapping.wgsl")}}

[[block]]
struct MaterialParams {
  tint: vec4<f32>;
};

[[group(1), binding(0)]]
var<uniform> materialParams: MaterialParams;

[[group(1), binding(1)]]
var sBloomSampler: sampler;

[[group(1), binding(2)]]
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
  let tmp1 = sceneParams;
  let tmp2 = postProcessParams;
  let tmp3 = materialParams;

  var colColorTarget = textureSample(tColorTarget, sColorTargetSampler, fragData.uv0).xyz;
  colColorTarget = 1.0 - TonemapACES(colColorTarget);

  let colBloom = vec4<f32>(textureSample(tBloom, sBloomSampler, fragData.uv0).xyz, 1.0);

  return FragmentOutput(vec4<f32>(colColorTarget, 1.0));
}
