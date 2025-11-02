# SuperNova Engine
**Github project page**: [https:///github.com/Jowy02/Motor-Grafico](https:///github.com/Jowy02/Motor-Grafico)

## Description
SuperNova is a geometry visualization tool developed as the foundation for a future 3D game engine.
In this first stage, the project aims to deliver a functional geometry visualizer featuring drag & drop model loading, an orbital camera system, and a basic UI editor built with ImGui.

The current release allows users to load and explore 3D models using a smooth and intuitive camera system.

Future iterations of the project will expand its capabilities to support more advanced rendering features and basic game logic.

### Technologies & Libraries Used

     * OpenGL – Rendering API
     * GLAD – OpenGL function loader
     * GLM – Mathematics library for graphics
     * ImGui – Immediate-mode GUI for UI editing
     * Assimp – Model loading (supports various 3D formats)
     * DevIL – Image loading and texture handling
     * SDL3 – Window management and input handling

## Installation
**_Unzip the [RELEASE FOLDER](https://github.com/Jowy02/Motor-Grafico/releases) and execute the .exe file_**

## How to Use  
### Camera Controls

     * Mouse Wheel = Zoom in & Zoom out
     * Right Click + WASD = Free look & first-person movement
     * Alt + Left Click = Orbit around the selected object
     * F = Center camera on selected object
     * Left Shift = Double the camera speed


## Main Functionalities

### **Meshes & Textures**
- **Baker House model** loads automatically at program startup.  
- Two additional **FBX models** (with diffuse texture channels) must be included in the release.  
- Supports **drag & drop** of:  
  - **FBX files** from any location on disk (to load new models).  
  - **DDS / PNG textures** (applied to the currently selected GameObject).  

### **GameObjects & Components**
- Each loaded mesh is represented as a **GameObject** within the scene.  
- Every **GameObject** includes at least the following components:  
  - **Transform**  
  - **Mesh**  
  - **Texture**

### **Camera**
- **Unity-like camera controls:**  
  - **Right Mouse Button** → Free look + **WASD** movement (FPS-style), Space bar to go up and LCNTRL to go down.  
  - **Mouse Wheel** → Zoom in / out.  
  - **Alt + Left Mouse Button** → Orbit around the selected object.  
  - **F** → Focus on the selected geometry.  
  - **Left Shift** → Doubles the movement speed.
  
### **Editor Windows**
- **Console:**  
  Logs geometry loading (via **Assimp**), texture loading (**DevIL**), external library initialization, application flow, and error messages.  

- **Configuration Window:**  
  - Displays an **FPS graph**.  
  - Allows adjusting configuration variables for each module (renderer, window, input, textures, etc.).  
  - Shows system information: memory usage, hardware detection, and library/software versions (**SDL3**, **OpenGL**, **DevIL**).  

- **Hierarchy Window:**  
  Displays a list of all GameObjects in the scene.  
  The user can **select** a GameObject directly from this list.  

- **Inspector Window:**  
  Shows component information for the selected GameObject:  
  - **Transform:** Displays position, rotation, and scale (optionally editable).  
  - **Mesh:** Displays mesh info and allows toggling normal visualization (per-face or per-triangle).  
  - **Texture:** Displays texture path and size; includes an option to preview the texture on the GameObject.  

- **Toolbar / Main Menu:**  
  - **File:** Exit the program.  
  - **View:** Show / hide editor windows.  
  - **Help:**  
    - **Documentation:** Opens the GitHub `/docs` page.  
    - **Report a Bug:** Opens the GitHub `/issues` page.  
    - **Download Latest:** Opens the GitHub `/releases` page.  
    - **About:** Displays a modal window showing the engine name, version, team members, libraries used, and license information.  

- **Geometry Menu:**  
  - Includes options to **load basic geometries** (e.g., primitives or sample meshes).

## Additional Functionalities

- **Hide / Unhide GameObjects:**  
  Temporarily hide selected objects in the scene to focus on specific elements during editing or visualization.  

- **Delete GameObjects:**  
  Remove objects directly from the hierarchy or scene view.  

- **Transformations:**  
  Modify the **position**, **rotation**, and **scale** of GameObjects in real time through the **ImGui** interface.  
    
## Credits
**All contributors working on this project**:

_Joel Vicente_ « **Github**: [Jowy02](https://github.com/Jowy02)

_Arthur Cordoba_ « **Github**: [000Arthur](https://github.com/000Arthur)

_Jana Puig_ « **Github**: [JanaPuig](https://github.com/JanaPuig)