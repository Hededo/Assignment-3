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
		createFromFile();
	}

	ObjObject::~ObjObject()
	{
		delete(verticies);
		delete(normals);
		verticies = NULL;
		normals = NULL;
	}

private:
	void ObjObject::createFromFile()
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

#pragma region Cube Pos Buffer
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(verticies->data()),
			verticies->data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion

#pragma region Cube Color Buffer
		glGenBuffers(1, &colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(verticies->data()),
			verticies->data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion

#pragma region Cube Normals Buffer
		glGenBuffers(1, &normalsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(normals->data()),
			normals->data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion
	}
};
