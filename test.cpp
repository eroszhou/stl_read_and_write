#include <string>
#include "stl.hpp"

int main(int argc, char** argv){
	std::string input = argv[1];
	std::string output = argv[2];
	int type = std::atoi(argv[3]);
	STL<float> stl(input);
	if(type > 0){
		stl.write2ascii(output);
	}else{
		stl.write2binary(output);
	}
	return 0;
}
