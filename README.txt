System for my 3rd year dissertation, consists of a Naive OpenGL engine, an AZDO OpenGL engine, and a Vulkan engine.
Each engine has 5 renderers which are used as experiments for later evaluation, the renderer that will run is controlled
by preprocessor definitions in System.cpp (naive and AZDO) and RenderPass.cpp (Vulkan).

Requirements:

OpenGL 4.6 for AZDO gl_BaseInstance used in the shaders
Vulkan SDK 1.1
Vulkan graphics and compute queues must be the same queue
A discrete GPU for Vulkan

REFERENCES:

- Tinyobjloader for loading the mesh files and my loader code is adapted from the example in the readme: https://github.com/syoyo/tinyobjloader
- nv_dds for loading dds images: https://github.com/paroj/nv_dds/blob/master/nv_dds.cpp
- Alexander Overvoorde (vulkan-tutorial author) for providing the basis of understanding for Vulkan and a lot of code to get it running, 
   most of which has been adapted in to my Vulkan engine: https://vulkan-tutorial.com/
- nvMcJohn apitest for implementation of SBTAs, which I took and adapted in to my AZDO engine: https://github.com/nvMcJohn/apitest (public domain)
- GLFW: https://www.glfw.org/
- GL3W: https://github.com/skaslev/gl3w (public domain)
- GLM: https://glm.g-truc.net/0.9.9/index.html
