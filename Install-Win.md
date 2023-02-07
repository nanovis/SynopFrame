## Download Houdini

1. Sign up and download Houdini Launcher from https://www.sidefx.com/download/
2. Open the launcher and install Houdini (as of writing, I installed `Production Build 19.5.435 win64-vc142`)
3. Use the default option and proceed to install
4. On the first launch, one will get a license error. Proceed with `Install (or reinstall) my free Houdini Apprentice License`
5. Run Houdini to test it is successfully installed 

## Download SynopFrame

1. Download the repository
2. Download the demo dataset from https://drive.google.com/file/d/16OjlFjXD_vdNULJiAklH0DFxA6f9Lf1f/view?usp=sharing
    - unzip the zip file
    - move its contents to the corresponding folders in the downloaded repository
    - in the end, you should have the following folder structure
```bash
├─demo_cube
│  ├─demo_cube
│  ├─input
│  └─demo_cube.hipnc
└─demo_RNA_Tile
    ├─demo_RNA_Tile
    ├─input
    └─demo_RNA_Tile.hipnc
```
3. Copy everything under `SynopFrame\houdini19.5` to `C:\Users\<Your_User_Name>\Documents\houdini19.5`. When asked whether to replace `houdini.env`, choose `Yes`
4. Double-click to open `SynopFrame\demo_cube\demo_cube.hipnc` (or `SynopFrame\demo_RNA_Tile\`)
5. Use [Cheat-Sheet-Win](https://github.com/nanovis/SynopFrame/blob/main/Cheat-Sheet-Win.md) to navigate inside SynopFrame



## Optional, for advanced users, if you want to compile the HDK project:
1. Open `SynopFrame\src\hdk\SynopFrame_Parser\src\CMakeLists.txt` in a text editor and change `Houdini 19.5.303` to your version. In my case, I should change it to `Houdini 19.5.435`. If you have changed the installation path of Houdini, you will need to change the whole path. 
2. Install CMake (https://cmake.org/download/), Make sure to `Add CMake to the system PATH`
3. Double click `SynopFrame\src\hdk\SynopFrame_Parser\Win-GenProjects.bat` to build the custom HDK program. 
4. Compile it in Visual Studio. It will install to the designated folder automatically. 

