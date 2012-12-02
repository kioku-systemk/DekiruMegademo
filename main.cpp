//
//  main.cpp
//
//  Created by kioku on 12/12/01.
//  Copyright (c) 2012 System K. All rights reserved.
//

#include "Core/CoreWindow.h"

// GL Headers
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include "glext.h"
#endif

#define MUSIC_TIME 29.53
#define MUSIC_FILE "Christmas.xm"


#if defined(_DEBUG) || defined(DEBUG)
static const bool fullscreenMode = false;
#else
static const bool fullscreenMode = true;
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string>

#include "Sound.h"

#include "Scene.h"

class MainWindow : public CoreWindow
{
public:
	MainWindow(int x, int y, int width, int height)
	: CoreWindow(x, y, width, height, "MEGADEMO", fullscreenMode)
	{
		m_sound = new Sound();
#ifdef __APPLE__
		m_sound->Load((std::string(GetExePath()) + std::string("/Contents/Resources/") + MUSIC_FILE).c_str()); //App Resourece path
#else
		m_sound->Load(MUSIC_FILE);
#endif
		m_sound->Play();

		m_scene1 = new Scene1();
		m_scene1->Init();
        m_scene2 = new Scene2();
		m_scene2->Init();
        m_scene3 = new Scene3();
		m_scene3->Init();
		m_scene4 = new Scene4();
		m_scene4->Init();

        Resize(GetWidth(),GetHeight());
	}
	~MainWindow()
    {
		delete m_sound;
    }
    SceneBase* getScene(double tm)
    {
        SceneBase* scene = 0;
        const double scenetime = 8.0 / 130.0 * 60.0;
        if (tm < scenetime * 2)
            scene = m_scene1;
        else if (tm < scenetime * 4)
            scene = m_scene2;
        else if (tm < scenetime * 6)
            scene = m_scene3;
        else
            scene = m_scene4;
        return scene;
    }
    void Draw()
    {
		glClearColor(0.0f,0.0f,0.0f,0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		float* fft = m_sound->GetFFT();
		const float syncVal[3] = {
			static_cast<float>(sqrt(fft[20]))*3.0f,
			static_cast<float>(sqrt(fft[50]))*3.0f,
			static_cast<float>(sqrt(fft[200]))*3.0f
		};
        double tm = m_sound->GetPosTime();
		 if (tm > MUSIC_TIME)
            exit(0);
        
        SceneBase* scene = getScene(tm);
        if (scene) {
            scene->Resize(m_w, m_h);
            scene->Draw(tm, syncVal);
        }
        SwapBuffer();
    }
     
	void KeyDown(int key)
	{
		if (key == 27) // ESC key
			exit(0);
	}
	void Idle(void)
	{
        Draw();
	}
	
	void Resize(int w, int h)
	{
		m_w = w;
		m_h = h;
		glViewport(0, 0, w, h);
        Draw();
	}
    
private:
	int m_w, m_h;
    Sound* m_sound;
    
	Scene1* m_scene1;
    Scene2* m_scene2;
	Scene3* m_scene3;
    Scene4* m_scene4;
};


#if defined(_DEBUG) || defined(__APPLE__) 
int main(int argc, char *argv[])
#else
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow )

#endif
{
    MainWindow win(32, 32, 1024, 768);
    CoreWindow::MainLoop();
    return 0;
}
