/*
 * jqDnR - Minimalistic Drag'n'Resize for jQuery.
 *
 * Copyright (c) 2007 Brice Burgess <bhb@iceburg.net>, http://www.iceburg.net
 * Licensed under the MIT License:
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * $Version: 2007.02.09 +r1
 * $Version: 2007.03.26 +r1.0.0.0.1 add event handlers (flymio fix)
		$("drag_object").jqDrag(
					{
						handler:'drag_handler',
						events: {
							start:function(obj){
								$(obj).css("zIndex",2);
							}
							drag:function(obj){
								$(obj).css("zIndex",2);
							}
							stop:function(obj){
								$(obj).css("zIndex",2);
							}
						}
					}
			);

 */


(function($){
$.fn.jqDrag=function(hash){$.jqDnR.init(this,hash.handle,'d',hash.events); return this;};
$.fn.jqResize=function(hash){$.jqDnR.init(this,hash.handle,'r',hash.events); return this;};
$.jqDnR={
init:function(w,r,t,ev){ r=(r)?$(r,w):w;
	r.bind('mousedown',{w:w,t:t,ev:ev},function(e){ var h=e.data; var w=h.w; if (h.ev.start){h.ev.start(w)};
	hash=$.extend({oX:f(w,'left'),oY:f(w,'top'),oW:f(w,'width'),oH:f(w,'height'),pX:e.pageX,pY:e.pageY,o:w.css('opacity')},h,ev);
	$().mousemove($.jqDnR.drag).mouseup($.jqDnR.stop);
	return false;});
},
drag:function(e) {var h=hash; var w=h.w[0];
	if(h.t == 'd') h.w.css({left:h.oX + e.pageX - h.pX,top:h.oY + e.pageY - h.pY});
	else h.w.css({width:Math.max(e.pageX - h.pX + h.oW,0),height:Math.max(e.pageY - h.pY + h.oH,0)});
	if (h.ev.drag){h.ev.drag(w)}
	return false;},
stop:function(){
var j=$.jqDnR; $().unbind('mousemove',j.drag).unbind('mouseup',j.stop);h=hash;if (h.ev.stop){h.ev.stop(w)}},
h:false};
var hash=$.jqDnR.h;
var f=function(w,t){return parseInt(w.css(t)) || 0};
})(jQuery);