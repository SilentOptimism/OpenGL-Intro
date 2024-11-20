#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

//The coordinates of our vertices are in 2d 
/*
	origin of our x and y axis is in the center our screen
	functioning like any normal 2d cartesian vector space

	We are using a normalized coordinate system where the left most
	x is -1 and the right most coord is 1, this is also the case for
	y where the top most coord is 1 and the bottom most coord is -1

*/

// Applys no transformation to the vertex's as we are in the 2d plane
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"	gl_Postion = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// Chooses a random color for all of the vertex's
const char* fragementShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f); \n"
"}\n\0";


int main() 
{


	if (!glfwInit()) {
		std::cout << "glfw failed to init" << std::endl;
		return -1;
	}

	// sets the glfw version to the opengl version we are using
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Sets the profile we are using core(modern) or compatability(legacy/modern)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Shaders, buffers, textures, etc are objects stored in the GPU memory and managed by OpenGL
	// These objects are managed and created by the OpenGl library
	// When an object is created OpenGl gives you a psuedo memory address
	// that it uses to identify the object in question
	// We can use this psuedo memory address in OpenGl functions to do interact with these objects
	
	// So if we want ot render our vertices we need to create a series of Shaders OpenGl objects that will
	// transform our vertexData until it is ready to be written to the back buffer
	
	// A Simple standardized OpenGl float variable: we could use normal floats but those differ in size depending on platform 

	// The vertex data
	GLfloat vertices[] = {
		//(x, y, z) these need to be between -1 and 1
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // left coorner
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // Right corner
		0.0f, 0.5f * float(sqrt(3)) / 3, 0.0f // top corner
	};


	// GLunint == an unisgned integer 
	//vertexShader == the name of our unsigned integer
	// glCreateShader(GL_VERTEX_SHADER) == Tells OpenGl to create an empty vertex shader object and return an unsigned integer 
	// which will function as a reference to it in OpenGl functions
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Fills our former empty shader by inputing the
	// unsigned int designating the shader, the amount of shaders we have, the address of the shader source code, and NULL
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

	// Currently the source code of our shader is an array of chars
	// This literally useless and the gpu can't understand this
	// glCompileShader() compilers our shader source code from 
	// an array of characters to machine code the gpu can process
	glCompileShader(vertexShader);


	// Explained above
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragementShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Don't forget our GLuint is just an unsigned integer and our "shader program" is just a reference to this integer
	// To use our shaders we need to connect them into a Graphics pipeline
	// to do this we need to create a shader program to string all of our shaders together
	GLuint shaderProgram = glCreateProgram();

	// Attaches our newly created shader machine code to the shader program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// Now attaches our created shader program to our back buffer
	glLinkProgram(shaderProgram);

	// Tells OpenGl to delete our shader objects as we have already copied them to our shader program
	// since they have already been assembled into a working shader program we will no longer need them
	// When actually developing our game we will probably not want to do this as we will constantly need to 
	// assemble and create a series of different shader programs (though we could pregenerate them at the begining)
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	// When working in OpenGl or just with the gpu in general we work in buffers as 
	// to transport data between the cpu and gpu is incredibly slow
	
	// Now that we have our shader assembled and attached to the back buffer we need to send it our vertex's
	// To this we use a vertex buffer or more simply a VERTEX BUFFER OBJECT or VBO for short

	// The unsigned integer that will be our reference to our vertex buffer
	// the VBO is usually an array of references but due to us having only 1 object we only
	// need 1 unsigned integer rather than an array of unsigned integers
	GLuint VBO;

	// Points to 1 or more VAO's and tells OpenGl how to interpret them
	// VAO's organize all of our VBO's and tell OpenGl how to switch between them
	GLuint VAO;
	
	// We have only 1 object pointing to the VAO
	// This NEEDS to be generated before the VBO VERY IMPORTANT
	// VAO's exist in order to quickly switch between different VBO's
	glGenVertexArrays(1, &VAO);

	// Generating the buffer on the openGl stack
	// We only have 3d 1 object that being our triangle and we give it the address of where we want to store the reference to the VBO
	// We would have specify how big our VBO would be with our first argument but since we only have 1 object we say 1
	// Simply generates a buffer
	glGenBuffers(1, &VBO);

	

	//Bind's the VAO as the Vertex Array
	glBindVertexArray(VAO);
	
	// Binding an object makes that object the current object and when we activate a function that would interact with that type of object
	// So when OpenGl needs to access the GL_ARRAY_BUFFER it will access the VBO since we binded our recently generated buffer to it
	// Simply binds Array Buffer to our generated buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Stores our vertices in the VBO and creates a nicely filled GL_ARRAY_BUFFER
	// glBufferData(What buffer are we filling?, the size of the vertices, the actual vertices, Specify use of the data);
	// Specify use of data: stream, dynamic, static
		// These will need to be specified to maximize performance
		// Stream: vertices will be modified once and used a few times
		// Static: vertices will be modified once and used MANY MANY TIMES
		// Dynamic: vertices will be modifed MULTIPLE TIMES and be used MANY MANY TIMES
	// Next you need to specify: DRAW, READ, COPY
		// Draw: The vertices will be modified and used to drawn an image to the screen
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// OUR VBO IS JUST A MAJOR CHUNK OF ALL OF OUR VERTEX DATA OUR VAO IS THE INDEX'S OF ALL VERTEX'S IN OUR VBO
	// WIll be covered in more detail in the Shader Chapter
	// A vertex attribute is a way of communicating with a vertex shader from the outside
	// (position of vertex attribute, values per vertex, type of values, THIS INPUT ONLY MATTERS IF USING INTEGERS, stride of our vertices "the amount of space between each vertex", where the vertices begin in the array) 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);

	// Assembles our vertex attribute array with our vertex attrib pointer
	// You need to give it the position of our vertex attribute
	glEnableVertexAttribArray(0);
	
	// Non mandatory Unbinds our VAO and VBO now that the VertexArray and ArrayBuffer's are filled and their pointers are setup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	// Makes a pointer to a windows object
	// width, height, NAME, windowed, unimportant
	GLFWwindow* iMadeThisWindow = glfwCreateWindow(800, 800, "YoutubeOpenGl",NULL, NULL);

	if (iMadeThisWindow == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Make's the window part of the current context (the context is the general object/environment/think the viewer sees)
	glfwMakeContextCurrent(iMadeThisWindow);

	// Loads glad config for opengl?
	gladLoadGL();

	// Tells opengl the viewport of our window (the space we want it to render in)
	// (0,0): bottom left coord (400,225): top right coord
	glViewport(0, 0, 800, 800);





	// Check's to see if the window is closed and exits the loop if it does
	// Now it also swaps our frames
	while (!glfwWindowShouldClose(iMadeThisWindow)) {


		// Sets up our background by applying the newcolor buffer to the back buffer
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Turns on our shaderprogram
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		// Tells opengl what type of primative we are using
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Swaps our buffers giving making our former back buffer our front buffer and than sets our front buffer as our back buffer (For next frame gen)
		glfwSwapBuffers(iMadeThisWindow);




		//Tells GLFW to process the window appearing being resized and etc
		//Without it the window will be unresponsive
		glfwPollEvents();

	}


	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	
	glfwDestroyWindow(iMadeThisWindow); //Destroys the GLFWWindow object our ptr points to
	glfwTerminate(); //Turns of glfw
	return 0;
}