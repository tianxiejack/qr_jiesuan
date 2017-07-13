/*
  Copyright (C) 1997-2016 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/

#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <glut.h>
#include "process021.hpp"
#include "dx_main.h"
#include "Gpio026.h"
#include  "Tasklist.h"

using namespace std;
using namespace cv;


int main(int argc, char **argv)
{

	CProcess021 proc;

	App_dxmain();

	proc.creat();
	

	proc.init();

	proc.run();
	mainCtrl();
	//Hard_Init();
	
	glutMainLoop();

	proc.destroy();

    return 0;
}


