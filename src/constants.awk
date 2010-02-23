BEGIN {
    print "#include \"pymodule.h\"";
    print "#include \"pyirssi_irc.h\"";
    print
    print "void pyconstants_init(void)"
    print "{"
}

{
    if (NF >= 2)
        constant = $2;
    else
        constant = $1;

    printf("    PyModule_AddIntConstant(py_module, \"%s\", %s);\n", $1, constant);
}

END {
    print "}"
}
