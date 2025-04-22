project('mango', 'cpp',
  version : '0.1',
  default_options : ['warning_level=everything', 'cpp_std=c++20'])

gtest_dep = dependency('gtest')

exe = executable('mango',
           'mango.cc',
           dependencies: [gtest_dep],
           install : true)

test('basic', exe)
