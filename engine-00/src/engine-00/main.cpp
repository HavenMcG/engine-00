#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Camera.h"
#include <vector>
#include "../HECS/Component/Model.h"
#include "../HECS/Component/Transform.h"
#include "../HECS/System/Renderer.h"
#include "../HECS/System/TransformSystem.h"
#include "../Asset/Storing/OpenGL/OglAssetStore.h"
#include "../Asset/Loading/Assimp/AssimpAssetLoader.h"

#include "HexGrid.h"

glm::vec3 normalize_rgb(glm::vec3 rgb) {
	return glm::vec3{ rgb.r / 255,rgb.g / 255,rgb.b / 255 };
}

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
	// initialize GLFW, specify which OpenGL version we are targeting (4.0), and which profile to use (core)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 16); // Enable MSAA

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

	// depth testing prevents triangles that are supposed to be behind something rendering on top of it
	glEnable(GL_DEPTH_TEST);

	// Enable blending to draw on top with transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable MSAA
	glEnable(GL_MULTISAMPLE);

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


	// Instantiate asset store
	OglAssetStore ogl_store;
	AssetStore& assets = ogl_store;

	// Instantiate asset loader
	AssimpAssetLoader aal;
	AssetLoader& asset_loader = aal;

	// Load assets
	Model monster_model = *asset_loader.load_model("../resources/models/forest-monster/forest-monster-final_FIXED.obj", assets);
	Model hex_2d = *asset_loader.load_model("../resources/models/2d-hex/2d-hex.glb", assets);
	hex_2d.materials[0].color_diffuse = normalize_rgb(glm::vec3{ 6.0f, 138.0f, 44.0f });

	// Load shaders
	Shader my_shader("src/engine-00/Shaders/material.vert.glsl", "src/engine-00/Shaders/material.frag.glsl");
	Shader hex_grid_shader("src/engine-00/Shaders/hexGrid.vert.glsl", "src/engine-00/Shaders/hexGridV2.frag.glsl");

	// Instantiate component collections
	ModelCollection model_col;
	TransformCollection transform_col;

	// Instantiate systems
	TransformSystem transform_sys{ transform_col };
	Renderer renderer;

	// Set up entities
	/*Entity monster = 7;
	transform_col.add_component(monster);
	model_col.add_component(monster, monster_model);*/

	/*Entity hex_tile = 11;
	transform_col.add_component(hex_tile);
	model_col.add_component(hex_tile, hex_2d);
	transform_sys.rotate_degrees(hex_tile, 0.0f, 30.0f, 0.0f);*/

	// Set camera start position
	my_cam.move_to(glm::vec3{ 0.0f, 20.0f, 50.0f });
	my_cam.look_at(glm::vec3{ 0.0f, 17.0f, 0.0f });
	glfwSetCursorPos(window, window_width / 2, window_height / 2);

	// Hex Testing
	Layout l{ pointy, glm::vec2{1,1}, glm::vec2{0,0} };

	glm::vec2 uv{ -49, -49.5 };
	Hex containing_hex = hex_round(pixel_to_hex(l, uv));
	glm::vec2 local_center = hex_to_pixel(l, containing_hex);
	glm::vec2 local_coords = uv - local_center;
	std::cout << "containing hex: " << containing_hex.q << "," << containing_hex.r << "," << containing_hex.s << std::endl;
	std::cout << "local_center: " << local_center.x << "," << local_center.y << std::endl;
	std::cout << "local_coords: " << local_coords.x << "," << local_coords.y << std::endl;

	auto distance_from_center = [](glm::mat3x2 axes, float hex_radius, glm::vec2 local_point) -> float {
		float max_r = 0.0;
		for (int i = 0; i < 3; i++) {
			float r = glm::dot(local_point, axes[i]);
			r /= (sqrt(3) * 0.5 * hex_radius);
			max_r = std::max(max_r, abs(r));
		}
		return max_r;
	};
	float distance = distance_from_center(l.orientation.axes(), l.size.x, local_coords);
	std::cout << "distance: " << distance << std::endl;

	// maps
	std::unordered_map<Hex, int> hex_map = rectangle_map_pointy<int>(0, 9, 0, 5);
	std::vector<Entity> hex_entities{};

	int i = 100;
	for (auto r : hex_map) {
		hex_entities.push_back(i);
		glm::vec2 p = hex_to_pixel(l, r.first);
		transform_col.add_component(i, TransformComponent{ glm::vec3{ p.x, -0.003, p.y }, glm::quat(glm::vec3{ 0.0f, glm::radians(-(60.0f * l.orientation.angle()) + 30.0f), 0.0f }) });
		model_col.add_component(i, hex_2d);
		++i;
	}

	// hex shader uniforms
	hex_grid_shader.use();
	hex_grid_shader.set_mat2("hex_layout.orientation.forward", l.orientation.forward_matrix());
	hex_grid_shader.set_mat2("hex_layout.orientation.inverse", l.orientation.inverse_matrix());
	hex_grid_shader.set_float("hex_layout.orientation.start_angle", l.orientation.angle());
	hex_grid_shader.set_vec2("hex_layout.size", l.size);
	hex_grid_shader.set_vec2("hex_layout.origin", l.origin);
	hex_grid_shader.set_mat3x2("axes", l.orientation.axes());
	hex_grid_shader.set_vec4("color", glm::vec4{ 0.8, 0.8, 0.8, 1.0 });
	hex_grid_shader.set_float("power", 32.0f);


	// plane for hex shader
	float vertices[] = {
		 0.5f,  0.0f, 0.5f,
		 0.5f, 0.0f, -0.5f,
		-0.5f, 0.0f, -0.5f,
		-0.5f,  0.0f, 0.5f
	};
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	unsigned int hex_vao;
	unsigned int hex_vbo;
	unsigned int hex_ebo;
	glGenVertexArrays(1, &hex_vao);
	glGenBuffers(1, &hex_vbo);
	glGenBuffers(1, &hex_ebo);

	glBindVertexArray(hex_vao);

	glBindBuffer(GL_ARRAY_BUFFER, hex_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hex_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

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

		//transform_sys.rotate_degrees(monster, 0.0f * delta_time, 24.0f * delta_time, 0.0f * delta_time);
		//transform_sys.rotate_degrees(hex_tile, 24.0f * delta_time, 0.0f * delta_time, 0.0f * delta_time);

		my_shader.use();

		glm::mat4 view = my_cam.view_matrix();
		my_shader.set_mat4("view", view);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 10000.0f); // args: fov, aspect ratio, near plane distance, far plane distance
		my_shader.set_mat4("projection", projection);

		// setting model matrix is done in the renderer

		my_shader.set_vec3("light_pos_world", glm::vec3{ 0.0f, 0.0f, 4.0f });

		glm::vec3 light_color{ 1.0f, 1.0f, 1.0f };
		glm::vec3 diffuse_color = light_color * glm::vec3(1.0f);
		glm::vec3 ambient_color = light_color * glm::vec3(1.0f);
		glm::vec3 specular_color = light_color * glm::vec3(1.0f);
		my_shader.set_vec3("light.ambient", ambient_color);
		my_shader.set_vec3("light.diffuse", diffuse_color);
		my_shader.set_vec3("light.specular", specular_color);
		my_shader.set_float("material.shininess", 16.0f);

		renderer.draw_models(view, my_shader, model_col, transform_col, ogl_store);


		hex_grid_shader.use();
		hex_grid_shader.set_mat4("model", glm::mat4(1.0f));
		hex_grid_shader.set_mat4("view", view);
		hex_grid_shader.set_mat4("projection", projection);
		glBindVertexArray(hex_vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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