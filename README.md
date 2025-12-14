# SuperNova Engine
**Github project page**: [https:///github.com/Jowy02/Motor-Grafico](https:///github.com/Jowy02/Motor-Grafico)

## Description
SuperNova is a lightweight 3D engine that evolved from an initial geometry visualization tool into a scene-based rendering and editing framework.

Originally focused on geometry visualization, the project provided features such as drag & drop model loading, an orbital camera system, and an ImGui-based editor. These foundations enabled the transition toward a more complete engine architecture.

The current version supports scene creation, modification, saving, and loading, along with real-time object manipulation through integrated editor tools, allowing users to build and explore 3D scenes interactively.

### Technologies & Libraries Used

     * OpenGL – Rendering API
     * GLAD – OpenGL function loader
     * GLM – Mathematics library for graphics
     * ImGui – Immediate-mode GUI for UI editing
     * + ImGuizmo – Gizmo manipulation library for translation, rotation, and scaling of objects within ImGui
     * + ImGuiFileDialog – File dialog extension for ImGui, used for browsing and selecting files at runtime
     * Assimp – Model loading (supports various 3D formats)
     * DevIL – Image loading and texture handling
     * SDL3 – Window management and input handling

## Installation
**_Unzip the [RELEASE FOLDER](https://github.com/Jowy02/Motor-Grafico/releases) and execute the .exe file_**

## How to Use  
### Camera Controls

     * Mouse Wheel = Zoom in & Zoom out
     * Right Click + WASD = Free look & first-person movement
     * Space / LCTRL = Move up / down
     * Alt + Left Click = Orbit around the selected object
     * F = Focus camera on selected object
     * Left Shift = Double the camera speed
     
## Fixes & Improvements since Last Release

### **Component Assignment Modularization**
- The assignment of components to GameObjects has been **modularized** for more versatile and maintainable code.  
- Implemented as a **new C++ module** managing GameObjects and their components.

### **FBX Import Fix**
- Previously, only the **first mesh** of an FBX file was loaded.  
- Now, **all meshes** in an FBX are imported and added as separate components to the GameObject.

### **Texture Import**
- When an FBX is imported, its **assigned texture** is also imported automatically.  
- The engine now stores a **reference** to the imported texture alongside the mesh.

### **Hierarchy Improvements**
- The **GameObject hierarchy** has been fixed and improved:  
  - Adding child GameObjects works reliably.  
  - Changing a GameObject’s parent now **properly updates all children**.

## Main Functionalities

### **Scene & Serialization**
- The **StreetEnvironment.fbx** scene loads automatically at application startup.  
- Scenes can be **serialized to disk** and later **loaded**, preserving hierarchy, transforms, meshes, textures, and component data.  
- All imported **models, meshes, and textures** are converted and stored using a **custom internal format** inside the `/Library` folder.  

### **GameObjects & Hierarchy**
- Each entity in the scene is represented as a **GameObject**.  
- The **Hierarchy Window** allows full scene structure editing:  
  - **Create empty GameObjects**.  
  - **Create child GameObjects**.  
  - **Delete GameObjects**.  
  - **Reparent GameObjects** by changing their parent.  
- GameObjects can be **selected directly in the scene view** using the mouse.  

### **Components & Inspector**
- GameObjects expose editable components through the **Inspector Window**, including:
  - **Transform** (position, rotation, scale)
  - **Mesh** and **Texture** assignment via selection or drag & drop
  - **Camera** component with configurable parameters

### **Camera & Interaction**
- Unity-like camera system with free look, orbit, zoom, focus, and adjustable movement speed.
- Full camera controls are detailed in the **Camera Controls** section.

### **Rendering & Spatial Optimization**
- All meshes generate an **Axis-Aligned Bounding Box (AABB)**.  
- **Frustum Culling** is applied to discard non-visible objects during rendering.  
- Culling and selection processes can be **visualized in the editor**, including:  
  - **AABB bounding boxes**.  
  - **Debug raycasts** used for object selection.  
- An **Octree** is used as an acceleration structure for:  
  - **Mouse picking / selection**.  
  - **Frustum culling optimization**.  

### **Simulation Control**
- The engine supports **Play / Pause / Stop** simulation states.  
- Stopping the simulation **restores the scene to its initial state** prior to execution.  

### **Assets & Resource Management**
- An **Assets Window** displays all project assets using a **tree-based visualizer**.  
- Users can **import assets** via **drag & drop**; the Assets window updates accordingly.  
- Assets can be **deleted** from the editor, automatically removing their corresponding files from `/Library`.  
- During engine startup:  
  - All unmanaged resources are processed and stored in `/Library`.  
  - The `/Library` folder is **fully regenerated** from `/Assets` and associated `.meta` files if needed.  
- Resources are **reference-counted**, ensuring:  
  - A mesh or texture exists **only once in memory**, regardless of how many GameObjects reference it.  
  - Resource usage and reference counts can be **visualized in the editor**.  

### **Editor Windows**
- **Console:**  
  Logs application flow, errors, resource loading, and engine events.

- **Configuration Window:**  
  - Displays an **FPS graph**.  
  - Allows real-time adjustment of engine and module settings.  
  - Shows system and hardware information.  

- **Hierarchy Window:**  
  Displays all GameObjects in the scene and allows direct selection and hierarchy editing.  

- **Inspector Window:**  
  Displays and edits component data for the selected GameObject.  

- **Toolbar / Main Menu:**  
  - **File:** Scene and application controls.  
  - **View:** Toggle editor windows.  
  - **Help:** Access documentation, bug reporting, releases, and engine information.  
## Additional Functionalities

### **Camera Management**
- Multiple **Camera GameObjects** can be created and stored within a scene.  
- Cameras are **saved and loaded** as part of the scene serialization process.  
- Any camera can be set as the **Main Camera** at runtime, allowing seamless switching between different viewpoints.  

### **GameObject Deletion**
- GameObjects can be **deleted individually** from the scene.  
- Deleting a GameObject also removes **all of its child GameObjects**, preserving hierarchy consistency.  

### **Scene Saving**
- Scenes support a **“Save As”** workflow.  
- Users can:  
  - Specify a **custom file name**.  
  - Choose the **destination path** where the scene file will be saved.  

### **Scene Management**
- **Clear Scene** functionality allows users to remove **all GameObjects** from the current scene.  
- This behaves similarly to creating a **new empty scene**, providing a clean starting point for further work.  

### **Texture Transparency Support**
- Imported model textures can now include **transparency (alpha channel)**.  
- Transparency is correctly applied to the model in the scene, preserving visual fidelity for transparent materials.

## Credits
**All contributors working on this project**:

_Joel Vicente_ « **Github**: [Jowy02](https://github.com/Jowy02)

_Arthur Cordoba_ « **Github**: [000Arthur](https://github.com/000Arthur)

_Jana Puig_ « **Github**: [JanaPuig](https://github.com/JanaPuig)
