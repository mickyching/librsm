#!/usr/bin/python
#-*- coding:utf-8 -*-

import os, sys, shlex, subprocess

def milling(args):
    args = shlex.split(args)
    popen = subprocess.Popen(args, stdout = subprocess.PIPE)
    infos = [line for line in popen.stdout]
    return infos

if __name__ == "__main__":
    if sys.argv[1] == "--clean":
        path = sys.argv[2]
        os.system("rm -rf %s/_* %s/*.dat" %(path, path))
        sys.exit()

    path = os.path.normpath(sys.argv[1])
    filename = "%s/%s.ply" %(path, path)

    ippargs = "../milling -c %s/finish.cfg " %path
    ippargs += "-o %s/_finish_offset.ply " %path
    ippargs += "-s %s/_finish_curve.ipc " %path
    ippargs += "-p %s/_finish_path.ipp " %path
    ippargs += "-g %s/_finish_gcode.nc " %path
    ippargs += filename
    ippinfo = milling(ippargs)
    ipplogs = file("%s/finish.dat" %path, "w")
    for line in ippinfo:
        ipplogs.write(line)

    ilpargs = "../milling -c %s/rough.cfg " %path
    ilpargs += "-o %s/_rough_offset.ply " %path
    ilpargs += "-s %s/_rough_curve.ipc " %path
    ilpargs += "-p %s/_rough_path.ipp " %path
    ilpargs += "-l %s/_rough_path.ilp " %path
    ilpargs += "-g %s/_rough_gcode.nc " %path
    ilpargs += filename
    ilpinfo = milling(ilpargs)
    ilplogs = file("%s/rough.dat" %path, "w")
    for line in ilpinfo:
        ilplogs.write(line)

