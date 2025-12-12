# ---- Toolchain ----
CXX := clang-cl

# ---- Paths ----
SRC_DIR := src
BIN_DIR := bin
OUT := $(BIN_DIR)\rasterizer.exe

# ---- Source files (ALL .cpp in src/) ----
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# ---- Libraries ----
LIBS := user32.lib gdi32.lib kernel32.lib

# ---- Default target ----
all: $(OUT)

# ---- Build rule ----
$(OUT): $(SRCS)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CXX) $(SRCS) $(LIBS) /link /SUBSYSTEM:WINDOWS /OUT:$(OUT)

# ---- Run ----
run: $(OUT)
	.\$(OUT)

# ---- Clean ----
clean:
	@if exist $(BIN_DIR) rmdir /s /q $(BIN_DIR)
