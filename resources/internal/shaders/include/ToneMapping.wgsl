// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
fn TonemapACES(x: vec3<f32>) -> vec3<f32> {
  let a = 2.51;
  let b = 0.03;
  let c = 2.43;
  let d = 0.59;
  let e = 0.14;

  return (x * (a * x + b)) / (x * (c * x + d) + e);
}
