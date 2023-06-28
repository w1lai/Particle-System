////////////////////////////////////////
// Window.cpp
////////////////////////////////////////

#include "Window.h"


#include "ParticleSystem.h"


////////////////////////////////////////////////////////////////////////////////

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "CSE 169 Particle System";

// Objects to render
Cube * Window::cube;

// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;





////////////////////////////////////////////////////////////////////////////////

//imgui string
const char* glsl_version = "#version 460";

// Our state
bool show_demo_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


/////////////////////////////////////////////////////////////////////////////////


//particle system

const int STARTING_SIZE = 1000;
int FLOOR = -1;

int refreshRate = 60;
int regenRate = 100;

float radius = 0.01f;

glm::vec3 initialPos = glm::vec3(0);
glm::vec3 variancePos = glm::vec3(0);

//I have no idea why I have to set this to non-zero but it works
glm::vec3 initialVelo = glm::vec3(0.0f, 0.001f, 0.0f);
glm::vec3 varianceVelo = glm::vec3(0);

float lifespan = 1.0f;
float varianceLife = 0.5f;

glm::vec3 aeroForce = glm::vec3(0);
float density = 0.0f;
float dragCoefficient = 0.0f;

float floorDamper = 0.7f;
float floorFriction = 0.2f;
bool collisionCheck = true;

ParticleSystem ps(STARTING_SIZE);

Cube* psFloor;

//////////////////////////////////////////////////////////////////////////////////

// Constructors and desctructors 
bool Window::initializeProgram() {
	
	// Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

bool Window::initializeObjects()
{
	// Create a cube
	cube = new Cube();
	//cube = new Cube(glm::vec3(-1, 0, -2), glm::vec3(1, 1, 1));
	

	//create a floor for reference (sorry it's fixed I know)
	psFloor = new Cube(glm::vec3(-20.0f, (float)(FLOOR - 0.1f), -20.0f), glm::vec3(20.0f, (float)FLOOR, 20.0f));
	psFloor->setColor(glm::vec3(1.0f));

	
	//populate with some particle first
	ps.populate(STARTING_SIZE, initialPos, initialVelo, variancePos, varianceVelo, lifespan, varianceLife, radius);

	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	delete psFloor;
	delete cube;

	//implicit call on destructing particle system (well it's not a pointer so lol)

	//cleanup imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	// Delete the shader program.
	glDeleteProgram(shaderProgram);
}

////////////////////////////////////////////////////////////////////////////////

// for the Window
GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	//imgui stuff
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);


#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// set up the camera
	Cam = new Camera();
	Cam->SetAspect(float(width) / float(height));

	// initialize the interaction variables
	LeftDown = RightDown = false;
	MouseX = MouseY = 0;

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	Cam->SetAspect(float(width) / float(height));
}

////////////////////////////////////////////////////////////////////////////////

// update and draw functions
void Window::idleCallback()
{
	// Perform any updates as necessary. 


	Cam->Update();

	//cube speening
	//cube->update();

	//generate new particles (holy hell that's a lot of parameters)
	ps.update(refreshRate, regenRate, density, dragCoefficient, floorDamper, floorFriction, collisionCheck, 
		initialPos, initialVelo, variancePos, varianceVelo, lifespan, varianceLife, radius, aeroForce);
	
}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Render the object.
	//cube->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	


	// Start the Dear ImGui frame

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	//draw the floor (if needed)
	if (ps.collisionCheck)
		psFloor->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

	//render all particles
	ps.render(Cam);
	
	//IMGUI WINDOW
	//control panel
	{
		ImGui::Begin("Parameters Editor");
		
		

		//Position

		if (ImGui::CollapsingHeader("Initial Position"))
		{
			if (ImGui::CollapsingHeader("Precise Control")) {
				ImGui::SliderFloat("starting x", &initialPos.x, -1.0f, 1.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
				ImGui::SliderFloat("starting y", &initialPos.y, -1.0f, 1.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
				ImGui::SliderFloat("starting z", &initialPos.z, -1.0f, 1.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
			}
			if (ImGui::CollapsingHeader("Coarse Control")) {
				ImGui::SliderFloat("starting x", &initialPos.x, -5.0f, 5.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
				ImGui::SliderFloat("starting y", &initialPos.y, -5.0f, 5.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
				ImGui::SliderFloat("starting z", &initialPos.z, -5.0f, 5.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
			}


			ImGui::SliderFloat("variance x", &variancePos.x, 0.0f, 2.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("variance y", &variancePos.y, 0.0f, 2.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("variance z", &variancePos.z, 0.0f, 2.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);

			if (ImGui::Button("Reset"))
			{
				initialPos.x = 0.0f;
				initialPos.y = 0.0f;
				initialPos.z = 0.0f;

				variancePos.x = 0.0f;
				variancePos.y = 0.0f;
				variancePos.z = 0.0f;

			}

		}
		//Velocity
		if (ImGui::CollapsingHeader("Initial Velocity")) {

			ImGui::SliderFloat("starting x", &initialVelo.x, -20.0f, 20.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("starting y", &initialVelo.y, -20.0f, 20.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("starting z", &initialVelo.z, -20.0f, 20.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);

			ImGui::SliderFloat("variance x", &varianceVelo.x, 0.0f, 10.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("variance y", &varianceVelo.y, 0.0f, 10.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("variance z", &varianceVelo.z, 0.0f, 10.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);



			if (ImGui::Button("Reset"))
			{
				initialVelo.x = 0.0f;
				initialVelo.y = 0.0f;
				initialVelo.z = 0.0f;

				varianceVelo.x = 0.0f;
				varianceVelo.y = 0.0f;
				varianceVelo.z = 0.0f;

			}
		}
		//life span
		if (ImGui::CollapsingHeader("Expected Life")) {
			ImGui::SliderFloat("lifespan", &lifespan, 0.01f, 10.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);

			ImGui::SliderFloat("variance", &varianceLife, 0.01f, lifespan, "%.4f", ImGuiSliderFlags_AlwaysClamp);

			if (ImGui::Button("Default"))
			{
				lifespan = 1.0f;
				varianceLife = 0.5f;
			}
		}
		//aerodynamic force
		//no gravity because it's fixed
		if (ImGui::CollapsingHeader("Aerodynamic Force")) {
			ImGui::SliderFloat("Aero force x", &aeroForce.x, -20.0f, 20.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("Aero force y", &aeroForce.y, -20.0f, 20.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("Aero force z", &aeroForce.z, -20.0f, 20.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("air density", &density, 0.0f, 10.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("drag coefficient (cd)", &dragCoefficient, 0.0f, 2.0f, "%.4f", ImGuiSliderFlags_Logarithmic);

			if (ImGui::Button("Reset"))
			{
				aeroForce.x = 0.0f;
				aeroForce.y = 0.0f;
				aeroForce.z = 0.0f;
				density = 1.3f;
				dragCoefficient = 0.0f;
			}
		}
		//collision enable
		if (ImGui::CollapsingHeader("Collision")) {
			ImGui::Checkbox("enable floor collision", &collisionCheck);
			ImGui::SliderFloat("Collision Elasticity", &floorDamper, 0.0f, 1.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("Collision Friction", &floorFriction, 0.0f, 2.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp);

			if (ImGui::Button("Reset"))
			{
				collisionCheck = true;
				floorDamper = 0.6f;
				floorFriction = 0.4f;
			}
		}
		//particle size, subdivision and others
		ImGui::BulletText("General");
		ImGui::SliderFloat("Particle Size (radius)", &radius, 0.0001f, 0.1f, "%.4f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderInt("subdivision", &refreshRate, 1, 120);
		ImGui::SliderInt("particles creation rate", &regenRate, 0, ps.MAX_SIZE/10);

		if (ImGui::Button("Default settings"))
		{
			radius = 0.01f;
			refreshRate = 60;
			regenRate = 100;
		}

		



		ImGui::End();
	}
	//Camera window
	{
		ImGui::Begin("Camera");
		static float cam_a = 0.0f;
		static float cam_d = 0.5f;
		static float cam_i = 0.0f;
		ImGui::SliderFloat("Azimuth", &cam_a, -360.0f, 360.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderFloat("Zoom", &cam_d, 0.0f, 5.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Incline", &cam_i, -360.0f, 360.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);


		if (ImGui::Button("Default"))
		{
			cam_a = 0.0f;
			cam_d = 0.5f;
			cam_i = 0.0f;
		}

		Cam->SetAzimuth(cam_a);
		Cam->SetDistance(cam_d);
		Cam->SetIncline(cam_i);



		ImGui::End();
	}


	//ImGui render
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();

	// Swap buffers.
	glfwSwapBuffers(window);
}

////////////////////////////////////////////////////////////////////////////////

// helper to reset the camera
void Window::resetCamera() 
{
	Cam->Reset();
	Cam->SetAspect(float(Window::width) / float(Window::height));
}

////////////////////////////////////////////////////////////////////////////////

// callbacks - for Interaction 
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*
	 * TODO: Modify below to add your key callbacks.
	 */

	
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key) 
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);				
			break;

		case GLFW_KEY_R:
			resetCamera();
			break;

		case GLFW_KEY_W:
			break;

		case GLFW_KEY_D:
			break;

		default:
			break;
		}
	}
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		LeftDown = (action == GLFW_PRESS);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && !ImGui::GetIO().WantCaptureMouse) {
		RightDown = (action == GLFW_PRESS);
	}
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {

	int maxDelta = 100;
	int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

	MouseX = (int)currX;
	MouseY = (int)currY;

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if (LeftDown && !ImGui::GetIO().WantCaptureMouse) {
		const float rate = 1.0f;
		Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
		Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
	}
	if (RightDown && !ImGui::GetIO().WantCaptureMouse) {
		const float rate = 0.005f;
		float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
		Cam->SetDistance(dist);
	}
}

////////////////////////////////////////////////////////////////////////////////