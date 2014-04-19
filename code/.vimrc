set nocompatible
if filereadable(glob("~/.vimrc_local"))
    source ~/.vimrc_local
endif
if exists("use_go")
    set rtp+=$GOROOT/misc/vim
endif
set nobackup
set history=50
set showcmd
set ruler
set hlsearch
set incsearch
set smartcase
set cmdheight=1
set backspace=indent,eol,start
" when ':set list' you see <tab> and trails more usable
let g:netrw_list_hide='\.pyc$,\.o$,^\.svn$,^\.swp$,^\.git$' 
set acd
set completeopt=menuone,menu,longest
set listchars=tab:>-,trail:-
set list
"set nu
set sw=4 ts=4 sta et
au FileType crontab,fstab,make set noet ts=8 sw=8
au BufEnter *.sh set ai sw=4 ts=4 sta et tw=80


set wildmenu
set wcm=<Tab>
 
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
 
 
" STATUS STRING
set wildmenu
set laststatus=2
set statusline=%<%f\ [%Y%R%W]%1*%{(&modified)?'\ [+]\ ':''}%*%=%c%V,%l\ %P\ [%n]

"Charset choice
menu Encoding.utf-8        :e ++enc=utf-8 <CR>
menu Encoding.windows-1251 :e ++enc=cp1251<CR>
menu Encoding.ibm-866      :e ++enc=ibm866<CR>
menu Encoding.koi8-r   :e ++enc=koi8-r<CR>
set fileencodings=utf-8,cp1251,koi8-r,cp866
map <F9> :emenu Encoding.<TAB>


"Tab
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
 
" мапинг для screen / tmux 
map <silent><ESC>[1;3D <A-LEFT>
map <silent><ESC>[1;3C <A-RIGHT>
map <silent><ESC>[1;4D <A-S-LEFT>
map <silent><ESC>[1;4C <A-S-RIGHT>
map <silent><ESC>[1;3A <A-UP>
map <silent><ESC>[1;3B <A-DOWN>
map <silent><ESC>t <A-t>
map <silent><ESC>w <A-w>

" новая вкладка
nnoremap <A-t> :tabnew<CR>
" закрыть вкладка
nnoremap <A-w> :tabclose<CR>

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




" inoremap <C-t> <Esc>:tabnew<CR>


set pastetoggle=<F3>
" On/off line numbers by F11
imap <F11> <Esc>:set<Space>nu!<CR>a
nmap <F11> :set<Space>nu!<CR>
 



if filereadable(glob("tmux"))
    autocmd VimEnter,VimLeave * silent !tmux set status
endif



"Vundle
set rtp+=~/.vim/bundle/vundle/
call vundle#rc()
Bundle 'gmarik/vundle'

Bundle 'terryma/vim-multiple-cursors'

" original repos on github
Bundle 'tpope/vim-pathogen'
call pathogen#infect()
syntax enable
filetype plugin indent on

Bundle 'tpope/vim-fugitive'
Bundle "vim-scripts/tlib"
Bundle "tselectbuffer"
Bundle "MarcWeber/vim-addon-mw-utils"
Bundle "snipmate-snippets"
"Bundle 'garbas/vim-snipmate'

"NerdTree
map <F8> :NERDTreeToggle<cr>
vmap <F8> <esc>:NERDTreeToggle<cr>
imap <F8> <esc>:NERDTreeToggle<cr>
Bundle "scrooloose/nerdtree"
let NERDTreeWinPos = 'right'
let NERDTreeShowBookmarks = 1
let NERDTreeIgnore = ['\~$', '*.pyc', '*.pyo', '.git']
let NERDChristmasTree = 0


Bundle 'Lokaltog/vim-easymotion'
Bundle 'rstacruz/sparkup', {'rtp': 'vim/'}
Bundle "vim-scripts/dbext.vim"

Bundle 'L9'
Bundle 'FuzzyFinder'
"Snippets

" IDE
Bundle "mileszs/ack.vim"
let g:ackprg="ack -H --nocolor --nogroup --column"
Bundle "git://github.com/godlygeek/tabular.git"
Bundle "git://github.com/scrooloose/syntastic"
set statusline+=%#warningmsg#
set statusline+=%{SyntasticStatuslineFlag()}
set statusline+=%*
let g:syntastic_check_on_open=1
let g:syntastic_enable_signs=1
let g:syntastic_enable_highlighting=1
let g:syntastic_error_symbol='✗'
let g:syntastic_warning_symbol='⚠'


" Source Explorer  http://www.vim.org/scripts/script.php?script_id=2179
Bundle "Source-Explorer-srcexpl.vim"
nmap <F7> :SrcExplToggle<CR> 
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


" taglist
Bundle "taglist.vim"
nnoremap <silent> <F6> :TlistToggle<CR>

"ctrlp
Bundle "kien/ctrlp.vim"

"NeoComplete
Bundle "Shougo/neocomplcache"
let g:acp_enableAtStartup = 0 
let g:neocomplcache_snippets_disable_runtime_snippets=1
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
let g:neocomplcache_force_overwrite_completefunc=1


"Bundle "https://github.com/motemen/git-vim"

" Go
if exists("use_go")
    Bundle "jnwhiteh/vim-golang.git"
endif

" Lua
if exists("use_lua")
    Bundle 'vim-scripts/lua.vim'
    Bundle 'rkowal/Lua-Omni-Vim-Completion'
    let g:lua_complete_omni = 1
endif
" PHP
if exists("use_php")
    Bundle 'git://github.com/vim-scripts/php.vim--Garvin.git'
    Bundle 'git://github.com/2072/PHP-Indenting-for-VIm.git'
endif 

" Python/Django
if exists("use_python")
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
let g:jedi#show_call_signatures=0
let g:jedi#popup_select_first=0
let g:jedi#popup_on_dot=0
let g:jedi#auto_vim_configuration=0
call LoadRope()
let ropevim_codeassist_maxfixes=10
let ropevim_vim_completion=1
let ropevim_guess_project=1
let ropevim_enable_autoimport=1
let ropevim_enable_shortcuts=1
let ropevim_extended_complete=1
endif
    au BufEnter *.py set ai sw=4 ts=4 sts=4 sta et fo=croql tw=80
    au FileType python set omnifunc=pythoncomplete#Complete
    autocmd FileType python setlocal omnifunc=pythoncomplete#Complete 
    autocmd FileType python map <buffer> <F10> :call Flake8()<CR>
    "Bundle 'git://github.com/fs111/pydoc.vim.git'
    let g:syntastic_python_checker="flake8"
    Bundle "vim-django-support"
    Bundle "vim-python-virtualenv"
    "Bundle "klen/python-mode"
    Bundle "https://github.com/davidhalter/jedi-vim.git"
    Bundle "https://github.com/nvie/vim-flake8.git"
    let g:jedi#popup_on_dot = 1
    let g:jedi#show_call_signatures = 1
    let g:jedi#popup_select_first = 0
    "Bundle 'vim-scripts/pep8'
    let g:pep8_map='<leader>8'
    if !exists("syntastic_python_checker_args")
        let g:syntastic_python_checker_args='--ignore=E711'
    endif
    if !exists("pep8_ignore")
        let g:pep8_ignore="E711"
    endif
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
endif

" Scala
if exists("g:use_scala")
    au BufRead,BufNewFile *.scala set filetype=scala
    Bundle 'akhil/scala-vim-bundle'
    Bundle 'https://github.com/derekwyatt/vim-scala.git'
endif

" Perl
if exists("g:use_perl")
    Bundle 'git://github.com/petdance/vim-perl.git'
    Bundle 'git://github.com/ggray/vim-tt2.git'
endif

"JS
if exists("g:use_js")
    autocmd FileType javascript setlocal omnifunc=javascriptcomplete#CompleteJS 
    au FileType javascript set dictionary+=$HOME/.vim/dict/node.dict
    Bundle 'git://github.com/guileen/vim-node.git'
    Bundle 'git://github.com/myhere/vim-nodejs-complete.git'
    Bundle 'git://github.com/pangloss/vim-javascript.git'
    Bundle 'git://github.com/briancollins/vim-jst.git'
    let g:html_indent_inctags = "html,body,head,tbody"
    let g:html_indent_script1 = "inc"
    let g:html_indent_style1 = "inc"
    let g:node_usejscomplete = 1
    au BufEnter *.js set ai sw=2 ts=2 sta et fo=croql tw=80
    au BufRead,BufNewFile *.ejs set filetype=jst
endif

"XML
if exists("g:use_xml")
    autocmd FileType xml setlocal omnifunc=xmlcomplete#CompleteTags 
endif

" HTML/HAML
if exists("use_html")
    autocmd FileType css setlocal omnifunc=csscomplete#CompleteCSS 
    autocmd FileType html,markdown setlocal omnifunc=htmlcomplete#CompleteTags 
    au BufEnter *.html set ai sw=2 ts=2 sta et fo=croql tw=100
    au BufEnter *.mint set filetype=mint ai sw=2 ts=2 sta et fo=croql tw=80
    au BufEnter *.jinja2 set filetype=htmljinja ai sw=2 ts=2 sta et fo=croql tw=80    
    Bundle 'git://github.com/othree/html5.vim.git'
    Bundle 'https://github.com/estin/htmljinja'                                       
    Bundle 'git://github.com/hokaccha/vim-html5validator.git'
    ""Bundle 'git://github.com/tyru/operator-html-escape.vim.git'
    Bundle 'git://github.com/tpope/vim-haml.git'
    Bundle 'git://github.com/gregsexton/MatchTag.git'
endif

" LXC
if exists("use_lxc")
    Bundle 'https://github.com/honza/dockerfile.vim.git'
endif

"CPP
if exists("use_c")
    autocmd FileType cpp map <F10> :call g:ClangUpdateQuickFix()<cr>
    autocmd FileType cpp vmap <F10> <esc>:call g:ClangUpdateQuickFix()<cr>
    autocmd FileType imap <F10> <esc>:call g:ClangUpdateQuickFix()<cr>
    Bundle "Shougo/neocomplcache-clang"
    Bundle "Rip-Rip/clang_complete"
    Bundle "osyo-manga/neocomplcache-clang_complete"
    let g:clang_complete_auto=0
    let g:clang_complete_copen=1
    let g:clang_hl_errors=1
    let g:clang_use_library=1
endif

"Erlang
if exists("use_erlang")
    Bundle "elixir-lang/vim-elixir"
    Bundle "mattonrails/vim-mix"
    Bundle "jimenezrick/vimerl"
    Bundle "aerosol/vim-erlang-skeletons"
    let g:erl_author="Nikita Kuznetsov"
    let g:erl_company="ACME inc."
endif

if exists("use_ruby")
"Rails
    autocmd FileType ruby,eruby set omnifunc=rubycomplete#Complete
    autocmd FileType ruby,eruby let g:rubycomplete_buffer_loading = 1
    autocmd FileType ruby,eruby let g:rubycomplete_rails = 1
    autocmd FileType ruby,eruby let g:rubycomplete_classes_in_global = 1
    Bundle "tpope/vim-rails"
    Bundle "vim-scripts/rubycomplete.vim"
endif


Bundle 'ervandew/supertab'
let g:SuperTabDefaultCompletionType = "<c-x><c-u>"
