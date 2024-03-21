
Attempt to load and sample ASTC compressed textures.
 
mtest4x4.astc generated using ARM compressor with `./astcenc-native -cl mtest.png mtest4x4.astc 4x4 -thorough`

This project is currently failing on a linux/x11 system running on
```
Extended renderer info (GLX_MESA_query_renderer):
    Vendor: Intel (0x8086)
    Device: Mesa Intel(R) UHD Graphics 620 (KBL GT2) (0x5917)
    Version: 23.2.1
    Accelerated: yes
    Video memory: 15887MB
    Unified memory: yes
    Preferred profile: core (0x1)
    Max core profile version: 4.6
    Max compat profile version: 4.6
    Max GLES1 profile version: 1.1
    Max GLES[23] profile version: 3.2
[...]
GL_KHR_robustness, GL_KHR_texture_compression_astc_ldr, 
GL_KHR_texture_compression_astc_sliced_3d, GL_MESA_framebuffer_flip_y, 
GL_KHR_texture_compression_astc_ldr, 
GL_KHR_texture_compression_astc_sliced_3d, GL_MESA_framebuffer_flip_y, 
GL_KHR_robustness, GL_KHR_texture_compression_astc_ldr, 
GL_KHR_texture_compression_astc_sliced_3d, GL_MESA_bgra, 
```

with

```
GLERROR( 111 ):  1281 // GL_INVALID_VALUE
GLERROR( 124 ):  1281 // GL_INVALID_VALUE
```
