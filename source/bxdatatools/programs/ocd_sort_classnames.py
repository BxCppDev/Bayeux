#!/usr/bin/env python3
import sys
import string

if __name__ == "__main__" :
    trace=False
    nargs=len(sys.argv)
    iarg=1
    ordered_namespaces=[]
    while iarg < nargs :
        arg=sys.argv[iarg]
        if arg == "--trace" :
            trace = True
        elif arg == "--ordered-namespace" or arg == "-o" :
            iarg = iarg+1
            ons = sys.argv[iarg]
            ordered_namespaces.append(ons)
        iarg = iarg+1
    if trace :
        for ons in ordered_namespaces:
            sys.stderr.write("Ordered namespace = '%s'\n" % ons)

    lines=sys.stdin.readlines()
    namespaces={}
    for line in lines:
        l = line.strip()
        tokens=l.split("::")
        ntokens=len(tokens)
        classname=tokens[-1]
        namespace=string.replace(l, "::%s" % classname, "", -1)
        if not namespace in namespaces.keys() :
            namespaces[namespace] = []
        namespaces[namespace].append(classname)
    if len(ordered_namespaces) > 0 :
        for ons in ordered_namespaces:
            namespace = ons
            if trace :
                sys.stderr.write("Processing namespace = '%s'\n" % namespace)
            if namespace in namespaces.keys() :
                for classname in namespaces[namespace]:
                    if trace :
                        sys.stderr.write(" - processing class = '%s'\n" % classname)
                    if len(classname) :
                        print("%s::%s" % (namespace,classname))
    else:
        for namespace in namespaces.keys() :
            for classname in namespaces[namespace]:
                if len(classname) :
                    print("%s::%s" % (namespace,classname))
