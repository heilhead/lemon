{{require("base/PostProcess.wgsl")}}

// This is a dummy shader intended only for extracting the common post process
// material bind group data in its most complete form.

[[stage(vertex)]]
fn VSMain(vertexData: VertexInput) -> FragmentInput {
  // Declare that bindings are part of the shader interface without using them.
  let tmp1 = sceneParams;

  return FragmentInput(
    vec4<f32>(0.0, 0.0, 0.0, 0.0),
    vec2<f32>(0.0, 0.0),
  );
}

[[stage(fragment)]]
fn FSMain(fragData: FragmentInput) -> FragmentOutput {
  // Declare that bindings are part of the shader interface without using them.
  let tmp1 = sceneParams;
  let tmp2 = postProcessParams;
  let tmp3 = textureSample(tColorTarget, sColorTargetSampler, vec2<f32>(0.0, 0.0));

  return FragmentOutput(vec4<f32>(1.0, 1.0, 1.0, 1.0));
}
