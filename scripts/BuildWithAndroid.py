import argparse
import os
import sys
import subprocess

from enum import Enum
from PySide6 import QtCore

SDK_MANAGER_TOOL_NAME = "sdkmanager.bat"
ADB_TOOL_NAME = "adb.exe"
ANDROID_CMAKE_TOOLCHAIN_NAME = "android.toolchain.cmake"
CMAKE_C_COMPILER = "clang.exe"
CMAKE_CXX_COMPILER = "clang++.exe"


class ProcessResult(Enum):
    FINISHED_WITH_SUCCESS = 0


class Android:
    jdk_location_path = None
    android_sdk_location_path = None
    sdk_tools_version = None

    def __init__(self, jdk_location_path, android_sdk_location_path):
        self.jdk_location_path = jdk_location_path
        self.android_sdk_location_path = android_sdk_location_path

        os.environ["JAVA_HOME"] = self.jdk_location_path

        validation_infos = []

        self._build_tools_installed_validation()

        android_sdk_path_exists_and_writable_validation_result = self._android_sdk_path_exists_and_writable_validation()
        validation_infos.append(
            ["Android SDK path exists and is writable.", android_sdk_path_exists_and_writable_validation_result])

        sdk_tools_installed_validation_result = self._sdk_tools_installed_validation()
        validation_infos.append(["Android SDK Command-line Tools installed.", sdk_tools_installed_validation_result])

        sdk_manager_successful_validation_result = self._sdk_manager_successful_validation()
        validation_infos.append(["Android SDK Command-line Tools run.", sdk_manager_successful_validation_result])

        build_tools_installed_validation_result = self._build_tools_installed_validation()
        validation_infos.append(["Android SDK Build-Tools installed.", build_tools_installed_validation_result])

        platform_tools_installed_validation_result = self._platform_tools_installed_validation()
        validation_infos.append(["Android SDK Platform-Tools installed.", platform_tools_installed_validation_result])

        for validation_info in validation_infos:
            print(validation_info[0], validation_info[1])
        print('\n')

        sdk_validation_result = \
            android_sdk_path_exists_and_writable_validation_result & \
            sdk_tools_installed_validation_result & \
            sdk_manager_successful_validation_result & \
            build_tools_installed_validation_result & \
            platform_tools_installed_validation_result

        if not sdk_validation_result:
            sys.exit()

    def _jdk_path_exists_and_writable_validation(self):
        return os.path.exists(self.jdk_location_path) and os.access(self.jdk_location_path, os.W_OK)

    def _android_sdk_path_exists_and_writable_validation(self):
        return os.path.exists(self.android_sdk_location_path) and os.access(self.android_sdk_location_path, os.W_OK)

    def _sdk_tools_installed_validation(self):
        source_group = os.path.join(self.android_sdk_location_path, "cmdline-tools\\latest\\source.properties")
        with open(source_group) as sdk_props_file:
            for line in sdk_props_file.readlines():
                tokens = line.split('=')
                tokens = [token.strip() for token in tokens]

                if "Pkg.Revision" in tokens:
                    self.sdk_tools_version = QtCore.QVersionNumber.fromString(tokens[1])
        return not self.sdk_tools_version.isNull()

    def _sdk_manager_successful_validation(self):
        sdk_manager_path = os.path.join(self.android_sdk_location_path, "cmdline-tools\\latest\\bin\\",
                                        SDK_MANAGER_TOOL_NAME)
        sdk_manager_arguments = ["--list", "--verbose"]
        if not os.path.exists(sdk_manager_path):
            return False

        process = QtCore.QProcess()
        process.setProgram(sdk_manager_path)
        process.setArguments(sdk_manager_arguments)
        process.start()
        process.waitForFinished()
        return process.exitCode() == ProcessResult.FINISHED_WITH_SUCCESS.value

    def _platform_tools_installed_validation(self):
        adb_tool_path = os.path.join(self.android_sdk_location_path, "platform-tools", ADB_TOOL_NAME)
        return os.path.exists(adb_tool_path)

    def _build_tools_installed_validation(self):
        max_version = QtCore.QVersionNumber()
        build_tools_path = os.path.join(self.android_sdk_location_path, "build-tools")
        for file in os.listdir(build_tools_path):
            max_version = max(max_version, QtCore.QVersionNumber.fromString(file))
        return not max_version.isNull()

    def android_cmake_toolchain_file_path(self):
        return os.path.join(self._ndk_home_path(), "build/cmake", ANDROID_CMAKE_TOOLCHAIN_NAME)

    def _ndk_home_path(self):
        max_version = QtCore.QVersionNumber()
        ndk_home = os.path.join(self.android_sdk_location_path, "ndk")
        for file in os.listdir(ndk_home):
            max_version = max(max_version, QtCore.QVersionNumber.fromString(file))
        ndk_home = os.path.join(ndk_home, max_version.toString())
        return ndk_home

    def android_cmake_c_compiler(self):
        return os.path.join(self._ndk_home_path(), "toolchains/llvm/prebuilt/windows-x86_64/bin", CMAKE_C_COMPILER)

    def android_cmake_cxx_compiler(self):
        return os.path.join(self._ndk_home_path(), "toolchains/llvm/prebuilt/windows-x86_64/bin", CMAKE_CXX_COMPILER)


class CMakeBuilder:
    cmake_location_path = None

    def __init__(self, cmake_location_path, android: Android):
        self.cmake_location_path = cmake_location_path
        self.__android = android
        self.__android_abi = None

    def set_android_abi(self, android_abi):
        self.__android_abi = android_abi

    def run(self):
        # cmake_run_arguments = [
        #     self.cmake_location_path,
        #     "-S", os.path.abspath(os.path.dirname(os.getcwd())),
        #     "-B", "C:\\Users\\zjczh\\Desktop\\BUILD\\jzcode-qsdf\\cmake_vcpkg_android_pack",
        #     "-DCMAKE_GENERATOR:STRING=Ninja",
        #     "-DCMAKE_BUILD_TYPE:STRING=Release",
        #     "-DCMAKE_MAKE_PROGRAM:FILEPATH=D:/Qt/Tools/Ninja/ninja.exe",
        #     "-DQt6_DIR:PATH=D:/Qt/6.4.2/android_arm64_v8a/lib/cmake/Qt6",
        #     "-DNpm_ROOT_PATH:PATH=D:/Program Files/node-v18.13.0-win-x64",
        #     "-DQSDF_BUILD_APP_BUNDLE:BOOL=OFF",
        #     "-DQSDF_DOMAIN_NAME:STRING=okrdp",
        #     "-DCMAKE_C_COMPILER:FILEPATH=%s" % self.__android.android_cmake_c_compiler(),
        #     "-DCMAKE_CXX_COMPILER:FILEPATH=%s" % self.__android.android_cmake_cxx_compiler(),
        #     "-DANDROID_ABI=%s" % self.__android_abi,
        #     "-DCMAKE_TOOLCHAIN_FILE=D:/Program Files/.clion-vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake",
        #     "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=%s" % self.__android.android_cmake_toolchain_file_path()
        # ]
        aaa = [
            self.cmake_location_path,
            "-S", os.path.abspath(os.path.dirname(os.getcwd())),
            "-B", "C:\\Users\\zjczh\\Desktop\\BUILD\\jzcode-qsdf\\cmake_vcpkg_android_pack",
            "-DCMAKE_GENERATOR:STRING=Ninja",
            "-DCMAKE_BUILD_TYPE:STRING=Debug",
            "-DCMAKE_MAKE_PROGRAM:FILEPATH=D:/Qt/Tools/Ninja/ninja.exe",
            "-DQT_QMAKE_EXECUTABLE:FILEPATH=D:/Qt/6.4.2/android_arm64_v8a/bin/qmake.bat",
            "-DCMAKE_PREFIX_PATH:PATH=D:/Qt/6.4.2/android_arm64_v8a",
            "-DCMAKE_C_COMPILER:FILEPATH=C:/Users/zjczh/AppData/Local/Android/Sdk/ndk/23.1.7779620/toolchains/llvm/prebuilt/windows-x86_64/bin/clang.exe",
            "-DCMAKE_CXX_COMPILER:FILEPATH=C:/Users/zjczh/AppData/Local/Android/Sdk/ndk/23.1.7779620/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe",
            "-DANDROID_NATIVE_API_LEVEL:STRING=23",
            "-DANDROID_NDK:PATH=C:/Users/zjczh/AppData/Local/Android/Sdk/ndk/23.1.7779620",
            "-DCMAKE_TOOLCHAIN_FILE=D:/Program Files/.clion-vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake",
            "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE:FILEPATH=C:/Users/zjczh/AppData/Local/Android/Sdk/ndk/23.1.7779620/build/cmake/android.toolchain.cmake",
            "-DANDROID_USE_LEGACY_TOOLCHAIN_FILE:BOOL=OFF",
            "-DANDROID_ABI:STRING=arm64-v8a",
            "-DANDROID_STL:STRING=c++_shared",
            "-DCMAKE_FIND_ROOT_PATH:PATH=D:/Qt/6.4.2/android_arm64_v8a",
            "-DQT_NO_GLOBAL_APK_TARGET_PART_OF_ALL:BOOL=ON",
            "-DQT_HOST_PATH:PATH=D:/Qt/6.4.2/mingw_64",
            "-DANDROID_SDK_ROOT:PATH=C:/Users/zjczh/AppData/Local/Android/Sdk",
            "-DCMAKE_CXX_FLAGS_INIT:STRING=-DQT_QML_DEBUG",
            "-DNpm_ROOT_PATH:PATH=D:/Program Files/node-v18.13.0-win-x64"
        ]
        subprocess.call(aaa)


parser = argparse.ArgumentParser(description="Build with android")
parser.add_argument("--jdk_location_path", help="JDK location", required=True)
parser.add_argument("--android_sdk_location_path", help="Android SDK location", required=True)
parser.add_argument("--cmake_location_path", help="CMake location", required=True)
parser.add_argument("--android_abi", help="Android ABI", choices=["arm64-v8a", "armeabi-v7a", "x86_64", "x86"],
                    default="arm64-v8a")
parser.add_argument("--domain_name", help="Build ep domain name", required=True)
args = parser.parse_args()


def main():
    print("Input:")
    print("JDK location:", args.jdk_location_path)
    print("Android SDK location:", args.android_sdk_location_path)
    print("CMake location:", args.cmake_location_path)
    print("\nCheck:")

    android = Android(jdk_location_path=args.jdk_location_path,
                      android_sdk_location_path=args.android_sdk_location_path)

    cmake_builder = CMakeBuilder(cmake_location_path=args.cmake_location_path, android=android)
    cmake_builder.set_android_abi(args.android_abi)
    cmake_builder.run()


if __name__ == '__main__':
    main()
