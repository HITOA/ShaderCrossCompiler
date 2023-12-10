#version 450

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;
layout (location = 10) in vec2 _texcoord0;

layout(location = 0) out struct FsInput {
    vec3 normalOS;
    vec2 texcoord0;
} fsInput;

uniform BgfxPredefinedUniforms {
    vec4  u_viewRect;
    vec4  u_viewTexel;
    mat4  u_view;
    mat4  u_invView;
    mat4  u_proj;
    mat4  u_invProj;
    mat4  u_viewProj;
    mat4  u_invViewProj;
    mat4  u_model[32];
    mat4  u_modelView;
    mat4  u_modelViewProj;
    vec4  u_alphaRef4;
};

void main() {
    fsInput.normalOS = _normal;
    fsInput.texcoord0 = _texcoord0;

    gl_Position = u_modelViewProj * vec4(_position, 1.0);
}