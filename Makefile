.PHONY: build-debug build-release clean-debug clean-release

build-debug:
	cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug
	cmake --build build/debug -- -j$(nproc)

build-release:
	cmake -B build/release -DCMAKE_BUILD_TYPE=Release
	cmake --build build/release -- -j$(nproc)

clean-debug:
	rm -rf build/debug

clean-release:
	rm -rf build/release
