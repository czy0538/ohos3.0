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
import shutil

from .hdf_command_handler_base import HdfCommandHandlerBase
from .hdf_vendor_kconfig_file import HdfVendorKconfigFile
from .hdf_vendor_mk_file import HdfVendorMkFile
from .hdf_module_kconfig_file import HdfModuleKconfigFile
from .hdf_module_mk_file import HdfModuleMkFile
from .hdf_driver_config_file import HdfDriverConfigFile
import hdf_utils


class HdfDeleteHandler(HdfCommandHandlerBase):
    def __init__(self, args):
        super(HdfDeleteHandler, self).__init__()
        self.cmd = 'delete'
        self.handlers = {
            'vendor': self._delete_vendor_handler,
            'module': self._delete_module_handler,
            'driver': self._delete_driver_handler,
        }
        self.parser.add_argument("--action_type",
                                 help=' '.join(self.handlers.keys()),
                                 required=True)
        self.parser.add_argument("--root_dir", required=True)
        self.parser.add_argument("--vendor_name")
        self.parser.add_argument("--module_name")
        self.parser.add_argument("--driver_name")
        self.parser.add_argument("--board_name")
        self.args = self.parser.parse_args(args)

    def _delete_vendor_handler(self):
        self.check_arg_raise_if_not_exist("vendor_name")
        self.check_arg_raise_if_not_exist("board_name")
        root, vendor, _, _, _ = self.get_args()
        vendor_hdf_dir = hdf_utils.get_vendor_hdf_dir(root, vendor)
        if not os.path.exists(vendor_hdf_dir):
            return
        for module in os.listdir(vendor_hdf_dir):
            mod_dir = os.path.join(vendor_hdf_dir, module)
            if os.path.isdir(mod_dir):
                self._delete_module(module)
        shutil.rmtree(vendor_hdf_dir)

    def _delete_module(self, module):
        root, vendor, _, _, _ = self.get_args()
        HdfVendorKconfigFile(root, vendor).delete_module(module)
        HdfVendorMkFile(root, vendor).delete_module(module)
        module_path = hdf_utils.get_module_dir(root, vendor, module)
        if not os.path.exists(module_path):
            return
        drv_root_dir = hdf_utils.get_drv_root_dir(root, vendor, module)
        if os.path.exists(drv_root_dir):
            for driver in os.listdir(drv_root_dir):
                drv_path = os.path.join(drv_root_dir, driver)
                if os.path.isdir(drv_path):
                    self._delete_driver(module, driver)
        shutil.rmtree(module_path)

    def _delete_module_handler(self):
        self.check_arg_raise_if_not_exist("vendor_name")
        self.check_arg_raise_if_not_exist("module_name")
        self.check_arg_raise_if_not_exist("board_name")
        _, _, module, _, _ = self.get_args()
        self._delete_module(module)

    def _delete_driver(self, module, driver):
        root, vendor, _, _, board = self.get_args()
        drv_dir = hdf_utils.get_drv_dir(root, vendor, module, driver)
        if os.path.exists(drv_dir):
            shutil.rmtree(drv_dir)
        k_path = hdf_utils.get_module_kconfig_path(root, vendor, module)
        HdfModuleKconfigFile(root, module, k_path).delete_driver(driver)
        HdfModuleMkFile(root, vendor, module).delete_driver(driver)
        HdfDriverConfigFile(root, board, module, driver).delete_driver()

    def _delete_driver_handler(self):
        self.check_arg_raise_if_not_exist("vendor_name")
        self.check_arg_raise_if_not_exist("module_name")
        self.check_arg_raise_if_not_exist("driver_name")
        self.check_arg_raise_if_not_exist("board_name")
        _, _, module, driver, _ = self.get_args()
        self._delete_driver(module, driver)
