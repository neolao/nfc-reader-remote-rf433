
# Defines the RPI variable which is needed by rc-switch/RCSwitch.h
CXXFLAGS=-DRPI

all: receiver

receiver: ./rc-switch/RCSwitch.o receiver.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi -lwiringPiDev -lcrypt
	

clean:
	$(RM) ./rc-switch/*.o *.o receiver

