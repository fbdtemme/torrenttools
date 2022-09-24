package torrenttools

import (
	"strconv"
	"dagger.io/dagger"
	"dagger.io/dagger/core"
	"universe.dagger.io/docker"
	"universe.dagger.io/bash"
)

#BuildEnv_GCC12: {
	// Resulting container image
	output: _build.output
	// Build steps
	_build: docker.#Dockerfile & {
		source: dagger.#Scratch
		dockerfile: contents: """
			FROM gcc:12 AS build
			ENV DEBIAN_FRONTEND=noninteractive
			RUN apt update && apt install -y cmake ninja-build libssl-dev libtbb-dev && rm -rf /var/lib/apt/lists/*
			"""
	}
}

#BuildEnv_GCC11: {
	// Resulting container image
	output: _build.output
	// Build steps
	_build: docker.#Dockerfile & {
		source: dagger.#Scratch
		dockerfile: contents: """
			FROM gcc:11 AS build
			ENV DEBIAN_FRONTEND=noninteractive
			RUN apt update && apt install -y cmake ninja-build libssl-dev libtbb-dev && rm -rf /var/lib/apt/lists/*
			"""
	}
}

#BuildEnv_GCC10: {
	// Resulting container image
	output: _build.output
	// Build steps
	_build: docker.#Dockerfile & {
		source: dagger.#Scratch
		// Manually install cmake since gcc:10 uses buster and cmake in the repos is too old
		dockerfile: contents: """
			FROM gcc:10 AS build
			ENV DEBIAN_FRONTEND=noninteractive
			RUN apt update && apt install -y rsync ninja-build libssl-dev libtbb-dev && rm -rf /var/lib/apt/lists/*
			RUN wget https://github.com/Kitware/CMake/releases/download/v3.23.3/cmake-3.23.3-linux-x86_64.tar.gz
			RUN tar -zxf cmake-3.23.3-linux-x86_64.tar.gz
			RUN rsync -a /cmake-3.23.3-linux-x86_64/ /usr/local
			RUN rm -r /cmake-3.23.3-linux-x86_64/
			"""
	}
}
#BuildEnv_Clang: {
	version: int
	// Resulting container image
	output: _build.output
	// Build steps
	_build: docker.#Dockerfile & {
		source: dagger.#Scratch
		dockerfile: contents: """
			FROM silkeh/clang:\( version )
			ENV DEBIAN_FRONTEND=noninteractive
			RUN apt-get update && \\
					apt-get install -y cmake ninja-build rsync libssl-dev libtbb-dev && \\
					rm -rf /var/lib/apt/lists/*
			RUN wget https://github.com/Kitware/CMake/releases/download/v3.23.3/cmake-3.23.3-linux-x86_64.tar.gz
			RUN tar -zxf cmake-3.23.3-linux-x86_64.tar.gz
			RUN rsync -a /cmake-3.23.3-linux-x86_64/ /usr/local
			RUN rm -r /cmake-3.23.3-linux-x86_64/
			"""
	}
}

#Build: {
	image:  docker.#Image
	source: dagger.#FS
	jobs:   int
	output: build.output

	prepare: {
		output: _copy.output
		_copy:  docker.#Copy & {
			input:    image
			contents: source
			dest:     "/torrenttools"
		}
	}
	configure: bash.#Run & {
		input: prepare.output
		script: contents: """
			cmake -S /torrenttools -B /debug \\
			-DTORRENTTOOLS_BUILD_TESTS=ON \\
			-DTORRENTOOLS_BUILD_TESTS=ON \\
			-DOTTORRENT_MB_CRYPTO_LIB=isal
			"""
	}
	build: bash.#Run & {
		input: configure.output
		script: contents: "cmake --build /debug -j\( strconv.FormatInt(jobs, 10) ) --target torrenttools torrenttools-tests -j "
	}
}

dagger.#Plan & {
	client: {
		filesystem: ".": read: {
			// Load the '.' directory (host filesystem) into dagger's runtime
			// Specify to Dagger runtime that it is a `dagger.#FS`
			contents: dagger.#FS
		}
		env: {
			BUILD_PARALLELISM: string
		}
	}
	actions: {
		source: core.#Source & {
			path: "."
			exclude: [
				"cmake-build-*",
				"build",
				".git",
			]
		}
		build: {
			_deps: {
				jobs:    strconv.Atoi(client.env.BUILD_PARALLELISM)
				gcc10:   #BuildEnv_GCC10
				gcc11:   #BuildEnv_GCC11
				gcc12:   #BuildEnv_GCC12
				clang14: #BuildEnv_Clang & {version: 14}
				clang13: #BuildEnv_Clang & {version: 13}
				clang12: #BuildEnv_Clang & {version: 12}
			}
			"gcc:12": #Build & {
				image:  _deps.gcc12.output
				source: actions.source.output
				jobs:   _deps.jobs
			}
			"gcc:11": #Build & {
				image:  _deps.gcc11.output
				source: actions.source.output
				jobs:   _deps.jobs
			}
			"gcc:10": #Build & {
				image:  _deps.gcc10.output
				source: actions.source.output
				jobs:   _deps.jobs
			}
			"clang:14": #Build & {
				image:  _deps.clang14.output
				source: actions.source.output
				jobs:   _deps.jobs
			}
			"clang:13": #Build & {
				image:  _deps.clang13.output
				source: actions.source.output
				jobs:   _deps.jobs
			}
			"clang:12": #Build & {
				image:  _deps.clang12.output
				source: actions.source.output
				jobs:   _deps.jobs
			}
		}
	}
}
