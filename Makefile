.PHONY: build-debug build-release clean-debug clean-release

PROGRAMS := aggregator best-bid-offer notional-volume-bands price-bands

build-debug:
	cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug
	cmake --build build/debug -- -j$(nproc)

build-release:
	cmake -B build/release -DCMAKE_BUILD_TYPE=Release
	cmake --build build/release -- -j$(nproc)

build-docker-context:
	mkdir -p docker/build-context
	@for PROG in $(PROGRAMS); do \
		cp build/release/src/$$PROG docker/build-context/ ; \
		cp config/$$PROG.json docker/build-context/ ; \
	done
	@for PROG in $(PROGRAMS); do \
		if [ $$PROG != aggregator ]; then \
			sed -i 's/127.0.0.1/aggregator/' docker/build-context/$$PROG.json ; \
		fi \
	done

build-docker-images:
	@for PROG in $(PROGRAMS); do \
		docker build -f docker/Dockerfile --build-arg PROGRAM_NAME=$$PROG -t $$PROG docker/build-context ; \
	done

clean-debug:
	rm -rf build/debug

clean-release:
	rm -rf build/release

clean-docker-context:
	rm -rf docker/build-context

clean-docker-images:
	@for PROG in $(PROGRAMS); do \
		docker image rm $$PROG ; \
	done
