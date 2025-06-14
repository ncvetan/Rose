// =============================================================================
//   shader for filling out gbuffers
// =============================================================================

#version 460 core

layout (location = 0) out vec4 gbuf_pos;
layout (location = 1) out vec3 gbuf_norm;
layout (location = 2) out vec4 gbuf_color;

in vs_data {
	mat3  tbn;
	vec3  frag_pos_ws;		// world space
	vec3  normal;			// tangent space
	vec2  tex_coords;
	float frag_pos_z_vs;	// view space
} fs_in;

struct Material {
	sampler2D	diffuse_map;
	sampler2D	specular_map;
	sampler2D	normal_map;
	sampler2D	displace_map;
	float		shine;
	bool		has_diffuse_map;
	bool		has_normal_map;
	bool		has_specular_map;
};

uniform Material material;

void main() {
	// if no normal map is present use surface normal otherwise convert normal to world space
	vec3 norm = (material.has_normal_map) ? fs_in.tbn * (texture(material.normal_map, fs_in.tex_coords).rgb * 2.0 - 1.0) : fs_in.normal;
	float spec = (material.has_specular_map) ? texture(material.specular_map, fs_in.tex_coords).r : 0.0;
	// TODO: reimplement displacement mapping

	// write positions, normals, colors, and specular instensities to the g-buffer
	gbuf_pos.rgb = fs_in.frag_pos_ws;
	gbuf_pos.a = fs_in.frag_pos_z_vs;
	gbuf_norm = normalize(norm);
	gbuf_color.rgb = (material.has_diffuse_map) ? texture(material.diffuse_map, fs_in.tex_coords).rgb : vec3(0.5, 0.5, 0.5);
	gbuf_color.a = spec;
}