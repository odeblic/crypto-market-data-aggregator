.PHONY: help
.PHONY: build-debug
.PHONY: build-release
.PHONY: build-docker-context
.PHONY: build-docker-images
.PHONY: run
.PHONY: run-with-docker
.PHONY: clean-debug
.PHONY: clean-release
.PHONY: clean-docker-context
.PHONY: clean-docker-images

PROGRAMS := aggregator best-bid-offer notional-volume-bands price-bands

XTERM := xterm -fa 'Monospace' -fs 12

help:
	@printf "\033[35mwelcome to the crypto market data aggregator\033[0m\n"

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

run:
	$(XTERM) -geometry 80x24+050+050 -title "aggregator"            -e "build/release/src/aggregator"            "config/aggregator.json" &
	sleep 1.0
	$(XTERM) -geometry 60x16+100+100 -title "best-bid-offer"        -e "build/release/src/best-bid-offer"        "config/best-bid-offer.json" &
	sleep 0.5
	$(XTERM) -geometry 60x16+150+150 -title "notional-volume-bands" -e "build/release/src/notional-volume-bands" "config/notional-volume-bands.json" &
	sleep 0.5
	$(XTERM) -geometry 60x16+200+200 -title "price-bands"           -e "build/release/src/price-bands"           "config/price-bands.json" &

run-with-docker:
	docker-compose -f docker/docker-compose.yml up

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
