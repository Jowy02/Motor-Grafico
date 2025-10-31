#pragma once
#include <string>

class GuiControl;

class Module
{
public:
    Module() : active(false) {}

    // Activate the module
    void Init() { active = true; } 

    virtual bool Awake() { return true; }            // Called before render is available
    virtual bool Start() { return true; }            // Called before the first frame
    virtual bool PreUpdate() { return true; }        // Called before each loop iteration
    virtual bool Update(float dt) { return true; }   // Called each loop iteration
    virtual bool PostUpdate() { return true; }       // Called after each loop iteration
    virtual bool CleanUp() { return true; }          // Called before quitting

    // GUI callback for mouse clicks
    virtual bool OnGuiMouseClickEvent(GuiControl* control) { return true; } 

public:
    std::string name; // Module name
    bool active;      // Whether the module is active
};