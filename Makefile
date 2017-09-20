PROJ = pbr-raytrace
OUT = pbr-raytrace
BIN = .bin
BUILD = build
CC = g++ -std=gnu++11 -msse -msse2 -msse3 -msse4 -mavx -mstackrealign -pthread -Wl,--no-as-needed
#WARN = -Wall -fpermissive -Wno-write-strings -Wno-unused-result -Wno-unknown-pragmas -Wno-format-security -Wno-parentheses
WARN = -w -fpermissive
DEFINE =
INCLUDE = -I"./source" -I"../core/source" -I"../core-tracing/source" -I"../core-forms/source" -I"/usr/include"
FLAGS = -O2 $(INCLUDE) $(WARN) $(DEFINE)
LIBS = -L"lib" -lpthread -lfreetype -lSDL2 -lGL -lGLU -lcore -lcore-tracing -lcore-forms 

DEPDIR = .d
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

SRC = \
	controller/controller.cpp \
	controller/controller.render.cpp \
	hdrloader/hdrloader.cpp \
	mainWindow/mainWindow.cpp \
	menuBar/menuBar.cpp \
	program/main.cpp \
	program/coreTest.cpp \
	render/imageRenderTask.cpp \
	render/progRenderTask.cpp \
	render/renderShader.cpp \
	render/pbsShader.cpp \
	render/renderTask.cpp \
	render/subRenderTask.cpp \
	render/volumetricShader.cpp \
	renderWindow/renderWindow.cpp \
	settings/settings.cpp \
	sidebar/sidebar.cpp \
	simdImage/simdImage.cpp \
	statusbar/statusbar.cpp \
	storage/storage.cpp

OBJ = $(addprefix $(BIN)/, $(SRC:.cpp=.o))
DIRS = $(sort $(addprefix $(BIN)/, $(dir $(SRC))))	$(sort $(addprefix $(DEPDIR)/, $(dir $(SRC))))

all: $(BUILD)/$(OUT)

$(BUILD)/$(OUT): $(OBJ) | $(BUILD)
	$(CC) $(OBJ) -o $(BUILD)/$(OUT) $(LIBS)

$(BIN)/%.o: source/%.cpp $(DEPDIR)/%.d | $(DIRS)
	$(CC) -c $< -o $@ $(FLAGS) $(DEPFLAGS)
	$(POSTCOMPILE)

$(DIRS): | $(BIN) $(DEPDIR)
	mkdir $@

$(BIN):
	mkdir $(BIN)

$(BUILD):
	mkdir $(BUILD)

$(DEPDIR):
	mkdir $(DEPDIR)

clean:
	rm -rf $(BUILD)
	rm -rf $(BIN)
	rm -rf $(DEPDIR)

run:
	./$(BUILD)/pbr-raytrace

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRC))))

