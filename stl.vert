#version 330 core
layout (location = 0) in vec3 aPos;   // 位置变量的属性位置值为 0
layout (location = 1) in vec3 aNormal; // 颜色变量的属性位置值为 1

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Normal = mat3(model) * aNormal;//用于旋转时，使得法向量一起改变
    FragPos = vec3(model * vec4(aPos, 1.0));
}

