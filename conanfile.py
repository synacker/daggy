'''
MIT License

Copyright (c) 2020 Mikhail Milovidov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
'''

from conans import ConanFile, CMake
from git_version import GitVersion

class DaggyConan(ConanFile):
    name = "daggy"
    license = "MIT"
    url = "https://daggy.dev"
    description = "Data Aggregation Utilty - aggregation and stream data via remote and local processes."
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "ssh2_support": [True, False],
        "yaml_support": [True, False],
        "console": [True, False],
        "shared": [True, False],
        "fPIC": [True, False],
        "circleci": [True, False]
    }
    default_options = {
        "ssh2_support": True,
        "yaml_support": True,
        "console": True,
        "shared": True,
        "fPIC": True,
        "circleci": False
    }
    generators = "cmake", "cmake_paths", "cmake_find_package"
    exports = ["CMakeLists.txt", "git_version.py", "cmake/*", "src/*", "LICENSE", "README.md"]

    _cmake = None

    def set_version(self):
        self.version = GitVersion().full_version()

    def configure(self):
        self.options.fPIC = self.options.shared
        self.options["qt"].shared = self.options.shared
        self.options["openssl"].shared = self.options.shared
        
        if self.options.ssh2_support:
            self.options["libssh2"].shared = self.options.shared
        if self.options.yaml_support:
            self.options["yaml-cpp"].shared = self.options.shared

    def requirements(self):
        self.requires("qt/6.2.2")
        self.requires("kainjow-mustache/4.1")

        if self.options.yaml_support:
            self.requires("yaml-cpp/0.7.0")

        if self.options.ssh2_support:
            self.requires("libssh2/1.10.0")

    def _libdir(self):
        result = "lib"
        if self.settings.arch == "x86_64" and self.settings.os == "Linux":
            result = "lib64"
        return result

    def _configure(self):
        if self._cmake:
            return self._cmake
        self._cmake = CMake(self)
        if self.settings.build_os == "Windows":
            if self.options.circleci:
                self._cmake.definitions["CMAKE_SYSTEM_VERSION"] = "10.1.18362.1"
        
        self._cmake.definitions["SSH2_SUPPORT"] = self.options.ssh2_support
        self._cmake.definitions["YAML_SUPPORT"] = self.options.yaml_support
        self._cmake.definitions["console"] = self.options.console
        self._cmake.definitions["PACKAGE_DEPS"] = True
        self._cmake.definitions["VERSION"] = self.version
        self._cmake.definitions["CMAKE_INSTALL_LIBDIR"] = self._libdir()
        if self.options.shared:
            self._cmake.definitions["CMAKE_C_VISIBILITY_PRESET"] = "hidden"
            self._cmake.definitions["CMAKE_CXX_VISIBILITY_PRESET"] = "hidden"
            self._cmake.definitions["CMAKE_VISIBILITY_INLINES_HIDDEN"] = 1
        self._cmake.configure()
        return self._cmake

    def build(self):
        cmake = self._configure()
        cmake.build()

    def package(self):
        cmake = self._configure()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["DaggyCore"]
        self.cpp_info.libdirs = [self._libdir()]

    def imports(self):
        if self.settings.os == "Windows":
            self.copy("*.dll", src="@bindirs", dst="bin")
            self.copy("*.dll", src="@libdirs", dst="bin")
        elif self.settings.os == "Linux":
            self.copy("*.so.*", src="@libdirs", dst="{}/{}".format(self._libdir(), self.name))
        else:
            self.copy("*.dylib", src="@libdirs", dst="{}/{}".format(self._libdir(), self.name))
