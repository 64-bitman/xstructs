project(
  'xstructs',
  'c',
  version: '1.0',
  license: 'Unlicense',
  default_options: [
    'c_std=c11',
    'warning_level=2',
    'b_ndebug=if-release'
  ]
)

includes = include_directories('.')

xstructs_dep = declare_dependency(
  include_directories: includes
)

subdir('tests')
