#! /usr/bin/env python3

import os, subprocess, argparse, sys, shutil, requests, platform
from pathlib import Path


def log_message(message, fOut):
  print(message, end="")
  fOut.write(message)
  sys.stdout.flush()
  fOut.flush()

def log_subprocess(cmd, cwd, env, fOut):
  if env:
    e = env
  else:
    e = os.environ
  with subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                        cwd=cwd, env=e, bufsize=0, text=True) as p:
    if p.stdout:
      for line in p.stdout:
        print(line, end="")
        sys.stdout.flush()
        if fOut:
          fOut.write(line)
          fOut.flush()
  return p.returncode

def execute(commands, cwd, e, logFile):
  for cmd in commands:
    if logFile:
      log_message('__________________\n' + ' '.join(cmd) + '\n', logFile)
    err = log_subprocess(cmd, cwd, e, logFile)
    if not err==0:
      return False
  return True

class Build():
  def __init__(self, baseDir, mode, verbose, parallel):
    self.baseDir = baseDir
    self.env = os.environ.copy()
    self.srcDir = os.path.join(self.baseDir, 'src')
    self.buildDir = os.path.join(self.baseDir, 'build', mode)
    self.installDir = os.path.join(self.baseDir, 'install', mode)
    self.testDir = os.path.join(self.baseDir, 'test')
    self.mode = mode
    self.verbose = verbose
    self.parallel = parallel
    
def build(a, opt):
  
  p = platform.system()
  Gen = "Unix Makefiles"
  if p == 'Windows':
    Gen = "Ninja"
    
  os.makedirs(a.buildDir, exist_ok=True)
  os.makedirs(a.testDir, exist_ok=True)
  
  configureCmd = ['cmake', 
                  '-B', a.buildDir,
                  '-S', a.srcDir,
                  '-DCMAKE_INSTALL_PREFIX=' + a.installDir,
                  '-DCMAKE_BUILD_TYPE=' + a.mode.capitalize(),
                  '-DTEST_DIR=' + a.testDir,
                  '-G', Gen
                  ]
  if opt:
    for o in opt:
      configureCmd += ['-D' + o]
    
  compileCmd = ['cmake', '--build', a.buildDir]
  if a.parallel > 1:
    compileCmd += ['--parallel', str(a.parallel)]
  installCmd = ['cmake', '--install', a.buildDir]
  
  res = False
  with open(os.path.join(a.buildDir, 'log_build.txt'), 'w') as logFile:
    res = execute([configureCmd, compileCmd, installCmd],
                  '.', a.env, logFile)
  return res

parser = argparse.ArgumentParser()
parser.add_argument('-m', '--mode', 
                    nargs='*', choices=['debug', 'release', 'prof'], default=['release'])
parser.add_argument('--test', action='store_true')
parser.add_argument('-v', '--verbose', action='store_true')
parser.add_argument('-j', '--parallel', type=int, default=1)
args = parser.parse_args()

for m in args.mode:
  b = Build(os.getcwd(), m, args.verbose, args.parallel)
  if not build(b, None):
    sys.exit(-1)

  if os.path.exists(os.path.join('build', m, 'compile_commands.json')):
    shutil.copyfile(os.path.join('build', m, 'compile_commands.json'), os.path.join('.', 'compile_commands.json'))

  if args.test:
    print('____________________')
    testCmd = ['ctest']
    execute([testCmd], b.buildDir, None, None)