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
		<li><a href="/admin/client/?add"><b><font color="red">Добавить</font></b></a></li>
		<li><a href="?exit"><b><font color="grey">выход</font></b></a></li>
	</ul>
</div><!-- #header -->
	

<div id="col-left">
{if {client}}
{foreach {client} as {site}}
    <ul class="menu-v">
	<li>{{key}}
	    <ul>
{foreach {site} as {row}}
{if {row.uri} == {""}}
		<li><a href="/admin/client/?id={{row.id}}">корень</a></li>
{else}
		<li><a href="/admin/client/?id={{row.id}}">{{row.uri}}</a></li>
{/if}
{/foreach}
	    </ul>
	</li>
    </ul>
{/foreach}
{/if}
	<br /><br />
</div><!-- #col-left -->

<div id="content" class="content">



{if {url}}

        <h1 style="color:grey;text-align:center">клиент - {{url}} <a href="?id={{id}}&remove" onclick="return confirm('вы уверены что не будет пиздеца если вы удалите этого клиента со всем его трафиком')">[удалить]</a></h1>
	

    <fieldset>

	<legend>Статистика на {{day}}</legend>

	<table>
		<tr>
			<th></th>
			    <td>всего</td>
			    <td>00-02</td>
			    <td>03-05</td>
			    <td>06-08</td>
			    <td>09-11</td>
			    <td>12-14</td>
			    <td>15-17</td>
			    <td>18-20</td>
			    <td>21-23</td>
		</tr>
{foreach {stat} as {day}}
		<tr>
			<td>{{key}}</td>
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

{else}

    <fieldset>

	<legend>глобальная статистика по всем клиента </legend>

	<table>
		<tr>
			<th>клиент</th>
			<th>трафик сегодня</th>
			<th>трафик сегодня должен быть</th>
			<th>трафик всего</th>
		</tr>
{foreach {stat} as {client}}
		<tr>
			<th  style="text-align:left;padding-left:10px"><a href="/admin/client/?id={{key}}">{{client.name}}</a></th>
			<td  style="text-align:center;"><b>{if {client.today}}{{client.today}}{else}0{/if}</b></td>
			<td  style="text-align:center;">{{client.total}}</td>
			<td  style="text-align:center;"><b>{{client.all}}</b></td>
		</tr>
{/foreach}

	</table>


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
                        <legend>Редактирование трафика к клиенту(в тысячах)</legend>
                        <div class="f-row">
                                <label>количество:<br/>формат: 200Х-ХХ-ХХ Х</label>
                                <div class="f-inputs">
                                        <textarea cols="15" rows="20" name="traf">{{traf}}</textarea>
                                </div>
                        </div>

                       <div class="f-row">
                                <label>активность:</label>
                                <div class="f-inputs"><input class="i-checkbox" type="checkbox" name="status" {if {checked}}checked{/if}></div>
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
</div><!-- #content -->

<div id="footer"></div>

</div><!-- #site -->
</body>
</html>
