#version 450

layout(location = 0) out struct TestStruct {
    vec3 position;
} testStruct;

layout(location = 0) in vec3 a_position;
layout(location = 4) in vec4 a_color0;

layout(binding = 0) uniform UniformBufferObjectOWO {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 test[23];
};

void main() {
    testStruct.position = a_position;
    gl_Position = model * view * proj * vec4(a_position, 1.0);
}