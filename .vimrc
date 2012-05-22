set nocompatible
set nobackup
set history=50
set showcmd
set ruler
set hlsearch
set incsearch
set smartcase
set cmdheight=1
syntax enable
filetype plugin indent on
" set fencs=utf-8,cp1251,koi8-r,ucs-2,cp866
" set encoding=utf-8
" set foldmethod=indent

set wildmenu
set wcm=<Tab>
menu Encoding.koi8-r   :e ++enc=koi8-r<CR>
menu Encoding.windows-1251 :e ++enc=cp1251<CR>
menu Encoding.ibm-866      :e ++enc=ibm866<CR>
menu Encoding.utf-8        :e ++enc=utf-8 <CR>

set fileencodings=utf-8,koi8-r,cp866,cp1251

 
" автоматическое закрытие скобок
" inoremap { {}<LEFT>
" inoremap ( ()<LEFT>
" inoremap [ []<LEFT>
 
if has('gui_running')
    colo wombat
    set guifont=Liberation15px
    set guioptions-=T
    set guioptions-=t
    set mouse=a
    set mousemodel=popup
else
    colo slate
endif
 
" when ':set list' you see <tab> and trails more usable
set listchars=tab:>-,trail:-
set list
set nu
set sw=4 ts=4 sta et
au FileType crontab,fstab,make set noet ts=8 sw=8
au BufEnter *.sh set ai sw=4 ts=4 sta et tw=80
au BufEnter *.py set ai sw=4 ts=4 sts=4 sta et fo=croql tw=80
au BufEnter *.js set ai sw=2 ts=2 sta et fo=croql tw=80
au BufEnter *.html set ai sw=2 ts=2 sta et fo=croql tw=100
au BufEnter *.mint set filetype=mint ai sw=2 ts=2 sta et fo=croql tw=80
au BufRead,BufNewFile *.scala set filetype=scala

 
" STATUS STRING
set wildmenu
set laststatus=2
set statusline=%<%f\ [%Y%R%W]%1*%{(&modified)?'\ [+]\ ':''}%*%=%c%V,%l\ %P\ [%n]
 
 
" новая вкладка
nnoremap <C-T> :tabnew<CR>
" предыдущая вкладка
nnoremap <silent><A-LEFT> :call TabJump('left')<CR>
" следующая вкладка
nnoremap <silent><A-RIGHT> :call TabJump('right')<CR>
" первая вкладка
nnoremap <A-UP> :tabfirst<CR>
" последняя вкладка
nnoremap <A-DOWN> :tablast<CR>
" переместить вкладку назад
nnoremap <silent><A-S-LEFT> :call TabMove('left')<CR>
" переместить вкладку вперёд
nnoremap <silent><A-S-RIGHT> :call TabMove('right')<CR>
 
set pastetoggle=<F3>
nnoremap <silent> <F6> :TlistToggle<CR>
nmap <F7> :SrcExplToggle<CR> 
" Обозреватель файлов (plugin-NERD_Tree)
map <F8> :NERDTreeToggle<cr>
vmap <F8> <esc>:NERDTreeToggle<cr>
imap <F8> <esc>:NERDTreeToggle<cr>
 
map <F9> :emenu Encoding.<TAB>
" On/off line numbers by F11
imap <F11> <Esc>:set<Space>nu!<CR>a
nmap <F11> :set<Space>nu!<CR>
 
 
function! TabJump(direction)
    let l:tablen=tabpagenr('$')
    let l:tabcur=tabpagenr()
    if a:direction=='left'
        if l:tabcur>1
            execute 'tabprevious'
        endif
    else
        if l:tabcur!=l:tablen
            execute 'tabnext'
        endif
    endif
endfunction
 
function! TabMove(direction)
    let l:tablen=tabpagenr('$')
    let l:tabcur=tabpagenr()
    if a:direction=='left'
        if l:tabcur>1
            execute 'tabmove' l:tabcur-2
        endif
    else
        if l:tabcur!=l:tablen
            execute 'tabmove' l:tabcur
        endif
    endif
endfunction

let g:netrw_list_hide='\.pyc$,\.o$,^\.svn$,^\.swp$,^\.git$' 
set acd

if has("python")
python << EOF
import os.path
import sys
import vim
if 'VIRTUAL_ENV' in os.environ:
    project_base_dir = os.environ['VIRTUAL_ENV']
    sys.path.insert(0, project_base_dir)
    activate_this = os.path.join(project_base_dir, 'bin/activate_this.py')
    execfile(activate_this, dict(__file__=activate_this))
EOF
function! LoadRope()
python << EOF
import ropevim
EOF
endfunction
call LoadRope()
let ropevim_codeassist_maxfixes=10
let ropevim_vim_completion=1
let ropevim_guess_project=1
let ropevim_enable_autoimport=1
let ropevim_enable_shortcuts=1
let ropevim_extended_complete=1
endif


let NERDTreeWinPos = 'right'
let NERDTreeShowBookmarks = 1
let NERDTreeIgnore = ['\~$', '*.pyc', '*.pyo', '.git']
let NERDChristmasTree = 0

set backspace=indent,eol,start



" Source Explorer  http://www.vim.org/scripts/script.php?script_id=2179
let g:SrcExpl_winHeight = 8 
let g:SrcExpl_refreshTime = 100 
" let g:SrcExpl_jumpKey = "<ENTER>" 
let g:SrcExpl_gobackKey = "<SPACE>" 
let g:SrcExpl_pluginList = [ 
        \ "__Tag_List__", 
        \ "_NERD_tree_", 
        \ "Source_Explorer" 
    \ ] 

let g:SrcExpl_searchLocalDef = 1 
let g:SrcExpl_isUpdateTags = 0 
let g:SrcExpl_updateTagsCmd = "ctags --sort=foldcase -R ." 
let g:SrcExpl_updateTagsKey = "<F12>" 



" neocomplete
let g:acp_enableAtStartup = 0 
let g:neocomplcache_disable_auto_complete = 1
let g:neocomplcache_enable_at_startup = 1 
" Use smartcase. 
let g:neocomplcache_enable_smart_case = 1 
" Use camel case completion. 
let g:neocomplcache_enable_camel_case_completion = 1 
" Use underbar completion. 
let g:neocomplcache_enable_underbar_completion = 1 
" Set minimum syntax keyword length. 
let g:neocomplcache_min_syntax_length = 2 
let g:neocomplcache_lock_buffer_name_pattern = '\*ku\*' 
autocmd FileType css setlocal omnifunc=csscomplete#CompleteCSS 
autocmd FileType html,markdown setlocal omnifunc=htmlcomplete#CompleteTags 
autocmd FileType javascript setlocal omnifunc=javascriptcomplete#CompleteJS 
autocmd FileType python setlocal omnifunc=pythoncomplete#Complete 
autocmd FileType xml setlocal omnifunc=xmlcomplete#CompleteTags 
autocmd FileType ruby,eruby set omnifunc=rubycomplete#Complete
autocmd FileType ruby,eruby let g:rubycomplete_buffer_loading = 1
autocmd FileType ruby,eruby let g:rubycomplete_rails = 1
autocmd FileType ruby,eruby let g:rubycomplete_classes_in_global = 1
au FileType python set omnifunc=pythoncomplete#Complete
autocmd FileType python map <buffer> <F10> :call Flake8()<CR>
autocmd FileType cpp map <F10> :call g:ClangUpdateQuickFix()<cr>
autocmd FileType cpp vmap <F10> <esc>:call g:ClangUpdateQuickFix()<cr>
autocmd FileType imap <F10> <esc>:call g:ClangUpdateQuickFix()<cr>

let g:neocomplcache_force_overwrite_completefunc=1
set completeopt=menuone,menu,longest
"pep8
let g:pep8_map='<leader>8'

" taglist
set rtp+=~/.vim/bundle/vundle/
call vundle#rc()


if filereadable($VIRTUAL_ENV . '/.vimrc')
        source $VIRTUAL_ENV/.vimrc
endif
"let $DJANGO_SETTINGS_MODULE='mysite.settings'
":python import sys; print sys.executable
"py << EOF
"import os.path 
"import sys 
"import vim
"# SET THIS MANUALLY
"# =================
"DJANGO_SETTINGS_MODULE='foo.settings'
"project_base_dir = os.environ['VIRTUAL_ENV']
"sys.path.insert(0, project_base_dir)
"activate_this = os.path.join(project_base_dir, 'bin/activate_this.py') 
"execfile(activate_this, dict(__file__=activate_this))
"os.environ['DJANGO_SETTINGS_MODULE'] = DJANGO_SETTINGS_MODULE 
"EOF 
"python activate_this = '/path/to/virtualenv/bin/activate_this.py'
"python execfile(activate_this, dict(__file__=activate_this))
"

" let Vundle manage Vundle
" required! 
Bundle 'gmarik/vundle'

" My Bundles here:
"
" original repos on github
Bundle 'tpope/vim-fugitive'
Bundle 'tpope/vim-pathogen'
call pathogen#infect()
Bundle "vim-scripts/tlib"
Bundle "tselectbuffer"
Bundle "MarcWeber/vim-addon-mw-utils"
Bundle "snipmate-snippets"
Bundle 'garbas/vim-snipmate'
Bundle "scrooloose/nerdtree"
Bundle 'Lokaltog/vim-easymotion'
Bundle 'rstacruz/sparkup', {'rtp': 'vim/'}
Bundle 'ervandew/supertab'
Bundle "vim-scripts/dbext.vim"
let g:SuperTabDefaultCompletionType = "<c-x><c-u>"

" vim-scripts repos
Bundle 'L9'
Bundle 'FuzzyFinder'
" Lua
" Bundle 'vim-scripts/lua.vim'
" Bundle 'rkowal/Lua-Omni-Vim-Completion'
" Bundle 'xolox/vim-lua-ftplugin'
" Bundle 'xolox/vim-lua-inspect'
"Snippets
Bundle "rygwdn/ultisnips"

" IDE
Bundle "mileszs/ack.vim"
Bundle "Source-Explorer-srcexpl.vim"
Bundle "taglist.vim"
Bundle "kien/ctrlp.vim"
Bundle "Shougo/neocomplcache"
Bundle "xolox/vim-easytags"

" PHP
Bundle 'git://github.com/vim-scripts/php.vim--Garvin.git'
Bundle 'git://github.com/2072/PHP-Indenting-for-VIm.git'

" Python/Django
Bundle 'git://github.com/fs111/pydoc.vim.git'
Bundle "pyflakes"
"Bundle "klen/python-mode"
Bundle "https://github.com/nvie/vim-flake8.git"
"Bundle 'vim-scripts/pep8'

" Scala
Bundle 'akhil/scala-vim-bundle'
Bundle 'bdd/vim-scala'

" Perl
"Bundle 'git://github.com/petdance/vim-perl.git'
"Bundle 'git://github.com/ggray/vim-tt2.git'

" HTML/HAML
Bundle 'git://github.com/othree/html5.vim.git'
Bundle 'git://github.com/hokaccha/vim-html5validator.git'
""Bundle 'git://github.com/tyru/operator-html-escape.vim.git'
Bundle 'git://github.com/tpope/vim-haml.git'
Bundle 'git://github.com/gregsexton/MatchTag.git'

"CPP
"Bundle "Shougo/neocomplcache-clang"
"Bundle "Rip-Rip/clang_complete"
"Bundle "osyo-manga/neocomplcache-clang_complete"
"let g:clang_complete_auto=0
"let g:clang_complete_copen=1
"let g:clang_hl_errors=1
"let g:clang_use_library=1

"Erlang
"Bundle "jimenezrick/vimerl"
"Bundle "aerosol/vim-erlang-skeletons"
"let g:erl_author="Nikita Kuznetsov"
"let g:erl_company="ACME inc."
"Rails
"Bundle "tpope/vim-rails"
"Bundle "vim-scripts/rubycomplete.vim"




"Solarized
"Bundle "altercation/vim-colors-solarized"
"set background=dark
"let g:solarized_termcolors=256
"colorscheme solarized
" non github repos
