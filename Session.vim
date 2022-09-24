let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd ~/Desktop/Projekte/PATRISPREDICTUM/Projekte/Schule/SortTest
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
set shortmess=aoO
badd +93 List.c
badd +9 List.h
badd +1 SortTest.c
badd +1 csv.c
badd +1 csv.h
badd +1 Sort.c
badd +1 Sort.h
badd +1 GUI.h
badd +205 GUI.c
badd +1 List/List.h
badd +9 SortTest.h
badd +1 argparse.c
badd +1 argparse.h
badd +1 List/List.c
badd +1 SortList.c
badd +1 SortList.h
badd +1 GUI_impl.h
badd +0 GUI_impl_unix.c
argglobal
%argdel
set stal=2
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabrewind
edit SortTest.c
argglobal
balt SortTest.h
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 28 - ((27 * winheight(0) + 33) / 66)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 28
normal! 029|
tabnext
edit GUI.h
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
wincmd _ | wincmd |
vsplit
2wincmd h
wincmd w
wincmd w
wincmd _ | wincmd |
split
1wincmd k
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 49 + 146) / 293)
exe 'vert 2resize ' . ((&columns * 120 + 146) / 293)
exe '3resize ' . ((&lines * 33 + 34) / 69)
exe 'vert 3resize ' . ((&columns * 122 + 146) / 293)
exe '4resize ' . ((&lines * 32 + 34) / 69)
exe 'vert 4resize ' . ((&columns * 122 + 146) / 293)
argglobal
balt Sort.h
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 36 - ((29 * winheight(0) + 33) / 66)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 36
normal! 0
wincmd w
argglobal
if bufexists("GUI.c") | buffer GUI.c | else | edit GUI.c | endif
if &buftype ==# 'terminal'
  silent file GUI.c
endif
balt GUI.h
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 446 - ((39 * winheight(0) + 33) / 66)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 446
normal! 0
wincmd w
argglobal
if bufexists("GUI_impl.h") | buffer GUI_impl.h | else | edit GUI_impl.h | endif
if &buftype ==# 'terminal'
  silent file GUI_impl.h
endif
balt GUI.c
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 1 - ((0 * winheight(0) + 16) / 33)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
wincmd w
argglobal
if bufexists("GUI_impl_unix.c") | buffer GUI_impl_unix.c | else | edit GUI_impl_unix.c | endif
if &buftype ==# 'terminal'
  silent file GUI_impl_unix.c
endif
balt GUI_impl.h
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 1 - ((0 * winheight(0) + 16) / 32)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
wincmd w
4wincmd w
exe 'vert 1resize ' . ((&columns * 49 + 146) / 293)
exe 'vert 2resize ' . ((&columns * 120 + 146) / 293)
exe '3resize ' . ((&lines * 33 + 34) / 69)
exe 'vert 3resize ' . ((&columns * 122 + 146) / 293)
exe '4resize ' . ((&lines * 32 + 34) / 69)
exe 'vert 4resize ' . ((&columns * 122 + 146) / 293)
tabnext
edit Sort.h
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 1resize ' . ((&columns * 49 + 146) / 293)
exe '2resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 2resize ' . ((&columns * 242 + 146) / 293)
argglobal
balt Sort.c
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 5 - ((3 * winheight(0) + 32) / 65)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 5
normal! 017|
wincmd w
argglobal
if bufexists("Sort.c") | buffer Sort.c | else | edit Sort.c | endif
if &buftype ==# 'terminal'
  silent file Sort.c
endif
balt csv.h
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 162 - ((33 * winheight(0) + 32) / 65)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 162
normal! 01|
wincmd w
exe '1resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 1resize ' . ((&columns * 49 + 146) / 293)
exe '2resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 2resize ' . ((&columns * 242 + 146) / 293)
tabnext
edit csv.h
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 1resize ' . ((&columns * 75 + 146) / 293)
exe '2resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 2resize ' . ((&columns * 216 + 146) / 293)
argglobal
balt csv.c
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 1 - ((0 * winheight(0) + 32) / 65)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
wincmd w
argglobal
if bufexists("csv.c") | buffer csv.c | else | edit csv.c | endif
if &buftype ==# 'terminal'
  silent file csv.c
endif
balt SortTest.c
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 18 - ((0 * winheight(0) + 32) / 65)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 18
normal! 0
wincmd w
exe '1resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 1resize ' . ((&columns * 75 + 146) / 293)
exe '2resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 2resize ' . ((&columns * 216 + 146) / 293)
tabnext
edit argparse.h
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 1resize ' . ((&columns * 49 + 146) / 293)
exe '2resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 2resize ' . ((&columns * 242 + 146) / 293)
argglobal
balt argparse.c
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 21 - ((16 * winheight(0) + 32) / 65)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 21
normal! 014|
wincmd w
argglobal
if bufexists("argparse.c") | buffer argparse.c | else | edit argparse.c | endif
if &buftype ==# 'terminal'
  silent file argparse.c
endif
balt csv.c
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 258 - ((34 * winheight(0) + 32) / 65)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 258
normal! 09|
wincmd w
exe '1resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 1resize ' . ((&columns * 49 + 146) / 293)
exe '2resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 2resize ' . ((&columns * 242 + 146) / 293)
tabnext
edit SortList.h
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 49 + 146) / 293)
exe 'vert 2resize ' . ((&columns * 243 + 146) / 293)
argglobal
balt SortList.c
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 1 - ((0 * winheight(0) + 33) / 66)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
wincmd w
argglobal
if bufexists("SortList.c") | buffer SortList.c | else | edit SortList.c | endif
if &buftype ==# 'terminal'
  silent file SortList.c
endif
balt List.c
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 1 - ((0 * winheight(0) + 33) / 66)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
wincmd w
exe 'vert 1resize ' . ((&columns * 49 + 146) / 293)
exe 'vert 2resize ' . ((&columns * 243 + 146) / 293)
tabnext
edit List/List.h
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 1resize ' . ((&columns * 75 + 146) / 293)
exe '2resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 2resize ' . ((&columns * 216 + 146) / 293)
argglobal
balt List.c
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 22 - ((17 * winheight(0) + 32) / 65)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 22
normal! 030|
wincmd w
argglobal
if bufexists("List/List.c") | buffer List/List.c | else | edit List/List.c | endif
if &buftype ==# 'terminal'
  silent file List/List.c
endif
balt List.c
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 125 - ((0 * winheight(0) + 32) / 65)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 125
normal! 0
wincmd w
exe '1resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 1resize ' . ((&columns * 75 + 146) / 293)
exe '2resize ' . ((&lines * 65 + 34) / 69)
exe 'vert 2resize ' . ((&columns * 216 + 146) / 293)
tabnext 2
set stal=1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0 && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20 shortmess=filnxtToOFAc
let &winminheight = s:save_winminheight
let &winminwidth = s:save_winminwidth
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
set hlsearch
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
