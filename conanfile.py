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
import os
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy
from conan.tools.build import check_min_cppstd
from git_version import GitVersion

required_conan_version = ">=2.0"
class DaggyConan(ConanFile):
    name = "daggy"
    license = "MIT"
    homepage = "https://daggy.dev"
    url = "https://github.com/synacker/daggy"
    description = "Data Aggregation Utility and C/C++ developer library for data streams catching."
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False]
    }
    default_options = {
        "shared": True,
        "fPIC": False
    }
    generators = "CMakeDeps"
    exports = ["git_version.py", "src/*"]


    _cmake = None

    def set_version(self):
        self.version = GitVersion().version

    def validate(self):
        check_min_cppstd(self, "17")

    def config_options(self):
        if not self.options.shared:
            self.options.fPIC = True

        if self.settings.os == "Windows":
            del self.options.fPIC

        self.options["qt"].shared = self.options.shared
        self.options["libssh2"].shared = self.options.shared
        self.options["yaml-cpp"].shared = self.options.shared
        
    def build_requirements(self):
        self.tool_requires("cmake/3.30.1")
        self.tool_requires("gtest/1.15.0")

    def requirements(self):
        self.requires("qt/6.7.3")
        self.requires("kainjow-mustache/4.1")
        self.requires("yaml-cpp/0.8.0")
        self.requires("libssh2/1.11.1")


    def layout(self):
        self.folders.source = "src"

        self.cpp.libdirs = ["lib"]
        self.cpp.bindirs = ["bin"]

        cmake_layout(self, src_folder=self.folders.source)
        
    def generate(self):
        libdir = os.path.normpath(os.path.join(self.build_folder, self.cpp.libdirs[0], self.name))
        bindir = os.path.normpath(os.path.join(self.build_folder, self.cpp.bindirs[0]))
        for dep in self.dependencies.values():
            if self.settings.os == "Windows":
                if dep.cpp_info.bindirs:
                    copy(self, "*.dll", dep.cpp_info.bindirs[0], bindir) 
                if dep.cpp_info.libdirs:
                    copy(self, "*.dll", dep.cpp_info.libdirs[0], bindir)
            elif self.settings.os == "Linux":
                if dep.cpp_info.libdirs:
                    copy(self, "*.so.*", dep.cpp_info.libdirs[0], libdir)
            else:
                if dep.cpp_info.libdirs:
                    copy(self, "*.dylib", dep.cpp_info.libdirs[0], libdir)

        tc = CMakeToolchain(self)
        tc.cache_variables["CMAKE_INSTALL_LIBDIR"] = self.cpp.libdirs[0]
        tc.cache_variables["SSH2_SUPPORT"] = True
        tc.cache_variables["YAML_SUPPORT"] = True
        tc.cache_variables["CONSOLE"] = True
        tc.cache_variables["PCAPNG_SUPPORT"] = False
        tc.cache_variables["PORTABLE_BUILD"] = True
        tc.cache_variables["BUILD_TESTING"] = True
        tc.cache_variables["CONAN_BUILD"] = True
        tc.cache_variables["VERSION"] = GitVersion().version
        tc.cache_variables["VERSION_EXTENDED"] = GitVersion().extended
        
        if self.options.shared:
            tc.cache_variables["CMAKE_C_VISIBILITY_PRESET"] = "hidden"
            tc.cache_variables["CMAKE_CXX_VISIBILITY_PRESET"] = "hidden"
            tc.cache_variables["CMAKE_VISIBILITY_INLINES_HIDDEN"] = 1
        tc.generate()    

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        

    def package(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["DaggyCore"]
        self.cpp_info.libdirs = [self._libdir()]
