#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
uniform mat4 projection, model_view, cam;
void main() {
	gl_Position = projection * cam * model_view * vec4(aPos, 1.0);
}
