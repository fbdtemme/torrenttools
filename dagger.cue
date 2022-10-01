package torrenttools

import (
	"strings"
	"strconv"
	"dagger.io/dagger"
	"dagger.io/dagger/core"
	"universe.dagger.io/docker"
	"universe.dagger.io/docker/cli"
	"universe.dagger.io/bash"
)

#BuildEnv_GCC: {
	version: int
	// Resulting container image
	output: _build.output

	// Build steps
	_build: docker.#Dockerfile & {
		source: dagger.#Scratch
		dockerfile: contents: """
			FROM gcc:\( version ) AS build
			ENV DEBIAN_FRONTEND=noninteractive
			ENV CXX="clang++"
			ENV CC="clang"
			RUN apt-get update && \\
					apt-get install -y cmake ninja-build rsync libssl-dev libtbb-dev curl zip unzip tar nasm pkg-config && \\
					rm -rf /var/lib/apt/lists/*
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
					apt-get install -y cmake ninja-build rsync libssl-dev libtbb-dev curl zip unzip tar nasm pkg-config && \\
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
//	output: prepare.output
	preset: string
	// socket to load container in for debugging
	socket?: dagger.#Socket

	prepare: docker.#Build & {
		steps: [
			docker.#Copy & {
				input:    image
				contents: source
				dest:     "/torrenttools"
			},
			docker.#Run & {
				workdir: "/torrenttools"
				command: {
					"name": "git"
					"args": ["clone", "https://github.com/microsoft/vcpkg"]
				}
			},
			docker.#Run & {
				workdir: "/torrenttools"
				command: {
					"name": "sh"
					"args": ["./vcpkg/bootstrap-vcpkg.sh"]
				}
			},
		]
	}
	if (socket != _|_) {
		load: cli.#Load & {
			image: prepare.output
			host:  socket
			tag:   "debug-build-env:latest"
		}
	}

	configure: bash.#Run & {
		input:   prepare.output
		workdir: "/torrenttools"
		script: contents: "cmake --preset \( preset )"
	}
	build: bash.#Run & {
		input:   configure.output
		workdir: "/torrenttools"
		script: contents: "cmake --build cmake-build-\( preset ) -j\( strconv.FormatInt(jobs, 10) )"
	}
}

#Test: {
	image:  docker.#Image
	preset: string

	test: docker.#Run & {
		input:   image
		workdir: "/torrenttools/cmake-build-\( preset )"
		command: {
			"name": "ctest"
		}
	}
}

dagger.#Plan & {
	client: network: "unix:///var/run/docker.sock": connect: dagger.#Socket
	client: platform: _
	client: filesystem: ".": read: contents: dagger.#FS
	client: env: {
		BUILD_PARALLELISM: string | *"4"
	}
	actions: {
		info: {
			_os: core.#Nop & {
				input: strings.TrimSpace(client.platform.os)
			}
			_arch: core.#Nop & {
				input: strings.TrimSpace(client.platform.arch)
			}
			os:   _os.output
			arch: _arch.output
		}
		source: core.#Source & {
			path: "."
			exclude: [
				"cue.mod",
				"*.cue",
				"cmake-build-*",
				"build",
				".git",
				"vcpkg",
			]
		}
		_deps: {
			jobs:     strconv.Atoi(client.env.BUILD_PARALLELISM)
			gcc_10:   #BuildEnv_GCC & {version:   10}
			gcc_11:   #BuildEnv_GCC & {version:   11}
			gcc_12:   #BuildEnv_GCC & {version:   12}
			clang_12: #BuildEnv_Clang & {version: 12}
			clang_13: #BuildEnv_Clang & {version: 13}
			clang_14: #BuildEnv_Clang & {version: 14}
		}

		build: {
			[Compiler=string]: [Preset=string]: #Build & {
				image:  actions._deps[Compiler].output
				source: actions.source.output
				jobs:   actions._deps.jobs
				// socket: client.network."unix:///var/run/docker.sock".connect
				preset: Preset
			}
			gcc_10: "debug-isal":     _
			gcc_11: "debug-isal":     _
			gcc_12: "debug-isal":     _
			gcc_10: "release-isal":   _
			gcc_11: "release-isal":   _
			gcc_12: "release-isal":   _
		}
	}
}
