#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace glm;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void window_position_callback(GLFWwindow* window, int xPos, int yPos);
void processInput(GLFWwindow* window);
void compileShaderAndLogErrors(unsigned int shaderId, const char* errMsgPrefix, const char* successMsg);
void ImGuiSetup(GLFWwindow* window);

// the input to the shader can be named anything, we called it aPos.
// the ouput always has to be called gl_Position and is always a vec4.
// we have 3 vertex attributes (location=X).
// we also have 2 outputs (ourColor and TexCoord) that are used only for forwarding the
// vertex attributes aColor and aTexCoord into the next shader, which is the fragment shader.
const char* vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout(location = 1) in vec3 aColor;\n"
    "layout(location = 2) in vec2 aTexCoord;\n"
    "\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "\n"
    "uniform mat4 transform;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   ourColor = aColor;\n"
    "   TexCoord = aTexCoord;\n"
    "   gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char* fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"
    "\n"
    "uniform sampler2D ourTexture;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    // the following line does glorious vertex coloring:\n"
    "    // FragColor = vec4(ourColor.r, ourColor.g, ourColor.b, 1.0f);\n"
    "\n"
    "    FragColor = texture(ourTexture, TexCoord);"
    "}\0";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // this has to be done when on mac:
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create a window - works cross platform. Without GLFW we'd have to do
    // platform-specific window creation

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    ImGuiSetup(window);

    // This just tells GLAD what is the address of the function 
    // (called glfwGetProcAddress) which gets all the addresses
    // of the openGL functions of the GPU driver. 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // These first 2 numbes specify the position within the
    // window of the lower left corner.
    // The 3rd and 4th specify the size of the rendering
    // window. We could make it smaller than the actual window size.
    glViewport(0, 0, 800, 600);

    // what function to call when the window gets resized and moved
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowPosCallback(window, window_position_callback);

    // rendering stuff initialization before the rendering loop -----------------------------------------
    
    // 3d cube
    // 3d cube
    float vertices[] = {
        // positions           // colors           // texture coords

        // front face
         0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // top left

        // back face
         0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f,   // top right
         0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, -0.5f,   0.5f, 0.5f, 0.5f,   1.0f, 1.0f,   // top left

        // left face
        -0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // top left

        // right face
         0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // bottom right
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   // bottom left
         0.5f,  0.5f,  0.5f,   0.5f, 0.5f, 0.5f,   0.0f, 1.0f,   // top left

         // top face
          0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
          0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
         -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
         -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // top left

         // bottom face
          0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // top right
          0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // bottom right
         -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   // bottom left
         -0.5f, -0.5f,  0.5f,   0.5f, 0.5f, 0.5f,   0.0f, 1.0f    // top left
    };

    unsigned int indices[] = {
        // front face
         0,  3,  1,
         1,  3,  2,

         // back face
          4,  5,  7,
          5,  6,  7,

          // left face
           8, 11,  9,
           9, 11, 10,

           // right face
           12, 15, 13,
           13, 15, 14,

           // top face
           16, 19, 17,
           17, 19, 18,

           // bottom face
           20, 23, 21,
           21, 23, 22
    };

    // Preparing the VBO, EBO and VAO --------------------------------------------------------------------

    // create a vertex buffer object. It stores vertex attributes. A vertex attribute
    // is all the data associated with a vertex. Must be position but can also be
    // for example color in case we wanna do some kind of vertex based coloring, I guess
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // The element buffer object stores the indices with which triangles are constructed
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // The vertex attribute object. Once we bind it, binding VBO and EBO will be remembered in the VAO.
    // So the VAO saves us time so we dont have to bind those 2 over again.
    // The VAO thus remembers:
    //  - what VBO is being used
    //  - how to interpret the data inside the VBO - done later via
    //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); )
    //  - what EBO is being used
    // Whatever the last VBO and EBO was bound will me remembered. However, if you unbind any of
    // those BEFORE unbinding the VAO, then the VAO will also remember that unbinding and thus will
    // have no associated EBO or VBO. So always unbind the VAO first and the EBO and VBO next.
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);



    // VAO binding and VBO and EBO config ---------------------------------------------------------------

    // Now everything regarding vertex shader config (the stride, etc), will be "stored" in this VAO.
    // A VAO literally just remembers:
    //  - how are the bytes in the associated VBO supposed to be interpreted. E.g. if every vertex remembers
    //    3 floats for the position attribute, followed by 2 floats for the uv coordinates, then
    //    the VAO remembers that at location=0 we have 3 floats and at location=1 we have 2 floats.
    //  - the associated VBO
    //
    // NOTE: If we had 2 objects whose vertices and indices would be stored in different VBOs and EBOs,
    // then in the rendering loop we'd have to switch the VAO every time we were drawing a different object.
    //
    // NOTE 2: the function name is a bit of a misnomer. its called bindVertexArray even though we're binding
    // a vertex attribute object
    glBindVertexArray(VAO);
    

    // set VBO to be the currently bound buffer. All subsequent buffer calls will operate 
    // on it.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // configure the EBO. Subsequent draw calls will be using this (provided we draw via
    // glDrawElements and NOT glDrawArrays. glDrawElements is more efficient since
    // the whole point of EBO is to reduce the number of stored vertices by being able
    // to reuse 'em)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // Tell the VAO how to interpret the data in the VBO via glVertexAttribPointer:
    // 
    // glVertexAttribPointer parameters explained (in order):
    //   0 - which vertex attribute we are configuring. Here it's location=0
    //   3 - size of the vertex attribute (how many "things" it consists of)
    //   GL_FLOAT - data type of each of those "things". Here we said "we have 3 floats"
    //   GL_FALSE - if we want the data to be normalized. Idk what that means, not important rn
    //   3 * sizeof(float - the STRIDE. Tells us how many bytes to hop in order to find the next 
    //       vertex attribute.
    //   (void*)0 - kinda weird but essentially says the offset where the attribute starts. 
    //   Start offset in bytes of the attribute
    //
    // vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // just turns the above on
    //
    // vertex colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //
    // uv's
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    
    // uploading data to the VBO and EBO -------------------------------------------------------------------

    // a.k.a uploading the vertex data and the index data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // shaders compilation, error checking and linking into a shader program ------------------------------

    // vertex shader. Typically these at the very least turn the raw 3d coordinates
    // of the model into NDC. We dont do that cos our shader is simple as can be.
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    compileShaderAndLogErrors(
        vertexShader, "Error compiling vertex shader: \0", "Vertex shader compilation succesful\0");

    

    // fragment shader. Always has the out vec4 FragColor as the ouput
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    compileShaderAndLogErrors(
        fragmentShader, "Error compiling fragment shader: \0", "Fragment shader compilation succesful\0");


    
    // linking the shaders into a shader program. the output of one shader must match the input of the other.
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // reporting success/failure of linking
    int  success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Error linking shaders: " << infoLog << std::endl;
    }
    else {
        std::cout << "Succesfully linked shaders" << std::endl;
    }

    // We don't need the shaders anymore since they have already been compiled
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // use the shader for all subsequent rendering.
    // Materials in rendering engines essentially do this exact thing.
    glUseProgram(shaderProgram);



    // texture stuff ---------------------------------------------------------------------------------

    // load texture image into the data array
    int width, height, nrChannels;
    unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);

    // create the texture object
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // copy the data into the texture.
    // more info about this long ahh function: https://learnopengl.com/Getting-started/Textures
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // since we copied the data, we can free the resource
    stbi_image_free(data);

    // settings for the currently bound texture 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // transformations stuff --------------------------------------------------------------------------

    // we only get the matrix' uniform. everything else is done in the render loop because 
    // we change the transform every frame
    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");

    // IMGUI-editable
    float xCamPos = 0.0; 
    float yCamPos = -1.0;
    float zCamPos = -5.0;
    float xModelRot = -90.0;
    float yModelRot = 0.0;
    float zModelRot = 0.0;
    float bgColor[3] = { 0.91f, 0.55f, 0.1 };

    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // rendering loop ---------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    { 
        // input
        glfwPollEvents();         // poll events checks key presses, window resize/move, etc.
        processInput(window);

        // imgui start
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Begin("Settings");
        ImGui::SliderFloat("xCamPos", &xCamPos, -5.0f, 5.0f);
        ImGui::SliderFloat("yCamPos", &yCamPos, -5.0f, 5.0f);
        ImGui::SliderFloat("zCamPos", &zCamPos, -5.0f, 5.0f);
        ImGui::Spacing();
        ImGui::SliderFloat("xModelRot", &xModelRot, -180.0f, 180.0f);
        ImGui::SliderFloat("yModelRot", &yModelRot, -180.0f, 180.0f);
        ImGui::SliderFloat("zModelRot", &zModelRot, -180.0f, 180.0f);
        ImGui::Spacing();
        ImGui::ColorEdit3("Background", bgColor);
        ImGui::End();
        

        // do all the rendering here
        // ...
        glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        
        // create a model matrix that does the following thing:
        //  - model scale stays unchanged
        //  - the model is rotated 90 degrees about the x axis
        //  - it's world space position shall be 0, 0, 0 (so unchanged)
        mat4 modelMatrix = mat4(1.0);
        modelMatrix = rotate(modelMatrix, radians(xModelRot), vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = rotate(modelMatrix, radians(yModelRot), vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = rotate(modelMatrix, radians(zModelRot), vec3(0.0f, 0.0f, 1.0f));

        // view matrix aka camera matrix. it says what the position transform of the camera is
        mat4 viewMatrix = mat4(1.0);
        viewMatrix = translate(viewMatrix, vec3(xCamPos, yCamPos, zCamPos));

        // projection matrix
        mat4 projectionMatrix = mat4(1.0);
        projectionMatrix = perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);


        mat4 finalTransMatrix = mat4(1.0f);

        finalTransMatrix = projectionMatrix * viewMatrix * modelMatrix;

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(finalTransMatrix));

        // draw polygons or wireframe?
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);



        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // post rendering stuff
        ImGui::Render();

        // imgui end
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    std::cout << "Rezised window to " << width << ", " << height << "\n";
    glViewport(0, 0, width, height);
}

void window_position_callback(GLFWwindow* window, int xPos, int yPos)
{
    std::cout << "Window moved to: " << xPos << ", " << yPos << '\n';
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void compileShaderAndLogErrors(unsigned int shaderId, const char* errMsgPrefix, const char* successMsg) {
    
    glCompileShader(shaderId);

    // check compilation status and log errors

    int  success;
    char infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cout << errMsgPrefix << infoLog << std::endl;
    }
    else {
        std::cout << successMsg << std::endl;
    }
}

void ImGuiSetup(GLFWwindow* window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}