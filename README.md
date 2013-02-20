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

3. Library Requirements for Running the Binary: Ensure that the relevant runtime libraries for these components are available, then run the application in bin/
    - Nvidia 3D vision DLLs (should be included when installing 3D Vision drivers)
    - DirectX 11 DLLs
    - Intel PCSDK
    - OpenGL

4. Installation Instructions (may be incomplete, please contact me for troubleshooting)
    - Unzip source to new location
    - Update property pages to point to library locations, i.e. include and library folders for the DirectX SDK, Perceptual Computing SDK, SDL, and FW1. 

5. Using the Application
    - Optimal setup is with the camera mounted on top of the monitor at approximately a 30 degree downward angle, with the user sitting about 80cm in front of the screen. 
    - Make sure stereoscopic 3D is turned on via the Nvidia control panel.
    - The current prototype application is an experiment that times how long it takes to translate the red icosahedron into the blue spheroid. The green tetrahedron indicates your finger position (cursor position) for calibration.
    - If you are not experienced with stereoscopic 3D, make sure you take awhile to test out the pop-out effects. Try and converge your eyes until you can see the objects on screen as a single object in front of you, rather than double images onscreen.
    - Calibration is very hacky right now. Use the arrow keys to adjust the position of the cursor until it appears to float slightly above your outstretched finger. To adjust the apparent depth, use the slash (/) and quote (') keys. 
    - For maximum robustness, the application tracks the closest blob to the camera as the "cursor position". The cursor position is indicated by the green tetrahedron. To select the icosahedron, place your finger right underneath the apparent location of the icosahedron. If calibration worked correctly, the cursor position should be snugly within the icosahedron and it will change color to blue. 
    - Hold down the spacebar while you move your finger to drag the icosahedron with your finger. Release the spacebar to place the object. If you place the icosahedron into the blue spheroid, you will have your time recorded and be presented with a new task.

WARNING: This application occasionally requires an unplug and a replugging in of the Gesture Camera into a different port. This is a known issue with the PCSDK. If the application doesn't run for an unknown reason, try this first.
