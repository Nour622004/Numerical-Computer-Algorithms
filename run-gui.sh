g++ -std=c++17 -O2 gui_secant_gtk.cpp libs/Tokenizer.cpp -o secant_gui_gtk $(pkg-config --cflags --libs gtk+-3.0)
./secant_gui_gtk