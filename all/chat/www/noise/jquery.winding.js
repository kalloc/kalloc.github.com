jQuery.winding = function()
{
	// so that firebug console.log statements don't break IE
	var config = {
	width : 200,
	height : 200,
	top: 0,
	left: 0,
	minWidth: 200,
	minHeight: 200,
	btnClose : false,
	btnMin : false,
	btnMax : false,
	fldStatus : false,
	rsx : false,
	rsy : false,
	rsxy : false,
	content: '',
	loadIcon: '/images/0.gif',
	scriptRegexp: '(?:<script.*?>)((\n|\r|.)*?)(?:<\/script>)',
	title : 'default title',
	status: '',
	zIndex: 1000,
	name: '',
	template: '<div class="#{classname}" id="window_#{id}" style="top:#{top}px;left:#{left}px;width:#{width}px;height:#{height}px"><div class="windowborder"></div><div class="top" ><div class="l_t_cor" id="ul#{id}"></div><div class="r_t_cor" id="ur#{id}"></div><div class="top_block" id="u#{id}"></26.03.2007div></div><div class="middle"><div class="left_block" id="l#{id}"></div><div class="right_block" id="r#{id}"></div><div class="center_block"><div class="header_block" id="header_block_#{id}"><div class="icon"><img src="/images/icons/#{icon}" id="icon_#{id}" align=absmiddle></div><div class="block" id="title_#{id}">#{title}</div></div><div class="mainblock" id="mainblock_#{id}" style="height:#{height2}px;"><div class="content" id="content_#{id}">#{content}</div></div><div class="footer_block"><div class="text">#{status}</div><div class="devider">&nbsp;</div>#{statusExtend}</div></div></div><div class="bottom"><div class="l_b_cor" id="dl#{id}"></div><div class="r_b_cor" id="dr#{id}"></div><div class="bottom_block" id="d#{id}"></div></div></div>'
	};
	var winding = {
		winlist: [],
		winMax: 0,
		defname: 'default_name'
	};
	return {
		setup: function(settings)
		{
			config = $.extend(settings,config);
			$(document.body).append("<div id='temp_block'></div>");
		},
		_find: function(name)
		{
 			for (var win in winding.winlist)
			{
//				console.log(win+" "+winding.winlist[win].settings.name);
				if (winding.winlist[win].settings)
				{
					if (winding.winlist[win].settings.name == name)
					{
						return win;
						//break;
					}
				}
			}
			//return null;
		},
		parseLinks: function(name)
		{
			$("#content_"+name+" a").each(function()
				{
					var url = $(this).attr("href");
					url= url.replace('http://'+document.domain,"");
					if (url.match(/html/))
					{
						if (!url.match("#file"))
						{
							var newUrl = "#file::"+url;
							$(this).attr("href",newUrl);
						}
						$(this).bind("click", function(){
							var oldtitle = $("#title_"+name).html();
							var oldurl = url;
							if (oldurl.substr(0,1)=="/")
							{
								oldurl=oldurl.substr(1,oldurl.length-1);
							}
							$("#title_"+name).html(oldtitle+" loading ("+oldurl+")");
							var oldIconSrc = $("#icon_"+name).attr("src");
							$("#icon_"+name).attr("src",config.loadIcon);
							$("#icon_"+name).addClass("loading");
							$("#content_"+name).load(url+"?content="+name,function() { $("#title_"+name).html(oldtitle); $("#icon_"+name).attr("src",oldIconSrc); $("#icon_"+name).removeClass("loading"); jQuery.winding.parseLinks(name); });
						    //$("#content_"+name).load()
						});
					}
					if (url.match(/bml/))
					{
						if (!url.match("#file"))
						{
							var newUrl = "#file::"+url;
							$(this).attr("href",newUrl);
						}
						$(this).bind("click", function(){
							var oldtitle = $("#title_"+name).html();
							//alert(oldtitle);
							var oldurl = url;
							if (oldurl.substr(0,1)=="/")
							{
								oldurl=oldurl.substr(1,oldurl.length-1);
							}
							$("#title_"+name).html(oldtitle+" loading ("+oldurl+")");
							var oldIconSrc = $("#icon_"+name).attr("src");
//							alert(config.loadIcon);
							$("#icon_"+name).attr("src",config.loadIcon);
//							alert($("#icon_"+name).attr("src"));
							$("#icon_"+name).addClass("loading");
							var rnd = Math.floor(Math.random()*100000+1)+100000;
							$.ajax({
							  type: "GET",
							  url: url,
							  dataType: "html",
							  data: "content="+name+"&sex="+escape("тест русских букв - test cyrilic words")+"&rnd="+rnd,
							  complete: function(res, status){
								    //console.log(res);
								    //console.log(status);
									$("#title_"+name).html(oldtitle);
									$("#icon_"+name).removeClass("loading");
									$("#icon_"+name).attr("src",oldIconSrc);
									if ( status == "success"){
										var text = res.responseText;
										text = text.replace(new RegExp(config.scriptRegexp, ''), '')
										$("#content_"+name).html(text);
										jQuery.winding.parseLinks(name);
									}
								}
							});
						    //$("#content_"+name).load()
						});
					}
					//console.log(this);
				}
			);
			//$("#content_"+name+" a").add("fuck");
		},
		resize: function(name,settings)
		{
			if (!winding.winlist[name])
			{
				name = jQuery.winding._find(name);
			}
			//if (!name){return}
			winding.winlist[name].settings = $.extend(winding.winlist[name].settings,settings);
			config.zIndex++;
			var ok = 0;
			if (settings.width){ok++;settings.width=parseInt(settings.width);}
			if (settings.height){ok++;settings.heigt=parseInt(settings.heigt);}
			if (settings.top){ok++;settings.top=parseInt(settings.top);}
			var baseOk = ok;
			if ((settings.width && parseInt($("#window_"+name).css("width"))!=settings.width)){ok--}
			if ((settings.height && parseInt($("#window_"+name).css("height"))!=settings.height)){ok--}
			if ((settings.top && parseInt($("#window_"+name).css("top"))!=settings.top)){ok--}
			if ((settings.left && parseInt($("#window_"+name).css("left"))!=settings.left)){ok--}
			//console.log(baseOk);
			//console.log(ok);
			if (baseOk==ok)
			{
				return;
			}
			$("#window_"+name).css("zIndex",config.zIndex++);
			$("#window_"+name).animate(settings,'fast');
			var settings1 = new Object;
			if (settings.height)
			{
				settings1.height=settings.height-40;
			}
			$("#mainblock_"+name).animate(settings1,'fast');
			if (settings.height)
			{
				settings1.height=settings.height;
			}
			$("#l"+name).animate(settings1,'fast');
			$("#r"+name).animate(settings1,'fast');
			
		},
		create: function(settings)
		{
			winding.winMax++;
			var id = winding.winMax;
			config.status="Status";
			config.statusExtend = "";
			if (!settings.icon){settings.icon='application.png';}
			if (settings.url && settings.title){
				settings.oldtitle = settings.title;
				if (settings.url.substr(0,1)=="/")
				{settings.title+=" loading ("+settings.url.substr(1,settings.url.length-1)+")";}
				else{settings.title+=" loading ("+settings.url+")";}
			}
			if (isNaN(settings.top)){settings.top=config.top;config.top+=200;}
			if (!settings.left){settings.left=config.left;config.left+=200;}
			if (!settings.classname){settings.classname='window'};
			
			var config_old = config;

			winding.winlist[id]=new Object;

			if (!settings.name){ settings.name = winding.defname+"_"+id;}
			winding.winlist[id].settings = new Object;
			winding.winlist[id].settings = $.extend(winding.winlist[id].settings,config,settings);
			winding.winlist[id].settings.id = id;
			winding.winlist[id].settings.name = settings.name;
//			alert(id+""+winding.winlist[id].settings.name);
			winding.winlist[id].settings.height2 = winding.winlist[id].settings.height-40;


			if (!settings.status){settings.status="";}

			if (settings.statusText && settings.statusText != "")
			{
				var statuses = new Array;
				statuses = settings.statusText.split("|");
				var statusText = "";
				for (var i=0;i<statuses.length;i++){
					statuses[i]="<div class=text>"+statuses[i]+"</div>";
				}
				winding.winlist[id].settings.statusExtend=statuses.join('<div class="devider">&nbsp;</div>');
			}
			$("#temp_block").append("<div id='block_"+id+"'></div>");
			$("#block_"+id).appendTemplated(winding.winlist[id].settings.template, winding.winlist[id].settings);
			$('#window_'+id).bgiframe();
			$('img[@src$=.png]').IEPNGHack();

			if (settings.url)
			{
				$("#content_"+id).load(settings.url,function() { $("#title_"+id).html(settings.oldtitle); jQuery.winding.parseLinks(id); });
			}
			$("#window_"+id).click(function(){config.zIndex++;$(this).css("zIndex",config.zIndex);});
			$("#window_"+id).jqDrag(
					{
						handle:'#header_block_'+id,
						events: {
							start:function(obj){
								config.zIndex++;
								//console.log(config.zIndex);
								$(obj).css("zIndex",config.zIndex);
							}
						}
					}
			);
			$("#window_"+id).Resizable(
				{
				minWidth: 200,minHeight: 200,
				handlers: {se: '#dr'+id,e: '#r'+id,ne: '#ur'+id,n: '#u'+id,nw: '#ul'+id,w: '#l'+id,sw: '#dl'+id,s: '#d'+id},
					onStart: function(obj){
							config.zIndex++;
							//console.log(config.zIndex);
							$(obj.resizeElement).css("zIndex",config.zIndex);
					},
					onResize : function(size, position) {
						var s = size.height;
						if (s){s=parseInt(s)-40;$("#mainblock_"+id).height(s);
							$("#l"+id).height(s+40);$("#r"+id).height(s+40);							
						}
					}
				}
			);
			return id;
		}
	};
}();



var trans = [];
for (var i = 0x410; i <= 0x44F; i++)
  trans[i] = i - 0x350; // А-Яа-я
trans[0x401] = 0xA8;    // Ё
trans[0x451] = 0xB8;    // ё

// Сохраняем стандартную функцию escape()
var escapeOrig = window.escape;

// Переопределяем функцию escape()
window.escape = function(str)
{
  var ret = [];
  // Составляем массив кодов символов, попутно переводим кириллицу
  for (var i = 0; i < str.length; i++)
  {
    var n = str.charCodeAt(i);
    if (typeof trans[n] != 'undefined')
      n = trans[n];
    if (n <= 0xFF)
      ret.push(n);
  }
  return escapeOrig(String.fromCharCode.apply(null, ret));
}
