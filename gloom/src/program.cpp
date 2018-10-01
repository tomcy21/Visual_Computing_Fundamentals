// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include "floats.hpp"
#include "mesh.hpp"
#include <cmath>


//global : path and start position
Path path1 = Path("coordinates_2.txt");
float2 startPosition(0.0,0.0);


void runProgram(GLFWwindow* window)
{
    // Enable depth (Z) buffer (accept "closest" fragment)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Configure miscellaneous OpenGL settings
    glEnable(GL_CULL_FACE);

    // Set upt transparency 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Set default colour after clearing the colour buffer
    // glClearColor(0.0f, 0.0f, 0.4f, 0.0f) ;

    Gloom::Shader shader;
    shader.attach("../gloom/shaders/simple.vert");
    shader.attach("../gloom/shaders/simple.frag");
    shader.link();

    int VAO_leftLeg, VAO_rightLeg, VAO_leftArm, VAO_rightArm,VAO_torso, VAO_head, VAO_chessboard;
    float4 tileColour1( 0.3f, 0.9f , 0.7f , 1.0f );
    float4 tileColour2( 0.9f, 0.7f , 0.3f , 1.0f );
    Mesh chessboard("chessboard terrain");
    MinecraftCharacter steve = loadMinecraftCharacterModel("../gloom/res/steve.obj");
    chessboard = generateChessboard( CHESSBOARD_WIDTH, CHESSBOARD_HEIGHT, CHESSBOARD_TILE_WIDTH, tileColour1, tileColour2 );

    int indexChessboard = chessboard.indices.size();
    int indexHead = steve.head.indices.size();
    int indexTorso = steve.torso.indices.size();
    int indexLeftLeg = steve.leftLeg.indices.size();
    int indexRightLeg = steve.rightLeg.indices.size();
    int indexLeftArm = steve.leftArm.indices.size();
    int indexRightArm =steve.rightArm.indices.size();


    VAO_leftLeg = setUpVAO(steve.leftLeg.vertices,steve.leftLeg.vertices.size()*4,
        steve.leftLeg.colours,steve.leftLeg.colours.size()*4,
        steve.leftLeg.indices,indexLeftLeg);

    VAO_rightLeg = setUpVAO(steve.rightLeg.vertices,steve.rightLeg.vertices.size()*4,
        steve.rightLeg.colours,steve.rightLeg.colours.size()*4,
        steve.rightLeg.indices,indexRightLeg);

    VAO_leftArm = setUpVAO(steve.leftArm.vertices,steve.leftArm.vertices.size()*4,
        steve.leftArm.colours,steve.leftArm.colours.size()*4,
        steve.leftArm.indices,indexLeftArm);

    VAO_rightArm = setUpVAO(steve.rightArm.vertices,steve.rightArm.vertices.size()*4,
        steve.rightArm.colours,steve.rightArm.colours.size()*4,
        steve.rightArm.indices,indexRightArm);

    VAO_torso = setUpVAO(steve.torso.vertices,steve.torso.vertices.size()*4,
        steve.torso.colours,steve.torso.colours.size()*4,
        steve.torso.indices,indexTorso);

    VAO_head = setUpVAO(
        steve.head.vertices,steve.head.vertices.size()*4,
        steve.head.colours,steve.head.colours.size()*4,
        steve.head.indices,indexHead);                            
                        

    VAO_chessboard = setUpVAO(
        chessboard.vertices,chessboard.vertices.size()*4,
        chessboard.colours,chessboard.colours.size()*4,
        chessboard.indices,indexChessboard);  


    SceneNode* sceneGraph;
    sceneGraph = sceneGraphCreation(VAO_chessboard, VAO_head, VAO_torso, VAO_leftLeg, VAO_rightLeg, VAO_leftArm, VAO_rightArm,
         indexChessboard, indexHead, indexTorso, indexLeftLeg, indexRightLeg, indexLeftArm, indexRightArm);


    //uniform variable to communicate with the vertex shader
    //GLint unifromId = glGetUniformLocation(VAO_leftLeg, "camera");
    
    float deltaTime = getTimeDeltaSeconds();
    float time = 0.0f;
    float ArmAndLegMotion = 0.0;
    // Declaration of the transformation matrices
    glm::mat4 ModelViewMatrix;
    glm::mat4 fowardBackwardMatrix ;
    glm::mat4 upwardDownwardMatrix ;
    glm::mat4 sidewaysMatrix ;
    glm::mat4 yawRotationMatrix ;
    glm::mat4 pitchRotationMatrix ;
    glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 4.0f/3.0f , 0.1f, 1000.0f);
    
    //structure 
    structMotion motion = { 0.0,30.0,-100.0,0.0,0.0,0.5,0.01};
 
    // Rendering Loop
    while (!glfwWindowShouldClose(window))
    {
        shader.activate();

        //body motion in function of the time
        deltaTime = getTimeDeltaSeconds();
        time += deltaTime;
        ArmAndLegMotion = sin(SPEED_FACTOR_ARM_LEG_MOTION*time);
        updateBodyMotion(ArmAndLegMotion, sceneGraph, deltaTime);

        //keybord instructions for moving the camera. 
        handleKeyboardInput(window, &motion);
        
        //define the transformation matrixes depending of the keys pressed. 
        fowardBackwardMatrix = glm::translate(glm::mat4(),glm::vec3(0.0,0.0,motion.z));
        sidewaysMatrix = glm::translate(glm::mat4(),glm::vec3(motion.x,0.0,0.0));
        upwardDownwardMatrix = glm::translate(glm::mat4(),glm::vec3(0.0,-motion.y,0.0));
        yawRotationMatrix = glm::rotate(glm::mat4(),-motion.yawRot,glm::vec3(0.0,1.0,0.0));
        pitchRotationMatrix = glm::rotate(glm::mat4(), -motion.pitchRot,glm::vec3(1.0,0.0,0.0));

        //transform the scene with the good matrix
        ModelViewMatrix = Projection*yawRotationMatrix*pitchRotationMatrix*fowardBackwardMatrix*sidewaysMatrix*upwardDownwardMatrix;
        // Clear colour and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // change the color of the background to dark blue
        glClearColor(0.4f, 0.1f, 0.7f, 1.0f) ;

        visitSceneNode(sceneGraph, ModelViewMatrix);
        
        // Handle other events
        glfwPollEvents();

        // Flip buffers
        glfwSwapBuffers(window);

        shader.deactivate();
    
    }
    shader.destroy();
}


void handleKeyboardInput(GLFWwindow* window, struct structMotion *m)
{
    // Use escape key for terminating the GLFW window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_Q)==GLFW_PRESS)
    {
        m->x += m->step ;
    }
    if (glfwGetKey(window, GLFW_KEY_E)==GLFW_PRESS)
    {
        m->x -= m->step ;
    }
    if (glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS)
    {
        m->y += m->step ;
    }
    if (glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS)
    {
        m->y -= m->step ;
    }
    if (glfwGetKey(window, GLFW_KEY_R)==GLFW_PRESS)
    {
        m->z += m->step ;
    }
    if (glfwGetKey(window, GLFW_KEY_F)==GLFW_PRESS)
    {
        m->z -= m->step ;
    }
    if (glfwGetKey(window, GLFW_KEY_T)==GLFW_PRESS)
    {
        m->yawRot += m->stepRot ;
    }
    if (glfwGetKey(window, GLFW_KEY_U)==GLFW_PRESS)
    {
        m->yawRot -= m->stepRot;
    }
    if (glfwGetKey(window, GLFW_KEY_Y)==GLFW_PRESS)
    {
        m->pitchRot += m->stepRot ;
    }
    if (glfwGetKey(window, GLFW_KEY_H)==GLFW_PRESS)
    {
        m->pitchRot -= m->stepRot ;
    }

}

int setUpVAO( 
    std::vector<float4> vertices, int size_of_vertex,
    std::vector<float4> colours, int size_of_color,
    std::vector<unsigned int> indices, int size_of_indices)
{
    unsigned int vertexArrayID = 0; 
    int count = 1; 
    unsigned int VBO_ID = 0;
    unsigned int VBO_ID1 = 0;  
    unsigned int bufferIndex = 0;
    
// VAO creation
    glGenVertexArrays(count, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

// VBO vertex setup
    glGenBuffers(count, &VBO_ID);
    glBindBuffer(GL_ARRAY_BUFFER , VBO_ID);
    glBufferData(GL_ARRAY_BUFFER, size_of_vertex*sizeof(float), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

// VBO color setup
    glGenBuffers(count, &VBO_ID1);
    glBindBuffer(GL_ARRAY_BUFFER , VBO_ID1);
    glBufferData(GL_ARRAY_BUFFER, size_of_color*sizeof(float), &colours.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);


//Index buffer
    glGenBuffers(count, &bufferIndex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,size_of_indices*sizeof(int), &indices.front(),GL_STATIC_DRAW);

    
    return(vertexArrayID);
}

SceneNode* sceneGraphCreation(
    int VAO_chessboard, int VAO_head, int VAO_torso, 
    int VAO_leftLeg, int VAO_rightLeg, int VAO_leftArm, int VAO_rightArm,
    int indexChessboard, int indexHead, int indexTorso, int indexLeftLeg,
    int indexRightLeg, int indexLeftArm, int indexRightArm){

    SceneNode* root = createSceneNode();
    SceneNode* terrain = createSceneNode();
    SceneNode* character = createSceneNode();
    SceneNode* head = createSceneNode();
    SceneNode* torso = createSceneNode();
    SceneNode* leftArm = createSceneNode();
    SceneNode* rightArm = createSceneNode();
    SceneNode* leftLeg = createSceneNode();
    SceneNode* rightLeg = createSceneNode();

    addChild(torso, head);
    addChild(torso, leftArm);
    addChild(torso, rightArm);
    addChild(torso, leftLeg);
    addChild(torso, rightLeg);

    //initialisation of each node. 
    //The position needs to be the reference point at the beginning
    //Important to fixe the VAO and the Index Count of each node. 
    float3 referencePoint(0.0,0.0,0.0);
    terrain->position = referencePoint; 
    terrain->vertexArrayObjectID = VAO_chessboard;
    terrain->referencePoint = referencePoint;
    terrain->VAOIndexCount = indexChessboard;

    referencePoint = {-2.000000, 12.000000, 0.000000};
    leftLeg->position = referencePoint;
    leftLeg->vertexArrayObjectID = VAO_leftLeg;
    leftLeg->referencePoint = referencePoint;
    leftLeg->VAOIndexCount = indexLeftLeg;

    referencePoint = {2.000000, 12.000000, 0.000000};
    rightLeg->position = referencePoint;
    rightLeg->vertexArrayObjectID = VAO_rightLeg;
    rightLeg->referencePoint = referencePoint;
    rightLeg->VAOIndexCount = indexRightLeg;

    referencePoint = {4.000000, 22.000000, 0.000000};
    rightArm->position = referencePoint;
    rightArm->vertexArrayObjectID = VAO_rightArm;
    rightArm->referencePoint = referencePoint;
    rightArm->VAOIndexCount = indexRightArm;

    referencePoint = {-4.000000, 22.000000, 0.000000};
    leftArm->position = referencePoint;
    leftArm->vertexArrayObjectID = VAO_leftArm;
    leftArm->referencePoint = referencePoint;
    leftArm->VAOIndexCount = indexLeftArm;

    referencePoint = {0.000000, 12.000000, 0.000000};
    torso->position = referencePoint;
    torso->vertexArrayObjectID = VAO_torso;
    torso->referencePoint = referencePoint;
    torso->VAOIndexCount = indexTorso;

    referencePoint = {0.000000, 24.000000, 0.000000};
    head->position = referencePoint;
    head->vertexArrayObjectID = VAO_head;
    head->referencePoint = referencePoint;
    head->VAOIndexCount = indexHead;
  

    addChild(character, torso);
    addChild(root, character);
    addChild(root, terrain);
    return root;
}

void visitSceneNode(SceneNode* node, glm::mat4 transformationThusFar)
{
    // Do transformations here

    glm::mat4 translation_origin = glm::translate(glm::mat4(),
                                glm::vec3(   - node->position.x,
                                             - node->position.y,
                                             - node->position.z));

     glm::mat4 translation_back_to_position = glm::translate(glm::mat4(),
                                glm::vec3(    node->position.x,
                                              node->position.y,
                                              node->position.z));

    glm::mat4 translation = glm::translate(glm::mat4(),
                                glm::vec3(  node->position.x - node->referencePoint.x,
                                            node->position.y - node->referencePoint.y,
                                            node->position.z - node->referencePoint.z));   
    
    glm::mat4 rotation_x = glm::rotate(glm::mat4(),node->rotation.x,glm::vec3(1.0,0.0,0.0));                                    
    glm::mat4 rotation_y = glm::rotate(glm::mat4(),node->rotation.y,glm::vec3(0.0,1.0,0.0));
    glm::mat4 rotation_z = glm::rotate(glm::mat4(),node->rotation.z,glm::vec3(0.0,0.0,1.0));
    
    node->currentTransformationMatrix = transformationThusFar* translation_back_to_position  * rotation_x * rotation_y * rotation_z * translation_origin * translation;

    glm::mat4 modelMatrix  =  node->currentTransformationMatrix ;

    // Render only nodes with correct VAO
    if( node->vertexArrayObjectID != -1){

        glUniformMatrix4fv(3,1 ,GL_FALSE, glm::value_ptr(modelMatrix));
        glBindVertexArray(node->vertexArrayObjectID);
        glDrawElements(GL_TRIANGLES, node->VAOIndexCount , GL_UNSIGNED_INT, 0);
    }

    //take care of the children
    for(SceneNode* child : node->children)
    {
        visitSceneNode(child, modelMatrix);
    }
}

void updateBodyMotion(float ArmAndLegMotion, SceneNode* node, float deltaTime){

    switch( node->vertexArrayObjectID){ //switch case to make different motions for each body part
        case 1: //left leg
            node->rotation.x = ArmAndLegMotion;
            break;
        case 2: //rigth leg
            node->rotation.x = -ArmAndLegMotion;
            break;
        case 3: //left Arm
            node->rotation.x = ArmAndLegMotion;
            break;
        case 4: //right Arm
            node->rotation.x = -ArmAndLegMotion;
            break;
        case 5: //torso
            followThePath(node ,  deltaTime);
            break;
        case 6: //head
            break;
        case 7: //chessboard
            break;
        default:
            break;
    }

    for(SceneNode* child : node->children)
    {
        updateBodyMotion(ArmAndLegMotion,child, deltaTime);
    }  

}


void followThePath(SceneNode* node, float deltaTime){
    
    float2 characterPosition(node->position.x,node->position.z);
    float2 wayPointPosition(path1.getCurrentWaypoint(CHESSBOARD_TILE_WIDTH).x,path1.getCurrentWaypoint(CHESSBOARD_TILE_WIDTH).y);
    float step = deltaTime * SPEED_FACTOR_BODY_MOTION;


    if(!path1.hasWaypointBeenReached(characterPosition, CHESSBOARD_TILE_WIDTH))
    { //if the character has not reach the waypoint

        if(startPosition.x == wayPointPosition.x ){ 
        //if the next waypoint is at same x coordonate than the previous waypoint
            if(characterPosition.y< wayPointPosition.y){
                node->position.z += step;
                node->rotation.y = 0;
             
            }
            if(characterPosition.y > wayPointPosition.y){
                node->position.z -= step;
                node->rotation.y = float(M_PI); 
            } 
                
        }
        else if(startPosition.y == wayPointPosition.y){
        //if the next waypoint is at same y coordonate than the previous waypoint
            if(characterPosition.x< wayPointPosition.x){
                node->position.x += step;
                node->rotation.y = float(M_PI)/2;
            }
            if(characterPosition.x > wayPointPosition.x) {
                node->position.x -= step;  
                node->rotation.y = float(M_PI)*3/2;
            } 
        }else{
            //if x and y of the next waypoint are different from previous waypoint
            float angle = std::atan2((wayPointPosition.y - startPosition.y) ,(wayPointPosition.x - startPosition.x));
            float coef = (startPosition.x - wayPointPosition.x)/(startPosition.y - wayPointPosition.y);

            if(coef > 0) //
            {
                node->rotation.y = angle ;
                if(startPosition.x < wayPointPosition.x)
                {
                    node->position.x += coef * step;
                    node->position.z += step;
                }else
                {
                    node->position.x -= coef * step;
                    node->position.z -= step;   
                }
            }
            else
            {
                node->rotation.y = angle + float(M_PI) ;
                if(startPosition.y > wayPointPosition.y)
                {
                    node->position.x -= coef * step;
                    node->position.z -= step;
                }else
                {
                    node->position.x += coef * step;
                    node->position.z += step;   
                }
            }
        }

     }else{
        startPosition = path1.getCurrentWaypoint(CHESSBOARD_TILE_WIDTH);
        path1.advanceToNextWaypoint();
     }
        
}