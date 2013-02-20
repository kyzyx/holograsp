holograsp
=========

Stereoscopic Display Aligned with a 3D Gestural Interaction System

Setup Instructions
==================
1. Hardware: Holograsp is a prototype system with very stringent hardware requirements. For a 3D display, we recommend
    - Nvidia 3D-Vision-Ready graphics card (e.g. GTX 400 series or higher, Quadro)
    - Nvidia 3D Vision Pro kit (Note! Regular Nvidia 3D Vision kits will _not_ work because the IR signals will interfere with the depth camera)
    - 120Hz 3D-Vision Ready monitor
    - Creative Interactive Gesture Camera

2. Software Requirements for Compiling from Source:
    - Windows Vista or Windows 7 with Visual Studio 2010 (Untested with other versions)
    - [SDL library](http://www.libsdl.org)
    - DirectX11 and Direct3D 11, with [FW1](http://fw1.codeplex.com/) add-on
    - Nvidia [NvAPI](https://developer.nvidia.com/nvapi)
    - [Intel Perceptual Computing SDK](http://software.intel.com/en-us/vcsource/tools/perceptual-computing-sdk)

3. Installation Instructions (may be incomplete, please contact me for troubleshooting)
    a. Unzip source to new location
    b. Update property pages to point to library locations, i.e. include and library folders for the DirectX SDK, Perceptual Computing SDK, SDL, and FW1. 
