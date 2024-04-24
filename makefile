# 编译器设置
CC=g++
CFLAGS=-I include/ -I/usr/include/python3.9 -std=c++17
LDFLAGS=-lpython3.9
SRC_DIR=src
OBJ_DIR=obj

# 定义目标可执行文件
TARGET=CSRTI

# 查找源文件
SOURCES=$(wildcard $(SRC_DIR)/*.cpp)
# 生成.o文件的名称
OBJECTS=$(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# 默认目标
all: directories $(TARGET)

# 在构建前确保目录已创建
directories:
	@mkdir -p $(OBJ_DIR)

# 链接程序
$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "Linking complete."

# 编译源文件的规则
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	@mkdir -p $(OBJ_DIR)

# 清理构建
clean:
	rm -f $(TARGET)
	rm -rf $(OBJ_DIR)/*.o

# 默认参数
ARGS ?= "Testdataset" "LPA" "3" "10" "1" "true" "true"

# 使用可选参数运行程序
run: $(TARGET)
	./$(TARGET) $(ARGS)

# 伪目标
.PHONY: all clean run directories

