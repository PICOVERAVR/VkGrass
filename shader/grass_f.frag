#version 460

layout (location = 0) in vec3 p;
layout (location = 1) in vec3 n;
layout (location = 2) in vec2 uv;

layout (set = 0, binding = 1) uniform sampler2D tex;

layout (location = 0) out vec4 fragcolor;

void main() {
	vec4 raw = texture(tex, uv);
	// discarding if 1.0 leads to aliasing issues on the edge of the texture
	// discarding if not 0.0 leads to transparency problems
	if (raw.a <= 0.9) {
		discard;
	}
	fragcolor = raw;
}