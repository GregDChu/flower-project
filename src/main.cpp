/*
ZJ Wood CPE 471 Lab 3 base code
*/

#include <iostream>
#include <glad/glad.h>
#include <vector>
#include <time.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;
double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d, i, k;
	camera()
	{
		w = a = s = d = i = k = 0;
		pos = glm::vec3(0, 0, -15);
		rot = glm::vec3(0, 0, 0);
	}
	glm::mat4 process(double frametime)
	{
		double ftime = frametime;
		float rotate = 0;
		/*if (a == 1)
		{
			rotate += 10 * ftime;
		}
		else if (d == 1)
		{
			rotate += -10 * ftime;
		}*/
		float speed = 0;
		float z_speed = 0;
		const float speed_multiplier = 10;
		if (w == 1)
		{
			speed = speed_multiplier *ftime;
		}
		else if (s == 1)
		{
			speed = -speed_multiplier *ftime;
		}
		float h_speed = 0;
		/*if (a == 1)
		{
			h_speed += ftime * 5;
		}
		else if (d == 1)
		{
			h_speed += ftime * -5;
		}*/
		if (i)
		{
			z_speed += ftime * speed_multiplier;
		}else if (k)
		{
			z_speed += ftime * -speed_multiplier;
		}
		
		float yangle=0;
		/*if (a == 1)
			yangle = -1*ftime;
		else if(d==1)
			yangle = 1*ftime;*/
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(h_speed, speed, z_speed,1);
		dir = dir * R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;

#define CYLINDER_RING_VERT_COUNT 50
#define CYLINDER_RING_COUNT 2
#define CYLINDER_VERT_COUNT (CYLINDER_RING_VERT_COUNT * CYLINDER_RING_COUNT)
void createCylVerts(vector<GLfloat> * vertBuffer)
{
	//vertBuffer = new float[CYLINDER_VERT_COUNT * 3];
	//*sizePtr = CYLINDER_VERT_COUNT * 3;
	float ring_dist = 1.0 / (CYLINDER_RING_COUNT - 1);
	for (int ring = 0; ring < CYLINDER_RING_COUNT - 1; ring++)
	{
		float angle;
		for (int vert = 0; vert < CYLINDER_RING_VERT_COUNT * 2; vert++)
		{
			angle = ((float)(vert / 2) / CYLINDER_RING_VERT_COUNT) * (3.1415926535 * 2);
			vertBuffer->push_back(0.5 * cos(angle));
			vertBuffer->push_back(ring_dist * (ring + (vert % 2)));
			vertBuffer->push_back(0.5 * sin(angle));
		}
	}
}

void createCylIndices(vector<GLushort> * indBuffer) {
	/*indBuffer->push_back(0);
	indBuffer->push_back(CYLINDER_RING_VERT_COUNT);
	indBuffer->push_back(1);*/
	for (GLushort vert = 0; vert < CYLINDER_VERT_COUNT; vert++)
	{
		indBuffer->push_back(vert);
		indBuffer->push_back((vert + 1) % CYLINDER_VERT_COUNT);
		indBuffer->push_back((vert + 2) % CYLINDER_VERT_COUNT);
	}
}

class Application : public EventCallbacks
{

public:
	int kn = 0;
	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog, shapeprog, shapeprog2, snowshapeprog, shapebug;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexColorIDBox, IndexBufferIDBox;

	Shape shape;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
		if (key == GLFW_KEY_I && action == GLFW_PRESS)
		{
			mycam.i = 1;
		}
		if (key == GLFW_KEY_I && action == GLFW_RELEASE)
		{
			mycam.i = 0;
		}
		if (key == GLFW_KEY_K && action == GLFW_PRESS)
		{
			mycam.k = 1;
		}
		if (key == GLFW_KEY_K && action == GLFW_RELEASE)
		{
			mycam.k = 0;
		}
		if (key == GLFW_KEY_N && action == GLFW_PRESS) kn = 1;
		if (key == GLFW_KEY_N && action == GLFW_RELEASE) kn = 0;
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		string resourceDirectory = "../resources";
		//try t800.obj or F18.obj ...
		shape.loadMesh(resourceDirectory + "/sphere.obj");
		shape.resize();
		shape.init();

		
		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		//Set up cylinder vertices
		vector<GLfloat> cylin_vertices;
		createCylVerts(&cylin_vertices);
		glBufferData(GL_ARRAY_BUFFER, cylin_vertices.size() * sizeof(GLfloat), &cylin_vertices[0], GL_STATIC_DRAW);

		glGenBuffers(2, &IndexBufferIDBox);

		//Set up cylinder indices for face elements
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		vector<GLushort> cylin_elements;
		createCylIndices(&cylin_elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,  cylin_elements.size() * sizeof(GLushort), &cylin_elements[0], GL_STATIC_DRAW);

		//Enable vertex attribute in VBO
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		glBindVertexArray(0);

	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		// Enable blending/transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_POLYGON_STIPPLE);

		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog->init())
			{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1); //make a breakpoint here and check the output window for the error message!
			}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertColor");
		//Petal shader
		shapeprog = std::make_shared<Program>();
		shapeprog->setVerbose(true);
		shapeprog->setShaderNames(resourceDirectory + "/shape_vertex.glsl", resourceDirectory + "/shape_fragment.glsl");
		if (!shapeprog->init())
			{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1); //make a breakpoint here and check the output window for the error message!
			}
		shapeprog->addUniform("P");
		shapeprog->addUniform("V");
		shapeprog->addUniform("M");
		shapeprog->addUniform("trait1"); //The intensity of the petal lip's curve
		shapeprog->addUniform("R");
		shapeprog->addUniform("windowSize");
		shapeprog->addAttribute("vertPos");
		shapeprog->addAttribute("vertNor");
		//shapeprog->addAttribute("vertTex");
		
		//Snow Globe Shader
		shapeprog2 = std::make_shared<Program>();
		shapeprog2->setVerbose(true);
		shapeprog2->setShaderNames(resourceDirectory + "/globe_vert_shader.glsl", resourceDirectory + "/globe_frag_shader.glsl");
		if (!shapeprog2->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1); //make a breakpoint here and check the output window for the error message!
		}
		shapeprog2->addUniform("P");
		shapeprog2->addUniform("V");
		shapeprog2->addUniform("M");
		shapeprog2->addUniform("cam");
		shapeprog2->addAttribute("vertPos");
		shapeprog2->addAttribute("vertNor");
		//shapeprog2->addAttribute("vertTex");
		//Snow Shader
		snowshapeprog = std::make_shared<Program>();
		snowshapeprog->setVerbose(true);
		snowshapeprog->setShaderNames(resourceDirectory + "/snow_vert_shader.glsl", resourceDirectory + "/snow_frag_shader.glsl");
		if (!snowshapeprog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1); //make a breakpoint here and check the output window for the error message!
		}
		snowshapeprog->addUniform("P");
		snowshapeprog->addUniform("V");
		snowshapeprog->addUniform("M");
		snowshapeprog->addAttribute("vertPos");
		snowshapeprog->addAttribute("vertNor");
		//Bug Shader
		shapebug = std::make_shared<Program>();
		shapebug->setVerbose(true);
		shapebug->setShaderNames(resourceDirectory + "/bug_vert_shader.glsl", resourceDirectory + "/bug_frag_shader.glsl");
		if (!shapebug->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1); //make a breakpoint here and check the output window for the error message!
		}
		shapebug->addUniform("P");
		shapebug->addUniform("V");
		shapebug->addUniform("M");
		shapebug->addAttribute("vertPos");
		shapebug->addAttribute("vertNor");
		//snowshapeprog->addAttribute("vertTex");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
#define PI2 6.283185307 //Don't touch

		double frametime = get_last_elapsed_time();
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);
		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = glm::mat4(1);
		M = glm::mat4(1);
		// Apply orthographic projection....
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones

		glm::vec3 windowSize;
		windowSize = glm::vec3(width, height, 1);

		//animation with the model matrix:
		static float w = 0.0;
		w += 0.01;//rotation angle
		static float t = 0;
		t += 0.01;
		float trans = 0;// sin(t) * 2;
		static float rtime = 0;

		V = mycam.process(frametime);
		
		//Ignore for now - used to add variation between each petal's shape
		srand(time(NULL));
		static vector<float> alpha;
		static vector<float> beta;
		static vector<float> gamma;
		static vector<float> flakePos;
		static bool variationsAssigned = false;
		static bool petalNormROffAssigned = false;
		static bool flakeAssigned = false;

		//Rotation variables
		static float ww = 0;
		ww += 0.01;
		static float globeAngle = 0;
		const static float angleSpeed = 0.01;
		if (mycam.a == 1)
		{
			globeAngle -= angleSpeed;
		}
		else if (mycam.d == 1)
		{
			globeAngle += angleSpeed;
		}
		static float theta = 0;
		static float flowerOpen = 0;

//CREATE THE FLOWER
		shapeprog->bind();

		glUniform3fv(shapeprog->getUniform("windowSize"), 3, (GLfloat *)&windowSize);
		glUniformMatrix4fv(shapeprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(shapeprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);

		//Matrices
		mat4 S;
		mat4 TRingRadius;
		mat4 R;
		//Rotate entire rose
		mat4 RWhole = glm::rotate(mat4(1.0f), globeAngle, vec3(0, 1, 0));
		RWhole *= glm::rotate(mat4(1.0f), 1.0f, vec3(1, 0, 0));
		//Rotate each petal 1.0 radians on the y axis
		mat4 TOffOrigin = glm::translate(mat4(1.0f), vec3(0, 0, -1));
		mat4 RPetal;
		mat4 TOnOrigin = glm::translate(mat4(1.0f), vec3(0, 0, 1));
		mat4 RNormals;
		mat4 ExportM;

		//Rose constants, adjust as needed
#define RINGS 4
#define RING_SPACE 0.3 //In world space units
#define ROSE_HEIGHT 1.2 //In world space units
#define DENSITY 0.5 //Least dense - 0, Most dense - 1.0
#define VARIATION 0.3

		//Iterate through each ring of the rose
		int petal = 0;
		rtime += 0.05;
		for (int i = 0; i < RINGS; i++) {
			theta = PI2 / pow(2, i + 2);
			//Scale each petal based on which ring it is in
			//Outermost ring has the largest petals
			S = glm::scale(glm::mat4(1.0f), glm::vec3(RING_SPACE * (i+1) , ROSE_HEIGHT, RING_SPACE * (i+1)));
			RPetal = glm::rotate(mat4(1.0f), (float)((0.1 + (0.15 * (cos(ww * 2) + 1))) * i), vec3(1, 0, 0));
			RPetal *= glm::rotate(mat4(1.0f), (float)(0.1 * i), vec3(0, 1, 0));
			//Translate each petal so it sits in the appropriate ring radius
			float horiz_distance = i * 0.1;
			TRingRadius = glm::translate(glm::mat4(1.0f), glm::vec3(0, -8 * pow(horiz_distance, 2), horiz_distance));
			//Iterate through each petal in the ring
			while (theta < PI2)
			{
				if (!variationsAssigned) {
					alpha.push_back((float)rand() / RAND_MAX);
					beta.push_back(VARIATION * (float)rand() / RAND_MAX);
				}
				//For a cool color effect, create a rotation matrix for the normals
				//Use petal variations to set rotation offset
				//Apply resulting rotation to uniform 'R' to rotate normals with
				RNormals = glm::rotate(mat4(1.0f), (float)(rtime + (PI2 * alpha.at(petal))), vec3(1, 1, 1));
				glUniformMatrix4fv(shapeprog->getUniform("R"), 1, GL_FALSE, &RNormals[0][0]);
				//Rotate the petal so it's always facing towards the center
				R = glm::rotate(mat4(1.0f), theta - beta.at(petal), vec3(0, 1, 0));
				//Create the model matrix
				M = RWhole * R * TRingRadius * TOnOrigin * RPetal * TOffOrigin * S;
				glUniformMatrix4fv(shapeprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
				//Gather relative information from a single petal to use for 'bug'
				if (petal == 10)
				{
					ExportM = RWhole * R * TRingRadius * TOnOrigin * RPetal * TOffOrigin;
				}
				//Ignore this for now - Will be used to add variation between petals
				glUniform1f(shapeprog->getUniform("trait1"), alpha.at(petal));
				shape.draw(shapeprog);
				//Set the next petal location in the ring
				theta += PI2 / pow(2, i + 1);
				petal++;
			}
		}
		if (!variationsAssigned)
			variationsAssigned = true;

		shapeprog->unbind();

//CREATE THE FLOWER BASE
		prog->bind();
		//send the matrices to the shaders
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);

		glBindVertexArray(VertexArrayID);

		mat4 Rot = glm::rotate(mat4(1.0f), globeAngle, vec3(0, 1, 0));
		mat4 TBody = glm::translate(mat4(1.0f), vec3(0, -1.5, 0));
		M = RWhole * Rot * TBody;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, (CYLINDER_VERT_COUNT) * 3, GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
		prog->unbind();

//CREATE A FLYING 'BUG' AROUND ROSE PETAL
		shapebug->bind();
		glUniformMatrix4fv(shapebug->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(shapebug->getUniform("V"), 1, GL_FALSE, &V[0][0]);

		mat4 TBug = glm::translate(mat4(1.0f), vec3(0, 1, 0.2));
		mat4 SBug = glm::scale(mat4(1.0f), vec3(0.1, 0.1, 0.1));
		mat4 RBug = glm::rotate(mat4(1.0f), ww * 3, vec3(0, 1, 0));
		M = ExportM * RBug * TBug * SBug;
		glUniformMatrix4fv(shapebug->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape.draw(shapebug);
		shapebug->unbind();

//CREATE THE SNOW INSIDE THE SNOWGLOBE
		snowshapeprog->bind();
		glUniformMatrix4fv(snowshapeprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(snowshapeprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);

		mat4 ScaleFlake = glm::scale(mat4(1.0f), vec3(0.02, 0.02, 0.02));
		float rotateDir = 1;
		float rotateX = 0;
		int rotateZ = 0;
		for (int i = 0; i < 100; i++)
		{
			if (!flakeAssigned)
			{
				flakePos.push_back((float)rand() / RAND_MAX);
				flakePos.push_back((float)rand() / RAND_MAX);
				flakePos.push_back((float)rand() / RAND_MAX);
			}
			mat4 TFlake = glm::translate(mat4(1.0f), vec3(1.6 * cos(PI2 * flakePos.at(i * 3)), 1.6 * sin(PI2 * flakePos.at((i * 3) + 1)), 1.6 * sin(PI2 * flakePos.at((i * 3) + 2))));
			if (i > 33)
			{
				rotateDir = -1;
				rotateZ += 0.1;
			}
			if (i > 66)
				rotateX += 0.1;
			mat4 RFlake = glm::rotate(mat4(1.0f), rotateDir * t, vec3(sin(rotateX), 1, sin(rotateZ)));
			mat4 RFlakeRelGlobe = glm::rotate(mat4(1.0f), globeAngle, vec3(0, 1, 0));
			M = RFlake * TFlake * ScaleFlake;
			M = RFlakeRelGlobe * M;
			glUniformMatrix4fv(snowshapeprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			shape.draw(snowshapeprog);
		}
		if (!flakeAssigned)
			flakeAssigned = true;
		snowshapeprog->unbind();

//CREATE THE SNOW GLOBE
		shapeprog2->bind();
		glUniform3fv(shapeprog2->getUniform("cam"), 1, &mycam.pos[0]);
			glUniformMatrix4fv(shapeprog2->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(shapeprog2->getUniform("V"), 1, GL_FALSE, &V[0][0]);

			mat4 ScaleGlobe = glm::scale(mat4(1.0f), vec3(3, 3, 3));
			mat4 RotateGlobe = glm::rotate(mat4(1.0f), globeAngle, vec3(0, 1, 0));
			M = RotateGlobe * ScaleGlobe;
			glUniformMatrix4fv(shapeprog2->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			shape.draw(shapeprog2);
		shapeprog2->unbind();
	}

};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(2560, 1920);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();


	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
