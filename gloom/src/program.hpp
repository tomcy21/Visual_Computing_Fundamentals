#ifndef PROGRAM_HPP
#define PROGRAM_HPP
#pragma once


// System headers
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>
#include "floats.hpp"
#include <vector>
#include "OBJLoader.hpp"
#include "sceneGraph.hpp"
#include <glm/mat4x4.hpp>
#include <math.h>
#include "toolbox.hpp"

#define CHESSBOARD_WIDTH 7
#define CHESSBOARD_HEIGHT 5
#define CHESSBOARD_TILE_WIDTH 10
#define SPEED_FACTOR_ARM_LEG_MOTION 2
#define SPEED_FACTOR_BODY_MOTION 5


// structure for the motion of the camera
struct structMotion {
    float x;
    float y;
    float z; 
    float yawRot; 
    float pitchRot;
    float step ;
    float stepRot;
};

//creation of graph scene 
SceneNode* sceneGraphCreation(
    int VAO_chessboard, int VAO_head, int VAO_torso, 
    int VAO_leftLeg, int VAO_rightLeg, int VAO_leftArm, int VAO_rightArm,
    int indexChessboard, int indexHead, int indexTorso, int indexLeftLeg,
    int indexRightLeg, int indexLeftArm, int indexRightArm);

//follow the path
void followThePath(SceneNode* node, float deltaTime);


//update the rotation and position field of the nodes, to represent motion
void updateBodyMotion(float ArmAndLegMotion, SceneNode* node, float deltaTime);

// visitSceneNode
void visitSceneNode(SceneNode* node, glm::mat4 transformationThusFar);

// Function for handling keypresses
void handleKeyboardInput(GLFWwindow* window, struct structMotion *);
// Main OpenGL program
void runProgram(GLFWwindow* window);



// Function which sets up a Vertex Array Object (VAO) containing triangles
int setUpVAO( 
    std::vector<float4> vertices, int size_of_vertex,
    std::vector<float4> colours, int size_of_color,
    std::vector<unsigned int> indices, int size_of_indices);


// Checks for whether an OpenGL error occurred. If one did,
// it prints out the error type and ID
inline void printGLError() {
    int errorID = glGetError();

    if(errorID != GL_NO_ERROR) {
        std::string errorString;

        switch(errorID) {
            case GL_INVALID_ENUM:
                errorString = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_OPERATION:
                errorString = "GL_INVALID_OPERATION";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                errorString = "GL_OUT_OF_MEMORY";
                break;
            case GL_STACK_UNDERFLOW:
                errorString = "GL_STACK_UNDERFLOW";
                break;
            case GL_STACK_OVERFLOW:
                errorString = "GL_STACK_OVERFLOW";
                break;
            default:
                errorString = "[Unknown error ID]";
                break;
        }

        fprintf(stderr, "An OpenGL error occurred (%i): %s.\n",
                errorID, errorString.c_str());
    }
}


#endif
