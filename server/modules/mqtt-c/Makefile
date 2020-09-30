# https://stackoverflow.com/questions/2734719/how-to-compile-a-static-library-in-linux
# https://www.geeksforgeeks.org/static-vs-dynamic-libraries/
# https://www.rapidtables.com/code/linux/gcc/gcc-l.html


COMPILER=gcc

TARGET=test

OBJ_DIR=obj
HEADERS_DIR=include
LIB_DEPS_DIR=lib


LIB_DEPS_=$(shell find lib -name '*.c' | sed 's/lib\/\(.*\).c/\1/') 
LIB_DEPS=$(LIB_DEPS_:%=$(OBJ_DIR)/%.o)

CFLAGS=-lpthread

$(TARGET): $(OBJ_DIR)/$(TARGET).o $(LIB_DEPS) $(DEPS)
	$(COMPILER) -I$(HEADERS_DIR) -o $@ $^ $(CFLAGS)

# Build library object files from library source files
$(OBJ_DIR)/%.o: $(LIB_DEPS_DIR)/%.c
	$(COMPILER) -I$(HEADERS_DIR) -c $^ -o$@

$(OBJ_DIR)/%.o: %.c
	$(COMPILER) -I$(HEADERS_DIR) -c $^ -o$@

clean:
	rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/*.a $(TARGET)





# $(TARGET): $(OBJ_DIR)/$(TARGET).o $(OBJ_DIR)/librabbitmq.a
# 	$(COMPILER) -I$(HEADERS_DIR) -o $@ $(OBJ_DIR)/$(TARGET).o -L$(OBJ_DIR) -lrabbitmq

# Build static library from object files
# $(OBJ_DIR)/librabbitmq.a: $(DEPS) $(LIB_DEPS) $(OBJ_DIR)/amqp_framing.o
# 	ar rcs $@ $^

# Build object file for main target
# $(OBJ_DIR)/%.o: %.c
# 	$(COMPILER) -I$(HEADERS_DIR) -c $^ -o$@