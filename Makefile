.PHONY: help
.PHONY: build-debug
.PHONY: build-debug-asan
.PHONY: build-debug-ubsan
.PHONY: build-debug-tsan
.PHONY: build-debug-lsan
.PHONY: build-release
.PHONY: build-docker-context
.PHONY: build-docker-images
.PHONY: issue-ssl-certificate
.PHONY: run-debug
.PHONY: run-debug-asan
.PHONY: run-debug-ubsan
.PHONY: run-debug-tsan
.PHONY: run-debug-lsan
.PHONY: run-release
.PHONY: run-with-docker
.PHONY: clean-debug
.PHONY: clean-debug-asan
.PHONY: clean-debug-ubsan
.PHONY: clean-debug-tsan
.PHONY: clean-debug-lsan
.PHONY: clean-release
.PHONY: clean-docker-context
.PHONY: clean-docker-images
.PHONY: clean-ssl-certificate

PROGRAMS := aggregator best-bid-offer notional-volume-bands price-bands

XTERM := xterm -fa 'Monospace' -fs 12

help:
	@printf "\033[35mwelcome to the crypto market data aggregator\033[0m\n"

build-debug:
	cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug
	cmake --build build/debug -- -j$(nproc)

build-debug-asan:
	cmake -B build/debug-asan -DCMAKE_BUILD_TYPE=Debug -DSAN_MODE=ASAN
	cmake --build build/debug-asan -- -j$(nproc)

build-debug-ubsan:
	cmake -B build/debug-ubsan -DCMAKE_BUILD_TYPE=Debug -DSAN_MODE=UBSAN
	cmake --build build/debug-ubsan -- -j$(nproc)

build-debug-tsan:
	cmake -B build/debug-tsan -DCMAKE_BUILD_TYPE=Debug -DSAN_MODE=TSAN
	cmake --build build/debug-tsan -- -j$(nproc)

build-debug-lsan:
	cmake -B build/debug-lsan -DCMAKE_BUILD_TYPE=Debug -DSAN_MODE=LSAN
	cmake --build build/debug-lsan -- -j$(nproc)

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

issue-ssl-certificate:
	mkdir -p ssl
	openssl req -x509 -newkey rsa:2048 -keyout ssl/private-key.pem -out ssl/certificate.pem -days 365 -nodes -subj "/CN=localhost"

run-debug:
	$(XTERM) -geometry 80x24+050+050 -title "aggregator"            -e "build/debug/src/aggregator"            "config/aggregator.json" &
	sleep 1.0
	$(XTERM) -geometry 60x16+100+100 -title "best-bid-offer"        -e "build/debug/src/best-bid-offer"        "config/best-bid-offer.json" &
	sleep 0.5
	$(XTERM) -geometry 60x16+150+150 -title "notional-volume-bands" -e "build/debug/src/notional-volume-bands" "config/notional-volume-bands.json" &
	sleep 0.5
	$(XTERM) -geometry 60x16+200+200 -title "price-bands"           -e "build/debug/src/price-bands"           "config/price-bands.json" &

run-debug-asan:
	$(XTERM) -geometry 80x24+050+050 -title "aggregator"            -e "build/debug-asan/src/aggregator"            "config/aggregator.json" &
	sleep 1.0
	$(XTERM) -geometry 60x16+100+100 -title "best-bid-offer"        -e "build/debug-asan/src/best-bid-offer"        "config/best-bid-offer.json" &
	sleep 0.5
	$(XTERM) -geometry 60x16+150+150 -title "notional-volume-bands" -e "build/debug-asan/src/notional-volume-bands" "config/notional-volume-bands.json" &
	sleep 0.5
	$(XTERM) -geometry 60x16+200+200 -title "price-bands"           -e "build/debug-asan/src/price-bands"           "config/price-bands.json" &

run-debug-ubsan:
	$(XTERM) -geometry 80x24+050+050 -title "aggregator"            -e "build/debug-ubsan/src/aggregator"            "config/aggregator.json" &
	sleep 1.0
	$(XTERM) -geometry 60x16+100+100 -title "best-bid-offer"        -e "build/debug-ubsan/src/best-bid-offer"        "config/best-bid-offer.json" &
	sleep 0.5
	$(XTERM) -geometry 60x16+150+150 -title "notional-volume-bands" -e "build/debug-ubsan/src/notional-volume-bands" "config/notional-volume-bands.json" &
	sleep 0.5
	$(XTERM) -geometry 60x16+200+200 -title "price-bands"           -e "build/debug-ubsan/src/price-bands"           "config/price-bands.json" &

run-debug-tsan:
	$(XTERM) -geometry 80x24+050+050 -title "aggregator"            -e "build/debug-tsan/src/aggregator"            "config/aggregator.json" &
	sleep 1.0
	$(XTERM) -geometry 60x16+100+100 -title "best-bid-offer"        -e "build/debug-tsan/src/best-bid-offer"        "config/best-bid-offer.json" &
	sleep 0.5
	$(XTERM) -geometry 60x16+150+150 -title "notional-volume-bands" -e "build/debug-tsan/src/notional-volume-bands" "config/notional-volume-bands.json" &
	sleep 0.5
	$(XTERM) -geometry 60x16+200+200 -title "price-bands"           -e "build/debug-tsan/src/price-bands"           "config/price-bands.json" &

run-debug-lsan:
	$(XTERM) -geometry 80x24+050+050 -title "aggregator"            -e "build/debug-lsan/src/aggregator"            "config/aggregator.json" &
	sleep 1.0
	$(XTERM) -geometry 60x16+100+100 -title "best-bid-offer"        -e "build/debug-lsan/src/best-bid-offer"        "config/best-bid-offer.json" &
	sleep 0.5
	$(XTERM) -geometry 60x16+150+150 -title "notional-volume-bands" -e "build/debug-lsan/src/notional-volume-bands" "config/notional-volume-bands.json" &
	sleep 0.5
	$(XTERM) -geometry 60x16+200+200 -title "price-bands"           -e "build/debug-lsan/src/price-bands"           "config/price-bands.json" &

run-release:
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

clean-debug-asan:
	rm -rf build/debug-asan

clean-debug-ubsan:
	rm -rf build/debug-ubsan

clean-debug-tsan:
	rm -rf build/debug-tsan

clean-debug-lsan:
	rm -rf build/debug-lsan

clean-release:
	rm -rf build/release

clean-docker-context:
	rm -rf docker/build-context

clean-docker-images:
	@for PROG in $(PROGRAMS); do \
		docker image rm $$PROG ; \
	done

clean-ssl-certificate:
	rm -rf ssl
