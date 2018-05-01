#include <main>

#ifdef ___WIN
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR message, int nCmdShow) {
#else
int main(void){
#endif
	oven::Debug::enable();
	return oven::FormInit()(new CoreTest);
}
