#include <main>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR message, int nCmdShow) {
//int main(void){
	return core::FormInit()(new CoreTest);
}
