// =============================================================================
//   final shader stage, adds effects and corrections to the output
// =============================================================================

#version 460

in vs_data {
	vec2 tex_coords;
} fs_in;

out vec4 frag_color;

uniform sampler2D tex;
uniform sampler2D bloom_tex;
uniform float gamma;
uniform float exposure;
uniform bool bloom_enabled;
uniform float bloom_factor;

void main() {
	
	vec3 hdr_color = texture(tex, fs_in.tex_coords).rgb;

	if (bloom_enabled) {
		// mix bloom component
		vec3 bloom_color = texture(bloom_tex, fs_in.tex_coords).rgb;
		hdr_color = mix(hdr_color, bloom_color, bloom_factor);
	}

	// tone mapping
	vec3 mapped_color = vec3(1.0) - exp(-hdr_color.rgb * exposure);
	
	// gamma correction
	mapped_color = pow(mapped_color, vec3(1.0 / gamma));
	
	frag_color = vec4(mapped_color, 1.0);
}