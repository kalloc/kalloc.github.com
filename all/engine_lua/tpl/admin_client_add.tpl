<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head profile="http://gmpg.org/xfn/11">
	<title>{{title}}</title>
	
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<link rel="stylesheet" href="/css/cssf-base.css" type="text/css" media="screen" />
	<!--[if lte IE 6]><link rel="stylesheet" href="/css/cssf-ie6.css" type="text/css" media="screen" /><![endif]-->
	<!--[if IE 7]><link rel="stylesheet" href="/css/cssf-ie7.css" type="text/css" media="screen" /><![endif]-->
</head>
<body>

<div id="site">

<div id="header">
	<ul class="menu-h">
		<li><a href="/admin/client/"><b>Клиенты</b></a></li>
		<li><a href="/admin/source/">Источники</a></li>
		<li><a href="?add"><b><font color="red">Добавить</font></b></a></li>
                <li><a href="?exit"><b><font color="grey">выход</font></b></a></li>
	</ul>
</div><!-- #header -->
	

<div id="col-left">
</div><!-- #col-left -->

<div id="content" class="content">
	


       <form action="" method="POST">
{if {ok}}
	    
                <div class="f-row f-actions">
                        <label>ok:</label>
                        <div>{{ok}}</div>
                </div>
{else}
                <fieldset>
                        <legend>Добавление клиента и трафика (в тысячах)</legend>
                        <div class="f-row">
                                <label>сайт:</label>
                            	<div class="f-inputs">
				    <input class="i-text" type="text" name="url" value="{{url}}" size="50">
				</div>

                        </div>
                        <div class="f-row">
                                <label>количество:<br/>формат: 200Х-ХХ-ХХ Х</label>
                                <div class="f-inputs">
					<textarea cols="15" rows="20" name="traf"></textarea>
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
                        <div color="red">{{error}}</div>
                </div>
{/if}
        </form>











</div><!-- #content -->

<div id="footer"></div>

</div><!-- #site -->
</body>
</html>