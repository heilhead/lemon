{{require("base/PostProcess.wgsl")}}
{{require("include/Math.wgsl")}}
{{require("include/ToneMapping.wgsl")}}

[[block]]
struct MaterialParams {
  toneMappingExposure: f32;
  toneMappingWhitePoint: f32;
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

fn FilmicToneMapping(x: vec3<f32>) -> vec3<f32> {
	var color = max(vec3<f32>(0.), x - vec3<f32>(0.004));
	color = (color * (6.2 * color + .5)) / (color * (6.2 * color + 1.7) + 0.06);
	return color;
}

[[stage(fragment)]]
fn FSMain(fragData: FragmentInput) -> FragmentOutput {
  let tmp1 = sceneParams;
  let tmp2 = postProcessParams;

  let exposure = materialParams.toneMappingExposure;

  var colColorTarget = textureSample(tColorTarget, sColorTargetSampler, fragData.uv0).xyz;
  let colBloom = textureSample(tBloom, sBloomSampler, fragData.uv0).xyz;

  var rgb = colColorTarget + colBloom;
  rgb = TonemapReinhardSimple(rgb, exposure);
  // rgb = TonemapACES2(rgb * exposure);

  return FragmentOutput(vec4<f32>(rgb, 1.0));
}
