Hello, this project is a game and/or game engine. Both of these components are as yet unnamed. I want to try to wor
The engine will likely not have a standalone editor etc but the project will hopefully be designed in a way that allows me to repurpose the engine code for use in future endeavors.

This project is for educational purposes though I hope to keep working on it long term.
I learned the basics of OpenGL in late July 2024 and began this project shortly after. As I am still a college student and new to graphics and engine programming (and large-scale programming in general), progress happens slowly.

Currently (2024-09-04) the main branch loads up a simple scene containing a 3d model with textures and normal maps, a hexagonal grid shader, a small hex map that responds to being moused over, and dynamic lighting.
Use WASD to move around the scene, spacebar to move up, and C to move down.

Forest monster asset from https://opengameart.org/content/forest-monster and is in the public domain.

External libraries used:
GLAD for OpenGL function handles.  
GLFW for window creation.  
GLM for vector/matrix/quaternion math.  
stb_image for loading texture files.  
Assimp for loading 3d models and materials.
