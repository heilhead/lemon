[[group(0), binding(0)]] var mySampler: sampler;
[[group(0), binding(1)]] var myTexture: texture_2d<f32>;

[[stage(vertex)]]
fn frag_main([[location(0)]] pos : vec4<f32>) -> [[builtin(position)]] vec4<f32> {
  return pos;
}

[[stage(fragment)]]
fn vert_main([[builtin(position)]] FragCoord : vec4<f32>) -> [[location(0)]] vec4<f32> {
  return textureSample(myTexture, mySampler, FragCoord.xy / vec2<f32>(640.0, 480.0));
}
