'''
The MIT License (MIT)
Copyright 2017-2020 Milovidov Mikhail
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

from conans import ConanFile, CMake
import git_version

class DaggyConan(ConanFile):
    name = "Daggy"
    version = git_version.version()
    license = "MIT"
    url = "https://daggy.dev"
    description = "Data Aggregation Utilty - aggregation and stream data via remote and local processes."
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "ssh2_support": [True, False],
        "ssh2_support": [True, False],
        "yaml_support": [True, False],
        "daggy_console": [True, False],
        "daggy_core_static": [True, False]
    }
    default_options = {
        "ssh2_support": True,
        "yaml_support": True,
        "daggy_console": True,
        "daggy_core_static": False
    }
    generators = "cmake"
    export_sources = ["CMakeListrs.txt", "src/*"]

    def requirements(self):
        self.requires("qt/5.14.1@bincrafters/stable")

    def configure(self):
        self.options["qt"].shared = False
        self.options["qt"].commercial = False
        self.options["qt"].opengl = "no"
        self.options["qt"].openssl = False
        self.options["qt"].GUI = False
        self.options["qt"].widgets = False
        self.options["qt"].with_pcre2 = False
        self.options["qt"].with_glib = False

        self.options["qt"].with_doubleconversion = True
        self.options["qt"].with_freetype = False
        self.options["qt"].with_freetype = False
        self.options["qt"].with_fontconfig = False
        self.options["qt"].with_icu = False
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

        self.options["qt"].device = None
        self.options["qt"].cross_compile = None
        self.options["qt"].sysroot = None
        self.options["qt"].config = None
        self.options["qt"].multiconfiguration = False
        self.options["libxcb"].shared = False

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
