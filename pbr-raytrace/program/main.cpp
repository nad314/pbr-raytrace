#include <main>

#ifdef ___WIN
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR message, int nCmdShow) {
#else
int main(void){
#endif
	core::Debug::enable();
	return core::FormInit()(new CoreTest);
}
