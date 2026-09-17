EXCHANGES := binance coinbase okx # bitmex kraken hyperliquid bybit internal
PROGRAMS := aggregator best-bid-offer notional-volume-bands price-bands statistics
BUILDS := release debug debug-asan debug-ubsan debug-tsan debug-lsan
XTERM := xterm -fa 'Monospace' -fs 12

.PHONY: help
help:
	@printf "Helper Makefile for \033[35mCrypto Market Data Aggregator\033[0m project.\n\n"
	@printf "These are targets for common tasks:\n\n"
	@printf "\033[32m  all\033[0m         invoke \033[32mconfigure\033[0m, \033[32mbuild\033[0m, \033[32mtest\033[0m\n"
	@printf "\033[32m  configure\033[0m   configure cmake builds\n"
	@printf "\033[32m  build\033[0m       build all artifacts for all builds\n"
	@printf "\033[32m  ssl\033[0m         generate SSL private key and certificate\n"
	@printf "\033[32m  test\033[0m        run all unit tests\n"
	@printf "\033[32m  docker\033[0m      build the docker stack and run it\n"
	@printf "\033[32m  clean\033[0m       cleanup all build artifacts\n"
	@printf "\033[32m  help\033[0m        display this message\n"
	@printf "\n"
	@printf "These are targets for build-specific tasks:\n\n"
	@printf "\033[32m  build-\033[33mx\033[0m     build all artifacts for build \033[33mx\033[0m\n"
	@printf "\033[32m  test-\033[33mx\033[0m      run all unit tests for build \033[33mx\033[0m\n"
	@printf "\033[32m  run-\033[33mx\033[0m       run the whole stack for build \033[33mx\033[0m\n"
	@printf "\033[32m  clean-\033[33mx\033[0m     cleanup all artifacts for build \033[33mx\033[0m\n"
	@printf "\n"
	@printf "Available builds:\n"
	@for BUILD in $(BUILDS); do \
		printf "\033[33m  $$BUILD\033[0m\n" ; \
	done

.PHONY: all
all: configure build test

.PHONY: configure
configure:
	@printf "\033[34mconfigure all builds\033[0m\n"
	cmake -B build/debug       -DCMAKE_BUILD_TYPE=Debug
	cmake -B build/debug-asan  -DCMAKE_BUILD_TYPE=Debug -DSAN_MODE=ASAN
	cmake -B build/debug-ubsan -DCMAKE_BUILD_TYPE=Debug -DSAN_MODE=UBSAN
	cmake -B build/debug-tsan  -DCMAKE_BUILD_TYPE=Debug -DSAN_MODE=TSAN
	cmake -B build/debug-lsan  -DCMAKE_BUILD_TYPE=Debug -DSAN_MODE=LSAN
	cmake -B build/release     -DCMAKE_BUILD_TYPE=Release

.PHONY: build
build: $(addprefix build-,$(BUILDS))

.PHONY: $(addprefix build-,$(BUILDS))
$(addprefix build-,$(BUILDS)): build-%:
	@printf "\033[34mbuild all artifacts for build $*\033[0m\n"
	cmake --build "build/$*" -- -j$(nproc)

.PHONY: docker
docker:
	@printf "\033[34msetup the entire docker stack and run it\033[0m\n"
	mkdir -p docker/build-context
	@for PROGRAM in $(PROGRAMS); do \
		cp build/release/src/$$PROGRAM docker/build-context/ ; \
		cp config/$$PROGRAM.json docker/build-context/ ; \
	done
	@for PROGRAM in $(PROGRAMS); do \
		if [ $$PROGRAM != aggregator ]; then \
			sed -i 's/127.0.0.1/aggregator/' docker/build-context/$$PROGRAM.json ; \
		fi \
	done
	@for PROGRAM in $(PROGRAMS); do \
		docker build -f docker/Dockerfile --build-arg PROGRAM_NAME=$$PROGRAM -t $$PROGRAM docker/build-context ; \
	done
	docker-compose -f docker/docker-compose.yml up

.PHONY: ssl
ssl:
	@printf "\033[34mgenerate SSL private key and certificate\033[0m\n"
	cmake -E make_directory ssl
	openssl req -x509 -newkey rsa:2048 -keyout ssl/private-key.pem -out ssl/certificate.pem -days 365 -nodes -subj "/CN=localhost"
	@for BUILD in $(BUILDS); do \
		cmake -E make_directory build/$$BUILD ; \
		cp ssl/private-key.pem build/$$BUILD ; \
		cp ssl/certificate.pem build/$$BUILD ; \
	done

.PHONY: test
test: $(addprefix test-,$(BUILDS))

.PHONY: $(addprefix test-,$(BUILDS))
$(addprefix test-,$(BUILDS)): test-%:
	@printf "\033[34mrun all unit tests for build $*\033[0m\n"
	@for PROGRAM in $(PROGRAMS); do \
		build/$*/test/$$PROGRAM-tests ; \
	done

.PHONY: $(addprefix run-,$(BUILDS))
$(addprefix run-,$(BUILDS)): run-%:
	@printf "\033[34mrun the whole stack for build $*\033[0m\n"
	PROGRAM="aggregator";            $(XTERM) -geometry 80x24+050+050 -title "$$PROGRAM" -e "build/$*/src/$$PROGRAM" --config "config/$$PROGRAM.json" $(EXCHANGES) &
	sleep 1.0
	PROGRAM="best-bid-offer";        $(XTERM) -geometry 60x16+100+100 -title "$$PROGRAM" -e "build/$*/src/$$PROGRAM" --config "config/$$PROGRAM.json" &
	sleep 0.5
	PROGRAM="notional-volume-bands"; $(XTERM) -geometry 60x16+150+150 -title "$$PROGRAM" -e "build/$*/src/$$PROGRAM" --config "config/$$PROGRAM.json" &
	sleep 0.5
	PROGRAM="price-bands";           $(XTERM) -geometry 60x16+200+200 -title "$$PROGRAM" -e "build/$*/src/$$PROGRAM" --config "config/$$PROGRAM.json" &
	sleep 0.5
	PROGRAM="statistics";            $(XTERM) -geometry 60x16+250+250 -title "$$PROGRAM" -e "build/$*/src/$$PROGRAM" --config "config/$$PROGRAM.json" &

.PHONY: clean
clean: $(addprefix clean-,$(BUILDS))
	@printf "\033[34mcleanup all generated files\033[0m\n"
	cmake -E rm -rf build
	cmake -E rm -rf docker/build-context
	cmake -E rm -rf ssl

.PHONY: $(addprefix clean-,$(BUILDS))
$(addprefix clean-,$(BUILDS)): clean-%:
	@printf "\033[34mcleanup all build artifacts for build $*\033[0m\n"
	cmake -E rm -rf build/$*
