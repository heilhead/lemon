{{require("include/Math.wgsl")}}

fn BSpline3Leftmost(x: vec2<f32>) -> vec2<f32> {
  return 0.16666667 * x * x * x;
}

fn BSpline3MiddleLeft(x: vec2<f32>) -> vec2<f32> {
  return 0.16666667 + x * (0.5 + x * (0.5 - x * 0.5));
}

fn BSpline3MiddleRight(x: vec2<f32>) -> vec2<f32> {
  return 0.66666667 + x * (-1.0 + 0.5 * x) * x;
}

fn BSpline3Rightmost(x: vec2<f32>) -> vec2<f32> {
  return 0.16666667 + x * (-0.5 + x * (0.5 - x * 0.16666667));
}

struct BicubicFilterResult {
  weights: array<vec2<f32>, 2>;
  offsets: array<vec2<f32>, 2>;
};

fn BicubicFilter(fracCoord: vec2<f32>) -> BicubicFilterResult {
  let r = BSpline3Rightmost(fracCoord);
  let mr = BSpline3MiddleRight(fracCoord);
  let ml = BSpline3MiddleLeft(fracCoord);
  let l = 1.0 - mr - ml - r;

  let weights = array<vec2<f32>, 2>(r + mr, ml + l);
  let offsets = array<vec2<f32>, 2>(-1.0 + mr * rcpf2(weights[0]), 1.0 + l * rcpf2(weights[1]));

  return BicubicFilterResult(weights, offsets);
}

fn SampleTexture2DBicubic(tex: texture_2d<f32>, smp: sampler, coord: vec2<f32>, texSize: vec4<f32>, maxCoord: vec2<f32>) -> vec4<f32> {
  let xy = coord * texSize.zw + 0.5;
  let ic = floor(xy);
  let fc = fract(xy);

  let res = BicubicFilter(fc);
  let weights = res.weights;
  let offsets = res.offsets;

  return weights[0].y * (weights[0].x * textureSampleLevel(tex, smp, min((ic + vec2<f32>(offsets[0].x, offsets[0].y) - 0.5) * texSize.xy, maxCoord), 0.0) +
    weights[1].x * textureSampleLevel(tex, smp, min((ic + vec2<f32>(offsets[1].x, offsets[0].y) - 0.5) * texSize.xy, maxCoord), 0.0)) +
    weights[1].y * (weights[0].x * textureSampleLevel(tex, smp, min((ic + vec2<f32>(offsets[0].x, offsets[1].y) - 0.5) * texSize.xy, maxCoord), 0.0) +
    weights[1].x * textureSampleLevel(tex, smp, min((ic + vec2<f32>(offsets[1].x, offsets[1].y) - 0.5) * texSize.xy, maxCoord), 0.0));
}
