#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
uniform mat4 projection, model_view, cam;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;

void main() {
	gl_Position = projection * cam * model_view * vec4(aPos, 1.0);
    FragPos = vec3(model_view * vec4(aPos, 1.0));
	Normal = aNormal;
    TexCoord = aTexCoord;
}