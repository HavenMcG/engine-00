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
#include "../Asset/Store/OpenGL/OglAssetStore.h"
#include "../Asset/Loader/Assimp/AssimpAssetLoader.h"
#include "RayCast.h"

#include "HexGrid.h"

#include "../HECS/Component/Link.h"
#include "../HECS/System/Hierarchy.h"

glm::vec3 normalize_rgb(glm::vec3 rgb) {
	return rgb/255.0f;
}

void print(const std::vector<Entity>& ets, const LinkCollection& rc) {
	std::cout << std::endl;
	std::cout << "==================================================================================" << std::endl;
	for (Entity et : ets) {
		std::cout << std::endl;
		rc.print(et);
	}
}

int window_width = 1800;
int window_height = 1200;

float delta_time = 0.0f;
float last_frame = 0.0f;

glm::vec3 lamp_color = { 1.0f, 1.0f, 1.0f };

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
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
	hex_2d.materials[0].color_diffuse = normalize_rgb(glm::vec3{ 27, 48, 17 });

	Model selected_hex = hex_2d;
	selected_hex.materials[0].color_diffuse = normalize_rgb(glm::vec3{ 255.0f, 0.0f, 0.0f });

	Mesh mesh_cube_1x1x1 = *assets.load(Cuboid{ { -0.5f,-0.5f,-0.5f },{ 0.5f,0.5f,0.5f } }.generate_mesh());
	Material mat_light_emitter{};
	mat_light_emitter.color_diffuse = lamp_color;
	mat_light_emitter.color_specular = lamp_color;
	mat_light_emitter.color_emissive = lamp_color;
	mat_light_emitter.shininess = 32.0f;
	Model model_light_cube{ "light emitter", { mesh_cube_1x1x1 }, { mat_light_emitter }};

	// Load shaders
	Shader my_shader("src/engine-00/Shaders/material.vert.glsl", "src/engine-00/Shaders/material.frag.glsl");
	Shader hex_grid_shader("src/engine-00/Shaders/hexGrid.vert.glsl", "src/engine-00/Shaders/hexGrid.frag.glsl");
	Shader gui_shader("src/engine-00/Shaders/gui.vert.glsl", "src/engine-00/Shaders/gui.frag.glsl");

	// Instantiate component collections
	EntityCollection entities;

	// Instantiate component collections
	ModelCollection model_col;
	TransformCollection transform_col;
	LightCollection light_col;

	// !!TEMP GUI STUFF!!
	Mesh gui_quad = *ogl_store.load(BASIC_QUAD_MESH_DATA);
	Material gui_material{};
	gui_material.color_diffuse = normalize_rgb({ 145.0f, 91.0f, 4.0f });
	gui_material.color_specular = { 1.0f, 1.0f, 1.0f };
	Model gui_model_1 = { "gui_model_1", { gui_quad }, { gui_material } };
	Transform gui_transform_1{};

	ModelCollection gui_model_col;
	TransformCollection gui_transform_col;
	LinkCollection relation_col;

	Entity gui_element_1 = entities.create_entity();
	gui_model_col.add_component(gui_element_1, gui_model_1);
	gui_transform_col.add_component(gui_element_1, gui_transform_1);

	gui_transform_col.set_position(gui_element_1, { 0.0f, 0.0f, 0.0f });
	// !!END GUI!!

	// Instantiate systems
	TransformSystem transform_sys{ transform_col };
	Renderer renderer;
	HierarchySystem hierarchy_sys{ relation_col, transform_col };

	// Set up entities
	Entity monster = entities.create_entity();
	transform_col.add_component(monster);
	model_col.add_component(monster, monster_model);

	Entity lamp = entities.create_entity();
	transform_col.add_component(lamp);
	light_col.add_component(lamp);
	light_col.set_color(lamp, lamp_color);
	light_col.set_constant(lamp, 1.0f);
	light_col.set_linear(lamp, 0.045f);
	light_col.set_quadratic(lamp, 0.0075f);
	transform_col.set_position(lamp, { 2.0f, 1.0f, 0.0f });
	transform_col.set_scale(lamp, { 0.2f, 0.2f, 0.2f });
	model_col.add_component(lamp, model_light_cube);

	Entity lamp2 = entities.create_entity();
	transform_col.add_component(lamp2);
	light_col.add_component(lamp2);
	light_col.set_color(lamp2, lamp_color);
	light_col.set_constant(lamp2, 1.0f);
	light_col.set_linear(lamp2, 0.09f);
	light_col.set_quadratic(lamp2, 0.032f);
	transform_col.set_position(lamp2, { 13.0f, 1.0f, 6.0f });
	transform_col.set_scale(lamp2, { 0.4f, 0.4f, 0.4f });
	model_col.add_component(lamp2, model_light_cube);


	// !!TEMP BOUNDING BOX STUFF!!
	Cuboid bound = *model_col.bounding_box(monster, assets);

	glm::vec3 center{};
	center.x = bound.p1.x + bound.size().x / 2.0f;
	center.y = 0;
	center.z = bound.p1.z + bound.size().z / 2.0f;

	float desired_size = 2.0f;
	float scale_x = desired_size / bound.size().x;
	float scale_z = desired_size / bound.size().z;
	float scale = std::min(scale_x, scale_z);

	transform_col.set_scale(monster, glm::vec3{ scale,scale,scale });
	auto act_scale = *transform_col.scale(monster);
	std::cout << std::endl;
	std::cout << "scale: " << act_scale.x << " " << act_scale.y << " " << act_scale.z << std::endl;
	glm::vec3 act_dimensions = bound.size() * act_scale;
	std::cout << "dimensions: " << act_dimensions.x << " x " << act_dimensions.y << " x " << act_dimensions.z << std::endl;
	std::cout << "center: " << center.x << ", " << center.y << ", " << center.z << std::endl;

	glm::vec3 t = glm::vec3{ 0, 0, 0 } - center;
	transform_col.set_position_offset(monster, t * scale);

	Mesh bounding_box_mesh = *assets.load(bound.generate_mesh());
	Material bounding_box_material{};
	bounding_box_material.color_diffuse = normalize_rgb({ 0.0f, 29.0f, 51.0f });
	bounding_box_material.color_specular = { 1.0f, 1.0f, 1.0f };
	bounding_box_material.opacity = 1.0f;
	bounding_box_material.shininess = 32.0f;
	Model bounding_box_model = { "bounding_box", { bounding_box_mesh }, { bounding_box_material } };

	Entity monster_bounding_box = entities.create_entity();
	ModelCollection transparent_model_col;
	//model_col.add_component(monster_bounding_box, bounding_box_model);
	transform_col.add_component(monster_bounding_box);
	relation_col.make_child(monster_bounding_box, monster);
	transform_col.set_position_offset(monster_bounding_box, *transform_col.position_offset(monster));

	glm::vec3 act_world_pos = *transform_col.world_position(monster);
	std::cout << "world position: " << act_world_pos.x << ", " << act_world_pos.y << ", " << act_world_pos.z << std::endl;
	glm::vec3 act_local_pos = *transform_col.position(monster);
	std::cout << "local position: " << act_local_pos.x << ", " << act_local_pos.y << ", " << act_local_pos.z << std::endl;

	Entity monster_owner = entities.create_entity();
	transform_col.add_component(monster_owner);
	transform_col.set_position(monster_owner, { 0,0,0 });
	relation_col.make_child(monster, monster_owner);
	// !!END BOUNDING BOX!!

	// Set camera start position
	my_cam.move_to(glm::vec3{ 0.0f, 6.0f, 8.0f });
	my_cam.look_at(glm::vec3{ 0.0f, 0.0f, 0.0f });
	glfwSetCursorPos(window, window_width / 2, window_height / 2);

	// maps
	Layout l{ pointy, glm::vec2{1,1}, glm::vec2{0,0} };
	std::unordered_map<Hex, Entity> hex_map = rectangle_map_pointy<Entity>(0, 9, 0, 5);
	std::unordered_set<Hex> selection = hexagon_set(1);

	Entity e_hex_map;
	transform_col.add_component(e_hex_map);
	transform_col.set_position(e_hex_map, { 0.0f, -0.01f, 0.0f });
	for (auto& r : hex_map) {
		Entity e = entities.create_entity();
		r.second = e;
		glm::vec2 p = hex_to_cartesian(l, r.first);
		transform_col.add_component(e, Transform{ glm::vec3{ p.x, 0.0f, p.y }, glm::quat(glm::vec3{ 0.0f, glm::radians(-(60.0f * l.orientation.angle()) + 30.0f), 0.0f }), glm::vec3{ 1,1,1 } });
		model_col.add_component(e, hex_2d);
		relation_col.make_child(e, e_hex_map);
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

	// track the previously selected hex
	Hex prev_sel = hex_map.begin()->first;

	// move light cube in orbit
	float angle = 0.0f;
	float radius = 2.0f;
	float speed = 2.0f;

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

		// move light cube in orbit
		angle += speed * delta_time;
		if (angle >= 360.0f) angle -= 360.0f;
		glm::vec3 ppos = *transform_col.position(monster);
		transform_col.set_position(lamp, { radius * cos(angle), 2, radius * sin(angle) });

		relation_col.pc.binary_insertion_sort();
		hierarchy_sys.apply_parent_transforms();
		
		my_shader.use();

		glm::mat4 view = my_cam.view_matrix();
		my_shader.set_mat4("view", view);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 10000.0f); // args: fov, aspect ratio, near plane distance, far plane distance
		my_shader.set_mat4("projection", projection);

		// setting model matrix is done in the renderer

		// render standard models
		renderer.draw_models(view, my_shader, model_col, transform_col, light_col, ogl_store);

		// render transparent models
		glDepthMask(GL_FALSE);  // Disable writing to the depth buffer
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		renderer.draw_models(view, my_shader, transparent_model_col, transform_col, light_col, ogl_store); // draw back faces first
		glCullFace(GL_BACK);
		renderer.draw_models(view, my_shader, transparent_model_col, transform_col, light_col, ogl_store); // then front faces
		glDepthMask(GL_TRUE);
		glDisable(GL_CULL_FACE);
		//renderer.draw_ui(gui_shader, gui_model_col, gui_transform_col, ogl_store);

		// render hex grid shader
		hex_grid_shader.use();
		hex_grid_shader.set_mat4("model", glm::mat4(1.0f));
		hex_grid_shader.set_mat4("view", view);
		hex_grid_shader.set_mat4("projection", projection);
		glBindVertexArray(hex_vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// find which hex the cursor is hovering:
		double cursor_pos_x = 0, cursor_pos_y = 0;
		glfwGetCursorPos(window, &cursor_pos_x, &cursor_pos_y);
		float ndc_x = 2.0f * static_cast<float>(cursor_pos_x) / window_width - 1.0f;
		float ndc_y = 1.0f - 2.0f * static_cast<float>(cursor_pos_y) / window_height;
		glm::vec3 ndc_near{ ndc_x, ndc_y, -1 };
		glm::vec3 ndc_far{ ndc_x, ndc_y, 1 };
		glm::mat4 inverse_view_projection = glm::inverse(projection * view);
		glm::vec4 world_near = inverse_view_projection * glm::vec4{ ndc_near, 1.0f };
		glm::vec4 world_far = inverse_view_projection * glm::vec4{ ndc_far, 1.0f };
		glm::vec3 world_near_pos = glm::vec3{ world_near } / world_near.w;
		glm::vec3 world_far_pos = glm::vec3{ world_far } / world_far.w;
		glm::vec3 ray_direction = glm::normalize(world_far_pos - world_near_pos);

		Ray ray{ my_cam.position(), ray_direction };
		auto intersection_point = intersection_point_xz(ray);
		if (intersection_point.has_value()) {
			model_col.models_[model_col.map_[hex_map[prev_sel]]] = hex_2d;
			Hex selo = hex_round(cartesian_to_hex(l, glm::vec2{ intersection_point->x, intersection_point->z }));
			if (hex_map.contains(selo)) {
				//if (selo != prev_sel) std::cout << "hex: " << selo.q << "," << selo.r << std::endl;
				model_col.models_[model_col.map_[hex_map[selo]]] = selected_hex;
				prev_sel = selo;
			}
		}

		my_shader.num_point_lights_ = 0;
		my_shader.num_directional_lights_ = 0;
		my_shader.num_spotlights_ = 0;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	std::cout << "point lights: " << my_shader.num_point_lights_ << std::endl;
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

	//my_cam.look_adjust(x_offset, y_offset);
}