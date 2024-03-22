# C module: testmodule

testmodule_src = files([
  'testmodule.c',
])
c_src_lint += testmodule_src

testmodule_mod = shared_module(
  'testmodule',
  testmodule_src,
  dependencies: libknot,
  include_directories: mod_inc_dir,
  name_prefix: '',
  install: true,
  install_dir: modules_dir,
  link_with: kresd,
)

#integr_tests += [
#  ['testmodule', meson.current_source_dir() / 'test.integr'],
#]
