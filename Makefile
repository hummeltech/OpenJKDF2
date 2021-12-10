CC := i686-w64-mingw32-gcc
TARGET := df2_reimpl_kvm.dll
TARGET_WIN := df2_reimpl.dll
OBJ := build

include sources.mk

CFLAGS := -O0 -g -I$(ROOT_DIR)/$(SRC) -I$(ROOT_DIR)/$(SRC)/external/libsmacker -DQOL_IMPROVEMENTS -DWIN32 -DWIN32_BLOBS -DARCH_X86 -DTARGET_HAS_DPLAY -Wuninitialized -fno-trapping-math

OPENJKDF2_NO_ASAN := 1

all: $(TARGET) $(TARGET_WIN)
include common.mk

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ) initial
	@mkdir -p $(dir $@)
	$(CC) -c -g -o $@ $< $(CFLAGS)

$(TARGET_WIN): $(OBJECTS)
	$(CC) -o $@ -g -shared $^ -Wl,--subsystem,windows -Wl,-Map=output.map -static-libgcc -fno-trapping-math

$(TARGET): $(OBJECTS)
	$(CC) -o $@ -g -shared $^ -Wl,--subsystem,windows -Wl,-Map=output.map -Wl,-e_hook_init -nostartfiles -static -static-libgcc -static-libstdc++ -fno-trapping-math
