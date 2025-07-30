#include <glad/glad.h>
//#include <core/core.h>
#include <file/image.h>
#include <run/pursuit_app.h>
#include <run/seek_runner.h>

using namespace Seek::Run;
int main() {
	const int screenWidth = 800;
	const int screenHeight = 450;

//	InitWindow(screenWidth, screenHeight, "raylib [models] example - models loading");

	Image  image = System::LoadImageSource("resource/res/gua.png");
	const char *  vsText = System::LoadFileText("resource/shader/1.model_loading.vs");
	SeekerApp* app = new SeekerApp();
	app->Run();
	return 0;
}