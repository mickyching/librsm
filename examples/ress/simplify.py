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
    return (float(errmax), float(errmean))

def mesh_number(mesh_number_info):
    nums = mesh_number_info.split("(")[-1]
    nv = int(nums.split("v")[0])
    ne = int(nums.split("e")[0].split("/")[-1])
    nf = int(nums.split("f")[0].split("/")[-1])
    return (nv, ne, nf)

def simplify(args):
    print args
    popen = subprocess.Popen(shlex.split(args), stdout = subprocess.PIPE)
    info_simplify = [line for line in popen.stdout]
    time_simplify = float(info_simplify[-1].split(" ")[-1])
    nums_simpfliy = mesh_number(info_simplify[0])
    return (time_simplify, nums_simpfliy)

def simplify_loop(prog, enums, ifile):
    path = "/".join(ifile.split("/")[:-1])
    args = "../simplify "
    if prog == "Ress":
        pass
    elif prog == "Ressu":
        args += "-u "
    elif prog == "RessU":
        args += "-U "
    elif prog == "Memless":
        args += "-m "
    else:
        print "unknown simplify program!"

    data = []
    for num in enums:
        ofile = "%s/_%s%s.ply" %(path, prog, str(num))
        sdata = simplify(args + "-e %d -o %s %s" %(num, ofile, ifile))
        nv, ne, nf = sdata[1]
        time = sdata[0]
        errmax, errmean = mesh_error(ifile, ofile)
        info = (nv, ne, nf, time, errmax, errmean, errmax*nf, errmean*nf)
        data.append(info)

    fdata = file("%s/%s.dat" %(path, prog), "w")
    topline  = "# nvertex nedge nface time(s) maxerror meanerror "
    topline += "nface*maxerror nface*meanerror\n"
    fdata.write(topline)
    for row in data:
        fdata.write("%d %d %d %f %f %f %f %f\n" %row)

def qslim_loop(prog, ifile):
    path = "/".join(ifile.split("/")[:-1])
    ismf = ".".join(ifile.split(".")[:-1]) + ".smf"
    args = "../tools/ply2smf < %s > %s" %(ifile, ismf)
    ply2smf = subprocess.Popen(args, shell = True)
    ply2smf.wait()

    tfile = open("%s/Memless.dat" %path, "r")
    tface = [line.split()[2] for line in tfile if line[0] != "#"]
    tnums = [int(nface) for nface in tface]

    data = []
    for num in tnums:
        osmf = "%s/_%s%d.smf" %(path, prog, num)
        ofile = "%s/_%s%d.ply" %(path, prog, num)

        args = "../tools/qslim -t %d -o %s %s" %(num, osmf, ismf)
        qslim = subprocess.Popen(args, shell = True,
                stderr = subprocess.PIPE)
        qslim.wait()
        print args

        args = "../tools/smf2ply < %s > %s; rm %s" %(osmf, ofile, osmf)
        smf2ply = subprocess.Popen(args, shell = True)
        smf2ply.wait()
        #print args

        info_qslim = [line for line in qslim.stderr]
        time_qslim = float(info_qslim[-1].split(" ")[-1])
        nums = info_qslim[1].split("(")[-1]
        nv = int(nums.split("v")[0])
        ne = 0
        nf = int(nums.split("f")[0].split("/")[-1])
        nums_qslim = (nv, ne, nf)
        sdata = (time_qslim, nums_qslim)
        errmax, errmean = mesh_error(ifile, ofile)
        nv, ne, nf = sdata[1]
        time = sdata[0]
        info = (nv, ne, nf, time, errmax, errmean, errmax*nf, errmean*nf)
        data.append(info)

    fdata = file("%s/%s.dat" %(path, prog), "w")
    topline  = "# nvertex nedge nface time(s) maxerror meanerror "
    topline += "nface*maxerror nface*meanerror\n"
    fdata.write(topline)
    for row in data:
        fdata.write("%d %d %d %f %f %f %f %f\n" %row)


def plot_item(path, col):
    item  = 'plot '
    item += '"%s/RessU.dat"   u 3:%d w lp pt 15 ps 2 lt 1 title "RessU", \\\n' %(path, col)
    item += '"%s/Ressu.dat"    u 3:%d w lp pt  5 ps 2 lt 3 title "Ressu", \\\n' %(path, col)
    item += '"%s/Ress.dat"    u 3:%d w lp pt  9 ps 2 lt 3 title "Ress", \\\n' %(path, col)
    item += '"%s/Qslim.dat"   u 3:%d w lp pt 8 ps 2 lt 0 title "Qslim", \\\n' %(path, col)
    item += '"%s/Memless.dat" u 3:%d w lp pt 6 ps 2 lt 0 title "Memless" \n ' %(path, col)
    return item

def gnuplot(fname):
    path = fname.split("/")[0]
    content = 'set term postscript eps enhanced color font "Times_New_Roman, 20"\n'
    content += 'set tmargin at screen 0.95\n'
    content += 'set bmargin at screen 0.12\n'
    content += 'set lmargin at screen 0.12\n'
    content += 'set rmargin at screen 0.95\n'
    content += 'set logscale x\n'
    content += 'set logscale y\n'
    content += 'set format x "10^{%L}"\n'
    content += 'set format y "10^{%L}"\n'
    content += 'set xlabel "Triangles"\n'

    content += 'set ylabel "Time(s)"\n'
    content += 'unset title\n'
    content += 'set output "%s/time.eps"\n' %path
    content += plot_item(path, 4)

    content += 'set ylabel "Error"\n'
    content += 'set output "%s/max.eps"\n' %path
    content += plot_item(path, 5)

    content += 'set output "%s/mean.eps"\n' %path
    content += plot_item(path, 6)

    fout = file(fname, "w")
    fout.write(content)
    args = "gnuplot " + fname
    popen = subprocess.Popen(shlex.split(args))

if __name__ == "__main__":
    path = os.path.normpath(sys.argv[1])
    if path == "--clean":
        path = sys.argv[2]
        args = "rm -f  %s/_* %s/*.dat %s/*.eps %s/*.smf" %(path, path, path, path)
        clean = subprocess.Popen(args, shell = True)
        sys.exit()

    ifile = "%s/%s.ply" %(path, path)

    infos = "../ioinfo " + ifile
    popen = subprocess.Popen(shlex.split(infos), stdout = subprocess.PIPE)
    infos = [line for line in popen.stdout]
    print infos[0],
    edges = mesh_number(infos[0])[1]
    nedge = edges
    enums = []
    while nedge > 0.002 * edges:
        nedge *= 0.5
        enums.append(int(nedge))

    simplify_loop("Ress", enums, ifile)
    simplify_loop("Ressu", enums, ifile)
    simplify_loop("RessU", enums, ifile)
    simplify_loop("Memless", enums, ifile)
    qslim_loop("Qslim", ifile)

    gnuplot("%s/gnuplot.dat" %path)
