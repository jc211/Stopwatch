#include <iostream>
#include <string>
#include <thread>
#include "gui/window.h"
#include "server.h"
#include "application.h"



int main(int argc, char* argv[])


{
    InitializeNetworking();

    Application app;
	Window win("Stopwatch Monitor");

	while (!win.ShouldClose()) {
        win.BeginFrame();
        app.Draw();
        win.EndFrame(); 
	}

	CleanUpNetworking();
}
