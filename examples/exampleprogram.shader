ShaderName = "Example Shader 1"
Vertex = {
    layout (location = 0) in vec3 aPos;
    
    void main()
    {
        gl_Position = vec4(aPos, 1.0);
    }
}
Fragment = {
    out vec4 fragcolor;

    void main()
    {
        fragcolor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}