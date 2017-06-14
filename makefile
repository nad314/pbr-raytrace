PROJ = pbr-raytrace
OUT = pbr-raytrace
BIN = bin
BUILD = build
CC = g++ -std=gnu++11 -msse -msse2 -msse3 -msse4 -mavx -mstackrealign -pthread -Wl,--no-as-needed
#WARN = -Wall -fpermissive -Wno-write-strings -Wno-unused-result -Wno-unknown-pragmas -Wno-format-security -Wno-parentheses
WARN = -w -fpermissive
DEFINE =
INCLUDE = -I"../core/core" -I"../core-tracing/core-tracing" -I"../core-forms/core-forms" -I"/usr/include" -I"./source"
FLAGS = -O2 $(INCLUDE) $(WARN) $(DEFINE)
LIBS = -L"lib" -lpthread -lfreetype -lSDL2 -lGL -lGLU -lcore -lcore-tracing -lcore-forms 

OBJ = \
	controller/controller.o \
	controller/controller.render.o \
	hdrloader/hdrloader.o \
	mainWindow/mainWindow.o \
	menuBar/menuBar.o \
	program/main.o \
	program/coreTest.o \
	randuin/randuin.o \
	render/imageRenderTask.o \
	render/progRenderTask.o \
	render/renderShader.o \
	render/pbsShader.o \
	render/renderTask.o \
	render/subRenderTask.o \
	render/volumetricShader.o \
	renderWindow/renderWindow.o \
	settings/settings.o \
	sidebar/sidebar.o \
	simdImage/simdImage.o \
	statusbar/statusbar.o \
	storage/storage.o
	

all: $(OBJ)
	$(CC) $(BIN)/* -o build/$(OUT) $(LIBS)

$(OBJ): %.o: source/%.cpp
	$(CC) -c $< -o $(BIN)/$(notdir $@) $(FLAGS)

reset:
	rm $(BUILD)/$(OUT)

clean:
	rm -rf $(BUILD)
	rm -rf $(BIN)

init:
	mkdir $(BUILD)
	mkdir $(BIN)

run:
	./$(BUILD)/pbr-raytrace
