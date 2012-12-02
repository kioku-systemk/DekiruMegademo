
// GL Headers
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include "glext.h"
#endif

#define SHADER(A) #A

class SceneBase
{
protected:
	SceneBase(){ m_resolution[0] = m_resolution[1] = 0; }
	float m_resolution[2];
public:
	virtual ~SceneBase(){}
	virtual void Init() = 0;
	virtual void Draw(double time, const float* fft) = 0;
	void Resize(int w, int h)
	{
		m_resolution[0] = static_cast<float>(w);
		m_resolution[1] = static_cast<float>(h);
	}
};

#if __APPLE__
void printShaderInfoLog(GLuint shader)
{
    GLsizei bufSize;
    
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &bufSize);
    
    if (bufSize > 1) {
        GLchar *infoLog;
        
        infoLog = (GLchar *)malloc(bufSize);
        if (infoLog != NULL) {
            GLsizei length;
            
            glGetShaderInfoLog(shader, bufSize, &length, infoLog);
            fprintf(stderr, "InfoLog:\n%s\n\n", infoLog);
            free(infoLog);
        }
        else
            fprintf(stderr, "Could not allocate InfoLog buffer.\n");
    }
}

void printProgramInfoLog(GLuint program)
{
    GLsizei bufSize;
    
    glGetProgramiv(program, GL_INFO_LOG_LENGTH , &bufSize);
    
    if (bufSize > 1) {
        GLchar *infoLog;
        
        infoLog = (GLchar *)malloc(bufSize);
        if (infoLog != NULL) {
            GLsizei length;
            
            glGetProgramInfoLog(program, bufSize, &length, infoLog);
            fprintf(stderr, "InfoLog:\n%s\n\n", infoLog);
            free(infoLog);
        }
        else
            fprintf(stderr, "Could not allocate InfoLog buffer.\n");
    }
}

#endif

class ShaderDrawer
{
public:
    ShaderDrawer(const char* fragmentShader)
    {
        static const char* vsh = "void main(){gl_Position = gl_Vertex;}";
#if _WIN32
		const GLuint p = ((PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram"))();
		GLuint s = ((PFNGLCREATESHADERPROC)(wglGetProcAddress("glCreateShader")))(GL_VERTEX_SHADER);
		((PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource"))(s,1, &vsh,0);
		((PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader"))(s);
		((PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader"))(p,s);
		s = ((PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader"))(GL_FRAGMENT_SHADER);
		((PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource"))(s,1, &fragmentShader,0);
		((PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader"))(s);
		((PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader"))(p,s);
		((PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram"))(p);
#else
		const GLuint p = glCreateProgram();
		GLuint s = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(s,1, &vsh,0);
		glCompileShader(s);
        printShaderInfoLog(s);
		glAttachShader(p,s);
		s = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(s,1, &fragmentShader,0);
		glCompileShader(s);
        printShaderInfoLog(s);
		glAttachShader(p,s);
		glLinkProgram(p);
        printProgramInfoLog(p);
#endif
		m_p = p;
    }
    ~ShaderDrawer()
    {
    }

    void Bind()
    {
#if _WIN32
		((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))(m_p);
#else
        glUseProgram(m_p);
#endif
    }
    void Unbind()
    {
#if _WIN32
		((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))(0);
#else
        glUseProgram(0);
#endif
    }
    void Uniform1f(const char* name, float f1)
    {
#if _WIN32
		const int val = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(m_p, name);
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(val, f1);
#else
        const int val = glGetUniformLocation(m_p,name);
		glUniform1f(val, f1);
#endif
    }
    void Uniform2f(const char* name, float f1,float f2)
    {
#if _WIN32
		const int val = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(m_p, name);
		((PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f"))(val, f1, f2);
#else
        const int val = glGetUniformLocation(m_p,name);
		glUniform2f(val, f1, f2);
#endif
    }
    void Uniform3f(const char* name, float f1,float f2,float f3)
    {
#if _WIN32
		const int val = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(m_p, name);
		((PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f"))(val, f1, f2, f3);
#else
        const int val = glGetUniformLocation(m_p,name);
		glUniform3f(val, f1, f2, f3);
#endif
    }
    
    void Draw()
    {
		glRecti(1,1,-1,-1);
    }
private:
    	GLuint m_p;
};

