#pragma once

//libraries go here:
#include <sb7.h>
#include <string>
#include <vector>

class ObjObject
{

public:
	std::string fileName;
	std::vector<GLfloat> * verticies;
	std::vector<GLfloat> * normals;
	GLuint vertexCount;

	GLuint vao;
	GLuint vertexbuffer;
	GLuint colorBuffer;
	GLuint normalsBuffer;

	ObjObject::ObjObject(std::string _fileName)
	{
		fileName = _fileName;
		verticies = new std::vector<GLfloat>;
		normals = new std::vector<GLfloat>;
		vertexCount = 0;
		CreateFromFile();
	}

	ObjObject::~ObjObject()
	{
		delete(verticies);
		delete(normals);
		verticies = NULL;
		normals = NULL;
	}

	void ObjObject::BindBuffers()
	{
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glEnableVertexAttribArray(0); //enable or disable a generic vertex attribute array
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0); //define an array of generic vertex attribute data void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)

		glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
		glEnableVertexAttribArray(1); //enable or disable a generic vertex attribute array
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0); //define an array of generic vertex attribute data void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)

		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glEnableVertexAttribArray(2); //enable or disable a generic vertex attribute array
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0); //define an array of generic vertex attribute data void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
	}

	void ObjObject::Draw()
	{
		glDrawArrays(GL_TRIANGLES, 0, vertexCount * 4);
	}

private:
	void ObjObject::CreateFromFile()
	{
		std::ifstream file = std::ifstream(fileName);
		std::string line;
		while (std::getline(file, line))
		{
			std::stringstream stream(line);
			std::string firstWord;
			stream >> firstWord;
			std::string a, b, c;

			bool matches = (std::strcmp(firstWord.c_str(), "v")) == 0;
			if (matches)
			{
				stream >> a >> b >> c;
				verticies->push_back(std::stof(a));
				verticies->push_back(std::stof(b));
				verticies->push_back(std::stof(c));
				verticies->push_back(1.0f);
				vertexCount += 1;
				continue;
			}
			matches = (std::strcmp(firstWord.c_str(), "vn")) == 0;
			if (matches)
			{
				stream >> a >> b >> c;
				normals->push_back(std::stof(a));
				normals->push_back(std::stof(b));
				normals->push_back(std::stof(c));
				normals->push_back(1.0f);
				continue;
			}
			matches = (std::strcmp(firstWord.c_str(), "f")) == 0;
			if (matches)
			{
				break;
			}
			else
			{
				continue;
			}
		}

		glGenVertexArrays(1, &vao);  //glGenVertexArrays(n, &array) returns n vertex array object names in arrays
		glBindVertexArray(vao); //glBindVertexArray(array) binds the vertex array object with name array.

		GLuint bufferSize = verticies->size() * sizeof(GLfloat);
#pragma region Cube Pos Buffer
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER,
			bufferSize,
			verticies->data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion

#pragma region Cube Color Buffer
		glGenBuffers(1, &colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER,
			bufferSize,
			verticies->data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion

#pragma region Cube Normals Buffer
		glGenBuffers(1, &normalsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
		glBufferData(GL_ARRAY_BUFFER,
			bufferSize,
			normals->data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion
	}
};
