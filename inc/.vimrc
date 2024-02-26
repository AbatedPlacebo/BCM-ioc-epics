source ../.vimrc_common
set path+=../
set path+=../src
set path+=../genlib
set path+=/usr/lib/epics/include
set fileencodings=utf-8
if has("cscope")
if filereadable("../cscope.out")
"set cscopequickfix=s-,c-,d-,i-,t-,e-
cs add ../cscope.out ../ -C
endif
endif


