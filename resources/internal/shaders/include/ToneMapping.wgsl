let gamma = 2.2;

fn TonemapACES(x: vec3<f32>) -> vec3<f32> {
  // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
  let a = 2.51;
  let b = 0.03;
  let c = 2.43;
  let d = 0.59;
  let e = 0.14;

  return (x * (a * x + b)) / (x * (c * x + d) + e);
}

fn TonemapACES2(x: vec3<f32>) -> vec3<f32>{
	let m1 = mat3x3<f32>(
    vec3<f32>(0.59719, 0.07600, 0.02840),
    vec3<f32>(0.35458, 0.90834, 0.13383),
    vec3<f32>(0.04823, 0.01566, 0.83777)
	);

	let m2 = mat3x3<f32>(
    vec3<f32>(1.60475, -0.10208, -0.00327),
    vec3<f32>(-0.53108,  1.10813, -0.07276),
    vec3<f32>(-0.07367, -0.00605,  1.07602)
	);

	let v = m1 * x;    
	let a = v * (v + 0.0245786) - 0.000090537;
	let b = v * (0.983729 * v + 0.4329510) + 0.238081;

	return pow(clamp(m2 * (a / b), vec3<f32>(0.0), vec3<f32>(1.0)), vec3<f32>(1.0 / gamma));
}

fn TonemapExposure(x: vec3<f32>, exposure: f32) -> vec3<f32> {
  return 1.0 - exp(-x * exposure);
}

fn TonemapLinear(x: vec3<f32>, exposure: f32) -> vec3<f32> {
	var color = clamp(exposure * x, vec3<f32>(0.), vec3<f32>(1.));
	color = pow(color, vec3<f32>(1. / gamma));
	return color;
}

fn TonemapReinhardSimple(x: vec3<f32>, exposure: f32) -> vec3<f32> {
	var color = x * exposure / (1. + x / exposure);
	color = pow(color, vec3<f32>(1. / gamma));
	return color;
}

fn ConvertRGB2XYZ(rgb: vec3<f32>) -> vec3<f32> {
	// Reference:
	// RGB/XYZ Matrices
	// http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
	return vec3<f32>(
	  dot(vec3<f32>(0.4124564, 0.3575761, 0.1804375), rgb),
	  dot(vec3<f32>(0.2126729, 0.7151522, 0.0721750), rgb),
	  dot(vec3<f32>(0.0193339, 0.1191920, 0.9503041), rgb)
  );
}

fn ConvertXYZ2RGB(xyz: vec3<f32>) -> vec3<f32> {
	return vec3<f32>(
	  dot(vec3<f32>(3.2404542, -1.5371385, -0.4985314), xyz),
	  dot(vec3<f32>(-0.9692660, 1.8760108, 0.0415560), xyz),
	  dot(vec3<f32>(0.0556434, -0.2040259, 1.0572252), xyz)
	);
}

fn ConvertXYZ2Yxy(xyz: vec3<f32>) -> vec3<f32> {
	// Reference:
	// http://www.brucelindbloom.com/index.html?Eqn_XYZ_to_xyY.html
	let inv: f32 = 1.0 / dot(xyz, vec3<f32>(1.0, 1.0, 1.0));
	return vec3<f32>(xyz.y, xyz.x * inv, xyz.y * inv);
}

fn ConvertYxy2XYZ(yxy: vec3<f32>) -> vec3<f32> {
	// Reference:
	// http://www.brucelindbloom.com/index.html?Eqn_xyY_to_XYZ.html
	return vec3<f32>(
    yxy.x * yxy.y / yxy.z,
    yxy.x,
    yxy.x * (1.0 - yxy.y - yxy.z) / yxy.z
  );
}

fn ConvertRGB2Yxy(rgb: vec3<f32>) -> vec3<f32> {
	return ConvertXYZ2Yxy(ConvertRGB2XYZ(rgb));
}

fn ConvertYxy2RGB(yxy: vec3<f32>) -> vec3<f32> {
	return ConvertXYZ2RGB(ConvertYxy2XYZ(yxy));
}
