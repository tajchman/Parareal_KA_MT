#! /usr/bin/env python

import sys, glob
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.backends.backend_pdf import PdfPages
import argparse, os

parser = argparse.ArgumentParser()
parser.add_argument('-d', '--dir', default='.')
args = parser.parse_args()
testDir = args.dir

class Plot:
  def open(self, fileName, title):
    self.fName = fileName
    self.fig, self.ax = plt.subplots()
    self.ax.set_title(title)
  
  def addplot(self, x, y, color, label=None):
    if label:
      self.ax.plot(x, y,color + 'o-', markersize=1, 
                   linewidth=0.5, label=label)
    else:
      self.ax.plot(x, y,color + 'o-', markersize=1, 
                   linewidth=0.5, label=label)

  def close(self, label=None):
    if label:
      self.ax.legend()
    plt.savefig(self.fName)
    plt.close(self.fig)
    
l = glob.glob(os.path.join(testDir, "out", "sequential", 'probeSeq*'))
if l:
  p = Plot()
  p.open(os.path.join(testDir, 'plot_sequential.svg'), 'Sequential computation')
  dataSeq = np.loadtxt(l[0])
  p.addplot(dataSeq[:,0], dataSeq[:,1], "k", 'sequential')
  p.close(True)

o = 0
while True:
  p = Plot()
  t = os.path.join(testDir, 'out', 'parareal', '**', 
                   'probe_Coarse_' + str(o) + '_*.dat')
  l1 = glob.glob(t)
  t = os.path.join(testDir, 'out', 'parareal', '**', 
                   'probe_Fine_' + str(o) + '_*.dat')
  l2 = glob.glob(t)
  l = l1 + l2
  if len(l) == 0:
    break

  l.sort()
  title = "Iteration " + str(o)
  p.open("_".join(["plot", str(o) + '.svg']), title)
    
  for f in l:
    sf = f.split('_')
    print(sf)
    data = np.loadtxt(f)
    print(data.shape)
    if len(data.shape) < 2:
      continue
    if sf[1] == "Fine":
      p.addplot(data[:,0], data[:,1], "", 
                sf[1] + ' scheme, process ' + str(sf[3]))
    if sf[1] == 'Coarse':
      if sf[3] == 0:
        p.addplot(data[:,0], data[:,1], "k", sf[1] + ' scheme')
      else:
        p.addplot(data[:,0], data[:,1], "k")
        
    
  if o==0:
    p.close()
  else:
    p.close(True)
  o += 1


sys.exit(0)
