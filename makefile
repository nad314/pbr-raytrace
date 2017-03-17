CC = g++ -std=gnu++11 -msse -msse2 -msse3 -msse4 -mavx -mstackrealign -pthread -Wl,--no-as-needed
#WARN = -Wall -fpermissive -Wno-write-strings -Wno-unused-result -Wno-unknown-pragmas -Wno-format-security -Wno-parentheses
WARN = -w -fpermissive
BIN = bin
BUILD = build
DEFINE =
INCLUDE = -I"../core/core" -I"../core-tracing/core-tracing" -I"../core-forms/core-forms" -I"/usr/include" -I"./pbr-raytrace"
FLAGS = -O3 -g $(INCLUDE) $(WARN) $(DEFINE)
OBJ = $(BIN)/main.o $(BIN)/coreTest.o $(BIN)/controller.o $(BIN)/controller.render.o $(BIN)/hdrloader.o $(BIN)/mainWindow.o $(BIN)/menuBar.o $(BIN)/imageRenderTask.o $(BIN)/msRenderTask.o $(BIN)/progRenderTask.o $(BIN)/renderShader.o $(BIN)/renderTask.o $(BIN)/subRenderTask.o $(BIN)/volumetricShader.o $(BIN)/renderWindow.o $(BIN)/settings.o $(BIN)/sidebar.o $(BIN)/simdImage.o $(BIN)/statusbar.o $(BIN)/storage.o $(BIN)/randuin.o
LIBS = -L"lib" -lpthread -lfreetype -lSDL2 -lGL -lGLU -lcore -lcore-tracing -lcore-forms 

all:
	$(CC) -c pbr-raytrace/program/main.cpp -o $(BIN)/main.o $(FLAGS)
	$(CC) -c pbr-raytrace/program/coreTest.cpp -o $(BIN)/coreTest.o $(FLAGS)
	$(CC) -c pbr-raytrace/controller/controller.cpp -o $(BIN)/controller.o $(FLAGS)
	$(CC) -c pbr-raytrace/controller/controller.render.cpp -o $(BIN)/controller.render.o $(FLAGS)
	$(CC) -c pbr-raytrace/hdrloader/hdrloader.cpp -o $(BIN)/hdrloader.o $(FLAGS)
	$(CC) -c pbr-raytrace/mainWindow/mainWindow.cpp -o $(BIN)/mainWindow.o $(FLAGS)
	$(CC) -c pbr-raytrace/menuBar/menuBar.cpp -o $(BIN)/menuBar.o $(FLAGS)
	$(CC) -c pbr-raytrace/render/imageRenderTask.cpp -o $(BIN)/imageRenderTask.o $(FLAGS)
	$(CC) -c pbr-raytrace/render/msRenderTask.cpp -o $(BIN)/msRenderTask.o $(FLAGS)
	$(CC) -c pbr-raytrace/render/progRenderTask.cpp -o $(BIN)/progRenderTask.o $(FLAGS)
	$(CC) -c pbr-raytrace/render/renderShader.cpp -o $(BIN)/renderShader.o $(FLAGS)
	$(CC) -c pbr-raytrace/render/renderTask.cpp -o $(BIN)/renderTask.o $(FLAGS)
	$(CC) -c pbr-raytrace/render/subRenderTask.cpp -o $(BIN)/subRenderTask.o $(FLAGS)
	$(CC) -c pbr-raytrace/render/volumetricShader.cpp -o $(BIN)/volumetricShader.o $(FLAGS)
	$(CC) -c pbr-raytrace/renderWindow/renderWindow.cpp -o $(BIN)/renderWindow.o $(FLAGS)
	$(CC) -c pbr-raytrace/settings/settings.cpp -o $(BIN)/settings.o $(FLAGS)
	$(CC) -c pbr-raytrace/sidebar/sidebar.cpp -o $(BIN)/sidebar.o $(FLAGS)
	$(CC) -c pbr-raytrace/simdImage/simdImage.cpp -o $(BIN)/simdImage.o $(FLAGS)
	$(CC) -c pbr-raytrace/statusbar/statusbar.cpp -o $(BIN)/statusbar.o $(FLAGS)
	$(CC) -c pbr-raytrace/storage/storage.cpp -o $(BIN)/storage.o $(FLAGS)
	$(CC) -c pbr-raytrace/randuin/randuin.cpp -o $(BIN)/randuin.o $(FLAGS)
	$(CC) $(OBJ) -o build/pbr-raytrace $(LIBS)

clean:
	rm -rf $(BUILD)
	rm -rf $(BIN)

init:
	mkdir $(BUILD)
	mkdir $(BIN)

run:
	./$(BUILD)/pbr-raytrace

fix: 
	$(CC) $(OBJ) -o build/pbr-raytrace $(LIBS)
