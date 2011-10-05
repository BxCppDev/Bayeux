BEGIN{tag=0}
{ 
  if (tag==0) 
  {
      if ($1=="<PRE>")
      {
	  tag=1
	  print $0
      }
  }
  else
  {
      if (tag==1) 
      {
	  print $0
	  if ($1=="</PRE>")
	  {
	      tag=0
	  }	  
      }    
  }
}  