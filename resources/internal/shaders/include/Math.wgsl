fn rcpf(x: f32) -> f32 {
  return 1.0 / x;
}

fn rcpf2(x: vec2<f32>) -> vec2<f32> {
  return 1.0 / x;
}

fn rcpf3(x: vec3<f32>) -> vec3<f32> {
  return 1.0 / x;
}

fn rcpf4(x: vec4<f32>) -> vec4<f32> {
  return 1.0 / x;
}

fn saturate(x: vec3<f32>) -> vec3<f32> {
  return clamp(x, vec3<f32>(0.0), vec3<f32>(1.0));
}

fn cubic(x: f32) -> vec4<f32> {
  let x2 = x * x;
  let x3 = x2 * x;

  let w = vec4<f32>(
    -x3 + 3.0 * x2 - 3.0 * x + 1.0,
    3.0 * x3 - 6.0 * x2 + 4.0,
    -3.0 * x3 + 3.0 * x2 + 3.0 * x + 1.0,
    x3
  );

  return w / 6.0;
}
