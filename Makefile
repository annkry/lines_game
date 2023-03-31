default: main

%: %.cpp
	g++ -I. $< -o $@ Window.cpp  -lepoxy -lGL -lglfw 
clean:
	rm a.out *.o *~ main
