# GL Texture Buddy Allocation Manager

This is a small library designed to manage texture memory using the buddy allocation technique.  Given the file paths to a set of image files, you may allocate those images, retrieve their rendering info such as their texture coordinates and designated texture sampler, then free those images using single function calls.  Because this library uses the buddy allocation technique, you may expect reduced fragmentation of the texture memory space.

This library requires that you have the GLEW and GLFW frameworks installed on your Linux machine.

##### Instructions

1. Copy `gltbam.cpp`, `gltbam.h`, and `stb/stb_image.h` into your project and include them in the compilation/linking process (`Makefile` shows an example of how to include the files).

2. Initialize and deinitalize GLTBAM using `gltbam::init()` and `gltbam::deinit()` respectively.

3. Use `gltbam::talloc(std::string imageFilepath)` to allocate your image, given its filepath.  This function will return a tag that you may use to reference the image later.

4. Use `gltbam::tcoords(std::string tag)` to retrieve the texture coordinates of your allocated image, given its associated tag.

5. Use `gltbam::tsampler(std::string tag)` to retrieve the the texture sampler of your allocated image (used in `glActiveTexture(GL_TEXTURE0 + samplerId)`), given its associated tag.

6. Use `gltbam::tfree(std::string tag)` to deallocate the image in the texture memory, given its tag.
