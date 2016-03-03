///*
// * Copyright © 2012-2015 Graham Sellers
// *
// * Permission is hereby granted, free of charge, to any person obtaining a
// * copy of this software and associated documentation files (the "Software"),
// * to deal in the Software without restriction, including without limitation
// * the rights to use, copy, modify, merge, publish, distribute, sublicense,
// * and/or sell copies of the Software, and to permit persons to whom the
// * Software is furnished to do so, subject to the following conditions:
// *
// * The above copyright notice and this permission notice (including the next
// * paragraph) shall be included in all copies or substantial portions of the
// * Software.
// *
// * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// * DEALINGS IN THE SOFTWARE.
// */
//
//#include <sb7.h>
//#include <vmath.h>
//
//#include <string>
//static void print_shader_log(GLuint shader)
//{
//    std::string str;
//    GLint len;
//
//    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
//    if (len != 0)
//    {
//        str.resize(len);
//        glGetShaderInfoLog(shader, len, NULL, &str[0]);
//    }
//
//#ifdef _WIN32
//    OutputDebugStringA(str.c_str());
//#endif
//}
//
//static const char * vs_source[] =
//{
//    "#version 420 core                                                              \n"
//    "                                                                               \n"
//    "void main(void)                                                                \n"
//    "{                                                                              \n"
//    "    const vec4 vertices[] = vec4[](vec4( 1.0, -1.0, 1.0, 1.0),               \n"
//    "                                   vec4(-1.0, -1.0, 1.0, 1.0),               \n"
//    "                                   vec4( 1.0,  1.0, 1.0, 1.0));              \n"
//    "                                                                               \n"
//    "    gl_Position = vertices[gl_VertexID];                                       \n"
//    "}                                                                              \n"
//};
//
//static const char * fs_source[] =
//{
//    "#version 430 core                                                              \n"
//    "                                                                               \n"
//    "uniform sampler2D s;                                                           \n"
//    "                                                                               \n"
//    "out vec4 color;                                                                \n"
//    "                                                                               \n"
//    "void main(void)                                                                \n"
//    "{                                                                              \n"
//    "    color = texture(s, gl_FragCoord.xy / textureSize(s, 0));                   \n"
//    "}                                                                              \n"
//};
//
//class simpletexture_app : public sb7::application
//{
//public:
//    void init()
//    {
//        static const char title[] = "OpenGL SuperBible - Simple Texturing";
//
//        sb7::application::init();
//
//        memcpy(info.title, title, sizeof(title));
//    }
//
//    void startup(void)
//    {
//
//		// Generate a name for the texture
//		glGenTextures(1, &tex_object);
//		// Now bind it to the context using the GL_TEXTURE_2D binding point
//		glBindTexture(GL_TEXTURE_2D, tex_object);
//		// Specify the amount of storage we want to use for the texture
//		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 16, 16);
//		// Assume the texture is already bound to the GL_TEXTURE_2D target
//		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//        program = glCreateProgram();
//        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
//        glShaderSource(fs, 1, fs_source, NULL);
//        glCompileShader(fs);
//
//        print_shader_log(fs);
//
//        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
//        glShaderSource(vs, 1, vs_source, NULL);
//        glCompileShader(vs);
//
//        print_shader_log(vs);
//
//        glAttachShader(program, vs);
//        glAttachShader(program, fs);
//
//        glLinkProgram(program);
//
//        glGenVertexArrays(1, &vao);
//        glBindVertexArray(vao);
//    }
//
//    void shutdown(void)
//    {
//        glDeleteProgram(program);
//        glDeleteVertexArrays(1, &vao);
//        glDeleteTextures(1, &texture);
//    }
//
//    void render(double t)
//    {
//        static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
//        glClearBufferfv(GL_COLOR, 0, green);
//
//        glUseProgram(program);
//        glDrawArrays(GL_TRIANGLES, 0, 3);
//    }
//
//private:
//    GLuint      texture;
//    GLuint      program;
//    GLuint      vao;
//	GLuint      tex_object;
//
//	#define B 0x00, 0x00, 0x00, 0x00
//	#define W 0xFF, 0xFF, 0xFF, 0xFF
//	     const GLubyte tex_data[16 * 16 * 4] =
//		{
//			B, B, B, B, B, B, B, B, W, W, W, W, W, W, W, W,
//			B, B, B, B, B, B, B, B, W, W, W, W, W, W, W, W,
//			B, B, B, B, B, B, B, B, W, W, W, W, W, W, W, W,
//		    B, B, B, B, B, B, B, B, W, W, W, W, W, W, W, W,
//			B, B, B, B, B, B, B, B, W, W, W, W, W, W, W, W,
//			B, B, B, B, B, B, B, B, W, W, W, W, W, W, W, W,
//			B, B, B, B, B, B, B, B, W, W, W, W, W, W, W, W,
//			B, B, B, B, B, B, B, B, W, W, W, W, W, W, W, W,
//			W, W, W, W, W, W, W, W, B, B, B, B, B, B, B, B,
//			W, W, W, W, W, W, W, W, B, B, B, B, B, B, B, B,
//			W, W, W, W, W, W, W, W, B, B, B, B, B, B, B, B,
//			W, W, W, W, W, W, W, W, B, B, B, B, B, B, B, B,
//			W, W, W, W, W, W, W, W, B, B, B, B, B, B, B, B,
//			W, W, W, W, W, W, W, W, B, B, B, B, B, B, B, B,
//			W, W, W, W, W, W, W, W, B, B, B, B, B, B, B, B,
//			W, W, W, W, W, W, W, W, B, B, B, B, B, B, B, B
//		};
//	#undef B
//	#undef W
//};
//
//DECLARE_MAIN(simpletexture_app);
