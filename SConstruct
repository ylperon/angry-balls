# vim: filetype=python

AddOption('--debug-build',
          default = False,
          action = 'store_true',
          help = 'Build in debug mode.'
         )
AddOption('--use-asan',
          default = False,
          action = 'store_true',
          help = 'Use AddressSanitizer.'
         )
AddOption('--use-tsan',
          default = False,
          action = 'store_true',
          help = 'Use ThreadSanitizer.'
         )
AddOption('--static-analysis',
          default = False,
          action = 'store_true',
          help = 'Use Clang code static analysis.'
         )
debug_mode = GetOption('debug_build')
use_asan = GetOption('use_asan')
use_tsan = GetOption('use_tsan')
static_analysis = GetOption('static_analysis')

import os

project_root = os.path.abspath('.')
build_subdir = 'debug' if debug_mode else 'release'


cpp_path = ['include/',
            'contrib/jsoncpp/include'
           ]
cpp_path = [os.path.join(project_root, x) for x in cpp_path]


cpp_flags = ['-Wall',
             '-Wextra',
             '-pedantic',
             '-std=c++11',
             '-stdlib=libc++',
             '-pthread'
            ]
if debug_mode:
    cpp_flags.append(['-g'])
else:
    cpp_flags.append(['-O2'])

if use_asan:
    cpp_flags.append(['-fno-omit-frame-pointer',
                      '-fno-optimize-sibling-calls',
                      '-fsanitize=address'
                     ]
                    )
if use_tsan:
    cpp_flags.append(['-fsanitize=thread'
                     ]
                    )
if static_analysis:
    cpp_flags.append(['--analyze'])
compiler = 'clang++'

lib_path = ['lib/protocol',
            'lib/coin_generators',
            'lib/player_generators',
            'lib/emulator',
            'lib/util',
            'src/strategies'
           ]
lib_path = [os.path.join(project_root, x) for x in lib_path]


env = Environment(CXX = compiler,
                  CPPFLAGS = cpp_flags,
                  CPPPATH = cpp_path
                 )

for lib in lib_path:
    SConscript(os.path.join(lib, 'SConscript'),
               variant_dir = os.path.join(lib, build_subdir),
               exports = ['env']
              )


libs = ['libprotocol.a',
        'libcoin_generators.a',
        'libplayer_generators.a',
        'libemulator.a',
        'libutil.a',
        'libstrategies.a',
        'c++',
        'pthread'
        ]


# Adding build directories for debug/release cases
lib_path = [os.path.join(x, build_subdir) for x in lib_path]

linker_flags = []
if use_asan:
    linker_flags.append(['-fsanitize=address'])
if use_tsan:
    linker_flags.append(['-fsanitize=thread'])

env.Append(LIBS = libs,
           LIBPATH = lib_path,
           LINKFLAGS = linker_flags
          )

binaries_path = ['src/client',
                 'src/web-ui-client',
                 'src/server'
                ]
binaries_path = [os.path.join(project_root, x) for x in binaries_path]

for binary in binaries_path:
    SConscript(os.path.join(binary, 'SConscript'),
               variant_dir = os.path.join(binary, build_subdir),
               exports = ['env']
              )
