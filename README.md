# ShaderCrossCompiler

Tool to cross compile Vulkan GLSL (WIP).

ShaderCrossCompiler simply glues 3rdparty library (glslang and spirv-cross) together.

ShaderCrossCompiler can also produce aggregate shader file. that is, file that contain all
shader format. You only have to include aggregateshader.h to parse those file.

This was mainly done to be used with vin-engine 2 : https://github.com/HITOA/vin-engine 

CLI options : 

```
ShaderCrossCompiler [OPTION...]

  -h, --help        Print usage.
  -i, --input arg   Input shader path
  -o, --output arg  Output shader directory
      --fragment    Compile input as a fragment shader
      --vertex      Compile input as a vertex shader
      --geometry    Compile input as a geometry shader
      --control     Compile input as a tesselation control shader
      --eval        Compile input as a tesselation evaluation shader
      --compute     Compile input as a compute shader
      --optimize    Enable glslang optimizer
      --glsl        Only output opengl shader
      --spirv       Only output vulkan shader
      --hlsl        Only output directx shader
      --msl         Only output metal shader
      --all         Output all shader format (shortcut)
      --aggregate   Output all shader format in one file
```