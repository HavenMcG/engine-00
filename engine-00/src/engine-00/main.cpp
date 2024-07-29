#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Shader.h"
#include "Camera.h"
#include <vector>
#include "../Asset/Model.h"
#include "../Asset/ModelLoader.h"
#include "Renderer.h"
#include "../Asset/TextureManager.h"

int window_width = 1800;
int window_height = 1200;

float delta_time = 0.0f;
float last_frame = 0.0f;

// camera variables
Camera my_cam{};
float last_x = window_width / 2; // to track the change in mouse cursor position
float last_y = window_height / 2;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void processInput(GLFWwindow* window);

int main() {
	// initialize GLFW, specify which OpenGL version we are targeting (3.3), and which profile to use (core)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// create a GLFW window object
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// initialize GLAD (manages function pointers for OpenGL)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD\n";
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	// tell OpenGL the size of the rendering window/viewport
	glViewport(0, 0, window_width, window_height);
	// register callback function
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// tell glfw that it should hide the mouse cursor and capture it
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// register mouse callback
	glfwSetCursorPosCallback(window, mouse_callback);

	// clear the screen first to avoid flashbang during loading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(window);

	// load shaders from their source files
	Shader my_shader("src/engine-00/colors.vert.glsl","src/engine-00/colors.frag.glsl");

	// load models
	// resources/objects/backpack/backpack.obj
	// ../../Random Assets/forest-monster/forest-monster-final_FIXED.glb
	TextureManager tm;
	MeshManager mm;
	ModelLoader ml;
	Model monster;
	ml.read_model(&monster, "../resources/models/forest-monster/forest-monster-final_FIXED.glb", tm, mm);
	//ml.ogl_load_model(&monster);

	// instantiate renderer
	Renderer r;

	my_cam.move_to(glm::vec3{ 0.0f, 20.0f, 50.0f });
	my_cam.look_at(glm::vec3{ 0.0f, 17.0f, 0.0f });
	glfwSetCursorPos(window, window_width / 2, window_height / 2);

	// RENDER LOOP
	while (!glfwWindowShouldClose(window)) {
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // state setting function
		glClear(GL_COLOR_BUFFER_BIT); // state using function

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		my_shader.use();

		glm::mat4 view = my_cam.view_matrix();
		my_shader.set_mat4("view", view);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f); // args: fov, aspect ratio, near plane distance, far plane distance
		my_shader.set_mat4("projection", projection);

		glm::mat4 model = glm::mat4(1.0f);
		my_shader.set_mat4("model", model);

		my_shader.set_vec3("light_pos_world", glm::vec3{ 0.0f, 0.0f, 4.0f });

		glm::vec3 light_color{ 1.0f, 1.0f, 1.0f };
		glm::vec3 diffuse_color = light_color * glm::vec3(0.5f);
		glm::vec3 ambient_color = light_color * glm::vec3(1.0f);
		glm::vec3 specular_color = light_color * glm::vec3(1.0f);
		my_shader.set_vec3("light.ambient", ambient_color);
		my_shader.set_vec3("light.diffuse", diffuse_color);
		my_shader.set_vec3("light.specular", specular_color);
		my_shader.set_float("material.shininess", 32.0f);

		r.draw_model(&monster, &my_shader, mm, tm);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// camera movement
	float cam_speed = 8.5f * delta_time;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		my_cam.move(Forward, cam_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		my_cam.move(Backward, cam_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		my_cam.move(Left, cam_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		my_cam.move(Right, cam_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		my_cam.move(Up, cam_speed);
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		my_cam.move(Down, cam_speed);
	}
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
	float x_offset = x_pos - last_x;
	float y_offset = last_y - y_pos; // reversed since y-coordinates range from bottom to top
	last_x = (float)x_pos;
	last_y = (float)y_pos;

	float sensitivity = 0.1f;
	x_offset *= sensitivity;
	y_offset *= sensitivity;

	my_cam.look_adjust(x_offset, y_offset);
}