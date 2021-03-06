#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of
#    conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list
#    of conditions and the following disclaimer in the documentation and/or other materials
#    provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may be used
#    to endorse or promote products derived from this software without specific prior written
#    permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import os
import re

from hdf_tool_settings import HdfToolSettings
import hdf_utils


class HdfLiteMkFile(object):
    def __init__(self, root):
        self.file_path = hdf_utils.get_hdf_lite_mk_path(root)
        self.orig_exist = False
        if os.path.exists(self.file_path):
            self.lines = hdf_utils.read_file_lines(self.file_path)
            self.orig_exist = True
        else:
            self.lines = []
        self.end_line_pattern = r'LITEOS_BASELIB\s*[+]=\s*--no-whole-archive'
        self.vendor_pattern = r'include.*hdf_vendor[.]mk'
        self.target_format = r'include .*vendor/%s/.*/hdf_vendor[.]mk'
        self.end_line_index = -1
        self.target_line_index = -1
        self.current_vendor_pattern = \
            r'^\s*include.*/vendor/(.*)/hdf_vendor.mk'

    def _save(self):
        if self.orig_exist:
            hdf_utils.write_file(self.file_path, ''.join(self.lines))

    def _find_all_vendors(self, current_vendor):
        vendor_lines = []
        for index, line in enumerate(self.lines):
            if self.target_line_index == -1:
                if re.search(self.target_format % current_vendor, line):
                    self.target_line_index = index
                    continue
            if self.end_line_index == -1:
                if re.search(self.end_line_pattern, line):
                    self.end_line_index = index
                    continue
            if re.search(self.vendor_pattern, line):
                vendor_lines.append(index)
        return vendor_lines

    def _comment_other_vendors(self, other_vendors):
        for index in other_vendors:
            if not hdf_utils.is_commented_line(self.lines[index], '#'):
                self.lines[index] = '# %s' % self.lines[index]

    def set_vendor(self, current_vendor):
        other_vendors = self._find_all_vendors(current_vendor)
        self._comment_other_vendors(other_vendors)
        drivers_path = HdfToolSettings().get_drivers_path()
        new_line = \
            'include $(LITEOSTOPDIR)/../../vendor/%s/%s/hdf_vendor.mk\n' % \
            (current_vendor, drivers_path)
        if self.target_line_index != -1:
            self.lines[self.target_line_index] = new_line
        elif self.end_line_index != -1:
            self.lines.insert(self.end_line_index, new_line)
        else:
            self.lines.append(new_line)
        self._save()

    def get_current_vendor(self):
        for line in self.lines:
            match_obj = re.search(self.current_vendor_pattern, line)
            if match_obj:
                vendor_drivers_path = match_obj.group(1)
                parts = vendor_drivers_path.split('/')
                valid_parts = [part for part in parts if part]
                if valid_parts:
                    return valid_parts[0]
        return ''
