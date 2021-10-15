//# include "BaseSurface.wgsl"

// This is a dummy shader intended only for extracting the common surface
// material bind group data in its most complete form, e.g. with enabled
// skinning and lighting.

[[stage(vertex)]]
fn vs_main(vertexData: VertexInput) -> FragmentInput {
  // Reference all shared bindings in both vertex and fragment entry points
  // to set proper visibility flags.
  let ref = sceneParams.time;

  return FragmentInput(
    vec4<f32>(0.0, 0.0, 0.0, 0.0),
    vec4<f32>(0.0, 0.0, 0.0, 0.0),
    vec4<f32>(0.0, 0.0, 0.0, 0.0),
    vec2<f32>(0.0, 0.0),
    vec2<f32>(0.0, 0.0),
    0.0,
  );
}

[[stage(fragment)]]
fn fs_main(fragData: FragmentInput) -> FragmentOutput {
  // Reference all shared bindings in both vertex and fragment entry points
  // to set proper visibility flags.
  let ref = sceneParams.time;

  return FragmentOutput(vec4<f32>(1.0, 1.0, 1.0, 1.0), 0.0);
}
