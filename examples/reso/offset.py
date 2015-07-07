#!/usr/bin/python
#-*- coding:utf-8 -*-

import os, sys, shlex, subprocess

def mesh_error(mesh1, mesh2):
    args = "wine ../tools/metro %s %s" %(mesh1, mesh2)
    popen = subprocess.Popen(shlex.split(args), stdout=subprocess.PIPE)
    metro = popen.stdout.read()
    lines = [line for line in metro.split("\n")
            if "mean" in line or "max" in line]
    error = [float(line.split()[2]) for line in lines]
    errmax = error[0] if error[0] > error[2] else error[2]
    errmean = error[1] if error[1] > error[3] else error[3]
    print args, ": %f %f" %(errmax, errmean)
    return (float(errmax), float(errmean))

def mesh_number(mesh_number_info):
    nums = mesh_number_info.split("(")[-1]
    nv = int(nums.split("v")[0])
    ne = int(nums.split("e")[0].split("/")[-1])
    nf = int(nums.split("f")[0].split("/")[-1])
    return (nv, ne, nf)

def offset(args):
    print args
    args = shlex.split(args)
    popen = subprocess.Popen(args, stdout = subprocess.PIPE)
    info_offset = [line for line in popen.stdout]
    time_offset = float(info_offset[-1].split(" ")[-1])
    nums_offset = mesh_number(info_offset[0])
    return (time_offset, nums_offset)

def simplify(args):
    print args
    args = shlex.split(args)
    popen = subprocess.Popen(args, stdout = subprocess.PIPE)
    info_simplify = [line for line in popen.stdout]
    time_simplify = float(info_simplify[-1].split(" ")[-1])
    nums_simpfliy = mesh_number(info_simplify[0])
    return (time_simplify, nums_simpfliy)

def record(fdata, data, cmd):
    time, nums = data
    data = "time: %f " %time + "elements: (%dv/%de/%df)" %nums
    line = "".join([cmd, ": ", data, "\n"])
    fdata.write(line)

if __name__ == "__main__":
    path = os.path.normpath(sys.argv[1])
    if path == "--clean":
        path = sys.argv[2]
        args = "rm -f  %s/_* %s/*.dat" %(path, path)
        clean = subprocess.Popen(args, shell = True)
        sys.exit()
    dist = 1.5875
    mesh = "%s/%s.ply" %(path, path)
    off0 = "%s/_Reso.ply" %path
    off1 = "%s/_Reso_fv.ply" %path
    off2 = "%s/_Reso_fe.ply" %path
    data = "%s/offset.dat" %path
    prog = "../offset"


    fdata = open(data, "w")
    infos = "../ioinfo -i " + mesh
    popen = subprocess.Popen(shlex.split(infos), stdout = subprocess.PIPE)
    infos = [line for line in popen.stdout]
    edges = mesh_number(infos[0])[1]

    simf2 = "%s/_Ress%s.ply" %(path, edges)
    for line in infos:
        print line,
        fdata.write("# %s"%line)

    data = offset("%s -t 0 -d %f -o %s %s"%(prog, dist, off0, mesh))
    record(fdata, data, "Reso\t")
    data = offset("%s -t 1 -d %f -o %s %s"%(prog, dist, off1, mesh))
    record(fdata, data, "Reso_fv\t")
    data = offset("%s -t 2 -d %f -o %s %s"%(prog, dist, off2, mesh))
    record(fdata, data, "Reso_fe\t")

    nedge = data[1][1]
    simf1 = "%s/_Ress%s.ply" %(path, nedge)
    data = simplify("../simplify -e %d -o %s %s" %(nedge, simf1, off0))
    record(fdata, data, "Ress\t")
    data = simplify("../simplify -e %d -o %s %s" %(edges, simf2, off0))
    record(fdata, data, "Ress\t")

    fdata.write("error fv: max: %f mean: %f \n" %mesh_error(off0, off1))
    fdata.write("error fe: max: %f mean: %f \n" %mesh_error(off0, off2))
    fdata.write("error s1: max: %f mean: %f \n" %mesh_error(off0, simf1))
    fdata.write("error s2: max: %f mean: %f \n" %mesh_error(off0, simf2))

