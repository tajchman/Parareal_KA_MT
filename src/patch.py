#! /bin/env python

import os, sys, shutil

base = sys.argv[1]

fName = os.path.join(base, 'DEAL.II', 'include', 'deal.II', 'lac', 'sparse_matrix.h')
fTmpName = os.path.join(base, 'DEAL.II', 'include', 'deal.II', 'lac', 'sparse_matrix.h_tmp')
shutil.copyfile(fName, fTmpName)

inSparseMatrix = False
done = False

with open(fTmpName) as fIn:
  with open(fName, "w") as fOut:
    for l in fIn:
      if "class SparseMatrix :" in l:
        inSparseMatrix = True
      if not done and inSparseMatrix and "protected" in l:
        fOut.write("  std::unique_ptr<number[]> & data() { return val; }\n")
        fOut.write("  size_t size() { return max_len; }\n")
        done = True
      fOut.write(l);      