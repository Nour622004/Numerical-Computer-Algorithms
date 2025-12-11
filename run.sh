# sudo g++ -std=c++11 -o secant_method "Secant Method Version 2.cpp" libs/Tokenizer.cpp -I.
# sudo ./secant_method

g++ -std=c++17 -O2 secant-method.cpp libs/Tokenizer.cpp -o secant_method
./secant_method