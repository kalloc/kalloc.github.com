<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head profile="http://gmpg.org/xfn/11">
	<title>{{title}}</title>
	
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<link rel="stylesheet" href="/css/cssf-base.css" type="text/css" media="screen" />
	<!--[if lte IE 6]><link rel="stylesheet" href="/css/cssf-ie6.css" type="text/css" media="screen" /><![endif]-->
	<!--[if IE 7]><link rel="stylesheet" href="css/cssf-ie7.css" type="text/css" media="screen" /><![endif]-->
</head>
<body>

<div id="site">

<div id="header">
	<ul class="menu-h">
		<li><a href="/admin/client/">Клиенты</a></li>
		<li><a href="/admin/source/"><b>Источники</b></a></li>
		<li><a href="/admin/source/add"><b><font color="red">Добавить</font></b></a>
                <li><a href="?exit"><b><font color="grey">выход</font></b></a></li>
	</ul>
</div><!-- #header -->
	

<div id="content" class="content">

       <form action="" method="POST">
{if {ok}}
	    
                <div class="f-row f-actions">
                        <label>ok:</label>
                        <div>{{ok}}</div>
                </div>
{else}
                <fieldset>
                        <legend>Добавление Источника</legend>
                        <div class="f-row">
                                <label>название:</label>
                                <div class="f-inputs"><input class="i-text" type="text" name="name" value="{{name}}"></div>

                        </div>
                        <div class="f-row">
                                <label>количество ссылок:</label>
                                <div class="f-inputs"><input class="i-text" type="text" name="count" value="1"></div>
                        </div>
                        <div class="f-row">
                                <label>User-Agent:</label>
                                <div class="f-inputs">
                                    <input class="i-checkbox" type="checkbox" name="ua_ie" checked>   msie
                                    <input class="i-checkbox" type="checkbox" name="ua_ff" checked>   firefox
                                    <input class="i-checkbox" type="checkbox" name="ua_op" checked>   opera
                                    <input class="i-checkbox" type="checkbox" name="ua_ot" checked>   other
                                </div>
                        </div>

                </fieldset>

                <div class="f-row">

                        <div class="f-inputs"><input value="добавить" class="button" type="submit"></div>
                </div>
{/if}
	    
{if{error}}
                <div class="f-row f-actions">
                        <label>ошибка:</label>
                        <div>{{error}}</div>
                </div>
{/if}
        </form>



</div><!-- #content -->
<div id="footer"></div>
</div><!-- #site -->
</body>
</html>
