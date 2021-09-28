[[block]]
struct SceneParams {
    // todo: game clock
    projection: mat4x4<f32>;
};

//[[block]]
//struct MaterialParams {
    // todo: any material params, e.g. textures or numeric constants
//};

//[[block]]
//struct PacketParams {
    // todo: model matrix and other render packet params
//};

[[block]]
struct SurfaceAttributes {
    baseColor: vec4<f32>;
    emissiveColor: vec4<f32>;
    metallic: f32;
    specular: f32;
    roughness: f32;
    anisotropy: f32;
    ambientOcclusion: f32;
    tangent: vec4<f32>;
    normal: vec4<f32>;
};

struct VertexInput {
    [[location(0)]] position: vec3<f32>;
    [[location(1)]] normal: vec4<f32>;
    [[location(2)]] tangent: vec4<f32>;
    [[location(3)]] uv0: vec2<f32>;
    // [[location(4)]] uv1: vec2<f32>;
    [[location(5)]] jointInfluence: vec4<u32>;
    [[location(6)]] jointWeight: vec4<f32>;
};

struct VertexOutput {
    [[builtin(position)]] position: vec4<f32>;
    [[location(0)]] normal: vec4<f32>;
    [[location(1)]] tangent: vec4<f32>;
    [[location(2)]] uv0: vec2<f32>;
    [[location(3)]] uv1: vec2<f32>;
};

[[group(0), binding(0)]]
var<uniform> sceneParams: SceneParams;

[[group(1), binding(0)]]
var mySampler: sampler;

[[group(1), binding(1)]]
var myTexture: texture_2d<f32>;

[[block]]
struct VecUniform {
    data: vec4<f32>;
};

[[group(2), binding(0)]]
var<uniform> colWhite: VecUniform;

[[group(3), binding(0)]]
var<uniform> colGray: VecUniform;

[[group(4), binding(0)]]
var<uniform> colRed: VecUniform;

[[stage(vertex)]]
fn vs_main(vertexData: VertexInput) -> VertexOutput {
    let position: vec4<f32> = sceneParams.projection * vec4<f32>(vertexData.position.xyz, 1.0);
    var normal: vec4<f32> = sceneParams.projection * vec4<f32>(vertexData.normal.xyz, 1.0);
    normal = vec4<f32>(normal.xyz / normal.w, 1.0);

    let tangent: vec4<f32> = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    let uv0: vec2<f32> = vec2<f32>(1.0, 1.0);
    let uv1: vec2<f32> = vec2<f32>(1.0, 1.0);

    return VertexOutput(
        position,
        normal,
        tangent,
        uv0,
        uv1
    );
}

struct FragmentOutput {
    [[location(0)]] color: vec4<f32>;
};

[[stage(fragment)]]
fn fs_main(fragData: VertexOutput) -> FragmentOutput {
    return FragmentOutput(vec4<f32>(colWhite.data.xyz, 1.0));

    // return FragmentOutput(vec4<f32>(fragData.normal.xyz, 1.0));
    
    // return FragmentOutput(textureSample(myTexture, mySampler, fragData.position.xy / vec2<f32>(640.0, 480.0)));
}
