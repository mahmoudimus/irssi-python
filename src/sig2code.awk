BEGIN {
    FS = "[ \t]*->[ \t]*";

    #read in codes
    while (getline < "sig2code.txt")
    {
        sub(/^[ \t]*/, ""); 
        if (NF < 2)
            continue;

        #print $1, $2
        sigmap[$2] = $1
    }

    close("sig2code.txt");

    FS = "[ \t]*,[ \t]*"; 

    print "/* Include in your C module */";
    print "static PY_SIGNAL_SPEC_REC py_sigmap[] = {";
}

function match_type(t)
{
    for (type in sigmap)
    {
        if (index(t, type) != 0)
            return sigmap[type];
    }

    return "?";
}

$1 ~ /^[ \t]*"/ && $1 !~ /"script error"/ { 
    sub(/^[ \t]*/, ""); 

    signal = $1
    if (signal ~ /.*<cmd>$/)
    {
        varsig = 1;
        sub(/<cmd>/, "", signal);
    }
    else
        varsig = 0;

    args = "";
    for (i = 2; i <= NF; i++)
    {
        args = args""match_type($i);
    }

    printf("    {%s, \"%s\", 0, 0, %d},\n", signal, args, varsig);
}

END {
    print "    {NULL}" ;
    print "};";
    print "";
    print "#define py_sigmap_len() (sizeof(py_sigmap) / sizeof(py_sigmap[0]) - 1)";
}

