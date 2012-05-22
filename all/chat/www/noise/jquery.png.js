jQuery.fn.IEPNGHack = function() {
		if (!$.browser.msie) {
			return false;
		}
		this.each(function(){
			var hack = "<span";
			hack += $(this).attr('id') ? " id=\""+$(this).attr('id')+"\"" : "";
			hack += $(this).attr('class') ? " class=\""+$(this).attr('class')+"\"" : "";
			hack += " style=\"display:inline-block;";
			if ($(this).parent().attr('href')) {
				hack += " cursor:pointer;";
			}
			hack += " width:"+$(this).width()+"px;height:"+$(this).height()+"px;";
			hack += " filter:progid:DXImageTransform.Microsoft.AlphaImageLoader(src='"+$(this).attr('src')+"',sizingMethod='scale')";
			hack += " \"></span>";
			$(this).wrap(hack).hide();
		})
	};