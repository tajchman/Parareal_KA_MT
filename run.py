#! /usr/bin/env python3

import os, subprocess, argparse, sys, shutil, requests
  

parser = argparse.ArgumentParser()
parser.add_argument('-s', '--sequential', action='store_true')
parser.add_argument('-p', '--parareal', action='store_true')
parser.add_argument('-tmax', type=float, default=-1.0)
parser.add_argument('-r', '--refinment', type=int, default=3)
parser.add_argument('-np', type=int, default=4)
parser.add_argument('--dt', type=float, default=0.001)
parser.add_argument('--dt_coarse', type=float, default=0.1)
parser.add_argument('--verbose', '-v', action='store_true')
parser.add_argument('-x', '--xterm', action='store_true')

parser.add_argument('-d', '--debug', action='store_true')
group = parser.add_mutually_exclusive_group()
group.add_argument('--gdb', action='store_true')
group.add_argument('--must', action='store_true')
group.add_argument('--valgrind', action='store_true')

args = parser.parse_args()

if args.tmax < 0.0:
  args.tmax = 1.0 * args.np
 
if args.gdb or args.valgrind or args.must:
  args.debug = True

paraRunArgs = ['-tmax', str(args.tmax), 
               '-refinment', str(args.refinment),
               '-output', str(args.dt_coarse),
               '-dt_coarse', str(args.dt_coarse),
               '-dt_fine', str(args.dt)]
seqRunArgs  = ['-tmax', str(args.tmax), 
               '-refinment', str(args.refinment),
               '-output', str(args.dt),
               '-dt', str(args.dt)]

if args.verbose:
  paraRunArgs += ['-verbose']
  seqRunArgs += ['-verbose']

if args.must:
  codePar = os.path.join('install', 'debug', 'PararealCode')
  if os.path.exists(codePar):
    subprocess.run(['mustrun', '-n', str(args.np), 
                    codePar ] + paraRunArgs)
elif args.valgrind:
  codePar = os.path.join('install', 'debug', 'PararealCode')
  if os.path.exists(codePar):
    subprocess.run(['mpirun', '-n', str(args.np), 'gnome-terminal', '--', 
                    'valgrind', '--suppressions=/usr/share/openmpi/openmpi-valgrind.supp', codePar] + paraRunArgs)
elif args.gdb:
  codePar = os.path.join('install', 'debug', 'PararealCode')
  if os.path.exists(codePar):
    s = ""
    if os.path.exists("gdb_breakpoints"):
      with open("gdb_breakpoints") as f:
        s = f.read();
    with open('.gdbinit', 'w') as f:
      f.write(' '.join(['set', 'args'] + paraRunArgs) + '\n')
      f.write(s + '\n')
    subprocess.run(['mpirun', '-n', str(args.np), 'xterm', '-hold', '-e', 
                    'gdb', '-ex', 'run', codePar])
elif args.debug:
  codeSeq = os.path.join('install', 'debug', 'SequentialCode')
  if args.sequential and os.path.exists(codeSeq):
    command = [codeSeq] + seqRunArgs
  codePar = os.path.join('install', 'debug', 'PararealCode')
  if os.path.exists(codePar):
    subprocess.run(['mpirun', '-n', str(args.np), 'xterm', '-hold', '-e', 
                    codePar] + paraRunArgs)
else:
  if not args.sequential and not args.parareal:
    args.sequential = True
    args.parareal = True

  codeSeq = os.path.join('install', 'release', 'SequentialCode')
  if args.sequential and os.path.exists(codeSeq):
    command = [codeSeq] + seqRunArgs
    print(' '.join(command))
    subprocess.run(command)
  codePar = os.path.join('install', 'release', 'PararealCode')
  if args.parareal and os.path.exists(codePar):
    command = [codePar] + paraRunArgs
    if args.xterm:
       command = ['xterm', '-hold', '-e'] + command

    command = ['mpirun', '-n', str(args.np)] + command
    print(' '.join(command))
    subprocess.run(command)
