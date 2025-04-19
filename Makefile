# Configurable variables
BUILD_DIR ?= build
NUM_CORES ?= 12

# Constants
BUILD_PATH := ${CURDIR}/${BUILD_DIR}

# Build the project using CMake
.PHONY: build
build: build_dir
# Update the CMake build output
	@cd ${BUILD_PATH}; cmake ..;
# Build the project
	@cmake --build ${BUILD_PATH}

# Create the build directory (ok if it already exists)
.PHONY: build_dir
build_dir:
	@mkdir -p ${BUILD_PATH}

# Clean out directories
.PHONY: clean 
clean:
	@rm -rf ${BUILD_PATH}

# Run all tests
.PHONY: test 
test: build 
	cd ${BUILD_PATH}; ctest 
