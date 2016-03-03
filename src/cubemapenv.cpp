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
//#include <object.h>
//#include <shader.h>
//#include <sb7ktx.h>
//
//class cubemapenv_app : public sb7::application
//{
//public:
//    cubemapenv_app()
//        :
//          render_prog(0)
//    {
//    }
//
//protected:
//
//	struct uniforms_block
//	{
//		vmath::mat4     mv_matrix;
//		vmath::mat4     view_matrix;
//		vmath::mat4     proj_matrix;
//		vmath::vec4     uni_color;
//		vmath::vec4     lightPos;
//		vmath::vec4	    useUniformColor;
//		vmath::vec4	    invertNormals;
//	};
//
//	GLuint          uniforms_buffer;
//
//    void init()
//    {
//        static const char title[] = "OpenGL SuperBible - Cubic Environment Map";
//
//        sb7::application::init();
//
//        memcpy(info.title, title, sizeof(title));
//    }
//
//    virtual void startup()
//    {
//		tex_envmap = sb7::ktx::file::load("mountaincube.ktx");
//
//        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
//
//		glGenBuffers(1, &uniforms_buffer);
//		glBindBuffer(GL_UNIFORM_BUFFER, uniforms_buffer);
//		glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms_block), NULL, GL_DYNAMIC_DRAW);
//
//		object.load("bin\\media\\objects\\sphere.sbm");
//
//        load_shaders();
//
//        glGenVertexArrays(1, &skybox_vao);
//        glBindVertexArray(skybox_vao);
//
//        glDepthFunc(GL_LEQUAL);
//    }
//
//    virtual void render(double currentTime)
//    {
//        static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
//        static const GLfloat ones[] = { 1.0f };
//        const float f = (float)currentTime * 0.1f;
//
//		glUnmapBuffer(GL_UNIFORM_BUFFER); //release the mapping of a buffer object's data store into the client's address space
//		glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
//		uniforms_block * block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
//
//		vmath::mat4 proj_matrix = vmath::perspective(60.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
//		vmath::mat4 view_matrix = vmath::lookat(vmath::vec3(15.0f * sinf(f), 0.0f, 15.0f * cosf(f)),
//                                                vmath::vec3(0.0f, 0.0f, 0.0f),
//                                                vmath::vec3(0.0f, 1.0f, 0.0f));
//		vmath::mat4 mv_matrix = view_matrix *
//                                vmath::rotate(f, 1.0f, 0.0f, 0.0f) *
//                                vmath::rotate(f * 130.1f, 0.0f, 1.0f, 0.0f) *
//                                vmath::translate(0.0f, -4.0f, 0.0f);
//
//		block->proj_matrix = proj_matrix;
//		block->view_matrix = view_matrix;
//		block->mv_matrix = mv_matrix;
//
//        glClearBufferfv(GL_COLOR, 0, gray);
//        glClearBufferfv(GL_DEPTH, 0, ones);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);
//
//        glViewport(0, 0, info.windowWidth, info.windowHeight);
//
//        glUseProgram(skybox_prog);
//        glBindVertexArray(skybox_vao);
//
//        glDisable(GL_DEPTH_TEST);
//
//        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//
//        glUseProgram(render_prog);
//
//        glEnable(GL_DEPTH_TEST);
//
//        object.render();
//    }
//
//    virtual void shutdown()
//    {
//        glDeleteProgram(render_prog);
//        //glDeleteTextures(3, tex_envmap);
//    }
//
//    void load_shaders()
//    {
//        if (render_prog)
//            glDeleteProgram(render_prog);
//
//        GLuint vs, fs;
//
//        vs = sb7::shader::load("render.vs.txt", GL_VERTEX_SHADER);
//        fs = sb7::shader::load("render.fs.txt", GL_FRAGMENT_SHADER);
//
//        render_prog = glCreateProgram();
//        glAttachShader(render_prog, vs);
//        glAttachShader(render_prog, fs);
//        glLinkProgram(render_prog);
//
//        glDeleteShader(vs);
//        glDeleteShader(fs);
//
//        vs = sb7::shader::load("skybox.vs.txt", GL_VERTEX_SHADER);
//        fs = sb7::shader::load("skybox.fs.txt", GL_FRAGMENT_SHADER);
//
//        skybox_prog = glCreateProgram();
//        glAttachShader(skybox_prog, vs);
//        glAttachShader(skybox_prog, fs);
//        glLinkProgram(skybox_prog);
//
//        glDeleteShader(vs);
//        glDeleteShader(fs);
//    }
//
//    virtual void onKey(int key, int action)
//    {
//        if (action)
//        {
//            switch (key)
//            {
//                case 'R': load_shaders();
//                    break;
//            }
//        }
//    }
//
//protected:
//    GLuint          render_prog;
//    GLuint          skybox_prog;
//
//    GLuint          tex_envmap;
//
//    sb7::object     object;
//
//    GLuint          skybox_vao;
//};
//
//DECLARE_MAIN(cubemapenv_app)
