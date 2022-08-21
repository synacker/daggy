'''
MIT License

Copyright (c) 2022 synacker

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
import re

class GitVersion():
    def __init__(self):
        os.chdir(os.path.dirname(os.path.realpath(__file__)))

    @property
    def tag(self):
        stream = os.popen("git describe --match v[0-9]* --abbrev=0 --tags")
        version_tag = stream.read().strip()
        if version_tag:
            version_tag = version_tag[1:]
        return version_tag

    @property
    def version(self):
        version = f"{self.tag}.{self.build_number}"
        if not (self.branch == self.default_branch or re.match("release/.*", self.branch)):
            version = f"{version}-{self.branch}"
        return version

    @property
    def default_branch(self):
        stream = os.popen("git config --get init.defaultBranch")
        return stream.read().strip()

    @property
    def build_number(self):
        stream = os.popen("git rev-list {}.. --count".format(self.tag))
        return stream.read().strip()

    @property
    def branch(self):
        stream = os.popen("git branch --show-current")
        return stream.read().strip()

if __name__ == "__main__":
    git_version = GitVersion()
    print(f"{git_version.version}")