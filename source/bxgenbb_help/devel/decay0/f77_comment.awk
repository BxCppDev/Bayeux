BEGIN {
    first_line=fl
    last_line=ll
    hook_line=hl
    nline=0
}
{
    nline=nline+1
    #print nline, first_line, last_line
    if ((nline >= first_line) && (nline <= last_line))
    {
	if (nline == hook_line)
	{
	    printf ("c@HOOK\n");
	}
	else 
	{
	    printf ("c%s\n", $0);
	}
    }
    else
    {
	printf ("%s\n", $0);
    }
}
END {
}