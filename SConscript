#
# Copyright 2016, Han Pengfei. All Rights Reserved.
# Distributed under the terms of the MIT License.
#

Import("genv")

framework_script = "framework/SConscript"
framework_files = genv.SConscript(framework_script)
genv.Install(genv["out"], framework_files)

examples_script = "examples/SConscript"
examples_files = genv.SConscript(examples_script)
genv.Install(genv["out"], examples_files)
