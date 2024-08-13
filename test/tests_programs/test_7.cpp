#include <iostream>
#include <string>
#include <string_view>
  
void print(std::string res){
    std::cout << res << std::endl;
}

std::string func(char *argv[]){
    
    std::string a = argv[1];
    std::string b = argv[2];

    return a + b;
}

int main(int argc, char *argv[]){
    
    std::string res = func(argv); 

    print(res);
}
 
