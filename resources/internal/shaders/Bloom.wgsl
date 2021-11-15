{{require("Sampling.wgsl")}}
{{require("ToneMapping.wgsl")}}

// Based on: https://github.com/Unity-Technologies/Graphics/blob/master/com.unity.render-pipelines.universal/Shaders/PostProcessing/Bloom.shader

struct BloomVertexInput {
  [[location(0)]] position: vec3<f32>;
  [[location(1)]] uv0: vec2<f32>;
};

struct BloomFragmentInput {
  [[builtin(position)]] position: vec4<f32>;
  [[location(0)]] uv0: vec2<f32>;
};

struct BloomFragmentOutput {
  [[location(0)]] color: vec4<f32>;
};

[[stage(vertex)]]
fn VSBloomMain(vertexData: BloomVertexInput) -> BloomFragmentInput {
  return BloomFragmentInput(
    vec4<f32>(vertexData.position, 1.0),
    vertexData.uv0,
  );
}

[[block]]
struct BloomParams {
    threshold: f32;
    strength: f32;
    texelSize: vec2<f32>;
    lowTexSize: vec4<f32>; //xy texel size, zw width height
    scatter: f32;
    clampMax: f32;
    thresholdKnee: f32;
};

[[group(1), binding(0)]]
var<uniform> bloomParams: BloomParams;

[[group(1), binding(1)]]
var sLinearClamp: sampler;

[[group(1), binding(2)]]
var tSrc: texture_2d<f32>;

[[group(1), binding(3)]]
var tSrcLow: texture_2d<f32>;

[[stage(fragment)]]
fn PSPrefilterMain(input: BloomFragmentInput) -> BloomFragmentOutput {
  let uv = input.uv0;
  let texelSize = bloomParams.texelSize;

  let A = textureSample(tSrc, sLinearClamp, uv + texelSize * vec2<f32>(-1.0, -1.0));
  let B = textureSample(tSrc, sLinearClamp, uv + texelSize * vec2<f32>(0.0, -1.0));
  let C = textureSample(tSrc, sLinearClamp, uv + texelSize * vec2<f32>(1.0, -1.0));
  let D = textureSample(tSrc, sLinearClamp, uv + texelSize * vec2<f32>(-0.5, -0.5));
  let E = textureSample(tSrc, sLinearClamp, uv + texelSize * vec2<f32>(0.5, -0.5));
  let F = textureSample(tSrc, sLinearClamp, uv + texelSize * vec2<f32>(-1.0, 0.0));
  let G = textureSample(tSrc, sLinearClamp, uv);
  let H = textureSample(tSrc, sLinearClamp, uv + texelSize * vec2<f32>(1.0, 0.0));
  let I = textureSample(tSrc, sLinearClamp, uv + texelSize * vec2<f32>(-0.5, 0.5));
  let J = textureSample(tSrc, sLinearClamp, uv + texelSize * vec2<f32>(0.5, 0.5));
  let K = textureSample(tSrc, sLinearClamp, uv + texelSize * vec2<f32>(-1.0, 1.0));
  let L = textureSample(tSrc, sLinearClamp, uv + texelSize * vec2<f32>(0.0, 1.0));
  let M = textureSample(tSrc, sLinearClamp, uv + texelSize * vec2<f32>(1.0, 1.0));

  let div = (1.0 / 4.0) * vec2<f32>(0.5, 0.125);

  var o = (D + E + I + J) * div.x
    + (A + B + G + F) * div.y
    + (B + C + H + G) * div.y
    + (F + G + L + K) * div.y
    + (G + H + M + L) * div.y;

  var color = o.xyz;

  // User controlled clamp to limit crazy high broken spec
  color = min(vec3<f32>(bloomParams.clampMax), color);

  // Thresholding
  let brightness = max(max(color.r, color.g), color.b);
  
  var softness = clamp(brightness - bloomParams.threshold + bloomParams.thresholdKnee, 0.0, 2.0 * bloomParams.thresholdKnee);
  softness = (softness * softness) / (4.0 * bloomParams.thresholdKnee + 0.0001);
  
  let multiplier = max(brightness - bloomParams.threshold, softness) / max(brightness, 0.0001);

  color = color * multiplier;

  // Clamp colors to positive once in prefilter. Encode can have a sqrt, and sqrt(-x) == NaN. Up/Downsample passes would then spread the NaN.
  color = max(color, vec3<f32>(0.0));

  return BloomFragmentOutput(vec4<f32>(color, 1.0));
}

[[stage(fragment)]]
fn PSBlurHMain(input: BloomFragmentInput) -> BloomFragmentOutput {
  let uv = input.uv0;
  let texelSize = bloomParams.texelSize.x * 2.0;

  // 9-tap gaussian blur on the downsampled source
  let c0 = textureSample(tSrc, sLinearClamp, uv - vec2<f32>(texelSize * 4.0, 0.0)).rgb;
  let c1 = textureSample(tSrc, sLinearClamp, uv - vec2<f32>(texelSize * 3.0, 0.0)).rgb;
  let c2 = textureSample(tSrc, sLinearClamp, uv - vec2<f32>(texelSize * 2.0, 0.0)).rgb;
  let c3 = textureSample(tSrc, sLinearClamp, uv - vec2<f32>(texelSize * 1.0, 0.0)).rgb;
  let c4 = textureSample(tSrc, sLinearClamp, uv).rgb;
  let c5 = textureSample(tSrc, sLinearClamp, uv + vec2<f32>(texelSize * 1.0, 0.0)).rgb;
  let c6 = textureSample(tSrc, sLinearClamp, uv + vec2<f32>(texelSize * 2.0, 0.0)).rgb;
  let c7 = textureSample(tSrc, sLinearClamp, uv + vec2<f32>(texelSize * 3.0, 0.0)).rgb;
  let c8 = textureSample(tSrc, sLinearClamp, uv + vec2<f32>(texelSize * 4.0, 0.0)).rgb;

  let color = c0 * 0.01621622 + c1 * 0.05405405 + c2 * 0.12162162 + c3 * 0.19459459
    + c4 * 0.22702703
    + c5 * 0.19459459 + c6 * 0.12162162 + c7 * 0.05405405 + c8 * 0.01621622;

  return BloomFragmentOutput(vec4<f32>(color, 1.0));
}

[[stage(fragment)]]
fn PSBlurVMain(input: BloomFragmentInput) -> BloomFragmentOutput {
  let uv = input.uv0;
  let texelSize = bloomParams.texelSize.y;

  // Optimized bilinear 5-tap gaussian on the same-sized source (9-tap equivalent)
  let c0 = textureSample(tSrc, sLinearClamp, uv - vec2<f32>(0.0, texelSize * 3.23076923)).rgb;
  let c1 = textureSample(tSrc, sLinearClamp, uv - vec2<f32>(0.0, texelSize * 1.38461538)).rgb;
  let c2 = textureSample(tSrc, sLinearClamp, uv).rgb;
  let c3 = textureSample(tSrc, sLinearClamp, uv + vec2<f32>(0.0, texelSize * 1.38461538)).rgb;
  let c4 = textureSample(tSrc, sLinearClamp, uv + vec2<f32>(0.0, texelSize * 3.23076923)).rgb;

  let color = c0 * 0.07027027 + c1 * 0.31621622
    + c2 * 0.22702703
    + c3 * 0.31621622 + c4 * 0.07027027;

  return BloomFragmentOutput(vec4<f32>(color, 1.0));
}

[[stage(fragment)]]
fn PSUpsampleMain(input: BloomFragmentInput) -> BloomFragmentOutput {
  let uv = input.uv0;
  let texelSize = bloomParams.texelSize.y;

  let highMip = textureSample(tSrc, sLinearClamp, uv).rgb;
  let lowMip = SampleTexture2DBicubic(tSrcLow, sLinearClamp, uv, bloomParams.lowTexSize, vec2<f32>(1.0)).rgb;
  // let lowMip = textureSample(tSrcLow, sLinearClamp, uv).rgb;

  return BloomFragmentOutput(vec4<f32>(mix(highMip, lowMip, bloomParams.scatter), 1.0));
}
