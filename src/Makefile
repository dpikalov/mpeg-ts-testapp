# The TSDuck development package must have been installed.

CXXFLAGS += $(shell tsconfig --cflags) -DTS_NO_VATEK
LDLIBS += $(shell tsconfig --libs)

OBJFILES = handler.o testapp.o
TARGET = testapp

$(TARGET): $(OBJFILES)
	g++ $(CXXFLAGS) -o $(TARGET) $(OBJFILES) $(LDLIBS)

clean:
	@rm -f $(OBJFILES) $(TARGET) *~
