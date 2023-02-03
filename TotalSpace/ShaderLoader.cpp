//
//  ShaderLoader.cpp
//  TotalSpace
//
//  Created by Gaspar NICOULAUD on 11/02/18.
//  Copyright © 2018 Gaspar Nicoulaud. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include "ShaderLoader.hpp"
#include "assert.h"

static GLuint LoadShader(GLenum type, const char *shader_path);
static char * loadFile(const char * src);

GLuint LoadAndLinkShader(const char *vert_path,const char *frag_path)
{
    //root shader directory path
    std::string root = "/Users/gaspar/Prog/TotalSpace/Shaders/";
    GLuint shader;
    
    std::string vert(vert_path);
    std::string frag(frag_path);
    vert = root + vert;
    frag = root + frag;
    
    const GLuint Vertex = LoadShader(GL_VERTEX_SHADER, vert.c_str());
    const GLuint Frag = LoadShader(GL_FRAGMENT_SHADER, frag.c_str());
    
    shader = glCreateProgram();
    glAttachShader(shader, Vertex);
    glAttachShader(shader, Frag);
    
    //#define PROGRAM_BINARY_RETRIEVABLE_HINT             0x8257
    //glProgramParameteri(shader, PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
    
    glLinkProgram(shader);
    
    //program binary retrieval seems unsupported
    /*#define GL_NUM_PROGRAM_BINARY_FORMATS 0x87FE
    #define GL_PROGRAM_BINARY_FORMATS 0x87FF
    GLint formats = 0;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
    printf("GL_NUM_PROGRAM_BINARY_FORMATS : %i\n",formats);
    GLint *binaryFormats = new GLint[formats];
    glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, binaryFormats);
    #define GL_PROGRAM_BINARY_LENGTH 0x8741
    GLint len=0;
    glGetProgramiv(shader, GL_PROGRAM_BINARY_LENGTH, &len);
    printf("GL_PROGRAM_BINARY_LENGTH : %i\n",len);
    
    GLbyte* binary = new GLbyte[len];
    glGetProgramBinary(shader, len, NULL, (GLenum*)binaryFormats, binary);
    FILE* fp = fopen("/Users/gaspar/Desktop/TotalSpaceShader.bin", "wb");
    fwrite(binary, len, 1, fp);
    fclose(fp);
    delete [] binary;*/
    
    return shader;
}

static GLuint LoadShader(GLenum type, const char *shader_path) {
    GLint compiled = 0;
    
    // Create the shader object
    const GLuint shader = glCreateShader(type);
    if (shader == 0) {
        printf("!! failed creating the shader\n");
        return 0;
    }
    
    char * shader_src = loadFile(shader_path);
    //printf("File loaded : \n%s\n",shader_src);
    
    // Load the shader source
    glShaderSource(shader, 1, &shader_src, NULL);
    delete[] shader_src;
    
    // Compile the shader
    glCompileShader(shader);
    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    
    if (compiled == GL_FALSE) {
        GLint info_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
        
        if (info_len > 1) {
            char *info_log = ((char *)malloc(sizeof(char) * info_len));
            glGetShaderInfoLog(shader, info_len, NULL, info_log);
            printf("!! Error loading shader : %s\n",info_log);
            free(info_log);
        }
        glDeleteShader(shader);
        printf("!! Failed loading shader %s\n",shader_path);
        assert(1 == 2); // CHECK CONSOLE FOR SHADER COMPILE ERROR
        return 0;
    }
    
    //printf("Shader %s was succesfully loaded and compiled\n",shader_path);
    return shader;
}

static char * loadFile(const char * src)
{
    std::ifstream file (src);
    std::string contents((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    char * retval = new char[contents.length()];
    
    if(contents.length() < 10)
    {
        printf("!! Failed loading shader %s !!\n   !! File not found or empty !!\n  !! Maybe you forgot to add the directory name to the path !!\n",src);
        assert(1 == 2);
    }
    
    strcpy (retval,contents.c_str());
    file.close();
    //printf("File loaded : \n%s",retval);
    return retval;
}




