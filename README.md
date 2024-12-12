# Final Project by Yuehuan Qiu & Yifan Sun
## Game Related Content

Press start to start the game. 

Press G to Jump.

Press J,I,K,L to move leftforward, rightforward, leftbackward, rightbackward

Once reached the rabbit side portal, you can jump twice as high.

Once reached the dragon side portal, you can move twice as fast.

Once travel through the portal too much (set to 3 times for testing), you will enter Land of Nowhere. You may only get out by pressing start to restart. You might want to do that especially when you are stucked.

The developer mode will allow you to move your camera through pressing W,A,S,D and moving mouse.

## Feature & Credit

Collision Detection - Yifan Sun

Random Map Generation (the Land of Nowhere) - Yifan Sun

Game Design - Yuehuan Qiu

Scene Generation - Yuehuan Qiu

Mesh file loading - Yuehuan Qiu

Texture Binding - Yuehuan Qiu

Shadow Mapping - Yuehuan Qiu

FXAA and Edge Filter - Yuehuan Qiu

Game Design - Yuehuan Qiu

## Basic Outline & Data Structure

Load light data and camera data from root json file, generate objects as basicMapFile class and stored them in m_allobjects. BasicMapFile contain modelMatrix, objectType, vbo, vao, textureID, material, hitbox. 

## Demo
![Demo Video](asset/demo.mp4)
