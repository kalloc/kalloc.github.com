<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head profile="http://gmpg.org/xfn/11">
	<title>{{title}} :: {{site}} (Loading)</title>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<link rel="stylesheet" href="/css/game.css" type="text/css" media="screen" />
	<!--[if lte IE 6]><link rel="stylesheet" href="/css/game-ie6.css" type="text/css" media="screen" /><![endif]-->
	<!--[if IE 7]><link rel="stylesheet" href="/css/game-ie7.css" type="text/css" media="screen" /><![endif]-->
	<script type="text/javascript">
		var js ="{{js}}";
		var init = "init_index";
		var resize = "resize_index";
	</script>
        <!--[if IE]>
                <script type="text/javascript" src="/js/.3dparty/excanvas-compressed.js"></script>
        <![endif]-->
                                
                                
	<script src="/js/.3dparty/mootools.js" type="text/javascript"></script>
	<script src="/js/.3dparty/mootools-more.js" type="text/javascript"></script>
	<script src="/js/.3dparty/mocha.js" type="text/javascript"></script>
	<script src="/js/script.js" type="text/javascript"></script>
<style type="text/css">
#load {
    right:40%;
    top:40%;
    left:40%;
    bottom:45%;
    z-index:2;
    width:200px;
    text-align:center;
    position:absolute;
    padding:0px;
}

body {
    font-family:verdana;
    font-size:10pt;
}
</style>


</head>

<body id="content">

    <div id="form" style="display:none">
        <form id="login_form" action="" method="post">
    	    <div style="margin:20px;text-align:center">
		Желаемое имя пользователя:<br/><input id="login_input" type="text" name="login" style="border:1px solid #7f7f7f" />
	    </div>
	    <div id="login_error" style="color:red;visibility:hidden;font-size:7pt;text-align:right;position:absolute;right:20px;bottom:40px">		
		*неверный формат*
	    </div>
	</form>
    </div>


    <div id="load">
	<img src="/img/load.gif" /><br/><br/>
	загрузка {{title}}...
    </div>
</body>
</html>
