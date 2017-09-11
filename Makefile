CC = gcc
INCLUDEPATH = ../include/
SRC = pthreadPool.c task.c
OBJ = libpthreadPool.so

$(OBJ):pthreadPool.o task.o
	@$(CC) --share -fPIC $^ -o $@
	@strip $@
	@echo "compiling $@ "
%.o:%.c
	@$(CC) -I $(INCLUDEPATH) -c $< -fPIC -o $@
	@echo "compiling $@ "
clean:
	@rm -rf *.o
	@echo "delete all obj files."
