{{require("BaseSurface.wgsl")}}

// This is a dummy shader intended only for extracting the common surface
// material bind group data in its most complete form, e.g. with enabled
// skinning and lighting.

[[stage(vertex)]]
fn VSMain(vertexData: VertexInput) -> FragmentInput {
  // Declare that bindings are part of the shader interface without using them.
  let tmp = sceneParams.time;

  return FragmentInput(
    vec4<f32>(0.0, 0.0, 0.0, 0.0),
    vec4<f32>(0.0, 0.0, 0.0, 0.0),
    vec4<f32>(0.0, 0.0, 0.0, 0.0),
    vec2<f32>(0.0, 0.0),
    vec2<f32>(0.0, 0.0),
  );
}

[[stage(fragment)]]
fn FSMain(fragData: FragmentInput) -> FragmentOutput {
  // Declare that bindings are part of the shader interface without using them.
  let tmp = sceneParams.time;

  return FragmentOutput(vec4<f32>(1.0, 1.0, 1.0, 1.0));
}
