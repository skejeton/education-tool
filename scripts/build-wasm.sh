mkdir -p ./bin/wasm-out
em++ -Ilib -O3 --shell-file=scripts/shell.html -sASYNCIFY=1 -sSTACK_SIZE=1MB -sFULL_ES2 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS="['_malloc', '_calloc', '_free', '_main']" -lGL -lc -lm lib/imgui/imgui.cpp lib/imgui/imgui_demo.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_tables.cpp lib/imgui/imgui_widgets.cpp src/*.cpp -o bin/wasm-out/output.html
