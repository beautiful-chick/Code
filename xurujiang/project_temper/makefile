#定义编译器和编译选项  
CC = gcc  
CFLAGS = -Wall -I./inc -I/home/iot24/xurujiang/project_init/install/include  
  
# 定义链接器选项和库  
LDFLAGS = -L/home/iot24/xurujiang/project_init/install/lib -lsqlite3  
  
# 定义源文件和目标文件  
SRCS = $(wildcard src/*.c)  
OBJS = $(SRCS:src/%.c=obj/%.o)  
TARGET = data_temper  
  
# 创建 obj 目录，如果它不存在  
$(shell mkdir -p obj)  
  
# 编译规则  
obj/%.o: src/%.c  
	$(CC) $(CFLAGS) -c $< -o $@  
  
# 链接规则  
$(TARGET): $(OBJS)  
	$(CC) $(OBJS) $(LDFLAGS) -o $@  
  
# 默认目标  
all: $(TARGET)  
  
# 清理目标  
clean:  
	rm -f obj/*.o $(TARGET)
