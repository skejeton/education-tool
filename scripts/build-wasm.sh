mkdir -p ./bin/wasm-out
em++ -Ilib -O3 --shell-file=scripts/shell.html -s ASYNCIFY=1 -sFULL_ES2 -s ALLOW_MEMORY_GROWTH=1 -lGL -lc -lm lib/imgui/imgui.cpp lib/imgui/imgui_demo.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_tables.cpp lib/imgui/imgui_widgets.cpp src/*.cpp -o bin/wasm-out/output.html
