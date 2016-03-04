#include <sb7.h>
#include <vmath.h>

#include <object.h>
#include <sb7ktx.h>
#include <shader.h>
#include <assert.h>
#include <cmath>

#include <vector>

#define PI 3.14159265
#define MANY_OBJECTS 1
#undef MANY_OBJECTS

class assignment3_app : public sb7::application
{

//Forward Declaration of Public functions
#pragma region protected
public:
	assignment3_app()
		: per_fragment_program(0),
		checkerFloorProgram(0),
		tex_index(0),
		render_prog(0)
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

	GLuint          per_fragment_program; //never used
	GLuint          checkerFloorProgram;
	GLuint          render_prog;
	GLuint          skybox_prog;

	GLuint          tex_object[2];
	GLuint          tex_index;
	GLuint          tex_envmap;
	GLuint          face_envmap;

	//Where uniforms are defined
	struct uniforms_block
	{
		vmath::mat4     mv_matrix;
		vmath::mat4     model_matrix;
		vmath::mat4     view_matrix;
		vmath::mat4     proj_matrix;
		vmath::vec4     uni_color;
		vmath::vec4     lightPos;
		vmath::vec4	    useUniformColor;
		vmath::vec4	    invertNormals;
		vmath::vec4	    colorPercent;
	};

	float           colorPercent = 0.2f;

	GLuint          uniforms_buffer;

	sb7::object     object;

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

#pragma region Vertex Data
	static const int numberOfCubeVertices = 36; 

	static const int sizeOfCubeBuffers = numberOfCubeVertices * 4;

	const GLfloat cube_data[sizeOfCubeBuffers] = {
		//B
		-1.0f, 1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f, 1.0f,

		//R
		1.0f, -1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 1.0f,

		//F
		1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,

		-1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,

		//L
		-1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f,

		-1.0f, -1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f,

		//D
		-1.0f, -1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 1.0f,

		//U
		-1.0f, 1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f, 1.0f,
		//End Cube
	};

	const GLfloat color_data[sizeOfCubeBuffers] = {
		//B (Green)
		0.0f,  1.0f,  0.0f, 1.0f,
		0.0f,  1.0f,  0.0f, 1.0f,
		0.0f,  1.0f,  0.0f, 1.0f,

		0.0f,  1.0f,  0.0f, 1.0f,
		0.0f,  1.0f,  0.0f, 1.0f,
		0.0f,  1.0f,  0.0f, 1.0f,

		//R (Red)
		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,

		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,

		//F (Green)
		0.0f,  1.0f,  0.0f, 1.0f,
		0.0f,  1.0f,  0.0f, 1.0f,
		0.0f,  1.0f,  0.0f, 1.0f,

		0.0f,  1.0f,  0.0f, 1.0f,
		0.0f,  1.0f,  0.0f, 1.0f,
		0.0f,  1.0f,  0.0f, 1.0f,

		//L (Red)
		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,

		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,

		//D (Blue)
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,

		//U
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		//End Cube
	};

	const GLfloat normals_data[sizeOfCubeBuffers] = {
		//B (Green)
		0.0f,  0.0f,  -1.0f, 1.0f,
		0.0f,  0.0f,  -1.0f, 1.0f,
		0.0f,  0.0f,  -1.0f, 1.0f,

		0.0f,  0.0f,  -1.0f, 1.0f,
		0.0f,  0.0f,  -1.0f, 1.0f,
		0.0f,  0.0f,  -1.0f, 1.0f,

		//R (Red)
		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,

		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  0.0f,  0.0f,  1.0f,

		//F (Green)
		0.0f,  0.0f,  1.0f, 1.0f,
		0.0f,  0.0f,  1.0f, 1.0f,
		0.0f,  0.0f,  1.0f, 1.0f,

		0.0f,  0.0f,  1.0f, 1.0f,
		0.0f,  0.0f,  1.0f, 1.0f,
		0.0f,  0.0f,  1.0f, 1.0f,

		//L (Red)
		-1.0f,  0.0f,  0.0f,  1.0f,
		-1.0f,  0.0f,  0.0f,  1.0f,
		-1.0f,  0.0f,  0.0f,  1.0f,

		-1.0f,  0.0f,  0.0f,  1.0f,
		-1.0f,  0.0f,  0.0f,  1.0f,
		-1.0f,  0.0f,  0.0f,  1.0f,

		//D (Blue)
		0.0f, -1.0f, 0.0f, 1.0f,
		0.0f, -1.0f, 0.0f, 1.0f,
		0.0f, -1.0f, 0.0f, 1.0f,

		0.0f, -1.0f, 0.0f, 1.0f,
		0.0f, -1.0f, 0.0f, 1.0f,
		0.0f, -1.0f, 0.0f, 1.0f,

		//U
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		//End Cube
	};

#define B 0x00, 0x00, 0x00, 0x00
#define W 0xFF, 0xFF, 0xFF, 0xFF
const GLubyte tex_data[16 * 16 * 4] =
{
	//B, B, W, W, B, B, W, W, B, B, W, W, B, B, W, W,
	//B, B, W, W, B, B, W, W, B, B, W, W, B, B, W, W,
	//W, W, B, B, W, W, B, B, W, W, B, B, W, W, B, B,
	//W, W, B, B, W, W, B, B, W, W, B, B, W, W, B, B,
	//B, B, W, W, B, B, W, W, B, B, W, W, B, B, W, W,
	//B, B, W, W, B, B, W, W, B, B, W, W, B, B, W, W,
	//W, W, B, B, W, W, B, B, W, W, B, B, W, W, B, B,
	//W, W, B, B, W, W, B, B, W, W, B, B, W, W, B, B,
	//B, B, W, W, B, B, W, W, B, B, W, W, B, sB, W, W,
	//B, B, W, W, B, B, W, W, B, B, W, W, B, B, W, W,
	//W, W, B, B, W, W, B, B, W, W, B, B, W, W, B, B,
	//W, W, B, B, W, W, B, B, W, W, B, B, W, W, B, B,
	//B, B, W, W, B, B, W, W, B, B, W, W, B, B, W, W,
	//B, B, W, W, B, B, W, W, B, B, W, W, B, B, W, W,
	//W, W, B, B, W, W, B, B, W, W, B, B, W, W, B, B,
	//W, W, B, B, W, W, B, B, W, W, B, B, W, W, B, B
	B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
	W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
	B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
	W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
	B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
	W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
	B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
	W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
	B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
	W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
	B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
	W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
	B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
	W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B,
	B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
	W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B
};
#undef B
#undef W
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
	float iLightPosX = 1.0f;
	float iLightPosY = 0.5f;
	float iLightPosZ = 0.3f;
	vmath::vec4 lightPos = vmath::vec4(iLightPosX, iLightPosY, iLightPosZ, 1.0f);

	GLuint buffer;
	GLuint colorBuffer;
	GLuint normalsBuffer;
	GLuint cube_vao;
	GLuint sphere_vao;
#pragma endregion
};


void assignment3_app::startup()
{
	load_shaders();

	glGenVertexArrays(1, &cube_vao);  //glGenVertexArrays(n, &array) returns n vertex array object names in arrays
	glBindVertexArray(cube_vao); //glBindVertexArray(array) binds the vertex array object with name array.

#pragma region Cube Pos Buffer
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(cube_data),
		cube_data,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion

#pragma region Cube Color Buffer
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(color_data),
		color_data,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion

#pragma region Cube Normals Buffer
	glGenBuffers(1, &normalsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(normals_data),
		normals_data,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion

#pragma region Buffer For Uniform Block
	glGenBuffers(1, &uniforms_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniforms_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms_block), NULL, GL_DYNAMIC_DRAW);
#pragma endregion

#pragma region Sphere vertex data
	glGenVertexArrays(1, &sphere_vao);
	glBindVertexArray(sphere_vao);
	object.load("bin\\media\\objects\\sphere.sbm");
#pragma endregion

#pragma region Bind Floor Texture
	// Generate a name for the texture
	glGenTextures(1, &tex_object[0]);
	// Now bind it to the context using the GL_TEXTURE_2D binding point
	glBindTexture(GL_TEXTURE_2D, tex_object[0]);
	// Specify the amount of storage we want to use for the texture
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 16, 16);
	// Assume the texture is already bound to the GL_TEXTURE_2D target
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);

	tex_object[1] = sb7::ktx::file::load("pattern1.ktx");
#pragma endregion

#pragma region Bind envMap Texture
	tex_envmap = sb7::ktx::file::load("mountaincube.ktx");
	face_envmap = sb7::ktx::file::load("face.ktx");
#pragma endregion


#pragma region OPENGL Settings

	glEnable(GL_CULL_FACE); // Use face culling to see into the room.
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
		//Light position tracks with the camera
		lightPos = vmath::vec4(iLightPosX * translationMatrix[0][0] + iLightPosX * translationMatrix[0][1] + iLightPosX * translationMatrix[0][2],
			iLightPosY * translationMatrix[1][0] + iLightPosY * translationMatrix[1][1] + iLightPosY * translationMatrix[1][2],
			iLightPosZ * translationMatrix[2][0] + iLightPosZ * translationMatrix[2][1] + iLightPosZ * translationMatrix[2][2],
			1.0f
			);
		lightPos = vmath::vec4(lightPos[0] * rotationMatrix[0][0] + lightPos[0] * rotationMatrix[0][1] + lightPos[0] * rotationMatrix[0][2],
			lightPos[1] * rotationMatrix[1][0] + lightPos[1] * rotationMatrix[1][1] + lightPos[1] * rotationMatrix[1][2],
			lightPos[2] * rotationMatrix[2][0] + lightPos[2] * rotationMatrix[2][1] + lightPos[2] * rotationMatrix[2][2],
			1.0f
			);
	}
#pragma endregion

	glViewport(0, 0, info.windowWidth, info.windowHeight);

	glBindTexture(GL_TEXTURE_2D, tex_object[tex_index]);

	// Create sky blue background
	glClearBufferfv(GL_COLOR, 0, skyBlue);
	glClearBufferfv(GL_DEPTH, 0, ones);

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
	block->lightPos = lightPos;
	block->colorPercent = vmath::vec4(colorPercent, colorPercent, colorPercent, colorPercent);
#pragma endregion

	glUseProgram(render_prog);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);
#pragma region Draw Semi-Reflective Sphere
	glBindVertexArray(sphere_vao);
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);


	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);
	vmath::mat4 model_matrix =
		//vmath::translate(-9.3f, -16.0f, 1.0f) *
		vmath::translate(10.0f, -6.3f, -1.0f) *
		vmath::scale(6.0f);
	block->model_matrix = model_matrix;
	block->mv_matrix = view_matrix * model_matrix;
	block->view_matrix = view_matrix;
	block->uni_color = orange;
	block->useUniformColor = trueVec;
	block->invertNormals = falseVec;

	glCullFace(GL_BACK);
	object.render();
#pragma endregion

#pragma region Draw Reflective Sphere
	glBindVertexArray(sphere_vao);
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);

	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);
    model_matrix =
		vmath::translate(-9.3f, -13.0f, 1.0f) *
		vmath::scale(6.0f);
	block->model_matrix = model_matrix;
	block->mv_matrix = view_matrix * model_matrix;
	block->view_matrix = view_matrix;
	block->uni_color = purple;
	block->useUniformColor = falseVec;
	block->invertNormals = falseVec;

	object.render();
#pragma endregion

	glUnmapBuffer(GL_UNIFORM_BUFFER); //release the mapping of a buffer object's data store into the client's address space
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);

#pragma region Uniforms that remain constant for cubes
	block->uni_color = orange;
	block->useUniformColor = falseVec;
#pragma endregion

#pragma region bind cube vertex data
	glBindVertexArray(cube_vao);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glEnableVertexAttribArray(0); //enable or disable a generic vertex attribute array
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0); //define an array of generic vertex attribute data void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)

	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glEnableVertexAttribArray(1); //enable or disable a generic vertex attribute array
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0); //define an array of generic vertex attribute data void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)

	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glEnableVertexAttribArray(2); //enable or disable a generic vertex attribute array
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0); //define an array of generic vertex attribute data void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);
#pragma endregion

	glUseProgram(checkerFloorProgram);
#pragma region Draw Floor

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, tex_object[0]);
	glUnmapBuffer(GL_UNIFORM_BUFFER); //release the mapping of a buffer object's data store into the client's address space
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);

	model_matrix =
		//vmath::rotate((float)currentTime * 14.5f, 0.0f, 1.0f, 0.0f) * //used to constantly rotate
		vmath::translate(vmath::vec3(0.0f, -22.5f, 0.0f)) * 
		vmath::rotate(45.0f, 0.0f, 1.0f, 0.0f)*
		vmath::scale(vmath::vec3(30.0f, 0.0f, 30.0f));
	block->model_matrix = model_matrix;
	block->mv_matrix = view_matrix * model_matrix;
	block->view_matrix = view_matrix;
	block->invertNormals = trueVec;
	block->uni_color = gray;

	glCullFace(GL_FRONT);
	glDrawArrays(GL_TRIANGLES, 0, numberOfCubeVertices);

#pragma endregion

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glUseProgram(skybox_prog);
#pragma region Draw Face Cube

	glBindTexture(GL_TEXTURE_CUBE_MAP, face_envmap);
	glUnmapBuffer(GL_UNIFORM_BUFFER); //release the mapping of a buffer object's data store into the client's address space
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);

	model_matrix =
		vmath::rotate(0.0f, 0.0f, 1.0f, 0.0f) *
		vmath::translate(10.0f, -17.3f, -1.0f) *
		vmath::scale(5.0f);
	block->model_matrix = model_matrix;
	block->mv_matrix = view_matrix * model_matrix;
	block->view_matrix = view_matrix;
	block->useUniformColor = falseVec;
	block->invertNormals = trueVec;

	glCullFace(GL_BACK);
	glDrawArrays(GL_TRIANGLES, 0, numberOfCubeVertices);
#pragma endregion

	
	//glUseProgram(skybox_prog);
#pragma region Draw Skybox
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	
	glUnmapBuffer(GL_UNIFORM_BUFFER); //release the mapping of a buffer object's data store into the client's address space
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
	block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT);

	model_matrix =
		vmath::translate(0.0f, 0.0f, 0.0f) *
		vmath::scale(150.0f);
	block->model_matrix = model_matrix;
	block->mv_matrix = view_matrix * model_matrix;
	block->view_matrix = view_matrix;

	glCullFace(GL_FRONT);
	glDrawArrays(GL_TRIANGLES, 0, numberOfCubeVertices);
#pragma endregion
}

/*
	Makes two separate programs that uses either perFragment or perVertex phong lighting by reading shader code from files. 
	From Phonglighting.cpp
*/
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

	vs = sb7::shader::load("floor.vs.txt", GL_VERTEX_SHADER);
	fs = sb7::shader::load("floor.fs.txt", GL_FRAGMENT_SHADER);

	if (checkerFloorProgram)
	{
		glDeleteProgram(checkerFloorProgram);
	}

	checkerFloorProgram = glCreateProgram();
	glAttachShader(checkerFloorProgram, vs);
	glAttachShader(checkerFloorProgram, fs);
	glLinkProgram(checkerFloorProgram);

	//________________________________________________________________
	if (render_prog)
		glDeleteProgram(render_prog);

	vs = sb7::shader::load("render.vs.txt", GL_VERTEX_SHADER);
	fs = sb7::shader::load("render.fs.txt", GL_FRAGMENT_SHADER);

	render_prog = glCreateProgram();
	glAttachShader(render_prog, vs);
	glAttachShader(render_prog, fs);
	glLinkProgram(render_prog);

	glDeleteShader(vs);
	glDeleteShader(fs);

	vs = sb7::shader::load("skybox.vs.txt", GL_VERTEX_SHADER);
	fs = sb7::shader::load("skybox.fs.txt", GL_FRAGMENT_SHADER);

	skybox_prog = glCreateProgram();
	glAttachShader(skybox_prog, vs);
	glAttachShader(skybox_prog, fs);
	glLinkProgram(skybox_prog);

	glDeleteShader(vs);
	glDeleteShader(fs);
	//___________________________________________________________
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
			break;
		case 'T':
			tex_index++;
			if (tex_index > 1)
				tex_index = 0;
			break;
		case 'P':
			if (colorPercent < 1.0) 
			{
				colorPercent += 0.05;
			}
			break;
		case 'O':
			if (colorPercent > 0.0)
			{
				colorPercent -= 0.05;
			}
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
