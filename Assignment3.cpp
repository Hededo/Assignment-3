#include <sb7.h>
#include <vmath.h>

#include <object.h>
#include <sb7ktx.h>
#include <shader.h>
#include <assert.h>
#include <cmath>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "objObject.cpp"
#include "lodepng.h"

#define PI 3.14159265
#define DEPTH_TEXTURE_SIZE      4096
#define FRUSTUM_DEPTH           1000

static vmath::vec4 mult(vmath::mat4 m, vmath::vec4 v)
{
	return vmath::vec4(
		v[0] * (m[0][0] + m[1][0] + m[2][0] + m[3][0]),
		v[1] * (m[0][1] + m[1][1] + m[2][1] + m[3][1]),
		v[2] * (m[0][2] + m[1][2] + m[2][2] + m[3][2]),
		v[3] * (m[0][3] + m[1][3] + m[2][3] + m[3][3])
		);
}

static vmath::mat4 ShadowMatrix(vmath::vec4 groundPlane, vmath::vec4 lightPos)
{
	GLfloat dot;
	vmath::mat4 shadowMat;

	dot = groundPlane[0] * lightPos[0] +
		  groundPlane[1] * lightPos[1] +
		  groundPlane[2] * lightPos[2] +
		  groundPlane[3] * lightPos[3];

	shadowMat[0][0] = dot - lightPos[0] + groundPlane[0];
	shadowMat[1][0] = 0.0 - lightPos[0] + groundPlane[1];
	shadowMat[2][0] = 0.0 - lightPos[0] + groundPlane[2];
	shadowMat[3][0] = 0.0 - lightPos[0] + groundPlane[3];

	shadowMat[0][1] = 0.0 - lightPos[1] * groundPlane[0];
	shadowMat[1][1] = dot - lightPos[1] * groundPlane[1];
	shadowMat[2][1] = 0.0 - lightPos[1] * groundPlane[2];
	shadowMat[3][1] = 0.0 - lightPos[1] * groundPlane[3];

	shadowMat[0][2] = 0.0 - lightPos[2] * groundPlane[0];
	shadowMat[1][2] = 0.0 - lightPos[2] * groundPlane[1];
	shadowMat[2][2] = dot - lightPos[2] * groundPlane[2];
	shadowMat[3][2] = 0.0 - lightPos[2] * groundPlane[3];

	shadowMat[0][3] = 0.0 - lightPos[3] * groundPlane[0];
	shadowMat[1][3] = 0.0 - lightPos[3] * groundPlane[1];
	shadowMat[2][3] = 0.0 - lightPos[3] * groundPlane[2];
	shadowMat[3][3] = dot - lightPos[3] * groundPlane[3];

	return shadowMat;
}

class assignment3_app : public sb7::application
{

//Forward Declaration of Public functions
#pragma region protected
public:
	assignment3_app()
		: per_fragment_program(0),
		flatColorProgram(0),
		floorProgram(0),
		projectedShadowProgram(0),
		shadowMapProgram(0)
	{
	}
#pragma endregion

//Forward Declaration of Protected functions and variables
#pragma region protected
protected:
	void init()
	{
		static const char title[] = "Assignment 3";

		sb7::application::init();

		memcpy(info.title, title, sizeof(title));

		info.windowWidth = 512;
		info.windowHeight = 512;
	}

	void startup();
	void render(double currentTime);

	//Listener
	void onKey(int key, int action);
	void onMouseMove(int x, int y);
	void onMouseButton(int button, int action);
	//_________________________________________

	vmath::vec3 getArcballVector(int x, int y);

	void load_shaders();

	GLuint          per_fragment_program;
	GLuint          flatColorProgram;
	GLuint          floorProgram;
	GLuint          projectedShadowProgram;
	GLuint          shadowMapProgram;

	GLuint          tex_floor;

	GLuint          depthBuffer;
	GLuint          depthTexture;

	GLuint          quad_vao;

	//Where uniforms are defined
	struct uniforms_block
	{
		vmath::mat4     mv_matrix;
		vmath::mat4     model_matrix;
		vmath::mat4     view_matrix;
		vmath::mat4     light_view_matrix;
		vmath::mat4     proj_matrix;
		vmath::mat4     shadow_matrix;
		vmath::mat4		depthMVP;
		vmath::vec4     uni_color;
		vmath::vec4     lightPos;
		vmath::vec4	    useUniformColor;
		vmath::vec4	    invertNormals;
		vmath::vec4	    colorPercent;
	};

	float           colorPercent = 0.2f;

	GLuint          uniforms_buffer;

	// Variables for mouse interaction
	bool bPerVertex;
	bool bShiftPressed = false;
	bool bZoom = false;
	bool bRotate = false;
	bool bPan = false;

	int iWidth = info.windowWidth;
	int iHeight = info.windowHeight;

	// Rotation and Translation matricies for moving the camera by mouse interaction.
	vmath::mat4 rotationMatrix = vmath::mat4::identity();
	vmath::mat4 translationMatrix = vmath::mat4::identity();

#pragma region Colors
	const GLfloat skyBlue[4] = { 0.529f, 0.808f, 0.922f, 1.0f };
	const GLfloat ones[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const vmath::vec4 white = vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	const vmath::vec4 orange = vmath::vec4(1.0f, 0.5f, 0.0f, 1.0f);
	const vmath::vec4 purple = vmath::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	const vmath::vec4 gray = vmath::vec4(0.8f, 0.8f, 0.8f, 1.0f);

#pragma endregion

#pragma region Boolean Vectors
const vmath::vec4 falseVec = vmath::vec4(0.0f, 0.0f, 0.0f, 0.0f);
const vmath::vec4 trueVec = vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f);
#pragma endregion

#pragma region Geometery
ObjObject * cube;
ObjObject * sphere;
ObjObject * teapot;

#pragma endregion

#pragma endregion

#pragma region private
private:
	// Variables for mouse position to solve the arcball vectors
	int iPrevMouseX = 0;
	int iPrevMouseY = 0;
	int iCurMouseX = 0;
	int iCurMouseY = 0;

	// Scale of the objects in the scene
	float fScale = 7.0f;

	// Initial position of the camera
	float fXpos = 0.0f;
	float fYpos = 0.0f;
	float fZpos = 75.0f;

	// Initial light pos
	vmath::vec4 initalLightPos = vmath::vec4(0.0f, 20.0f, -2.0f, 1.0f);
	//vmath::vec4 initalLightPos = vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	// Offset move location cube with sphere
	vmath::vec3 lightPosOffset = vmath::vec3(0, 0, 0);

	bool projectedShadows = false;

	struct planesStruct
	{
		vmath::vec4     PosX;
		vmath::vec4     NegX;
		vmath::vec4     PosY;
		vmath::vec4     NegY;
		vmath::vec4     PosZ;
		vmath::vec4     NegZ;
		vmath::vec4		Floor;
		vmath::vec4		Wall;
	};

	planesStruct * planes;


#pragma endregion
};


void assignment3_app::startup()
{
	load_shaders();
	cube = new ObjObject("bin\\media\\objects\\cube.obj");
	sphere = new ObjObject("bin\\media\\objects\\sphere.obj");
	teapot = new ObjObject("bin\\media\\objects\\wt_teapot.obj");

	planes = new planesStruct();
	planes->PosX = vmath::vec4( 1.0f, 0.0f, 0.0f, 1.0f);
	planes->NegX = vmath::vec4(-1.0f, 0.0f, 0.0f, 1.0f);
	planes->PosY = vmath::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	planes->NegY = vmath::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	planes->PosZ = vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	planes->NegZ = vmath::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	planes->Floor = vmath::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	planes->Wall = vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f);


#pragma region Buffer For Uniform Block
	glGenBuffers(1, &uniforms_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniforms_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms_block), NULL, GL_DYNAMIC_DRAW);
#pragma endregion

#pragma region Load Textures
	std::vector<unsigned char> floorImage;
	std::string floorFilePath = "bin\\media\\textures\\floor.png";

	unsigned floor_width, floor_height;
	unsigned error = lodepng::decode(floorImage, floor_width, floor_height, floorFilePath);

	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	// Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
	size_t u2 = 1; while (u2 < floor_width) u2 *= 2;
	size_t v2 = 1; while (v2 < floor_height) v2 *= 2;
	// Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
	double u3 = (double)floor_width / u2;
	double v3 = (double)floor_height / v2;

	// Make power of two version of the image.
	std::vector<unsigned char> floorTexture(u2 * v2 * 4);
	for (size_t y = 0; y < floor_height; y++)
		for (size_t x = 0; x < floor_width; x++)
			for (size_t c = 0; c < 4; c++)
			{
				floorTexture[4 * u2 * y + 4 * x + c] = floorImage[4 * floor_width * y + 4 * x + c];
			}

	// Enable the texture for OpenGL.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); //GL_NEAREST = no smoothing

	// Generate a name for the texture
	glGenTextures(1, &tex_floor); //GLuint tex_floor
	// Now bind it to the context using the GL_TEXTURE_2D binding point
	glBindTexture(GL_TEXTURE_2D, tex_floor); 
	// Specify the amount of storage we want to use for the texture
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, floor_width, floor_height);
	// Assume the texture is already bound to the GL_TEXTURE_2D target
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, floor_width, floor_height, GL_RGBA, GL_UNSIGNED_BYTE, &floorTexture[0]);
#pragma endregion

#pragma region Shadow Map stuff
	glGenFramebuffers(1, &depthBuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, depthBuffer);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexStorage2D(GL_TEXTURE_2D, 11, GL_DEPTH_COMPONENT32F, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

	glGenVertexArrays(1, &quad_vao);
	glBindVertexArray(quad_vao);

#pragma endregion

#pragma region OPENGL Settings

	glEnable(GL_TEXTURE_2D);
    glFrontFace(GL_CW); //glFrontFace(GLenum mode) In a scene composed entirely of opaque closed surfaces, back-facing polygons are never visible.
	glEnable(GL_DEPTH_TEST); //glEnable(GLenum cap) glEnable and glDisable enable and disable various capabilities.
	glDepthFunc(GL_LEQUAL);	//glDepthFunc(GLenum func) specifies the function used to compare each incoming pixel depth value with the depth value present in the depth buffer. 
#pragma endregion
}

void assignment3_app::render(double currentTime)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	const float f = (float)currentTime * 0.1f;

#pragma region Calculations for mouse interaction camera rotation and translation matrix
	float fAngle = 0.0f;
	vmath::vec3 axis_in_camera_coord = (0.0f, 1.0f, 0.0f);
	if (iCurMouseX != iPrevMouseX || iCurMouseY != iPrevMouseY) {
		// Arcball Rotation
		if (bRotate) {
			vmath::vec3 va = getArcballVector(iPrevMouseX, iPrevMouseY);
			vmath::vec3 vb = getArcballVector(iCurMouseX, iCurMouseY);
			fAngle = acos(fmin(1.0f, vmath::dot(va, vb)));
			axis_in_camera_coord = vmath::cross(va, vb);
			axis_in_camera_coord = vmath::normalize(axis_in_camera_coord);
			iPrevMouseX = iCurMouseX;
			iPrevMouseY = iCurMouseY;
			rotationMatrix *= vmath::rotate(vmath::degrees(fAngle), axis_in_camera_coord);
		}
		// Zoom in and out
		if (bZoom) {
			fZpos += (iCurMouseY - iPrevMouseY);
			if (fZpos > 500)
			{
				fZpos = 500;
			}
			else if (fZpos < 10)
			{
				fZpos = 10;
			}
			iPrevMouseY = iCurMouseY;
			iPrevMouseX = iCurMouseX;
		}
		// Pan camera left, right, up, and down
		if (bPan) {
			fXpos += (iCurMouseX - iPrevMouseX);
			fYpos += (iCurMouseY - iPrevMouseY);
			iPrevMouseY = iCurMouseY;
			iPrevMouseX = iCurMouseX;
			translationMatrix = vmath::translate(fXpos / (info.windowWidth / fZpos), -fYpos / (info.windowWidth / fZpos), 0.0f);
		}
	}
#pragma endregion

	glViewport(0, 0, info.windowWidth, info.windowHeight);

	// Create sky blue background
	glClearBufferfv(GL_COLOR, 0, skyBlue);
	glClearBufferfv(GL_DEPTH, 0, ones);

	glBindTexture(GL_TEXTURE_2D, depthTexture);

	// Set up view and perspective matrix
	vmath::vec3 view_position = vmath::vec3(0.0f, 0.0f, fZpos);
	vmath::mat4 view_matrix = vmath::lookat(view_position,
		vmath::vec3(0.0f, 0.0f, 0.0f),
		vmath::vec3(0.0f, 1.0f, 0.0f));
	view_matrix *= translationMatrix;
	view_matrix *= rotationMatrix;

	vmath::mat4 perspective_matrix = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);

	glUnmapBuffer(GL_UNIFORM_BUFFER); //release the mapping of a buffer object's data store into the client's address space
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	uniforms_block * block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

#pragma region Uniforms that remain constant for all geometery
	block->proj_matrix = perspective_matrix;
	vmath::vec4 lightPos = vmath::vec4(initalLightPos[0] + lightPosOffset[0], initalLightPos[1] + lightPosOffset[1], initalLightPos[2] + lightPosOffset[2], 1.0f);
	vmath::mat4 light_view_matrix = vmath::lookat(vmath::vec3(lightPos[0], lightPos[1], lightPos[2]),
		vmath::vec3(0.0f, 0.0f, 0.0f),
		vmath::vec3(0.0f, 1.0f, 0.0f));
	block->light_view_matrix = light_view_matrix;
	block->lightPos = lightPos;

	block->colorPercent = vmath::vec4(colorPercent, colorPercent, colorPercent, colorPercent);
	
	// Compute the MVP matrix from the light's point of view

	vmath::mat4 light_proj_matrix = vmath::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 200.0f);
	vmath::mat4 depthModelMatrix = vmath::mat4(1.0);
	vmath::mat4 depthMVP = light_proj_matrix * light_view_matrix * depthModelMatrix;

	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	block->depthMVP = depthMVP;
#pragma endregion

#pragma region Draw Light Source
	sphere->BindBuffers();

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);

	glUseProgram(flatColorProgram);

	vmath::mat4 model_matrix =
		vmath::translate(block->lightPos[0], block->lightPos[1], block->lightPos[2]);
	block->model_matrix = model_matrix;
	block->mv_matrix = view_matrix * model_matrix;
	block->view_matrix = view_matrix;
	block->uni_color = white;
	block->useUniformColor = trueVec;
	block->invertNormals = falseVec;

	sphere->Draw();
#pragma endregion

#pragma region Draw Room
	cube->BindBuffers();

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);

	glUseProgram(per_fragment_program);

    model_matrix = vmath::scale(50.0f);
	block->model_matrix = model_matrix;
	block->mv_matrix = view_matrix * model_matrix;
	block->view_matrix = view_matrix;
	block->uni_color = gray;
	block->useUniformColor = trueVec;
	block->invertNormals = trueVec;

	glCullFace(GL_BACK);
	cube->Draw();
#pragma endregion

	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#pragma region Draw Teapot Stand Shadow
	cube->BindBuffers();

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);

	glUseProgram(projectedShadowProgram);

	model_matrix =
		vmath::translate(-15.0f, -19.7f, -5.0f) *
		vmath::rotate(45.0f, 0.0f, 1.0f, 0.0f) *
		vmath::scale(10.0f);
	block->model_matrix = model_matrix;
	block->mv_matrix = view_matrix * model_matrix;
	block->view_matrix = view_matrix;
	block->uni_color = orange;
	block->useUniformColor = trueVec;
	block->invertNormals = falseVec;
	block->shadow_matrix = ShadowMatrix(planes->Floor, mult(light_view_matrix, lightPos));

	if (projectedShadows) {
		cube->Draw();
	}

#pragma endregion
	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);

#pragma region Draw Stand
	cube->BindBuffers();

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);

	glUseProgram(per_fragment_program);

	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	cube->Draw();
#pragma endregion

	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#pragma region Draw Teapot Shadow
	teapot->BindBuffers();

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);

	glUseProgram(projectedShadowProgram);

	model_matrix =
		vmath::translate(-15.0f, -14.7f, -5.0f) *
		vmath::rotate(225.0f, 0.0f, 1.0f, 0.0f) *
		vmath::scale(5.0f);
	block->model_matrix = model_matrix;
	block->mv_matrix = view_matrix * model_matrix;
	block->view_matrix = view_matrix;
	block->uni_color = purple;
	block->useUniformColor = trueVec;
	block->invertNormals = falseVec;
	block->shadow_matrix = ShadowMatrix(planes->Floor, mult(light_view_matrix, lightPos));

	if (projectedShadows) {
		teapot->Draw();
	}
	
#pragma endregion
	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);

#pragma region Draw Teapot
	glUseProgram(per_fragment_program);
	teapot->BindBuffers();
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);

	teapot->Draw();
#pragma endregion

	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#pragma region Draw Teapot Shadow
	cube->BindBuffers();

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);

	glUseProgram(projectedShadowProgram);

	model_matrix =
		vmath::translate(10.0f, -19.7f, 0.0f) *
		vmath::scale(10.0f);
	block->model_matrix = model_matrix;
	block->mv_matrix = view_matrix * model_matrix;
	block->view_matrix = view_matrix;
	block->uni_color = white;
	block->useUniformColor = trueVec;
	block->invertNormals = falseVec;
	block->shadow_matrix = ShadowMatrix(planes->Floor, mult(light_view_matrix, lightPos));

	if (projectedShadows) {
		cube->Draw();
	}

#pragma endregion
	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);

#pragma region Draw tatoo free cube
	cube->BindBuffers();
	glUseProgram(per_fragment_program);
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);

	cube->Draw();
#pragma endregion

#pragma region Draw Floor
	cube->BindBuffers();

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);

	glUseProgram(floorProgram);
	glBindTexture(GL_TEXTURE_2D, tex_floor);

	model_matrix =
		vmath::translate(0.0f, -24.9f, 0.0f) *
		vmath::scale(50.0f, 1.0f, 50.0f);
	block->model_matrix = model_matrix;
	block->mv_matrix = view_matrix * model_matrix;
	block->view_matrix = view_matrix;
	block->uni_color = purple;
	block->useUniformColor = trueVec;
	block->invertNormals = falseVec;

	cube->Draw();
#pragma endregion
}

void assignment3_app::load_shaders()
{
	GLuint vs;
	GLuint fs;

	vs = sb7::shader::load("phong_perfragment.vs.txt", GL_VERTEX_SHADER);
	fs = sb7::shader::load("phong_perfragment.fs.txt", GL_FRAGMENT_SHADER);

	if (per_fragment_program)
	{
		glDeleteProgram(per_fragment_program);
	}

	per_fragment_program = glCreateProgram();
	glAttachShader(per_fragment_program, vs);
	glAttachShader(per_fragment_program, fs);
	glLinkProgram(per_fragment_program);

	vs = sb7::shader::load("flatColor.vs.txt", GL_VERTEX_SHADER);
	fs = sb7::shader::load("flatColor.fs.txt", GL_FRAGMENT_SHADER);

	if (flatColorProgram)
	{
		glDeleteProgram(flatColorProgram);
	}

	flatColorProgram = glCreateProgram();
	glAttachShader(flatColorProgram, vs);
	glAttachShader(flatColorProgram, fs);
	glLinkProgram(flatColorProgram);

	vs = sb7::shader::load("floor.vs.txt", GL_VERTEX_SHADER);
	fs = sb7::shader::load("floor.fs.txt", GL_FRAGMENT_SHADER);

	if (floorProgram)
	{
		glDeleteProgram(floorProgram);
	}

	floorProgram = glCreateProgram();
	glAttachShader(floorProgram, vs);
	glAttachShader(floorProgram, fs);
	glLinkProgram(floorProgram);

	vs = sb7::shader::load("projectedShadow.vs.txt", GL_VERTEX_SHADER);
	fs = sb7::shader::load("projectedShadow.fs.txt", GL_FRAGMENT_SHADER);

	if (projectedShadowProgram)
	{
		glDeleteProgram(projectedShadowProgram);
	}

	projectedShadowProgram = glCreateProgram();
	glAttachShader(projectedShadowProgram, vs);
	glAttachShader(projectedShadowProgram, fs);
	glLinkProgram(projectedShadowProgram);

	vs = sb7::shader::load("shadowMap.vs.txt", GL_VERTEX_SHADER);
	fs = sb7::shader::load("shadowMap.fs.txt", GL_FRAGMENT_SHADER);

	if (shadowMapProgram)
	{
		glDeleteProgram(shadowMapProgram);
	}

	shadowMapProgram = glCreateProgram();
	glAttachShader(shadowMapProgram, vs);
	glAttachShader(shadowMapProgram, fs);
	glLinkProgram(shadowMapProgram);
}

#pragma region Event Handlers
void assignment3_app::onKey(int key, int action)
{
	// Check to see if shift was pressed
	if (action == GLFW_PRESS && (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT))
	{
		bShiftPressed = true;
	}
	if (action)
	{
		switch (key)
		{

		case 'R':
			load_shaders();
			rotationMatrix = vmath::mat4::identity();
			translationMatrix = vmath::mat4::identity();
			fXpos = 0.0f;
			fYpos = 0.0f;
			fZpos = 75.0f;
			lightPosOffset = vmath::vec3(0, 0, 0);
			break;
		case '1':
			lightPosOffset[0] += 1;
			break;
		case '2':
			lightPosOffset[0] -= 1;
			break;
		case 'Q':
			lightPosOffset[1] += 1;
			break;
		case 'W':
			lightPosOffset[1] -= 1;
			break;
		case 'A':
			lightPosOffset[2] += 1;
			break;
		case 'S':
			lightPosOffset[2] -= 1;
			break;
		case 'P':
			projectedShadows = !projectedShadows;
			break;
		}
	}
	// Check to see if shift was released
	if (action == GLFW_RELEASE) {
		bShiftPressed = false;
	}
}

void assignment3_app::onMouseButton(int button, int action)
{
	int x, y;

	getMousePosition(x, y);
	// Check to see if left mouse button was pressed for rotation
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		bRotate = true;
		iPrevMouseX = iCurMouseX = x;
		iPrevMouseY = iCurMouseY = y;
	}
	// Check to see if right mouse button was pressed for zoom and pan
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		bZoom = false;
		bPan = false;
		if (bShiftPressed == true)
		{
			bZoom = true;
		}
		else if (bShiftPressed == false)
		{
			bPan = true;
		}
		iPrevMouseX = iCurMouseX = x;
		iPrevMouseY = iCurMouseY = y;
	}
	else {
		bRotate = false;
		bZoom = false;
		bPan = false;
	}

}

void assignment3_app::onMouseMove(int x, int y)
{
	// If rotating, zooming, or panning save mouse x and y
	if (bRotate || bZoom || bPan)
	{
		iCurMouseX = x;
		iCurMouseY = y;
	}
}

// Modified from tutorial at the following website:
// http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball

vmath::vec3 assignment3_app::getArcballVector(int x, int y) {
	// find vector from origin to point on sphere
	vmath::vec3 vecP = vmath::vec3(1.0f*x / info.windowWidth * 2 - 1.0f, 1.0f*y / info.windowHeight * 2 - 1.0f, 0.0f);
	// inverse y due to difference in origin location on the screen
	vecP[1] = -vecP[1];
	float vecPsquared = vecP[0] * vecP[0] + vecP[1] * vecP[1];
	// solve for vector z component
	if (vecPsquared <= 1)
		vecP[2] = sqrt(1 - vecPsquared);
	else
		vecP = vmath::normalize(vecP);
	return vecP;
}
#pragma endregion

DECLARE_MAIN(assignment3_app)
