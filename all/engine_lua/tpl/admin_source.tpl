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
		<li><a href="/admin/client">Клиенты</a></li>
		<li><a href="/admin/source"><b>Источники</b></a></li>
		<li><a href="?add"><b><font color="red">Добавить</font></b></a>
                <li><a href="?exit"><b><font color="grey">выход</font></b></a></li>
	</ul>
</div><!-- #header -->
	

<div id="col-left">
{if {source}}
	<ul class="menu-v">
{foreach {source} as {row}}
		<li><a href="/admin/source/?id={{row.id}}">{{row.name}} ({{row.count}})</a></li>
{/foreach}
	</ul>
{/if}
	<br /><br />

</div><!-- #col-left -->

<div id="content" class="content">
{if {day}}

   <h1 style="color:grey;text-align:center">источник - {{name}}</h1>
   
   
    {if {stat}}

    <fieldset>

        <legend>Статистика на {{day}}</legend>

        <table>
                <tr>
                        <th></th>
                            <th>всего</th>
                            <th>00-02</th>
                            <th>03-05</th>
                            <th>06-08</th>
                            <th>09-11</th>
                            <th>12-14</th>
                            <th>15-17</th>
                            <th>18-20</th>
                            <th>21-23</th>
                </tr>
{foreach {stat} as {day}}
    		<tr>
                        <th>{{key}}</th>
                        <td><b>{{day.t}}</b></td>
{if {today} == {key}}
    {foreach {day.hours} as {hour}}<td><b>{{hour}}</b></td>{/foreach}
{else}
    {foreach {day.hours} as {hour}}<td>{{hour}}</td>{/foreach}
{/if}
                </tr>
{/foreach}

        </table>


    </fieldset>
{if {hash}}
        <fieldset>
                <legend>Ссылки для источника</legend>
    {foreach {hash} as {row}}
                <div class="f-row">
		[<a href="?id={{id}}&del={{row.id}}" onclick="return confirm('what do you wont?');">x</a>]
		http://engine.adct.ru/{{row.hash}}.html 
		[<a href="http://engine.adct.ru/{{row.hash}}_full.html">iframe</a>]
		</div>
		
    {/foreach}
        </fieldset>

{/if}

    {if {traf}}
       <form action="" method="POST">
	{if {ok}}

                <div class="f-row f-actions">
                        <label>ok:</label>
                        <div>{{ok}}</div>
                </div>
	{/if}
                <fieldset>
                        <legend>Редактирование трафика от источника(в тысячах)</legend>
                        <div class="f-row">
                                <label>количество:<br/>формат: 200Х-ХХ-ХХ Х</label>
                                <div class="f-inputs">
                                        <textarea cols="15" rows="20" name="traf">{{traf}}</textarea>
                                </div>
                        </div>
                        <div class="f-row">
                                <label>добавить ссылок:</label>
                                <div class="f-inputs"><input class="i-text" type="text" name="count" value="0"></div>
                        </div>

                        <div class="f-row">
                                <label>активность:</label>
                                <div class="f-inputs"><input class="i-checkbox" type="checkbox" name="status" {if {checked}}checked{/if}></div>
                        </div>


                        <div class="f-row">
                                <label>User-Agent:</label>
                                <div class="f-inputs">
				    <input class="i-checkbox" type="checkbox" name="ua_ie" {if {ie_checked}}checked{/if}>   msie
				    <input class="i-checkbox" type="checkbox" name="ua_ff" {if {ff_checked}}checked{/if}>   firefox 
				    <input class="i-checkbox" type="checkbox" name="ua_op" {if {op_checked}}checked{/if}>   opera 
				    <input class="i-checkbox" type="checkbox" name="ua_ot" {if {ot_checked}}checked{/if}>   other 
				</div>
                        </div>

                </fieldset>

                <div class="f-row">
                        <div class="f-inputs"><input value="изменить" class="button" type="submit"></div>
                </div>

	{if{error}}
                <div class="f-row f-actions">
                        <label>ошибка:</label>
                        <div color="red">{{error}}</div>
                </div>
	{/if}
        </form>

    {/if}


{/if}



{else}

    <fieldset>

        <legend>глобальлная статистика по всем источник </legend>

        <table>
                <tr>
                        <th>источник</th>
                        <th>трафик сегодня</th>
                        <th>хитов сегодня</th>
                        <th>fail сегодня</th>
                        <th>трафик сегодня должен быть</th>
                        <th></th>
                        <th>трафик всего</th>
                </tr>
	{foreach {stat} as {source}}

               <tr>
                       <th  style="text-align:left;padding-left:10px"><a href="/admin/source/?id={{key}}">{{source.name}}</a></th>
                       <td  style="text-align:center;"><b>{if {source.today}}{{source.today}}{else}0{/if}</b></td>
                       <td  style="text-align:center;">{{source.hit}}</td>
                       <td  style="text-align:center;">{{source.fail}}</td>
                       <td  style="text-align:center;">{{source.total}}</td>
                       <td  style="text-align:center;">&nbsp;</td>
                       <td  style="text-align:center;"><b>{{source.all}}</b></td>
               </tr>
															       
	{/foreach}

        </table>


    </fieldset>


{/if}

</div><!-- #content -->
<div id="footer"></div>

</div><!-- #site -->
</body>
</html>