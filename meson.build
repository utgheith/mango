project('mango', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++23'])

cpp = meson.get_compiler('cpp')
cpp.has_header('format')

gtest_dep = dependency('gtest')

main = executable('mango',
           'mango.cc',
           dependencies: [gtest_dep],
           install : true)

test('basic', main)
