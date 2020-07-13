#version 430 core

in vec2 v_pos;

layout(binding = 1) uniform sampler2D current_texture;

out vec4 fragment_output;

void main()
{
		fragment_output = texture(current_texture, vec2(0.5*(v_pos.x+1.0),0.5*(v_pos.y+1.0)));
		// fragment_output = vec4(0.5*(v_pos.x+1.0),0.5*(v_pos.y+1.0),0,1);
}
