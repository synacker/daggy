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
    version = GitVersion().full_version()
    license = "MIT"
    url = "https://daggy.dev"
    description = "Data Aggregation Utilty - aggregation and stream data via remote and local processes."
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "ssh2_support": [True, False],
        "yaml_support": [True, False],
        "daggy_console": [True, False],
        "daggy_core_static": [True, False],
        "package_deps": [True, False]
    }
    default_options = {
        "ssh2_support": True,
        "yaml_support": True,
        "daggy_console": True,
        "daggy_core_static": False,
        "package_deps": True
    }
    generators = "cmake"
    exports = ["CMakeLists.txt", "git_version.py", "cmake/*", "src/*"]
    export_sources = ["src/*"]

    def requirements(self):
        self.requires("openssl/1.1.1g")
        self.requires("qt/[>=5.14.1]@bincrafters/stable")
        self.requires("kainjow-mustache/[>=3.2.1]@bincrafters/stable")

        if self.options.yaml_support:
            self.requires("yaml-cpp/[>=0.6.3]")

        if self.options.ssh2_support:
            self.requires("libssh2/[>=1.9.0]")

    def configure(self):
        self.options["qt"].shared = True
        self.options["qt"].commercial = False


        self.options["qt"].opengl = "no"
        self.options["qt"].openssl = False
        self.options["qt"].with_vulkan = False
        self.options["qt"].with_pcre2 = False
        self.options["qt"].with_glib = False
        self.options["qt"].with_freetype = False
        self.options["qt"].with_fontconfig = False
        self.options["qt"].with_harfbuzz = False
        self.options["qt"].with_libjpeg = False
        self.options["qt"].with_libpng = False
        self.options["qt"].with_sqlite3 = False
        self.options["qt"].with_mysql = False
        self.options["qt"].with_pq = False
        self.options["qt"].with_odbc = False
        self.options["qt"].with_sdl2 = False
        self.options["qt"].with_libalsa = False
        self.options["qt"].with_openal = False
        self.options["qt"].with_zstd = False
        self.options["qt"].with_pq = False
        self.options["qt"].GUI = False
        self.options["qt"].widgets = False

        self.options["yaml-cpp"].shared = False
        self.options["libssh2"].shared = False

    def _libdir(self):
        result = "lib"
        if self.settings.arch == "x86_64":
            result = "lib64"
        return result

    def _configure(self):
        cmake = CMake(self)
        cmake.definitions["SSH2_SUPPORT"] = self.options.ssh2_support
        cmake.definitions["YAML_SUPPORT"] = self.options.yaml_support
        cmake.definitions["DAGGY_CONSOLE"] = self.options.daggy_console
        cmake.definitions["DAGGY_CORE_STATIC"] = self.options.daggy_core_static
        cmake.definitions["VERSION"] = self.version
        cmake.definitions["PACKAGE_DEPS"] = self.options.package_deps
        cmake.definitions["CMAKE_INSTALL_LIBDIR"] = self._libdir()
        cmake.configure()
        return cmake

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
        if self.options.package_deps:
            if self.settings.os == "Windows":
                self.copy("*.dll", src="@bindirs", dst="bin")
                self.copy("*.dll", src="@libdirs", dst="bin")
            else:
                self.copy("*.so.*", src="@libdirs", dst="{}/{}".format(self._libdir(), self.name))
