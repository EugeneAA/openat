CC=gcc
CFLAGS=-O3 -march=native -mtune=native

# Проектная директория с исходными кодами
SRCDIR=src
# Директория, где будут расположены объектные файлы
OBJDIR=obj
# Директория, где будет лежать исполняемый файл
BINDIR=bin

# Вычисляем список исходников и соответствующих объектных файлов
SOURCES=$(wildcard $(SRCDIR)/*.c)
OBJECTS=$(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
# Имя исполняемого файла
EXECUTABLE=$(BINDIR)/openat

all: $(BINDIR) $(OBJDIR) $(EXECUTABLE)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJDIR)/*.o $(EXECUTABLE)
