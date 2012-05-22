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
<div id="header" class="content">

	<form action="" method="POST">
		<fieldset>
			<legend>вы можете представиться, но не факт что мы вас впустим</legend>
			<div class="f-row">
				<label>логин:</label>
				<div class="f-inputs"><input class="i-text" type="text" name="login" value="{{login}}"></div>

			</div>
			<div class="f-row">
				<label>пароль:</label>
				<div class="f-inputs"><input class="i-text" type="password" name="password"></div>
			</div>
		</fieldset>
		
		<div class="f-row">

			<div class="f-inputs"><input value="Войти" class="button" type="submit"></div>
		</div>

	    {if{error}}
		<div class="f-row f-actions">
			<label>ошибка:</label>
			<div>{{error}}</div>
		</div>
	    {/if}
	</form>
</div>
</div>
</body>
</html>
