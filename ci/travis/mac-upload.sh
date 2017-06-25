#!/bin/bash

if [[ "$TRAVIS_SECURE_ENV_VARS" == "false" ]]; then
	exit;
fi

cd install
export BUILD_TAG="Phoenix-travis${TRAVIS_BUILD_NUMBER}-${TRAVIS_BRANCH}-${TRAVIS_COMMIT:0:6}-macOS"
echo "Build tag: ${BUILD_TAG}"
artifacts upload --target-paths /macOS/${BUILD_TAG} --working-dir `pwd` Phoenix.app

export FILE_NAME="${BUILD_TAG}.7z"
echo "Filename: ${FILE_NAME}"
7z a "$FILE_NAME" Phoenix.app
artifacts upload --target-paths /macOS --working-dir `pwd` "$FILE_NAME"
