dnl func.m4
dnl
dnl
dnl
dnl set macro quote 
changequote([[,]])dnl
dnl
define([[mym4_html_test]],[[dnl
<div>
This is a test
</div>
]])dnl
dnl
define([[mym4_html_source_code]],[[dnl
<div style="background-color: #ccc; padding: 4px 2px; border: 1px" >
<img src="images/c++-source-code.png" height="20pt" style="float:left" />&nbsp;The <tt class="fs">[[$1]].cpp</tt> source code <a href="./code/[[$1]].cpp" class="button" >Download</a> 
&nbsp; <a href="javascript:toggleDiv('[[$1]]_cpp');" class="button" >Show/hide</a>
<div id="[[$1]]_cpp" >include([[$1.cpp.html]])</div><br></div>
]])dnl
dnl
define([[mym4_html_to_top2]],[[dnl
<div style="background-color: #ccc; padding: 4px 2px; border: 1px" >
<a href="#top" class="button" >To top</a>
</div>
]])dnl
dnl
define([[mym4_html_to_top]],[[dnl
<!--div style="background-color: #ccc; padding: 4px 2px; border: 1px" -->
<!--img src="images/c++-source-code.png" height="20pt" style="float:left" /-->
<div>
<a href="#top" class="button" >To top</a> 
<!--div id="[[$1]]_cpp" ></div--><br>
</div>
]])dnl
dnl
dnl
dnl
dnl end of func.m4
