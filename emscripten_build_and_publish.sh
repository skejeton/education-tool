python3 build.py build-wasm
cp bin/wasm-out/* ../website/dat/etc/edu-tool/
cd ../website/
./post.sh